#pragma once
#include "stdafx.h"
#include <list>
#include <set>
#include <algorithm>
#include <fstream>
#include "ComponentPath.h"
#include "PrjException.h"
#include "AbstractResTables.h"
#include "MVID.h"
#include "norm_float.h"

#include <unordered_map>
//#include "Vertex.h"
//#include "InUseCheckpoint.h"
class _CSubCircuitSocket;
class _CComponent;

class _CSimpleVertexContainer;
//class _CSimpleVertexMap;
//class _CSimpleVerticesMaps;
class _CCircuit;
class _CMainCircuit;
class _CVertex;
class _CSExpandedVertices;
class _CSimpleVerticesMapsSet;
//class _CConstComponentPath;
class _CSubCircuitVerticesMap;
class _CSimilarVertMaps;
class _CSimilarVertMapsStack;

#define ANY_VERTEX (const _CSimpleVertexContainer*)(-1)

bool operator==(_CSubCircuitVerticesMap& Left, _CSubCircuitVerticesMap& Right);

template<class InputArguments>
size_t GetInputOperatorHash(size_t& Core, InputArguments Arg);

class _CSuplementParameters
{
public:
	~_CSuplementParameters() {}
	_CSuplementParameters() {}
	_CSuplementParameters(const _CSuplementParameters& Soure) {}
	//virtual long long HashKey() const {return 0;}
	//virtual bool Equals(const _CSuplementParameters* pRight) const {
	//	return typeid(*pRight)==typeid(_CSuplementParameters);
	//}
	//virtual const _CSuplementParameters* CreateMyCopy() const {return NULL;}
};
//_CAbstractOperator
//template<class OutputType, class InputArguments=const _CSimpleVertexContainer*, class SuplementParams=_CSuplementParameters>
//typedef enum {NOT_EQUAL,EXACT_EQUAL,DIFFERS_WITH_MAP} COMP_RES;

template<class OutputType, typename OutputType* pEmptyResult, class InputArguments=const _CSimpleVertexContainer*>
class _CAbstractOperator
{
public:
	typedef typename OutputType MY_TYPE;
	virtual const _CSuplementParameters* GetSuplementParameters() const {return NULL;}
	virtual void PostOperateSubCircuitResult(size_t EntryId, size_t PosId, typename OutputType& NewRes) {}
	virtual void PostOperateTerminals(long long VertId, typename OutputType& NewRes) {}
	virtual bool SkipMapDifference() {return false;}
	//class _CResContainer;
	//class _CExactOutContainer;
	//****************************_COutContainer
	//class _COutContainer
	//{
	//protected:
	//	typename OutputType m_Result;
	//public:
	//	_COutContainer(typename OutputType Result=*pEmptyResult):
	//		m_Result(Result) {}
	//	_COutContainer(const _COutContainer& Source):
	//		m_Result(Source.m_Result) {}
	//	typename OutputType GetResult() const {return m_Result;}
	//};
	//****************************_COutContainer
	//****************************_CBaseContainer
	//class _CBaseContainer
	//{
	//public:
	//	_CBaseContainer(InputArguments Vertices,
	//					const string& Context,
	//					_CConstComponentPath& FullPath):
	//			m_Vertices(Vertices), 
	//			m_Context(Context), 
	//			m_FullPath(FullPath),
	//			m_BaseHash(-1) {}
	//	_CBaseContainer(const _CBaseContainer& Source):
	//				m_Vertices(Source.m_Vertices),
	//				m_Context(Source.m_Context),
	//				m_FullPath(Source.m_FullPath),
	//				m_BaseHash(Source.m_BaseHash){}
	//	long long GetBaseHash() 
	//	{
	//		if(m_BaseHash==-1)
	//		{
	//			m_BaseHash=Hash_value(HASH_CORE,m_Context);
	//			m_BaseHash*=HASH_FACTOR;
	//			m_BaseHash=(unsigned long long)GetInputOperatorHash(m_BaseHash,m_Vertices);
	//			//m_BaseHash*=HASH_FACTOR;
	//			m_BaseHash=m_FullPath.ClearComponent().GetHash(m_BaseHash);
	//		}
	//		return m_BaseHash;
	//	}
	//	bool operator==(const _CBaseContainer& Right)
	//	{
	//		if(m_Context!=Right.m_Context)
	//			return false;
	//		if(!(m_Vertices==Right.m_Vertices || *m_Vertices==*Right.m_Vertices))
	//			return false;
	//		if(!(m_FullPath==Right.m_FullPath))
	//			return false;
	//		return true;
	//	}
	//	_CConstComponentPath& GetFullPath()
	//	{
	//		_CComponent* pComponent=m_FullPath.GetBaseCircuit()->GlobalVertexFromId(m_Vertices->GetLocalVertex())->GetComponent();
	//		m_FullPath.SetComponent(pComponent);
	//		return m_FullPath;
	//	}
	//	const string& GetContext() {return m_Context;}
	//protected:
	//	InputArguments m_Vertices;
	//	const string& m_Context;
	//	_CConstComponentPath m_FullPath;
	//	long long m_BaseHash;
	//};
	//****************************_CBaseContainer
	//****************************_CExactContainer
	//class _CExactContainer
	//{
	//public:
	//	_CExactContainer(_CSubCircuitVerticesMap* pVerticesMap,
	//					 const MVIDS& MVId,
	//					 unsigned AuxId=0):
	//			m_pVerticesMap(pVerticesMap), 
	//			m_MVId(MVId),
	//			m_AuxId(AuxId),
	//			m_ExactHash(-1) {}
	//	_CExactContainer(const _CExactContainer& Source):
	//			m_pVerticesMap(Source.m_pVerticesMap), 
	//			m_MVId(Source.m_MVId),
	//			m_AuxId(Source.m_AuxId),
	//			m_ExactHash(Source.m_ExactHash) {}
	//	long long GetExactHash(const string& Context)
	//	{
	//		if(m_ExactHash==-1)
	//		{
	//			m_ExactHash=HASH_CORE;
	//			//*HASH_FACTOR;
	//			m_ExactHash=m_pVerticesMap==NULL?0:m_pVerticesMap->GetHash(m_ExactHash,&Context);
	//			m_ExactHash*=HASH_FACTOR;
	//			m_ExactHash^=m_AuxId;
	//			//m_ExactHash*=HASH_FACTOR;
	//			m_ExactHash=m_MVId.GetHash(m_ExactHash);
	//		}
	//		return m_ExactHash;
	//	}
	//	const _CSubCircuitVerticesMap* GetVerticesMap() {return m_pVerticesMap;}
	//	unsigned GetAuxId() const {return m_AuxId;}
	//	const MVIDS& GetMVId() const {return m_MVId;}
	//	bool operator==(const _CExactContainer& Right)
	//	{
	//		return (m_pVerticesMap==Right.m_pVerticesMap || *m_pVerticesMap==*Right.m_pVerticesMap) &&
	//			m_AuxId==Right.m_AuxId && m_MVId==Right.m_MVId;
	//	}
	//protected:
	//	_CSubCircuitVerticesMap* m_pVerticesMap;
	//	unsigned m_AuxId;
	//	MVIDS m_MVId;
	//	_CExactOutContainer* m_PrevExactOutContainer;
	//	long long m_ExactHash;
	//};
	//****************************_CExactContainer
	//****************************_CPostAnalyzeContainer
	//
	//class _CPostAnalyzeContainer
	//{
	//public:
	//	_CPostAnalyzeContainer(_CExactOutContainer* pVert0ExactOutContainer=NULL,
	//						   _CExactOutContainer* pVert1ExactOutContainer=NULL): 
	//			m_pVert0ExactOutContainer(pVert0ExactOutContainer),
	//			m_pVert1ExactOutContainer(pVert1ExactOutContainer),
	//			m_PostAnalyzeHash(-1) {}
	//	_CPostAnalyzeContainer(const _CPostAnalyzeContainer& Source): 
	//			m_pVert0ExactOutContainer(Source.m_pVert0ExactOutContainer),
	//			m_pVert1ExactOutContainer(Source.m_pVert1ExactOutContainer),
	//			m_PostAnalyzeHash(Source.m_PostAnalyzeHash) {}
	//	//_CPostAnalyzeContainer( typename OutputType Vert1Res=*pEmptyResult,
	//	//						const _CSimpleVertexContainer* p1NextRealVertex=NULL,
	//	//						MVIDS Vertex1MVId=MVIDS(),
	//	//						unsigned Succ1AuxId=0,
	//	//						typename OutputType Vert0Res=*pEmptyResult,
	//	//						const _CSimpleVertexContainer* p0NextRealVertex=NULL,
	//	//						MVIDS Vertex0MVId=MVIDS(),
	//	//						unsigned Succ0AuxId=0): 
	//	//		m_p1NextRealVertex(p1NextRealVertex),
	//	//		m_Vertex1MVId(Vertex1MVId),
	//	//		m_Succ1AuxId(Succ1AuxId),
	//	//		m_Vert1Res(Vert1Res),
	//	//		m_p0NextRealVertex(p0NextRealVertex),
	//	//		m_Vertex0MVId(Vertex0MVId),
	//	//		m_Succ0AuxId(Succ0AuxId),
	//	//		m_Vert0Res(Vert0Res),
	//	//		m_PostAnalyzeHash(-1) {}
	//	//_CPostAnalyzeContainer(const _CPostAnalyzeContainer& Source): 
	//	//		m_p1NextRealVertex(Source.m_p1NextRealVertex),
	//	//		m_Vertex1MVId(Source.m_Vertex1MVId),
	//	//		m_Succ1AuxId(Source.m_Succ1AuxId),
	//	//		m_Vert1Res(Source.m_Vert1Res),
	//	//		m_p0NextRealVertex(Source.m_p0NextRealVertex),
	//	//		m_Vertex0MVId(Source.m_Vertex0MVId),
	//	//		m_Succ0AuxId(Source.m_Succ0AuxId),
	//	//		m_Vert0Res(Source.m_Vert0Res),
	//	//		m_PostAnalyzeHash(Source.m_PostAnalyzeHash) {}
	//	long long GetPostAnalyzeHash() 
	//	{
	//		if(m_PostAnalyzeHash==-1)
	//		{
	//			m_PostAnalyzeHash=HASH_CORE;
	//			m_PostAnalyzeHash^=(long long)m_pVert0ExactOutContainer;
	//			m_PostAnalyzeHash*=HASH_FACTOR;
	//			m_PostAnalyzeHash^=(long long)m_pVert1ExactOutContainer;
	//		//	if(m_p1NextRealVertex!=NULL)
	//		//		if(m_p1NextRealVertex!=ANY_VERTEX)
	//		//		{
	//		//			m_PostAnalyzeHash^=GetTypedHash(*m_p1NextRealVertex);
	//		//			m_PostAnalyzeHash*=HASH_FACTOR;
	//		//			m_PostAnalyzeHash^=m_Succ1AuxId;
	//		//			m_PostAnalyzeHash*=HASH_FACTOR;
	//		//			//m_PostAnalyzeHash^=m_Vertex1MVId.GetHashLess();
	//		//			m_PostAnalyzeHash^=m_Vertex1MVId.GetHash();
	//		//			m_PostAnalyzeHash*=HASH_FACTOR;
	//		//			m_PostAnalyzeHash^=GetTypedHash(m_Vert1Res);

