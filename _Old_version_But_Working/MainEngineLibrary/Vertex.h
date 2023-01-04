#pragma once
#include "StdAfx.h"
#include "InUseCheckpoint.h"
#include "Components.h"
#include "CacheTmpl.h"
#include "binary_filer.h"
#include "StateGraphTables.h"
#include "NodesManager.h"
#include "CSimpleVertexContainer.h"
#include "norm_float.h"
#include "FlatVertex.h"
#include "RedefParamValues.h"
//#include "DirectNumCache.h"
//#include "PathTraitor.h"

class _CCircuit;
class _CDeletions;
class _CSubCircuit;
class _CPathTraitor;

class _CAbstractVertex
{
public:
	_CAbstractVertex(_CCircuit* Owner=NULL):m_OwnerCircuit(Owner) {}
	~_CAbstractVertex() {}
	virtual bool IsTerminal() const=0;
	virtual short GetTerminalValue() const=0;
	virtual bool Is0() const=0;
	virtual bool Is1() const=0;
	inline _CAbstractVertex& operator=(const _CAbstractVertex& Source)
	{
		Copy(Source);
		return *this;
	}
	bool operator==(const _CAbstractVertex& Operand) {return IsEqual(Operand);}
protected:
	virtual bool IsEqual(const _CAbstractVertex& Operand)=0;
	virtual void Copy(const _CAbstractVertex& Source)=0;
	_CCircuit* m_OwnerCircuit;
};

class _CGenericVertex : public _CAbstractVertex, public _CInUseCheckpoint, public _CHashable
{
public:
	_CGenericVertex(_CCircuit* Owner=NULL):_CAbstractVertex(Owner) {}
	~_CGenericVertex()
	{
	}
protected:
};

//class _CTerminalVertex : public _CGenericVertex
//{
//public:
//	_CTerminalVertex(short TermValue): _CGenericVertex(NULL), m_TermValue(TermValue) {}
//	virtual bool IsTerminal() const {return true;}
//	virtual short GetTerminalValue() const {return m_TermValue;}
//	virtual bool Is0() const {return !m_TermValue==0;}
//	virtual bool Is1() const {return m_TermValue!=0;}
//	virtual bool IsUseless() {return false;}
//	virtual void TouchMe() {}
//	virtual void UnTouchMe() {}
//protected:
//	virtual long DetermineHashKey() {return m_TermValue;}
//	virtual bool IsEqual(const _CAbstractVertex& Operand);
//	virtual void Copy(const _CAbstractVertex& Source)
//	{
//		m_TermValue=dynamic_cast<const _CTerminalVertex*>(&Source)->m_TermValue;
//	}
//	short m_TermValue;
//};


class _CDescSigns : public vector<short>
{
public:
	_CDescSigns(short N=0):vector<short>(N,1) {}
	short ExtractCommonFactor();
protected:
	bool IsZero();
	bool IsDevidedBy2();
	void DevideBy2();
	bool IsEachNegative();
	void ChangeSgn();
};


class _CVertex;
class _CDuplicatedVertexCatcher;
class _CModelVertex;
class _CNewSimplifierData;
class _CDirectNumContainer;
class _CPreFlatVertexContainer;
class _CDirNumContainers;
class _CDescendantContainer
{
public:
	_CDescendantContainer():m_Multiplier(0), m_pDescendant(NULL) {}
	short m_Multiplier;
	_CModelVertex* m_pDescendant;
};


