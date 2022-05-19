#pragma once
#include "stdafx.h"
#include "ComponentPath.h"
#include "norm_float.h"
#include "AbstractOperationDataContainer.h"
#include "InUseCheckpoint.h"
#include "CacheTmpl.h"
#include "AbstractResTables.h"

//#include "Components.h"

class _CModelVertex;
class _CCompRedefEntry;
//class VERTICES_PATH : public list<const _CModelVertex*> 
//{
//public:
//	friend class _CModelVerticesTraitor;
//	VERTICES_PATH() {}
//	VERTICES_PATH(const VERTICES_PATH& Source):list<const _CModelVertex*>(Source) {}
//	VERTICES_PATH(const _CModelVerticesTraitor& Traitor);
//	bool HasBasicComponent() const;
//	long long GetHash(long long Core=HASH_CORE) const;
//	//VERTICES_PATH& operator==(const VERTICES_PATH& Right) {return }
//protected:
//};

class _CModelVerticesPath
{
public:
	typedef list<const _CModelVertex*> LIST;
	_CModelVerticesPath(_CMainCircuit* pMainCircuit):m_pBaseCircuit(pMainCircuit) {}
	_CModelVerticesPath(const _CModelVerticesPath& Source)
		:m_pBaseCircuit(Source.m_pBaseCircuit), m_List(Source.m_List) {}
	void Step2NextVertex(const _CModelVertex* pNextVertex,_CSubModelNumericPattern* pNumPatt=NULL);
	void StepOutOneVertex();
	_CSubCircuitSocket* GetParentSocket() const;
	const _CModelVertex* GetParentMetaVertex() const;
	_CMainCircuit* GetBaseCircuit() const {return m_pBaseCircuit;}
	bool operator==(const _CModelVerticesPath& Right) const
	{
		return (m_pBaseCircuit==Right.m_pBaseCircuit && m_List==Right.m_List);
	}
	long long GetHash(long long Core=HASH_CORE) const;
	void LeaveMetaVertex(const _CModelVertex*& pTopMacro,const _CSubModelNumericPattern*& pTopNumPatt);
	void LeaveMetaVertex2(LIST& StoredHeep,const _CSubModelNumericPattern*& pTopNumPatt);
	void Return2MetaVertex(LIST& StoredHeep);
	void Return2MetaVertex(const _CModelVertex* pTopMacro,const _CSubModelNumericPattern* pTopNumPatt);
	const _CSubModelNumericPattern* GetTopSubModelNumericPatter() const;
	const LIST& GetList() const {return m_List;}
	bool IsInvalid() {return m_pBaseCircuit==NULL;}
	const NumericType& GetNumericValue(_CCompRedefEntry& Entry) const;
	const _CModelVertex* GetTopVertex() const
	{
		ASSERTPDD(!m_List.empty());
		return m_List.back();
	}
protected:
	friend class COMPONENTS_PATH;
	_CMainCircuit* m_pBaseCircuit;
	LIST m_List;
	//typedef unordered_map<const _CModelVertex*,_CSubModelNumericPattern*> NUM_PATT_MAP;
	//NUM_PATT_MAP m_Map;
	typedef list<const _CSubModelNumericPattern*> NUM_PATT_LIST;
	NUM_PATT_LIST m_NumPatList;
};

//class _CModelVerticesTraitor
//{
//public:
//	friend class _CMainCircuit;
//	_CModelVerticesTraitor(_CMainCircuit* pMainCircuit):m_pBaseCircuit(pMainCircuit) {}
//	_CModelVerticesTraitor(const _CModelVerticesTraitor& Source)
//		:m_pBaseCircuit(Source.m_pBaseCircuit), m_List(Source.m_List) {}
//	void Step2NextVertex(const _CModelVertex* pNextVertex);
//	void StepOutOneVertex();
//	void GetVerticesPath(VERTICES_PATH& Path) const;
//	_CMainCircuit* GetBaseCircuit() const {return m_pBaseCircuit;}
//	_CSubCircuitSocket* GetParentSocket() const;
//protected:
//	_CMainCircuit* m_pBaseCircuit;
//	typedef list<const _CModelVertex*> IN_LIST;
//	typedef list<IN_LIST> GLOBAL_LIST;
//	GLOBAL_LIST m_List;
//};

class COMPONENTS_PATH : public list<const _CComponent*>
{
public:
	COMPONENTS_PATH() {}
	COMPONENTS_PATH(const COMPONENTS_PATH& Source):list<const _CComponent*>(Source) {}
	//COMPONENTS_PATH(const VERTICES_PATH& Source);
	//COMPONENTS_PATH(const _CModelVerticesTraitor& Traitor);
	COMPONENTS_PATH(const _CModelVerticesPath& Traitor);
	COMPONENTS_PATH(const _CModelVerticesPath::LIST& TraitorLIST);
	string ToString() const;
	long long  GetHash(long long Core=HASH_CORE) const;
	//double GetNumericValue() const;
	const NumericType& GetNumericValue(_CCompRedefEntry& RedefEntry) const;
	const NumericType& GetNumericValue() const;
protected:
	//void Copy(const VERTICES_PATH& Source);
	void Copy(const _CModelVerticesPath& Source);
};

short PCompare(const COMPONENTS_PATH& Left, const COMPONENTS_PATH& Right,_CMainCircuit* pMainCircuit);

class COMPONENTS_PATH_KeyCompare
{
public:
	COMPONENTS_PATH_KeyCompare(_CMainCircuit*& pMainCircuit):m_pMainCircuit(pMainCircuit) {}
	bool operator() (const COMPONENTS_PATH& Left, const COMPONENTS_PATH& Right) const
	{
		return ::PCompare(Left,Right,m_pMainCircuit)<0;
	}
protected:
	_CMainCircuit*& m_pMainCircuit;
};

//typedef unordered_map<const COMPONENTS_PATH*,size_t> COMP_PATH_ORDER;
typedef unordered_map<COMPONENTS_PATH*,size_t> COMP_PATH_ORDER;
class _CCompPathStorage
{
public:
	_CCompPathStorage():m_pMainCircuit(NULL),m_KeyComp(m_pMainCircuit),m_Stg(m_KeyComp) {}
	//const COMPONENTS_PATH& GetUnique(const COMPONENTS_PATH& Source) {return *(m_Stg.insert(Source).first);}
	const COMPONENTS_PATH& GetUnique(COMPONENTS_PATH& Source) {return *(m_Stg.insert(Source).first);}
	//void PathSt2PathOrdr(COMP_PATH_ORDER& PO) const;
	void PathSt2PathOrdr(COMP_PATH_ORDER& PO);
	string ToString() const;
protected:
	friend class _CMainCircuit;
	_CMainCircuit* m_pMainCircuit;
	COMPONENTS_PATH_KeyCompare m_KeyComp;
	//typedef set<const COMPONENTS_PATH,COMPONENTS_PATH_KeyCompare> COMP_STG;
	typedef set<COMPONENTS_PATH,COMPONENTS_PATH_KeyCompare> COMP_STG;
	COMP_STG m_Stg;
};