	//		//		}
	//		//		else
	//		//			m_PostAnalyzeHash^=(long long)ANY_VERTEX;
	//		//	m_PostAnalyzeHash*=HASH_FACTOR;
	//		//	if(m_p0NextRealVertex!=NULL)
	//		//		if(m_p0NextRealVertex!=ANY_VERTEX)
	//		//		{
	//		//			m_PostAnalyzeHash^=GetTypedHash(*m_p0NextRealVertex);
	//		//			m_PostAnalyzeHash*=HASH_FACTOR;
	//		//			m_PostAnalyzeHash^=m_Succ0AuxId;
	//		//			m_PostAnalyzeHash*=HASH_FACTOR;
	//		//			//m_PostAnalyzeHash^=m_Vertex0MVId.GetHashLess();
	//		//			m_PostAnalyzeHash^=m_Vertex0MVId.GetHash();
	//		//			m_PostAnalyzeHash*=HASH_FACTOR;
	//		//			m_PostAnalyzeHash^=GetTypedHash(m_Vert0Res);
	//		//		}
	//		//		else
	//		//			m_PostAnalyzeHash^=(long long)ANY_VERTEX;
	//		}
	//		return m_PostAnalyzeHash;
	//	}
	//	unsigned MaxAuxId() 
	//	{
	//		unsigned A0=m_pVert0ExactOutContainer==NULL?0:m_pVert0ExactOutContainer->GetAuxId();
	//		unsigned A1=m_pVert1ExactOutContainer==NULL?0:m_pVert1ExactOutContainer->GetAuxId();
	//		return max(A0,A1);
	//	}
	//	//const _CSimpleVertexContainer* Get1NextRealVertex() const {return m_p1NextRealVertex;}
	//	//const MVIDS& GetVertex1MVId() const {return  m_Vertex1MVId;}
	//	//unsigned GetSucc1AuxId() const {return m_Succ1AuxId;}
	//	//const _CSimpleVertexContainer* Get0NextRealVertex() const {return m_p0NextRealVertex;}
	//	//const MVIDS& GetVertex0MVId() const {return m_Vertex0MVId;}
	//	//unsigned GetSucc0AuxId() const {return m_Succ0AuxId;}
	//	bool operator==(const _CPostAnalyzeContainer& Right)
	//	{
	//		return m_pVert0ExactOutContainer==Right.m_pVert0ExactOutContainer 
	//			&& m_pVert1ExactOutContainer==Right.m_pVert1ExactOutContainer;
	//		//return ((m_p1NextRealVertex==ANY_VERTEX || 
	//		//	((m_p1NextRealVertex==Right.m_p1NextRealVertex || *m_p1NextRealVertex==*Right.m_p1NextRealVertex)
	//		//	//&& m_Succ1AuxId==Right.m_Succ1AuxId) && m_Vertex1MVId.AlmostEq(Right.m_Vertex1MVId)) &&
	//		//	&& m_Vert1Res==Right.m_Vert1Res && m_Succ1AuxId==Right.m_Succ1AuxId) && m_Vertex1MVId==Right.m_Vertex1MVId)) &&
	//		//	(m_p0NextRealVertex==ANY_VERTEX || 
	//		//	((m_p0NextRealVertex==Right.m_p0NextRealVertex || *m_p0NextRealVertex==*Right.m_p0NextRealVertex)
	//		//	//&& m_Succ0AuxId==Right.m_Succ0AuxId && m_Vertex0MVId.AlmostEq(Right.m_Vertex0MVId))));
	//		//	&& m_Vert0Res==Right.m_Vert0Res && m_Succ0AuxId==Right.m_Succ0AuxId && m_Vertex0MVId==Right.m_Vertex0MVId));
	//	}
	//	_CExactOutContainer* GetVert0ExactOutContainer() const {return m_pVert0ExactOutContainer;}
	//	_CExactOutContainer* GetVert1ExactOutContainer() const {return m_pVert1ExactOutContainer;}
	//protected:
	//	//typename OutputType m_Vert1Res;
	//	//const _CSimpleVertexContainer* m_p1NextRealVertex;
	//	//MVIDS m_Vertex1MVId;
	//	//unsigned m_Succ1AuxId;
	//	//typename OutputType m_Vert0Res;
	//	//const _CSimpleVertexContainer* m_p0NextRealVertex;
	//	//MVIDS m_Vertex0MVId;
	//	//unsigned m_Succ0AuxId;
	//	_CExactOutContainer* m_pVert0ExactOutContainer;
	//	_CExactOutContainer* m_pVert1ExactOutContainer;
	//	long long m_PostAnalyzeHash;
	//};
	//****************************_CPostAnalyzeContainer
	//****************************_CExactOutContainer
	//class _CExactOutContainer : public _COutContainer, public _CExactContainer
	//{
	//public:
	//	_CExactOutContainer(const _CExactOutContainer& Source): 
	//	  _COutContainer(Source), _CExactContainer(Source),m_pPostAnalyzeContainer(NULL) {} 
	//	_CExactOutContainer(const _CResContainer& Source): 
	//	  _COutContainer(Source), _CExactContainer(Source),m_pPostAnalyzeContainer(NULL) {} 
	//	_CExactOutContainer(const _COutContainer& OutSource,const _CExactContainer& BaseSource,
	//		const _CPostAnalyzeContainer& PostAnalyzeContainer): 
	//	  _COutContainer(OutSource), _CExactContainer(BaseSource),
	//		  m_pPostAnalyzeContainer(&PostAnalyzeContainer) {} 
	//	_CExactOutContainer(const _CExactContainer& BaseSource): 
	//	  _CExactContainer(BaseSource),m_pPostAnalyzeContainer(NULL) {}
	//	void SetPostAnalyzeContainer(const _CPostAnalyzeContainer& PostAnalyzeContainer) 
	//	{
	//		m_pPostAnalyzeContainer=&PostAnalyzeContainer;
	//	}
	//	unsigned MaxAuxId() const {return max(m_Succ1AuxId,m_Succ0AuxId);}

	//	//const _CSimpleVertexContainer* Get1NextRealVertex() const
	//	//{
	//	//	return m_pPostAnalyzeContainer->Get1NextRealVertex();
	//	//}
	//	//const MVIDS& GetVertex1MVId() const
	//	//{
	//	//	return  m_pPostAnalyzeContainer->GetVertex1MVId();
	//	//}
	//	//unsigned GetSucc1AuxId() const
	//	//{
	//	//	return m_pPostAnalyzeContainer->GetSucc1AuxId();
	//	//}
	//	//const _CSimpleVertexContainer* Get0NextRealVertex() const
	//	//{
	//	//	return m_pPostAnalyzeContainer->Get0NextRealVertex();
	//	//}
	//	//const MVIDS& GetVertex0MVId() const
	//	//{
	//	//	return m_pPostAnalyzeContainer->GetVertex0MVId();
	//	//}
	//	//unsigned GetSucc0AuxId() const
	//	//{
	//	//	return m_pPostAnalyzeContainer->GetSucc0AuxId();
	//	//}
	//	//bool operator==(const _CExactOutContainer& Right)
	//	//{
	//	//	return *(dynamic_cast<_CExactContainer*>(this))==(dynamic_cast<_CExactContainer>(Right));
	//	//}
	//	_CExactOutContainer* GetVert0ExactOutContainer() {return m_pPostAnalyzeContainer->GetVert0ExactOutContainer();}
	//	_CExactOutContainer* GetVert1ExactOutContainer() {return m_pPostAnalyzeContainer->GetVert1ExactOutContainer();}
	//protected:
	//	const _CPostAnalyzeContainer* m_pPostAnalyzeContainer;
	//};
	//****************************_CExactOutContainer
	//****************************_CPostAnalyzeContainer
//	class _CPostOutContainer : public /*_COutContainer,*/ _CPostAnalyzeContainer
//	{
//	public:
//		_CPostOutContainer(const _CPostOutContainer& Source): 
//		  /*_COutContainer(Source),*/ _CPostAnalyzeContainer(Source),m_pExactOutContainer(Source.m_pExactOutContainer) {} 
//		_CPostOutContainer(_CExactOutContainer& ExactOutSource,const _CPostAnalyzeContainer& BaseSource): 
//		  /*_COutContainer(OutSource),*/ _CPostAnalyzeContainer(BaseSource),m_pExactOutContainer(&ExactOutSource) {} 
//		_CPostOutContainer(const _CPostAnalyzeContainer& BaseSource): 
//		  _CPostAnalyzeContainer(BaseSource) {}
//		void SetExactOutContainer(const _CExactOutContainer* ExactOutContainer) {m_pExactOutContainer=ExactOutContainer;}
//		typename OutputType GetResult() {return m_pExactOutContainer->GetResult();}
//		unsigned GetAuxId() {return m_pExactOutContainer->GetAuxId();}
//		const MVIDS& GetMVId() {return m_pExactOutContainer->GetMVId();}
//		_CExactOutContainer* GetExactOutContainer() {return m_pExactOutContainer;}
//
//	protected:
//		_CExactOutContainer* m_pExactOutContainer;
////		const _COutContainer* m_pOutContainer;
//	};
	//****************************_CExactOutContainer
	//class _CResContainer;
	//****************************_CAuxBasePlatorm
	//class _CAuxBasePlatorm : public _CBaseContainer
	//{
	//public:
	//	_CAuxBasePlatorm():m_PostIdFree(0) {}
	//	_CAuxBasePlatorm(const _CBaseContainer& BaseContainer):_CBaseContainer(BaseContainer),
	//		m_PostIdFree(0) {}
	//	_CAuxBasePlatorm(const _CAuxBasePlatorm& Source):_CBaseContainer(Source),
	//		m_PostIdFree(Source.m_PostIdFree),m_ExactMap(Source.m_ExactMap), m_PostMap(Source.m_PostMap) {}
	//	_CExactOutContainer* Register(_CResContainer& ResContainer)
	//	{
	//		long long ExactHash=ResContainer.GetExactHash(ResContainer.GetContext());
	//		long long PostAnalyzeHash=ResContainer.GetPostAnalyzeHash();
	//		pair<EXACT_MAP::iterator,bool> res=m_ExactMap.insert(EXACT_MAP::value_type(ExactHash,ResContainer));
	//		_CExactOutContainer& ExactContainer=res.first->second;
	//		_CPostOutContainer PostOutContainer(ExactContainer,ResContainer);
	//		pair<POST_MAP::iterator,bool> res2=m_PostMap.insert(POST_MAP::value_type(PostAnalyzeHash,PostOutContainer));
	//		ExactContainer.SetPostAnalyzeContainer(res2.first->second);
	//		m_PostIdFree=max(m_PostIdFree,ResContainer.MaxAuxId());
	//		return &ExactContainer;
	//	}
	//	_CExactOutContainer* GetExactOutContainer(_CExactContainer& ExactContainer,const string& Context)
	//	{
	//		long long ExactHash=ExactContainer.GetExactHash(Context);
	//		pair<EXACT_MAP::iterator,EXACT_MAP::iterator> res=m_ExactMap.equal_range(ExactHash);
	//		for(EXACT_MAP::iterator it=res.first;it!=res.second;it++)
	//		{
	//			_CExactOutContainer& ExactRes=it->second;
	//			if(ExactRes==ExactContainer)
	//				return &ExactRes;
	//		}
	//		return NULL;
	//	}
	//	//bool FitTheResult(unsigned& AuxId,
	//	//				  typename OutputType& Result,
	//	//				  MVIDS& CurrentMVId,
	//	//				  typename OutputType Vert1Res,
	//	//				  const _CSimpleVertexContainer* p1NextVertex,
	//	//				  const MVIDS& Vertex1MVId,
	//	//				  unsigned Succ1AuxId,
	//	//				  typename OutputType Vert0Res,
	//	//				  const _CSimpleVertexContainer* p0NextVertex,
	//	//				  const MVIDS& Vertex0MVId,
	//	//				  unsigned Succ0AuxId)
	//	//bool FitTheResult(unsigned& AuxId,
	//	//				  typename OutputType& Result,
	//	//				  MVIDS& CurrentMVId,
	//	//				  _CExactOutContainer*& pCurrentExactOutContainer,
	//	//				  _CExactOutContainer* pVert1ExactOutContainer,
	//	//				  _CExactOutContainer* pVert0ExactOutContainer)
	//	//{
	//	//	//_CPostAnalyzeContainer PostAnalyzeContainer(
	//	//	//	Vert1Res,
	//	//	//	p1NextVertex,
	//	//	//	Vertex1MVId,
	//	//	//	Succ1AuxId,
	//	//	//	Vert0Res,
	//	//	//	p0NextVertex,
	//	//	//	Vertex0MVId,
	//	//	//	Succ0AuxId);
	//	//	_CPostAnalyzeContainer PostAnalyzeContainer(
	//	//		pVert0ExactOutContainer,
	//	//		pVert1ExactOutContainer);
	//	//	long long PostAnalyzeHash=PostAnalyzeContainer.GetPostAnalyzeHash();
	//	//	pair<POST_MAP::iterator,POST_MAP::iterator> res=m_PostMap.equal_range(PostAnalyzeHash);
	//	//	for(POST_MAP::iterator it=res.first;it!=res.second;it++)
	//	//	{
	//	//		_CPostOutContainer& PostOutContainer=it->second;
	//	//		if(!(PostOutContainer==PostAnalyzeContainer))
	//	//			continue;
	//	//		Result=PostOutContainer.GetResult();
	//	//		AuxId=PostOutContainer.GetAuxId();
	//	//		pCurrentExactOutContainer=PostOutContainer.GetExactOutContainer();
	//	//		//CurrentMVId=PostOutContainer.GetMVId();
	//	//		return true;
	//	//	}
	//	//	AuxId=m_PostIdFree+1;
	//	//	return false;
	//	//}
	//protected:
	//	typedef unordered_map<long long, _CExactOutContainer> EXACT_MAP;
	//	typedef unordered_map<long long, _CPostOutContainer> POST_MAP;
	//	EXACT_MAP m_ExactMap;
	//	POST_MAP m_PostMap;
	//	unsigned m_PostIdFree;
	//	//_CBaseContainer m_BaseData;

