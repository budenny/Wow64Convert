#include "stdafx.h"
#include "SectionPtr.h"
#include <assert.h>
#include "DumpMemory.h"


template <typename T, MINIDUMP_STREAM_TYPE TYPE>
T* GetDumpStream(void * dump)
{
	PMINIDUMP_DIRECTORY d = nullptr;
	T* out = nullptr;
	ULONG sz = 0;
	if (::MiniDumpReadDumpStream(dump, TYPE, &d, (PVOID*)&out, &sz) == FALSE && 
		::GetLastError() != ERROR_NOT_FOUND )
		throw std::runtime_error("MiniDumpReadDumpStream");

	return out;
}

static void SetCpuArchitectureX86(void * dump)
{
	PMINIDUMP_SYSTEM_INFO si = GetDumpStream<MINIDUMP_SYSTEM_INFO, SystemInfoStream>(dump);
	if (!si)//todo: validation
	{
		printf("SetCpuArchitectureX86 - system info not found");
		return;
	}
	

	si->ProcessorArchitecture = 0;
	printf("Set processor architecture to x86\n");
}

static void SwitchThreadTebToX86(PMINIDUMP_THREAD thrd)
{
	//todo: validation
	printf("thread id 0x%x teb 0x%I64x\n", thrd->ThreadId, thrd->Teb);
}

static void SwitchThreadsTebToX86(void * dump)
{
	PMINIDUMP_THREAD_LIST tl = GetDumpStream<MINIDUMP_THREAD_LIST, ThreadListStream>(dump);
	if (!tl)//todo: validation
	{
		printf("SwitchThreadsTebToX86 - thread list not found");
		return;
	}

	for (unsigned int i = 0; i < tl->NumberOfThreads; ++i)
		SwitchThreadTebToX86(tl->Threads + i);
}

static DumpMemory dm;

static bool ListMemX64(void * dump)
{
	PMINIDUMP_MEMORY64_LIST ml = GetDumpStream<MINIDUMP_MEMORY64_LIST, Memory64ListStream>(dump);
	if (!ml)//todo: validation
	{
		printf("ListMemX64 - memory blocks not found");
		return false;
	}

	ULONG64 data_addr = (ULONG64)dump + ml->BaseRva;
	for (ULONG64 i = 0; i < ml->NumberOfMemoryRanges; ++i)
	{
		dm.AddBlock(
			ml->MemoryRanges[i].StartOfMemoryRange,
			ml->MemoryRanges[i].StartOfMemoryRange + ml->MemoryRanges[i].DataSize,
			data_addr);
		
		data_addr += ml->MemoryRanges[i].DataSize;
	}

	return ml->NumberOfMemoryRanges != 0;
}

static bool ListMemX86(void * dump)
{
	PMINIDUMP_MEMORY_LIST ml = GetDumpStream<MINIDUMP_MEMORY_LIST, MemoryListStream>(dump);
	if (!ml)//todo: validation
	{
		printf("ListMemX64 - memory blocks not found");
		return false;
	}

	for (ULONG64 i = 0; i < ml->NumberOfMemoryRanges; ++i)
	{
		dm.AddBlock(
			ml->MemoryRanges[i].StartOfMemoryRange,
			ml->MemoryRanges[i].StartOfMemoryRange + ml->MemoryRanges[i].Memory.DataSize,
			(ULONG64)dump + ml->MemoryRanges[i].Memory.Rva);
	}

	return ml->NumberOfMemoryRanges != 0;
}


// 0x24cf9d8 024cf9da  00007709
// - partial mapping

static void MemTest(ULONG64 addr)
{
	ULONG64 translated = dm.TranslateAddress(addr);
	if (!translated)
	{
		printf("not found 0x%I64x\n", addr);
		return;
	}

	printf("0x%I64x 0x%x\n", addr, *((DWORD*)translated));
}

int _tmain(int argc, _TCHAR* argv[])
{
	const wchar_t dmp_file[] = L"d:\\x64.dmp";

	try
	{
		const size_t min_mapping_size = 0/*0x1000*/;
		const SectionPtr dump(CreateFileSectionRW(dmp_file), min_mapping_size);
		
		//SetCpuArchitectureX86(dump);
		//SwitchThreadsTebToX86(dump);
		if (!ListMemX64(dump))
			ListMemX86(dump);

		MemTest(0x24cf9d8);
		MemTest(0x24cf9da);
		MemTest(0x76ff0512);
		MemTest(0x77ff0512);
	}
	catch (const std::runtime_error & e)
	{
		printf("runtime err: %s le=%u\n", e.what(), ::GetLastError());
	}

	return 0;
}

