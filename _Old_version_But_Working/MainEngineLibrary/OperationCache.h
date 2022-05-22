#pragma once
#include "CacheTmpl.h"
#include "InUseCheckpoint.h"
#include <vector>
#include "Vertex.h"
#include "AbstractOperationDataContainer.h"


typedef _CAbstractOperationDataContainer<_CSExpandedVertices> _COperationDataContainer;
//class _COperationDataContainer : public _CHashable, public _CInUseCheckpoint
//{
//public:
//	virtual ~_COperationDataContainer()
//	{
//	}
//	virtual _CSExpandedVertices* GetVertexResult()=0;
////	virtual double GetNumericalResult()=0;
//	bool operator==(const _COperationDataContainer& Right) {return IsEqualTo(Right);}
//	virtual _COperationDataContainer* AllocateCopy()=0;
//protected:
//	virtual bool IsEqualTo(const _COperationDataContainer& Right)=0;
//};



class _CRealGraphDevelopement : public _COperationDataContainer
{
public:
	//_CRealGraphDevelopement(unsigned InCofId,const string& SocketName,const _CSExpandedDescendends& Leaves,_CModelVertex* MVertex,
	//	_CSExpandedVertices Result=_CSExpandedVertices()):m_InCofId(InCofId),m_SocketName(SocketName),m_Leaves(Leaves),m_ModelVetex(MVertex),
	//	m_ResSimpleVertex(Result) {}
	_CRealGraphDevelopement(unsigned InCofId,const string& SocketName,const _CSExpandedDescendends& Leaves,_CModelVertex* MVertex,
		_CSExpandedVertices* pResult=NULL, short ResTopSgn=1):m_InCofId(InCofId),m_SocketName(SocketName),m_Leaves(Leaves),m_ModelVetex(MVertex),
		m_pResSimpleVertex(pResult), m_ResTopSgn(ResTopSgn) {}
	//_CRealGraphDevelopement(const _CRealGraphDevelopement& Source):m_InCofId(Source.m_InCofId),
	//	m_SocketName(Source.m_SocketName),m_Leaves(Source.m_Leaves),m_ModelVetex(Source.m_ModelVetex),
	//	m_ResSimpleVertex(Source.m_ResSimpleVertex) 
	//{
	//}
	_CRealGraphDevelopement(const _CRealGraphDevelopement& Source):m_InCofId(Source.m_InCofId),
		m_SocketName(Source.m_SocketName),m_Leaves(Source.m_Leaves),m_ModelVetex(Source.m_ModelVetex),
		m_pResSimpleVertex(Source.m_pResSimpleVertex), m_ResTopSgn(Source.m_ResTopSgn) 
	{
	}
	virtual ~_CRealGraphDevelopement()
	{
	}
	//_CSExpandedVertices& GetVertexResult() {return m_ResSimpleVertex;}
	_CSExpandedVertices* GetVertexResult() {return m_pResSimpleVertex;}
	short& GetTopSgnResult() {return m_ResTopSgn; }
	//double GetNumericalResult();
	void SetResult(_CSExpandedVertices* pVertices, short ResTopSgn) {m_pResSimpleVertex=pVertices; m_ResTopSgn=ResTopSgn;}
	virtual _COperationDataContainer* AllocateCopy() {return new _CRealGraphDevelopement(*this);}
protected:
	bool IsEqualTo(const _COperationDataContainer& Right);
	long DetermineHashKey(long Core=HASH_CORE);
	unsigned m_InCofId;
	_CSExpandedDescendends m_Leaves;
	_CModelVertex* m_ModelVetex;
	_CSExpandedVertices* m_pResSimpleVertex;
	short m_ResTopSgn;
	string m_SocketName;
	const string* m_pContext = nullptr; 
};

typedef _CAbstractOperationCache<_CSExpandedVertices> _COperationCache;
//class _COperationCache : public _CCacheTmpl<_COperationDataContainer>
//{
//public:
//	~_COperationCache()
//	{
//	}
//	void CacheTheResult(_COperationDataContainer* pContainer);
//	_COperationDataContainer* GetExistingResult(_COperationDataContainer* pContainer) {return FindExisting(pContainer);}
//	_CSExpandedVertices* FindResult(_COperationDataContainer* pContainer);
//	_CSExpandedVertices* FindOrRegisterResult(_COperationDataContainer* pContainer);
//	void RegisterResult(_COperationDataContainer* pContainer);
//};