class _CModelVertex : public _CInUseCheckpoint, public _CHashable, public _streamable
{
public:
	_CModelVertex():/*m_NoOfDescendants(0),*/m_OwnerCircuit(NULL),m_pComponent(NULL), 
		/*m_pDescendants(NULL),*/m_pInputState(NULL),m_WasGarbageCleaned(false)/*,m_OptionalMultiplier(0)*/ {}
	_CModelVertex(_CGraphState* pInputState,_CCircuit* Owner,bool ForceNewStateCopy=false);
	_CModelVertex(const _CModelVertex& Source, _CCircuit* Owner=NULL);
	_CModelVertex(RES_STATE ForcedState);
	_CModelVertex(const _CModelVertex& Source,unsigned NoOfBoundaryNodes);
	~_CModelVertex();
//	void AddComponent(const _CComponent& Component, int ParamId=0);
	_CComponentPredicator TestComponent(const _CComponent& Component);
	short GetTerminalValue() const;
	bool Is0() const;
	bool Is1() const;
	bool IsM1() const;
	bool IsTerminal()const ;
	bool IsDegenerated(int& HollowOutput) const;
	bool IsEmptyMacro() const;
	bool IsLeaf() const;
	void DefineBoundaryParam(unsigned NoOfBoundaries);
	_declspec(property(get=GetInputState, put=SetInputState)) _CGraphState* pInputState;
//	_declspec(property(get=GetP1Succ, put=SetP1Succ)) _CModelVertex* p1Succ;
//	_declspec(property(get=GetP0Succ, put=SetP0Succ)) _CModelVertex* p0Succ;
	void SetInputState(_CGraphState* pState);
	inline _CGraphState* GetInputState() const {return m_pInputState;}
//	void SetP1Succ(_CModelVertex* P1Succ);
//	inline _CModelVertex* GetP1Succ() const {return m_p1Succ;}
//	void SetP0Succ(_CModelVertex* P0Succ);
//	inline _CModelVertex* GetP0Succ() const {return m_p0Succ;}
	void SetDescendant(unsigned Index,_CModelVertex* pVertex,short Multiplier);
	void ReSetDescendant(unsigned Index,_CModelVertex* pVertex);
	_CModelVertex* GetDescendant(unsigned Index) const;
	short GetMultiplier(unsigned Index);
//	_declspec(property(get=GetDescendent, put=SetDescendent)) _CModelVertex*[unsigned] pDescendents;
	void DestroyIfUnregistered()
	{
		if(!m_IsRegistered)
			delete this;
	}
	bool operator==(_CModelVertex& Operand) {return IsEqual(Operand);}
//	void ConnectDescendent(unsigned Index,_CModelVertex* pDescendent);
	void AssignComponent(_CComponent* pComponent);
	void RegisterMembers();
	void SetOwnerCircuit(_CCircuit* pOwner);
	void ClearStateIfPossible();
//	void Skip0Suppressed(bool CutLeaves);
	void Skip0Suppressed();
//	void Vertex2Deletions(short& Multiplier, _CDeletions& Deletions, const _CIntNodes& BoundaryNodes);
	_CIntegrityTable* GetIntegrityTable() const {return m_pInputState==NULL?NULL:m_pInputState->GetIntegrityTable();}
	_CGraphTable* GetGraphTable() const {return m_pInputState==NULL?NULL:m_pInputState->GetGraphTable();}
//	_CSimpleVertexContainer GetSimpleVertex(unsigned CofId, const _CSimpleVertexVector& TerminalVertices);
	void GetSimpleVertex(_CSExpandedVertices*& pSEVertices,unsigned CofId, const string& SocketName, _CSExpandedDescendends& TerminalVertices,
		short& TopSgn, const _CDescSigns& BottomSgns,bool Approximated, const NumericType& Accuracy);
	//void PrepareDirectNumericValue(_CSPolynomial& ResPolynomial,/*unsigned CofId, const string& SocketName,*/ _CModelNumericPattern& NumericInputPatern,
	//void GetSimpleVertexApprox(_CSExpFlatVertices*& pOutResVertices, _CFlatVerticesSPowerLimiter& Limiter,_CLeafsSExpVertices& TerminalVertices, short& TopSgn, const _CDescSigns& BottomSgns,_CPathTraitor& MainTraitor,_CModelVerticesTraitor& VerticesTraitor);
	//void GetSimpleVertexApprox(_CMultLevelSExpFlatVertices*& pOutResVertices, _CFlatVerticesSPowerLimiter& Limiter,_CLeafsSExpVertices& TerminalVertices, short& TopSgn, const _CDescSigns& BottomSgns,_CPathTraitor& MainTraitor,_CModelVerticesTraitor& VerticesTraitor,size_t Level=0);
	//void GetSimpleVertexApprox( _CSExpFlatVertices*& pOutResVertices, _CFlatVerticesResContainer*& pCacheContainer,_CSPowerLimiter& Limiter,_CLeafsSExpVertices& TerminalVertices, short& TopSgn, const _CDescSigns& BottomSgns,_CPathTraitor& MainTraitor,_CModelVerticesTraitor& VerticesTraitor,size_t Level=0);
	//void GetSimpleVertexApprox( _CMultLevelSExpFlatVertices*& pOutResVertices, 
	//	                        _CFlatVerticesResContainer*& pCacheContainer,
	//							_CFlatVerticesSPowerLimiter& Limiter,
	//							_CLeafsSExpVertices& TerminalVertices, 
	//							short& TopSgn, 
	//							const _CDescSigns& BottomSgns,
	//							_CPathTraitor& MainTraitor,
	//							_CModelVerticesTraitor& VerticesTraitor);
	//void GetSimpleVertexApprox( _CSExpFlatVertices*& pOutResVertices, 
	//	                        _CFlatVerticesResContainer*& pCacheContainer,
	//							const _CSparsePolynomial& TopNumValue,
	//							const _CSPowerLimiter& Limiter,
	//							const _CSubModelNumericPattern& Scaler,
	//							_CMultLevelSExpFlatVertices& TerminalVertices, 
	//							//short& TopSgn, 
	//							//const _CDescSigns& BottomSgns,
	//							_CPathTraitor& MainTraitor,
	//							//_CModelVerticesTraitor& VerticesTraitor);
	//							_CModelVerticesPath& VerticesPath);
	void GetSimpleVertexApprox2(const _CFlatVertex*& pResultVertex,
								short& ResultSgn,
								//const _CSparsePolynomial& TopNumValue,
								unsigned long Power,
								NumericType& AbsolutInaccuracyAllowed,  
								//const _CSubModelNumericPattern& Scaler,
								_CPathTraitor& MainTraitor,
								_CModelVerticesPath& VerticesPath,
								_CCompRedefEntry& RedefEntry);
	bool/*NewAdded*/ PerformNewSimplyfyApprox(_CPreFlatVertexContainer*& pResultVertex,
								  short& ResultSgn,
								  unsigned long Power,
								  _CNewSimplifierData& Data,
								  _CCompRedefEntry& RedefEntry);
	void PrepareDirectNumericValue(const _CSubModelNumericPattern*& pOutputPatern, _CDirectNumContainer*& pCacheContainer,const string* Context, 
		_CCompRedefEntry& RedefParamPtr);
	void PrepareDirectNumericalDivRem(const _CSubModelNumericPattern*& pOutputPatern, _CDirectNumContainer*& pCacheContainer, _CPathTraitor& Traitor,
		_CCompRedefEntry& RedefParamPtr);
	//void PrepareDirectNumericValue(const _CSubModelNumericPattern*& pOutputPatern, _CDirectNumContainer*& pCacheContainer, const string* Context,
	//	_CSubCircuitSocket* pMasterSocket = NULL);
	//void PrepareDirectNumericalDivRem(const _CSubModelNumericPattern*& pOutputPatern, _CDirectNumContainer*& pCacheContainer, _CPathTraitor& Traitor,
	//	_CSubCircuitSocket* pMasterSocket = NULL);
	//void DetermineNumericDescendants(_CDescModelNumericPattern& DescPattern, _CDescSigns2& Sgns,
	//	_CDirNumContainers& Locker, _CNewSimplifierData& Data);
	void DetermineNumericDescendants(_CDescModelNumericPattern& DescPattern, _CDescSigns2& Sgns,
		_CDirNumContainers& Locker, _CNewSimplifierData& Data, _CCompRedefEntry& RedefParamPtr);
	_CCircuit* GetOwnerCircuit(){ return m_OwnerCircuit; }
	void WriteToTextStream(iostream& stream);
	//void SkipDuplicated(_CDuplicatedVertexCatcher& Catcher);
	void Skip0SupressedAndDuplicated(_CDuplicatedVertexCatcher& Catcher);
	_CComponent* GetPComponent() const {return m_pComponent; }
	const string& GetAssociatedValueName() 
	{
		return m_pComponent==NULL? EmptyString:m_pComponent->GetParameterName();
	}
	bool IsEqualDsc(const _CModelVertex* pOperand) const;
	unsigned long long GetHashDsc() const;
	_CModelVertex* GetNextHollowVertex();
	long long GetMyId();
	//void SetMultiplier(short Multiplier);
	short DetermineCommonDivider();
	inline bool WasGarbageCleaned() {return m_WasGarbageCleaned;}
	bool IsMacroDependent() const {return !IsLeaf() && m_pComponent!=NULL && m_pComponent->IsMacroDependant();}
	_CDescendantContainer GetDescendantNo(size_t Index) const;
	size_t RealIndex(size_t Index) const
	{
		ASSERTPDD(m_pComponent->IsMacroDependant());
		return m_pComponent->RealIndex(Index);
	}
	void RealiseState();
	void RealiseIntTable();
	unsigned NoOfDescendants() const { return (unsigned)m_Descendants.size(); }
	void GetNumberOfOperations(size_t& NoOfMults, size_t& NoOfAdds, size_t& NoOfMultsNonOne);
protected:
	DECLARE_DYNAMIC_CREATION(_CModelVertex);
	void ClearDescendants();
	void ResizeDescendants(unsigned NewSize) 
	{ 
		m_Descendants.resize(NewSize); 
	}
	_CCircuit* m_OwnerCircuit;
//	_CModelVertex* GetNextNontube();
	virtual size_t DetermineHashKey(size_t Core=HASH_CORE) override;
	bool IsEqual( _CModelVertex& Operand);
	void Copy(const _CModelVertex& Source);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	_CComponent* m_pComponent;
	_CGraphState* m_pInputState;
//	typedef vector<_CModelVertex*> DESCENDANTS;
//	DESCENDANTS m_Descendants;
	//class _CDescendantContainer
	//{
	//public:
	//	_CDescendantContainer():m_Multiplier(0), m_pDescendant(NULL) {}
	//	short m_Multiplier;
	//	_CModelVertex* m_pDescendant;
	//};
	vector<_CDescendantContainer> m_Descendants;
	//unsigned short m_NoOfDescendants;
	//_CModelVertex* m_p1Succ;
	//_CModelVertex* m_p0Succ;
	friend class _CComponent;
	friend class _CCircuit;
	friend class _CSubCircuit;
	friend class _CModelVerticesCache;
	bool m_WasGarbageCleaned;
	friend class _CDuplicatedVertexCatcher2;
	//short m_OptionalMultiplier;
	bool DescDescription(iostream& stream, _CDescendantContainer& Desc);
};

