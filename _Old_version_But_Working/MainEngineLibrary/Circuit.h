#pragma once
#include <deque>
#include <algorithm>
#include "NodesManager.h"
#include "StateGraphTables.h"
#include "Vertex.h"
#include "Cofactor.h"
#include "binary_filer.h"
#include "OperationCache.h"
#include "NumericalResTables.h"
#include "NextComponentOptimizer.h"
#include "DirectNumCache.h"
#include <iostream>
#include "NewCore.h"
#include "CompRedefiner.h"
using namespace std;
using namespace stdext;

class _CSubCircuit;

class _CContextSExpandedCofactorValues
{
public:
	~_CContextSExpandedCofactorValues()
	{
	}
	_CContextSExpandedCofactorValues():m_pBaseCircuit(NULL),m_Counter(0) {}
	typedef enum{MUST_EXIST, FORCE_IF_NOT_EXIST/*,INHERIT_FROM_PARENTS*/} TO_DO_IF_NOT_EXISTS;
	_CSExpandedCofactorValues& GetSExpandedCofValues(const string& Context,TO_DO_IF_NOT_EXISTS ToDo=FORCE_IF_NOT_EXIST,const string** ppNewConetxt=NULL);
	_CSExpandedVertices& GetSExpandedVertices(const string& Context,TO_DO_IF_NOT_EXISTS ToDo=FORCE_IF_NOT_EXIST,unsigned long CofId=0,const string** ppNewConetxt=NULL);
	_CSimpleVertexContainer& GetSExpandedCoef(const string& Context,TO_DO_IF_NOT_EXISTS ToDo=MUST_EXIST,unsigned sPower=0, unsigned long CofId=0,const string** ppNewConetxt=NULL, long* pSPowerShift=NULL);
	bool Exists(const string& Context, unsigned sPower, unsigned long CofId);
	void SetNoOfCofactors(const string& Context, size_t NewNo);
	void SetNoOfSCoef(const string& Context,unsigned long CofId,unsigned sPowers);
	_CSExpandedVertices& GetNewSVertices(const string& Context);
	unsigned long PushUniqueSExpandedVertices(const string& Context,const _CSExpandedVertices& Vertices);
	//bool /*Was Changed*/ RegisterAndPreparInputVertex(size_t EntryId, size_t PosId, const _CSubCircuitSocket* pSocket, _CSimpleVerticesMaps* pMap, _CSimpleVertexContainer& NewVertex, const string& Context, const string& PostContext); 
	bool /*Was Changed*/ RegisterAndPreparInputVertex(unsigned long EntryId, unsigned long PosId, 
		const _CSubCircuitSocket* pSocket, _CSubCircuitVerticesMap* pMap, const MVIDS& MVId,
		_CSimpleVertexContainer& NewVertex, const string& Context, const string& PostContext); 
	void WriteToStream(iostream& stream, _CSimpleVerticesMapsSet& Set);
	void WriteToStream(size_t CofId, iostream& stream, _CSimpleVerticesMapsSet& Set);
	//void WriteToStream(iostream& stream);
	//void WriteToStream(size_t CofId, iostream& stream);
protected:
	_CSimpleVertexContainer* FindSimpleVertexContainer(const string& Context,unsigned sPower, unsigned long CofId, const string** ppNewContext);
	friend class _CCircuit;
	unsigned long PushUniqueVertex(unsigned long EntryId, unsigned long PosId, _CSimpleVertexContainer& NewVertex, 
		const string& Context, const string& PostContext);
	typedef unordered_map<string,_CSExpandedCofactorValues> VMAP;
	typedef VMAP::iterator iterator;
	typedef VMAP::value_type value_type;
	typedef _CSExpandedCofactorValues::size_type size_type;
	template<typename OutType, typename OutType EmptyValue>
	class _CPos2Out : public unordered_map<unsigned long long, typename OutType>
	{
	public:
		void put(unsigned long long ID, typename OutType OutValue)
		{
			(*this)[ID]=OutValue;
		}
		typename OutType get(unsigned long long ID)
		{
			iterator it=find(ID);
			if(it!=end())
				return it->second;
			return EmptyValue;
		}
	};