//class _CSubModelNumericParrernStorage
//{
//public:
//	_CSubModelNumericPattern* GetPatrernIfExist(const _CModelVerticesPath& Path);
//	bool ForcePattern(const _CModelVerticesPath& Path, _CSubModelNumericPattern*& pOutPattern);
//protected:
//	typedef unordered_map<const COMPONENTS_PATH*,_CSubModelNumericPattern> MAP;
//	MAP m_Map;
//};

class _CFlatVertex;
typedef vector<short> MULTIPLIERS;
typedef vector<const _CFlatVertex*> DESCENDANTS;
typedef vector<_CFlatVertex*> WEEK_DESCENDANTS;

//bool IsOnlyOneDescendant(const DESCENDANTS& Descs, size_t& NonZeroDesc);
//bool IsHollow(const DESCENDANTS& Descs);
bool HasCommonDescSgn(const MULTIPLIERS& Mults, short& CommonMult);
short ExtractCommonFactor(MULTIPLIERS& Mults, bool HighestPositive);
MULTIPLIERS& operator/=(MULTIPLIERS& Mults, short Divider);

template<typename Desc>
bool IsOnlyOneDescendant(const vector<Desc*>& Descs, size_t& NonZeroDesc)
{
	size_t Index = 0;
	for (; Index < Descs.size(); Index++)
		if (Descs[Index] != nullptr)
			break;
	if (Index == Descs.size())
		return false;
	if (Index == Descs.size()-1)
	{
		NonZeroDesc = Descs.size() - 1;
		return true;
	}
	for (size_t NextIndex = Index + 1; NextIndex < Descs.size(); NextIndex++)
		if (Descs[NextIndex] != nullptr)
			return false;
	NonZeroDesc = Index;
	return true;
}

template<typename Desc>
bool IsHollow(const vector<Desc*>& Descs)
{
	size_t Pos = 0;
	bool OnlyOne = IsOnlyOneDescendant(Descs, Pos);
	return OnlyOne && Pos == 0;
}



class _CFlatVertex : public _CHashable
{
public:
	friend class _CMainCircuit;
	friend class _CFlatVertexCache;
	friend class _CFlatVertexWalker;
	_CFlatVertex(size_t NoOfDesc = 2);
	_CFlatVertex(const DESCENDANTS& Descendants, const MULTIPLIERS& Multipliers, const COMPONENTS_PATH& ComponentsPath);
	//_CFlatVertex(const _CFlatVertex* Descendent1,
	//	const _CFlatVertex* Descendent0,
	//	short Multiplier1,
	//	short Multiplier0,
	//	const COMPONENTS_PATH& ComponentsPath);
	_CFlatVertex(const _CFlatVertex& Source);
	bool operator==(const _CFlatVertex& Right) const;
	string ToString(const _CFlatVertexCache& MyCache) const;
	void ToStream(txt_stream& stream, const _CFlatVertexCache& MyCache, size_t& NoOfMults, size_t& NoOfAdds, size_t& NoOfMultsNonOne) const;
	void WalkFlatResult(_CFlatVertexWalker& Walker) const;
	string CompPathStr() const {return m_pComponentsPath==NULL?"":m_pComponentsPath->ToString();}
	//bool NoZero() const;
	bool IsOnlyOneDescendant(size_t& NonZeroDesc) const;
	const COMPONENTS_PATH* GetPath() const {return m_pComponentsPath;}
	const _CFlatVertex* GetDescendant(size_t Index) const
	{
		ASSERTPDD(Index < m_Descendants.size());
		return m_Descendants[Index]; 
	}
	short GetMult(size_t Index) const
	{
		ASSERTPDD(Index < m_Multipliers.size());
		return m_Multipliers[Index];
	}
	//const _CFlatVertex* Get1Descendent() const {return m_1Descendent;}
	//const _CFlatVertex* Get0Descendent() const {return m_0Descendent;}
	//short Get1Mult() const {return m_1Multiplier;}
	//short Get0Mult() const {return m_0Multiplier;}
	size_t GetNoOfDescs() const { return m_Descendants.size(); }
	size_t GetCompPower() const { return m_Multipliers.size()-1; }
protected:
	long long pDesc2Index(const _CFlatVertex* pDesc,const _CFlatVertexCache& MyCache) const;
	//const _CFlatVertex* m_1Descendent;
	//const _CFlatVertex* m_0Descendent;
	//short m_1Multiplier;
	//short m_0Multiplier;
	MULTIPLIERS m_Multipliers;
	DESCENDANTS m_Descendants;
	const COMPONENTS_PATH* m_pComponentsPath;
	virtual long DetermineHashKey(long Core=HASH_CORE);
};

extern _CFlatVertex OneFlatVertex;

class _CFlatVertexWalker
{
public:
	_CFlatVertexWalker(txt_stream& stream);
	void ProcessNextVertex(const _CFlatVertex* pNextVertex, short LocalMultiplier, short CompPower);
	void ProcessNextVertex(const _CFlatVertex* pNextVertex, short LocalMultiplier);
protected:
	void ListOutTheTerm(short LocalMultiplier);
	txt_stream& m_stream;
	typedef pair<const _CFlatVertex*, short/*Power*/> ENTRY;
	typedef list<ENTRY> MAIN_LIST;
	MAIN_LIST m_List;
	typedef list<short> SGN_LIST;
	SGN_LIST m_Sgns;
};

//typedef pair<const COMPONENTS_PATH*, short> SIGNED_COMPONENT_PATH;
typedef tuple<const COMPONENTS_PATH*, /*Sgn*/ short, /*Power*/ short> SIGNED_COMPONENT_PATH;
typedef list<SIGNED_COMPONENT_PATH> SIGNED_COMPONENT_PATH_LIST;
typedef list<SIGNED_COMPONENT_PATH_LIST> SIGNED_COMPONENT_PATH_MULTILIST;
//typedef pair<const _CFlatVertex*,short> SIGNED_FLAT_VERTEX;
typedef list<const _CFlatVertex*> FLAT_VERT_LIST;
typedef list<FLAT_VERT_LIST> FLAT_VERT_MULTILIST;

typedef pair<const _CFlatVertex*,size_t> LEVEL_FLAT_VERTEX;
typedef list<LEVEL_FLAT_VERTEX> LEVEL_FLAT_VERTEX_LIST;
typedef list<LEVEL_FLAT_VERTEX_LIST> LEVEL_FLAT_VERTEX_MULTILIST;