typedef unordered_map<_CModelVertex*, short> MVERTEX_2_SIGN;

_CModelVertex* GetGlobal0Vertex();
_CModelVertex* GetGlobal1Vertex();
_CModelVertex* GetGlobalM1Vertex();

class _CModelVerticesCache : public _CStreamableCacheTmpl<_CModelVertex>
{
public:
	_CModelVerticesCache(_CCircuit* pOwnerCircuit=NULL):m_pOwnerCircuit(pOwnerCircuit),
		m_PosCurrentContinue(-1),m_StoredId(0)
	{
		m_StoreInReversOrder=true;
		m_Id=2;
	}
	~_CModelVerticesCache();
	void SetOwnerCircuit(_CCircuit* pNewOwner);
	void DoSomeExtras(_streamable* pMe);
	virtual void Store(_binary_filer& Filer);
	void AppendUnstored(_binary_filer& Filer);
	virtual void Load(_binary_filer& Filer);
protected:
	typedef _CStreamableCacheTmpl<_CModelVertex> PARENT_CLASS;
	DECLARE_DYNAMIC_CREATION(_CModelVerticesCache);
	long long m_PosCurrentContinue;
	long long m_StoredId;
	//void DoSomethingElse(_CModelVertex* pVertex);
private:
	_CCircuit* m_pOwnerCircuit;
};

