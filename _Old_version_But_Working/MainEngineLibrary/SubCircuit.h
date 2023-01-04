#pragma once
#include "Circuit.h"
#include "NodesManager.h"
#include "binary_filer.h"
#include "RedefParamValues.h"
//#include "SubCircuitDeveloper.h"

//class ORIGINAL_DESC_STATES;

class _CSubCircuit : public _CCircuit
{
public:
	//_CSubCircuit(_CCircuit* Owner,bool OrderOptimalization ,const string& Name, const _CStrNodes& InjectedBoundaryNodes, const string& GlobalRefNode, bool MustBeNew,const SIZE_T_SET* pDeletionAdmissible);
	_CSubCircuit(_CCircuit* Owner,bool OrderOptimalization,bool UseNewAnalyser, const string& Name, const _CStrNodes& InjectedBoundaryNodes, const string& GlobalRefNode, bool MustBeNew,const SIZE_T_RANGE* pDeletionAdmissible);
	_CSubCircuit(_CCircuit* Owner, bool OrderOptimalization,bool UseNewAnalyser ,const string& Name, const path_str FileName, const string GlobalRefNode, bool MustBeNew);
	_CSubCircuit(_CCircuit* Owner, const string& Name, const string& GlobalRefNode, bool MustBeNew,bool UseNewAnalyser);//Only for DynNet
	void DefineBoundaryBounds(const _CStrNodes& InjectedBoundaryNode);
	void DefineBoundaryBoundsAndConnect(const _CIntNodes& InjectedConnectionNodes);
	void ForceBoundariesAndConnect(const INT_INT_MAP& Connections);
	void ReorderNodesIfNeccessary();
	bool AddBoudaryNode(string& Node);
	bool AddBoudaryNode(int Node);
	~_CSubCircuit(void);
	virtual _CSubCircuit* GetSubcircuit(const string& name);
	virtual _CSubCircuit* GetSubcircuit(unsigned long Id) override;
	unsigned long GetCircuitId(const _CCircuit* pCircuit) override {return m_Owner->GetCircuitId(pCircuit);}
	_CCircuit* GetCircuit(unsigned long Id) {return m_Owner->GetCircuit(Id);}
//	unsigned GetNoOfBoudnaryNodes() {return m_NoOfBoundaryNodes;}
	const string& GetNameOfBoundaryNodes(unsigned Index) {return m_BoundaryNodes[Index];}
	void ForceGlobalRefNode(const string& RefName);
	int ConvertBoundaryNode(const string& sNode);
	bool IsReady() {return m_IsReady;}
	const string& GetName() const {return m_Name;}
	void RequestModelAnalysis(bool IgnoreSgnPropagation=false) {PrepareSubModelPDD(IgnoreSgnPropagation);}//w przysz³oœci zmieniæ do wielow¹tkowej wersji
	const path_str& GetModelsPath() {return m_Owner->GetModelsPath();}
	const path_str& GetSubcircuitModelPath();
	bool IsModelInFile();
	void LoadModel();
	void StoreModel();
	unsigned NoOfBoundaryNodes();
	unsigned NoOfBoundaryParams() {return (unsigned)m_BoundaryParameters.size();}
	//unsigned NoOfPotentialBoundaryParams() {return m_}
	_CMultiDeletions* PushIntoCache(_CMultiDeletions* pObject);
	bool GetBoundaryParamsEntry(path_str& Path);
	path_str PrepareShortFileName();
	path_str PrepareFileName();
	_CSubCircuit* PrepareSubcircuitFromName(const string& Name, const path_str& ModelFile);
	virtual size_t GetOuterVertexIndex(_CModelVertex* pMVertex);
	//virtual short GetOuterVertexSign(_CModelVertex* pMVertex);
	virtual void GetOuterVertex(_CSExpandedVertices*& Vertices, _CModelVertex* pMVertex, _CSExpandedDescendends& TerminalVertices);
	void DevelopeRealTree(const string& SocketName,_CSExpandedVertices*& pTreeVertices, _CSExpandedDescendends& ExternalVertices,
		short& TopSgn,const _CDescSigns& BottomSgns, bool Approximated, const NumericType& Accuracy);
	//void DevelopeRealTree( _CSExpFlatVertices*& pOutResVertices, 
	//	                                   _CFlatVerticesResContainer*& pCacheContainer,
	//									   const _CSparsePolynomial& TopNumValue,
	//									   const _CSPowerLimiter& Limiter,
	//									   const _CSubModelNumericPattern& Scaler,
	//									   _CMultLevelSExpFlatVertices& TerminalVertices, 
	//									   //short& TopSgn, 
	//									   //const _CDescSigns& BottomSgns,
	//									   _CPathTraitor& MainTraitor,
	//									   //_CModelVerticesTraitor& VerticesTraitor);
	//									   _CModelVerticesPath& VerticesPath);
	void WriteHeader(iostream& stream);
	_CMultiDeletions* MVertex2Dels(_CModelVertex* pVertex);
	bool IsSExpanded() {return m_Owner->IsSExpanded();}
	_CSubCircuitVerticesMap* RegisterMap(_CSubCircuitVerticesMap* pMap)
	{
		return m_MapsBuffer.RegisterMap(pMap);
	}
	size_t FirstInternalNode();
	int GetHollowOutput();
	virtual _CSExpandedVertices* GetUniqueSEExpandedVertices(_CSExpandedVertices& Source)
	{
		return m_Owner->GetUniqueSEExpandedVertices(Source);
	}
	//virtual const string* GetParentContext(const string& Child, size_t& ParentNo) {return m_Owner->GetParentContext(Child,ParentNo);}
	//void DetermineNumericPattern(_CSubCircuitSocket* pSocket,_CModelNumericPattern& NumericPattern);
	void DetermineNumericPattern(_CSubCircuitSocket* pSocket,_CSubModelNumericPattern& NumericPattern);
	//_CSubModelNumericPattern& GetExistingNumPoly(const string* Context, const _CRedefParamValues& RedefParams, _CSubCircuitSocket*& pOriginalSocket)
	//{
	//	return m_NumResultsCache.GetExisting(Context, RedefParams, pOriginalSocket);
	//}
	_CSubModelNumericPattern& GetExistingNumPoly(const string* Context, const _CCompRedefEntry* pRedefParams)
	{
		return m_NumResultsCache.GetExisting(Context, pRedefParams);
	}
	void NotifyReadyNew(const string* Context, const _CCompRedefEntry* pRedefParams)
	{
		m_NumResultsCache.NotifyReadyNew(Context, pRedefParams);
	}
	//void NotifyReadyNew(const string* Context, const _CRedefParamValues& RedefParams)
	//{
	//	m_NumResultsCache.NotifyReadyNew(Context, RedefParams);
	//}
	//void RegisterNumPoly(const _CRedefParamValues& RedefParams,const _CSubModelNumericPattern* Polynomial)
	//{
	//	m_NumResultsCache.Register(RedefParams,Polynomial);
	//}
	void ClearNumResultCache()
	{
		m_NumResultsCache.clear();
	}
	virtual const string* GetUniqueContextStringName(const string* String)
	{
		return m_Owner->GetUniqueContextStringName(String);
	}
	virtual const string& GetUniqueContextStringName(const string& String)
	{
		return m_Owner->GetUniqueContextStringName(String);
	}
	virtual void FindDuplicateLeaf(_CModelVertex* pOriginalVertex, _CModelVertex*& pNonDuplicateVertex,
		short& MovedSign,MVERTEX_2_SIGN* pMVertex2Sgn);
	inline void WriteIntermediateNumericPolynomials(iostream& stream)
	{
		m_NumResultsCache.WriteIntermediateNumericPolynomials(stream);
	}
	virtual const string& GetGlobalModelFileName() const 
	{ 
		ASSERTPDD(m_Owner!=NULL);
		return m_Owner->GetGlobalModelFileName();
	}
	virtual const string& GetGlobalCircuitFileName() const 
	{ 
		ASSERTPDD(m_Owner!=NULL);
		return m_Owner->GetGlobalCircuitFileName();
	}
	virtual const string& GetGlobalRaportFileName() const 
	{ 
		ASSERTPDD(m_Owner!=NULL);
		return m_Owner->GetGlobalRaportFileName();
	}
	virtual _CModelVertex* CountTerminals(_CModelVertex* pNewVertex) 
	{
		m_TerminalsCounter[pNewVertex]++;
		return pNewVertex;
	}
	virtual void ClearTerminals() {m_TerminalsCounter.clear();}
	//virtual bool IsDeletionAdmissibleDef() {return m_DeletionAdmissibleDef;}
	bool IsDeletionAdmissible(const _CMultiDeletions* pDeletions);
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
								  _CNewSimplifierData& Data, _CCompRedefEntry& RedefEntry);
	bool IsModelSymetric() const;
	virtual bool IsPromising(_CGraphState* pState) override;
	virtual void PostProcessComponent(_CComponent* pJustProcessedComponent) override;
	void SetRankLimits(const SIZE_T_RANGE& Range);