class _CSignedComponentPath
{
public:
	_CSignedComponentPath(const _CFlatVertex* pVertex);
	SIGNED_COMPONENT_PATH& GetCurrent() {return m_Current;}
	SIGNED_COMPONENT_PATH& GetNext() {return m_Next;}
protected:
	SIGNED_COMPONENT_PATH m_Current;
	SIGNED_COMPONENT_PATH m_Next;
};

//class _CReversFlatVertexConfig
//{
//public:
//	_CReversFlatVertexConfig(_CMainCircuit*& pMainCircuit):m_Sorted(true),
//		m_pMainCircuit(pMainCircuit),
//		m_Comp(m_pMainCircuit),
//		m_Set_comp(m_pMainCircuit),
//		m_CPDividers(m_Set_comp),
//		m_SimplifiedCPSet(m_Comp),
//		m_FeasableDividersReady(false),
//		m_CPDividersReady(false) {}
//	void Insert(const _CFlatVertex* pFlatVertex);
//	void VerticesFromDescendants(const _CFlatVertex* pDesc1,
//								 const _CFlatVertex* pDesc0,
//								 FLAT_VERT_LIST& ResultList);
//protected:
//	void Sort();
//	enum{D0=0,D1=1};
//	typedef pair<const _CFlatVertex*,short> FLAT_PRED;
//	class _Comp
//	{
//	public:
//		bool operator()(const FLAT_PRED& Left,const FLAT_PRED& Right) const
//		{
//			if(Left.first==Right.first)
//				return Left.second<Right.second;
//			else
//				return Left.first<Right.first;
//		}
//	};
//	//typedef set<FLAT_PRED,_Comp> SET;
//	//typedef unordered_map<const _CFlatVertex*,SET> MAP;
//	//MAP m_Map;
//	typedef vector<const _CFlatVertex*> FLAT_VERT_VECTOR;
//	//typedef vector<const _CFlatVertex*> FLAT_VERT_LIST;
//	typedef map<FLAT_PRED,FLAT_VERT_LIST,hash_compare<FLAT_PRED,_Comp>> MAP2;
//	MAP2 m_Map2;
//	bool m_Sorted;
//	typedef list<FLAT_VERT_LIST> DIVIDER_SET;
//	class _CCP_Comp
//	{
//	public:
//		_CCP_Comp(_CMainCircuit*& pMainCircuit):m_pMainCircuit(pMainCircuit) {}
//		bool operator()(const SIGNED_COMPONENT_PATH& Left,const SIGNED_COMPONENT_PATH& Right)
//		{
//			short comp=PCompare(*Left.first,*Right.first,m_pMainCircuit);
//			if(comp!=0)
//				return comp<0;
//			return Left.second<Right.second;
//		}
//	protected:
//		_CMainCircuit*& m_pMainCircuit;
//	};
//public:
//	typedef set<SIGNED_COMPONENT_PATH,_CCP_Comp> CP_SET;
//	//typedef set<const COMPONENTS_PATH*,_CCP_Comp> CP_SET;
//protected:
//	class _CCP_Set_comp
//	{
//	public:
//		_CCP_Set_comp(_CMainCircuit*& pMainCircuit):m_pMainCircuit(pMainCircuit) {}
//		bool operator()(const CP_SET& Left, const CP_SET& Right)
//		{
//			return Comp(Left,Right)<0;
//		}
//		short Comp(const CP_SET& Left, const CP_SET& Right);
//	protected:
//		_CMainCircuit*& m_pMainCircuit;
//	};
//public:
//	typedef set<CP_SET,_CCP_Set_comp> CP_DIVIDERS;
//	CP_DIVIDERS& GetCPDividers() {return m_CPDividers;}
//	class _CDividerSet : public DIVIDER_SET
//	{
//	public:
//		FLAT_VERT_LIST& GetNew(FLAT_VERT_LIST* pSource=NULL)
//		{
//			push_back((pSource==NULL)?FLAT_VERT_LIST():FLAT_VERT_LIST(*pSource));
//			return back();
//		}
//		string ToString(const _CFlatVertexCache& MyCache) const;
//		void ToStream(txt_stream& stream,const _CFlatVertexCache& MyCache) const;
//	};
//	void ClearDividers()
//	{
//		m_FeasableDividers.clear();
//		m_CPDividers.clear();
//		m_FeasableDividersReady=false;
//	}
//	string ToStringFeasibleDeviders(const _CFlatVertexCache& Cache) 
//	{
//		PrepFeasibleDivieders();
//		return m_FeasableDividers.ToString(Cache);
//	}
//	void ToStreamFeasibleDeviders(txt_stream& stream,const _CFlatVertexCache& Cache) 
//	{
//		PrepFeasibleDivieders();
//		m_FeasableDividers.ToStream(stream,Cache);
//	}
//	string ToStringFeasibleDevidersPaths();
//	void ToStreamFeasibleDevidersPaths(txt_stream& stream); 
//	void PrepareDividersPaths();
//protected:
//	void PrepFeasibleDivieders()
//	{
//		if(!m_FeasableDividersReady)
//		{
//			FindFeasibleDividers(m_FeasableDividers);
//			m_FeasableDividersReady=true;
//		}
//	}
//	void FindFeasibleDividers(_CDividerSet& Result);
//	void AddBump(_CDividerSet& GlobalSet,FLAT_VERT_LIST& CurrentList);
//	friend class _CFlatVertexCache;
//	_CMainCircuit*& m_pMainCircuit;
//	_CCP_Comp m_Comp;
//	_CCP_Set_comp m_Set_comp;
//	CP_DIVIDERS m_CPDividers;
//	CP_SET m_SimplifiedCPSet;
//	bool m_CPDividersReady;
//	_CDividerSet m_FeasableDividers;
//	bool m_FeasableDividersReady;
//};

class _CFactorizedVertexContainer;
class _CSExpFlatVertices;
class _CSExpFlatVerticesFactorized;
class _CCofactorsVector;
class _CContextSExpFlatVertices;

typedef vector<bool> BOOL_VECTOR;