class _CModelVerticesList : public list<_CModelVertex*>, public _streamable
{
public:
	~_CModelVerticesList()
	{
	}
	_CModelVerticesList():m_pOwnerCircuit(NULL) {}
	void SetOwner(_CCircuit* pOwner);
	void FreeStatestIfPosibble();
protected:
	DECLARE_DYNAMIC_CREATION(_CModelVerticesList);
	_CCircuit* m_pOwnerCircuit;
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
};
//template<class ClassToCatch>
//class _CDuplicationCatcher
//{
//public:
//	ClassToCatch* GetUnique(ClassToCatch* pVertex);
//protected:
//	typedef typename hash_multimap<unsigned long long,ClassToCatch*> VERTICES_MAP;
//	typedef typename VERTICES_MAP::value_type VERTICES_TYPE;
//	typedef typename VERTICES_MAP::iterator VERTICES_ITERATOR;
//	typedef pair<VERTICES_ITERATOR,VERTICES_ITERATOR> RANGE;
//	VERTICES_MAP m_MainMap;
//	bool AreEqual(const ClassToCatch* pLeft, const ClassToCatch* pRight);
//	unsigned long long GetHashKey(const ClassToCatch* pVertex);
//};

template<class ClassToCheck,class MY_TYPE>
class _CDuplicationCatcher
{
protected:
	typedef typename unordered_multimap<unsigned long long,ClassToCheck*> MAP;
	typedef typename MAP::value_type TYPE;
	typedef typename MAP::iterator ITERATOR;
	typedef typename pair<ITERATOR,ITERATOR> RANGE;
	MAP m_MainMap;
	//unsigned long long GetHashKey(const ClassToCheck* pObject) {return (unsigned long long)pObject;}
	//bool IsEqual(const ClassToCheck* pLeft, const ClassToCheck* pRight) {return (pLeft==pRight);}
	//bool IsOutOf(ClassToCheck* pObject) {return false;}
	_CCircuit& m_OwnerCircuit;
public:
	_CDuplicationCatcher(_CCircuit& OwnerCircuit):m_OwnerCircuit(OwnerCircuit) {}
	~_CDuplicationCatcher()
	{
	}
	bool IsAlreadyInCache(const ClassToCheck* pObject)
	{
		if(IsOutOf(pObject)) return false;
		unsigned long long Key=static_cast<MY_TYPE*>(this)->GetHashKey(pObject);
		RANGE Range=m_MainMap.equal_range(Key);
		for(ITERATOR it=Range.first;it!=Range.second;it++)
			if(static_cast<MY_TYPE*>(this)->IsEqual(it->second,pObject))
				return true;
		return false;
	}
	void AddToCache(ClassToCheck* pObject)
	{
		m_MainMap.insert(TYPE(Key,pVertex));
	}
	ClassToCheck* GetUnique(ClassToCheck* pObject)
	{
//		if(MY_TYPE::IsOutOf(pObject)) return pObject;
		if(static_cast<MY_TYPE*>(this)->IsOutOf(pObject)) return pObject;
		unsigned long long Key=static_cast<MY_TYPE*>(this)->GetHashKey(pObject);
		RANGE Range=m_MainMap.equal_range(Key);
		for(ITERATOR it=Range.first;it!=Range.second;it++)
			if(static_cast<MY_TYPE*>(this)->IsEqual(it->second,pObject))
				return it->second;
		m_MainMap.insert(TYPE(Key,pObject));
		return pObject;
	}
};