	template<typename OutType, typename OutType EmptyValue>
	class _COutMap : public unordered_map<const string*,_CPos2Out<typename OutType,EmptyValue> > 
	{
	protected:
	unsigned long long Compose(unsigned long EntryId, unsigned long PosId)
		{
			TGlobal_Vertex_Id ID{0};
			ID.PARTS.Circuit_Id=EntryId;
			ID.PARTS.Local_Id=PosId;
			return ID.Global_Id;
		}
	public:
		void put(const string* CurrContext,unsigned long EntryId, unsigned long PosId, typename OutType OutValue)
		{
			if(OutValue==EmptyValue)
				return;
			unsigned long long Id=Compose(EntryId,PosId);
			(*this)[CurrContext].put(Id,OutValue);
		}
		typename OutType get(const string* CurrContext,unsigned long EntryId,unsigned long PosId)
		{
			iterator it=find(CurrContext);
			if(it!=end())
			{
				unsigned long long Id=Compose(EntryId,PosId);
				return it->second.get(Id);
			}
			return EmptyValue;
		}
	};
	//typedef _CPos2Out<const string*,&EmptyString> _CPos2NextContext;
	typedef _COutMap<const string*,&EmptyString> _CNextContextMap;
	VMAP m_vMap;
	_CNextContextMap m_NextContextMap;
	typedef _COutMap<long,0> _CNextSPower;
	_CNextSPower m_NextSPower;
	_CCircuit* m_pBaseCircuit;
	size_t m_Counter;
	void SetCircuit(_CCircuit* pBase) {m_pBaseCircuit=pBase;}
	_CSExpandedCofactorValues* GetSExpandedCofValues2(const string& Context,TO_DO_IF_NOT_EXISTS ToDo=FORCE_IF_NOT_EXIST,const string** ppNewConetxt=NULL);
	_CSExpandedVertices* GetSExpandedVertices2(const string& Context,TO_DO_IF_NOT_EXISTS ToDo=FORCE_IF_NOT_EXIST,unsigned long CofId=0,const string** ppNewConetxt=NULL);
	_CSimpleVertexContainer* GetSExpandedCoef2(const string& Context,TO_DO_IF_NOT_EXISTS ToDo=MUST_EXIST,unsigned sPower=0,unsigned long CofId=0,const string** ppNewConetxt=NULL);
};


class  _CCircuit : public _streamable
{
public:
	_CCircuit(bool IsLocal,bool OrderOptimalization,bool UseNewAnalyser,const string& GlobalRefNode=EmptyString);
	~_CCircuit(void);
	_CGraphTable* PushIntoCache(_CGraphTable* pObject);
	_CIntegrityTable* PushIntoCache(_CIntegrityTable* pObject);
	_CGraphState* PushIntoCache(_CGraphState* pObject);
	_CModelVertex* PushIntoCache(_CModelVertex* pObject);
	_CVertex* PushIntoCache(_CVertex* pObject);
	virtual void SetOptimizeComponentOrder(bool Optimize) {m_OptimizeComponentOrder=Optimize;}
	virtual _CSubCircuit* GetSubcircuit(const string& name)=0;
	virtual _CSubCircuit* GetSubcircuit(unsigned long Id)=0;
	virtual unsigned long GetCircuitId(const _CCircuit* pCircuit)=0;
	virtual bool IsSExpanded()=0;
	unsigned long GetMyId() {return GetCircuitId(this);}
	virtual _CCircuit* GetCircuit(unsigned long Id)=0;
	vector<int> TranslateOrAddNodes(const vector<string>& Nodes);
	int TranslateOrAddNode(const string& Node);
	int TranslateNode(const string& Node);
	//const string& TranslateNode(int Node); 
	string TranslateNode(int Node); 
	//*****************Component's Inclusion
	long AddResistor(const string& name, const string& NodeA, const string& NodeB,bool IsConductance=false,long double Value=0.0);
	long AddCapacitor(const string& name, const string& NodeA, const string& NodeB,long double Value=0.0);
	long AddInductor(const string& name, const string& NodeA, const string& NodeB,long double Value=0.0);
	long AddTransconductance(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value, int sPower);
	long AddTransimpedance(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value, int sPower);
	long AddVoltageGain(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value, int sPower);
	long AddCurrentGain(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value, int sPower);
	long AddNullor(const string& OutP, const string& OutM, const string& InP, const string& InM);
	long AddDVCCII(const string& Ix, const string& Iy1, const string& Iy2, const string& Iz1, const string& Iz2, const string& GND = EmptyString);
	long AddFDCCII(const string& Vx1, const string& Vx2, const string& Vy1, const string& Vy2, const string& Vy3, const string& Vy4, 
		const string& Iz1, const string& Iz2, const string& GND = EmptyString);
	long AddFDCFOA_RM_INF(const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn, 
		const string& Izp, const string& Izn, const string& Vwp, const string& Vwn);
	long AddFDCFOA_RM(const string& name, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
		const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long double Value = 0.0);
	long AddPathologicalPair(const string& OutP, const string& OutM, const string& InP, const string& InM, bool CurrentMirror = false, bool VoltageMirror = false);

