#pragma once
#include <string>
#include <list>
#ifdef _UNICODE
#else
#endif

#ifdef _UNICODE
class _search_filer : public list<wstring>
#else
class _search_filer : public list<string>
#endif
{
public:
#ifdef _UNICODE
	_search_filer(const wstring& PathPattern);
#endif
	_search_filer(const string& PathPattern);
	~_search_filer(void);
protected:
#ifdef _UNICODE
	void Init(const wstring& PathPattern);
#else
	void Init(const string& PathPattern);
#endif
};
