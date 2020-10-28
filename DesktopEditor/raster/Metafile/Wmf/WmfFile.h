/*
 * (c) Copyright Ascensio System SIA 2010-2019
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at 20A-12 Ernesta Birznieka-Upisha
 * street, Riga, Latvia, EU, LV-1050.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */
#ifndef _METAFILE_WMF_WMFFILE_H
#define _METAFILE_WMF_WMFFILE_H

#include "../../../common/StringExt.h"
#include "../../../fontengine/FontManager.h"

#include "../Common/IOutputDevice.h"
#include "../Common/MetaFile.h"

#include "../Emf/EmfFile.h"

#include "WmfTypes.h"
#include "WmfPlayer.h"

#include <cmath>
#include <iostream>

#undef DrawText

#ifdef _DEBUG
#define LOG_TRACE std::wcout << "Entering: " << __FUNCTION__ << std::endl;
#else
#define LOG_TRACE
#endif

#ifdef _DEBUG
#define LOGGING(_value) std::wcout << _value << std::endl;
#else
#define LOGGING
#endif

namespace MetaFile
{
	class CWmfFile : public IMetaFileBase
	{
	public:
		CWmfFile() : m_oPlayer(this)
		{
		}
		~CWmfFile()
		{
			ClearFile();
		}

        TRectD       GetBounds()
		{
            TRect  oBoundsBox = GetBoundingBox();
            TRectD oBounds = oBoundsBox;
			if (IsPlaceable())
			{				
				double dLogicalToMM = (m_oPlaceable.Inch > 0 ? 25.4 / m_oPlaceable.Inch : 25.4 / 1440);
				oBounds *= dLogicalToMM;
			}
			else
			{
				// TODO:
			}
			return oBounds;
		}
		void         PlayMetaFile()
		{
			if (!m_oStream.IsValid())
				SetError();

			unsigned int unSize;
			unsigned short ushType;
			unsigned int ulNumber = 0;

			bool bEof = false;

			Read_META_HEADER();

			unsigned int unRecordIndex = 1;

			if (m_pOutput)
				m_pOutput->Begin();

			do
			{
				if (m_oStream.CanRead() < 6)
					return SetError();

				m_unRecordPos = m_oStream.Tell();

				m_oStream >> unSize;
				m_oStream >> ushType;

				m_unRecordSize = unSize * 2; // Размер указан в WORD

				switch (ushType)
				{
					//-----------------------------------------------------------
					// 2.3.1 Bitmap records
					//-----------------------------------------------------------
					case META_BITBLT: Read_META_BITBLT(); break;
					case META_DIBBITBLT: Read_META_DIBBITBLT(); break;
					case META_DIBSTRETCHBLT: Read_META_DIBSTRETCHBLT(); break;
					case META_SETDIBTODEV: Read_META_SETDIBTODEV(); break;
					case META_STRETCHBLT: Read_META_STRETCHBLT(); break;
					case META_STRETCHDIB: Read_META_STRETCHDIB(); break;
					//-----------------------------------------------------------
					// 2.3.2 Control records
					//-----------------------------------------------------------
					case META_EOF: bEof = true; break;
					//-----------------------------------------------------------
					// 2.3.3 Drawing records
					//-----------------------------------------------------------
					case META_ARC: Read_META_ARC(); break;
					case META_CHORD: Read_META_CHORD(); break;
					case META_ELLIPSE: Read_META_ELLIPSE(); break;
					case META_EXTFLOODFILL: Read_META_UNSUPPORTED(); break;
					case META_EXTTEXTOUT: Read_META_EXTTEXTOUT(); break;
					case META_FILLREGION: Read_META_FILLREGION(); break;
					case META_FLOODFILL: Read_META_UNSUPPORTED(); break;
					case META_FRAMEREGION: Read_META_FRAMEREGION(); break;
					case META_INVERTREGION: Read_META_INVERTREGION(); break;
					case META_LINETO: Read_META_LINETO(); break;
					case META_PAINTREGION: Read_META_PAINTREGION(); break;
					case META_PATBLT: Read_META_PATBLT(); break;
					case META_PIE: Read_META_PIE(); break;
					case META_POLYLINE: Read_META_POLYLINE(); break;
					case META_POLYGON: Read_META_POLYGON(); break;
					case META_POLYPOLYGON: Read_META_POLYPOLYGON(); break;
					case META_RECTANGLE: Read_META_RECTANGLE(); break;
					case META_ROUNDRECT: Read_META_ROUNDRECT(); break;
					case META_SETPIXEL: Read_META_SETPIXEL(); break;
					case META_TEXTOUT: Read_META_TEXTOUT(); break;
					//-----------------------------------------------------------
					// 2.3.4 Object records
					//-----------------------------------------------------------
					case META_CREATEBRUSHINDIRECT: Read_META_CREATEBRUSHINDIRECT(); break;
					case META_CREATEFONTINDIRECT: Read_META_CREATEFONTINDIRECT(); break;
					case META_CREATEPALETTE: Read_META_CREATEPALETTE(); break;
					case META_CREATEPATTERNBRUSH: Read_META_CREATEPATTERNBRUSH(); break;
					case META_CREATEPENINDIRECT: Read_META_CREATEPENINDIRECT(); break;
					case META_CREATEREGION: Read_META_CREATEREGION(); break;
					case META_DELETEOBJECT: Read_META_DELETEOBJECT(); break;
					case META_DIBCREATEPATTERNBRUSH: Read_META_DIBCREATEPATTERNBRUSH(); break;
					case META_SELECTCLIPREGION: Read_META_SELECTCLIPREGION(); break;
					case META_SELECTOBJECT: Read_META_SELECTOBJECT(); break;
					case META_SELECTPALETTE: Read_META_SELECTPALETTE(); break;
					//-----------------------------------------------------------
					// 2.3.5 State records
					//-----------------------------------------------------------
					case META_ANIMATEPALETTE: Read_META_UNSUPPORTED(); break;
					case META_EXCLUDECLIPRECT: Read_META_EXCLUDECLIPRECT(); break;
					case META_INTERSECTCLIPRECT: Read_META_INTERSECTCLIPRECT(); break;
					case META_MOVETO: Read_META_MOVETO(); break;
					case META_OFFSETCLIPRGN: Read_META_OFFSETCLIPRGN(); break;
					case META_OFFSETVIEWPORTORG: Read_META_OFFSETVIEWPORTORG(); break;
					case META_OFFSETWINDOWORG: Read_META_OFFSETWINDOWORG(); break;
					case META_REALIZEPALETTE: Read_META_UNSUPPORTED(); break;
					case META_RESIZEPALETTE: Read_META_UNSUPPORTED(); break;
					case META_RESTOREDC: Read_META_RESTOREDC(); break;
					case META_SAVEDC: Read_META_SAVEDC(); break;
					case META_SCALEVIEWPORTEXT: Read_META_SCALEVIEWPORTEXT(); break;
					case META_SCALEWINDOWEXT: Read_META_SCALEWINDOWEXT(); break;
					case META_SETBKCOLOR: Read_META_SETBKCOLOR(); break;
					case META_SETBKMODE: Read_META_SETBKMODE(); break;
					case META_SETLAYOUT: Read_META_SETLAYOUT(); break;
					case META_SETMAPMODE: Read_META_SETMAPMODE(); break;
					case META_SETMAPPERFLAGS: Read_META_UNSUPPORTED(); break;
					case META_SETPALENTRIES: Read_META_UNSUPPORTED(); break;
					case META_SETPOLYFILLMODE: Read_META_SETPOLYFILLMODE(); break;
					case META_SETRELABS: Read_META_UNSUPPORTED(); break;
					case META_SETROP2: Read_META_SETROP2(); break;
					case META_SETSTRETCHBLTMODE: Read_META_SETSTRETCHBLTMODE(); break;
					case META_SETTEXTALIGN: Read_META_SETTEXTALIGN(); break;
					case META_SETTEXTCHAREXTRA: Read_META_SETTEXTCHAREXTRA(); break;
					case META_SETTEXTCOLOR: Read_META_SETTEXTCOLOR(); break;
					case META_SETTEXTJUSTIFICATION: Read_META_SETTEXTJUSTIFICATION(); break;
					case META_SETVIEWPORTEXT: Read_META_SETVIEWPORTEXT(); break;
					case META_SETVIEWPORTORG: Read_META_SETVIEWPORTORG(); break;
					case META_SETWINDOWEXT: Read_META_SETWINDOWEXT(); break;
					case META_SETWINDOWORG: Read_META_SETWINDOWORG(); break;
					//-----------------------------------------------------------
					// 2.3.6 State records
					//-----------------------------------------------------------
					case META_ESCAPE: Read_META_ESCAPE(); break;
					//-----------------------------------------------------------
					// Неизвестные записи
					//-----------------------------------------------------------
					default:
					{
						//std::cout << ushType << " ";
						Read_META_UNKNOWN();
						break;
					}
				}

				if (bEof)
					break;

				unRecordIndex++;

				// Пропускаем лишние байты, которые могли быть в записи
				unsigned int unReadedSize = m_oStream.Tell() - m_unRecordPos;
				m_oStream.Skip(m_unRecordSize - unReadedSize);
			} while (!CheckError());

			if (!CheckError())
				m_oStream.SeekToStart();

			if (m_pOutput)
				m_pOutput->End();
		}
		void         ClearFile()
		{
			m_oPlayer.Clear();
			m_pDC = m_oPlayer.GetDC();
		}
		TRect*       GetDCBounds()
		{
			TWmfWindow* pViewport = m_pDC->GetViewport();

			m_oDCRect.nLeft   = pViewport->x;
			m_oDCRect.nTop    = pViewport->y;
			m_oDCRect.nRight  = pViewport->w + pViewport->x;
			m_oDCRect.nBottom = pViewport->h + pViewport->y;

			return &m_oDCRect;
		}
		double       GetPixelHeight()
		{
			return m_pDC->GetPixelHeight();
		}
		double       GetPixelWidth()
		{
			return m_pDC->GetPixelWidth();
		}
		int          GetTextColor()
		{
			TWmfColor& oColor = m_pDC->GetTextColor();
			return METAFILE_RGBA(oColor.r, oColor.g, oColor.b);
		}
		IFont*       GetFont()
		{
			CWmfFont* pFont = m_pDC->GetFont();
			if (!pFont)
				return NULL;

			return (IFont*)pFont;
		}
		IBrush*      GetBrush()
		{
			CWmfBrush* pBrush = m_pDC->GetBrush();
			if (!pBrush)
				return NULL;

			return (IBrush*)pBrush;
		}
		IPen*        GetPen()
		{
			CWmfPen* pPen = m_pDC->GetPen();
			if (!pPen)
				return NULL;

			return (IPen*)pPen;
		}
		unsigned int GetTextAlign()
		{
			return (unsigned int)m_pDC->GetTextAlign();
		}
		unsigned int GetTextBgMode()
		{
			return (unsigned int)m_pDC->GetTextBgMode();
		}
		int          GetTextBgColor()
		{
			TWmfColor& oColor = m_pDC->GetTextBgColor();
			return METAFILE_RGBA(oColor.r, oColor.g, oColor.b);
		}
		unsigned int GetFillMode()
		{
			return (unsigned int)m_pDC->GetPolyFillMode();
		}
		TPointD      GetCurPos()
		{
			TPointL oPoint = m_pDC->GetCurPos();
			double dX, dY;
			TranslatePoint(oPoint.x, oPoint.y, dX, dY);
			return TPointD(dX, dY);
		}
		TXForm*      GetInverseTransform()
		{
			return m_pDC->GetInverseTransform();
		}
		TXForm*      GetTransform(int iGraphicsMode = GM_ADVANCED)
		{
			TRect* pBounds = GetDCBounds();
			double dT = pBounds->nTop;
			double dL = pBounds->nLeft;

			TXForm oShiftXForm(1, 0, 0, 1, -dL, -dT);
			m_oTransform.Copy(m_pDC->GetFinalTransform(iGraphicsMode));
			m_oTransform.Multiply(oShiftXForm, MWT_RIGHTMULTIPLY);
			return &m_oTransform;
		}
		unsigned int GetMiterLimit()
		{
			return m_pDC->GetMiterLimit();
		}
		unsigned int GetRop2Mode()
		{
			return (unsigned int)m_pDC->GetRop2Mode();
		}
		IClip*       GetClip()
		{
			CWmfClip* pClip = m_pDC->GetClip();			
			if (!pClip)
				return NULL;

			return (IClip*)pClip;
		}
		int          GetCharSpace()
		{
			return m_pDC->GetCharSpacing();
		}
		bool         IsWindowFlippedY()
		{
			TWmfWindow* pWindow = m_pDC->GetWindow();
			return (pWindow->h < 0);
		}
		bool         IsWindowFlippedX()
		{
			TWmfWindow* pWindow = m_pDC->GetWindow();
			return (pWindow->w < 0);
		}


