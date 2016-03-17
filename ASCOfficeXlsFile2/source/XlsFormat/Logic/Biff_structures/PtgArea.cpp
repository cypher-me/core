
#include "PtgArea.h"
#include "CellRangeRef.h"
#include "PtgParam.h"
#include <Binary/CFRecord.h>

namespace XLS
{


PtgArea::PtgArea()
{
}


PtgArea::PtgArea(const std::wstring& word, const PtgDataType data_type)
:	OperandPtg(fixed_id | (static_cast<unsigned char>(data_type) << 5)),
	area(word)
{
}


BiffStructurePtr PtgArea::clone()
{
	return BiffStructurePtr(new PtgArea(*this));
}

//
//void PtgArea::setXMLAttributes(MSXML2::IXMLDOMElementPtr xml_tag)
//{
//	area.toXML(xml_tag);
//}


void PtgArea::storeFields(CFRecord& record)
{
	record << area;
}


void PtgArea::loadFields(CFRecord& record)
{
	record >> area;
}


void PtgArea::assemble(AssemblerStack& ptg_stack, PtgQueue& extra_data, bool full_ref)
{
	RgceArea range(area);
	if(!ptg_stack.empty())
	{
		// Check whether we should process PtgElfRadical's value
		PtgParam param(ptg_stack.top());
		if(param.getType() == PtgParam::ptELF_RADICAL)
		{
			ptg_stack.pop();
			if(range.getColumnFirst() == range.getColumnLast())
			{
				range.setColumnRelativity(0 != param.getFirstParam());
			}
			if(range.getRowFirst() == range.getRowLast())
			{
				range.setRowRelativity(0 != param.getFirstParam());
			}
		}
	}
	ptg_stack.push(range.toString());
}

} // namespace XLS

