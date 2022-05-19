#pragma once
//#include "SimplifierEngine.h"
#include "FlatVertex.h"
#include "PrjException.h"
#include "StateGraphTables.h"
#include "binary_filer.h"
#include "CSimpleVertexContainer.h"
#include "PredictionResult.h"
#include "norm_float.h"
#include <iostream>
#include <memory>
#include "Deletions.h"

class _CCircuit;
class _CModelVertex;
class _CSimpleVertexMap;
class _CPathTraitor;
class _CDescSigns;
class _CDescSigns2;
class _CPreFlatVertexContainer;
class _CNewSimplifierData;
class _CDeletions;
class _CCompRedefEntry;

class _CComponent: public _streamable 
{
public:
	_CComponent(_CCircuit* pParentCircuit):m_pBaseCircuit(pParentCircuit) {}
	virtual ~_CComponent(void);
	virtual int GetNoOfPorts() const=0;
	virtual bool HasParameterValue() const {return false;}
	virtual unsigned NoOfDescendants() const {return 1;}
	virtual unsigned NoOfEffevtiveDescendants() {return  NoOfDescendants();}
	virtual unsigned GreedyNoOfDescendants() {return NoOfDescendants();}
	//virtual void GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions)=0;// {}
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) {}
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions)
	//{
	//	RISEPDD(eWrongCondition, "Only _CTreeForkComponents");
	//}
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions)
	{
		RISEPDD(eWrongCondition, "Only _CTreeForkComponents");
	}
	//typedef vector<const _CBasicSetOfDeletions*> DESC_DELETIONS;
	typedef vector<const _CMultiBasicSetOfDeletions*> DESC_DELETIONS;
	void GetCompleteOfDeletions(DESC_DELETIONS& DescDeletions);
	virtual const string& GetParameterName() const=0;
	virtual const NumericType& GetParameterValue() const=0;
	virtual size_t MaxDescRank()=0;
	virtual void PrepareInnerStructure(_CPathTraitor& Traitor) {}
	//const NumericType& GetParameterValueInSocket(const _CSubCircuitSocket* pSocket) const;
	virtual long GetHash(long Core=HASH_CORE) const=0;
	virtual void PlugInComponent(_CIntegrityTable& IntegrityTable)=0;
	//virtual int GetNextParamID() const {return -1;}
	void ProcessCMVertex(_CModelVertex& Vertex,unsigned ReduceToNoNodes);
	virtual bool IsReadyToPlug() {return true;}
	virtual bool IsMacroDependant() const {return false;}
	virtual void RaportConnections(COMPONENT_PINS& Pins) const=0;// {}
	void RaportUniqConnections(COMPONENT_PINS_LIST& Pins) const;
	void SetOwnerCircuit(_CCircuit *pOwner);
	bool operator==(const _CComponent& RightComp) {return IsEqualTo(RightComp);}
	bool operator!=(const _CComponent& RightComp) {return !operator==(RightComp);}
	virtual void GetSimplyVertex(_CModelVertex* pBaseVertex,_CSExpandedVertices& Vertices,_CSExpandedDescendends& Descendents,
		short& TopSgn, const _CDescSigns& BottomSgn,bool Approximated, const NumericType& Accuracy);
	virtual void WirteToTextStream(iostream& stream);
	unsigned GetSShift();
	virtual unsigned short sPower() const=0;
	virtual bool Translate2RealDescendant(size_t& Index) const {return true;}
	virtual void ModifyValue(long double NewValue) 
	{
		RISEPDD(eIllegalOperation,(string("The component ")+GetParameterName()+" has no value").c_str());
	}
	virtual void CheckNextStep(_CPredictionResult& PredictionResult, const _CGraphState& GraphState, size_t MinInternal, size_t MaxInternal);
	virtual bool IsControledSource() {return false;}
	virtual void CleanGarbage() {}
	virtual int HollowOutput() {return 0;}
	//virtual void ApplyMe2Result(/*_CSPolynomial*/_CSparsePolynomial& ResPolynomila, /*_CModelNumericPattern*/_CSubModelNumericPattern& DescendantsNumResuls,_CSubCircuitSocket* pOwnerSocket) {}
	//virtual void ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials, 
	//	const _CDescModelNumericPattern& DescendantsNumResuls,
	//	const _CDescSignsVector& DescendantsSigns,
	//	_CSubCircuitSocket* pOwnerSocket, 
	//	const string* Context) {}
	virtual void ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials,
		const _CDescModelNumericPattern& DescendantsNumResuls,
		const _CDescSignsVector& DescendantsSigns,
		_CCompRedefEntry& CompRedefEntry,
		const string* Context) {}
	//virtual void ApplyNumerical(_CModelNumericPattern& NumericalRes) {}
	//virtual void DetermineNumericalSPolynomial(_CModelNumericPattern& ResPolynomials, _CSubCircuitSocket* pOwnerSocket) {}
	virtual size_t Desc2RealDesc(size_t Index) {return Index;}
	virtual const _CSubModelNumericPattern* GetMyNumPattern(_CCompRedefEntry& RedefEntry, _CPathTraitor& PathTraitor)
	{
		RISEPDD(eIllegalOperation, "Only for SimplyComponents and ModelSockets GetMyNumPatter is available");
	}
	//virtual const _CSubModelNumericPattern* GetMyNumPattern(_CSubCircuitSocket* pOwnerSocket, _CPathTraitor& PathTraitor)
	//{
	//	RISEPDD(eIllegalOperation, "Only for SimplyComponents and ModelSockets GetMyNumPatter is available");
	//}
	//virtual void GetSimplyFlatVertex(_CModelVerticesTraitor& VerticesTraitor,
	//void GetSimplyFlatVertex(const _CModelVertex* pCallerVertex,
	//						 _CModelVerticesPath& VerticesPath,
	//						 _CPathTraitor& PathTraitor,
	//						 const _CSparsePolynomial& TopNumValue,
	//						 const _CSPowerLimiter& TopLimiter,
	//						 const _CFlatVerticesSPowerLimiter& DescLimiters,
	//						 _CSExpFlatVertices& ResVertices,
	//                         const _CMultLevelSExpFlatVertices& Descendents,
	//						 const _CSubModelNumericPattern& Scaler,
	//						 const _CDescModelNumericPattern& DescNumValues,
	//					     //short& TopSgn, 
	//                         const _CDescSigns2& BottomSgn)
	//{
	//	VerticesPath.Step2NextVertex(pCallerVertex);
	//	_GetSimplyFlatVertex(VerticesPath, PathTraitor, TopNumValue,TopLimiter, DescLimiters, ResVertices,
	//                         Descendents, Scaler, DescNumValues/*, TopSgn*/, BottomSgn);
	//	VerticesPath.StepOutOneVertex();

	//}
	virtual void GetSimplyFlatVertex2(const _CModelVertex* pCallerVertex,
		_CModelVerticesPath& VerticesPath,
		_CPathTraitor& PathTraitor,
		size_t SPower,
		NumericType& CurrentAllowedInaccuracy,
		const _CFlatVertex*& pResultVertex,
		short& TopSgn,
		const _CDescModelNumericPattern& DescNumValues,
		_CCompRedefEntry& RedefEntry) 
	{
		RISEPDD(eIllegalOperation, "Only for SimplyComponents and ModelSockets GetMyNumPatter is available");
	}
	virtual bool /*NewPath*/ PerformSimplyFlatVertexTravers(
		_CModelVertex* pCallerVertex,
		size_t SPower,
		_CPreFlatVertexContainer*& pResultVertex,
		short& TopSgn,
		_CNewSimplifierData& Data,
		_CCompRedefEntry& RedefEntry
		) 
	{
		RISEPDD(eIllegalOperation, "Only for SimplyComponents and ModelSockets GetMyNumPatter is available");
	}
	virtual size_t RealIndex(size_t Index) const
	{
		RISEPDD(eIllegalOperation,"Only for MetaVertices");
	}
	virtual void CurrentPins(_CGraphTable& Tables,TWO_GRAPH_PINS& Pins) const=0;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)=0;
	virtual bool DeletionsAreForced() {return false;}
	_CCircuit* GetOwnerCircuit() const { return m_pBaseCircuit; }
	long GetMyIndex() const;
	virtual bool IsMetaComponent() const { return false; }
	bool IsTheSameType(const _CComponent* pComp);
	virtual void GenerateMyNumPattern(const NumericType& NumericValue, _CSubModelNumericPattern& NumericPattern)
	{
		RISEPDD(eIllegalOperation, "Only for SimplyComponents and ModelSockets GetMyNumPatter is available");
	}
	bool IsTheSameCompType(const _CComponent* pOtherComp) const;