	private:

		void TranslatePoint(short shX, short shY, double& dX, double &dY)
		{
			dX = (double)shX;
			dY = (double)shY;
		}
        TRect GetBoundingBox()
		{
			TRect oBB;
			if (IsPlaceable())
			{
				oBB = m_oPlaceable.BoundingBox;

				// Иногда m_oPlaceable.BoundingBox задается нулевой ширины и высоты
				if (abs(oBB.nRight - oBB.nLeft) <= 1)
				{
					oBB.nRight = m_oBoundingBox.nRight;
					oBB.nLeft  = m_oBoundingBox.nLeft;
				}
				if (abs(oBB.nBottom - oBB.nTop) <= 1)
				{
					oBB.nTop    = m_oBoundingBox.nTop;
					oBB.nBottom = m_oBoundingBox.nBottom;
				}
			}
			else
				oBB = m_oBoundingBox;

			if (abs(oBB.nRight - oBB.nLeft) <= 1)
				oBB.nRight = oBB.nLeft + 1024;

			if (abs(oBB.nBottom - oBB.nTop) <= 1)
				oBB.nBottom = m_oBoundingBox.nTop + 1024;

			return oBB;
		}
		bool IsPlaceable()
		{
			return (0x9AC6CDD7 == m_oPlaceable.Key);
		}
		int GetRecordRemainingBytesCount()
		{
			unsigned int unReadedSize = m_oStream.Tell() - m_unRecordPos;
			return (m_unRecordSize - unReadedSize);
		}
		inline double GetSweepAngle(const double& dStartAngle, const double& dEndAngle)
		{
			return (dEndAngle - dStartAngle) - 360;
		}
		void MoveTo(short shX, short shY)
		{
			if (m_pOutput)
			{
				double dX, dY;
				TranslatePoint(shX, shY, dX, dY);
				m_pOutput->MoveTo(dX, dY);
			}
			else
			{
				RegisterPoint(shX, shY);
			}
			m_pDC->SetCurPos(shX, shY);
		}
		void LineTo(short shX, short shY)
		{
			if (m_pOutput)
			{
				double dX, dY;
				TranslatePoint(shX, shY, dX, dY);
				m_pOutput->LineTo(dX, dY);
			}
			else
			{
				RegisterPoint(shX, shY);
			}
			m_pDC->SetCurPos(shX, shY);
		}
		void ArcTo(short shL, short shT, short shR, short shB, double dStart, double dSweep)
		{
			// Тут не делаем пересчет текущей точки, т.к. при вызове данной функции не всегда он нужен (например эллипс).
			// Текущая точка обновляется на том уровне, на котором вызывалась данная функция.
			if (m_pOutput)
			{
				double dL, dT, dR, dB;
				TranslatePoint(shL, shT, dL, dT);
				TranslatePoint(shR, shB, dR, dB);
				m_pOutput->ArcTo(dL, dT, dR, dB, dStart, dSweep);
			}
			else
			{
				// TODO: Возможно нужно регистрировать более точно
				RegisterPoint(shL, shT);
				RegisterPoint(shR, shB);
			}
		}
		void ClosePath()
		{
			if (m_pOutput)
			{
				m_pOutput->ClosePath();
			}
		}
		void DrawPath(bool bStroke, bool bFill)
		{
			if (m_pOutput)
			{
				int lType = (bStroke ? 1 : 0) + (bFill ? 2 : 0);
				m_pOutput->DrawPath(lType);
				m_pOutput->EndPath();
			}
		}
		void DrawText(const unsigned char* pString, unsigned int unCharsCount, short _shX, short _shY, short* pDx)
		{
			int nX = _shX;
			int nY = _shY;

			if (m_pDC->GetTextAlign() & TA_UPDATECP)
			{
				nX = m_pDC->GetCurPos().x;
				nY = m_pDC->GetCurPos().y;
			}

			IFont* pFont = GetFont();
			NSStringExt::CConverter::ESingleByteEncoding eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_DEFAULT;;
			if (pFont)
			{
				// Соответствие Charset -> Codepage: http://support.microsoft.com/kb/165478
				// http://msdn.microsoft.com/en-us/library/cc194829.aspx
				//  Charset Name       Charset Value(hex)  Codepage number
				//  ------------------------------------------------------
				//
				//  DEFAULT_CHARSET           1 (x01)
				//  SYMBOL_CHARSET            2 (x02)
				//  OEM_CHARSET             255 (xFF)
				//  ANSI_CHARSET              0 (x00)            1252
				//  RUSSIAN_CHARSET         204 (xCC)            1251
				//  EASTEUROPE_CHARSET      238 (xEE)            1250
				//  GREEK_CHARSET           161 (xA1)            1253
				//  TURKISH_CHARSET         162 (xA2)            1254
				//  BALTIC_CHARSET          186 (xBA)            1257
				//  HEBREW_CHARSET          177 (xB1)            1255
				//  ARABIC _CHARSET         178 (xB2)            1256
				//  SHIFTJIS_CHARSET        128 (x80)             932
				//  HANGEUL_CHARSET         129 (x81)             949
				//  GB2313_CHARSET          134 (x86)             936
				//  CHINESEBIG5_CHARSET     136 (x88)             950
				//  THAI_CHARSET            222 (xDE)             874	
				//  JOHAB_CHARSET	        130 (x82)            1361
				//  VIETNAMESE_CHARSET      163 (xA3)            1258

				switch (pFont->GetCharSet())
				{
					default:
					case DEFAULT_CHARSET:       eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_DEFAULT; break;
					case SYMBOL_CHARSET:        eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_DEFAULT; break;
					case ANSI_CHARSET:          eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1252; break;
					case RUSSIAN_CHARSET:       eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1251; break;
					case EASTEUROPE_CHARSET:    eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1250; break;
					case GREEK_CHARSET:         eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1253; break;
					case TURKISH_CHARSET:       eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1254; break;
					case BALTIC_CHARSET:        eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1257; break;
					case HEBREW_CHARSET:        eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1255; break;
					case ARABIC_CHARSET:        eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1256; break;
					case SHIFTJIS_CHARSET:      eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP932; break;
					case HANGEUL_CHARSET:       eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP949; break;
					case 134/*GB2313_CHARSET*/: eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP936; break;
					case CHINESEBIG5_CHARSET:   eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP950; break;
					case THAI_CHARSET:          eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP874; break;
					case JOHAB_CHARSET:         eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1361; break;
					case VIETNAMESE_CHARSET:    eCharSet = NSStringExt::CConverter::ESingleByteEncoding::SINGLE_BYTE_ENCODING_CP1258; break;
				}
			}

			std::wstring wsText = NSStringExt::CConverter::GetUnicodeFromSingleByteString((const unsigned char*)pString, (long)unCharsCount, eCharSet);

			if (m_pOutput)
			{
				double dX, dY;
				TranslatePoint(nX, nY, dX, dY);

				double* pdDx = NULL;
				if (NULL != pDx)
				{
					pdDx = new double[unCharsCount];
					if (pdDx)
					{
						int nCurX = nX;
						double dCurX = dX;
						for (unsigned int unCharIndex = 0; unCharIndex < unCharsCount; unCharIndex++)
						{
							int nX1 = nCurX + pDx[unCharIndex];
							double dX1, dY1;
							TranslatePoint(nX1, nY, dX1, dY1);

							pdDx[unCharIndex] = dX1 - dCurX;

							nCurX = nX1;
							dCurX = dX1;
						}
					}
				}

				m_pOutput->DrawString(wsText, unCharsCount, dX, dY, pdDx);

				if (pdDx)
					delete[] pdDx;
			}
			else
			{
				// TODO: Здесь идет точное повторение кода из CMetaFileRenderer->DrawString
				//       неплохо бы перенести этот пересчет в базовый класс IMetaFileBase.

				CFontManager* pFontManager = GetFontManager();
				if (pFont && pFontManager)
				{
					int lLogicalFontHeight = pFont->GetHeight();
					if (lLogicalFontHeight < 0)
						lLogicalFontHeight = -lLogicalFontHeight;
					if (lLogicalFontHeight < 0.01)
						lLogicalFontHeight = 18;

					double dFontHeight = lLogicalFontHeight;

					std::wstring wsFaceName = pFont->GetFaceName();

					int lStyle = 0;
					if (pFont->GetWeight() > 550)
						lStyle |= 0x01;
					if (pFont->IsItalic())
						lStyle |= 0x02;

					float fL = 0, fT = 0, fW = 0, fH = 0;
					pFontManager->LoadFontByName(wsFaceName, dFontHeight, lStyle, 72, 72);
					pFontManager->SetCharSpacing(GetCharSpace());
					double dFHeight  = pFontManager->m_pFont ? (dFontHeight * pFontManager->m_pFont->GetHeight() / pFontManager->m_pFont->m_lUnits_Per_Em) : 0;
					double dFDescent = pFontManager->m_pFont ? (dFontHeight * pFontManager->m_pFont->GetDescender() / pFontManager->m_pFont->m_lUnits_Per_Em) : 0;
					double dFAscent  = dFHeight - std::abs(dFDescent);

					if (NULL != pDx && unCharsCount > 1)
					{
						// Тогда мы складываем все pDx кроме последнего символа, последний считаем отдельно
						double dTempTextW = 0;
						for (unsigned int unCharIndex = 0; unCharIndex < unCharsCount - 1; unCharIndex++)
						{
							dTempTextW += pDx[unCharIndex];
						}

						std::wstring wsTempText;
                        wsTempText += wsText.at(wsText.length() - 1);
                        //wsTempText += wsText.at(unCharsCount - 1);

						pFontManager->LoadString1(wsTempText, 0, 0);
						TBBox oBox = pFontManager->MeasureString2();
						dTempTextW += (oBox.fMaxX - oBox.fMinX);

						fL = 0;
						fW = (float)dTempTextW;
					}
					else
					{
						pFontManager->LoadString1(wsText, 0, 0);
						TBBox oBox = pFontManager->MeasureString2();
						fL = (float)(oBox.fMinX);
						fW = (float)(oBox.fMaxX - oBox.fMinX);
					}

					pFontManager->LoadString1(wsText, 0, 0);
					TBBox oBox = pFontManager->MeasureString2();
					fL = (float)(oBox.fMinX);
					fW = (float)(oBox.fMaxX - oBox.fMinX);

					fT = (float)-dFAscent;
					fH = (float)dFHeight;

					
					double dTheta = -((((double)pFont->GetEscapement()) / 10) * 3.14159265358979323846 / 180);
					double dCosTheta = (float)cos(dTheta);
					double dSinTheta = (float)sin(dTheta);

					double dX = (double)nX;
					double dY = (double)nY;

					// Найдем начальную точку текста
					unsigned int ulTextAlign = GetTextAlign();
					if (ulTextAlign & TA_BASELINE)
					{
						// Ничего не делаем
					}
					else if (ulTextAlign & TA_BOTTOM)
					{
						float fTemp = -(-fT + fH);

						dX += -fTemp * dSinTheta;
						dY +=  fTemp * dCosTheta;
					}
					else // if (ulTextAlign & TA_TOP)
					{
						float fTemp = -fT;

						dX += -fTemp * dSinTheta;
						dY +=  fTemp * dCosTheta;
					}

					if (ulTextAlign & TA_CENTER)
					{
						dX += -fW / 2 * dCosTheta;
						dY += -fW / 2 * dSinTheta;
					}
					else if (ulTextAlign & TA_RIGHT)
					{
						dX += -fW * dCosTheta;
						dY += -fW * dSinTheta;
					}
					else //if (ulTextAlign & TA_LEFT)
					{
						// Ничего не делаем
					}

					double dX0 = dX + fL, dY0 = dY + fT;
					double dX1 = dX + fL + fW, dY1 = dY + fT;
					double dX2 = dX + fL + fW, dY2 = dY + fT + fH;
					double dX3 = dX + fL, dY3 = dY + fT + fH;
					if (0 != pFont->GetEscapement())
					{
						TXForm oForm(dCosTheta, dSinTheta, -dSinTheta, dCosTheta, dX - dX * dCosTheta + dY * dSinTheta, dY - dX * dSinTheta - dY * dCosTheta);

						oForm.Apply(dX0, dY0);
						oForm.Apply(dX1, dY1);
						oForm.Apply(dX2, dY2);
						oForm.Apply(dX3, dY3);
					}

					RegisterPoint((short)dX0, (short)dY0);
					RegisterPoint((short)dX1, (short)dY1);
					RegisterPoint((short)dX2, (short)dY2);
					RegisterPoint((short)dX3, (short)dY3);
				}
				else
				{
					RegisterPoint(nX, nY);
				}
			}

			if (NULL != pDx)
			{
				int nTextW = 0;
				for (unsigned int unCharIndex = 0; unCharIndex < unCharsCount; unCharIndex++)
				{
					nTextW += pDx[unCharIndex];
				}
				m_pDC->SetCurPos(nX + nTextW, nY);
			}
			else
				m_pDC->SetCurPos(nX, nY);
		}
		void RegisterPoint(short shX, short shY)
		{
			if (m_bFirstPoint)
			{
				m_oBoundingBox.nLeft   = shX;
				m_oBoundingBox.nRight  = shX;
				m_oBoundingBox.nTop    = shY;
				m_oBoundingBox.nBottom = shY;
				m_bFirstPoint = false;
			}
			else
			{
				if (shX < m_oBoundingBox.nLeft)
					m_oBoundingBox.nLeft = shX;
				else if (shX > m_oBoundingBox.nRight)
					m_oBoundingBox.nRight = shX;

				if (shY < m_oBoundingBox.nTop)
					m_oBoundingBox.nTop = shY;
				else if (shY > m_oBoundingBox.nBottom)
					m_oBoundingBox.nBottom = shY;
			}
		}
		bool ReadImage(unsigned short ushColorUsage, BYTE** ppBgraBuffer, unsigned int* pulWidth, unsigned int* pulHeight)
		{
			unsigned int unRemainBytes = GetRecordRemainingBytesCount();
			if (unRemainBytes <= 0)
				return false;

			BYTE* pBuffer = m_oStream.GetCurPtr();
			MetaFile::ReadImage(pBuffer, unRemainBytes, ushColorUsage, ppBgraBuffer, pulWidth, pulHeight);
			return true;
		}
		void DrawImage(int nX, int nY, int nW, int nH, unsigned int unColorUsage, unsigned int unRasterOperation)
		{
			if (m_pOutput)
			{
				BYTE* pBgra = NULL;
				unsigned int unWidth, unHeight;
				if (ReadImage(unColorUsage, &pBgra, &unWidth, &unHeight))
				{
					ProcessRasterOperation(unRasterOperation, &pBgra, unWidth, unHeight);

					double dX, dY, dX1, dY1;
					TranslatePoint(nX, nY, dX, dY);
					TranslatePoint(nX + nW, nY + nH, dX1, dY1);

					m_pOutput->DrawBitmap(dX, dY, fabs(dX1 - dX), fabs(dY1 - dY), pBgra, unWidth, unHeight);
				}

				if (pBgra)
					delete[] pBgra;

				int nRemainingBytes = GetRecordRemainingBytesCount();
				if (nRemainingBytes < 0)
					return SetError();
			}
			else
			{
				RegisterPoint(nX, nY);
				RegisterPoint(nX + nW, nY + nH);
			}
		}
		void UpdateOutputDC()
		{
			if (m_pOutput)
				m_pOutput->UpdateDC();
		}

