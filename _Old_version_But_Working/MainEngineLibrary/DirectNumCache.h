#pragma once
#include "Vertex.h"
#include "AbstractOperationDataContainer.h"
#include "norm_float.h"
#include "CompRedefiner.h"

//class _CTempCacheFiler : public _binary_filer
//{
//public:
//	void open(_binary_filer::OPEN_MODE mode, const mstr& FileName);
//};

typedef _CAbstractOperationDataContainer<_CSubModelNumericPattern> _CAbstractDirectNumContainer;
class _CDirectNumContainer : public _CAbstractOperationDataContainer<_CSubModelNumericPattern>
{
public:
	//_CDirectNumContainer(const _CCompRedefEntry* pCompRedefEntry, _CModelVertex* pVertex, const string* Context = &EmptyString) :
	//	m_pVertex(pVertex), m_Context(Context), m_PosInFile(-1), 
	//	m_pRedefEntry(pCompRedefEntry)
	//{}
	_CDirectNumContainer(const _CCompRedefEntry* pCompRedefEntry, _CModelVertex* pVertex, const string* Context = &EmptyString) :
		m_pVertex(pVertex), m_Context(Context), m_PosInFile(-1), m_pRes(make_shared<_CSubModelNumericPattern>()), 
		m_pRedefEntry(pCompRedefEntry)
	{}
	_CDirectNumContainer(const _CDirectNumContainer& Source) : _CAbstractOperationDataContainer<_CSubModelNumericPattern>(Source),
		m_pVertex(Source.m_pVertex), m_Context(Source.m_Context),
		m_PosInFile(Source.m_PosInFile), m_pRes(Source.m_pRes), m_pRedefEntry(Source.m_pRedefEntry)
	{}
	virtual ~_CDirectNumContainer()
	{
		//if (m_pRes != NULL)
		//	delete m_pRes;
	}
	_CSubModelNumericPattern* GetVertexResult();
	void ReleaseResult();
	//virtual short& GetTopSgnResult() {return m_TopSgn; }
	virtual _CAbstractDirectNumContainer* AllocateCopy() { return this; }
	long long GetVertexIndex() { return m_pVertex->GetMyId(); }
	const string& GetVertexAssociatedName() { return m_pVertex->GetAssociatedValueName(); }
	_CDirectNumContainer& operator=(const _CDirectNumContainer& Left);
	void CachItToHD();
	void Hibernate();
protected:
	virtual bool IsEqualTo(const _CAbstractDirectNumContainer& Right);
	virtual size_t DetermineHashKey(size_t Core = HASH_CORE) override;
	shared_ptr<_CSubModelNumericPattern> m_pRes = {};
	//_CSubModelNumericPattern m_Res;
	_CModelVertex* m_pVertex;
	const string* m_Context;
	//_CCompRedefEntryShrdPtr m_pRedefEntry;
	const _CCompRedefEntry* m_pRedefEntry;
	long long m_PosInFile;
	//unsigned m_UsageCounter;
	//short m_TopSgn;
	_CTempCacheFiler* m_p_tmp_file = nullptr;
	friend class _CDirectNumCache;
};

//class _CDirectNumContainer : public _CAbstractOperationDataContainer<_CSubModelNumericPattern>
//{
//public:
//	_CDirectNumContainer(_CModelVertex* pVertex, const string* Context = &EmptyString) :
//		m_pVertex(pVertex), m_Context(Context), m_PosInFile(-1), m_pRes(new _CSubModelNumericPattern)
//	{}
//	_CDirectNumContainer(const _CDirectNumContainer& Source) : _CAbstractOperationDataContainer<_CSubModelNumericPattern>(Source),
//		m_pVertex(Source.m_pVertex), m_Context(Source.m_Context),
//		m_PosInFile(Source.m_PosInFile), m_pRes(new _CSubModelNumericPattern(*Source.m_pRes))
//	{}
//	virtual ~_CDirectNumContainer()
//	{
//		if (m_pRes != NULL)
//			delete m_pRes;
//	}
//	virtual _CSubModelNumericPattern* GetVertexResult();
//	void ReleaseResult();
//	//virtual short& GetTopSgnResult() {return m_TopSgn; }
//	virtual _CAbstractDirectNumContainer* AllocateCopy() { return this; }
//	long long GetVertexIndex() { return m_pVertex->GetMyId(); }
//	const string& GetVertexAssociatedName() { return m_pVertex->GetAssociatedValueName(); }
//	_CDirectNumContainer& operator=(const _CDirectNumContainer& Left);
//	void CachItToHD();
//	void Hibernate();
//protected:
//	virtual bool IsEqualTo(const _CAbstractDirectNumContainer& Right);
//	virtual long DetermineHashKey(long Core = HASH_CORE);
//	_CSubModelNumericPattern* m_pRes;
//	_CModelVertex* m_pVertex;
//	const string* m_Context;
//	long long m_PosInFile;
//	//unsigned m_UsageCounter;
//	//short m_TopSgn;
//	_CTempCacheFiler* m_p_tmp_file;
//	friend class _CDirectNumCache;
//};

class _CDirNumContainerWrapper
{
public:
	_CDirNumContainerWrapper(_CDirectNumContainer* pNumContainer=NULL)
		:m_pNumContainer(pNumContainer) {}
	~_CDirNumContainerWrapper()
	{
		if(m_pNumContainer!=NULL)
			m_pNumContainer->ReleaseResult();
	}
	operator  _CDirectNumContainer*&()  {return m_pNumContainer;}
protected:
	 _CDirectNumContainer* m_pNumContainer;
};

class _CDirectNumCache : public _CAbstractOperationCache<_CSubModelNumericPattern>
{
public:
	_CDirectNumCache();
	~_CDirectNumCache()
	{
		//ReportHitTest();
	}
	void Clear()
	{
		//ReportHitTest();
		_CAbstractOperationCache<_CSubModelNumericPattern>::Clear();
	}
	void ReportHitTest();
	_CDirectNumContainer* FindOrRegisterResult(_CDirectNumContainer* pContainer);
	void RegisterResult(_CDirectNumContainer* pContainer);
	void HibernateAll();
protected:
	_CTempCacheFiler m_tmp_file;
};

class _CDirNumContainers : public self_exp_vector<_CDirectNumContainer*>
{
public:
	_CDirNumContainers(size_t Size=0):self_exp_vector<_CDirectNumContainer*>(Size) {}
	~_CDirNumContainers();
};
//typedef _CAbstractOperationCache<_CSubModelNumericPattern> _CDirectNumCache;