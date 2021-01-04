#pragma once
#include "stdafx.h"

#include "InUseCheckpoint.h"
#include "CacheTmpl.h"

template<class OutputType>
class _CAbstractOperationCache;

template<class OutputType>
class _CAbstractOperationDataContainer : public _CHashable, public _CInUseCheckpoint
{
public:
	_CAbstractOperationDataContainer<OutputType>():m_AccessCounter(0) {}
	_CAbstractOperationDataContainer<OutputType>(const _CAbstractOperationDataContainer<OutputType>& Source):
		m_AccessCounter(Source.m_AccessCounter) {}
	virtual ~_CAbstractOperationDataContainer<OutputType>() {}
	//virtual OutputType* GetVertexResult1()=0;
	//virtual short& GetTopSgnResult1()=0;
	bool operator==(const _CAbstractOperationDataContainer& Right) {return IsEqualTo(Right);}
	virtual _CAbstractOperationDataContainer* AllocateCopy()=0;
	unsigned long PopAccessCounter() {return ++m_AccessCounter;}
	unsigned long AccessCounter() const {return m_AccessCounter;}
protected:
	virtual bool IsEqualTo(const _CAbstractOperationDataContainer& Right)=0;
	unsigned long m_AccessCounter;
};

template<class OutputType>
class _CAbstractOperationCache : public _CCacheTmpl<typename _CAbstractOperationDataContainer<OutputType>>
{
protected:
	unsigned long m_MaxAccessCounter;
	path_str m_PathTempFile;
public:
	typedef typename _CAbstractOperationDataContainer<OutputType> OutputContainer;
	_CAbstractOperationCache<OutputType>():m_MaxAccessCounter(0) {}
	~_CAbstractOperationCache<OutputType>() 
	{
	}
	typedef _CAbstractOperationDataContainer<OutputType> _COperationDataContainer;

	void CacheTheResult(_COperationDataContainer* pContainer)
	{
		_COperationDataContainer* pOldResult=FindExisting(pContainer);
		if(pOldResult!=NULL)
			RISEPDD(eIllegalOperation,"The result already exist.\n"
									  "Use FindResult before a determination of the result.");
		PushInCache(pContainer);
	}
	_COperationDataContainer* GetExistingResult(_COperationDataContainer* pContainer) 
	{
		_COperationDataContainer* pRes=FindExisting(pContainer);
		return pRes;
	}
	OutputContainer* FindExisting(OutputContainer* pContainer)
	{
		OutputContainer* pRes=_CCacheTmpl<typename _CAbstractOperationDataContainer<OutputType>>::FindExisting(pContainer);
		if(pRes!=NULL)
			m_MaxAccessCounter=max(m_MaxAccessCounter,pRes->PopAccessCounter());
		return pRes;
	}

	OutputContainer* FindOrRegisterResult(OutputContainer* pContainer)
	{
		OutputContainer* pResContainer=FindExisting(pContainer);
		if(pResContainer==NULL)
		{
			pResContainer=pContainer->AllocateCopy();
			ForceIntoCache(pResContainer);
		}
		return pResContainer;
	}
	void RegisterResult(OutputContainer* pContainer)
	{
		OutputContainer* pResContainer=pContainer->AllocateCopy();
		ForceIntoCache(pResContainer);
	}
};
