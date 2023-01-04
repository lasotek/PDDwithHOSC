#pragma once
#include "Components.h"
#include "Circuit.h"
#include "SubCircuit.h"
#include <map>
#include "CSimpleVertexContainer.h"
#include "norm_float.h"
#include "RedefParamValues.h"

typedef vector<int> PARAM_CONNECTIONS;

class _CSubCircuitSocket : public _CTreeForkComponent
{
public:
	_CSubCircuitSocket(_CCircuit* pOwner=NULL):_CTreeForkComponent(pOwner),
		m_RealParameters(pOwner),m_MaxRank(0)/*, m_VerticesMaps(pOwner)*/ 
	{
		//m_IAmNumericCopyOf=this;
	}
	_CSubCircuitSocket(_CCircuit* pOwner, const string& InstanceName, _CSubCircuit *pModel,
		const _CIntNodes& ConnectedNodes, long hInternalCofactor);
	virtual ~_CSubCircuitSocket(void);
	virtual unsigned GetNoOfPorts() const override {return (unsigned)m_ConnectedNodes.size();}
	virtual unsigned GetNoOfParametes() const {return 1;}
	virtual const string& GetParameterName() const override {return m_InstanceName;}
	virtual const NumericType& GetParameterValue() const override {return EmptyFloat;}
	virtual size_t GetHash(size_t Core=HASH_CORE) const override;
	virtual void PlugInComponent(_CIntegrityTable& IntegrityTable) override;
	virtual short sPower(int ParamId) {return 0;}
	bool IsReadyToPlug() {return m_pModel && m_pModel->IsReady();}
	bool IsMacroDependant() const {return true;}
	void RequestModelAnalysis() { if(m_pModel) m_pModel->RequestModelAnalysis();}
	virtual void GetSimplyVertex(_CModelVertex* pBaseVertex,_CSExpandedVertices& Vertices,_CSExpandedDescendends& Descendents,
		short& TopSgn, const _CDescSigns& BottomSgn, bool Approximated, const NumericType& Accuracy);
	//_CSimpleVertexContainer GetSimplyVertex(_CSimpleVertexVector& Descendents);
	PARAM_CONNECTIONS GetConnections() {return m_AllParamConnections;}
	_CSubCircuit* GetModelCircuit(/*const string& Context=EmptyString*/) const;
	//void RedefineParameterValue(const string& ParameterName, long double NewValue);
	//void RedefineParameterValue(const _CComponent* pComponent, long double NewValue);
	//const NumericType& GetParameterCurrentValue(const _CComponent* pComponent) const;
	//const NumericType& GetParameterCurrentValue(const string& ParameterName) const;
	//_CVerticesMapCache& GetMapCache() {return m_VerMapCache;}
	//virtual void CleanGarbage() 
	//{
	//	if(m_VerMapCache.CleanGarbage())
	//		m_pModel->ClearPostSimpleVerticesDetermination();
	//}
	virtual int HollowOutput();
	//virtual void DetermineNumericalSPolynomial(_CModelNumericPattern& ResPolynomials, _CSubCircuitSocket* pOwnerSocket);
	//virtual void ApplyMe2Result(_CSPolynomial& ResPolynomials, _CModelNumericPattern& DescendantsNumResuls, _CSubCircuitSocket* pOwnerSocket);
	//virtual void ApplyMe2Result(_CSparsePolynomial& ResPolynomials, _CSubModelNumericPattern& DescendantsNumResuls, _CSubCircuitSocket* pOwnerSocket);
	virtual void ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials,
		const _CDescModelNumericPattern& DescendantsNumResuls,
		const _CDescSignsVector& DescendantsSigns,
		_CCompRedefEntry& CompRedefEntry,
		const string* Context) override;
	//virtual void ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials,
	//	const _CDescModelNumericPattern& DescendantsNumResuls,
	//	const _CDescSignsVector& DescendantsSigns,
	//	_CSubCircuitSocket* pOwnerSocket,
	//	const string* Context);
	void CacheTheResult(_CDirectNumContainer* pCacheContainer)
	{
		m_DirNumCache.CacheTheResult(pCacheContainer);
	}
	//void FindResult(_CDirectNumContainer* pCacheContainer, _CSubModelNumericPattern*& pResNumPat, short& ResTopSgn)
	//{
	//	return m_DirNumCache.FindResult(pCacheContainer,pResNumPat,ResTopSgn);
	//}
	_CDirectNumContainer* FindOrRegisterResult(_CDirectNumContainer* pCacheContainer)
	{
		return dynamic_cast<_CDirectNumContainer*> (m_DirNumCache.FindOrRegisterResult(pCacheContainer));
	}
	void RegisterResult(_CDirectNumContainer* pCacheContainer)
	{
		return m_DirNumCache.RegisterResult(pCacheContainer);
	}
	_CDirectNumContainer* FindExisting(_CDirectNumContainer* pCacheContainer)
	{
		return dynamic_cast<_CDirectNumContainer*> (m_DirNumCache.FindExisting(pCacheContainer));
	}
	virtual unsigned NoOfEffevtiveDescendants() override
	{
		PrepareSetOfDeletions();
		return (unsigned)m_BasicSetOfDeletions.size();
	}

	virtual unsigned NoOfDescendants() const override
	{
		//return m_RealParameters.size();
		return (unsigned)m_AllParamConnections.size();
	}
	virtual unsigned GreedyNoOfDescendants() override
	{
		//return m_RealParameters.size();
		PrepareSetOfDeletions();
		return (unsigned)m_GreedyDeletions.size();
	}
	virtual size_t Desc2RealDesc(size_t Index) {return m_AllParamConnections[Index];}
	virtual const _CSubModelNumericPattern* GetMyNumPattern(_CCompRedefEntry& RedefEntry, _CPathTraitor& PathTraitor) override;
	//virtual const _CSubModelNumericPattern* GetMyNumPattern(_CSubCircuitSocket* pOwnerSocket, _CPathTraitor& PathTraitor) override;
	//_CSubModelNumericPattern* GetNumPatIfRedefined(const _CComponent* pComponent, long double& Value);
	//_CSubModelNumericPattern* GetNumPatIfRedefined(const _CComponent* pComponent, const NumericType*& pValue);
	//virtual void GetSimplyFlatVertex(_CModelVerticesTraitor& VerticesTraitor,
	virtual void GetSimplyFlatVertex2(const _CModelVertex* pCallerVertex,
		_CModelVerticesPath& VerticesPath,
		_CPathTraitor& PathTraitor,
		unsigned long SPower,
		NumericType& CurrentAllowedInaccuracy,
		const _CFlatVertex*& pResultVertex,
		short& TopSgn,
		const _CDescModelNumericPattern& DescNumValues,
		_CCompRedefEntry& RedefEntry) override;
	virtual bool /*NewPath*/ PerformSimplyFlatVertexTravers(
		 _CModelVertex* pCallerVertex,
		unsigned long SPower,
		_CPreFlatVertexContainer*& pResultVertex,
		short& TopSgn,
		_CNewSimplifierData& Data,
		_CCompRedefEntry& RedefEntry
		) override;
	virtual size_t RealIndex(size_t Index) const override
	{
		ASSERTPDD(Index<m_AllParamConnections.size());
		return m_AllParamConnections[Index];
	}
	//bool AmICopy() {return m_IAmNumericCopyOf!=this;}
	//_CSubCircuitSocket* GetOriginal() {return m_IAmNumericCopyOf;}
	void CurrentPins(_CGraphTable& Tables,TWO_GRAPH_PINS& Pins) const;
	virtual void ProcessDescendant(unsigned Desc, _CGraphState* pState) override;
	virtual void CheckDisconnection(_CGraphState* pState) const override;
	virtual unsigned MaxDescRank() override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	//virtual void GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions) override;
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions) override;
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	virtual bool DeletionsAreForced() override;
	virtual bool Translate2RealDescendant(size_t& Index) const override;
