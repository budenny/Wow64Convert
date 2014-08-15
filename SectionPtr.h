#pragma once

typedef std::shared_ptr<std::remove_pointer<HANDLE>::type> HandleHolder;
inline HandleHolder make_handle_holder(const HANDLE & h)
{
	return HandleHolder(h, &::CloseHandle);
}

class SectionPtr
{
public:
	SectionPtr(const HandleHolder & section, const size_t & sz);

	inline operator void*() const { return _ptr.get(); }

private:
	SectionPtr(const SectionPtr&);
	SectionPtr& operator=(const SectionPtr&);

	HandleHolder _section;

	struct Unmapper { void operator ()( void *p){ ::UnmapViewOfFile(p);} };
	std::unique_ptr<void, Unmapper> _ptr;

	static void * MapSection(const HandleHolder & fm, const size_t & sz);
};

HandleHolder CreateFileSectionRW(const std::wstring & fname);