	//};
	//****************************_CAuxBasePlatorm
	//****************************_CNResContainer
	class _CNResContainer
	{
	public:
		_CNResContainer(const InputArguments Vertices, 
						const string& Context, 
						//_CConstComponentPath& FullPath, 
						//_CSubCircuitVerticesMap* pVerticesMap,
						long sPower,
						const MVIDS& MVId,
						const typename OutputType& Result=*pEmptyResult) : 
		m_Vertices(Vertices), m_Context(Context), /*m_FullPath(FullPath),*/ /*m_pVerticesMap(pVerticesMap),*/
			m_SPower(sPower), m_MVId(MVId), m_Result(Result)
		{}
		_CNResContainer(const _CNResContainer& Source):
		m_Vertices(Source.m_Vertices), m_Context(Source.m_Context),
			//m_pVerticesMap(Source.m_pVerticesMap), /*m_FullPath(Source.m_FullPath),*/
			m_SPower(Source.m_SPower), m_MVId(Source.m_MVId), m_Result(Source.m_Result) {}
		virtual size_t GetHash(size_t& Core)
		{
			Core=Hash_value(Core,m_Context);
			Core*=HASH_FACTOR;
			Core=GetInputOperatorHash(Core,m_Vertices);
			Core*=HASH_FACTOR;
			Core^=(size_t)m_SPower;
			Core*=HASH_FACTOR;
			Core=m_MVId.GetHash(Core);
			return Core;
		}
		bool operator==(const _CNResContainer& Right) const
		{
			return m_Context==Right.m_Context && (m_Vertices==Right.m_Vertices || *m_Vertices==*Right.m_Vertices) &&
				m_SPower==Right.m_SPower && m_MVId==Right.m_MVId;
		}
		typename OutputType GetResult() {return m_Result;}
		const MVIDS& GetMVId() const {return m_MVId;}
		void ReSetsPower() {}
	protected:
		InputArguments m_Vertices;
		const string& m_Context;
		long m_SPower;
		MVIDS m_MVId;
		typename OutputType m_Result;
	};
	//****************************_CNResContainer
	//****************************_CResContainer
	//class _CResContainer : public _COutContainer, 
	//	public _CBaseContainer, public _CExactContainer, public _CPostAnalyzeContainer
	//{
	//public:
	//	_CResContainer(const _CResContainer& Source):
	//		_CBaseContainer(Source), 
	//		_CExactContainer(Source),
	//		_CPostAnalyzeContainer(Source),
	//		_COutContainer(Source)
	//		{}
	//	_CResContainer(const InputArguments Vertices, 
	//					const typename OutputType& Result, 
	//					const string& Context, 
	//					_CConstComponentPath& FullPath, 
	//					_CSubCircuitVerticesMap* pVerticesMap,
	//					const MVIDS& MVId,
	//					unsigned AuxId,
	//					_CExactOutContainer* pVert0ExactOutContainer,
	//					_CExactOutContainer* pVert1ExactOutContainer
	//					//typename OutputType Vert1Res,
	//					//const MVIDS& Vert1MVId,
	//					//const _CSimpleVertexContainer* p1NextVertex,
	//					//unsigned Succ1AuxId,
	//					//typename OutputType Vert0Res,
	//					//const MVIDS& Vert0MVId,
	//					//const _CSimpleVertexContainer* p0NextVertex,
	//					//unsigned Succ0AuxId
	//					):
	//		_CBaseContainer(Vertices,Context,FullPath), 
	//		_CExactContainer(pVerticesMap,MVId,AuxId),
	//		//_CPostAnalyzeContainer(Vert1Res,p1NextVertex,Vert1MVId,Succ1AuxId,Vert0Res,p0NextVertex,Vert0MVId,Succ0AuxId),
	//		_CPostAnalyzeContainer(pVert0ExactOutContainer,pVert1ExactOutContainer),
	//		_COutContainer(Result)
	//		{}
	//	_CResContainer(const InputArguments Vertices, 
	//				   const string& Context, 
	//				   _CConstComponentPath& FullPath,
	//				   _CSubCircuitVerticesMap* pVerticesMap, 
	//				   const MVIDS& MVId,
	//				   unsigned AuxId=0):
	//		_CBaseContainer(Vertices,Context,FullPath), 
	//		_CExactContainer(pVerticesMap,MVId,AuxId),
	//		_COutContainer(*pEmptyResult)
	//	{}
	//protected:
	//};
	//****************************_CNResContainer
	//***************************_CNCache
	class _CResCache
	{
	public:
		~_CResCache()
		{
		}
		void Register(_CNResContainer& ResContainer)
		{
			size_t Hash=HASH_CORE;
			Hash=ResContainer.GetHash(Hash);
			m_Map2.insert(MAP2::value_type(Hash,ResContainer));
		}
		bool GetExisted(_CNResContainer& ResContainer,typename OutputType& Result)
		{
			size_t Hash=HASH_CORE;
			Hash=ResContainer.GetHash(Hash);
			pair<MAP2::iterator,MAP2::iterator> res=m_Map2.equal_range(Hash);
			for(MAP2::iterator it=res.first;it!=res.second;it++)
			{
				_CNResContainer& OutContainer=it->second;
				if(OutContainer==ResContainer)
				{
					Result=OutContainer.GetResult();
					return true;
				}
			}
			return false;
		}
		//_CExactOutContainer* Register(_CResContainer& ResContainer,_CAuxBasePlatorm* pPrevBasePlatform)
		//{
		//	_CAuxBasePlatorm* pBasePlatform=pPrevBasePlatform==NULL?
		//		GetBasePlatform(ResContainer,true):pPrevBasePlatform;
		//	return pBasePlatform->Register(ResContainer);
		//}
		void clear() {m_Map2.clear();}
		void ClearLower(_CConstComponentPath& CPath)
		{
			MAP2::iterator it=m_Map2.begin(), _end=m_Map2.end();	
			for(;it!=_end;)
			{
				//MVIDS MVId;
				//_CBaseContainer& BaseCont=it->second;
				_CNResContainer& ResCont=it->second;
				//if(CPath.Comp(MVId)>=0)
				if(CPath.Comp(ResCont.GetMVId())>=0)
					it=m_Map2.erase(it);
				else
					it++;
			}
		}
		//COMP_RES GetExisted(_CResContainer& Cont, 
		//	                typename OutputType& Result,
		//					unsigned& AuxId,
		//					_CExactOutContainer** ppPrevExactOutContainer,
		//					_CExactOutContainer** ppVert0ExactOutContainer,
		//					_CExactOutContainer** ppVert1ExactOutContainer,
		//					_CAuxBasePlatorm** ppAuxBasePlatform 
		//					//const _CSimpleVertexContainer** pp1PrevVertex,
		//					//unsigned* pSucc1AuxId,
		//					//MVIDS* pVert1MVId,
		//					//const _CSimpleVertexContainer** pp0PrevVertex,
		//					//unsigned* pSucc0AuxId,
		//					//MVIDS* pVert0MVId,
		//					//MVIDS* pPrevMVId
		//					)
		//{
		//	_CAuxBasePlatorm* pBasePlatform=GetBasePlatform(Cont,false);
		//	if(pBasePlatform==NULL)
		//		return NOT_EQUAL;
		//	_CExactOutContainer* pExactOutContainer=
		//		pBasePlatform->GetExactOutContainer(Cont,Cont.GetContext());
		//	if(pExactOutContainer==NULL)
		//	{
		//		if(ppAuxBasePlatform!=NULL)
		//			*ppAuxBasePlatform=pBasePlatform;
		//		return DIFFERS_WITH_MAP;
		//	}
		//	Result=pExactOutContainer->GetResult();
		//	AuxId=pExactOutContainer->GetAuxId();
		//	if(ppPrevExactOutContainer!=NULL)
		//		*ppPrevExactOutContainer=pExactOutContainer;
		//	if(ppVert0ExactOutContainer!=NULL)
		//		*ppVert0ExactOutContainer=pExactOutContainer->GetVert0ExactOutContainer();
		//	if(ppVert1ExactOutContainer!=NULL)
		//		*ppVert1ExactOutContainer=pExactOutContainer->GetVert1ExactOutContainer();
		//	//if(pp1PrevVertex!=NULL)
		//	//	*pp1PrevVertex=pExactOutContainer->Get1NextRealVertex();
		//	//if(pSucc1AuxId!=NULL)
		//	//	*pSucc1AuxId=pExactOutContainer->GetSucc1AuxId();
		//	//if(pVert1MVId!=NULL)
		//	//	*pVert1MVId=pExactOutContainer->GetVertex1MVId();
		//	//if(pp0PrevVertex!=NULL)
		//	//	*pp0PrevVertex=pExactOutContainer->Get0NextRealVertex();
		//	//if(pSucc0AuxId!=NULL)
		//	//	*pSucc0AuxId=pExactOutContainer->GetSucc0AuxId();
		//	//if(pVert0MVId!=NULL)
		//	//	*pVert0MVId=pExactOutContainer->GetVertex0MVId();
		//	return EXACT_EQUAL;
		//}
	protected:
		//typedef unordered_map<long long, _CAuxBasePlatorm> MAP;
		typedef unordered_map<long long, _CNResContainer> MAP2;
		//_CAuxBasePlatorm* GetBasePlatform(_CBaseContainer& BaseContainer,bool ForceIfNotExist)
		//{
		//	long long BaseHash=BaseContainer.GetBaseHash();
		//	pair<MAP::iterator,MAP::iterator> range=m_Map.equal_range(BaseHash);
		//	for(MAP::iterator it=range.first;it!=range.second;it++)
		//	{
		//		_CAuxBasePlatorm& Platform=it->second;
		//		if(Platform==BaseContainer)
		//			return &Platform;
		//	}
		//	if(!ForceIfNotExist)
		//		return NULL;
		//	pair<MAP::iterator,bool> InsRes=m_Map.insert(MAP::value_type(BaseHash,BaseContainer));
		//	return &InsRes.first->second;
		//}
		//MAP m_Map;
		MAP2 m_Map2;
	};
	//***************************_CNCache
	//****************************_CResContainer
	//class _CResContainer
	//{
	//protected:
	//	InputArguments m_Vertices;
	//	typename OutputType m_Result;
	//	const string& m_Context;
	//	_CConstComponentPath m_FullPath;
	//	_CSubCircuitVerticesMap* m_pVerticesMap;
	//	MVIDS m_MVId;
	//	unsigned m_AuxId;
	//	const _CSimpleVertexContainer* m_p1NextRealVertex;
	//	MVIDS m_Vertex1MVId;
	//	unsigned m_Succ1AuxId;
	//	const _CSimpleVertexContainer* m_p0NextRealVertex;
	//	MVIDS m_Vertex0MVId;
	//	unsigned m_Succ0AuxId;

	//	long long m_ExactHash,
	//		m_MaybeHash,
	//		m_PostAnalyzeHash;
	//public:
	//	_CResContainer(const InputArguments Vertices, 
	//				   const string& Context, 
	//				   _CConstComponentPath& FullPath,
	//				   _CSubCircuitVerticesMap* pVertcesMap, 
	//				   const MVIDS& MVId,
	//				   unsigned AuxId=0):
	//						m_Vertices(Vertices), 
	//						m_Context(Context),
	//						m_FullPath(FullPath),
	//						m_pVerticesMap(pVertcesMap),
	//						m_MVId(MVId),
	//						m_AuxId(AuxId),
	//						m_p1NextRealVertex(NULL),
	//						m_p0NextRealVertex(NULL),
	//						//m_Vertex1MVId(0),
	//						//m_Vertex0MVId(0),
	//						m_Succ1AuxId(0),
	//						m_Succ0AuxId(0),
	//						m_ExactHash(-1),
	//						m_MaybeHash(-1),
	//						m_PostAnalyzeHash(-1)
	//						{}
	//	_CResContainer(const InputArguments Vertices, 
	//					const typename OutputType& Result, 
	//					const string& Context, 
	//					_CConstComponentPath& FullPath, 
	//					_CSubCircuitVerticesMap* pVertcesMap,
	//					const MVIDS& MVId,
	//					unsigned AuxId,
	//					//const string& sz1Path,
	//					const MVIDS& Vert1MVId,
	//					const _CSimpleVertexContainer* p1NextVertex,
	//					unsigned Succ1AuxId,
	//					//const string& sz0Path,
	//					const MVIDS& Vert0MVId,
	//					const _CSimpleVertexContainer* p0NextVertex,
	//					unsigned Succ0AuxId):
	//						    m_Vertices(Vertices),
	//							m_Result(Result), 
	//							m_Context(Context),
	//							m_FullPath(FullPath),
	//							m_pVerticesMap(pVertcesMap),
	//							m_MVId(MVId),
	//							m_AuxId(AuxId),
	//							//m_1Path(FullPath.GetPath()==sz1Path?"":sz1Path),
	//							//m_0Path(FullPath.GetPath()==sz0Path?"":sz0Path),
	//							m_p1NextRealVertex(p1NextVertex),
	//							m_p0NextRealVertex(p0NextVertex),
	//							m_Vertex1MVId(Vert1MVId),
	//							m_Vertex0MVId(Vert0MVId),
	//							m_Succ1AuxId(Succ1AuxId),
	//							m_Succ0AuxId(Succ0AuxId),
	//							m_ExactHash(-1),
	//							m_MaybeHash(-1),
	//							m_PostAnalyzeHash(-1)
	//							{}
	//	_CResContainer(const _CResContainer& Source):
	//							m_Vertices(Source.m_Vertices), 
	//							m_Context(Source.m_Context),
	//							m_Result(Source.m_Result),
	//							m_FullPath(Source.m_FullPath),
	//							m_pVerticesMap(Source.m_pVerticesMap),
	//							m_MVId(Source.m_MVId),
	//							m_AuxId(Source.m_AuxId),
	//							//m_1Path(Source.m_1Path),
	//							//m_0Path(Source.m_0Path),
	//							m_p1NextRealVertex(Source.m_p1NextRealVertex),
	//							m_p0NextRealVertex(Source.m_p0NextRealVertex),
	//							m_Vertex1MVId(Source.m_Vertex1MVId),
	//							m_Vertex0MVId(Source.m_Vertex0MVId),
	//							m_Succ1AuxId(Source.m_Succ1AuxId),
	//							m_Succ0AuxId(Source.m_Succ0AuxId),
	//							m_ExactHash(-1),
	//							m_MaybeHash(-1),
	//							m_PostAnalyzeHash(-1)
	//							{}