protected:
	//_CCompRedefEntry m_InstanceSubCirCompRedefEntry;//???
	//virtual void _GetSimplyFlatVertex(_CModelVerticesPath& VerticesPath,
	//								  _CPathTraitor& PathTraitor,
	//								  const _CSparsePolynomial& TopNumValue,
	//								  const _CSPowerLimiter& TopLimiter,
	//								  const _CFlatVerticesSPowerLimiter& DescLimiters,
	//								  _CSExpFlatVertices& ResVertices,
	//								  const _CMultLevelSExpFlatVertices& Descendents,
	//								  const _CSubModelNumericPattern& Scaler,
	//								  const _CDescModelNumericPattern& DescNumValues,
	//								  //short& TopSgn, 
	//								  const _CDescSigns2& BottomSgn) override;
	virtual bool IsAllways0(size_t DescNo) { return m_AllParamConnections[DescNo]<0; }
	//void DetermineNumericPattern()
	//{
	//	if(IsNumericVirgin())
	//		m_pModel->DetermineNumericPattern(this,m_ModelNumericPatern);
	//}
	//inline bool IsNumericVirgin() {m_ModelNumericPatern.empty();}
	virtual unsigned short sPower() const override;
	void WriteValue(iostream& stream) {stream<<" "<<m_pModel->GetName();}
	void WritePos(iostream& stream);
	void WriteType(iostream& stream) {stream<<"Macromodel : ";}
	bool PrepareInterface2Model();
	void SimplyModelTranslation(_CBounderyParametersCache& BPC);
	void AdvancedModelTranslation(_CBounderyParametersCache& BPC);
	void NewModelTranslation(/*_CBounderyParametersCache& BPC*/);
	virtual bool GetModelInterfaceSpace(path_str& FilePath);
	DECLARE_DYNAMIC_CREATION(_CSubCircuitSocket);
	bool IsEqualIfSameType(const _CComponent& RightComp);
	void Load(_binary_filer& Filer);
	void Store(_binary_filer& Filer);
	typedef _CSubCircuit::BOUNDARY_NODES_MAP BOUNDARY_NODES_MAP;
	typedef _CSubCircuit::BOUNDARY_NODES_IT BOUNDARY_NODES_IT;
	typedef _CSubCircuit::BOUNDARY_NODES_TYPE BOUNDARY_NODES_TYPE;
	_CSubCircuit* m_pModel = nullptr;
	//typedef unordered_map<const string*,_CSubCircuit*> CONTEXT_MODEL;
	//CONTEXT_MODEL m_Models;
	_CIntNodes m_ConnectedNodes;
	string m_InstanceName;
	_CBounderyParametersCache m_RealParameters;
	PARAM_CONNECTIONS m_AllParamConnections;
	friend class _CCircuit;
	//class REDEF_PARAMETERS_VALUE: public unordered_map<const _CComponent*,long double> 
	//{
	//public:
	//	long long GetHush(long long Core) const;
	//};
	//typedef unordered_map<const _CComponent*,long double> REDEF_PARAMETERS_VALUE;
	//typedef REDEF_PARAMETERS_VALUE::value_type REDEF_VALUE_TYPE;
	//typedef REDEF_PARAMETERS_VALUE::iterator REDEF_ITERATOR;
	//typedef REDEF_PARAMETERS_VALUE::const_iterator REDEF_CONST_ITERATOR;
	//typedef _CRedefParamValues::value_type REDEF_VALUE_TYPE;
	//typedef _CRedefParamValues::iterator REDEF_ITERATOR;
	//typedef _CRedefParamValues::const_iterator REDEF_CONST_ITERATOR;
	//_CRedefParamValues m_RedefParameters;

	typedef map<const _CComponent*,_CSubModelNumericPattern> COMP_2_NUM_PAT;
	COMP_2_NUM_PAT m_Comp2NumPat;
	//_CModelNumericPattern m_ModelNumericPatern;
	//_CSubModelNumericPattern m_ModelNumericPatern;
	//_CVerticesMapCache m_VerMapCache;
	_CDirectNumCache m_DirNumCache;
	//typedef unordered_map<const string*,_CSubModelNumericPattern> TContextSubModelNumericPattern;
	typedef unordered_map<const _CCompRedefEntry*, const _CSubModelNumericPattern*> REDEF_SUBMODEL_PATTERN;
	typedef unordered_map<const string*, REDEF_SUBMODEL_PATTERN> TContextSubModelNumericPattern;
	//typedef unordered_map<const string*, const _CSubModelNumericPattern*> TContextSubModelNumericPattern;
	typedef TContextSubModelNumericPattern::iterator iContextSubModelNumericPatter;
	TContextSubModelNumericPattern m_LocalResPolynomials;
	//_CSubCircuitSocket* m_IAmNumericCopyOf;//ci¹æ
	unsigned m_MaxRank;
	//typedef map<size_t,_CBasicSetOfDeletions> BASIC_SET_OF_DELETIONS;
	//BASIC_SET_OF_DELETIONS m_BasicSetOfDeletions;
	MULTI_BASIC_SET_OF_DELETIONS m_BasicSetOfDeletions;
	//typedef vector<_CBasicSetOfDeletions*> GREEDY_DELETIONS;
	typedef vector<_CMultiBasicSetOfDeletions*> GREEDY_DELETIONS;
	GREEDY_DELETIONS m_GreedyDeletions;
	void PrepareSetOfDeletions();
	long m_hInternalCofactor = -1;
};

class _string_list : public list<string>
{
public:
	_string_list() {}
	~_string_list() {}
	_string_list(const string& input_line,const string& white_spaces) {parse(input_line,white_spaces);}
	void parse(const string& input_line,const string& white_spaces);
};