		void Read_META_UNKNOWN()
		{
                    LOG_TRACE
                }
		void Read_META_UNSUPPORTED()
		{
                    LOG_TRACE
			// META_EXTFLOODFILL
			// META_FLOODFILL

			// META_ANIMATEPALETTE
			// META_REALIZEPALETTE
			// META_RESIZEPALETTE
			// META_SETMAPPERFLAGS
			// META_SETPALENTRIES
		}
		void Read_META_HEADER()
		{
                    LOG_TRACE
                        m_oStream >> m_oPlaceable.Key;
			if (0x9AC6CDD7 == m_oPlaceable.Key)
			{
				m_oStream >> m_oPlaceable.HWmf;
				m_oStream >> m_oPlaceable.BoundingBox;
				m_oStream >> m_oPlaceable.Inch;
				m_oStream >> m_oPlaceable.Reserved;
				m_oStream >> m_oPlaceable.Checksum;

				m_pDC->SetViewportOrg(m_oPlaceable.BoundingBox.Left, m_oPlaceable.BoundingBox.Top);
				m_pDC->SetViewportExt(m_oPlaceable.BoundingBox.Right - m_oPlaceable.BoundingBox.Left, m_oPlaceable.BoundingBox.Bottom - m_oPlaceable.BoundingBox.Top);
			}
			else
			{
				m_oStream.SeekBack(m_oStream.Tell());
				m_oPlaceable.Key                = 0;
				m_oPlaceable.HWmf               = 0;
				m_oPlaceable.BoundingBox.Left   = 0;
				m_oPlaceable.BoundingBox.Top    = 0;
				m_oPlaceable.BoundingBox.Right  = 0;
				m_oPlaceable.BoundingBox.Bottom = 0;
				m_oPlaceable.Inch               = 0;
				m_oPlaceable.Reserved           = 0;
				m_oPlaceable.Checksum           = 0;
			}

			m_oStream >> m_oHeader.Type;
			m_oStream >> m_oHeader.HeaderSize;
			m_oStream >> m_oHeader.Version;
			m_oStream >> m_oHeader.Size;
			m_oStream >> m_oHeader.NumberOfObjects;
			m_oStream >> m_oHeader.MaxRecord;
			m_oStream >> m_oHeader.NumberOfMembers;

			if (0x0001 != m_oHeader.Type && 0x0002 != m_oHeader.Type)
				return SetError();

			if (0x0009 != m_oHeader.HeaderSize)
				return SetError();

			if (0x0100 != m_oHeader.Version && 0x0300 != m_oHeader.Version)
				return SetError();

			// Если у нас не задан Output, значит мы считаем, что идет сканирование метафайла.
			// Во время сканирования мы регистрируем все точки и вычисляем BoundingBox
			if (m_pOutput)
			{
				m_oRect = GetBoundingBox();
				m_pDC->SetWindowOff(m_oRect.nLeft, m_oRect.nTop);
				m_pDC->SetWindowExt(m_oRect.nRight - m_oRect.nLeft, m_oRect.nBottom - m_oRect.nTop);
			}
			else
			{
				m_bFirstPoint = true;
			}
		}
		void Read_META_BITBLT()
		{
                    LOG_TRACE
			TWmfBitBlt oBitmap;
			m_oStream >> oBitmap;

			unsigned int unRecordSizeDword = m_unRecordSize >> 1;
			unsigned int unValue = (META_BITBLT >> 8) + 3;
			if (unRecordSizeDword == unValue)
			{
				m_oStream.Skip(2); // Reserved
			}
			else
			{
				if (m_pOutput)
				{
					TWmfBitmap16 oBitmap16;
					m_oStream >> oBitmap16;

					// TODO: Сделать чтение Bitmap16
				}
				else
				{
					RegisterPoint(oBitmap.XDest, oBitmap.YDest);
					RegisterPoint(oBitmap.XDest + oBitmap.Width, oBitmap.YDest + oBitmap.Height);
				}


				int nRemainingBytes = GetRecordRemainingBytesCount();
				if (nRemainingBytes < 0)
					return SetError();
			}
		}
		void Read_META_DIBBITBLT()
		{
                    LOG_TRACE
			TWmfBitBlt oBitmap;
			m_oStream >> oBitmap;

			unsigned int unRecordSizeDword = m_unRecordSize >> 1;
			unsigned int unValue = (META_DIBBITBLT >> 8) + 3;
			if (unRecordSizeDword == unValue)
			{
				m_oStream.Skip(2); // Reserved
			}
			else
			{
				DrawImage(oBitmap.XDest, oBitmap.YDest, oBitmap.Width, oBitmap.Height, 0, oBitmap.RasterOperation);
			}
		}
		void Read_META_DIBSTRETCHBLT()
		{
                    LOG_TRACE
			TWmfStretchBlt oBitmap;
			m_oStream >> oBitmap;

			unsigned int unRecordSizeDWORD = m_unRecordSize >> 1;
			unsigned int unValue = (META_DIBSTRETCHBLT >> 8) + 3;

			if (unRecordSizeDWORD == unValue)
			{
				m_oStream.Skip(2); // Reserved
			}
			else
			{
				DrawImage(oBitmap.XDest, oBitmap.YDest, oBitmap.DestWidth, oBitmap.DestHeight, 0, oBitmap.RasterOperation);
			}
		}
		void Read_META_SETDIBTODEV()
		{
                    LOG_TRACE
			TWmfSetDibToDev oBitmap;
			m_oStream >> oBitmap;

			// TODO: Тут надо делать обрезку в зависимости от ScanCount и StartScan. Как встретится файл сделать.
			DrawImage(oBitmap.xDest, oBitmap.yDest, oBitmap.Width, oBitmap.Height, oBitmap.ColorUsage, 0);
		}
		void Read_META_STRETCHBLT()
		{
                    LOG_TRACE
			TWmfStretchBlt oBitmap;
			m_oStream >> oBitmap;

			unsigned int unRecordSizeDWORD = m_unRecordSize >> 1;
			unsigned int unValue = (META_STRETCHBLT >> 8) + 3;

			if (unRecordSizeDWORD == ((META_STRETCHBLT >> 8) + 3))
			{
				m_oStream.Skip(2); // Reserved
			}
			else
			{
				if (m_pOutput)
				{
					TWmfBitmap16 oBitmap16;
					m_oStream >> oBitmap16;

					// TODO: Сделать чтение Bitmap16
				}
				else
				{
					RegisterPoint(oBitmap.XDest, oBitmap.YDest);
					RegisterPoint(oBitmap.XDest + oBitmap.DestWidth, oBitmap.YDest + oBitmap.DestHeight);
				}

				int nRemainingBytes = GetRecordRemainingBytesCount();
				if (nRemainingBytes < 0)
					return SetError();
			}
		}
		void Read_META_STRETCHDIB()
		{
                    LOG_TRACE
			TWmfStretchDib oBitmap;
			m_oStream >> oBitmap;

			DrawImage(oBitmap.xDst, oBitmap.yDst, oBitmap.DestWidth, oBitmap.DestHeight, oBitmap.ColorUsage, oBitmap.RasterOperation);
		}
		void Read_META_ARC()
		{
                    LOG_TRACE
			short shYEndArc, shXEndArc, shYStartArc, shXStartArc, shBottom, shRight, shTop, shLeft;
			m_oStream >> shYEndArc >> shXEndArc >> shYStartArc >> shXStartArc >> shBottom >> shRight >> shTop >> shLeft;
			double dStartAngle = GetEllipseAngle((int)shLeft, (int)shTop, (int)shRight, (int)shBottom, (int)shXStartArc, (int)shYStartArc);
			double dEndAngle   = GetEllipseAngle((int)shLeft, (int)shTop, (int)shRight, (int)shBottom, (int)shXEndArc, (int)shYEndArc);
			double dSweepAngle = GetSweepAngle(dStartAngle, dEndAngle);

			m_pDC->SetCurPos(shXStartArc, shYStartArc);
			ArcTo(shLeft, shTop, shRight, shBottom, dStartAngle, dSweepAngle);
			DrawPath(true, false);
			m_pDC->SetCurPos(shXEndArc, shYEndArc);
		}
		void Read_META_CHORD()
		{
                    LOG_TRACE
			short shYEndArc, shXEndArc, shYStartArc, shXStartArc, shBottom, shRight, shTop, shLeft;
			m_oStream >> shYEndArc >> shXEndArc >> shYStartArc >> shXStartArc >> shBottom >> shRight >> shTop >> shLeft;
			double dStartAngle = GetEllipseAngle((int)shLeft, (int)shTop, (int)shRight, (int)shBottom, (int)shXStartArc, (int)shYStartArc);
			double dEndAngle   = GetEllipseAngle((int)shLeft, (int)shTop, (int)shRight, (int)shBottom, (int)shXEndArc, (int)shYEndArc);
			double dSweepAngle = GetSweepAngle(dStartAngle, dEndAngle);

			MoveTo(shXStartArc, shYStartArc);
			ArcTo(shLeft, shTop, shRight, shBottom, dStartAngle, dSweepAngle);
			LineTo(shXStartArc, shYStartArc);
			DrawPath(true, true);
			m_pDC->SetCurPos(shXEndArc, shYEndArc);
		}
		void Read_META_ELLIPSE()
		{
                    LOG_TRACE
			short shBottom, shRight, shTop, shLeft;
			m_oStream >> shBottom >> shRight >> shTop >> shLeft;
			ArcTo(shLeft, shTop, shRight, shBottom, 0, 360);
			DrawPath(true, true);
			m_pDC->SetCurPos((shLeft + shRight) / 2, (shTop + shBottom) / 2);
		}
		void Read_META_EXTTEXTOUT()
		{
                    LOG_TRACE
			short shY, shX, shStringLength;
			unsigned short ushFwOptions;
			TWmfRect oRectangle;
			m_oStream >> shY >> shX >> shStringLength >> ushFwOptions;

			if (shStringLength <= 0)
				return;

			if (ushFwOptions & ETO_CLIPPED || ushFwOptions & ETO_OPAQUE)
				m_oStream >> oRectangle;

			unsigned char* pString = new unsigned char[shStringLength + 1];
			if (!pString)
				return SetError();

			pString[shStringLength] = 0x00;
			m_oStream.ReadBytes(pString, shStringLength);

			short* pDx = NULL;
			if (shStringLength > 1 && ((GetRecordRemainingBytesCount() >= shStringLength * 2 && !(ushFwOptions & ETO_PDY)) || (GetRecordRemainingBytesCount() >= shStringLength * 4 && ushFwOptions & ETO_PDY)))
			{
				if (shStringLength & 1) // Если длина нечетная, тогда пропускаем 1 байт, т.к. тут прилегание по 2 байта
					m_oStream.Skip(1);

				pDx = new short[shStringLength];
				if (pDx)
				{
					if (ushFwOptions & ETO_PDY)
					{
						for (short shIndex = 0; shIndex < shStringLength; shIndex++)
						{
							m_oStream >> pDx[shIndex];
							m_oStream.Skip(2);
						}
					}
					else
					{
						for (short shIndex = 0; shIndex < shStringLength; shIndex++)
						{
							m_oStream >> pDx[shIndex];
						}
					}
				}
			}

			DrawText(pString, shStringLength, shX, shY, pDx);

			if (pString)
				delete[] pString;

			if (pDx)
				delete[] pDx;
		}
		void Read_META_FILLREGION()
		{
                    LOG_TRACE
			unsigned short ushRegionIndex, ushBrushIndex;
			m_oStream >> ushRegionIndex >> ushBrushIndex;
			// TODO: Реализовать регионы
		}
		void Read_META_FRAMEREGION()
		{
                    LOG_TRACE
			unsigned short ushRegionIndex, ushBrushIndex;
			short shHeight, shWidth;
			m_oStream >> ushRegionIndex >> ushBrushIndex >> shHeight >> shWidth;
			// TODO: Реализовать регионы
		}
		void Read_META_INVERTREGION()
		{
                    LOG_TRACE
			unsigned short ushRegionIndex;
			m_oStream >> ushRegionIndex;
			// TODO: Реализовать регионы
		}
		void Read_META_LINETO()
		{
                    LOG_TRACE
			short shY, shX;
			m_oStream >> shY >> shX;
			LineTo(shX, shY);
			DrawPath(true, false);
		}
		void Read_META_PAINTREGION()
		{
                    LOG_TRACE
			unsigned short ushRegionIndex;
			m_oStream >> ushRegionIndex;
			// TODO: Реализовать регионы
		}
		void Read_META_PATBLT()
		{
                    LOG_TRACE
			unsigned int unRasterOperation;
			short shX, shY, shW, shH;
			m_oStream >> unRasterOperation >> shH >> shW >> shY >> shX;

			// TODO: Нужно использовать растровую операцию unRasterOperation

			MoveTo(shX, shY);
			LineTo(shX + shW, shY);
			LineTo(shX + shW, shY + shH);
			LineTo(shX, shY + shH);
			ClosePath();
			DrawPath(false, true);
		}
		void Read_META_PIE()
		{
                    LOG_TRACE
			short shXRadial1, shYRadial1, shXRadial2, shYRadial2;
			short shL, shT, shR, shB;
			m_oStream >> shYRadial2 >> shXRadial2 >> shYRadial1 >> shXRadial1;
			m_oStream >> shB >> shR >> shT >> shL;

			double dStartAngle = GetEllipseAngle(shL, shT, shR, shB, shXRadial1, shYRadial1);
			double dEndAngle   = GetEllipseAngle(shL, shT, shR, shB, shXRadial2, shYRadial2);
			double dSweepAngle = GetSweepAngle(dStartAngle, dEndAngle);

			short shCenterX = (shL + shR) / 2;
			short shCenterY = (shT + shB) / 2;
			MoveTo(shCenterX, shCenterY);
			LineTo(shXRadial1, shYRadial1);
			ArcTo(shL, shT, shR, shB, dStartAngle, dSweepAngle);
			LineTo(shCenterX, shCenterY);
			DrawPath(true, true);
		}
		void Read_META_POLYLINE()
		{
                    LOG_TRACE
			short shNumberOfPoints;
			m_oStream >> shNumberOfPoints;
			if (shNumberOfPoints < 1)
				return;

			TWmfPointS oPoint;
			m_oStream >> oPoint;
			MoveTo(oPoint.x, oPoint.y);

			for (short shIndex = 1; shIndex < shNumberOfPoints; shIndex++)
			{
				m_oStream >> oPoint;
				LineTo(oPoint.x, oPoint.y);
			}
			DrawPath(true, false);
		}
		void Read_META_POLYGON()
		{
                    LOG_TRACE
			short shNumberOfPoints;
			m_oStream >> shNumberOfPoints;
			if (shNumberOfPoints < 1)
				return;

			TWmfPointS oPoint;
			m_oStream >> oPoint;
			MoveTo(oPoint.x, oPoint.y);

			for (short shIndex = 1; shIndex < shNumberOfPoints; shIndex++)
			{
				m_oStream >> oPoint;
				LineTo(oPoint.x, oPoint.y);
			}
			ClosePath();
			DrawPath(true, true);
		}
		void Read_META_POLYPOLYGON()
		{
                    LOG_TRACE
			unsigned short ushNumberOfPolygons;
			m_oStream >> ushNumberOfPolygons;
			if (ushNumberOfPolygons <= 0)
				return;

			unsigned short* pushPointsPerPolygon = new unsigned short[ushNumberOfPolygons];
			if (!pushPointsPerPolygon)
				return SetError();

			for (unsigned short ushIndex = 0; ushIndex < ushNumberOfPolygons; ushIndex++)
			{
				m_oStream >> pushPointsPerPolygon[ushIndex];
			}

			for (unsigned short ushPolygonIndex = 0; ushPolygonIndex < ushNumberOfPolygons; ushPolygonIndex++)
			{
				unsigned short ushPointsCount = pushPointsPerPolygon[ushPolygonIndex];

				if (ushPointsCount <= 0)
					continue;

				TWmfPointS oPoint;
				m_oStream >> oPoint;
				MoveTo(oPoint.x, oPoint.y);
				for (unsigned short ushPointIndex = 1; ushPointIndex < ushPointsCount; ushPointIndex++)
				{
					m_oStream >> oPoint;
					LineTo(oPoint.x, oPoint.y);
				}
				ClosePath();
			}
			DrawPath(true, true);

			delete[] pushPointsPerPolygon;
		}
		void Read_META_RECTANGLE()
		{
                    LOG_TRACE
			short shL, shT, shR, shB;
			m_oStream >> shB >> shR >> shT >> shL;

			MoveTo(shL, shT);
			LineTo(shR, shT);
			LineTo(shR, shB);
			LineTo(shL, shB);
			ClosePath();
			DrawPath(true, true);

			m_pDC->SetCurPos((shL + shR) / 2, (shT + shB) / 2);
		}
		void Read_META_ROUNDRECT()
		{
                    LOG_TRACE
			short shL, shT, shR, shB, shW, shH;
			m_oStream >> shH >> shW >> shB >> shR >> shT >> shL;

			MoveTo(shL + shW, shT);
			LineTo(shR - shW, shT);
			ArcTo(shR - shW, shT, shR, shT + shH, 270, 90);
			LineTo(shR, shB - shH);
			ArcTo(shR - shW, shB - shH, shR, shB, 0, 90);
			LineTo(shL + shW, shB);
			ArcTo(shL, shB - shH, shL + shW, shB, 90, 90);
			LineTo(shL, shT + shH);
			ArcTo(shL, shT, shL + shW, shT + shH, 180, 90);
			ClosePath();
			DrawPath(true, true);
		}
		void Read_META_SETPIXEL()
		{
                    LOG_TRACE
			TWmfColor oColor;
			short shX, shY;
			m_oStream >> oColor >> shY >> shX;

			BYTE pBgraBuffer[4];
			pBgraBuffer[0] = oColor.b;
			pBgraBuffer[1] = oColor.g;
			pBgraBuffer[2] = oColor.r;
			pBgraBuffer[3] = 0xff;

			if (m_pOutput)
				m_pOutput->DrawBitmap(shX, shY, 1, 1, pBgraBuffer, 1, 1);
		}
		void Read_META_TEXTOUT()
		{
                    LOG_TRACE
			short shStringLength;
			m_oStream >> shStringLength;

			if (shStringLength <= 0)
				return;

			unsigned char* pString = new unsigned char[shStringLength + 1];
			if (!pString)
				return SetError();

			pString[shStringLength] = 0x00;
			m_oStream.ReadBytes(pString, shStringLength);

			if (shStringLength & 1)
				m_oStream.Skip(1);

			short shX, shY;
			m_oStream >> shY >> shX;
			DrawText(pString, shStringLength, shX, shY, NULL);
			delete[] pString;
		}
		void Read_META_CREATEBRUSHINDIRECT()
		{
                    LOG_TRACE
			TWmfLogBrush oLogBrush;
			m_oStream >> oLogBrush;
			CWmfBrush* pBrush = new CWmfBrush(oLogBrush);
			if (!pBrush)
				return SetError();

			m_oPlayer.RegisterObject((CWmfObjectBase*)pBrush);
		}
		void Read_META_CREATEFONTINDIRECT()
		{
                    LOG_TRACE
			CWmfFont* pFont = new CWmfFont();
			if (!pFont)
				return SetError();

			m_oStream >> pFont;
			m_oPlayer.RegisterObject((CWmfObjectBase*)pFont);
		}
		void Read_META_CREATEPALETTE()
		{
                    LOG_TRACE
			CWmfPalette* pPalette = new CWmfPalette();
			if (!pPalette)
				return SetError();

			m_oStream >> pPalette;
			m_oPlayer.RegisterObject((CWmfObjectBase*)pPalette);
		}
		void Read_META_CREATEPATTERNBRUSH()
		{
                    LOG_TRACE
			CWmfBrush* pBrush = new CWmfBrush();
			if (!pBrush)
				return SetError();

			m_oPlayer.RegisterObject((CWmfObjectBase*)pBrush);

			if (m_pOutput)
			{
				TWmfBitmap16 oBitmap16;
				m_oStream >> oBitmap16;

				// TODO: Сделать чтение Bitmap16
			}
		}
		void Read_META_CREATEPENINDIRECT()
		{
                    LOG_TRACE
			CWmfPen* pPen = new CWmfPen();
			if (!pPen)
				return SetError();

			m_oStream >> pPen;
			m_oPlayer.RegisterObject((CWmfObjectBase*)pPen);
		}
		void Read_META_CREATEREGION()
		{
                    LOG_TRACE
			CWmfRegion* pRegion = new CWmfRegion();
			if (!pRegion)
				return SetError();
			m_oStream >> pRegion;
			m_oPlayer.RegisterObject((CWmfObjectBase*)pRegion);
		}
		void Read_META_DELETEOBJECT()
		{
                    LOG_TRACE
			unsigned short ushIndex;
			m_oStream >> ushIndex;
			m_oPlayer.DeleteObject(ushIndex);

			UpdateOutputDC();
		}
		void Read_META_DIBCREATEPATTERNBRUSH()
		{
                    LOG_TRACE
			unsigned short ushStyle, ushColorUsage;
			m_oStream >> ushStyle >> ushColorUsage;

			CWmfBrush* pBrush = new CWmfBrush();
			if (!pBrush)
				return SetError();
			
			if (m_pOutput)
			{
				BYTE* pBgra = NULL;
				unsigned int unWidth, unHeight;
				if (ReadImage(ushColorUsage, &pBgra, &unWidth, &unHeight))
				{
					pBrush->SetDibPattern(pBgra, unWidth, unHeight);
				}
			}

			m_oPlayer.RegisterObject((CWmfObjectBase*)pBrush);
		}
		void Read_META_SELECTCLIPREGION()
		{
                    LOG_TRACE
			unsigned short ushIndex;
			m_oStream >> ushIndex;

			// Тут просто сбрасываем текущий клип. Ничего не добавляем в клип, т.е. реализовать регионы с
			// текущим интерфейсом рендерера невозможно.
			m_pDC->GetClip()->Reset();

			UpdateOutputDC();
		}
		void Read_META_SELECTOBJECT()
		{
                    LOG_TRACE
			unsigned short ushIndex;
			m_oStream >> ushIndex;
			m_oPlayer.SelectObject(ushIndex);

			UpdateOutputDC();
		}
		void Read_META_SELECTPALETTE()
		{
                    LOG_TRACE
			unsigned short ushIndex;
			m_oStream >> ushIndex;
			m_oPlayer.SelectPalette(ushIndex);
			UpdateOutputDC();
		}
		void Read_META_EXCLUDECLIPRECT()
		{
                    LOG_TRACE
			short shLeft, shTop, shRight, shBottom;
			m_oStream >> shBottom >> shRight >> shTop >> shLeft;

			double dL, dT, dR, dB;

			// Поскольку мы реализовываем данный тип клипа с помощью разницы внешнего ректа и заданного, и
			// пересечением с полученной областью, то нам надо вычесть границу заданного ректа.
			if (shLeft < shRight)
			{
				shLeft--;
				shRight++;
			}
			else
			{
				shLeft++;
				shRight--;
			}

			if (shTop < shBottom)
			{
				shTop--;
				shBottom++;
			}
			else
			{
				shTop++;
				shBottom--;
			}

			TranslatePoint(shLeft, shTop, dL, dT);
			TranslatePoint(shRight, shBottom, dR, dB);

			TWmfWindow* pWindow = m_pDC->GetWindow();
			double dWindowL, dWindowT, dWindowR, dWindowB;
			TranslatePoint(pWindow->x, pWindow->y, dWindowL, dWindowT);
			TranslatePoint(pWindow->x + pWindow->w, pWindow->y + pWindow->h, dWindowR, dWindowB);

			m_pDC->GetClip()->Exclude(dL, dT, dR, dB, dWindowL, dWindowT, dWindowR, dWindowB);
			UpdateOutputDC();
		}
		void Read_META_INTERSECTCLIPRECT()
		{
                    LOG_TRACE
			short shLeft, shTop, shRight, shBottom;
			m_oStream >> shBottom >> shRight >> shTop >> shLeft;

			double dL, dT, dR, dB;
			TranslatePoint(shLeft, shTop, dL, dT);
			TranslatePoint(shRight, shBottom, dR, dB);

			m_pDC->GetClip()->Intersect(dL, dT, dR, dB);
			UpdateOutputDC();
		}
		void Read_META_MOVETO()
		{
                    LOG_TRACE
			short shX, shY;
			m_oStream >> shY >> shX;
			MoveTo(shX, shY);
		}
		void Read_META_OFFSETCLIPRGN()
		{
                    LOG_TRACE
			short shOffsetX, shOffsetY;
			m_oStream >> shOffsetY >> shOffsetX;
			// TODO: Реализовать
			UpdateOutputDC();
		}
		void Read_META_OFFSETVIEWPORTORG()
		{
                    LOG_TRACE
			short shXOffset, shYOffset;
			m_oStream >> shYOffset >> shXOffset;
			m_pDC->SetViewportOff(shXOffset, shYOffset);
			UpdateOutputDC();
		}
		void Read_META_OFFSETWINDOWORG()
		{
                    LOG_TRACE
			short shXOffset, shYOffset;
			m_oStream >> shYOffset >> shXOffset;
			m_pDC->SetWindowOff(shXOffset, shYOffset);
			UpdateOutputDC();
		}
		void Read_META_RESTOREDC()
		{
                    LOG_TRACE
			m_pDC = m_oPlayer.RestoreDC();
			UpdateOutputDC();
		}
		void Read_META_SAVEDC()
		{
                    LOG_TRACE
			m_pDC = m_oPlayer.SaveDC();
			UpdateOutputDC();
		}
		void Read_META_SCALEVIEWPORTEXT()
		{
                    LOG_TRACE
			short yDenom, yNum, xDenom, xNum;
			m_oStream >> yDenom >> yNum >> xDenom >> xNum;
			m_pDC->SetViewportScale((double)xNum / (double)xDenom, (double)yNum / (double)xDenom);
			UpdateOutputDC();
		}
		void Read_META_SCALEWINDOWEXT()
		{
                    LOG_TRACE
			short yDenom, yNum, xDenom, xNum;
			m_oStream >> yDenom >> yNum >> xDenom >> xNum;
			m_pDC->SetWindowScale((double)xNum / (double)xDenom, (double)yNum / (double)xDenom);
			UpdateOutputDC();
		}
		void Read_META_SETBKCOLOR()
		{
                    LOG_TRACE
			TWmfColor oColor;
			m_oStream >> oColor;
			m_pDC->SetTextBgColor(oColor);
			UpdateOutputDC();
		}
		void Read_META_SETBKMODE()
		{
                    LOG_TRACE
			unsigned short ushMode;
			m_oStream >> ushMode;
			m_pDC->SetTextBgMode(ushMode);
			UpdateOutputDC();
		}
		void Read_META_SETLAYOUT()
		{
                    LOG_TRACE
			unsigned short ushLayout, ushReserved;
			m_oStream >> ushLayout >> ushReserved;
			m_pDC->SetLayout(ushLayout);
			UpdateOutputDC();
		}
		void Read_META_SETMAPMODE()
		{
                    LOG_TRACE
			unsigned short ushMapMode; 
			m_oStream >> ushMapMode;
			m_pDC->SetMapMode(ushMapMode);
			UpdateOutputDC();
		}
		void Read_META_SETPOLYFILLMODE()
		{
                    LOG_TRACE
			unsigned short ushMode;
			m_oStream >> ushMode;
			m_pDC->SetPolyFillMode(ushMode);
			UpdateOutputDC();
		}
		void Read_META_SETROP2()
		{
                    LOG_TRACE
			unsigned short ushMode;
			m_oStream >> ushMode;
			m_pDC->SetRop2Mode(ushMode);
			UpdateOutputDC();
		}
		void Read_META_SETSTRETCHBLTMODE()
		{
                    LOG_TRACE
			unsigned short ushMode;
			m_oStream >> ushMode;
			m_pDC->SetStretchBltMode(ushMode);
			UpdateOutputDC();
		}
		void Read_META_SETTEXTALIGN()
		{
                    LOG_TRACE
			unsigned short ushTextAlign;
			m_oStream >> ushTextAlign;
			m_pDC->SetTextAlign(ushTextAlign);
			UpdateOutputDC();
		}
		void Read_META_SETTEXTCHAREXTRA()
		{
                    LOG_TRACE
			unsigned short ushCharSpacing;
			m_oStream >> ushCharSpacing;
			m_pDC->SetCharSpacing(ushCharSpacing);
			UpdateOutputDC();
		}
		void Read_META_SETTEXTCOLOR()
		{
                    LOG_TRACE
			TWmfColor oColor;
			m_oStream >> oColor;
			m_pDC->SetTextColor(oColor);
			UpdateOutputDC();
		}
		void Read_META_SETTEXTJUSTIFICATION()
		{
                    LOG_TRACE
			unsigned short ushBreakCount, ushBreakExtra;
			m_oStream >> ushBreakCount >> ushBreakExtra;
			// TODO: Реализовать
			UpdateOutputDC();
		}
		void Read_META_SETVIEWPORTEXT()
		{
                    LOG_TRACE
			short shX, shY;
			m_oStream >> shY >> shX;
			m_pDC->SetViewportExt(shX, shY);
			UpdateOutputDC();
		}
		void Read_META_SETVIEWPORTORG()
		{
                    LOG_TRACE
			short shX, shY;
			m_oStream >> shY >> shX;
			m_pDC->SetViewportOrg(shX, shY);
			UpdateOutputDC();
		}
		void Read_META_SETWINDOWEXT()
		{
                    LOG_TRACE
			short shX, shY;
			m_oStream >> shY >> shX;
			m_pDC->SetWindowExt(shX, shY);
			UpdateOutputDC();
		}
		void Read_META_SETWINDOWORG()
		{
                    LOG_TRACE
			short shX, shY;
			m_oStream >> shY >> shX;
			m_pDC->SetWindowOrg(shX, shY);
			UpdateOutputDC();
		}
                void  Read_META_ESCAPE()
		{
                    LOG_TRACE
			unsigned short ushEscapeFunction;
                        m_oStream >> ushEscapeFunction;

                        switch (ushEscapeFunction)
                        {
                            case NEWFRAME: READ_NEWFRAME(); break;
                            case ABORTDOC: READ_ABORTDOC(); break;
                            case NEXTBAND: READ_NEXTBAND(); break;
                            case SETCOLORTABLE: READ_SET_COLORTABLE(); break;
                            case GETCOLORTABLE: READ_GET_COLORTABLE(); break;
                            case FLUSHOUTPUT: READ_UNSUPPORTED(); break;
                            case DRAFTMODE: READ_UNSUPPORTED(); break;
                            case QUERYESCSUPPORT: READ_QUERY_ESCSUPPORT(); break;
                            case SETABORTPROC: READ_UNSUPPORTED(); break;
                            case STARTDOC: READ_STARTDOC(); break;
                            case ENDDOC: READ_ENDDOC(); break;
                            case GETPHYSPAGESIZE: READ_GET_PHYSPAGESIZE(); break;
                            case GETPRINTINGOFFSET: READ_GET_PRINTINGOFFSET(); break;
                            case GETSCALINGFACTOR: READ_GET_SCALINGFACTOR();  break;
                            case MFCOMMENT: READ_META_ESCAPE_ENHANCED_METAFILE();  break;
                            case GETPENWIDTH /* SETPENWIDTH */: READ_UNSUPPORTED(); break;
                            case SETCOPYCOUNT: READ_SET_COPYCOUNT(); break;
                            case SELECTPAPERSOURCE: READ_UNSUPPORTED(); break;
                            case PASSTHROUGH: READ_PASSTHROUGH(); break;
                            case GETTECHNOLOGY: READ_UNSUPPORTED(); break;
                            case SETLINECAP: READ_SET_LINECAP(); break;
                            case SETLINEJOIN: READ_SET_LINEJOIN(); break;
                            case SETMITERLIMIT: READ_SET_MITERLIMIT(); break;
                            case BANDINFO: READ_UNSUPPORTED(); break;
                            case DRAWPATTERNRECT: READ_DRAW_PATTERNRECT(); break;
                            case GETVECTORPENSIZE: READ_UNSUPPORTED(); break;
                            case GETVECTORBRUSHSIZE: READ_UNSUPPORTED(); break;
                            case ENABLEDUPLEX: READ_UNSUPPORTED(); break;
                            case GETSETPAPERBINS: READ_UNSUPPORTED(); break;
                            case GETSETPRINTORIENT: READ_UNSUPPORTED(); break;
                            case ENUMPAPERBINS: READ_UNSUPPORTED(); break;
                            case SETDIBSCALING: READ_UNSUPPORTED(); break;
                            case EPSPRINTING: READ_EPS_PRINTING(); break;
                            case ENUMPAPERMETRICS: READ_UNSUPPORTED(); break;
                            case GETSETPAPERMETRICS: READ_UNSUPPORTED(); break;
                            case POSTSCRIPT_DATA: READ_POSTSCRIPT_DATA(); break;
                            case POSTSCRIPT_IGNORE: READ_POSTSCRIPT_IGNORE(); break;
                            case GETDEVICEUNITS: READ_GET_DEVICEUNITS(); break;
                            case GETEXTENDEDTEXTMETRICS: READ_GET_EXTENDED_TEXTMETRICS(); break;
                            case GETPAIRKERNTABLE: READ_GET_PAIRKERNTABLE(); break;
                            case EXTTEXTOUT: READ_EXTTEXTOUT(); break;
                            case GETFACENAME: READ_GET_FACENAME(); break;
                            case DOWNLOADFACE: READ_DOWNLOAD_FACE(); break;
                            case METAFILE_DRIVER: READ_METAFILE_DRIVER(); break;
                            case QUERYDIBSUPPORT: READ_QUERY_DIBSUPPORT(); break;
                            case BEGIN_PATH: READ_BEGIN_PATH(); break;
                            case CLIP_TO_PATH: READ_CLIP_TO_PATH(); break;
                            case END_PATH: READ_END_PATH(); break;
                            case OPENCHANNEL: READ_OPEN_CHANNEL(); break;
                            case DOWNLOADHEADER: READ_DOWNLOAD_HEADER(); break;
                            case CLOSECHANNEL: READ_CLOSE_CHANNEL(); break;
                            case POSTSCRIPT_PASSTHROUGH: READ_POSTSCRIPT_PASSTHROUGH(); break;
                            case ENCAPSULATED_POSTSCRIPT: READ_ENCAPSULATED_POSTSCRIPT(); break;
                            case POSTSCRIPT_IDENTIFY: READ_POSTSCRIPT_IDENTIFY(); break;
                            case POSTSCRIPT_INJECTION: READ_POSTSCRIPT_INJECTION(); break;
                            case CHECKJPEGFORMAT: READ_CHECK_JPEGFORMAT(); break;
                            case CHECKPNGFORMAT: READ_CHECK_PNGFORMAT(); break;
                            case GET_PS_FEATURESETTING: READ_GET_PS_FEATURESETTING(); break;
                            case GDIPLUS_TS_QUERYVER: READ_UNSUPPORTED(); break;
                            case SPCLPASSTHROUGH2: READ_SPCLPASSTHROUGH2(); break;
                            default: READ_UNKNOWN(); break;
                        }
			// TODO: Реализовать
		}

