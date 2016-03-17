#pragma once

#include "Ptg.h"

namespace XLS
{

class CFRecord;

class PtgMemNoMemN : public Ptg
{
	BASE_STRUCTURE_DEFINE_CLASS_NAME(PtgMemNoMemN)
public:
	BiffStructurePtr clone();

	virtual void load(CFRecord& record);
	virtual void store(CFRecord& record);

	virtual void assemble(AssemblerStack& ptg_stack, PtgQueue& extra_data, bool full_ref = false);

private:
	unsigned char type1;
	unsigned short cce;
};

} // namespace XLS