protected:
	_CBounadryStatesVector* m_pNewDescndantsState;
	virtual void PrepareNewCircuitAnalyser() override 
	{
		COMPONENT_PINS_SET Set;
		if(m_IsGlobal0Node)
			Set.insert(0);
		for(_CStrNodes::iterator it=m_BoundaryNodes.begin(),_e=m_BoundaryNodes.end();it!=_e;it++)
		{
			Set.insert(m_LocalNodes.ConvertNode(*it));
		}
		m_pNewAnalyxer=new _CCircuitAnalyzer(this,m_IdealComponentsList,m_LocalComponentsList,true,&Set,m_Cofactors);
	}
	virtual void CleanExtraMaps();
	//void TranslateExtVertices2LocTerm(const _CSimpleVertexVector& External,_CSimpleVertexMap& Local);
	virtual void WriteOutParamsToTextStream(iostream& stream);
	unsigned long FindSubId(const string& Name) {return m_Owner->FindSubId(Name);}
	void TermLeave2Param(_CModelVertex* pVertex,MVERTEX_2_SIGN& MVertex2Sgn);
	long long GetBoundaryParamIndex(_CMultiDeletions* pDel)
	{
		return m_BoundaryParameters.GetIndexOf(pDel, false);
	}
	//long long GetBoundaryParamIndex(_CDeletions* pDel)
	//{
	//	return m_BoundaryParameters.GetIndexOf(pDel, false);
	//}
	long long GetBoundaryParamIndex(_CModelVertex* pLeafVertex);
	long long GetBoundaryParamIndexIfUnique(_CModelVertex* pLeafVertex);
	//_CDeletions* GetBoundaryParamFromIndex(long long Index) { return m_BoundaryParameters[Index]; }
	_CMultiDeletions* GetBoundaryParamFromIndex(long long Index) { return m_BoundaryParameters[Index]; }
	DECLARE_CLASS_NAME(_CSubCircuit);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	string m_Name;
	void PrepareSubModelPDD(bool IgnoreSgnPropagation=false);
	void FinalizeLeaves(MVERTEX_2_SIGN& MVertex2Sgn);
	void NewFinalizeLeaves(MVERTEX_2_SIGN& MVertex2Sgn);
	virtual void PlugComponents(_CIntegrityTable& IntegrityTable);
	_CCircuit* m_Owner;
	_CStrNodes m_BoundaryNodes;
	typedef unordered_map<string,int> BOUNDARY_NODES_MAP;
	typedef BOUNDARY_NODES_MAP::value_type BOUNDARY_NODES_TYPE;
	typedef BOUNDARY_NODES_MAP::iterator BOUNDARY_NODES_IT;
