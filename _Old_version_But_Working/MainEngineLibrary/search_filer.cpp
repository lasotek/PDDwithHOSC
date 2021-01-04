#include "StdAfx.h"
#include "search_filer.h"
#include <Windows.h>
#include <AtlConv.h>
#include <atlbase.h>

using namespace ATL;
#ifdef _UNICODE
_search_filer::_search_filer(const string& PathPattern)
{
	USES_CONVERSION;
	wstring wPathPattern(CA2W(PathPattern.c_str()));
	Init(wPathPattern);
}

_search_filer::_search_filer(const wstring& PathPattern)
#else
_search_filer::_search_filer(const string& PathPattern)
#endif
{
	Init(PathPattern);
}

#ifdef _UNICODE
void _search_filer::Init(const wstring& PathPattern)
#else
void _search_filer::Init(const string& PathPattern)
#endif
{
	USES_CONVERSION;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind=FindFirstFile(PathPattern.c_str(),&FindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return;
	push_back(FindFileData.cFileName);
	while(FindNextFile(hFind,&FindFileData))
	{
		push_back(FindFileData.cFileName);
	}
	FindClose(hFind);
}

_search_filer::~_search_filer(void)
{
}