                void READ_ABORTDOC()
                {
                    LOG_TRACE
                }

                void READ_BEGIN_PATH()
                {
                    LOG_TRACE
                }

                void READ_CHECK_JPEGFORMAT()
                {
                    LOG_TRACE
                }

                void READ_CHECK_PNGFORMAT()
                {
                    LOG_TRACE
                }

                void READ_CLIP_TO_PATH()
                {
                    LOG_TRACE
                }

                void READ_CLOSE_CHANNEL()
                {
                    LOG_TRACE
                }

                void READ_DOWNLOAD_FACE()
                {
                    LOG_TRACE
                }

                void READ_DOWNLOAD_HEADER()
                {
                    LOG_TRACE
                }

                void READ_DRAW_PATTERNRECT()
                {
                    LOG_TRACE
                }

                void READ_ENCAPSULATED_POSTSCRIPT()
                {
                    LOG_TRACE
                }

                void READ_END_PATH()
                {
                    LOG_TRACE
                }

                void READ_ENDDOC()
                {
                    LOG_TRACE
                }

                void READ_EPS_PRINTING()
                {
                    LOG_TRACE
                }

                void READ_EXTTEXTOUT()
                {
                    LOG_TRACE
                }

                void READ_GET_COLORTABLE()
                {
                    LOG_TRACE
                    unsigned short ushByteCount;
                    unsigned short ushBegin;
                    m_oStream >> ushByteCount;
                    m_oStream >> ushBegin;
                }

