#pragma once
#include "stdafx.h"
#include "FlatVertex.h"
#include "PathTraitor.h"
#include "Vertex.h"
#include "norm_float.h"
#include "Components.h"

typedef list<const _CModelVertex*> MVerticesList;
ULONGLONG ToHash(const MVerticesList& List);

//class _CNewSimplifierData;

class _CModelVerticesPathBasedContainer
{
public:
	_CModelVerticesPathBasedContainer() {}
	_CModelVerticesPathBasedContainer(const MVerticesList& VerticesPath):
	  m_VerticesPath(VerticesPath) {}
	const MVerticesList& VerticesPath() { return m_VerticesPath; }
protected:
	MVerticesList m_VerticesPath;
	template<class StoredData>
		friend class _CModelVerticesPathBasedStorage;
};

class _CPreFlatVertexList;

class _CPreFlatVertexContainer /*: public _CModelVerticesPathBasedContainer*/
{
public:
	typedef vector<_CPreFlatVertexContainer*> PRE_FLAT_DESCS;
	typedef vector<NumericType> DESC_VALUES;

	_CPreFlatVertexContainer(_CPreFlatVertexList& ParentList, const NumericType& ParamValue, size_t NoOfDescs) :
		m_CurrDescs(NoOfDescs,nullptr),
		//m_Curr1Desc(NULL),
		m_CurrentDescValues(NoOfDescs,0.0),
		//m_1CurrentValue(0),
		m_CurrentTopValue(0),
		m_Sgns(NoOfDescs,1),
		//m_0Sgn(1),
		m_ParentList(ParentList),
		m_NeedToBeUpdated(true),
		m_ParamValue(ParamValue) 
	{
	}
	_CPreFlatVertexContainer(_CPreFlatVertexList& ParentList, const NumericType& ParamValue, PRE_FLAT_DESCS& Descs) :
		m_CurrDescs(Descs),
		m_CurrentDescValues(Descs.size(),0.0),
		m_CurrentTopValue(0),
		m_Sgns(Descs.size(),1),
		m_ParentList(ParentList),
		m_NeedToBeUpdated(true),
		m_ParamValue(ParamValue)
	{
	}
	//_CPreFlatVertexContainer(_CPreFlatVertexList& ParentList, const NumericType& ParamValue,
	//	_CPreFlatVertexContainer* Curr1Desc,
	//	_CPreFlatVertexContainer* Curr0Desc) :
	//	/*_CModelVerticesPathBasedContainer(VerticesPath),*/
	//	m_Curr0Desc(Curr0Desc),
	//	m_Curr1Desc(Curr1Desc),
	//	m_0CurrentValue(0),
	//	m_1CurrentValue(0),
	//	m_CurrentTopValue(0),
	//	m_1Sgn(1),
	//	m_0Sgn(1),
	//	m_ParentList(ParentList),
	//	m_NeedToBeUpdated(true),
	//	m_ParamValue(ParamValue)
	//{
	//}

