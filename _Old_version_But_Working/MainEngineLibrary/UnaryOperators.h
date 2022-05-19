#pragma once
#include "StdAfx.h"
//#include <string>
//#include "Cofactor.h"
#include "Vertex.h"
//#include "SubCircuit.h"
//#include "SubCircuitSocket.h
#include <unordered_map>
#include "ComponentPath.h"
#include "CSimpleVertexContainer.h"
#include "NumericalResTables.h"
#include "norm_float.h"
#include "UniqStringBuffer.h"
using namespace stdext;


template<class ComponentPath>
class _CAbstractComplementStore 
{
public:
	_CAbstractComplementStore():m_pBaseCircuit(NULL) {}
	void SetBaseCircuit(_CMainCircuit* pBaseCircuit) {m_pBaseCircuit=pBaseCircuit;}
	ComponentPath& GetPath(const string& strPath=EmptyString)
	{
		iterator fit=m_Store.find(strPath);
		if(fit!=m_Store.end())
			return fit->second;
		ComponentPath NewPath(m_pBaseCircuit);
		pair<iterator,bool> res=m_Store.insert(value_type(strPath,NewPath));
		if(!res.second)
			RISEPDD(eIllegalOperation,"ComponentPath::GePath - the problem with map");
		fit=res.first;
		ComponentPath& ResPath=fit->second;
		try
		{
			if(!strPath.empty())
				ResPath.ParsePath(m_pBaseCircuit,strPath);
			return ResPath;
		}
		catch(exception)
		{
			m_Store.erase(fit);
			throw;
		}
	}

protected:
	typedef unordered_map<string,ComponentPath> STORE;
	typedef typename STORE::iterator iterator;
	typedef typename STORE::value_type value_type;
	STORE m_Store;
	_CMainCircuit* m_pBaseCircuit;
};

typedef _CAbstractComplementStore<_CConstComponentPath> _CConstComplementStore;
typedef _CAbstractComplementStore<_CComponentPath> _CComplementStore;
//class _CConstComplementStore:public _CAbstractComplementStore<_CConstComponentPath>
//{
//};
//class _CConstComplementStore 
//{
//public:
//	~_CConstComplementStore()
//	{
//	}
//	_CConstComplementStore():m_pBaseCircuit(NULL) {}
//	void SetBaseCircuit(_CMainCircuit* pBaseCircuit) {m_pBaseCircuit=pBaseCircuit;}
//	_CConstComponentPath& GetConstantPath(const string& strPath=EmptyString);
//protected:
//	typedef unordered_map<string,_CConstComponentPath> STORE;
//	typedef STORE::iterator iterator;
//	typedef STORE::value_type value_type;
//	unordered_map<string,_CConstComponentPath> m_Store;
//	_CMainCircuit* m_pBaseCircuit;
//};
class _CRepeatContainer
{
public:
	_CRepeatContainer(const string* ValuePath=NULL,const _CRepeatContainer* Res1=NULL,
		const _CRepeatContainer* Res0=NULL,short Multiplier=1, 
		short Multiplier1=1,short Multiplier0=1, unsigned long Index=0):
	m_Index(Index),m_pValuePath(ValuePath),m_Res1(Res1),m_Res0(Res0),m_Multiplier(Multiplier),
		m_0Multiplier(Multiplier0), m_1Multiplier(Multiplier1) {}

	_CRepeatContainer(const _CRepeatContainer& Source):
	m_Index(Source.m_Index),m_pValuePath(Source.m_pValuePath),
		m_Res1(Source.m_Res1),m_Res0(Source.m_Res0),m_Multiplier(Source.m_Multiplier),
		m_0Multiplier(Source.m_0Multiplier), m_1Multiplier(Source.m_1Multiplier) {}
	