//	int m_NoOfBoundaryNodes;
	BOUNDARY_NODES_MAP m_BoundaryNodesMap;
	_CBounderyParametersCache m_BoundaryParameters;
	bool m_IsReady;
//	_CCofactorsVector m_BoundaryCofactors;
	typedef unordered_map<_CModelVertex*,_CMultiDeletions*>  MVERTEX_2_DELS;
	typedef unordered_multimap<_CMultiDeletions*,_CModelVertex*> DELS_2_MVERTEX;
	//class _CMTerminalVertex2STerminalVertex : public unordered_map<_CModelVertex*,_CSimpleVertexContainer>
	//{
	//};
	DELS_2_MVERTEX m_Dels2VertexMap;
	MVERTEX_2_DELS m_Vertex2Dels;
	//MVERTEX_2_SIGN m_Vertex2Sign;
	friend class _CModelVertex;
	friend class _CSubCircuitSocket;
	friend class _CCircuit;
	path_str m_ModelPath;
	long long m_InterfaceOffset;
	long long m_NumericOffset;
	friend class _CSparsePolynomialCache;
	inline long long GetNumericOffset() {return m_NumericOffset;}
	bool m_MustBeNew;
	//_CSimpleVertexVector m_LocalTerminalSimpleVertex;
	//typedef _CMTerminalVertex2STerminalVertex::value_type MVERTEX_SVERTEX_PAIR;
	//_CMTerminalVertex2STerminalVertex m_LocalBoundarySimpleVertices;
	_CSubCircuitVerticesMapBuffer m_MapsBuffer;
	int m_HollowOutput;
	bool m_IsHollowDetermined;
	_CSparsePolynomialCache m_NumResultsCache;
	typedef unordered_map<_CModelVertex*,unsigned long> TERMINALS_COUNTER;
	TERMINALS_COUNTER m_TerminalsCounter;
	//SIZE_T_SET m_DeletionAdmissible;
	SIZE_T_RANGE m_DeletionAdmissible;
	size_t m_MaxPossibleDeletions;
	void PreparePromissing();
	bool m_DeletionAdmissibleDef;
	friend class _CDuplicatedVertexCatcher;
	bool IsSuppresedLeaf(_CModelVertex* pLeaf);
	bool m_IsSpecialNetDynamic;
};