	//	~_CResContainer()
	//	{
	//	}
	//	long long GetExactHash()
	//	{
	//		if(m_ExactHash==-1)
	//		{
	//			m_ExactHash=GetMaybeHash();
	//			m_ExactHash*=HASH_FACTOR;
	//			m_ExactHash^=m_pVerticesMap==NULL?0:m_pVerticesMap->GetHash(&m_Context);
	//		}
	//		return m_ExactHash;
	//	}
	//	long long GetMaybeHash()
	//	{
	//		if(m_MaybeHash==-1)
	//		{
	//			m_MaybeHash=hash_value(m_Context);
	//			m_MaybeHash*=HASH_FACTOR;
	//			m_MaybeHash^=(unsigned long long)GetInputOperatorHash(m_Vertices);
	//			m_MaybeHash*=HASH_FACTOR;
	//			m_MaybeHash^=m_FullPath.ClearComponent().GetHash();
	//		}
	//		return m_MaybeHash;
	//	}
	//	long long GetPostAnalyzeHash()
	//	{
	//		if(m_PostAnalyzeHash==-1)
	//		{
	//			m_PostAnalyzeHash=GetMaybeHash();
	//			if(m_p1NextRealVertex!=NULL)
	//				if(m_p1NextRealVertex!=ANY_VERTEX)
	//				{
	//					m_PostAnalyzeHash^=GetTypedHash(*m_p1NextRealVertex);
	//					m_PostAnalyzeHash*=HASH_FACTOR;
	//					m_PostAnalyzeHash^=m_Succ1AuxId;
	//				}
	//				else
	//					m_PostAnalyzeHash^=(long long)ANY_VERTEX;
	//			m_PostAnalyzeHash*=HASH_FACTOR;
	//			if(m_p0NextRealVertex!=NULL)
	//				if(m_p0NextRealVertex!=ANY_VERTEX)
	//				{
	//					m_PostAnalyzeHash^=GetTypedHash(*m_p0NextRealVertex);
	//					m_PostAnalyzeHash*=HASH_FACTOR;
	//					m_PostAnalyzeHash^=m_Succ0AuxId;
	//				}
	//				else
	//					m_PostAnalyzeHash^=(long long)ANY_VERTEX;
	//		}
	//		return m_PostAnalyzeHash;
	//	}
	//	const _CSimpleVertexContainer* GetPrev1Vertex() {return m_p1NextRealVertex; }
	//	const _CSimpleVertexContainer* GetPrev0Vertex() {return m_p0NextRealVertex; }
	//	unsigned GetSucc1AuxId() {return m_Succ1AuxId;}
	//	unsigned GetSucc0AuxId() {return m_Succ0AuxId;}
	//	const MVIDS& GetCurrnetMVId() {return m_MVId;}
	//	const MVIDS& GetVertex1MVId() {return m_Vertex1MVId;}
	//	const MVIDS& GetVertex0MVId() {return m_Vertex0MVId;}
	//	//const string& GetVertex1Path() {return m_1Path;}
	//	//const string& GetVertex0Path() {return m_0Path;}
	//	_CSubCircuitVerticesMap* GetSubCircuitVerticesMap() {return m_pVerticesMap;}
	//	unsigned GetAuxId() {return m_AuxId;}
	//	void SetNextId(const _CResContainer Source) {m_AuxId=Source.m_AuxId+1;}
	//	long long HashKey() 
	//	{
	//		long long Key=hash_value(m_Context);
	//		Key*=HASH_FACTOR;
	//		Key^=(unsigned long long)GetInputOperatorHash(m_Vertices);
	//		//XOR(Key,GetInputOperatorHash(m_Vertices));
	//		Key*=HASH_FACTOR;
	//		//Key+=(long long)m_pSocket;
	//		Key^=m_FullPath.ClearComponent().GetHash();
	//		//Key*=HASH_FACTOR;
	//		//Key^=m_pVerticesMap==NULL?0:m_pVerticesMap->GetHash();
	//		return Key;
	//	}
	//	COMP_RES Comp(_CResContainer& Right)
	//	{
	//		if(&m_Context!=&Right.m_Context)
	//			return NOT_EQUAL;
	//		if((GetInputOperatorHash(m_Vertices)!=GetInputOperatorHash(Right.m_Vertices))
	//			|| /*(m_pSocket!=Right.m_pSocket)*/ !(m_FullPath.ClearComponent()==Right.m_FullPath.ClearComponent()))
	//			return NOT_EQUAL;
	//		//if(!(m_FullPath==Right.m_FullPath))
	//		//	return NOT_EQUAL;
	//		//if(m_pVerticesMap!=Right.m_pVerticesMap)
	//		//	return false;
	//		if(m_MVId==Right.m_MVId &&(m_pVerticesMap==Right.m_pVerticesMap || m_pVerticesMap->IsContextEql(*Right.m_pVerticesMap,&m_Context)))
	//			return EXACT_EQUAL;
	//		else
	//			return DIFFERS_WITH_MAP;
	//		//if(m_pVerticesMap==NULL || Right.m_pVerticesMap==NULL || 
	//		//	//!((*m_pVerticesMap)==(*Right.m_pVerticesMap))	)
	//		//	!m_pVerticesMap->IsContextEql(*Right.m_pVerticesMap,&m_Context) ||
	//		//	m_1Path!=Right.m_1Path & m_0Path!=Right.m_0Path)
	//		//	return DIFFERS_WITH_MAP;
	//		//return EXACT_EQUAL;
	//	}
	//	bool operator!=(const _CResContainer& Right)
	//	{
	//		return !(operator==(Right));
	//	}
	//	_CResContainer& operator=(const _CResContainer& Right)
	//	{
	//		m_Vertices=Right.m_Vertices; 
	//		m_Context=Right.m_Context;
	//		m_Result=Right.m_Result;
	//		m_FullPath=Right.m_FullPath;
	//		m_pVerticesMap=Right.m_pVerticesMap;
	//		m_MVId=Right.m_MVId;
	//		m_AuxId=Right.m_AuxId;
	//		m_p1NextRealVertex=Right.m_p1NextRealVertex;
	//		m_p0NextRealVertex=Right.m_p0NextRealVertex;
	//		m_Vertex1MVId=Right.m_Vertex1MVId;
	//		m_Vertex0MVId=Right.m_Vertex0MVId;
	//		m_Succ1AuxId=Right.m_Succ1AuxId;
	//		m_Succ0AuxId=Right.m_Succ0AuxId;
	//		return *this;
	//	}
	//	void SetResult(typename OutputType Res) {m_Result=Res;}
	//	typename OutputType GetResult() {return m_Result;}
	//	_CConstComponentPath& GetFullPath()
	//	{
	//		_CComponent* pComponent=m_FullPath.GetBaseCircuit()->GlobalVertexFromId(m_Vertices->GetLocalVertex())->GetComponent();
	//		m_FullPath.SetComponent(pComponent);
	//		return m_FullPath;
	//	}
	//	friend class _CResContList;
	//};
	//****************************_CResContainer
	//****************************_CResContList
//	class _CResContList : public hash_multimap <long long,_CResContainer*>
//	{
//	public:
//		_CResContList():m_MaxAuxId(0) {}
//		void push(_CResContainer* pResContainer)
//		{
//			m_MaxAuxId=max(m_MaxAuxId,pResContainer->GetAuxId());
//			long long Hash=PrepareHash(
//				pResContainer->GetResult(),
//				pResContainer->GetPrev1Vertex(),
//				pResContainer->GetVertex1MVId(),
//				pResContainer->GetSucc1AuxId(),
//				pResContainer->GetPrev0Vertex(),
//				pResContainer->GetVertex0MVId(),
//				pResContainer->GetSucc0AuxId());
//			insert(value_type(Hash,pResContainer));
//		}
//		bool FitTheResult(unsigned& AuxId,
//						  typename OutputType& Result,
//						  MVIDS& CurrentMVId,
//						  const _CSimpleVertexContainer* p1NextVertex,
//						  const MVIDS& Vertex1MVId,
//						  //const string& Vertex1Path,
//						  unsigned Succ1AuxId,
//						  const _CSimpleVertexContainer* p0NextVertex,
//						  const MVIDS& Vertex0MVId,
//						  //const string& Vertex0Path,
//						  unsigned Succ0AuxId)
//		{
//			long long Hash=PrepareHash(Result,p1NextVertex, Vertex1MVId, Succ1AuxId, 
//				p0NextVertex, Vertex0MVId,Succ0AuxId);
//			pair<iterator,iterator> Range=equal_range(Hash);
//			bool EqualsDescendentButNotAuxId=false;
//			for(iterator it=Range.first;it!=Range.second;it++)
//			{
//				_CResContainer* pCont=it->second;
//				const _CSimpleVertexContainer* p1ContVertex=pCont->GetPrev1Vertex();
//				//const string& ContVert1Path=pCont->GetVertex1Path();
//				const MVIDS& ContVert1MVId=pCont->GetVertex1MVId();
//				unsigned cSucc1AuxId=pCont->GetSucc1AuxId();
//				const _CSimpleVertexContainer* p0ContVertex=pCont->GetPrev0Vertex();
//				//const string& ContVert0Path=pCont->GetVertex0Path();
//				const MVIDS& ContVert0MVId=pCont->GetVertex0MVId();
//				unsigned cSucc0AuxId=pCont->GetSucc0AuxId();
//				if((p1ContVertex==ANY_VERTEX || ((p1NextVertex==p1ContVertex || *p1NextVertex==*p1ContVertex)
//					//&& Succ1AuxId==cSucc1AuxId) && Vertex1Path==ContVert1Path) &&
//					&& Succ1AuxId==cSucc1AuxId) /*&& Vertex1MVId==ContVert1MVId*/) &&
//					(p0ContVertex==ANY_VERTEX || ((p0NextVertex==p0ContVertex || *p0NextVertex==*p0ContVertex)
//					&& Succ0AuxId==cSucc0AuxId /*&& Vertex0MVId==ContVert0MVId*/)) 
//					//&& Succ0AuxId==cSucc0AuxId && Vertex0Path==ContVert0Path)) 
//					/*&& Result==pCont->GetResult()*/)
//				{
//					Result=pCont->GetResult();
//					AuxId=pCont->GetAuxId();
//					//CurrentMVId=pCont->GetCurrnetMVId();
//					return true;
//				}
//#ifdef _DEBUG
//				else
//				{
//					int a=5;
//				}
//#endif
//			}
//			AuxId=m_MaxAuxId+1;
//			return false;
//		}
//	protected:
//		unsigned m_MaxAuxId;
//		long long PrepareHash(const typename OutputType& Result,
//							  const _CSimpleVertexContainer* p1NextVertex,
//							  const MVIDS& Vert1MVId,
//							  //const string& Vert1Path,
//							  unsigned Succ1AuxId,
//							  const _CSimpleVertexContainer* p0NextVertex,
//							  const MVIDS& Vert0MVId,
//							  //const string& Vert0Path,
//							  unsigned Succ0AuxId)
//		{
//			long long Hash=HASH_CORE;
//			if(p1NextVertex!=NULL)
//				if(p1NextVertex!=ANY_VERTEX)
//				{
//					Hash^=GetTypedHash(*p1NextVertex);
//					Hash*=HASH_FACTOR;
//					Hash^=Succ1AuxId;
//					//Hash*=HASH_FACTOR;
//					//Hash^=Vert1MVId.GetHash();
//					//Hash^=(long long)hash_value(Vert1Path);
//				}
//				else
//					Hash^=(long long)ANY_VERTEX;
//			Hash*=HASH_FACTOR;
//			if(p0NextVertex!=NULL)
//				if(p0NextVertex!=ANY_VERTEX)
//				{
//					Hash^=GetTypedHash(*p0NextVertex);
//					Hash*=HASH_FACTOR;
//					Hash^=Succ0AuxId;
//					//Hash*=HASH_FACTOR;
//					//Hash^=Vert0MVId.GetHash();
//					//Hash^=(long long)hash_value(Vert0Path);
//				}
//				else
//					Hash^=(long long)ANY_VERTEX;
//			return Hash;
//		}
//	};
	//****************************_CResContList
	//****************************_CResCache
	//class _CResCache : public hash_multimap<long long,_CResContainer>
	//{
	//protected:
	//	typedef pair<iterator,iterator> RANGE;
	//	typedef hash_multimap<long long,_CResContainer> BASE_CACHE;
	//	class AUX_CACHE : public hash_multimap<long long,typename BASE_CACHE::value_type::second_type*>
	//	{
	//	protected:
	//	};
	//	BASE_CACHE m_ExactCache;
	//	AUX_CACHE m_MaybeCache;
	//public:
	//	_CResCache() {}
	//	_CResCache(const _CResCache& Source):
	//	hash_multimap<long long,_CResContainer>(Source) {}
	//	COMP_RES GetExisted(_CResContainer& Cont, 
	//		                typename OutputType& Result,
	//						unsigned& AuxId,
	//						_CResContList* pResContList, 
	//						const _CSimpleVertexContainer** pp1PrevVertex,
	//						unsigned* pSucc1AuxId,
	//						MVIDS* pVert1MVId,
	//						const _CSimpleVertexContainer** pp0PrevVertex,
	//						unsigned* pSucc0AuxId,
	//						MVIDS* pVert0MVId,
	//						MVIDS* pPrevMVId
	//						)
	//	{
	//		long long Hash=Cont.HashKey();
	//		iterator e=end();
	//		RANGE range=equal_range(Hash);
	//		iterator it_dif=e;
	//		AuxId=0;
	//		for(iterator it=range.first;it!=range.second;it++)
	//		{
	//			_CResContainer& ResCont=it->second;
	//			COMP_RES Res=ResCont.Comp(Cont);
	//			if(Res==EXACT_EQUAL)
	//			{
	//				Result=ResCont.GetResult();
	//				AuxId=ResCont.GetAuxId();
	//				//Cont.SetResult(Result);
	//				if(pp1PrevVertex!=NULL)
	//					*pp1PrevVertex=ResCont.GetPrev1Vertex();
	//				if(pSucc1AuxId!=NULL)
	//					*pSucc1AuxId=ResCont.GetSucc1AuxId();
	//				if(pVert1MVId!=NULL)
	//					*pVert1MVId=ResCont.GetVertex1MVId();
	//				if(pp0PrevVertex!=NULL)
	//					*pp0PrevVertex=ResCont.GetPrev0Vertex();
	//				if(pSucc0AuxId!=NULL)
	//					*pSucc1AuxId=ResCont.GetSucc0AuxId();
	//				if(pVert0MVId!=NULL)
	//					*pVert0MVId=ResCont.GetVertex0MVId();
	//				if(pPrevMVId!=NULL)
	//					*pPrevMVId=ResCont.GetCurrnetMVId();
	//				return EXACT_EQUAL;
	//			}
	//			else if(Res==DIFFERS_WITH_MAP)
	//			{
	//				it_dif=it;
	//				//_CResContainer& ResCont=it->second;
	//				unsigned Id=ResCont.GetAuxId();
	//				AuxId=max(AuxId,Id);
	//				if(pResContList!=NULL)
	//					pResContList->push(&ResCont);
	//				//{
	//				//	_CSubCircuitVerticesMap* pMap=ResCont.GetSubCircuitVerticesMap();
	//				//	//pVertMaps->push_back(_CSimilarVertMaps::PAIR(pMap,Id));
	//				//	pVertMaps->insert(_CSimilarVertMaps::PAIR(pMap,Id));
	//				//}
	//			}
	//		}
	//		if(it_dif!=e)
	//		{
	//			Result=it_dif->second.GetResult();
	//			//Cont.SetResult(Result);
	//			return DIFFERS_WITH_MAP;
	//		}
	//		return NOT_EQUAL;
	//	}
	//	void Register(_CResContainer& Cont)
	//	{
	//		long long MaybeHash=Cont.GetMaybeHash();
	//		long long ExactHash=Cont.GetExactHash();
	//		long long PostAnalyseHash=Cont.GetPostAnalyzeHash();