	unsigned long long GetHash() const;
	unsigned long GetIndex() const {return m_Index;}
	const _CRepeatContainer& operator=(const _CRepeatContainer& Source);
	inline bool operator==(const _CRepeatContainer& Right)
	{
		return m_pValuePath==Right.m_pValuePath 
			&& m_Res1==Right.m_Res1 && 
			m_Res0==Right.m_Res0 && 
			m_Multiplier==Right.m_Multiplier &&
			m_1Multiplier==Right.m_1Multiplier &&
			m_0Multiplier==Right.m_0Multiplier;
	}
	string GetNodeName() const;
	string Get0NodeName() const;
	string Get1NodeName() const;
	void SetIndex(unsigned long Index) {m_Index=Index;}
	void SetMultiplier(short Multiplier) {m_Multiplier=Multiplier;}
protected:
	string GetMainNodeName() const;
	const string* m_pValuePath;
	const _CRepeatContainer* m_Res1;
	const _CRepeatContainer* m_Res0;
	short m_Multiplier;
	short m_0Multiplier;
	short m_1Multiplier;
	unsigned long m_Index;
};

extern _CRepeatContainer* EmptyRepeatContainer;

//class _CVertex2TextOperator : public _CUnaryAbstractOperator<string,&EmptyString>
class _CVertex2TextOperator : public _CUnaryAbstractOperator<_CRepeatContainer*,&EmptyRepeatContainer>
{
public:
	typedef _CRepeatContainer* MyType;
	_CVertex2TextOperator(_CMainCircuit* pMainCircuit,long sPower, iostream& stream,  const string& Context):m_ClearPaths(/*0*/), m_RepeatCache(m_ClearPaths),
	  _CUnaryAbstractOperator(pMainCircuit,sPower,Context),m_stream(stream),m_pOperationContext(&Context),m_RepCounter(0) 
	{
	}
	//_CVertex2TextOperator(const _CVertex2TextOperator& Source):
	//  _CUnaryAbstractOperator<string,&EmptyString>(Source), m_stream(Source.m_stream),
	//	  m_pOperationContext(Source.m_pOperationContext),m_RepCounter(0) {}
	~_CVertex2TextOperator();
	virtual MyType DoTheOperation(const _CVertex& Vertex, 
		const _CSimpleVertexContainer& SVertex, const MyType& Res1, const MyType& Res0);
	//virtual string DoTheOperation(const _CVertex& Vertex, 
	//	const _CSimpleVertexContainer& SVertex, const string& Res1, const string& Res0);
	//virtual void FinishDoTheOperation(string& Result);
	virtual void ModifyByMultiplier(MyType& Result, short Multiplier);
	//virtual void ModifyByMultiplier(string& Result, short Multiplier);
	virtual MyType DoThe01Operation(const _CSimpleVertexContainer& Vertex);
	//virtual string DoThe01Operation(const _CSimpleVertexContainer& Vertex);
	//void PostOperateSubCircuitResult(size_t EntryId, size_t PosId, string& NewRes) {}
	//virtual const string& GetBaseContext() {return m_ContextStr;}
	virtual const string& GetOperationContext() {return *m_pOperationContext;}
	//void Mult0Str(string& Str) {Mult2Str(Str,m_0Multiplier);}
	//void Mult1Str(string& Str) {Mult2Str(Str,m_1Multiplier);}
	//void MultGStr(string& Str,const _CSimpleVertexContainer& MainVertex)
	//{Mult2Str(Str,MainVertex.GetLocalMultiplier());}
protected:
	class _CUniqPaths : public _CUniqStringBufferWithCounter
	{
	public:
		_CUniqPaths(/*int Void*/):m_0Str("0"), m_1Str("1"), m_M1Str("-1") {}
		const string* Get0Str() const {return &m_0Str;}
		const string* Get1Str() const {return &m_1Str;}
		const string* GetM1Str() const {return &m_M1Str;}
	protected:
		string m_0Str;
		string m_1Str;
		string m_M1Str;
	};
	class _CRepeatCache : unordered_multimap<unsigned long long,_CRepeatContainer>
	{
	public:
		_CRepeatCache(const _CUniqPaths& InitPaths):m_0Desc(InitPaths.Get0Str()),
			m_1Desc(InitPaths.Get1Str()), m_M1Desc(InitPaths.GetM1Str())
		{
		}
		_CRepeatContainer* Register(const _CRepeatContainer& Source);
		bool FindExisted(_CRepeatContainer** ppSource);
		_CRepeatContainer* Get0Desc()  {return &m_0Desc;}
		_CRepeatContainer* Get1Desc()  {return &m_1Desc;}
		_CRepeatContainer* GetM1Desc()  {return &m_M1Desc;}
	protected:
		_CRepeatContainer m_0Desc;	
		_CRepeatContainer m_1Desc;	
		_CRepeatContainer m_M1Desc;
	};
	//void WritePrefix();
	//void Mult2Str(string& Str, short Multilpier);
	const string& GetInstancePrefix() {return m_CurrentPath.GetPath();}
	const string* m_pOperationContext;
	//string m_OutStr;
	//string m_sMult;
	iostream& m_stream;
	unsigned m_RepCounter;
	_CUniqPaths m_ClearPaths;
	_CRepeatCache m_RepeatCache;
};