	_CPreFlatVertexContainer(const _CPreFlatVertexContainer& Source) :
		m_CurrDescs(Source.m_CurrDescs),
		m_CurrentDescValues(Source.m_CurrentDescValues),
		m_CurrentTopValue(Source.m_CurrentTopValue),
		m_Sgns(Source.m_Sgns),
		m_ParentList(Source.m_ParentList),
		m_NeedToBeUpdated(true),
		m_ParamValue(Source.m_ParamValue)
	{
	}
	//_CPreFlatVertexContainer(const _CPreFlatVertexContainer& Source) :
	//	m_Curr0Desc(Source.m_Curr0Desc),
	//	m_Curr1Desc(Source.m_Curr1Desc),
	//	m_0CurrentValue(Source.m_0CurrentValue),
	//	m_1CurrentValue(Source.m_1CurrentValue),
	//	m_CurrentTopValue(Source.m_CurrentTopValue),
	//	m_1Sgn(Source.m_1Sgn),
	//	m_0Sgn(Source.m_0Sgn),
	//	m_ParentList(Source.m_ParentList),
	//	m_NeedToBeUpdated(true),
	//	m_ParamValue(Source.m_ParamValue)
	//{
	//}
	_CPreFlatVertexContainer& operator=(const _CPreFlatVertexContainer Source);
	//double GetParameterValue();
	void CreateFlatVertex(_CNewSimplifierData& Data, _CFlatVertex*& pResVertex, short& OutSgn);
	bool NotifyToBeUpdated() //return false if already notified
	{
		if(m_NeedToBeUpdated)
			return false;
		m_NeedToBeUpdated=true;
		return true;
	}
	bool IsNew() const;
	bool IsEmpty() const; 
	bool IsTerminal();
	MVerticesList& GetVerticesPath();
	//NumericType UpdateValue(_CPreFlatVertexContainer* pChild,NumericType Increase);//??????
	const MVerticesList& VerticesPath();
	size_t NoOfDescs()
	{
		ASSERTPDD(m_CurrDescs.size()==m_CurrentDescValues.size() && m_CurrDescs.size()==m_Sgns.size() );
		return m_CurrDescs.size();
	}
	_CPreFlatVertexContainer*& CurrentDesc(size_t Index) 
	{
		ASSERTPDD(Index < m_CurrDescs.size());
		return m_CurrDescs[Index]; 
	}
	NumericType& CurrentDescValue(size_t Index)
	{
		ASSERTPDD(Index < m_CurrentDescValues.size());
		return m_CurrentDescValues[Index];
	}
	short& CurrentSgn(size_t Index)
	{
		ASSERTPDD(Index < m_Sgns.size());
		return m_Sgns[Index];
	}
protected:
	//MVerticesList m_VerticesPath;
	//_CPreFlatVertexContainer* m_Curr1Desc;
	MULTIPLIERS m_Sgns;
	PRE_FLAT_DESCS m_CurrDescs;
	DESC_VALUES m_CurrentDescValues;
	//short m_1Sgn;
	//_CPreFlatVertexContainer* m_Curr0Desc;
	//short m_0Sgn;
	NumericType m_CurrentTopValue;
	//NumericType m_0CurrentValue;
	//NumericType m_1CurrentValue;
	const NumericType& m_ParamValue;
	bool m_NeedToBeUpdated;
	_CPreFlatVertexList& m_ParentList;
	friend class _CPreFlatMap;
	friend class _CTreeForkComponent;
	friend class _CSimplyComponent;
	friend class _CMetaComponent;
	friend class _CNewSimplifierData;
};

class _CPreFlatVertexList : /*public self_exp_vector<_CPreFlatVertexContainer>,*/ public _CModelVerticesPathBasedContainer
{
public:
	typedef pair<size_t,size_t> TOP_COORDINATERS;
	_CPreFlatVertexList() {}
	_CPreFlatVertexList(const MVerticesList& VerticesPath/*,size_t Power=0*/):
	_CModelVerticesPathBasedContainer(VerticesPath) 
	{
		//if(Power>0)
		//	resize(Power,_CPreFlatVertexContainer(*this));
	}
	_CPreFlatVertexList(const _CPreFlatVertexList& Source):
	_CModelVerticesPathBasedContainer(Source),m_Map(Source.m_Map)/*,
	self_exp_vector<_CPreFlatVertexContainer>(Source)*/
	{
		//m_Map2=Source.m_Map2;
	}
	MVerticesList& GetVerticesPath() {return m_VerticesPath;}
	_CPreFlatVertexContainer& at(size_t Power, const _CModelVerticesPath& Path,  _CCompRedefEntry& RedefEntry);
	//_CPreFlatVertexContainer& at(TOP_COORDINATERS& TopCoordinaters, const _CModelVerticesPath& Path, _CCompRedefEntry& RedefEntry);
protected:
	typedef map<size_t/*Current Power*/,_CPreFlatVertexContainer> MAP;
	MAP m_Map;//do likwidacji
	//typedef map<TOP_COORDINATERS,_CPreFlatVertexContainer> MAP2;
	//typedef unordered_map<unsigned long long,_CPreFlatVertexContainer> MAP2;
	//typedef map<unsigned long long,_CPreFlatVertexContainer> MAP2;
	//MAP2 m_Map2;

};