	//		insert(value_type(Cont.HashKey(),Cont));
	//	}
	//	void ClearLower(_CConstComponentPath& CPath)
	//	{
	//		iterator it=begin(), _end=end();	
	//		for(;it!=_end;)
	//		{
	//			_CResContainer& ResCont=it->second;
	//			if(CPath.Comp(it->second.GetFullPath())>=0)
	//				it=erase(it);
	//			else
	//				it++;
	//		}
	//	}
	//};
	//****************************_CResCache

public:
	_CAbstractOperator(_CMainCircuit* pMainCircuit, 
					   long StartSPower,
		               const string& BaseContext=EmptyString,
					   _CResCache* pExternalCache=NULL,
					   bool NoCache=false):
							m_pBaseCircuit(pMainCircuit),
							m_CurrentSPower(StartSPower),
							m_pCache(pExternalCache),
							m_PrivateCache(false),
							m_pBaseContext(&BaseContext),
							m_CurrentPath(pMainCircuit),
							m_1Multiplier(1), 
							m_0Multiplier(1)
	{
		if(m_pCache==NULL && !NoCache)
		{
			m_pCache=new _CResCache;
			m_PrivateCache=true;
		}
	 }
	  _CAbstractOperator(const _CAbstractOperator<typename OutputType,pEmptyResult,InputArguments>& Source):
							m_PrivateCache(Source.m_PrivateCache), 
							m_pBaseCircuit(Source.m_pBaseCircuit),
							m_Result(Source.m_Result), 
							m_CurrentTerminalVertices(Source.m_CurrentTerminalVertices),
							m_CurrentMVIds(Source.m_CurrentMVIds),
							m_CurrentSPower(Source.m_CurrentSPower),
							m_pBaseContext(Source.m_pBaseContext),
							m_CurrentPath(Source.m_CurrentPath),
							m_1Multiplier(Source.m_1Multiplier), 
							m_0Multiplier(Source.m_0Multiplier)//, /*m_State(Source.m_State),*/
							//m_CurrAuxId(Source.m_CurrAuxId),
							//m_Succ0AuxId(Source.m_Succ0AuxId),
							//m_Succ1AuxId(Source.m_Succ1AuxId),
							//m_PrevMVId(Source.m_PrevMVId),
							//m_Vert1MVId(Source.m_Vert1MVId),
							//m_Vert0MVId(Source.m_Vert0MVId),
							//m_pPreviousRealSimpleVertex(Source.m_pPreviousRealSimpleVertex), 
							//m_pDescSimpleVertex1(Source.m_pDescSimpleVertex1), 
							//m_pDescSimpleVertex0(Source.m_pDescSimpleVertex0), 
							//m_PrevValueAlreadyDefined(Source.m_PrevValueAlreadyDefined)/*,
							//m_PrimeLevel(Source.m_PrimeLevel)*/
	 {
		 if(m_PrivateCache)
			 m_pCache=new _CResCache(*Source.m_pCache);
		 else
			 m_pCache=Source.m_pCache;
	 }
	~_CAbstractOperator()
	{
		if(m_PrivateCache)
			delete m_pCache;
	}
	_CCircuit* GetCurrentCircuit()
	{
		const _CSubCircuitSocket* pSocket=GetCurrentSocket();
		if(pSocket==NULL)
			return reinterpret_cast<_CCircuit*> (m_pBaseCircuit);
		else
			return pSocket->GetModelCircuit();
		//if(m_SubInstances.empty())
		//	return (_CCircuit*) (m_pBaseCircuit);
		//else
		//	return m_SubInstances.back()->GetModelCircuit();

	}	
	typename OutputType GetLastResult() {return m_Result;}
	bool FindProperMap(long long  lTerminalVertexToCheck,const _CSimpleVertexContainer& ExpectedVertex)
	{
		_CSimilarVertMaps* pMap=m_SVMStack.smart_back();
		if(pMap==NULL)
			RISEPDD(eUnexpectedCondition,"Something wrong!!!");
		return pMap->FindProperMap(&GetOperationContext(),lTerminalVertexToCheck,&ExpectedVertex,m_CurrAuxId);
	}

	//COMP_RES IsExisted(InputArguments Vertices,typename OutputType& Result,
	//	_CAuxBasePlatorm** ppAuxBasePlatform=NULL,bool RetrivePreviousVertex=false)
	//{
	//	if(m_pCache==NULL)
	//		return NOT_EQUAL;
	//	else
	//	{
	//		COMP_RES Res=m_pCache->GetExisted(
	//			_CResContainer(
	//				Vertices,
	//				GetOperationContext(),
	//				m_CurrentPath.ClearComponent(),
	//				GetTerminalVert(),
	//				GetCurrentMVIds()),
	//			Result,
	//			m_CurrAuxId,
	//			RetrivePreviousVertex?&m_pPrevExactContainer:NULL,
	//			RetrivePreviousVertex?&m_pVert0ExactContainer:NULL,
	//			RetrivePreviousVertex?&m_pVert1ExactContainer:NULL,
	//			ppAuxBasePlatform
	//			//RetrivePreviousVertex?&m_pDescSimpleVertex1:NULL,
	//			//RetrivePreviousVertex?&m_Succ1AuxId:NULL,
	//			//RetrivePreviousVertex?&m_Vert1MVId:NULL,
	//			//RetrivePreviousVertex?&m_pDescSimpleVertex0:NULL,
	//			//RetrivePreviousVertex?&m_Succ0AuxId:NULL,
	//			//RetrivePreviousVertex?&m_Vert0MVId:NULL,
	//			//RetrivePreviousVertex?&m_PrevMVId:NULL
	//			);
	//		m_PrevValueAlreadyDefined=Res==EXACT_EQUAL?Defined:(Res==NOT_EQUAL?NotDefined:Maybe);
	//		return Res;
	//	}
	//}
	bool IsExisted(InputArguments Vertices,typename OutputType& Result)
	{
		if(m_pCache==NULL)
			return false;
		else
		{
			 return m_pCache->GetExisted(
				_CNResContainer(
					Vertices,
					GetOperationContext(),
					GetCurrentSPower(),
					GetCurrentMVIds()),
				Result);
		}
	}
	void RegisterInCache(InputArguments Vertices,
		                 typename OutputType& Result)
	{
		if(m_pCache!=NULL)
			m_pCache->Register(_CNResContainer(Vertices,
											  GetOperationContext(),
											  GetCurrentSPower(),
											  GetCurrentMVIds(),
											  Result));
	}
	//_CExactOutContainer* RegisterInCache(InputArguments Vertices,
	//	                 typename OutputType& Result,_CAuxBasePlatorm* pPrevBasePlatform)
	//{
	//	if(m_pCache!=NULL)
	//		return m_pCache->Register(_CResContainer(Vertices,
	//										  Result,
	//										  GetOperationContext(),
	//										  m_CurrentPath.ClearComponent(),
	//										  GetTerminalVert(),
	//										  GetCurrentMVIds(),
	//										  m_CurrAuxId,
	//										  m_pVert0ExactContainer,
	//										  m_pVert1ExactContainer
	//										  ////m_Vertex1Path,
	//										  //m_Vert1Res,
	//										  //m_Vert1MVId,
	//										  //m_pDescSimpleVertex1,
	//										  //m_Succ1AuxId,
	//										  ////m_Vertex0Path,
	//										  //m_Vert0Res,
	//										  //m_Vert0MVId,
	//										  //m_pDescSimpleVertex0,
	//										  //m_Succ0AuxId
	//										  ),pPrevBasePlatform);
	//	return NULL;
	//}
	//void PushState(_CSimpleVerticesMaps* pTerminalVertices,const _CSubCircuitSocket* pSocket)
	void PushState(_CSubCircuitVerticesMap* pTerminalVertices,const _CSubCircuitSocket* pSocket,
		long long MVId)
	{
		m_CurrentTerminalVertices.push_back(pTerminalVertices);
		m_CurrentMVIds.push_back(MVId);
		//m_SubInstances.push_back(pSocket);
		m_CurrentPath.SubmergeSocket(pSocket);
		//m_SVMStack.push_back_and_copy(pSimilarVertMap);
	}
	void PopState(_CSubCircuitVerticesMap*& pTerminalVertices,const _CSubCircuitSocket*& pSocket,
		long long& MVId)
	{
		pTerminalVertices=m_CurrentTerminalVertices.back();
		pSocket=m_CurrentPath.GetCurrentSocket();
		MVId=m_CurrentMVIds.back();
		//_CSimilarVertMaps* pSVMToStore=m_SVMStack.smart_back();
		//if(pSVMToStore==NULL)
		//	RISEPDD(eUnexpectedCondition,"Ooops!!!");
		//SVMToStore=*pSVMToStore;
		//pSocket=m_SubInstances.back();
		PopState();
	}
	void PopState()
	{
		m_CurrentTerminalVertices.pop_back();
		m_CurrentMVIds.pop_back();
		//if(!m_SVMStack.pop_back_and_destroy())
		//	RISEPDD(eUnexpectedCondition,"Ooops!");
		//m_SubInstances.pop_back();
		m_CurrentPath.EmergeSocket();
	}
	virtual typename OutputType DoTheOperation(const _CVertex& Vertex, 
		const _CSimpleVertexContainer& SVertex, const typename OutputType& Res1, 
		const typename OutputType& Res0)=0;
	//virtual void FinishDoTheOperation(typename OutputType& Result) {}
	//virtual void ModifyByMultiplier(typename OutputType& Result, short Multiplier) {}
	virtual void ModifyByMultiplier(typename OutputType& Result, short Multiplier) {}
	virtual typename OutputType DoThe01Operation(const _CSimpleVertexContainer& Vertex)=0;
	virtual typename OutputType Operate(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex/*,_CAuxBasePlatorm* pAuxBasePlatform*/)=0;
	//virtual typename OutputType  CheckExisting(const _CVertex& Vertex,const _CSimpleVertexContainer& SVertex, 
	//	const typename OutputType& ExpectedRes,_CResContList& ContList)=0;
	virtual bool AutoPredictResult(const _CSimpleVertexContainer& Successor,typename OutputType& Result) {return false;}
	virtual const string& GetOperationContext() {return *m_pBaseContext;}
	virtual bool CacheResult() {return true;}
	const string& GetBaseContext() {return *m_pBaseContext;}
	void ChangeBaseOperationContext(const string& NewContext) {m_pBaseContext=&NewContext;}
	//const NumericType& GetRealCompValue(const _CComponent& Component)
	//{
	//	const _CSubCircuitSocket* pSocket=GetCurrentSocket();
	//	if(pSocket!=NULL)
	//		return pSocket->GetParameterCurrentValue(&Component);
	//	else
	//		return Component.GetParameterValue();
	//}
	//const NumericType& GetRealCompValue(const _CVertex& Vertex)
	//{
	//	return GetRealCompValue(*Vertex.GetComponent());
	//}
	short GetRealMultiplier(const _CSimpleVertexContainer& SVertex)
	{
		if(SVertex.IsLocal())
			return SVertex.GetLocalMultiplier();
		if(SVertex.IsInputConnector())
		{
			unsigned long EntryId{0}, PosId{0};
			SVertex.DecomposeInputIdx(EntryId,PosId);
			_CCircuit* pModel=SVertex.GetSocket()->GetModelCircuit();
			const string* pTopContext=&GetBaseContext();
			const string* pNewContext=pTopContext;
			_CSimpleVertexContainer& NextVertex=pModel->GetSVertexForCofactor/*WithInheritance*/(EntryId,PosId,*pTopContext,&pNewContext);
			return 0;
		}
		if(SVertex.IsLocalTerminal())
			return 0;

		return 1;
	}
	short Pop1Multiplier() 
	{
		if(m_1Multiplier==1)
			return 1;
		short m=m_1Multiplier;
		m_1Multiplier=1;
		return m;
	}
	short Pop0Multiplier() 
	{
		if(m_0Multiplier==1)
			return 1;
		short m=m_0Multiplier;
		m_0Multiplier=1;
		return m;
	}
	short Get1Multiplier() { return m_1Multiplier; }
	short Get0Multiplier() { return m_0Multiplier; }