protected:
	virtual void _GetSimplyFlatVertex(_CModelVerticesPath& VerticesPath,
									 _CPathTraitor& PathTraitor,
									 const _CSparsePolynomial& TopNumValue,
									 const _CSPowerLimiter& TopLimiter,
									 const _CFlatVerticesSPowerLimiter& DescLimiters,
									 _CSExpFlatVertices& ResVertices,
	                                 const _CMultLevelSExpFlatVertices& Descendents,
									 const _CSubModelNumericPattern& Scaler,
									 const _CDescModelNumericPattern& DescNumValues,
								     //short& TopSgn, 
	                                 const _CDescSigns2& BottomSgn)
	{
		RISEPDD(eIllegalOperation, "Only for SimplyComponents and ModelSockets GetMyNumPatter is available");
	}
	bool IsSExpanded() const;
	virtual void WriteType(iostream& stream)=0;
	virtual void WritePos(iostream& stream) {}
	virtual void WriteValue(iostream& stgream) {}
	DECLARE_CLASS_NAME(_CComponent);
	bool IsEqualTo(const _CComponent& RightComp);
	virtual bool IsEqualIfSameType(const _CComponent& RightComp)=0;
	_CGraphTable* PushIntoCache(_CGraphTable* pObject) const;
	_CIntegrityTable* PushIntoCache(_CIntegrityTable* pObject) const;
	_CGraphState* PushIntoCache(_CGraphState* pObject) const;
	_CModelVertex* PushIntoCache(_CModelVertex* pObject) const;
	_CModelVertex* GetRegistredVertex(_CGraphState* pUnregisteredState) const;
	_CGraphState& GetGraphState(const _CModelVertex& Vertex) const;
	void SetComponent2Vertex(_CModelVertex& Vertex,_CComponent* pComponent) const;
	virtual _CGraphState* PrepareNewState(_CModelVertex& Vertex) const;
	virtual void UnPrepareNewState(_CGraphState* pGraphState) const {}