class _CFlatVertexCache : public _CStreamableCacheTmpl<_CFlatVertex>
{
public:
	_CFlatVertexCache():m_CurrentCof(0),m_pMainCircuit(NULL),
		/*m_ReversFlatVertexConfig(m_pMainCircuit),*/m_Comp(m_pMainCircuit), m_CPSet(m_Comp), m_SCPMap(m_Comp), m_Factorisation(false) 
		{m_Id=2;}
	long long GetIndex(const _CFlatVertex* pVertex) const;
	string ToString() const;
	void ToStream(txt_stream& stream) const;
	bool IsMultiInheriter(const _CFlatVertex* pVertex) const;
	//string ToStringFeasibleDeviders() 
	//{
	//	return m_ReversFlatVertexConfig.ToStringFeasibleDeviders(*this);
	//}
	//void ToStreamFeasibleDeviders(txt_stream& stream) 
	//{
	//	m_ReversFlatVertexConfig.ToStreamFeasibleDeviders(stream,*this);
	//}
	//string PotSimpToString(size_t CofId) const;
	//void PotSimpToStream(txt_stream& stream, size_t CofId) const;
	void SwitchCofactor(size_t CofId) 
	{
		m_CurrentCof=CofId;
	}
	//void LeaveZeroDesc();
	//void NotifySkipped(const COMPONENTS_PATH* pPath);
	void ClearDividers()
	{
		bool IsFree=false;
		for(FLAT_VERT_LIST::iterator it=m_FeasDivList.begin(),_e=m_FeasDivList.end();it!=_e;it++)
			UnTouchVertex(*it,IsFree);
		m_FeasDivList.clear();
		//m_ReversFlatVertexConfig.ClearDividers();
	}
	//string ToStringFeasibleDevidersPaths()
	//{
	//	return m_ReversFlatVertexConfig.ToStringFeasibleDevidersPaths();
	//}
	void ToStreamFeasibleDevidersPaths(txt_stream& stream)
	{
		//m_ReversFlatVertexConfig.ToStreamFeasibleDevidersPaths(stream);
		m_CPSet.ToStream(stream);
	}
	void ReleaseVertex(const _CFlatVertex* pVertex);
	void ReleaseAndKillVertex(const _CFlatVertex* pVertex);
	void ClearReleased();
	void ClearFeasDiv() {m_FeasDivList.clear();}
	void ClearOperCache() {m_DivisionCache.Clear();}
	bool UnTouchVertex(const _CFlatVertex* pVertex,bool& IsFree);
	bool TouchVertex(const _CFlatVertex* pVertex);
	void TouchDescendants(const _CFlatVertex* pVertex);
	void UnTouchDescendants(const _CFlatVertex* pVertex, BOOL_VECTOR& AreFreed);
	void TouchVertices(_CSExpFlatVertices& Vertices);
	void TouchVertices(_CSExpFlatVerticesFactorized& Vertices);
	bool IsRealised(const _CFlatVertex* pVertex);
	typedef enum{None,Divider,HasReminder,InCompatible} DIV_STATUS;
	//To be return
	//const _CFlatVertex* MultiplyDiagrams(const _CFlatVertex* Left, const _CFlatVertex* Right, short& TopSgn);
	//const _CFlatVertex* DivideByASingleValue(DIV_STATUS& Status, const _CFlatVertex* Left, const SIGNED_COMPONENT_PATH& Right, short& TopSgn);
	//To be return
	//const _CFlatVertex* CPSet2Rossette(const _CReversFlatVertexConfig::CP_SET& CpSet);
	//const _CFlatVertex* CPSet2Rossette(const _CPSet& CpSet);
	//const _CFlatVertex* DivideByARossette(DIV_STATUS& Status, const _CFlatVertex* Left, const _CReversFlatVertexConfig::CP_SET& Right, short& TopSgn);
	//To be return
	//DIV_STATUS DivideWithoutReminder(const _CFlatVertex*& Result, const _CFlatVertex* Left, const _CFlatVertex* Right, short& TopSgn, const bool ReminderAlllowed);
	//To be return
	//DIV_STATUS DivideWithoutReminder2(const _CFlatVertex*& Result,const _CFlatVertex* Left, const _CFlatVertex* Right, short& TopSgn);
	//To be return
	//bool IsDivider(const _CFlatVertex* Left, const _CFlatVertex* Right);
	//void FactorizeMe(_CFactorizedVertexContainer& VertexContainer);
	//void FactorizeMe2(_CFactorizedVertexContainer& VertexContainer);
	//void FactorizeAll(size_t Size,_CContextSExpFlatVertices& ExpFlatVerticesFactorized,const string* Context);
	//void FactorizeAll2(size_t NoOfCofacotors,_CContextSExpFlatVertices& ExpFlatVerticesFactorized,const string* Context);
	//To be return
	//void PrepareDividersPaths() {m_ReversFlatVertexConfig.PrepareDividersPaths();}
protected:
	//bool DivideWithoutReminderR(const _CFlatVertex*& Result,const _CFlatVertex* Left, const _CFlatVertex* Right, short& TopSgn);
	//_CFlatVertex(const DESCENDANTS& Descendants, const MULTIPLIERS& Multipliers, const COMPONENTS_PATH& ComponentsPath);

