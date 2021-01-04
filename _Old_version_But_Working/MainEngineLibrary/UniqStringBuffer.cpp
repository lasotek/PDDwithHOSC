#include "StdAfx.h"
#include "UniqStringBuffer.h"

_CUniqStringBuffer::_CUniqStringBuffer(void)
{
}

_CUniqStringBuffer::~_CUniqStringBuffer(void)
{
}


const string* _CUniqStringBuffer::CheckUniqName(const string& Context) const
{
	if(Context.empty())
		return &EmptyString;
	STRING_SET::const_iterator it=m_Set.find(Context);
	if(it==m_Set.end())
		return NULL;
	else
		return &(*it);
}


const string& _CUniqStringBuffer::GetUniqueString(const string& Source)
{
	if(Source.empty())
		return EmptyString;
	return *(m_Set.insert(Source).first);
}


const string* _CUniqStringBufferWithCounter::CheckUniqName(const string& String) const
{
	STRING_SET::const_iterator it=m_Set.find(String);
	if(it!=m_Set.end())
		return &(it->first);
	return NULL;
}

const string& _CUniqStringBufferWithCounter::GetUniqueString(const string& String, unsigned long** ppCounter)
{
	STRING_SET::iterator it=m_Set.find(String);
	if(it!=m_Set.end())
	{
		if(ppCounter!=NULL)
			*ppCounter=&(it->second);
		return it->first;
	}
	it=m_Set.insert(STRING_SET::value_type(String,0)).first;
	if(ppCounter!=NULL)
		*ppCounter=&(it->second);
	return it->first;
}

unsigned long& _CUniqStringBufferWithCounter::GetCurrentCounter(const string& String)
{
	STRING_SET::iterator it=m_Set.find(String);
	if(it!=m_Set.end())
		return it->second;
	return m_Set.insert(STRING_SET::value_type(String,0)).first->second;
}