	long AddVoltageFollower(const string& OutP, const string& OutM, const string& InP, const string& InM);
	long AddCurrentFollower(const string& OutP, const string& OutM, const string& InP, const string& InM);

	long AddOpAmp(const string& Out, const string& II, const string& NI);
	long AddCurrentConveyor(const string& X, const string& Y, const string& Z, short Type);
	//long AddMOCCII(const string& X, const string& Y, const vector<string>& Zs, short Type);
	long AddBOCII(const string& X, const string& Y, const string& Zp, const string& Zm, short Type);
	long IncludeSubcircuit(const string& InstanceName, const string& ModelName, const _CStrNodes& Connections, long InternalCofactor = 1);
	long IncludeSubcircuit(const string& InstanceName, long ModelID, const _CStrNodes& Connections, long InternalCofactor = 1);
	long ForceModelCalculation(long ModelID, bool IgnoreSgnPropagation=false);
	inline bool IsSubcircuit() {return m_IsLocal;}
	_CComponent* PrepareAndGetNextComponent(bool& IsLast);
	void PrepareCofactors();
	unsigned long CreateNewCofactor();
	long getCofactor(unsigned long hCofactor, _CCofactor*& pCofacotor);
	_CCofactor* GetCofactor(unsigned long CId);
	unsigned NoOfNodes() const {return m_LocalNodes.GetCount();}
	unsigned HighestNoOfNode() const {return m_LocalNodes.GetHighestNo();}
	virtual unsigned NoOfBoundaryNodes() {return NoOfNodes();}
	_CModelVertex* GetRegisteredMVertex(_CGraphState* pInputState);
	_CModelVertex* GetRegisteredMVertex(_CModelVertex* pSourceVertex);
	virtual const string& GetName() const =0;
	//long long GetGraphTableIndex(_CGraphTable* pTable) {return m_GraphTableCache.GetIndexOf(pTable,false);}
	long long GetSourceIntegrityTableIndex(_CIntegrityTable* pTable) {return m_AdvIntegrityTableCache.GetSource().GetIndexOf(pTable,false);}
	long long GetTargetIntegrityTableIndex(_CIntegrityTable* pTable) {return m_AdvIntegrityTableCache.GetTarget().GetIndexOf(pTable,false);}
	_CGraphTable* GetGraphTableFromIndex(long long Index) {return m_GraphTableCache[Index];}
	//_CIntegrityTable* GetIntegrityTableFromIndex(long long Index) {return m_IntegrityTableCache[Index];}
	_CIntegrityTable* GetSourceIntegrityTableFromIndex(long long Index) {return m_AdvIntegrityTableCache.GetSource()[Index];}
	_CIntegrityTable* GetTargetIntegrityTableFromIndex(long long Index) {return m_AdvIntegrityTableCache.GetTarget()[Index];}
	long long GetStateTableIndex(_CGraphState* pTable); 
	_CGraphState* GetStateTableFromIndex(long long Index); 
	long long GetModelVertexIndex(_CModelVertex* pVertex);
	_CModelVertex* GetModelVertexFromIndex(long long Index);
	long GetComponentIndex(const _CComponent* pComponent) {return m_LocalComponentsList.Comp2Index(pComponent);}
	_CComponent* GetComponentFromIndex(long Index) {return m_LocalComponentsList.Index2Comp(Index);}
	virtual const path_str& GetModelsPath() =0;
	void Skip0SuppresesVertices();
	virtual _CSubCircuit* PrepareSubcircuitFromName(const string& Name, const path_str& ModelFile)=0;
	_CVertex* GlobalVertexFromId(unsigned long long Id);
	long long GlobalIdFromLocalVertex(_CVertex* pVertex);
	void TouchVertex(long long iVertex);
	void UnTouchVertex(long long iVertex);
	void CacheTheResult(_CRealGraphDevelopement* pCacheContainer)
	{
		m_DevelopingVerticesCache.CacheTheResult(pCacheContainer);
	}
	//void FindResult(_CRealGraphDevelopement* pCacheContainer, _CSExpandedVertices*& pSEVerteces, short& ResSgn)
	//{
	//	return m_DevelopingVerticesCache.FindResult(pCacheContainer,pSEVerteces,ResSgn);
	//}
	_CRealGraphDevelopement* FindOrRegisterResult(_CRealGraphDevelopement* pCacheContainer)
	{
		return dynamic_cast<_CRealGraphDevelopement*> (m_DevelopingVerticesCache.FindOrRegisterResult(pCacheContainer));
	}
	void RegisterResult(_CRealGraphDevelopement* pCacheContainer)
	{
		return m_DevelopingVerticesCache.RegisterResult(pCacheContainer);
	}
	_CRealGraphDevelopement* FindExisting(_CRealGraphDevelopement* pCacheContainer)
	{
		return dynamic_cast<_CRealGraphDevelopement*> (m_DevelopingVerticesCache.FindExisting(pCacheContainer));
	}