                void READ_GET_DEVICEUNITS()
                {
                    LOG_TRACE

                }

                void READ_GET_EXTENDED_TEXTMETRICS()
                {
                    LOG_TRACE
                }

                void READ_GET_FACENAME()
                {
                    LOG_TRACE
                }

                void READ_GET_PAIRKERNTABLE()
                {
                    LOG_TRACE
                }

                void READ_GET_PHYSPAGESIZE()
                {
                    LOG_TRACE
                }

                void READ_GET_PRINTINGOFFSET()
                {
                    LOG_TRACE
                }

                void READ_GET_PS_FEATURESETTING()
                {
                    LOG_TRACE
                }

                void READ_GET_SCALINGFACTOR()
                {
                    LOG_TRACE
                }

                void READ_META_ESCAPE_ENHANCED_METAFILE()
                {
                    LOG_TRACE
                    unsigned short ushByteCount;
                    unsigned int uiCommentIdentifier;
                    unsigned int uiCommentType;
                    unsigned int uiVersion;
                    unsigned short ushChecksum;
                    unsigned int uiBunting;
                    unsigned int uiCommentRecordCount;
                    unsigned int uiCurrentRecordSize;
                    unsigned int uiRemainingBytes;
                    unsigned int uiEnhancedMetafileDataSize;

                    m_oStream >> ushByteCount;
                    m_oStream >> uiCommentIdentifier;
                    m_oStream >> uiCommentType;
                    m_oStream >> uiVersion;
                    m_oStream >> ushChecksum;
                    m_oStream >> uiBunting;
                    m_oStream >> uiCommentRecordCount;
                    m_oStream >> uiCurrentRecordSize;
                    m_oStream >> uiRemainingBytes;
                    m_oStream >> uiEnhancedMetafileDataSize;

//                    std::wcout << L"----------------------" << std::endl;
//                    std::wcout << ushByteCount << std::endl;
//                    std::wcout << uiCommentIdentifier << std::endl;
//                    std::wcout << uiCommentType << std::endl;
//                    std::wcout << uiVersion << std::endl;
//                    std::wcout << uiBunting << std::endl;
//                    std::wcout << uiCommentRecordCount << std::endl;
//                    std::wcout << uiCurrentRecordSize << std::endl;
//                    std::wcout << uiRemainingBytes << std::endl;
//                    std::wcout << uiEnhancedMetafileDataSize << std::endl;
//                    std::wcout << L"----------------------" << std::endl;

                    if (uiCommentIdentifier == 0x43464D57 &&
                        uiCommentType       == 0x00000001 &&
                        uiVersion           == 0x00010000 &&
                        uiBunting           == 0x00000000 &&
                        uiCurrentRecordSize <= 0x00002000)
                    {
                        unsigned int ulSize, ulType;

                        m_oStream >> ulType;
                        m_oStream >> ulSize;

                        switch (ulType)
                        {
                                //-----------------------------------------------------------
                                // 2.3.1 Bitmap
                                //-----------------------------------------------------------
                                case EMR_ALPHABLEND:        LOGGING(L"EMR_ALPHABLEND"); break;
                                case EMR_BITBLT:            LOGGING(L"EMR_BITBLT"); break;
                                case EMR_STRETCHDIBITS:     LOGGING(L"EMR_STRETCHDIBITS"); break;
                                case EMR_SETDIBITSTODEVICE: LOGGING(L"EMR_SETDIBITSTODEVICE"); break;
                                case EMR_STRETCHBLT:        LOGGING(L"EMR_STRETCHBLT"); break;
                                        //-----------------------------------------------------------
                                        // 2.3.2 Clipping
                                        //-----------------------------------------------------------
                                case EMR_EXCLUDECLIPRECT:   LOGGING(L"EMR_EXCLUDECLIPRECT"); break;
                                case EMR_EXTSELECTCLIPRGN:  LOGGING(L"EMR_EXTSELECTCLIPRGN"); break;
                                case EMR_INTERSECTCLIPRECT: LOGGING(L"EMR_INTERSECTCLIPRECT"); break;
                                case EMR_SELECTCLIPPATH:    LOGGING(L"EMR_SELECTCLIPPATH"); break;
                                case EMR_SETMETARGN:        LOGGING(L"EMR_SETMETARGN"); break;
                                        //-----------------------------------------------------------
                                        // 2.3.4 Control
                                        //-----------------------------------------------------------
                                case EMR_HEADER: LOGGING(L"EMR_HEADER"); break;
                                case EMR_EOF:    LOGGING(L"EMR_EOF"); break;
                                        //-----------------------------------------------------------
                                        // 2.3.5 Drawing
                                        //-----------------------------------------------------------
                                case EMR_ANGLEARC:          LOGGING(L"EMR_ANGLEARC"); break;
                                case EMR_ARC:               LOGGING(L"EMR_ARC"); break;
                                case EMR_ARCTO:             LOGGING(L"EMR_ARCTO"); break;
                                case EMR_CHORD:             LOGGING(L"EMR_CHORD"); break;
                                case EMR_ELLIPSE:           LOGGING(L"EMR_ELLIPSE"); break;
                                case EMR_EXTTEXTOUTA:       LOGGING(L"EMR_EXTTEXTOUTA"); break;
                                case EMR_EXTTEXTOUTW:       LOGGING(L"EMR_EXTTEXTOUTW"); break;
                                case EMR_FILLPATH:          LOGGING(L"EMR_FILLPATH"); break;
                                case EMR_LINETO:            LOGGING(L"EMR_LINETO"); break;
                                case EMR_PIE:               LOGGING(L"EMR_PIE"); break;
                                case EMR_POLYBEZIER:        LOGGING(L"EMR_POLYBEZIER"); break;
                                case EMR_POLYBEZIER16:      LOGGING(L"EMR_POLYBEZIER16"); break;
                                case EMR_POLYBEZIERTO:      LOGGING(L"EMR_POLYBEZIERTO"); break;
                                case EMR_POLYBEZIERTO16:    LOGGING(L"EMR_POLYBEZIERTO16"); break;
                                case EMR_POLYDRAW:          LOGGING(L"EMR_POLYDRAW"); break;
                                case EMR_POLYDRAW16:        LOGGING(L"EMR_POLYDRAW16"); break;
                                case EMR_POLYGON:           LOGGING(L"EMR_POLYGON"); break;
                                case EMR_POLYGON16:         LOGGING(L"EMR_POLYGON16"); break;
                                case EMR_POLYLINE:          LOGGING(L"EMR_POLYLINE"); break;
                                case EMR_POLYLINE16:        LOGGING(L"EMR_POLYLINE16"); break;
                                case EMR_POLYLINETO:        LOGGING(L"EMR_POLYLINETO"); break;
                                case EMR_POLYLINETO16:      LOGGING(L"EMR_POLYLINETO16"); break;
                                case EMR_POLYPOLYGON:       LOGGING(L"EMR_POLYPOLYGON"); break;
                                case EMR_POLYPOLYGON16:     LOGGING(L"EMR_POLYPOLYGON16"); break;
                                case EMR_POLYPOLYLINE:      LOGGING(L"EMR_POLYPOLYLINE"); break;
                                case EMR_POLYPOLYLINE16:    LOGGING(L"EMR_POLYPOLYLINE16"); break;
                                case EMR_POLYTEXTOUTA:      LOGGING(L"EMR_POLYTEXTOUTA"); break;
                                case EMR_POLYTEXTOUTW:      LOGGING(L"EMR_POLYTEXTOUTW"); break;
                                case EMR_RECTANGLE:         LOGGING(L"EMR_RECTANGLE"); break;
                                case EMR_ROUNDRECT:         LOGGING(L"EMR_ROUNDRECT"); break;
                                case EMR_SETPIXELV:         LOGGING(L"EMR_SETPIXELV"); break;
                                case EMR_SMALLTEXTOUT:      LOGGING(L"EMR_SMALLTEXTOUT"); break;
                                case EMR_STROKEANDFILLPATH: LOGGING(L"EMR_STROKEANDFILLPATH"); break;
                                case EMR_STROKEPATH:        LOGGING(L"EMR_STROKEPATH"); break;
                                        //-----------------------------------------------------------
                                        // 2.3.7 Object Creation
                                        //-----------------------------------------------------------
                                case EMR_CREATEBRUSHINDIRECT:     LOGGING(L"EMR_CREATEBRUSHINDIRECT"); break;
                                case EMR_CREATEDIBPATTERNBRUSHPT: LOGGING(L"EMR_CREATEDIBPATTERNBRUSHPT"); break;
                                case EMR_CREATEPALETTE:           LOGGING(L"EMR_CREATEPALETTE"); break;
                                case EMR_CREATEPEN:               LOGGING(L"EMR_CREATEPEN"); break;
                                case EMR_EXTCREATEFONTINDIRECTW:  LOGGING(L"EMR_EXTCREATEFONTINDIRECTW"); break;
                                case EMR_EXTCREATEPEN:            LOGGING(L"EMR_EXTCREATEPEN"); break;
                                        //-----------------------------------------------------------
                                        // 2.3.8 Object Manipulation
                                        //-----------------------------------------------------------
                                case EMR_SELECTOBJECT:  LOGGING(L"EMR_SELECTOBJECT"); break;
                                case EMR_DELETEOBJECT:  LOGGING(L"EMR_DELETEOBJECT"); break;
                                case EMR_SELECTPALETTE: LOGGING(L"EMR_SELECTPALETTE"); break;
                                        //-----------------------------------------------------------
                                        // 2.3.10 Path Bracket
                                        //-----------------------------------------------------------
                                case EMR_BEGINPATH:   LOGGING(L"EMR_BEGINPATH"); break;
                                case EMR_ENDPATH:     LOGGING(L"EMR_ENDPATH"); break;
                                case EMR_CLOSEFIGURE: LOGGING(L"EMR_CLOSEFIGURE"); break;
                                case EMR_FLATTENPATH: LOGGING(L"EMR_FLATTENPATH"); break;
                                case EMR_WIDENPATH:   LOGGING(L"EMR_WIDENPATH"); break;
                                case EMR_ABORTPATH:   LOGGING(L"EMR_ABORTPATH"); break;
                                        //-----------------------------------------------------------
                                        // 2.3.11 State
                                        //-----------------------------------------------------------
                                case EMR_MOVETOEX:          LOGGING(L"EMR_MOVETOEX"); break;
                                case EMR_SETARCDIRECTION:   LOGGING(L"EMR_SETARCDIRECTION"); break;
                                case EMR_SAVEDC:            LOGGING(L"EMR_SAVEDC"); break;
                                case EMR_RESTOREDC:         LOGGING(L"EMR_RESTOREDC"); break;
                                case EMR_SETTEXTCOLOR:      LOGGING(L"EMR_SETTEXTCOLOR"); break;
                                case EMR_SETTEXTALIGN:      LOGGING(L"EMR_SETTEXTALIGN"); break;
                                case EMR_SETBKMODE:         LOGGING(L"EMR_SETBKMODE"); break;
                                case EMR_SETMITERLIMIT:     LOGGING(L"EMR_SETMITERLIMIT"); break;
                                case EMR_SETPOLYFILLMODE:   LOGGING(L"EMR_SETPOLYFILLMODE"); break;
                                case EMR_SETMAPMODE:        LOGGING(L"EMR_SETMAPMODE"); break;
                                case EMR_SETWINDOWORGEX:    LOGGING(L"EMR_SETWINDOWORGEX"); break;
                                case EMR_SETWINDOWEXTEX:    LOGGING(L"EMR_SETWINDOWEXTEX"); break;
                                case EMR_SETVIEWPORTORGEX:  LOGGING(L"EMR_SETVIEWPORTORGEX"); break;
                                case EMR_SETVIEWPORTEXTEX:  LOGGING(L"EMR_SETVIEWPORTEXTEX"); break;
                                case EMR_SETBKCOLOR:        LOGGING(L"EMR_SETBKCOLOR"); break;
                                case EMR_SETSTRETCHBLTMODE: LOGGING(L"EMR_SETSTRETCHBLTMODE"); break;
                                case EMR_SETICMMODE:        LOGGING(L"EMR_SETICMMODE"); break;
                                case EMR_SETROP2:           LOGGING(L"EMR_SETROP2"); break;
                                case EMR_REALIZEPALETTE:    LOGGING(L"EMR_REALIZEPALETTE"); break;
                                case EMR_SETLAYOUT:         LOGGING(L"EMR_SETLAYOUT"); break;
                                case EMR_SETBRUSHORGEX:     LOGGING(L"EMR_SETBRUSHORGEX"); break;
                                        //-----------------------------------------------------------
                                        // 2.3.12 Transform
                                        //-----------------------------------------------------------
                                case EMR_SETWORLDTRANSFORM: LOGGING(L"EMR_MODIFYWORLDTRANSFORM"); break;
                                case EMR_MODIFYWORLDTRANSFORM: LOGGING(L"EMR_MODIFYWORLDTRANSFORM"); break;
                                        //-----------------------------------------------------------
                                        // Неподдерживаемые записи
                                        //-----------------------------------------------------------
                                case EMR_GDICOMMENT: LOGGING(L"EMR_GDICOMMENT"); break;
                                        //-----------------------------------------------------------
                                        // Неизвестные записи
                                        //-----------------------------------------------------------
                                default:
                                {
                                        LOGGING(L"UNKNOWN");
                                        break;
                                }
                        }
                    }

                }