//	virtual void ProcessModel(_CModelVertex* pVertex)=0;
	virtual void ProcessModel(_CModelVertex& ParentVertex, _CGraphState* pNewState,unsigned ReduceToNoNodes)=0;
	_CCircuit* m_pBaseCircuit;
	friend class _CMetaComponent;
};

class _CTreeForkComponent : public _CComponent
{
public:
	~_CTreeForkComponent() {}
	_CTreeForkComponent(_CCircuit* pParentCircuit):_CComponent(pParentCircuit) {}
	void CheckNextStep(_CPredictionResult& PredictionResult, const _CGraphState& GraphState, size_t MinInternal, size_t MaxInternal);
	unsigned NoOfDescendants() const {return 2;}
	virtual void ProcessDescendant(unsigned Desc, _CGraphState* pState)=0;
	virtual void CheckDisconnection(_CGraphState* pState) const=0;
	virtual bool /*NewPath*/ PerformSimplyFlatVertexTravers(
		_CModelVertex* pCallerVertex,
		size_t SPower,
		_CPreFlatVertexContainer*& pResultVertex,
		short& TopSgn,
		_CNewSimplifierData& Data,
		_CCompRedefEntry& RedefEntry
		) override;
	virtual short TraversSPower() { return sPower(); }
protected:
	virtual bool IsAllways0(size_t DescNo) { return false; }
	void UnPrepareNewState(_CGraphState* pGraphState) const;
	_CGraphState* PrepareNewState(_CModelVertex& Vertex) const;
	void ProcessModel(_CModelVertex& ParentVertex, _CGraphState* pNewState,unsigned ReduceToNoNodes);
};