//class _CSPowerCoeffOperator : public _CUnaryAbstractOperator<NumericType,&EmptyFloat,false>
//{
//public:
//	~_CSPowerCoeffOperator()
//	{
//	}
//	_CSPowerCoeffOperator(_CMainCircuit* pMainCircuit, long sPower, const string& BaseContext=EmptyString,_CResCache* pExternalCache=NULL):
//	  _CUnaryAbstractOperator<NumericType,&EmptyFloat,false>(pMainCircuit,sPower,BaseContext,pExternalCache) {}
//	_CSPowerCoeffOperator(const _CSPowerCoeffOperator& Source):
//	  _CUnaryAbstractOperator<NumericType, &EmptyFloat,false>(Source) {}
//	virtual NumericType DoTheOperation(const _CVertex& Vertex, 
//		const _CSimpleVertexContainer& SVertex, const NumericType& Res1, 
//		const NumericType& Res0);
//	virtual NumericType DoThe01Operation(const _CSimpleVertexContainer& Vertex);
//	//void PostOperateSubCircuitResult(size_t EntryId, size_t PosId, long double& NewRes) {}
//	virtual void ModifyByMultiplier(NumericType& Result, short Multiplier) 
//	{
//		if(Multiplier!=1)
//			Result*=Multiplier;
//	}
//};

//typedef pair<bool,long double> _CNumericalSEntry;
//
//class _CNumericalSCoeff : public vector<_CNumericalSEntry> 
//{
//public:
//	bool IsSCoeffDefined(unsigned Index) {return Index>=size()?false:at(Index).first;}
//	bool GetSCoeff(unsigned Index, long double& Value);
//	void SetSCoeff(unsigned Index, const long double& Value);
//};
//
//class _CNumericalSCofactors : public vector<_CNumericalSCoeff>
//{
//public:
//	void Reset(size_type NewSize) {resize(0);resize(NewSize);}
//};


class _CCofactorReminderSuplement : public _CSuplementParameters
{
public:
	~_CCofactorReminderSuplement()
	{
	}
	_CCofactorReminderSuplement(_CConstComponentPath& SourcePath, bool IsCofactor,const string& BaseContext):
	  m_CurrentPath(SourcePath),m_Cofactor(IsCofactor),m_BaseContext(BaseContext) {}
	_CCofactorReminderSuplement(const _CCofactorReminderSuplement& Source): _CSuplementParameters(Source),
		m_CurrentPath(Source.m_CurrentPath), m_Cofactor(Source.m_Cofactor),m_BaseContext(Source.m_BaseContext) {}
	operator size_t() const 
	{
		size_t Res=HASH_CORE;
		Res*=HASH_FACTOR;
		Res^=m_CurrentPath.GetHash();
		Res*=HASH_FACTOR;
		Res^=(size_t)m_Cofactor;
		Res*=HASH_FACTOR;
		Res^=hash_value(m_BaseContext);
		return Res;
	}
	const _CConstComponentPath& GetPath() const {return m_CurrentPath;}
protected:
	_CConstComponentPath& m_CurrentPath;
	const string& m_BaseContext;
	bool m_Cofactor;
};

class _CSuplementStorage
{
public:
	~_CSuplementStorage();
	const _CCofactorReminderSuplement& GetCofRemSuplement(_CConstComponentPath& SourcePath, bool IsCofactor,const string& BaseContext);
protected:
	typedef unordered_map<string, _CSuplementParameters*> SUPLEMENTS_MAP;
	typedef SUPLEMENTS_MAP::value_type value_type;
	typedef SUPLEMENTS_MAP::iterator iterator;
	SUPLEMENTS_MAP m_Map;
};


