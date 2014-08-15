#pragma once
#include "IntervalTree.h"

class DumpMemory
{
public:

	void AddBlocks(void * dump, PMINIDUMP_MEMORY64_LIST ml);
	void AddBlocks(void * dump, PMINIDUMP_MEMORY_LIST ml);

	ULONG64 TranslateAddress(const ULONG64 & addr) const;

	typedef std::shared_ptr<DumpMemory> Ptr;

private:
	struct MemoryBlock : public Interval<ULONG64>
	{
		ULONG64 ptr;
		
		explicit MemoryBlock(const ULONG64 &start_); //XXX
		MemoryBlock(const ULONG64 &start_, const ULONG64 &end_, const ULONG64 &ptr_);
		ULONG64 TranslateAddress(const ULONG64 &addr) const;
	};

	IntervalTree<MemoryBlock> _memory;
};

