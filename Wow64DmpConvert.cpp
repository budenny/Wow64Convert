#include "stdafx.h"
#include "SectionPtr.h"
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

static void SwitchThreadTebToX86(PMINIDUMP_THREAD thrd, const DumpMemory::Ptr & dm)
{
	//todo: validation
	printf("thread id 0x%x teb 0x%I64x\n", thrd->ThreadId, thrd->Teb);
}

static void SwitchThreadsTebToX86(void * dump, const DumpMemory::Ptr & dm)
{
	PMINIDUMP_THREAD_LIST tl = GetDumpStream<MINIDUMP_THREAD_LIST, ThreadListStream>(dump);
	if (!tl)//todo: validation
	{
		printf("SwitchThreadsTebToX86 - thread list not found");
		return;
	}

	for (unsigned int i = 0; i < tl->NumberOfThreads; ++i)
		SwitchThreadTebToX86(tl->Threads + i, dm);
}

static DumpMemory * CreateDumpMemory(void * dump)
{
	std::unique_ptr<DumpMemory> dm(new DumpMemory());
	
	const PMINIDUMP_MEMORY64_LIST mlx64 = 
		GetDumpStream<MINIDUMP_MEMORY64_LIST, Memory64ListStream>(dump);
	if (mlx64) dm->AddBlocks(dump, mlx64);

	const PMINIDUMP_MEMORY_LIST mlx86 = 
		GetDumpStream<MINIDUMP_MEMORY_LIST, MemoryListStream>(dump);
	if (mlx86) dm->AddBlocks(dump, mlx86);

	return mlx64 || mlx86 ? dm.release() : nullptr;
}

static void MemTest(ULONG64 addr, const DumpMemory::Ptr & dm)
{
	ULONG64 translated = dm->TranslateAddress(addr);
	if (!translated)
	{
		printf("not found 0x%I64x\n", addr);
		return;
	}

	printf("0x%I64x 0x%x\n", addr, *((DWORD*)translated));
}

int _tmain(int argc, _TCHAR* argv[])
{
	// - partial mapping

	const wchar_t dmp_file[] = L"d:\\x64.dmp";

	try
	{
		const size_t min_mapping_size = 0/*0x1000*/;
		const SectionPtr dump(CreateFileSectionRW(dmp_file), min_mapping_size);
		
		//TODO: enable
		//SetCpuArchitectureX86(dump);
		
		DumpMemory::Ptr dm(CreateDumpMemory(dump));
		SwitchThreadsTebToX86(dump, dm);

		//MemTest(0x24cf9d8, dm);
		//MemTest(0x24cf9da, dm);
		//MemTest(0x76ff0512, dm);
		//MemTest(0x77ff0512, dm);
	}
	catch (const std::runtime_error & e)
	{
		printf("runtime err: %s le=%u\n", e.what(), ::GetLastError());
	}

	return 0;
}