class _CDuplicatedVertexCatcher : public _CDuplicationCatcher<_CModelVertex,_CDuplicatedVertexCatcher>
{
public:
	_CDuplicatedVertexCatcher(_CCircuit& OwnerCircuit,MVERTEX_2_SIGN* pMVertex2Sgn,bool IgnoreSgnPropagation):
	  _CDuplicationCatcher<_CModelVertex,_CDuplicatedVertexCatcher> (OwnerCircuit),
	  m_pMVertex2Sgn(pMVertex2Sgn),m_IgnoreSgnPropagation(IgnoreSgnPropagation){}
	~_CDuplicatedVertexCatcher()
	{
	}
	void GetUnique(_CModelVertex* pInObject, _CModelVertex*& pOutObject, short& RootSgn);
	short GetOutSgn(_CModelVertex* pVertex);
	inline MVERTEX_2_SIGN* GetMVertexSgn() const {return m_pMVertex2Sgn;}
	bool IsSuppressedLeaf(_CModelVertex* pLeafVertex);
protected:
	friend class _CDuplicationCatcher<_CModelVertex,_CDuplicatedVertexCatcher>;
	bool IsEqual(const _CModelVertex* pLeft, const _CModelVertex* pRight) {return pLeft->IsEqualDsc(pRight);}
	unsigned long long GetHashKey(const _CModelVertex* pVertex) {return pVertex->GetHashDsc();}
	bool IsOutOf(_CModelVertex* pVertex) {return pVertex->IsTerminal() || pVertex->IsLeaf();}
	typedef pair<_CModelVertex*, short> DUP_RESULT;
	typedef unordered_map<_CModelVertex*, DUP_RESULT> DUP_MAP;
	DUP_MAP m_DupMap;
	MVERTEX_2_SIGN* m_pMVertex2Sgn;
	bool m_IgnoreSgnPropagation;
};