	const _CFlatVertex* GetVertex(const DESCENDANTS& Descendants,
		const MULTIPLIERS& Multipliers, 
		const COMPONENTS_PATH& ComponentsPath,
		short& TopSgn);
	//const _CFlatVertex* GetVertex(const _CFlatVertex* Descendent1,
	//	const _CFlatVertex* Descendent0,
	//	short Multiplier1,
	//	short Multiplier0,
	//	const COMPONENTS_PATH& ComponentsPath,
	//	short& TopSgn);
	class _CBinaryOperationContainer
	{
	public:
		_CBinaryOperationContainer(const _CFlatVertex* LeftOperand,
								   //short LeftMultiplier,
								   const _CFlatVertex* RightOperand,
								   //short RightMultiplier,
								   const _CFlatVertex* Result=NULL,
								   short ResultMultiplier=0):
			m_LeftOperand(LeftOperand), /*m_LeftMultiplier(LeftMultiplier),*/
			m_RightOperand(RightOperand), /*m_RightMultiplier(RightMultiplier),*/
			m_Result(Result), m_ResultMultiplier(ResultMultiplier) {}
		_CBinaryOperationContainer(const _CBinaryOperationContainer& Source):
			m_LeftOperand(Source.m_LeftOperand), /*m_LeftMultiplier(Source.m_LeftMultiplier),*/
				m_RightOperand(Source.m_RightOperand), /*m_RightMultiplier(Source.m_RightMultiplier),*/
			m_Result(Source.m_Result), m_ResultMultiplier(Source.m_ResultMultiplier) {}
		const _CFlatVertex* GetResult() const {return m_Result;}
		short GetResMultiplier() const {return m_ResultMultiplier;}
		void SetResult(const _CFlatVertex* pResult, short Multiplier)
		{
			m_Result=pResult;
			m_ResultMultiplier=Multiplier;
		}
		ULONGLONG GetHash();
		bool operator==(const _CBinaryOperationContainer& Right)
		{
			return (m_LeftOperand==Right.m_LeftOperand &&
				//m_LeftMultiplier==Right.m_LeftMultiplier &&
				m_RightOperand==Right.m_RightOperand /*&&
				m_RightMultiplier==Right.m_RightMultiplier*/);
		}
	protected:
		const _CFlatVertex* m_LeftOperand;
		const _CFlatVertex* m_RightOperand;
		const _CFlatVertex* m_Result;
		//short m_LeftMultiplier;
		//short m_RightMultiplier;
		short m_ResultMultiplier;
	};
	class _CDivOperationContainer : public _CBinaryOperationContainer
	{
	public:
		_CDivOperationContainer(const _CFlatVertex* LeftOperand,
								   const _CFlatVertex* RightOperand,
								   const _CFlatVertex* Result=NULL,
								   short ResultMultiplier=0,
								   //bool WithoutReminder=true,
								   _CFlatVertexCache::DIV_STATUS Status=_CFlatVertexCache::None
								   ):
		    _CBinaryOperationContainer(LeftOperand,RightOperand,Result,ResultMultiplier),
			/*m_WithoutReminder(WithoutReminder),*/m_Status(Status) {}
		_CDivOperationContainer(const _CDivOperationContainer& Source):
			_CBinaryOperationContainer(Source),/*m_WithoutReminder(Source.m_WithoutReminder),*/
				m_Status(Source.m_Status) {}
		//void SetResult(const _CFlatVertex* pResult, short Multiplier,bool WithoutReminder)
		//{
		//	m_Result=pResult;
		//	m_ResultMultiplier=Multiplier;
		//	m_WithoutReminder=WithoutReminder;
		//}
		void SetResult(const _CFlatVertex* pResult, short Multiplier,
			_CFlatVertexCache::DIV_STATUS Status)
		{
			m_Result=pResult;
			m_ResultMultiplier=Multiplier;
			m_Status=Status;
		}
		//bool GetWithoutReminder() const {return m_WithoutReminder;}
		_CFlatVertexCache::DIV_STATUS GetStatus() const {return m_Status;}
	protected:
		_CFlatVertexCache::DIV_STATUS m_Status;
		//bool m_WithoutReminder;
	};
	template<class ContainerType>
	class _CBinaryOperationCache
	{
	public:
		const ContainerType* FindRegistered(ContainerType& Container)
		{
			ULONGLONG Hash=Container.GetHash();
			pair<MAP::iterator,MAP::iterator> range=m_Map.equal_range(Hash);
			for(MAP::iterator it=range.first;it!=range.second;it++)
			{
				ContainerType& ResCont=it->second;
				if(ResCont==Container)
					return &ResCont;
			}
			return NULL;
		}
		const ContainerType* Register(ContainerType& Container)
		{
			ASSERTPDD(FindRegistered(Container)==NULL);
			ULONGLONG Hash=Container.GetHash();
			return &(m_Map.insert(MAP::value_type(Hash,Container))->second);
		}
		void Clear() {m_Map.clear();}
	protected:
		typedef unordered_multimap<ULONGLONG,ContainerType> MAP;
		MAP m_Map;
	};

	bool ValidateVertex(const _CFlatVertex* pVertex);
	//void Test();
	void PrepFeasibleDivieders()
	{
		//if(!m_FeasableDividersReady)
		//	m_ReversFlatVertexConfig.FindFeasibleDividers(m_FeasableDividers);
	}
	virtual void ProcessIndex(long long Index, _CFlatVertex* pVertex) override;
	class _CExtraData
	{
	public:
		_CExtraData(unsigned long long Id):m_Id(Id),m_UsageCounter(0) {}
		_CExtraData(const _CExtraData& Source):m_Id(Source.m_Id),m_UsageCounter(0) {}
		void Touch() {m_UsageCounter++;}
		bool UnTouch() {return --m_UsageCounter==0;}
		bool IsFree() const {return m_UsageCounter==0;}
		unsigned long long GetId() { return m_Id;}
		bool IsMultiInheriter() const {return m_UsageCounter>1; }
	protected:
		unsigned long long m_Id;
		size_t m_UsageCounter;
		friend class _CFlatVertexCache;
	};
	void RemoveByReference(const _CFlatVertex* pFlatVertex);
	typedef unordered_map<const _CFlatVertex*, long long> ID_MAP;
	typedef unordered_map<const _CFlatVertex*, _CExtraData> EXTRA_DATA_MAP;
	//typedef pair<const COMPONENTS_PATH*,bool> ZERO_DESC_ELEM;
	//typedef unordered_map<const COMPONENTS_PATH*,bool> ZERO_DESC_MAP;
	//typedef self_exp_vector<ZERO_DESC_MAP> ZERO_DESC_MAPS;
	ID_MAP m_IdMap;
	EXTRA_DATA_MAP m_ExtraDataMap;
	//ZERO_DESC_MAP m_ZeroDescMap;
	//ZERO_DESC_MAPS m_ZeroDescMaps;
	size_t m_CurrentCof;
	//void RegisterNoZero(_CFlatVertex* pVertex);
	//_CReversFlatVertexConfig m_ReversFlatVertexConfig; 
	//_CReversFlatVertexConfig::_CDividerSet m_FeasableDividers;
	//bool m_FeasableDividersReady;
	friend class _CMainCircuit;
	_CMainCircuit* m_pMainCircuit;
	_CBinaryOperationCache<_CBinaryOperationContainer> m_MultiplyCache;
	_CBinaryOperationCache<_CDivOperationContainer> m_DivisionCache;
	class _CCP_Comp
	{
	public:
		_CCP_Comp(_CMainCircuit*& pMainCircuit):m_pMainCircuit(pMainCircuit) {}
		bool operator()(const SIGNED_COMPONENT_PATH& Left,const SIGNED_COMPONENT_PATH& Right) const
		{
			//short comp = PCompare(*Left.first, *Right.first, m_pMainCircuit);
			short comp = PCompare(*get<0>(Left), *get<0>(Right), m_pMainCircuit);
			if (comp != 0)
				return comp<0;
			comp = get<2>(Left);
			comp -= get<2>(Right);
			if (comp != 0)
				return comp < 0;
			return get<1>(Left)<get<1>(Right);
		}
	protected:
		_CMainCircuit*& m_pMainCircuit;
	};
	class _CPSet : public set<SIGNED_COMPONENT_PATH,_CCP_Comp>
	{
	public:
		_CPSet(_CCP_Comp& Comp):set<SIGNED_COMPONENT_PATH,_CCP_Comp>(Comp) {}
		string ToString() const;
		void ToStream(txt_stream& stream) const;
	};
	_CCP_Comp m_Comp;
	_CPSet m_CPSet;
	FLAT_VERT_LIST m_FeasDivList;
	void PrepareFeasDivList();
	bool m_Factorisation;
	typedef multimap<SIGNED_COMPONENT_PATH,SIGNED_COMPONENT_PATH,_CCP_Comp> SCP_MAP;
	class _SCPMap : public SCP_MAP
	{
	public:
		_SCPMap(_CCP_Comp& Comp): SCP_MAP(Comp) {}
		bool Include(_CSignedComponentPath& CompPath);
		void GetSolidPath(SIGNED_COMPONENT_PATH_MULTILIST& List);
	protected:
		void GetNextPathToken(SIGNED_COMPONENT_PATH_MULTILIST& GlobalLists,
			SIGNED_COMPONENT_PATH_LIST& CurrentList, 
			const SIGNED_COMPONENT_PATH& Next);
	};
	_SCPMap m_SCPMap;
	//LEVEL_FLAT_VERTEX_MULTILIST m_FeasLevelDivList;
	FLAT_VERT_MULTILIST m_FeasLevelDivList;
	void PathList2VertList(FLAT_VERT_MULTILIST& Dest,SIGNED_COMPONENT_PATH_MULTILIST& Source);
};