	virtual void CacheTheResult(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket);
	//virtual _CSubModelNumericPattern* FindResults(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket);
	virtual void RegisterResult(_CDirectNumContainer* pCacheContainer);
	//virtual void RegisterResult(_CDirectNumContainer* pCacheContainer, _CSubCircuitSocket* pMasterSocket);
	//virtual _CDirectNumContainer* FindExisting(_CDirectNumContainer* pCacheContainer, _CSubCircuitSocket* pMasterSocket);
	virtual _CDirectNumContainer* FindExisting(_CDirectNumContainer* pCacheContainer);

	virtual size_t GetOuterVertexIndex(_CModelVertex* pMVertex)=0;
	//virtual short GetOuterVertexSign(_CModelVertex* pMVertex)=0;
	virtual void GetOuterVertex(_CSExpandedVertices*& pVertices, _CModelVertex* pMVertex, _CSExpandedDescendends& TerminalVertices)=0;
	_CVertex* GetSimpleVertex(_CComponent* pParameter, 
		const _CSimpleVertexContainer& hSucc, const _CSimpleVertexContainer& lSucc, bool Negated=false);
	_CSimpleVertexContainer GetSimpleVertexId(_CComponent* pParameter, 
		const _CSimpleVertexContainer& hSucc, const _CSimpleVertexContainer& lSucc, short Mult1=1, short Mult0=1, short MultTop=1,bool Negated=false);
	bool IsDestroyingInProgress() {return m_DestroyingInProgess;}
	void WriteCircuitModelToStream(iostream& stream, bool WithCofactros = true);
	void WriteDescriptionToStream(iostream& stream);
	virtual void WriteHeader(iostream& stream)=0;
	string ConvertNode(int NodeNo) {return m_LocalNodes.ConvertNode(NodeNo);}
	void WriteSimpleVertices(long CircuitId,iostream& stream, _CSimpleVerticesMapsSet& Set);
	//void WriteSimpleVertices(long CircuitId,iostream& stream);
	_CComponent* Name2Component(const string& Name) const;
	const string& Component2Name(const _CComponent* pComponent) const; 
	//_CSExpandedVertices& SetMainSimpleVertices(_CSExpandedVertices& Vertices, short GlobalMultiplier) 
	//{
	//	m_MainVertices=Vertices;
	//	for each(_CSimpleVertexContainer Vert in m_MainVertices)
	//		Vert.MultiplyMultiplier(GlobalMultiplier);
	//	return m_MainVertices;
	//}
	_CSExpandedVertices& GetMainSVertices(unsigned IdNo,bool ForceIfNotExist);
	_CSExpandedVertices& GetNewSVertices();
	unsigned long PushLocalSVertices(const _CSExpandedVertices& Vertices,const string& Context=EmptyString);
	unsigned GetSVerticesIndexForCofactor(_CCofactor* pCofactor);
	_CSExpandedVertices& GetSVerticesForCofactor(_CCofactor* pCofactor,const string& Context=EmptyString) 
	{
		return m_CSSimpleMainVertices.GetSExpandedVertices(Context,_CContextSExpandedCofactorValues::MUST_EXIST,GetSVerticesIndexForCofactor(pCofactor));
		//return m_SimpleMainVertices[GetSVerticesIndexForCofactor(pCofactor)];
	}
	_CSExpandedVertices& GetSVerticesForCofactor(unsigned  CofId,const string& Context=EmptyString) 
	{
		return m_CSSimpleMainVertices.GetSExpandedVertices(Context,_CContextSExpandedCofactorValues::MUST_EXIST,CofId);
	}
	//_CSExpandedVertices& GetSVerticesForCofactorWithInheritance(unsigned  CofId,const string& Context,const string** ppNewContext) 
	//{
	//	return m_CSSimpleMainVertices.GetSExpandedVertices(Context,_CContextSExpandedCofactorValues::INHERIT_FROM_PARENTS,CofId,ppNewContext);
	//}
	//_CSimpleVertexContainer& GetSVertexForCofactorWithInheritance(unsigned  CofId,unsigned short sPower,const string& Context,const string** ppNewContext) 
	//{
	//	return m_CSSimpleMainVertices.GetSExpandedCoef(Context,_CContextSExpandedCofactorValues::INHERIT_FROM_PARENTS,sPower,CofId,ppNewContext);
	//}
	_CSimpleVertexContainer& GetSVertexForCofactor(unsigned  CofId,unsigned short sPower,const string& Context,const string** ppNewContext, long* pSPowerShift) 
	{
		return m_CSSimpleMainVertices.GetSExpandedCoef(Context,_CContextSExpandedCofactorValues::MUST_EXIST,sPower,CofId,ppNewContext,pSPowerShift);
	}
	_CComponent* FindComponent(const string& Name);
	short CompLocalComponents(const _CComponent* pLeft, const _CComponent *pRight) 
	{
		return m_LocalComponentsList.ComparePosition(pLeft,pRight);
	}
	bool /*Was Changed*/ RegisterAndPreparInputVertex
		(unsigned long EntryId, unsigned long PosId, const _CSubCircuitSocket* pSocket, _CSubCircuitVerticesMap* pMap,
		const MVIDS& MVId,
		_CSimpleVertexContainer& NewVertex, const string& Context, const string& PostContext)
	{
		return m_CSSimpleMainVertices.RegisterAndPreparInputVertex(EntryId,PosId,pSocket,pMap,MVId,NewVertex, Context,PostContext);
	}
	//virtual const string* GetParentContext(const string& Child, size_t& ParentNo)=0;
	void EnumCofoctorForContexts(_CCofactor* pCofactor, iostream& stream,_CSimpleVerticesMapsSet& Set);
	//void EnumCofoctorForContexts(_CCofactor* pCofactor, iostream& stream);
	void WriteInputConnectionEntry(size_t CofId, iostream& stream, _CSimpleVerticesMapsSet& Set)
	//void WriteInputConnectionEntry(size_t CofId, iostream& stream)
	{
		m_CSSimpleMainVertices.WriteToStream(CofId, stream, Set);
		//m_CSSimpleMainVertices.WriteToStream(CofId, stream);
	}
	void WriteInputConnectionEntry(iostream& stream, _CSimpleVerticesMapsSet& Set)
	//void WriteInputConnectionEntry(iostream& stream)
	{
		m_CSSimpleMainVertices.WriteToStream(stream, Set);
		//m_CSSimpleMainVertices.WriteToStream(stream);
	}
	virtual size_t FirstInternalNode() {return 0;}
	void ClearPostSimpleVerticesDetermination();
	long long ModelVertex2Index(_CModelVertex* pMVertex);
	const _CModelVertex* Index2ModelVertex(long long Index);
	virtual _CSExpandedVertices* GetUniqueSEExpandedVertices(_CSExpandedVertices& Source)=0;
	virtual void ClearDevelopingVerticesCache() {m_DevelopingVerticesCache.Clear();}
	virtual const string* GetUniqueContextStringName(const string* String)=0;
	virtual const string& GetUniqueContextStringName(const string& String)=0;
	void PreparDivider(_CComponent* pToComponent);
	void PreparReminder(_CComponent* pToComponent);
	virtual void FindDuplicateLeaf(_CModelVertex* pOriginalVertex, _CModelVertex*& pNonDuplicateVertex,
		short& MovedSign, MVERTEX_2_SIGN* MVertex2Sgn);
	virtual const string& GetGlobalModelFileName() const =0;
	virtual const string& GetGlobalCircuitFileName() const =0;
	virtual const string& GetGlobalRaportFileName() const =0;
	virtual _CModelVertex* CountTerminals(_CModelVertex* pNewVertex) {return pNewVertex;}
	virtual void ClearTerminals() {}
	virtual bool IsDeletionAdmissibleDef() {return false; }
	virtual bool IsPromising(_CGraphState* pState) {return true;}
	virtual void PostProcessComponent(_CComponent* pJustProcessedComponent) {}
	long IncludeSubcircuit(const string& InstanceName, _CSubCircuit* pModel, const _CNodesConnections& Connections, long InternalCofactor = 1);
	long IncludeSubcircuit(const string& InstanceName, _CSubCircuit* pModel, const _CIntNodes& ConnectionsTable, long InternalCofactor = 1);
	unsigned long MaxPower() {return m_LocalComponentsList.MaxPower();}
	_CMainCircuit* GetMainTopCircuit();
	bool ModifySubParameter(const string& ParamPath, long double NewValue);
	const _CCompRedefEntry& GetComponentRedefEntry(_CComponent* pComp);
	const _CCompRedefEntry& GetComponentRedef() { return m_CirCompRedefEntry; }
	const _CCompRedefEntry* GetUniqCompRedef(_CCompRedefEntry* pOrgRedefEntry) { return m_UniqCompRedEntires.GetUniq(pOrgRedefEntry); }
	bool IsGlobal0Defined() { return m_IsGlobal0Node;  }
	long SetCompPriority(size_t Priority) { m_CompPriority = Priority; return 0; }
	long ClearCompPriority() { return SetCompPriority(numeric_limits<size_t>::max()); }
protected:
	size_t m_CompPriority = numeric_limits<size_t>::max();
	friend class _CSparsePolynomialCache::DATA2;
	_CDirectNumCache m_DirNumCache;
	_CCompRedefEntry m_CirCompRedefEntry;
	_CUniqCompRedefEntry m_UniqCompRedEntires;//Zapisz to
	virtual void PrepareNewCircuitAnalyser()=0;
	void KillNewCricuitAnalyser()
	{
		//if(m_pNewAnalyxer!=NULL)
		//	m_pNewAnalyxer->ClearGarbage();
		delete m_pNewAnalyxer;
		m_pNewAnalyxer=NULL;
	}
	virtual void CleanExtraMaps() {}
	virtual void WriteOutParamsToTextStream(iostream& stream) {}
	bool TestNameUniqness(const string& Name);
	void Skip0SupressedAndDuplicated(MVERTEX_2_SIGN* pMVertex2Sgn,bool IgnoreSgnPropagation);
	//void SkipDuplicates();
	//void GetSimpleTree(_CSExpandedVertices& TreeVertices, unsigned CofId, _CSExpandedDescendends& ExternalVertices);
	DECLARE_CLASS_NAME(_CCircuit);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	virtual unsigned long FindSubId(const string& Name)=0;
	void ClearGarbage(MVERTEX_2_SIGN* pMVertex2Sgn=NULL,bool IgnoreSgnPropagation=false);
	//void CNodesConnections2IntNodes(const _CNodesConnections& Connections, _CIntNodes IntNodes);
	bool m_IsGlobal0Node;
	void CreateModelPDD();
	void TranslateTempVertices2ModelVertices();
	virtual void PlugComponents(_CIntegrityTable& IntegrityTable);
	bool m_IsLocal;
	bool m_UseNewAnalyser;
	_CNodesManager m_LocalNodes;
	_CGraphTableCache m_GraphTableCache;
	//_CIntegrityTableCache m_IntegrityTableCache;
	class _CAdvIntegrityTableCache
	{
	public:
		_CAdvIntegrityTableCache():m_Counter(0),m_2Cache(2) {}
		_CIntegrityTableCache& GetSource() {return m_2Cache[(size_t)m_Counter%2];}
		_CIntegrityTableCache& GetTarget() {return m_2Cache[((size_t)m_Counter+1)%2];}
		void SwapAndPrepare()
		{
			m_Counter++;
			GetTarget().Clear();
		}
		void Clear()
		{
			m_2Cache[0].Clear();
			m_2Cache[1].Clear();
		}
	protected:
		unsigned long m_Counter;
		vector<_CIntegrityTableCache> m_2Cache;
	};
	_CAdvIntegrityTableCache m_AdvIntegrityTableCache;
	_CGraphStateCache m_GraphStateCache;
	_CModelVerticesCache m_ModelVerticesCache;
	_CModelVerticesList m_VerticesInAnalysis;
	_CModelVerticesList m_NewCreatedVertices;
	typedef _CModelVerticesList::iterator MODEL_V_IT;
//	_CVerticesCache m_VertexCache;
	bool m_OptimizeComponentOrder;
	bool m_ComponentListFinished;
	_CComponents m_LocalComponentsList;
	_CComponents m_IdealComponentsList;
	_CNextComponentOptimizer* m_pNextComponentOptimizer;
	typedef _CComponents::iterator COMP_IT;
	//COMP_IT m_CurrentComponentIt;
//	_CCofactorsList m_CofactorsList;
	typedef _CCofactorsVector::iterator COFACTORS_IT;
	_CCofactorsVector m_Cofactors;
	friend class _CComponent;
	friend class _CSubCircuit;
	friend class _CMainCircuit;
//	friend class _CRequrrentChecker;
	//class _CRecurrentChecker : public deque<_CCircuit*>
	//{
	//public:
	//	bool IsAlreadyCaller(_CCircuit* pSubCircuit) {return find(begin(),end(),pSubCircuit)!=end();}
	//protected:
	//};
	//void CheckRecurrentPath(_CRecurrentChecker& RecChecker);
	_CVertexCache m_SimpleLocalVerticesCache;
//	_CSimpleVertexVector m_InSimpleVertices;
//	_CSimpleVertexVector m_OutSimpleVertices;
	_COperationCache m_DevelopingVerticesCache;
	//_CDirectNumCache m_DirNumCache;
	bool m_DestroyingInProgess;
	//_CSExpandedCofactorValues m_SimpleMainVertices;
	//_CLocalSimpleVertexStore m_LocalInputVertexStore;
	_CContextSExpandedCofactorValues m_CSSimpleMainVertices;
	typedef set<string> UQSet;
	UQSet m_UniqeNamesSet;
	unsigned m_LargestDescs; 
	_CCircuitAnalyzer* m_pNewAnalyxer;
	typedef map<string, _CComponent*> UNIQ_NAMES_MAP;
	UNIQ_NAMES_MAP m_UniqNamesMap;
};

//class eRecurrentLoop : public exception
//{
//public:
//	eRecurrentLoop(const char* Message=""):exception(Message) {}
//};