//class _CDuplicatedVertexCatcher2
//{
//public:
//	_CModelVertex* GetUnique(_CModelVertex* pVertex);
//protected:
//	typedef hash_multimap<unsigned long long,_CModelVertex*> VERTICES_MAP;
//	typedef VERTICES_MAP::value_type VERTICES_TYPE;
//	typedef VERTICES_MAP::iterator VERTICES_ITERATOR;
//	typedef pair<VERTICES_ITERATOR,VERTICES_ITERATOR> RANGE;
//	VERTICES_MAP m_MainMap;
//	bool AreEqual(const _CModelVertex* pLeft, const _CModelVertex* pRight);
//	unsigned long long GetHashKey(const _CModelVertex* pVertex);
//};

typedef union tagGLOBAL_VERTEX_ID{
	unsigned long long Global_Id;
	struct tagPARTS
	{
		unsigned long Local_Id;
		unsigned long Circuit_Id;
	}PARTS;
} TGlobal_Vertex_Id;

class _CVertex : public _CInUseCheckpoint, public _CHashable
{
public:
	_CVertex(_CCircuit* pOwner, _CComponent* pComponent, 
		const _CSimpleVertexContainer& Succ1,
		const _CSimpleVertexContainer& Succ0,
		bool Negated=false);
	~_CVertex();
	string GetAssociatedParamName();
	short GetAssociatedSPower();
	const NumericType& GetAssociatedValue();
	short GlobalMultiplier();
	void WriteToTextStream(iostream& stream, _CSimpleVerticesMapsSet& Set);
	//void WriteToTextStream(iostream& stream);
	void WriteGlobalParameterName(iostream& stream);
	short Get1SuccMultiplier() const {return m_1Succ.GetLocalMultiplier();}
	short Get0SuccMultiplier() const {return m_0Succ.GetLocalMultiplier();}
	const _CSimpleVertexContainer& Get1Succ() const {return m_1Succ;}
	const _CSimpleVertexContainer& Get0Succ() const {return m_0Succ;}
	_CComponent* GetComponent() const {return m_pParameter;}
	bool IsLameLeg() {return m_1Succ.IsLocalTerminal() && m_0Succ.Is0(); }
	bool operator==(const _CVertex& Right) const;
	short GetSPower() const {return m_pParameter->GetSShift();}
protected:
	virtual size_t DetermineHashKey(size_t Core=HASH_CORE) override;
	_CComponent* m_pParameter;
	_CSimpleVertexContainer m_1Succ;
	_CSimpleVertexContainer m_0Succ;
	bool m_IsNegated;
	_CCircuit* m_pOwnerCircuit;
};

class _CVertexCache : public _CStreamableCacheTmpl<_CVertex>
{
public:
	~_CVertexCache() {}
	_CVertexCache() {m_Id=2;}
};