//class _CNumericalCofactorReminderOperator : public _CSPowerCoeffOperator/*, public _COperatorWithSuplement<_CCofactorReminderSuplement>*/
//{
//public:
//	~_CNumericalCofactorReminderOperator()
//	{
//	}
//	_CNumericalCofactorReminderOperator(_CMainCircuit* pMainCircuit,long sPower, const _CCofactorReminderSuplement& Suplement, 
//		const string& OperationContext, const string& BaseContext=EmptyString,_CResCache* pExternalCache=NULL):
//		_CSPowerCoeffOperator(pMainCircuit,sPower,BaseContext,pExternalCache),
//		  m_Suplement(Suplement), m_OperationContext(OperationContext)/*,*/
//				/*m_CurrentPath(Suplement.GetPath().GetBaseCircuit())*/ {}
//	_CNumericalCofactorReminderOperator(const _CNumericalCofactorReminderOperator& Source):
//		_CSPowerCoeffOperator(Source), m_OperationContext(Source.m_OperationContext),
//			  m_Suplement(Source.m_Suplement)/*, */
//				/*m_CurrentPath(Source.m_CurrentPath)*/ {}
//	//virtual _CSuplementParameters* CreateSuplementParameters() {return new _CCofactorReminderSuplement(m_CurrentPath);}
//	//virtual void PushState(_CSimpleVerticesMaps* pTerminalVertices,_CSubCircuitSocket* pSocket);
//	//virtual void PopState();
//	virtual const _CSuplementParameters* GetSuplementParameters() const {return &m_Suplement;}
//	virtual const string& GetOperationContext() {return m_OperationContext;}
//	short CompVertex(const _CVertex& Vertex,const _CSimpleVertexContainer& SVertex);
//protected:
//	const _CCofactorReminderSuplement& m_Suplement;
//	//_CConstComponentPath m_CurrentPath;
//	const string& m_OperationContext;
//	//_CComponentPath& m_CurrentPath;
//};

//class _CNumericalCofactorOperator : public _CNumericalCofactorReminderOperator
//{
//public:
//	~_CNumericalCofactorOperator()
//	{
//	}
//	_CNumericalCofactorOperator(_CMainCircuit* pMainCircuit,long sPower,const _CCofactorReminderSuplement& Suplement, 
//		const string& OperationContext, const string& BaseContext=EmptyString,_CResCache* pExternalCache=NULL):
//		_CNumericalCofactorReminderOperator(pMainCircuit,sPower,Suplement,OperationContext,BaseContext,pExternalCache) {}
//	_CNumericalCofactorOperator(const _CNumericalCofactorOperator& Source):
//	  _CNumericalCofactorReminderOperator(Source) {}	
//	//virtual NumericType Operate(const _CVertex& Vertex, 
//	//	const _CSimpleVertexContainer& SVertex,_CAuxBasePlatorm* pAuxBasePlatform);
//	virtual NumericType Operate(const _CVertex& Vertex,const _CSimpleVertexContainer& SVertex);
//	virtual NumericType DoThe01Operation(const _CSimpleVertexContainer& Vertex) {return 0.0;}
//};
//
//class _CNumericalReminderOperator : public _CNumericalCofactorReminderOperator
//{
//public:
//	~_CNumericalReminderOperator()
//	{
//	}
//	_CNumericalReminderOperator(_CMainCircuit* pMainCircuit,long sPower,const _CCofactorReminderSuplement& Suplement, 
//		const string& OperationContext,const string& BaseContext=EmptyString,_CResCache* pExternalCache=NULL):
//	  _CNumericalCofactorReminderOperator(pMainCircuit,sPower,Suplement,OperationContext,BaseContext,pExternalCache) {}
//	_CNumericalReminderOperator(const _CNumericalReminderOperator& Source):
//	  _CNumericalCofactorReminderOperator(Source) {}
//	//virtual NumericType Operate(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex,
//	//	_CAuxBasePlatorm* pAuxBasePlatform);
//	virtual NumericType Operate(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex);
//};