                void READ_METAFILE_DRIVER()
                {
                    LOG_TRACE
                }

                void READ_NEWFRAME()
                {
                    LOG_TRACE
                }

                void READ_NEXTBAND()
                {
                    LOG_TRACE
                }

                void READ_PASSTHROUGH()
                {
                    LOG_TRACE
                }

                void READ_POSTSCRIPT_DATA()
                {
                    LOG_TRACE
                }

                void READ_POSTSCRIPT_IDENTIFY()
                {
                    LOG_TRACE
                }

                void READ_POSTSCRIPT_IGNORE()
                {
                    LOG_TRACE
                }

                void READ_POSTSCRIPT_INJECTION()
                {
                    LOG_TRACE
                }

                void READ_POSTSCRIPT_PASSTHROUGH()
                {
                    LOG_TRACE
                }

                void READ_OPEN_CHANNEL()
                {
                    LOG_TRACE
                }

                void READ_QUERY_DIBSUPPORT()
                {
                    LOG_TRACE
                }

                void READ_QUERY_ESCSUPPORT()
                {
                    LOG_TRACE
                    unsigned short ushByteCount;
                    unsigned short ushRequest;
                    m_oStream >> ushByteCount;
                    m_oStream >> ushRequest;

                    std::wcout << ushRequest << std::endl;
                }