	void SetMultiplier(bool Is0Multiplier, short Multiplier)
	{
		if(Is0Multiplier)
			m_0Multiplier=Multiplier;
		else
			m_1Multiplier=Multiplier;
	}
	//void NotifySearchExsisted() {m_State=oDownLocalToSerchExisted;}
	//bool IsSearchExisted() {return m_State==oDownLocalToSerchExisted;}
	//void NotifyExactEqualIsFound() 
	//{
	//	ASSERTPDD(m_State==oDownLocalToSerchExisted);
	//	m_State=oUpExisting;
	//}
	//void NotifyNotEqualIsFound() 
	//{
	//	ASSERTPDD(m_State==oDownLocalToSerchExisted);
	//	m_State=oUpNotExisting;
	//	m_CurrAuxId++;
	//}
	//void NotifyTerminalWasReached() 
	//{
	//	ASSERTPDD(m_State==oDownLocalToSerchExisted);
	//	m_State=oLocalTerminalWasReached;
	//}
	//void NotifyNormal()
	//{
	//	ASSERTPDD(m_State==oDownLocalToSerchExisted);
	//	m_State=oNormal;
	//}
	//bool IsExactFound() {return m_State==oUpExisting;}
	//const _CSimpleVertexContainer* m_pPreviousRealSimpleVertex;
	//string m_PrevPath;
	//MVIDS m_PrevMVId;

	//_CExactOutContainer* m_pPrevExactContainer;
	//_CExactOutContainer* m_pVert1ExactContainer;
	//_CExactOutContainer* m_pVert0ExactContainer;


	//const _CSimpleVertexContainer* m_pDescSimpleVertex1;
	////string m_Vertex1Path;
	//MVIDS m_Vert1MVId;

	//typename OutputType m_Vert1Res;

	//const _CSimpleVertexContainer* m_pDescSimpleVertex0;
	////string m_Vertex0Path;
	//MVIDS m_Vert0MVId;

	//typename OutputType m_Vert0Res;

	unsigned GetAuxId() const {return m_CurrAuxId;}
	//void NotifyAlreadyDefined() {m_PrevValueAlreadyDefined=Defined;/*m_CurrAuxId=0;*/}
	//void NotifyNew() {m_PrevValueAlreadyDefined=NotDefined; /*m_CurrAuxId=0;*/}
	template<class SecondType, typename SecondType* pEmptyResult>
	void CopyPreviousDescendants(_CAbstractOperator<typename SecondType,pEmptyResult>& ParentOperator)
	{
		ParentOperator.m_pPreviousRealSimpleVertex=m_pPreviousRealSimpleVertex;
		//ParentOperator.m_pDescSimpleVertex0=m_pDescSimpleVertex0;
		//ParentOperator.m_pDescSimpleVertex1=m_pDescSimpleVertex1;
		//ParentOperator.m_Succ1AuxId=m_Succ1AuxId;
		//ParentOperator.m_Succ0AuxId=m_Succ0AuxId;
		ParentOperator.m_CurrAuxId=m_CurrAuxId;
	}
	//void NotifyPrevPath()
	//{
	//	m_PrevPath=m_CurrentPath.GetPath();
	//}
	//void NotifyPrevMVId()
	//{
	//	//m_PrevMVId=m_CurrentMVIds.empty()?0:m_CurrentMVIds.back();
	//	m_PrevMVId=m_CurrentMVIds;
	//}
	void ReSetSPower(unsigned long sPower) {m_CurrentSPower=sPower;}
	long GetCurrentSPower() const {return m_CurrentSPower;}
	void IncCurrentSPower(unsigned long Value) {m_CurrentSPower+=Value;}
	void DecCurrentSPower(unsigned long Value) {m_CurrentSPower-=Value;}
protected:
	//typedef enum {oNormal=0x01, 
	//	         oDownLocalToSerchExisted=0x02, 
	//			 oLocalTerminalWasReached=0x04,
	//			 oUpExisting=0x08, 
	//			 oUpNotExisting=0x10} TRACE_STATE;
	//TRACE_STATE m_State;
	typedef enum{Maybe,Defined,NotDefined} ALREADY_DEFINED;
	//ALREADY_DEFINED m_PrevValueAlreadyDefined;
	short m_1Multiplier;
	short m_0Multiplier;
	const string* m_pBaseContext;
	inline const _CSubCircuitSocket* GetCurrentSocket() const
	{
		//return m_SubInstances.empty()?NULL:m_SubInstances.back();
		//const _CSubCircuitSocket* pSock1=m_SubInstances.empty()?NULL:m_SubInstances.back();
		return m_CurrentPath.GetCurrentSocket();
		//ASSERTPDD(pSock1==pSock2);
	}
	_CMainCircuit* m_pBaseCircuit;
	_CSubCircuitVerticesMap* GetTerminalVert()
	{
		return m_CurrentTerminalVertices.empty()?NULL:m_CurrentTerminalVertices.back();
	}
	//long long GetCurrentMVId()
	//{
	//	return m_CurrentMVIds.empty()?0:m_CurrentMVIds.back();
	//}
	const MVIDS& GetCurrentMVIds()
	{
		return m_CurrentMVIds;
	}
	//list<_CSubCircuitSocket*> m_SubInstances;
	//list<_CSimpleVerticesMaps*> m_CurrentTerminalVertices;
	list<_CSubCircuitVerticesMap*> m_CurrentTerminalVertices;
	MVIDS m_CurrentMVIds;
	long m_CurrentSPower;
	_CConstComponentPath m_CurrentPath;
	_CResCache* m_pCache;
	//_CNCache* m_pNCache;
	typename OutputType m_Result = {};
	//unsigned m_CurrAuxId;
	//unsigned m_Succ1AuxId;
	//unsigned m_Succ0AuxId;
	//unsigned m_PrimeLevel;
	bool m_PrivateCache;
	//_CSimilarVertMapsStack m_SVMStack;
	friend class _CSimpleVertexContainer;
};

template<typename OutputType, typename OutputType* pEmptyResult, bool DistinguishPaths=true>
class _CUnaryAbstractOperator : public _CAbstractOperator<typename OutputType,pEmptyResult>
{
public:
	~_CUnaryAbstractOperator()
	{
	}
	_CUnaryAbstractOperator(_CMainCircuit* pMainCircuit, long StartSPower, const string& BaseContext=EmptyString,_CResCache* pExternalCache=NULL,bool NoCache=false): 
	  _CAbstractOperator<typename OutputType,pEmptyResult>(pMainCircuit,StartSPower,BaseContext,pExternalCache,NoCache) {}
	  _CUnaryAbstractOperator(const _CUnaryAbstractOperator<typename OutputType,pEmptyResult>& Source):
	  _CAbstractOperator<typename OutputType,pEmptyResult>(Source) {}
	//virtual OutputType Determine0DescendantRes(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex)
	//{
	//	const _CSimpleVertexContainer &D=Vertex.Get0Succ();
	//	return D.DoUnaryOpertationForWholeTree(*this);
	//}
	//virtual OutputType Determine1DescendantRes(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex)
	//{
	//	const _CSimpleVertexContainer &D=Vertex.Get1Succ();
	//	return D.DoUnaryOpertationForWholeTree(*this);
	//}
	virtual typename OutputType Operate(const _CVertex& Vertex, 
		const _CSimpleVertexContainer& SVertex/*,
		_CAuxBasePlatorm* pAuxBasePlatform*/)
	{
		typename OutputType Res0=TraceDescendant(true,Vertex);
		typename OutputType Res1=TraceDescendant(false,Vertex);
		typename OutputType Res=DoTheOperation(Vertex,SVertex,Res1,Res0);
		return Res;
	}
	//typename OutputType TraceDescendant(
	//	bool Is0Descendant,
	//	const _CVertex& Vertex,
	//	const _CSimpleVertexContainer*& pRealPreviousVertex,
	//	unsigned& PrevAuxId,
	//	MVIDS& PrevMVId,
	//	_CExactOutContainer*& pPrevExactOutContainer)
	//	//string& PrevPath)
	//{
	//	const _CSimpleVertexContainer* pVertCont=Is0Descendant?&Vertex.Get0Succ():&Vertex.Get1Succ();
	//	typename OutputType Res=pVertCont->DoUnaryOpertationForWholeTree(*this,Is0Descendant);
	//	pRealPreviousVertex=m_pPreviousRealSimpleVertex;
	//	PrevAuxId=m_CurrAuxId;
	//	PrevMVId=m_PrevMVId;
	//	pPrevExactOutContainer=m_pPrevExactContainer;
	//	//PrevPath=m_PrevPath;
	//	return Res;
	//}
	typename OutputType TraceDescendant(
		bool Is0Descendant,
		const _CVertex& Vertex)
	{
		const _CSimpleVertexContainer* pVertCont=Is0Descendant?&Vertex.Get0Succ():&Vertex.Get1Succ();
		long sPowerShift=Is0Descendant?0:Vertex.GetSPower();
		if(sPowerShift!=0)
			m_CurrentSPower-=sPowerShift;
		typename OutputType Res=pVertCont->DoUnaryOpertationForWholeTree(*this,Is0Descendant);
		if(sPowerShift!=0)
			m_CurrentSPower+=sPowerShift;
		return Res;
	}
	//virtual typename OutputType Trace0(const _CVertex& Vertex)
	//{
	//	return TraceDescendant(true,Vertex);
	//}
	//virtual typename OutputType Trace1(const _CVertex& Vertex)
	//{
	//	return TraceDescendant(false,Vertex);
	//}
	//virtual typename OutputType  CheckExisting(const _CVertex& Vertex,const _CSimpleVertexContainer& SVertex, 
	//	const typename OutputType& ExpectedRes,_CResContList& ContList)
	//{
	//	unsigned OldAuxId=m_CurrAuxId;
	//	Vertex.Get0Succ().DoUnaryOpertationForWholeTree(*this,true);
	//	if(m_PrevValueAlreadyDefined==Defined)
	//		Vertex.Get1Succ().DoUnaryOpertationForWholeTree(*this,false);
	//	if(m_PrevValueAlreadyDefined==Defined)
	//	{
	//		m_CurrAuxId=OldAuxId;
	//		typename OutputType Res=ExpectedRes;
	//		if(CacheResult())
	//			RegisterInCache(&SVertex,Res);
	//		return Res;
	//	}
	//	unsigned OldPrimeLevel=m_CurrAuxId/*m_PrimeLevel*/;
	//	//m_PrimeLevel=OldAuxId+1;
	//	typename OutputType Res=Operate(Vertex,SVertex,ContList);
	//	//m_CurrAuxId=m_PrimeLevel;
	//	if(CacheResult())
	//		RegisterInCache(&SVertex,Res);
	//	//m_PrimeLevel=OldPrimeLevel;
	//	m_CurrAuxId=OldAuxId;
	//	m_PrevValueAlreadyDefined=NotDefined;
	//	return Res;
	//	

	//	//TRACE_STATE OldState=m_State;
	//	//Vertex.Get0Succ().DoUnaryOpertationForWholeTree(*this,true);
	//	//bool Is0ExactFound=m_State==oUpExisting;
	//	//unsigned AuxId0=m_CurrAuxId;
	//	//m_State=OldState;
	//	//Vertex.Get1Succ().DoUnaryOpertationForWholeTree(*this,false);
	//	//bool Is1ExactFound=IsExactFound();
	//	//unsigned AuxId1=m_CurrAuxId;
	//	//m_State=OldState;
	//	//if(Is0ExactFound && Is1ExactFound)
	//	//{
	//	//	NotifyExactEqualIsFound();
	//	//	return ExpectedRes;
	//	//}
	//	//if(IsSearchExisted())
	//	//	NotifyNotEqualIsFound();
	//	//unsigned AuxId=m_CurrAuxId;
	//	//typename OutputType Res=Operate(Vertex,SVertex);
	//	//m_CurrAuxId=AuxId;
	//	//if(CacheResult())
	//	//	RegisterInCache(&SVertex,Res);
	//	//return ExpectedRes;
	//}
};

class _C2Args : public pair<const _CSimpleVertexContainer*,const _CSimpleVertexContainer*>
{
};

template<typename OutputType,typename OutputType EmptyResult>
class _CBinaryAbstractOperator : public _CAbstractOperator<typename OutputType,EmptyResult, _C2Args>
{
public:
	_CBinaryAbstractOperator() {}
	~_CBinaryAbstractOperator() {}
};

