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
	if (!si)
	{
		printf("SetCpuArchitectureX86 - system info not found");
		return;
	}
	
	si->ProcessorArchitecture = 0;
	printf("Set processor architecture to x86\n");
}

static void SwitchThreadTebToX86(PMINIDUMP_THREAD thrd, const DumpMemory::Ptr & dm)
{
	const ULONG64 original_teb = dm->TranslateAddress(thrd->Teb);
	if (!original_teb)
	{
		printf("SwitchThreadTebToX86(0x%x) - there is no memory block(0x%I64x) in dump\n",
			thrd->ThreadId, thrd->Teb);
		return;
	}

	const DWORD wow64_teb = *((DWORD*)original_teb);

	printf("Change thread 0x%x TEB 0x%I64x->0x%x\n",
		thrd->ThreadId, thrd->Teb, wow64_teb);

	thrd->Teb = wow64_teb;
}

static void SwitchThreadsTebToX86(void * dump, const DumpMemory::Ptr & dm)
{
	PMINIDUMP_THREAD_LIST tl = GetDumpStream<MINIDUMP_THREAD_LIST, ThreadListStream>(dump);
	if (!tl)
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

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		printf("usage: Wow64DmpConvert.exe dmp_file\n");
		return -1;
	}


	try
	{
		const SectionPtr dump(CreateFileSectionRW(argv[1]), 0);
		
		SetCpuArchitectureX86(dump);
		
		DumpMemory::Ptr dm(CreateDumpMemory(dump));
		SwitchThreadsTebToX86(dump, dm);
	}
	catch (const std::runtime_error & e)
	{
		printf("runtime err: %s le=%u\n", e.what(), ::GetLastError());
	}

	return 0;
}