//class _CVertexOld : public _CGenericVertex
//{
//public:
//	_CVertexOld(_CCircuit* Owner=NULL,
//		_CComponent* pComponent=NULL, 
//		int ParamOfComponent=0,
//		_CGenericVertex* p1Succ=NULL,
//		_CGenericVertex* p0Succ=NULL, int Multiplier=1, bool IsNegated=false): _CGenericVertex(Owner),
//		m_pComponent(pComponent), m_ParamOfComponent(ParamOfComponent), 
//		m_p1Succ(p1Succ), m_p0Succ(p0Succ), m_IsNegated(IsNegated),m_Multiplier(Multiplier)
//	{
//		TouchDepended();
//	}
//	_CVertexOld(const _CVertexOld& Source):_CGenericVertex(Source.m_OwnerCircuit),
//		m_pComponent(Source.m_pComponent),
//		m_ParamOfComponent(Source.m_ParamOfComponent), 
//		m_p1Succ(Source.m_p1Succ), m_p0Succ(Source.m_p0Succ), 
//		m_IsNegated(Source.m_IsNegated),m_Multiplier(Source.m_Multiplier)
//	{
//		TouchDepended();
//	}
//	virtual ~_CVertexOld(void);
//	virtual bool IsTerminal() const {return false;}
//	virtual short GetTerminalValue() const {return 0;}
//	virtual bool Is0() const {return false;}
//	virtual bool Is1() const {return false;}
//protected:
//	virtual long DetermineHashKey();
//	_declspec(property(get=GetOSucc, put=Set0Succ)) _CGenericVertex* p0Succ;
//	_declspec(property(get=Get1Succ, put=Set1Succ)) _CGenericVertex* p1Succ;
//	virtual void Copy(const _CAbstractVertex& Source); 
//	virtual void TouchDepended();
//	virtual void UnTouchDepended();
//	virtual bool IsEqual(const _CAbstractVertex& Operand);
//	_CGenericVertex* GetOSucc() const {return m_p0Succ;}
//	_CGenericVertex* Get1Succ() const {return m_p1Succ;}
//	void Set0Succ(_CGenericVertex* p0Succ);
//	void Set1Succ(_CGenericVertex* p1Succ);
//	_CGenericVertex* m_p0Succ;
//	_CGenericVertex* m_p1Succ;
//	_CComponent* m_pComponent;
//	int m_ParamOfComponent;
//	int m_Multiplier;
//	bool m_IsNegated;
//};


//class _CVerticesCache : public _CCacheTmpl<_CGenericVertex>
//{
//public:
//	_CGenericVertex* GetVertexS(_CCircuit* Owner,
//		_CComponent* pComponent, 
//		int ParamOfComponent,
//		_CGenericVertex* p1Succ,
//		_CGenericVertex* p0Succ,int Multiplier=1);
//	_CGenericVertex* GetVertexB(_CCircuit* Owner,
//		_CComponent* pComponent, 
//		int ParamOfComponent,
//		_CGenericVertex* p1Succ,
//		_CGenericVertex* p0Succ, bool IsNegated);
//
//};

//const _CTerminalVertex* GetVertex0();
//const _CTerminalVertex* GetVertex1();

//typedef enum tagPlusMinus{plusik,minusik} PLUS_MINUS;
//
//class _CVerticesVector : public vector<_CGenericVertex*> 
//{
//public:
//   _CVerticesVector& operator=(const _CVerticesVector& Source)
//   {
//	   Copy(Source);
//	   return *this;
//   }
//   reference at(size_type _Pos)
//   {
//	   if(_Pos>=size())
//		   resize(_Pos);
//	   vector<_CGenericVertex*>::at(_Pos);
//   }
//   reference operator[](size_type _Pos) {return at(_Pos);} 
//protected:
//	void Copy(const _CVerticesVector& Source) {assign(Source.begin(),Source.end());}
//};

//class _CSExpandedVertices
//{
//public:
//	_CSExpandedVertices():m_IsNegated(false) {}
//	_CSExpandedVertices(const _CSExpandedVertices& Source):m_IsNegated(Source.m_IsNegated),
//		m_Plus(Source.m_Plus),m_Minus(Source.m_Plus) {}
//	_CSExpandedVertices(const _CVerticesVector& Plus, const _CVerticesVector& Minus):m_IsNegated(false),
//		m_Plus(Plus),m_Minus(Minus) {}
//	_CVerticesVector& operator[](PLUS_MINUS pm) {return pm==plusik?(m_IsNegated?m_Minus:m_Plus):(m_IsNegated?m_Plus:m_Minus);}
//	_CSExpandedVertices& operator-()
//	{
//		m_IsNegated=!m_IsNegated;
//		return *this;
//	}
//	_CSExpandedVertices& operator=(const _CSExpandedVertices& Source)
//	{
//		Copy(Source);
//		return *this;
//	}
//protected:
//	void Copy(const _CSExpandedVertices& Source)
//	{
//		m_IsNegated=Source.m_IsNegated;
//		m_Plus=Source.m_Plus;
//		m_Minus=Source.m_Minus;
//	}
//	bool m_IsNegated;
//	_CVerticesVector m_Plus;
//	_CVerticesVector m_Minus;
//};
//