class _CSimpleVertexContainer
{
public:
	~_CSimpleVertexContainer()
	{
	}
	//lVertex==-2 IsEmpty
	//lVertex==-3 IsLocalTerminal
	//lVertex==-4 IsInputConnector
	enum class VertType{Empty=0, Local,LocalTerminal, InputConnector};
	//_CSimpleVertexContainer::_CSimpleVertexContainer(long long lVertex, short Multiplier, const _CSubCircuitSocket* pSocket,const _CSimpleVertexMap* pToSubcircuitInput):
	//	m_lVertex(lVertex), m_pSocket(pSocket), 
	//		m_Multiplier(Multiplier), m_pConnectionVector(pToSubcircuitInput){}
	_CSimpleVertexContainer():
		m_lVertex(0),m_pConnectionVector(NULL),m_pSocket(NULL),m_VertexType(VertType::Empty),m_Multiplier(0),
			m_MVId(0)
		{
		}
	_CSimpleVertexContainer(unsigned long long LocalTerminalIndex):
		m_lVertex(LocalTerminalIndex),m_pConnectionVector(NULL),m_pSocket(NULL),m_Multiplier(1), 
			m_VertexType(VertType::LocalTerminal), m_MVId(0)
	{
	}
	_CSimpleVertexContainer(long long lVertex, short Multiplier):
		m_lVertex(lVertex), m_pConnectionVector(NULL),m_pSocket(NULL),m_VertexType(VertType::Local),
			m_Multiplier(Multiplier), m_MVId(0) 
	{
	}
	_CSimpleVertexContainer(const _CSimpleVertexContainer& Source, short ExtraMultiplier=1): 
		m_lVertex(Source.m_lVertex),m_Multiplier(Source.m_Multiplier),m_VertexType(Source.m_VertexType),
			m_pConnectionVector(Source.m_pConnectionVector),m_pSocket(Source.m_pSocket), m_MVId(Source.m_MVId) 
	{
		if(ExtraMultiplier!=1)
			m_Multiplier*=ExtraMultiplier;
	}
	//_CSimpleVertexContainer(const _CSimpleVertexContainer& Source, const _CSubCircuitSocket* pSocket,_CSimpleVerticesMaps* pToSubcircuitInput):
		_CSimpleVertexContainer(const _CSimpleVertexContainer& Source, 
			const _CSubCircuitSocket* pSocket,_CSubCircuitVerticesMap* pToSubcircuitInput,
			long long MVId):
		m_lVertex(Source.m_lVertex),m_Multiplier(Source.m_Multiplier),m_VertexType(VertType::InputConnector),
			m_pConnectionVector(pToSubcircuitInput),m_pSocket(pSocket), m_MVId(MVId) 
	{
	}
	//_CSimpleVertexContainer(long long lVertex, const _CSubCircuitSocket* pSocket, _CSimpleVerticesMaps* pToSubcircuitLeves):
		_CSimpleVertexContainer(long long lVertex, const _CSubCircuitSocket* pSocket, 
			_CSubCircuitVerticesMap* pToSubcircuitLeves, long long MVId):
		m_lVertex(lVertex),m_Multiplier(1), m_VertexType(VertType::LocalTerminal),m_pSocket(pSocket), 
			m_pConnectionVector(pToSubcircuitLeves), m_MVId(MVId)
	{
	}
	_CSimpleVertexContainer(long long Index):
		m_lVertex(Index),m_Multiplier(1), m_VertexType(VertType::LocalTerminal),m_pSocket(NULL), 
			m_pConnectionVector(NULL), m_MVId(0)
	{
	}
	//_CSimpleVertexContainer(size_t EntryId, size_t PosId, const _CSubCircuitSocket* pSocket, _CSimpleVerticesMaps* pToSubcircuitLeves);
	_CSimpleVertexContainer(unsigned long EntryId, unsigned long PosId, const _CSubCircuitSocket* pSocket, 
		_CSubCircuitVerticesMap* pToSubcircuitLeves,long long MVId);
	bool IsEmpty() const {return m_VertexType==VertType::Empty;}
	bool IsLocal() const {return m_VertexType==VertType::Local;}
	bool IsLocalTerminal() const {return m_VertexType==VertType::LocalTerminal;}
	bool IsInputConnector() const {return m_VertexType==VertType::InputConnector && m_pConnectionVector!=NULL && m_pSocket!=NULL;}
	bool Is0() const {return IsLocal() && m_lVertex*m_Multiplier==0;}
	bool Is1() const {return IsLocal() && m_lVertex*m_Multiplier==1;}
	bool IsM1() const {return IsLocal() && m_lVertex*m_Multiplier==-1;}
	bool Is01M1() const {return IsLocal() && (m_lVertex>=-1 && m_lVertex<=1);}
	_CSimpleVertexContainer& operator=(const _CSimpleVertexContainer& Right);
	bool IsLameLeg();
	bool operator==(const _CSimpleVertexContainer& Right) const;
	bool operator!=(const _CSimpleVertexContainer& Right) const
	{
		return !(operator==(Right));
	}
	//iostream& operator<<(iostream& stream) const
	//{
	//	WriteToStream(stream,false);
	//	return stream;
	//}
	long long GetLocalVertex() const {
		ASSERTPDD(!IsLocalTerminal());
		return m_lVertex;}
	short GetLocalMultiplier() const {return m_Multiplier;}
	void MultiplyMultiplier(short Multiplier) {
		/*ASSERTPDD(IsLocal());*/ m_Multiplier*=Multiplier;}
	void ReduceMultiplier() { /*ASSERTPDD(IsLocal());*/
		if(m_Multiplier!=0) 
			m_Multiplier=1;
	}
	size_t GetHash(size_t& Core) const;
	string iVertex2string() const;
	//***************DoUnaryOprtationForWholeTree*****************
	template<typename OutputType, typename OutputType* pEmptyResult, bool DistinguishPaths>
	typename OutputType DoUnaryOpertationForWholeTree
		(_CUnaryAbstractOperator<typename OutputType, pEmptyResult,DistinguishPaths>& Operator,
		bool Is0Descendent) const
	{
		if(Is01M1())
		{
			Operator.SetMultiplier(Is0Descendent,1);
			return Operator.DoThe01Operation(*this);
		}
		typename OutputType Res;		
   		if(IsInputConnector())
		{
			//size_t EntryId,PosId;
			unsigned long EntryId{0}, PosId{0};
			DecomposeInputIdx(EntryId, PosId);
			_CCircuit* pModel=m_pSocket->GetModelCircuit();
			const string* pTopContext=&Operator.GetBaseContext();
			const string* pNewContext=pTopContext;
			long SPowerShift=0;
			//size_t SPower=(size_t)Operator.GetCurrentSPower();
			unsigned short SPower=(unsigned short)Operator.GetCurrentSPower();
			_CSimpleVertexContainer& NextVertex=pModel->GetSVertexForCofactor/*WithInheritance*/(EntryId,/*PosId*/SPower,*pTopContext,&pNewContext,&SPowerShift);
			if(pTopContext!=pNewContext)
				Operator.ChangeBaseOperationContext(*pNewContext);
			if(SPowerShift!=0)
				Operator.DecCurrentSPower(SPowerShift);
			Operator.PushState(m_pConnectionVector,m_pSocket,m_MVId);
 			if(!Operator.AutoPredictResult(NextVertex,Res))
				Res=NextVertex.DoUnaryOpertationForWholeTree(Operator,Is0Descendent);
			Operator.PostOperateSubCircuitResult(EntryId, PosId,Res);
			Operator.PopState();
			if(SPowerShift!=0)
				Operator.IncCurrentSPower(SPowerShift);
			if(pTopContext!=pNewContext)
				Operator.ChangeBaseOperationContext(*pTopContext);
			if(NextVertex.IsLocal())
				Operator.SetMultiplier(Is0Descendent,NextVertex.GetLocalMultiplier());
			return Res;
		}
		if(IsLocalTerminal())
		{
			_CSubCircuitVerticesMap* pTempVector = nullptr;
			const _CSubCircuitSocket* pTempSocket = nullptr;
			long long TempMVId = -1;
			Operator.PopState(pTempVector,pTempSocket, TempMVId);

			ASSERTPDD(pTempVector!=NULL);
			ASSERTPDD(pTempSocket!=NULL);
			const string* pCurrContext=&Operator.GetBaseContext();
			const string* pNewContext=pCurrContext;
			unsigned long EntryId{0}, PosId{0};
			DecomposeInputIdx(EntryId, PosId);
			size_t SPower=(size_t)Operator.GetCurrentSPower();
			const _CSimpleVertexContainer& OutVertex=pTempVector->get(EntryId,SPower,pNewContext);
			if(pCurrContext!=pNewContext)
				Operator.ChangeBaseOperationContext(*pNewContext);
			if(!Operator.AutoPredictResult(OutVertex,Res))
				Res=OutVertex.DoUnaryOpertationForWholeTree(Operator,Is0Descendent);
			Operator.PushState(pTempVector,pTempSocket,TempMVId);
			Operator.PostOperateTerminals(m_lVertex,Res);
			if(pCurrContext!=pNewContext)
				Operator.ChangeBaseOperationContext(*pCurrContext);
			if(OutVertex.IsLocal() && !OutVertex.Is01M1())
				Operator.SetMultiplier(Is0Descendent,OutVertex.GetLocalMultiplier());
			return Res;
		}
		if(Operator.IsExisted(this,Res))
		{
			Operator.ModifyByMultiplier(Res,m_Multiplier);
			return Res;
		}
		_CVertex* pVertex=Operator.GetCurrentCircuit()->GlobalVertexFromId(m_lVertex);
		Res=Operator.Operate(*pVertex,*this);
		if(Operator.CacheResult())
			Operator.RegisterInCache(this,Res);
		Operator.ModifyByMultiplier(Res,m_Multiplier);
		return Res;
	}
	long long GetLVertex() const {return m_lVertex;}
	//unsigned long long GetTerminalIndex() const {ASSERTPDD(!IsLocal()); return m_Par2.m_TerminalIndex;}
	//const _CSimpleVerticesMaps* GetConnectionMap() const {return m_pConnectionVector;}
	const _CSubCircuitVerticesMap* GetConnectionMap() const {return m_pConnectionVector;}
	const _CSubCircuitSocket* GetSocket() const {return m_pSocket;}
	bool less(const _CSimpleVertexContainer& Right) const
	{
		return m_VertexType<Right.m_VertexType ||
			m_lVertex<Right.m_lVertex ||
			m_Multiplier<Right.m_Multiplier;
	}
	operator size_t() const 
	{
		unsigned long long Res=HASH_CORE;
		Res*=HASH_FACTOR;
		Res^=(unsigned long long)m_VertexType;
		Res*=HASH_FACTOR;
		Res^=m_Multiplier;
		Res*=HASH_FACTOR;
		Res^=m_lVertex;
		return (size_t)Res;
	}
	void WriteToStream(iostream& stream, bool StopEmbading, _CSimpleVerticesMapsSet& Set) const;
	//void WriteToStream(iostream& stream, bool StopEmbading) const;
	void DecomposeInputIdx(unsigned long& EntryId, unsigned long& PosId) const;
	long long ConsiderInHash(long long& Seed) const;
	short GetSPowerShift() 
	{
		VERIFYPDD(m_VertexType==VertType::LocalTerminal);
		return (short)m_MVId;
	}
	void SetSPowerShift(short sPower) 
	{
		VERIFYPDD(m_VertexType==VertType::LocalTerminal);
		m_MVId=(long long)sPower;
	}
protected:
	long long m_lVertex;
	short m_Multiplier;
	VertType m_VertexType;
	//union tagPar2
	//{
	//	short m_Multiplier;
	//	unsigned long long m_TerminalIndex;
	//} m_Par2;
	//_CSimpleVerticesMaps* m_pConnectionVector;
	_CSubCircuitVerticesMap* m_pConnectionVector;
	const _CSubCircuitSocket* m_pSocket;
	long long m_MVId;//ModelVertexId or sPowerShift
	friend class _CSExpandedVertices;
	//friend class _CSimpleVertexMap;
	friend class _CSimpleVerticesMaps;
	friend class _CSubCircuitVerticesMap;
	friend class _CBoundaryTerminals;
	//friend class _CVertex;
};

extern _CSimpleVertexContainer EmptyVertex;
long long GetTypedHash(long long& Core, const _CSimpleVertexContainer& Vertex);

extern _CSimpleVertexContainer ZeroSVertex;
extern _CSimpleVertexContainer OneSVertex;
extern _CSimpleVertexContainer MOneSVertex;

//iostream& operator<<(iostream& stream, const _CSimpleVertexContainer& Container);

class _CSExpandedVertices : public vector<_CSimpleVertexContainer>
{
public:
	~_CSExpandedVertices() 
	{
	}
	enum {EMPTY=0,ZERO,ONE,MINUS_ONE};
	_CSExpandedVertices(int BasicType=EMPTY)
	{
		switch(BasicType)
		{
		case ZERO: SetZero();break;
		case ONE: SetOne();break;
		case MINUS_ONE: SetMinusOne();break;
		default: ;
		}
	}
	_CSimpleVertexContainer& GetVertex(int Index);
	void SetVertex(const _CSimpleVertexContainer& Vertex,int Index);
	//void Copy(const _CSExpandedVertices& Source, _CSubCircuitSocket* pSocket,_CSimpleVerticesMaps* pToSubcircuitInput);
	void Copy(const _CSExpandedVertices& Source, _CSubCircuitSocket* pSocket,_CSubCircuitVerticesMap* pToSubcircuitInput,long long MVId);
	//void SetInputConnectorData(size_t LocalVerticesId,const _CSExpandedVertices& LocalSource,_CSubCircuitSocket* pSocket,_CSimpleVerticesMaps* pToSubcircuitInput);
	void SetInputConnectorData(unsigned long LocalVerticesId,const _CSExpandedVertices& LocalSource,
		_CSubCircuitSocket* pSocket,_CSubCircuitVerticesMap* pToSubcircuitInput, long long MVId);
	//reference at(size_type Index)
	//{
	//	if(((int)Index)<0)
	//		return ZeroSVertex;
	//	if(Index>=size())
	//		resize(Index+1);
	//	vector<_CSimpleVertexContainer>::at(Index);
	//}
	size_t GetHash(size_t& Core) const;
	void SetZero() {resize(0); push_back(_CSimpleVertexContainer(0,0));}
	void SetOne() {resize(0); push_back(_CSimpleVertexContainer(1,1));}
	void SetMinusOne() {resize(0);push_back(_CSimpleVertexContainer(1,-1));}
	void SetEmpty() {resize(0);}
	inline bool IsEmpty() {return empty();}
	//_CSimpleVertexContainer& operator[](size_type Index) {return at(Index);}
	//iostream& operator<<(iostream& stream) const;
	void WriteToStream(iostream& stream, _CSimpleVerticesMapsSet& Set);
	//void WriteToStream(iostream& stream);
};