//class _CPreFlatVertexList : public self_exp_vector<_CPreFlatVertexContainer>, public _CModelVerticesPathBasedContainer
//{
//public:
//	_CPreFlatVertexList() {}
//	_CPreFlatVertexList(const MVerticesList& VerticesPath/*,size_t Power=0*/):
//	_CModelVerticesPathBasedContainer(VerticesPath) 
//	{
//		//if(Power>0)
//		//	resize(Power,_CPreFlatVertexContainer(*this));
//	}
//	_CPreFlatVertexList(const _CPreFlatVertexList& Source):
//	_CModelVerticesPathBasedContainer(Source),
//	self_exp_vector<_CPreFlatVertexContainer>(Source)
//	{}
//	MVerticesList& GetVerticesPath() {return m_VerticesPath;}
//protected:
//};

template<class StoredData>
class _CModelVerticesPathBasedStorage
{
public:
	StoredData& GetAt(const _CModelVerticesPath& Path)
	{
		const MVerticesList& PPath=Path.GetList();
		_CPreFlatVertexContainer* pRes=GetExistedAt(PPath);
		if(pRes!=NULL)
			return *GetExistedAt(PPath);
		ULONGLONG Hash=ToHash(PPath);
		return m_Map.insert(MAP::value_type(Hash,StoredData(PPath)))->second;
	}
	StoredData* GetExistedAt(const _CModelVerticesPath& Path)
	{
		const MVerticesList& PPath=Path.GetList();
		ULONGLONG Hash=ToHash(PPath);
		pair<MAP::iterator,MAP::iterator> Range=m_Map.equal_range(Hash);
		for(MAP::iterator it=Range.first;it!=Range.second;it++)
			if(it->second.m_VerticesPath==PPath)
				return &(it->second);
		return NULL;
	}
	bool /*IsNew*/ ForceAt(const _CModelVerticesPath& Path,StoredData*& pResult)
	{
		const MVerticesList& PPath=Path.GetList();
		ULONGLONG Hash=ToHash(PPath);
		pair<MAP::iterator,MAP::iterator> Range=m_Map.equal_range(Hash);
		for(MAP::iterator it=Range.first;it!=Range.second;it++)
			if(it->second.m_VerticesPath==PPath )
			{
				pResult=&(it->second);
				return false;
			}
		pResult=&(m_Map.insert(MAP::value_type(Hash,StoredData(PPath)))->second);
		return true;
	}
	void clear() {m_Map.clear();}
protected:
	typedef unordered_multimap<ULONGLONG,StoredData> MAP;
	MAP m_Map;
};

class _CSubModelNumericParrernContainer : public _CModelVerticesPathBasedContainer, public _CSubModelNumericPattern
{
public:
	_CSubModelNumericParrernContainer() {}
	_CSubModelNumericParrernContainer(const MVerticesList& VerticesPath):_CModelVerticesPathBasedContainer(VerticesPath) {}
};

class _CSubModelNumericParrernStorage : public _CModelVerticesPathBasedStorage<_CSubModelNumericParrernContainer>
{
public:
	_CSubModelNumericPattern* GetPatrernIfExist(const _CModelVerticesPath& Path)
	{
		return GetExistedAt(Path);
	}
	bool ForcePattern(const _CModelVerticesPath& Path, _CSubModelNumericPattern*& pOutPattern)
	{
		_CSubModelNumericParrernContainer* pOut;
		bool Res=ForceAt(Path,pOut);
		pOutPattern=pOut;
		return Res;
	}
protected:
};