class _CFlatVertexCache;

class _CSExpFlatVertices 
{
public:
	class _CVertexContainer
	{
	public:
		_CVertexContainer(short S=0, const _CFlatVertex* pV=NULL):Sgn(S),pVertex(pV) {}
		_CVertexContainer(const _CVertexContainer& Source):Sgn(Source.Sgn),pVertex(Source.pVertex) {}
		_CVertexContainer& operator=(const _CVertexContainer& Right)
		{
			Sgn=Right.Sgn;
			pVertex=Right.pVertex;
			return *this;
		}
		short Sgn;
		const _CFlatVertex* pVertex;
	};
	typedef enum {ZERO,ONE,MINUS_ONE,EMPTY} INIT_TYPE;
	_CSExpFlatVertices(INIT_TYPE Type=EMPTY);
	_CSExpFlatVertices(const _CSExpFlatVertices& Source):m_MaxPower(Source.m_MaxPower), 
		m_IsEmpty(Source.m_IsEmpty),m_Map(Source.m_Map) {}
	bool empty() {return m_IsEmpty;}
	size_t size() const {return m_MaxPower;}
	_CSExpFlatVertices& operator=(const _CSExpFlatVertices& Right)
	{
		m_MaxPower=Right.m_MaxPower;
		m_IsEmpty=Right.m_IsEmpty;
		m_Map=Right.m_Map;
		return *this;
	}
	_declspec(property(get=getFlatCont,put=putFlatCont)) const _CVertexContainer* sPowerCont[];
	const _CVertexContainer* getFlatCont(size_t Index) const;
	void putFlatCont(size_t Index,/*const*/ _CVertexContainer* pCont);

	string ToString(const _CFlatVertexCache& MyCache) const;
	void ToStream(txt_stream& stream,const _CFlatVertexCache& MyCache) const;
	void SoPToStream(txt_stream& stream) const;
	_CSExpFlatVertices& operator*=(short Multiplier);
	void TouchVertices(_CFlatVertexCache& Cache);
protected:
	//typedef map<size_t,const _CFlatVertex*,hash_compare<size_t,less<size_t>>> MAP_TYPE;
	typedef map<size_t,_CVertexContainer,hash_compare<size_t,less<size_t>>> MAP_TYPE;
	MAP_TYPE m_Map;
	size_t m_MaxPower;
	bool m_IsEmpty;
	friend class _CSimplyComponent;
	friend class _CSubCircuitSocket;
	friend class _CSExpFlatVerticesFactorized;
};

class _CFactorizedVertexContainer 
{
public:
	~_CFactorizedVertexContainer();
	_CFactorizedVertexContainer(short S=1,const _CFlatVertex* pVertex=NULL,_CMainCircuit* pMainCircuit=NULL);
	_CFactorizedVertexContainer(const _CFactorizedVertexContainer& Source);
	_CFactorizedVertexContainer(const _CSExpFlatVertices::_CVertexContainer& Source,_CMainCircuit* pMainCircuit);
	//_CFactorizedVertexContainer& operator=(const _CSExpFlatVertices::_CVertexContainer& Right)
	//{
	//	Sgn=Right.Sgn;
	//	m_List.clear();
	//	m_List.push_back(Right.pVertex);
	//	return *this;
	//}
	_CFactorizedVertexContainer& operator=(const _CFactorizedVertexContainer& Right)
	{
		Sgn=Right.Sgn;
		m_List=Right.m_List;
		m_pMainCircuit=Right.m_pMainCircuit;
		return *this;
	}
	short Sgn;
	bool NotFactorize() {return m_AllreadyFactorized || m_List.front()==&OneFlatVertex;}
	inline void SetFactorized() {m_AllreadyFactorized=true;}
	const _CFlatVertex* GetNFVertx() {return m_List.front(); }
protected:
	void TouchAll();
	void UnTouchAll();
	typedef list<const _CFlatVertex*> FACTORED_LIST;
	FACTORED_LIST m_List;
	_CMainCircuit* m_pMainCircuit;
	bool m_AllreadyFactorized;
	friend class _CSExpFlatVerticesFactorized;
	friend class _CFlatVertexCache;
};

class _CSExpFlatVerticesFactorized 
{
public:
	_CSExpFlatVerticesFactorized(_CSExpFlatVertices::INIT_TYPE Type=_CSExpFlatVertices::EMPTY);
	_CSExpFlatVerticesFactorized(const _CSExpFlatVerticesFactorized& Source):m_MaxPower(Source.m_MaxPower), 
		m_IsEmpty(Source.m_IsEmpty),m_Map(Source.m_Map) {}
	_CSExpFlatVerticesFactorized(const _CSExpFlatVertices& Source,_CMainCircuit* pMainCircuit);
	bool empty() {return m_IsEmpty;}
	size_t size() const {return m_MaxPower;}
	_CSExpFlatVerticesFactorized& operator=(const _CSExpFlatVerticesFactorized& Right)
	{
		m_MaxPower=Right.m_MaxPower;
		m_IsEmpty=Right.m_IsEmpty;
		m_Map=Right.m_Map;
		return *this;
	}
	_declspec(property(get=getFlatCont,put=putFlatCont)) /*const*/ _CFactorizedVertexContainer* sPowerCont[];
	/*const*/ _CFactorizedVertexContainer* getFlatCont(size_t Index) /*const*/;
	void putFlatCont(size_t Index,/*const*/ _CFactorizedVertexContainer* pCont);

	string ToString(const _CFlatVertexCache& MyCache) const;
	void ToStream(txt_stream& stream,const _CFlatVertexCache& MyCache) const;
	void SoPToStream(txt_stream& stream) const;
	_CSExpFlatVerticesFactorized& operator*=(short Multiplier);
	void TouchVertices(_CFlatVertexCache& Cache);
	void SetOwner(_CMainCircuit* pMainCircuit)
	{
		m_pMainCircuit=pMainCircuit;
	}
	void clear()
	{
		m_Map.clear();
		m_IsEmpty=true;
	}
protected:
	//typedef map<size_t,const _CFlatVertex*,hash_compare<size_t,less<size_t>>> MAP_TYPE;
	typedef map<size_t,_CFactorizedVertexContainer,hash_compare<size_t,less<size_t>>> MAP_TYPE;
	MAP_TYPE m_Map;
	size_t m_MaxPower;
	bool m_IsEmpty;
	friend class _CSimplyComponent;
	friend class _CSubCircuitSocket;
	friend class _CContextSExpFlatVertices;
	_CMainCircuit* m_pMainCircuit;
};
class _CSExpFlatVerticesFilter
{
public:
	_CSExpFlatVertices* PutFiltered(_CSExpFlatVertices* pOrigin,short ExtraSgn);
protected:
	typedef list<_CSExpFlatVertices> LIST;
	LIST m_List;
};
//basic_ostream<char, char_traits<char> >& operator<<(basic_ostream<char, char_traits<char> >& stream, const _CSExpFlatVertices& VertRes);