class _CSExpandedUniqeSet
{
public:
	_CSExpandedUniqeSet() {}
	~_CSExpandedUniqeSet() 
	{
	}
	_CSExpandedVertices* GetUnique(_CSExpandedVertices& Source);
	void clear() {m_Set.clear();}
protected:
	typedef unordered_multimap<long long,_CSExpandedVertices> SET;
	typedef SET::iterator iterator;
	typedef pair<iterator,iterator> RANGE;
	typedef SET::value_type value_type;
	SET m_Set;
};

//iostream& operator<<(iostream& stream, const _CSExpandedVertices& Vertices);

extern _CSExpandedVertices StaticEmptyVertices;
extern _CSExpandedVertices StaticZeroVertices;
extern _CSExpandedVertices StaticOneVertices;
extern _CSExpandedVertices StaticMinusOneVertices;

class _CSExpandedDescendends : public vector<_CSExpandedVertices*>
{
public:
	~_CSExpandedDescendends() {}
	_CSExpandedDescendends(size_type n=0): vector<_CSExpandedVertices*>(n) {}
	size_t GetHash(size_t& Core) const;
};

class _CSExpandedCofactorValues : public vector<_CSExpandedVertices>
{
public:
	~_CSExpandedCofactorValues() {}
	_CSExpandedCofactorValues(size_type n=0): vector<_CSExpandedVertices>(n) {}
	size_t GetHash(size_t& Core) const;
};
//typedef _CSExpandedDescendends _CSExpandedCofactorValues;


//class _CSimpleVertexMap 
//{
//public:
//	_CSimpleVertexMap(_CCircuit* pBaseCircuit):m_MaxSPower(0),m_NoOfDescandence(0),
//		m_pBaseCircuit(pBaseCircuit) {}
//	_CSimpleVertexMap(const _CSimpleVertexMap& Source):m_NoOfDescandence(Source.m_NoOfDescandence),
//		m_MaxSPower(Source.m_MaxSPower),m_pBaseCircuit(Source.m_pBaseCircuit) {}
//	void TranslateLocalTerminals(_CSExpandedDescendends& Descendants, const string& Context);
//	//long long GetHash() const;
//	const _CSimpleVertexContainer& get(long long Index,const string*& pContext) const;
//	void put(long long Index,const string& Context, _CSimpleVertexContainer& Value); 
//	iostream& operator<<(iostream& stream) const;
//	_CSimpleVertexMap& operator=(const _CSimpleVertexMap& Source);
//	bool operator==(const _CSimpleVertexMap& Right) const;
//protected:
//
//	_CCircuit* m_pBaseCircuit;
//	class BASIC_MAP : public unordered_map<long long, const _CSimpleVertexContainer>
//	{
//	};
//	//typedef unordered_map<long long, const _CSimpleVertexContainer> BASIC_MAP;
//	//typedef unordered_map<string, BASIC_MAP> ALL_MAPS;
//	class ALL_MAPS : public unordered_map<const string*, BASIC_MAP>
//	{
//	};
//	//typedef unordered_map<const string*, BASIC_MAP> ALL_MAPS;
//	ALL_MAPS m_Maps;
//	typedef ALL_MAPS::size_type size_type;
//	size_type m_NoOfDescandence;
//	size_type m_MaxSPower;
//	BASIC_MAP& GetMapInContext(const string& Context);
//	ALL_MAPS::const_iterator SearchMapIteContextOrInherited(const string*& pContext, size_t& Counter) const;
//	const _CSimpleVertexContainer* SearchSVertex(long long Index,const string*& pContext) const;
//	const _CSimpleVertexContainer* SearchNextSVertex(long long Index,const string*& pContext) const;
//	class _Cache : public list<pair<_CSimpleVertexContainer,long long>>
//	{
//	public:
//		long long already_existed(const _CSimpleVertexContainer& New);
//	protected:
//		class _VertPred
//		{
//		public:
//			typedef _Cache::value_type value_type;
//			_VertPred(const _CSimpleVertexContainer& Value):m_Value(Value) {}
//			bool operator()(const value_type& Value) const {return Value.first==m_Value;}
//		protected:
//			const _CSimpleVertexContainer& m_Value;
//		};
//	};
//};
class __Int_Type : public pair<_CSimpleVertexContainer,const string*>
{
public:
	~__Int_Type() {}
	__Int_Type():m_Empty(true) 
	{
	}
	__Int_Type(const __Int_Type& Source):
	pair<_CSimpleVertexContainer,const string*>(Source),m_Empty(Source.m_Empty){}
	typedef const string* _SCArg;
	__Int_Type(const _CSimpleVertexContainer& VC, const _SCArg Context):
	pair<_CSimpleVertexContainer,const string*>(VC,Context),m_Empty(false) {}
	bool IsEmpty() {return m_Empty;}
	__Int_Type& operator=(const __Int_Type& Source)
	{
		first=Source.first;
		second=Source.second;
		m_Empty=Source.m_Empty;
		return *this;
	}
	bool operator==(const __Int_Type& Right)
	{
		if(m_Empty!=Right.m_Empty)
			return false;
		if(m_Empty)
			return true;
		return  (first==Right.first) && (second==Right.second);
	}
protected:
	bool m_Empty;
};

class _CVerticesMapCache;

class _CSubCircuitVerticesMap : public _CAbstractResTables<__Int_Type>
{
//protected:
//	typedef  Int_Type;
public:
	~_CSubCircuitVerticesMap()
	{
	}
	typedef unsigned long long T_INDEX;
	_CSubCircuitVerticesMap(_CCircuit* pCircuit/*, _CVerticesMapCache& Cache*/):m_pBaseCircuit(pCircuit),m_ValidHash(false),
		m_IsRegistered(false)/*,m_Cache(Cache)*/ {}
	void TranslateLocalTerminals(_CSExpandedDescendends& Descendants, const string& Context);
	const _CSimpleVertexContainer& get(T_INDEX Index,const string*& pContext) const;
	const _CSimpleVertexContainer* smart_get(T_INDEX Index,const string*& pContext) const;
	const _CSimpleVertexContainer& get(size_t EntryNo, size_t SPower,const string*& pContext) const;
	const _CSimpleVertexContainer* smart_get(size_t EntryNo, size_t SPower,const string*& pContext) const;
	T_INDEX put(T_INDEX Index,const string& Context, _CSimpleVertexContainer& Value, 
		const string& PostTerminalContext, const string& BaseContext); 
	T_INDEX put(unsigned long EntryNo, unsigned long SPower,const string& Context, _CSimpleVertexContainer& Value, 
		const string& PostTerminalContext, const string& BaseContext); 
	//iostream& operator<<(iostream& stream) const;
	void WriteToStream(iostream& stream,_CSimpleVerticesMapsSet& Set) const;
	_CSubCircuitVerticesMap& operator=(const _CSubCircuitVerticesMap& Source);
	bool operator==(const _CSubCircuitVerticesMap& Right) const;
	bool IsContextEql(_CSubCircuitVerticesMap& Right, const string* pContext) ;
	long long GetHash(const string* pContext);
	bool IsRegisterd() const {return m_IsRegistered;}
protected:
class _Cache
{
public:
	bool check_if_exists(unsigned long& Entry, unsigned long& sPower, _CSimpleVertexContainer& VC);
	bool CleanGarbage();
protected:
	class __Container
	{
	public:
		__Container(unsigned long& Entry, unsigned long& sPower, _CSimpleVertexContainer& VC):
		m_Entry(Entry), m_sPower(sPower), m_VC(VC) {}
		__Container(const __Container& Source):
		m_Entry(Source.m_Entry), m_sPower(Source.m_sPower),m_VC(Source.m_VC) {}
		const unsigned long m_Entry;
		const unsigned long m_sPower;
		const _CSimpleVertexContainer m_VC;
	};
	typedef unordered_multimap<long long,__Container> MAP;
	long long SCHash(long long& Core, const _CSimpleVertexContainer& VC) const 
	{
		return VC.ConsiderInHash(Core);
	}
	MAP m_Map;
};
	bool m_IsRegistered;
	friend class _CSubCircuitVerticesMapBuffer;
	//class _CVerticesMapCache
	//{
	//public:
	//	bool check_if_exists(size_t& Entry, size_t& sPower, _CSimpleVertexContainer& VC);
	//protected:
	//	class __Container
	//	{
	//	public:
	//		__Container(size_t& Entry, size_t& sPower, _CSimpleVertexContainer& VC):
	//		m_Entry(Entry), m_sPower(sPower), m_VC(VC) {}

	//		const size_t m_Entry;
	//		const size_t m_sPower;
	//		const _CSimpleVertexContainer m_VC;
	//	};
	//	typedef hash_multimap<size_t,__Container> MAP;
	//	size_t SCHash(const _CSimpleVertexContainer& VC) const {return VC.ConsiderInHash(HASH_CORE,HASH_FACTOR);}
	//	MAP m_Map;
	//};
	_CCircuit* m_pBaseCircuit = nullptr;
	long long m_MyHash = 0;
	bool m_ValidHash = false;
	//_CVerticesMapCache& m_Cache;
};

class _CSubCircuitVerticesMapBuffer : unordered_multimap<long long,pair<_CSubCircuitVerticesMap*,size_t>>
{
public:
	_CSubCircuitVerticesMapBuffer():m_Counter(1) {}
	~_CSubCircuitVerticesMapBuffer();
	_CSubCircuitVerticesMap* RegisterMap(_CSubCircuitVerticesMap* pMap, bool DeleteIfAlreadyExist=true);
	size_t Map2Index(_CSubCircuitVerticesMap* pMap);
	_CSubCircuitVerticesMap* Index2Map(size_t Index) {return m_Vector[Index];}
	void WriteToStream(iostream& stream);
	void SetCircuit(_CCircuit* pBaseCircuit) {m_pBaseCircuit=pBaseCircuit;}
protected:
	typedef pair<_CSubCircuitVerticesMap*,size_t> ENTRY;
	typedef self_exp_vector<_CSubCircuitVerticesMap*> AUX_VECTOR;
	iterator __Find(_CSubCircuitVerticesMap* pMap);
	size_t m_Counter;
	AUX_VECTOR m_Vector;
	_CCircuit* m_pBaseCircuit = nullptr;
};


class _CSimpleVerticesMapsSet
{
public:
	~_CSimpleVerticesMapsSet()
	{
	}
	//size_t Add(const _CSimpleVerticesMaps* pVertex);
	size_t Add(const _CSubCircuitVerticesMap* pVertex);
	//const _CSimpleVerticesMaps* operator[](size_t Index) const {return m_Index2Vector[Index];}
	const _CSubCircuitVerticesMap* operator[](size_t Index) const {return m_Index2Vector[Index];}
	size_t size() const {return m_Index2Vector.size();}
	void WriteToStream(iostream& stream);
protected:
	//typedef unordered_map<const _CSimpleVerticesMaps*, size_t> CONT_2_INDEX;
	//typedef vector<const _CSimpleVerticesMaps*> INDEX_2_VECTOR;
	typedef unordered_map<const _CSubCircuitVerticesMap*, size_t> CONT_2_INDEX;
	typedef vector<const _CSubCircuitVerticesMap*> INDEX_2_VECTOR;
	CONT_2_INDEX m_Cont2Index;
	INDEX_2_VECTOR m_Index2Vector;
};

//class _my_txt_fstream : public fstream
//{
//public:
//	_my_txt_fstream(const char *_Filename, ios_base::openmode _Mode = ios_base::in | ios_base::out,
//		int _Prot = (int)ios_base::_Openprot):fstream(_Filename,_Mode,_Prot) {}
//protected:
//	_CSimpleVerticesMapsSet m_MapsSet;
//};

//****************************_CSimilarVertMaps
//class _CSimilarVertMaps : public set<pair<_CSubCircuitVerticesMap*,unsigned>>
//{
//public:
//	typedef pair<_CSubCircuitVerticesMap*,unsigned> PAIR;
//	void Add(_CSubCircuitVerticesMap* pMap, unsigned AuxId);
//	bool FindProperMap(const string* pContext, long long  lTerminalVertexToCheck,
//		const _CSimpleVertexContainer* pExpectedVertex,
//		unsigned& OldAuxId);
//};
//****************************_CSimilarVertMaps

//class _CSimilarVertMapsStack : public list<_CSimilarVertMaps*>
//{
//public:
//	~_CSimilarVertMapsStack();
//	void push_back_new();
//	bool pop_back_and_destroy();
//	bool pop_back_and_store(_CSimilarVertMaps& VMToStore);
//	void push_back_and_copy(const _CSimilarVertMaps* pSource);
//	_CSimilarVertMaps* smart_back();
//};