class _CResVertexOperator : public _CUnaryAbstractOperator< _CSimpleVertexContainer,&EmptyVertex>
{
public:
	~_CResVertexOperator()
	{
	}
	_CResVertexOperator(_CMainCircuit* pMainCircuit, long sPower, const string& CurrentContext,
		const string& BaseContext,_CResCache* pExternalCache=NULL):
		_CUnaryAbstractOperator<_CSimpleVertexContainer,&EmptyVertex>(pMainCircuit,sPower,BaseContext,pExternalCache),
			m_pResContextString(&CurrentContext),m_Stage(Normal){}
	_CResVertexOperator(const _CResVertexOperator& Source):
	  _CUnaryAbstractOperator<_CSimpleVertexContainer,&EmptyVertex>(Source),
		  m_pResContextString(Source.m_pResContextString),m_Stage(Source.m_Stage) {}
	virtual void PostOperateSubCircuitResult(size_t EntryId, size_t PosId, _CSimpleVertexContainer& NewRes);
	virtual void PostOperateTerminals(long long VertId, _CSimpleVertexContainer& NewRes);
	virtual _CSimpleVertexContainer DoTheOperation(const _CVertex& Vertex, 
		const _CSimpleVertexContainer& SVertex, const _CSimpleVertexContainer& Res1, 
		const _CSimpleVertexContainer& Res0);
	//const string& GetCurrentOperationContext() {return m_ResContextString;}
	virtual _CSimpleVertexContainer Operate(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex);
	//virtual _CSimpleVertexContainer Operate(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex,_CAuxBasePlatorm* pAuxBasePlatform);
	virtual const string& GetOperationContext() {return *m_pResContextString;}
	virtual bool AutoPredictResult(const _CSimpleVertexContainer& Successor,_CSimpleVertexContainer& Result)
	{
		if(m_Stage==Normal)
			return false;
		Result=Successor;
		//m_pPreviousRealSimpleVertex=&Successor;
		return true;
	}
	virtual bool CacheResult() {return m_Stage==Normal;}
	const _CSimpleVertexContainer& FixSubTree(const _CSimpleVertexContainer& SubTree,bool Is0Descendant)
	{
		m_Stage=Enumeration;
		SubTree.DoUnaryOpertationForWholeTree(*this,Is0Descendant);
		m_Stage=Normal;
		return SubTree;
	}
	void ChangeResultOperationContext(const string& NewContext) {m_pResContextString=&NewContext;}
protected:
	const string& GetPostTerminalDescendantContext()
	{
		if(m_Stage==Normal)
			return *m_pResContextString;
		else if(m_Stage==Enumeration)
			return *m_pBaseContext;
		RISEPDD(eUnexpectedCondition,"Upss!!");
	}
	typedef enum{Normal,Enumeration/*,Cloning*/} OPERATION_STAGE;
	OPERATION_STAGE m_Stage;
	const string* m_pResContextString;
};

class _CVertexCofactorReminderOperator : public _CResVertexOperator
{
public:
	~_CVertexCofactorReminderOperator() {}
	_CVertexCofactorReminderOperator(_CMainCircuit* pMainCircuit,long sPower,const _CCofactorReminderSuplement& Suplement,
		const string& CurrentContext,const string& BaseContext,_CResCache* pExternalCache=NULL):
		_CResVertexOperator(pMainCircuit,sPower,CurrentContext,BaseContext,pExternalCache),
		  m_Suplement(Suplement){}
	_CVertexCofactorReminderOperator(const _CVertexCofactorReminderOperator& Source):
	  _CResVertexOperator(Source), m_Suplement(Source.m_Suplement){}
	virtual const _CSuplementParameters* GetSuplementParameters() const {return &m_Suplement;}
//	virtual bool SkipMapDifference() {return true;}
protected:
	short CompVertex(const _CVertex& Vertex,const _CSimpleVertexContainer& SVertex);
	const _CCofactorReminderSuplement& m_Suplement;
};
class _CVertexCofactorOperator : public _CVertexCofactorReminderOperator
{
public:
	~_CVertexCofactorOperator()
	{
	}
	_CVertexCofactorOperator(_CMainCircuit* pMainCircuit,long sPower,const _CCofactorReminderSuplement& Suplement,
		const string& CurrntContext,const string& BaseContext,_CResCache* pExternalCache=NULL):
	 _CVertexCofactorReminderOperator(pMainCircuit,sPower, Suplement,CurrntContext,BaseContext,pExternalCache) {}
	virtual _CSimpleVertexContainer Operate(const _CVertex& Vertex, 
		const _CSimpleVertexContainer& SVertex);
	//virtual _CSimpleVertexContainer Operate(const _CVertex& Vertex, 
	//	const _CSimpleVertexContainer& SVertex,
	//	_CAuxBasePlatorm* pAuxBasePlatform);
	virtual _CSimpleVertexContainer DoThe01Operation(const _CSimpleVertexContainer& Vertex);
};