class _CSimplyComponent : public _CTreeForkComponent
{
public:
	~_CSimplyComponent() 
	{
		//delete m_pCompPatter;
	}
	_CSimplyComponent(_CCircuit* pParentCircuit):_CTreeForkComponent(pParentCircuit)/*,m_pCompPatter(NULL)*/ 
	{
	}
	_CSimplyComponent(_CCircuit* pParentCircuit,string Name,long double Value):
		_CTreeForkComponent(pParentCircuit),m_Name(Name),m_Value(Value)/*,m_pCompPatter(NULL)*/
	{
	}
	virtual int GetNoOfPorts() const {return 2;}
	virtual const string& GetParameterName() const {return m_Name;}
	virtual long GetHash(long Core=HASH_CORE) const;
	virtual const NumericType& GetParameterValue() const {return m_Value;}
	bool HasParameterValue() const {return true;}
	virtual void ModifyValue(long double NewValue) {m_Value=NewValue;}
	//virtual void DetermineNumericalSPolynomial(_CModelNumericPattern& ResPolynomials, _CSubCircuitSocket* pOwnerSocket);
	//virtual void ApplyMe2Result(_CSubModelNumericPattern& ResPolynomila, const _CDescModelNumericPattern& DescendantsNumResuls,_CSubCircuitSocket* pOwnerSocket) {}
	//virtual void ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials, 
	//	const _CDescModelNumericPattern& DescendantsNumResuls,
	//	const _CDescSignsVector& DescendantsSigns,
	//	_CSubCircuitSocket* pOwnerSocket, 
	//	const string* Context);
	virtual void ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials,
		const _CDescModelNumericPattern& DescendantsNumResuls,
		const _CDescSignsVector& DescendantsSigns,
		_CCompRedefEntry& CompRedefEntry,
		const string* Context) override;
	//_CDescModelNumericPattern* GetCompPattern() {return m_ptr_CompPatter.get();}
	virtual const _CSubModelNumericPattern* GetMyNumPattern(_CCompRedefEntry& RedefEntry, _CPathTraitor& PathTraitor) override;
	//virtual const _CSubModelNumericPattern* GetMyNumPattern(_CSubCircuitSocket* pOwnerSocket, _CPathTraitor& PathTraitor) override;
	//const _CSubModelNumericPattern* GetMyBaseNumPattern(_CSubCircuitSocket* pOwnerSocket);
	const _CSubModelNumericPattern* GetMyBaseNumPattern(_CCompRedefEntry& RedefEntry);
	//virtual void GetSimplyFlatVertex(_CModelVerticesTraitor& VerticesTraitor,
	virtual void GetSimplyFlatVertex2(const _CModelVertex* pCallerVertex,
		_CModelVerticesPath& VerticesPath,
		_CPathTraitor& PathTraitor,
		size_t Power,
		NumericType& CurrentAllowedInaccuracy,
		const _CFlatVertex*& pResultVertex,
		short& TopSgn,
		const _CDescModelNumericPattern& DescNumValues,
		_CCompRedefEntry& RedefEntry) override;
	//virtual bool /*NewPath*/ PerformSimplyFlatVertexTravers(
	//	_CModelVertex* pCallerVertex,
	//	size_t SPower,
	//	_CPreFlatVertexContainer*& pResultVertex,
	//	short& TopSgn,
	//	_CNewSimplifierData& Data,
	//	_CCompRedefEntry& RedefEntry
	//	) override;
	virtual void GenerateMyNumPattern(const NumericType& NumericValue, _CSubModelNumericPattern& NumericPattern) override;
