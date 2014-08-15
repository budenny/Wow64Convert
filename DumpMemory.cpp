#include "stdafx.h"
#include "DumpMemory.h"

void DumpMemory::AddBlock( const ULONG64 & start, const ULONG64 & end, const ULONG64 & ptr )
{
	MemoryBlock mb;
	mb.start = start;
	mb.end = end;
	mb.ptr = ptr;
	_memory.insert(mb);
}

ULONG64 DumpMemory::TranslateAddress( const ULONG64 & addr ) const
{
	const MemoryBlock * mb = Find(addr);
	if (!mb)
		return 0;
	
	return mb->Translate(addr);
}

const DumpMemory::MemoryBlock * DumpMemory::Find( const ULONG64 & addr ) const
{
	const MemoryBlock * out = nullptr;
	MemoryBlockSet::const_iterator i = _memory.lower_bound(MemoryBlock(addr));
	if(i == _memory.end() || addr < i->start)
	{
		if(i == _memory.begin())
			return out;

		--i;
	}

	if (i->Contains(addr))
		out = &*i;

	return out;
}

void DumpMemory::Test() const
{
	for(MemoryBlockSet::const_iterator i = _memory.begin(); i != _memory.end(); ++i)
	{
		printf("X86 addr=0x%I64x end=0x%I64x ptr=0x%I64x 0x%x\n",
			i->start, i->end, i->ptr, *((DWORD*)i->ptr));
	}

}

bool DumpMemory::MemoryBlock::operator<( const MemoryBlock & r ) const
{
	return start < r.start;
}

bool DumpMemory::MemoryBlock::Contains( const ULONG64 &v ) const
{
	return v >= start && v < end;
}

ULONG64 DumpMemory::MemoryBlock::Translate( const ULONG64 & addr ) const
{

	ULONG64 out = ptr + (addr - start);
	printf("Translate: 0x%I64x 0x%I64x 0x%I64x 0x%I64x\n",
		addr, start, out, ptr);
	return out;
}
