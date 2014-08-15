#pragma once
#include "IntervalTree.h"

class DumpMemory
{
public:
	void AddBlock(const ULONG64 & start, const ULONG64 & end, const ULONG64 & ptr);
	ULONG64 TranslateAddress(const ULONG64 & addr) const;
		
	void Test() const;
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