protected:
	virtual void _GetSimplyFlatVertex(_CModelVerticesPath& VerticesPath,
									  _CPathTraitor& PathTraitor,
									  const _CSparsePolynomial& TopNumValue,
									  const _CSPowerLimiter& TopLimiter,
									  const _CFlatVerticesSPowerLimiter& DescLimiters,
									  _CSExpFlatVertices& ResVertices,
									  const _CMultLevelSExpFlatVertices& Descendents,
									  const _CSubModelNumericPattern& Scaler,
									  const _CDescModelNumericPattern& DescNumValues,
									  //short& TopSgn, 
									  const _CDescSigns2& BottomSgn) override;
	void WriteValue(iostream& stream) {stream<<" "<<m_Value;}
	bool IsEqualIfSameType(const _CComponent& RighComp);
	void Load(_binary_filer& Filer);
	void Store(_binary_filer& Filer);
	NumericType m_Value;
	string m_Name;
	//_CSubModelNumericPattern* m_pCompPatter;
	unique_ptr<_CSubModelNumericPattern> m_pCompPattern;
};
typedef list<_CComponent*> LIST_OF_COMPONENTS;

class _CMetaComponent;

class _CComponents : public LIST_OF_COMPONENTS,public _streamable, public _co_streamable
{
public:
	_CComponents(_CCircuit* pOwnerCircuit=NULL):m_pOwnerCircuit(pOwnerCircuit),m_pComp2Index(NULL)/*,
		m_pIndex2Comp(NULL)*/ {}
	~_CComponents();
	iterator find_next_macro(iterator first);
	long Comp2Index(const _CComponent* pComponent) const;
	_CComponent* Index2Comp(long Index) const;
	void SetOwner(_CCircuit* pOwner);
	short ComparePosition(const _CComponent* pLeft, const _CComponent* pRight);
	void PushToEnd(iterator it);
	void PushToEnd(_CComponent* pComp);
	//void PushToEnd(size_t Index);
	void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange);
	void Move2Me(_CComponent* pComponent,_CComponents& Source);
	void Move2Me(_CComponents& Source);
	void Interpcept(_CComponents& Source);
	size_t MaxPower() const;
	bool AddAnotherComponent(_CComponent* pComponent, size_t Priority);//Generated by new - false if unacceptable coupling
	void ReduceLowerPriority(LIST_OF_COMPONENTS& List);
	bool IsTheMostImportant(_CComponent* pComp);
protected:
	_CMetaComponent* GetMetaIfAlreadyExist(const string& CompName, const type_info& CompTypeInfo);
	iterator FindByName(const string& CompName);
	DECLARE_DYNAMIC_CREATION(_CComponents);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	void DoSomeExtras(_streamable* pstrObj);
	typedef unordered_map<const _CComponent*,unsigned long> COMP_TO_INDEX_MAP;
	typedef COMP_TO_INDEX_MAP::value_type COMP_TO_INDEX_TYPE;
	typedef COMP_TO_INDEX_MAP::iterator COMP_TO_INDEX_ITERATOR;
	typedef unordered_map<long,const _CComponent*> INDEX_TO_COMP_MAP;
	//typedef INDEX_TO_COMP_MAP::value_type INDEX_TO_COMP_TYPE;
	//typedef INDEX_TO_COMP_MAP::iterator INDEX_TO_COMP_ITERATOR;
	COMP_TO_INDEX_MAP* m_pComp2Index;
	//INDEX_TO_COMP_MAP* m_pIndex2Comp;
	void InitMaps();
	void ClearMaps() 
	{
		delete m_pComp2Index;
		m_pComp2Index=NULL;
	}
	typedef multimap<size_t, _CComponent*> PRIORITY2COMP;
	typedef map<_CComponent*, PRIORITY2COMP::iterator> COMP2PRIORITY;
	COMP2PRIORITY m_Comp2Priority;
	PRIORITY2COMP m_Priority2Comp;
private:
	_CCircuit* m_pOwnerCircuit;
};