//_CPreFlatVertexList
//class _CPreFlatMap : public _CModelVerticesPathBasedStorage<_CPreFlatVertexContainer>
class _CPreFlatMap : public _CModelVerticesPathBasedStorage<_CPreFlatVertexList>
{
public:
	typedef enum{STILL_OLD,NEW_ONE,TERMINAL_REACHED,EMPTY} TRACE_RESULTS;
	_CPreFlatMap(_CMainCircuit* pMainCircuit):m_OldTrace(true),
		//TerminalVerticesPath(NULL),
		//m_pTopContainer(NULL),
		m_pMainCircuit(pMainCircuit) {}
	void StartTrace() {m_OldTrace=true;}
	bool ReturnedToOld(bool CurrentOld);
	TRACE_RESULTS DetermineTraceResult(bool CurrentNew);
	TRACE_RESULTS PreparePreFlatVC(const _CModelVerticesPath& Path,size_t SPower,size_t MaxPower,_CPreFlatVertexContainer*& pResult,_CCompRedefEntry& Entry);
	//TRACE_RESULTS PreparePreFlatVC(_CNewSimplifierData& Data,_CPreFlatVertexContainer*& pResult);
	//The path has returned to the old trace
	NumericType& UpdateValues(_CPreFlatVertexContainer* pTopContainer, _CCompRedefEntry& Entry)
	{
		return _UpdateValues(pTopContainer,/*COMPONENTS_PATH(TerminalVerticesPath),*/m_pMainCircuit,Entry);
	}
protected:
	NumericType& _UpdateValues(_CPreFlatVertexContainer* pTopContainer, /*const COMPONENTS_PATH& TerminalPath,*/_CMainCircuit* pMainCircuit,_CCompRedefEntry& RedefEntry);
	//_CModelVerticesPath TerminalVerticesPath;
	//_CPreFlatVertexContainer* m_pTopContainer;//???
	bool m_OldTrace;
	_CMainCircuit* m_pMainCircuit;
	friend class _CNewSimplifierData;
};


extern _CPreFlatVertexContainer OnePreFlatVertex;

class _CExpectedRange: public pair<NumericType,NumericType> 
{
public:
	_CExpectedRange(NumericType V1, NumericType V2):
		pair<NumericType,NumericType>(min(V1,V2),max(V1,V2)) {}
	bool IsInBeetween(NumericType V) {return V>=first && V<=second;} 
	bool IsZero() {return ::IsZero(first) && ::IsZero(second);}
};

class _CPreFlat2FlatCache
{
public:
	_CPreFlat2FlatCache() {}
	typedef pair<_CFlatVertex*,short> OUT_PAIR;
	bool Register(_CPreFlatVertexContainer* PreFV,_CFlatVertex* FV,short FVSgn);
	OUT_PAIR GetExisted(_CPreFlatVertexContainer* PreFV);
protected:
	typedef unordered_map<_CPreFlatVertexContainer*,OUT_PAIR> MAP;
	MAP m_Map;
};


