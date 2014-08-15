#pragma once

class DumpMemory
{
public:
	void AddBlock(const ULONG64 & start, const ULONG64 & end, const ULONG64 & ptr);
	ULONG64 TranslateAddress(const ULONG64 & addr) const;
		
	void Test() const;
private:

	struct MemoryBlock
	{
		ULONG64 start;
		ULONG64 end;
		ULONG64 ptr;

		explicit MemoryBlock(const ULONG64 & addr = 0)
			: start(addr)
			, end(addr + 1)
			, ptr(0)
		{
		};

		bool operator <(const MemoryBlock & r) const;
		bool Contains(const ULONG64 &v) const;
		ULONG64 Translate(const ULONG64 & addr) const;
	};

	typedef std::set<MemoryBlock> MemoryBlockSet;
	MemoryBlockSet _memory;

	const MemoryBlock * Find(const ULONG64 & addr) const;
};