//extern _CSExpFlatVertices ZeroSExpFlatVertices;
//extern _CSExpFlatVertices OneSExpFlatVertices;

class _CMultLevelSExpFlatVertices : public self_exp_vector< _CSExpFlatVertices*>
{
public:
	typedef _CSExpFlatVertices* _BaseType;
	typedef enum {ZERO,ONE,PLAIN} INIT_TYPE;
	_CMultLevelSExpFlatVertices(INIT_TYPE Type=PLAIN);
	_CMultLevelSExpFlatVertices(size_type _Count):self_exp_vector<_BaseType> (_Count),m_Type(PLAIN) {}
	_CMultLevelSExpFlatVertices(size_type _Count, const _BaseType& _Val):self_exp_vector<_BaseType> (_Count, _Val),m_Type(PLAIN) {}
    _CSExpFlatVertices* get_smart(size_t Index) const;
protected:
	INIT_TYPE m_Type;
};

extern _CMultLevelSExpFlatVertices ZeroMultLevelSExpFlatVertices;
extern _CMultLevelSExpFlatVertices OneMultLevelSExpFlatVertices;

//class _CLeafsSExpVertices : public self_exp_vector<_CSExpFlatVertices*>
class _CLeafsSExpVertices : public self_exp_vector<_CMultLevelSExpFlatVertices*>
{
public:
	//typedef _CSExpFlatVertices* _BaseType;
	typedef _CMultLevelSExpFlatVertices* _BaseType;
	_CLeafsSExpVertices():self_exp_vector<_BaseType> () {}
	_CLeafsSExpVertices(size_type _Count):self_exp_vector<_BaseType> (_Count) {}
	_CLeafsSExpVertices(size_type _Count, const _BaseType& _Val):self_exp_vector<_BaseType> (_Count, _Val) {}
	const _CMultLevelSExpFlatVertices* get_smart(size_t Index) const;
	//const _CSExpFlatVertices* get_smart(size_t Index) const;
};

extern _CSExpFlatVertices ZeroFlatVertices;

extern _CSExpFlatVertices OneFlatVertices;

extern _CSExpFlatVertices MinusOneFlatVertices;

class _CApproxCriterion;

class _CSPowerLimiter
{
public:
	_CSPowerLimiter() {}
	_CSPowerLimiter(const _CSparsePolynomial* pNumResult, const _CApproxCriterion* pAppCriterion,size_t FullSize)
	{
		InitGlobal(pNumResult,pAppCriterion,FullSize);
	}
	bool IsAllowed(unsigned short sPower) const;
	void SetAllowed(unsigned short sPower) 
	{
		m_Set.insert(sPower);
	}
	void CancelAllowed(unsigned short sPower) 
	{
		if(IsAllowed(sPower))
			m_Set.erase(sPower);
	}
	bool IsEmpty() const {return m_Set.empty();}
	void WorkOutNew(const _CSPowerLimiter& TopLimiter,
		const _CSparsePolynomial& TopPattern,
		const _CSparsePolynomial& CompPattern,
		const _CSparsePolynomial& DescPattern,
		const _CApproxCriterion& AppCriterion);
	void InitGlobal(const _CSparsePolynomial* pNumResult, const _CApproxCriterion* pAppCriterion,size_t FullSize);
	bool operator==(const _CSPowerLimiter& Right) const {return m_Set==Right.m_Set;}
	long long GetHash(long long Core) const;
protected:
	//typedef set<unsigned short> SET;
	SHORT_SET m_Set;
	friend class _CMainCircuit;
};

class _CFlatVerticesResContainer2 : public _CAbstractOperationDataContainer<const _CFlatVertex*>
{
public:
	_CFlatVerticesResContainer2(const _CModelVertex* pCurrentVertex,_CModelVerticesPath& Path,size_t Power,const string* Context,NumericType& Inacurracy):
		  m_Path(Path),
		  m_Power(Power),
		  m_Context(Context),
		  m_Result(NULL),
		  m_Sign(1),
		  m_Inaccuracy(Inacurracy),
		  m_p_tmp_file(NULL)
	{
		m_Path.Step2NextVertex(pCurrentVertex);
	}
    _CFlatVerticesResContainer2(const _CFlatVerticesResContainer2& Source):
		  m_Path(Source.m_Path),
		  m_Power(Source.m_Power),
		  m_Context(Source.m_Context),
		  m_Result(Source.m_Result),
		  m_Sign(Source.m_Sign),
		  m_Inaccuracy(Source.m_Inaccuracy),
		  m_p_tmp_file(NULL)
	{
	}
	virtual _CAbstractOperationDataContainer<const _CFlatVertex*>* AllocateCopy() {return this;}
	void CachItToHD() {}//napisaæ
	void Hibernate() {}//napisaæ
	void ReleaseResult()
	{
		if(--m_UsageCounter==0)
			CachItToHD();
	}
	const _CFlatVertex* GetVertex() {return m_Result;}
	short GetSgn() {return m_Sign;}
	const NumericType& GetInaccuracy() {return m_Inaccuracy;}
	void SetResultVert(const _CFlatVertex* pVertex) {m_Result=pVertex;}
	void SetResultSgn(short Sgn) {m_Sign=Sgn;}
	void SetResults(const _CFlatVertex* pVertex,  short Sgn, NumericType& CurrentAccuracy) 
	{
		m_Result=pVertex;
		m_Sign=Sgn;
		m_Inaccuracy-=CurrentAccuracy;
	}
protected:
	virtual bool IsEqualTo(const _CAbstractOperationDataContainer<const _CFlatVertex*>& Right);
	virtual long DetermineHashKey(long Core=HASH_CORE);
	_CModelVerticesPath m_Path;
	const string* m_Context;
	_CTempCacheFiler* m_p_tmp_file;
	size_t m_Power;
	friend class _CFlatVerticesResCache2;
	const _CFlatVertex* m_Result;
	short m_Sign;
	NumericType m_Inaccuracy;
};

class _CFlatVerticesResCache2 : public _CAbstractOperationCache<const _CFlatVertex*>
{
};

extern _CSPowerLimiter ZeroSPowertLimiter;