class _CSimplificationEntry
{
public:
	_CSimplificationEntry(_CModelVertex* pEntryModelVertex,NumericType V1, NumericType V2,short TopSgn):
	  m_pEntryModelVertex(pEntryModelVertex),m_ExpectedRange(V1,V2),
		  m_pOutputEntry(NULL), m_OutSgn(1), m_TopSgn(TopSgn), m_FinalFlatVertex(NULL), m_FinalOutSign(1)
	{}
	  _CSimplificationEntry(const _CSimplificationEntry& Source):
	  m_pEntryModelVertex(Source.m_pEntryModelVertex),m_ExpectedRange(Source.m_ExpectedRange),
		  m_pOutputEntry(Source.m_pOutputEntry), m_OutSgn(Source.m_OutSgn), m_TopSgn(Source.m_TopSgn),
		  m_FinalFlatVertex(Source.m_FinalFlatVertex), m_FinalOutSign(Source.m_FinalOutSign)
	  {}
	_CSimplificationEntry& operator=(const _CSimplificationEntry& Source)
	{
		m_pEntryModelVertex=Source.m_pEntryModelVertex;
		m_ExpectedRange=Source.m_ExpectedRange;
		m_pOutputEntry=Source.m_pOutputEntry;
		m_OutSgn=Source.m_OutSgn;
		m_TopSgn=Source.m_TopSgn;
		m_FinalFlatVertex=Source.m_FinalFlatVertex;
		m_FinalOutSign=Source.m_FinalOutSign;
	}
	void FindSimplifications(_CNewSimplifierData& Data/*,size_t Power*/, _CCompRedefEntry& RedefEntry);
	void TranslatePreFlat2Flat(_CNewSimplifierData& Data,_CFlatVertexCache& OutCache);
protected:
	_CModelVertex* m_pEntryModelVertex;
	_CExpectedRange m_ExpectedRange;
	_CPreFlatVertexContainer* m_pOutputEntry;
	_CFlatVertex* m_FinalFlatVertex;
	short m_OutSgn;
	short m_TopSgn;
	short m_FinalOutSign;
	friend class _CSimplificationOrdinateur;
};


class _CSimplificationOrdinateur
{
public:
	_CSimplificationOrdinateur():m_MaxCofId(0),m_MaxPower(0) {}
	bool/*IsNew*/ RegisterEntry(unsigned long CofId, unsigned long Power,_CModelVertex* pEntryModelVertex,NumericType V1, NumericType V2,short TopSg);
	_CSimplificationEntry* GetPEntry(unsigned long CofId, unsigned long Power);
	void FindSimplifications(_CNewSimplifierData& Data, _CCompRedefEntry& RedefEntry);
	void TranslatePreFlat2Flat(_CNewSimplifierData& Data,_CFlatVertexCache& OutCache);
	void MoveTo(_CContextSExpFlatVertices& OutContextSExpFlatVertices,const string* Context);
	typedef pair<unsigned long/*CofId*/,unsigned long/*Power*/> COORDINATES;
protected:
	typedef map<COORDINATES,_CSimplificationEntry> MAP;
	MAP m_Map;
	unsigned long m_MaxCofId;
	unsigned long m_MaxPower;
};

