#include "stdafx.h"
#include "DumpMemory.h"

ULONG64 DumpMemory::TranslateAddress( const ULONG64 & addr ) const
{
	const MemoryBlock * mb = _memory.Find(addr);
	if (!mb)
		return 0;
	
	return mb->TranslateAddress(addr);
}

void DumpMemory::AddBlocks( void * dump, PMINIDUMP_MEMORY64_LIST ml )
{
	ULONG64 data_addr = (ULONG64)dump + ml->BaseRva;
	for (unsigned int i = 0; i < ml->NumberOfMemoryRanges; ++i)
	{
		const MemoryBlock mb(
			ml->MemoryRanges[i].StartOfMemoryRange,
			ml->MemoryRanges[i].StartOfMemoryRange + ml->MemoryRanges[i].DataSize,
			data_addr);

		_memory.Insert(mb);
		data_addr += ml->MemoryRanges[i].DataSize;
	}
}

void DumpMemory::AddBlocks( void * dump, PMINIDUMP_MEMORY_LIST ml )
{
	for (unsigned int i = 0; i < ml->NumberOfMemoryRanges; ++i)
	{
		const MemoryBlock mb(
			ml->MemoryRanges[i].StartOfMemoryRange,
			ml->MemoryRanges[i].StartOfMemoryRange + ml->MemoryRanges[i].Memory.DataSize,
			(ULONG64)dump + ml->MemoryRanges[i].Memory.Rva);

		_memory.Insert(mb);
	}
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
	return ptr + (addr - start);
}