class _CFlatVerticesResContainer : public _CAbstractOperationDataContainer<_CSExpFlatVertices>
//class _CFlatVerticesResContainer : public _CAbstractOperationDataContainer<_CMultLevelSExpFlatVertices>
{
public:
	_CFlatVerticesResContainer(const _CModelVertex* pCurrentVertex,_CModelVerticesPath& Path,const _CSPowerLimiter& Limiter,const string* Context):
		m_Path(Path), 
		m_Limiter(Limiter),
		m_Context(Context),
		m_p_tmp_file(NULL)
		{
			m_Path.Step2NextVertex(pCurrentVertex);
		}
	//_CFlatVerticesResContainer(_CModelVerticesTraitor& Traitor,/*size_t Level,*/const string* Context);
	_CFlatVerticesResContainer(const _CFlatVerticesResContainer& Source):
	    m_Path(Source.m_Path), 
		m_Limiter(Source.m_Limiter),
		m_Context(Source.m_Context),
		m_ResVertices(Source.m_ResVertices),
		m_p_tmp_file(NULL)//,
		//m_Level(Source.m_Level) 
		{}
	//virtual _CAbstractOperationDataContainer<_CMultLevelSExpFlatVertices>* AllocateCopy() {return this;}
	virtual _CAbstractOperationDataContainer<_CSExpFlatVertices>* AllocateCopy() {return this;}
	//_CMultLevelSExpFlatVertices& GetResult() {return m_ResVertices;}
	_CSExpFlatVertices& GetResult() {return m_ResVertices;}
	void CachItToHD() {}//napisaæ
	void Hibernate() {}//napisaæ
	void ReleaseResult();
protected:
	//virtual bool IsEqualTo(const _CAbstractOperationDataContainer<_CMultLevelSExpFlatVertices>& Right);
	virtual bool IsEqualTo(const _CAbstractOperationDataContainer<_CSExpFlatVertices>& Right);
	virtual long DetermineHashKey(long Core=HASH_CORE);
	_CModelVerticesPath m_Path;
	const string* m_Context;
	//size_t m_Level;
	//_CMultLevelSExpFlatVertices m_ResVertices;
	_CSExpFlatVertices m_ResVertices;
	_CTempCacheFiler* m_p_tmp_file;
	_CSPowerLimiter m_Limiter;
	friend class _CFlatVerticesResCache;
};

class _CFlatVerticesResContainerWrapper
{
public:
	_CFlatVerticesResContainerWrapper(_CFlatVerticesResContainer* pContainer=NULL)
		:m_pContainer(pContainer) {}
	~_CFlatVerticesResContainerWrapper()
	{
		if(m_pContainer!=NULL)
			m_pContainer->ReleaseResult();
	}
	operator  _CFlatVerticesResContainer*&()  {return m_pContainer;}
protected:
	 _CFlatVerticesResContainer* m_pContainer;
};

class _CFlatVerticesResCache : public _CAbstractOperationCache<_CSExpFlatVertices>
//class _CFlatVerticesResCache : public _CAbstractOperationCache<_CMultLevelSExpFlatVertices>
{
public:
	_CFlatVerticesResCache();
	_CFlatVerticesResContainer* FindOrRegisterResult(_CFlatVerticesResContainer* pContainer);
	void RegisterResult(_CFlatVerticesResContainer* pContainer);
	void HibernateAll();
protected:
	_CTempCacheFiler m_tmp_file;
};

typedef self_exp_vector<_CFlatVerticesResContainerWrapper> _CFlatVerticesResCacheKiller;
//class _CFlatVerticesResCacheKiller : public self_exp_vector<_CFlatVerticesResContainer*>
//{
//public:
//	_CFlatVerticesResCacheKiller(size_t Size):self_exp_vector<_CFlatVerticesResContainer*>(Size) {}
//	~_CFlatVerticesResCacheKiller();
//};

class _CContextSExpFlatVertices
{
public:
	_CSExpFlatVerticesFactorized& GetSExpFlatResVertices(size_t CofId,const string* Context);
	//_CMultLevelSExpFlatVertices& GetSExpFlatResVertices(size_t CofId,const string* Context);
	void clear()
	{
		m_Map.clear();
	}
	void clear(const string* Context)
	{
		m_Map.erase(Context);
	}
	bool IsContextDetermined(const string* Context);
	_CMainCircuit* GetMainCircuit() {return m_pMainCircuit;}
protected:
	typedef self_exp_vector<_CSExpFlatVerticesFactorized> VECTOR;
	//typedef self_exp_vector<_CExpFlatVertices> VECTOR;
	//typedef self_exp_vector<_CMultLevelSExpFlatVertices> VECTOR;
	typedef unordered_map<const string*, VECTOR> MAP;
	MAP m_Map;
	friend class _CMainCircuit;
	_CMainCircuit* m_pMainCircuit;
};


class _CFlatVerticesSPowerLimiter
{
public:
	_CFlatVerticesSPowerLimiter(/*unsigned short MaxPower=USHRT_MAX*/)/*:m_MaxPower(MaxPower)*/ {}
	_CFlatVerticesSPowerLimiter(const _CSparsePolynomial* pNumResult, const _CApproxCriterion* pAppCriterion);
	void SetSPowerAllowed(size_t EntryNo, unsigned short sPower);
	bool IsSPowerAllowed(size_t EntryNo, unsigned short sPower) const;
	bool IsEntryAllowed(size_t EntryNo) const;
	void CancelSPowerAllowed(size_t EntryNo, unsigned short sPower);
	void SetAllowedRange(size_t EntryNo, unsigned short sMinPower, unsigned short sMaxPower=numeric_limits<unsigned short>::max());
	void CancelAllowedRange(size_t EntryNo, unsigned short sMinPower, unsigned short sMaxPower=numeric_limits<unsigned short>::max());
	void WorkOutNew(const _CFlatVerticesSPowerLimiter& TopLimiter,
		const _CSubModelNumericPattern& TopPattern,
		const _CSparsePolynomial& CompPattern,
		const _CSubModelNumericPattern& DescPattern,
		const _CApproxCriterion& AppCriterion);
	bool IsEmpty() const;
	_CSPowerLimiter& GetEntryLimiter(size_t EntryNo) {return m_Map[EntryNo];}
	void RemoveEntryLimiter(size_t EntryNo) {m_Map.erase(EntryNo);}
	const _CSPowerLimiter& GetSmartEntryLimiter(size_t EntryNo) const ;
protected:
	typedef map<size_t,_CSPowerLimiter,hash_compare<size_t,less<size_t>>> MAP_TYPE;
	MAP_TYPE m_Map;
	//unsigned short m_MaxPower;
};

//typedef self_exp_vector<_CFlatVerticesSPowerLimiter> _CFlatVerticesSPowerLimiterDesc;