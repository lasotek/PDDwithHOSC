#pragma once
#include <set>
#include <hash_map>
#include <string>

class _CUniqStringBuffer
{
public:
	_CUniqStringBuffer(void);
	~_CUniqStringBuffer(void);
	const string* CheckUniqName(const string& Context) const;
	const string& GetUniqueString(const string& Source);
protected:
	typedef set<const string> STRING_SET;
	STRING_SET m_Set;
};

class _CUniqStringBufferWithCounter
{
public:
	typedef hash_map<string,unsigned long> STRING_SET;
	typedef STRING_SET::value_type value_type;
	const string* CheckUniqName(const string& String) const;
	const string& GetUniqueString(const string& String, unsigned long** ppCounter=NULL);
	unsigned long& GetCurrentCounter(const string& String);
protected:
	typedef hash_map<string,unsigned long> STRING_SET;
	STRING_SET m_Set;
};