                void READ_SET_COLORTABLE()
                {
                    LOG_TRACE
                    unsigned short ushByteCount;

                    m_oStream >> ushByteCount;
                    m_oStream.Skip(ushByteCount);
                }

                void READ_SET_COPYCOUNT()
                {
                    LOG_TRACE
                }

                void READ_SET_LINECAP()
                {
                    LOG_TRACE
                }

                void READ_SET_LINEJOIN()
                {
                    LOG_TRACE
                }

                void READ_SET_MITERLIMIT()
                {
                    LOG_TRACE
                    unsigned short ushByteCount;
                    int nMiterLimit;
                    m_oStream >> ushByteCount;
                    m_oStream >> nMiterLimit;

                    if (ushByteCount == 0x0004)
                    {
                        // TODO:
                    }
                }

                void READ_SPCLPASSTHROUGH2()
                {
                    LOG_TRACE
                }

                void READ_STARTDOC()
                {
                    LOG_TRACE
                }

                void READ_UNSUPPORTED()
                {
                    LOG_TRACE
                }

                void READ_UNKNOWN()
                {
                    LOG_TRACE
                }

	private:

		unsigned int   m_unRecordSize;
		unsigned int   m_unRecordPos;

		TWmfPlaceable  m_oPlaceable;
		TWmfHeader     m_oHeader;
		TRect          m_oRect;
		TRect          m_oDCRect;

		CWmfPlayer     m_oPlayer;
		CWmfDC*        m_pDC;

		TRect          m_oBoundingBox;
		bool           m_bFirstPoint;

		TXForm         m_oTransform;
	};
}

#endif // _METAFILE_WMF_WMFFILE_H
