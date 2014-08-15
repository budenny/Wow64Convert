#include "stdafx.h"
#include "SectionPtr.h"

SectionPtr::SectionPtr( const HandleHolder & section, const size_t & sz )	
	: _section(section)
	, _ptr(MapSection(_section, sz))
{
}

void * SectionPtr::MapSection( const HandleHolder & fm, const size_t & sz )
{
	void *out = ::MapViewOfFile(fm.get(), FILE_MAP_ALL_ACCESS, 0, 0, sz);
	if (out == NULL)
		throw std::runtime_error("map file mapping");

	return out;
}

HandleHolder CreateFileSectionRW( const std::wstring & fname )
{
	const HandleHolder & file = make_handle_holder(
		::CreateFile(fname.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));

	if (file.get() == INVALID_HANDLE_VALUE)
		throw std::runtime_error("open file");

	const HandleHolder & fm = make_handle_holder(
		::CreateFileMapping(file.get(), NULL, PAGE_READWRITE, 0, 0, NULL));

	if (fm == NULL)
		throw std::runtime_error("create file mapping");

	return fm;
}