class _CVertexReminderOperator : public _CVertexCofactorReminderOperator
{
public:
	~_CVertexReminderOperator()
	{
	}
	_CVertexReminderOperator(_CMainCircuit* pMainCircuit,long sPower,const _CCofactorReminderSuplement& Suplement,
		const string& CurrntContext,const string& BaseContext,_CResCache* pExternalCache=NULL):
	_CVertexCofactorReminderOperator(pMainCircuit,sPower, Suplement,CurrntContext,BaseContext,pExternalCache) {}
	virtual _CSimpleVertexContainer Operate(const _CVertex& Vertex, 
		const _CSimpleVertexContainer& SVertex);
	//virtual _CSimpleVertexContainer Operate(const _CVertex& Vertex, 
	//	const _CSimpleVertexContainer& SVertex,
	//	_CAuxBasePlatorm* pAuxBasePlatform);
	virtual _CSimpleVertexContainer DoThe01Operation(const _CSimpleVertexContainer& Vertex);
};

class _CLSPath : public list<_CConstComponentPath>
{
public:
	~_CLSPath()
	{
	}
	void ToVectorString(VECTOR_STRING& VString);
	void ToOneString(string& StrValue);
	//_CLSPath(_CMainCircuit* pBaseCircuit):m_pBaseCircuit(pBaseCircuit) {}
protected:
	//_CMainCircuit* m_pBaseCircuit;
};

//class _CWeightResult
//{
//public:
//	~_CWeightResult() {}
//	_CWeightResult():m_Value(0.0), m_Direction(None)/*,m_pVertex(NULL),m_pSVertex(NULL),m_pSocket(NULL)*/ {}
//	_CWeightResult(NumericType Value):m_Value(Value),m_Direction(Desc1)/*,m_pVertex(NULL),m_pSVertex(NULL),m_pSocket(NULL) */{}
//	_CWeightResult(const _CVertex* pVertex, const _CSimpleVertexContainer* pContainer, const _CSubCircuitSocket* pSocket,NumericType ldDesc1, NumericType ldDesc0);
//	_CWeightResult(const _CWeightResult& Source):m_Value(Source.m_Value),
//		/*m_pVertex(Source.m_pVertex),m_pSVertex(Source.m_pSVertex),*/m_Direction(Source.m_Direction)/*,m_pSocket(Source.m_pSocket)*/ {}
//	_CWeightResult& operator=(const _CWeightResult& Right)
//	{
//		m_Value=Right.m_Value;
//		//m_pVertex=Right.m_pVertex;
//		//m_pSVertex=Right.m_pSVertex;
//		m_Direction=Right.m_Direction;
//		//m_pSocket=Right.m_pSocket;
//		return *this;
//	}
//	//bool IsTerminal() {return m_pSVertex==NULL && m_Direction==Desc1;}
//	NumericType GetValue() const {return m_Value;}
//	bool IsEmpty() const {return m_Direction==None;}
//	void ClearAll() {m_Value=0.0; m_Direction=None; /*m_pVertex=NULL;m_pSVertex=NULL;m_pSocket=NULL;*/}
//	typedef enum{None,Desc0,Desc1} DIRECTION;
//	long long GetHash(long long& Core) const
//	{
//		Core^=GetTypedHash(Core,m_Value);
//		Core*=HASH_FACTOR;
//		Core^=(long long)m_Direction;
//		return Core;
//	}
//	bool operator==(const _CWeightResult& Right) const
//	{
//		return m_Value==Right.m_Value && m_Direction==Right.m_Direction;
//	}
//protected:
//	NumericType m_Value;
//	DIRECTION m_Direction;
//	//const _CVertex* m_pVertex;
//	//const _CSimpleVertexContainer* m_pSVertex;
//	//const _CSubCircuitSocket* m_pSocket;
//public:
//	DIRECTION GetDirection() const {return m_Direction;}
//};
//
//extern _CWeightResult EmptyWeightResult;
////bool operator==(const _CWeightResult& Left,const _CWeightResult& Right);
//long long GetTypedHash(long long& Core, const _CWeightResult& WR);

