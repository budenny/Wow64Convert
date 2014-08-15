#include "stdafx.h"
#include "DumpMemory.h"

void DumpMemory::AddBlock( const ULONG64 & start, const ULONG64 & end, const ULONG64 & ptr )
{
	MemoryBlock mb(start, end, ptr);
	_memory.Insert(mb);
}

ULONG64 DumpMemory::TranslateAddress( const ULONG64 & addr ) const
{
	const MemoryBlock * mb = _memory.Find(addr);
	if (!mb)
		return 0;
	
	return mb->TranslateAddress(addr);
}

DumpMemory::MemoryBlock::MemoryBlock( 
	const ULONG64 &start_,
	const ULONG64 &end_,
	const ULONG64 &ptr_ )	
	: Interval<ULONG64>(start_, end_)
	, ptr(ptr_)
{
}

DumpMemory::MemoryBlock::MemoryBlock( const ULONG64 &start_ )
	: Interval<ULONG64>(start_)
	, ptr(0)
{
}

ULONG64 DumpMemory::MemoryBlock::TranslateAddress( const ULONG64 &addr ) const
{
	ULONG64 out = ptr + (addr - start);
	printf("Translate: 0x%I64x 0x%I64x 0x%I64x 0x%I64x\n",
		addr, start, out, ptr);
	return out;
}