class _CNewSimplifierData
{
public:
	_CNewSimplifierData(_CMainCircuit* pMainCircuit,
		_CContextSExpFlatVertices& ContextSExpFlatVertices,
		_CPathTraitor& InPathTraitor/*, 
		size_t MaxPower=UINT_MAX*/)
		:m_pMainCircuit(pMainCircuit),
		ModelVerticesPath(pMainCircuit),
		/*PreFlatMap(pMainCircuit),*/
		PathTraitor(InPathTraitor),
		m_MaxPower(0),
		OutContextSExpFlatVertices(ContextSExpFlatVertices),
		m_BackStatus(FORWARD),
		m_GreadyPentration(true),
		m_pPreFlatMap(NULL){} 
	_CMainCircuit* GetMainCircuit() const {return m_pMainCircuit;}
	unsigned long MaxPower() const {return m_MaxPower;}
	void FindSimplifications(_CCompRedefEntry& RedefEntry);
	bool/*IsNew*/ RegisterEntry(unsigned long CofId,unsigned long Power,_CModelVertex* pEntryModelVertex,NumericType ExactValue, NumericType Tolerance,short TopSgn);
	void NotifyParentsToBeUpdated(_CPreFlatVertexContainer* pChildVertex);
	void RegisterParent(_CPreFlatVertexContainer* pParentVertex);
	void RegisterParentAndUpdateValues(_CPreFlatVertexContainer* pParentVertex,_CPreFlatMap::TRACE_RESULTS Res);
	void UpdateParentsValue(_CPreFlatVertexContainer* pChildVertexToBeChange);
	void StartTrace() 
	{
		m_BackStatus=FORWARD;
		PreFlatMap().StartTrace();
	}
	void SartBack() {m_BackStatus=NEW_ONE_BACK;}
	bool IsNewOneBack() const { return m_BackStatus==NEW_ONE_BACK;}
	void SetFirstOldBack() {m_BackStatus=FIRST_OLD_BACK;}
	bool IsFirstOldBack() const {return m_BackStatus==FIRST_OLD_BACK;}
	bool IsJustBack() const {return m_BackStatus==JUST_BACK;}
	bool IsForward() const {return m_BackStatus==FORWARD;}
#ifdef _DEBUG
	NumericType m_PrevResult;
#endif
	const NumericType& GetAccuracy() {return PathTraitor.GetApproxCriterion()->GetAccurracy();}
	bool IsExact() const {return PathTraitor.GetApproxCriterion()->IsExact();}
protected:
	void SetJustBack() {m_BackStatus=JUST_BACK;}
	typedef enum{FORWARD,NEW_ONE_BACK,FIRST_OLD_BACK,JUST_BACK} BACK_STATUS;
public:
	class _CTraceStatusStore
	{
	protected:
		friend class _CNewSimplifierData;
		_CNewSimplifierData::BACK_STATUS m_OldBackStatus;
		bool m_OldTrace;
	};
	void StoreOldTraceStat(_CTraceStatusStore& Store) const
	{
		Store.m_OldBackStatus=m_BackStatus;
		Store.m_OldTrace=PreFlatMap().m_OldTrace;
	}
	void ReStoreOldTraceStat(const _CTraceStatusStore& Store)
	{
		m_BackStatus=Store.m_OldBackStatus;
		PreFlatMap().m_OldTrace=Store.m_OldTrace;
	}
	bool IsGready() {return m_GreadyPentration;}
	void ResetGready() {m_GreadyPentration=true;}
	void GreadyFinished() {m_GreadyPentration=false;}
	_CPreFlatMap& PreFlatMap() const {ASSERTPDD(m_pPreFlatMap!=NULL); return *m_pPreFlatMap;}
protected:
	bool RegisterParent(_CPreFlatVertexContainer* pChildVertex,_CPreFlatVertexContainer* pParentVertex);
	_CSimplificationOrdinateur SimplificationOrdinateur;
	_CPreFlat2FlatCache PF2FCache;
	//_CPreFlatMap PreFlatMap;
	_CModelVerticesPath ModelVerticesPath;
	_CPathTraitor& PathTraitor;
	_CSubModelNumericParrernStorage SubModelNumericParrernStorage; 
	_CFlatVertexCache& GetFlatVertexCache();
	_CMainCircuit* m_pMainCircuit;
	unsigned long m_MaxPower;
	typedef unordered_multimap<_CPreFlatVertexContainer*,_CPreFlatVertexContainer*> PARENT_MAP;
	PARENT_MAP m_ParentMap;
	BACK_STATUS m_BackStatus;
	bool m_GreadyPentration;
	friend class _CModelVertex;
	friend class _CTreeForkComponent;
	friend class _CSimplyComponent;
	friend class _CSubCircuitSocket;
	friend class _CSimplificationEntry;
	friend class _CPreFlatVertexContainer;
	friend class _CSimplificationOrdinateur;
	friend class _CPreFlatMap;
	friend class _CMetaComponent;
	_CContextSExpFlatVertices& OutContextSExpFlatVertices;
	_CSimplificationOrdinateur::COORDINATES m_TopCoordinates;
	typedef unordered_map<unsigned long long, _CPreFlatMap> PRE_FLAT_MAP;
	PRE_FLAT_MAP m_PreFlatMap;
	_CNewSimplifierData& SetCordinates(const _CSimplificationOrdinateur::COORDINATES& Cordinates);
	_CPreFlatMap* m_pPreFlatMap;
};