//class _CPathWeightOperator : public _CUnaryAbstractOperator<_CWeightResult,&EmptyWeightResult>
//{
//public:
//	~_CPathWeightOperator()
//	{
//	}
//	_CPathWeightOperator(_CMainCircuit* pMainCircuit, long sPower, const string& BaseContext,_CResCache* pExternalCache):
//	  _CUnaryAbstractOperator<_CWeightResult,&EmptyWeightResult>(pMainCircuit,sPower, BaseContext,pExternalCache){}
//	virtual _CWeightResult DoTheOperation(const _CVertex& Vertex, 
//		const _CSimpleVertexContainer& SVertex, const _CWeightResult& Res1, 
//		const _CWeightResult& Res0);
//	virtual _CWeightResult DoThe01Operation(const _CSimpleVertexContainer& Vertex);
//};
//
//class _CLeastSgnfcntPathOperator : public _CUnaryAbstractOperator<NumericType,&EmptyFloat>
//{
//public:
//	~_CLeastSgnfcntPathOperator()
//	{
//	}
//	typedef NumericType RES_TYPE;
//	_CLeastSgnfcntPathOperator(_CMainCircuit* pMainCircuit, long sPower, const string& BaseContext,
//		_CPathWeightOperator::_CResCache& Weights, _CLSPath& PathResult):
//	  _CUnaryAbstractOperator<RES_TYPE,&EmptyFloat>(pMainCircuit,sPower,BaseContext,NULL,true),
//		  m_Weights(Weights),m_PathResult(PathResult) {}
//	virtual RES_TYPE DoTheOperation(const _CVertex& Vertex, 
//		const _CSimpleVertexContainer& SVertex, const RES_TYPE& Res1, 
//		const RES_TYPE& Res0) {return NULL;}
//	virtual RES_TYPE DoThe01Operation(const _CSimpleVertexContainer& Vertex) {return 0.0;}
//	virtual RES_TYPE Operate(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex);
//	//virtual RES_TYPE Operate(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex,
//	//	_CAuxBasePlatorm* pAuxBasePlatform);
//protected:
//	typedef _CPathWeightOperator::_CNResContainer _CRES_CONTAINER;
//	_CPathWeightOperator::_CResCache& m_Weights;
//	_CLSPath& m_PathResult;
//};


class _CPathRemoveOperator : public _CResVertexOperator
{
public:
	~_CPathRemoveOperator()
	{
	}
	_CPathRemoveOperator(_CMainCircuit* pMainCircuit,long sPower, _CLSPath& RemPath,
		const string& CurrntContext,const string& BaseContext,_CResCache* pExternalCache=NULL):
			_CResVertexOperator(pMainCircuit,sPower,CurrntContext,BaseContext,pExternalCache),m_RemPath(RemPath) 
			{
				m_RemPathIterator=m_RemPath.begin();
			}
	_CPathRemoveOperator(const _CPathRemoveOperator& Source):
			_CResVertexOperator(Source), m_RemPath(Source.m_RemPath) {}
	virtual _CSimpleVertexContainer Operate(const _CVertex& Vertex, 
		const _CSimpleVertexContainer& SVertex);
	//virtual _CSimpleVertexContainer Operate(const _CVertex& Vertex, 
	//	const _CSimpleVertexContainer& SVertex,
	//	_CAuxBasePlatorm* pAuxBasePlatform);
	virtual _CSimpleVertexContainer DoThe01Operation(const _CSimpleVertexContainer& Vertex) {return ZeroSVertex;}
	void RestartPath() {m_RemPathIterator=m_RemPath.begin();}
protected:
	short CompVertex(const _CVertex& Vertex,const _CSimpleVertexContainer& SVertex);
	_CLSPath& m_RemPath;
	_CLSPath::iterator m_RemPathIterator;
};