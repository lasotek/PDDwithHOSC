#include "StdAfx.h"
#include <algorithm>
#include "Components.h"
#include "Circuit.h"
#include "Vertex.h"
#include "SubCircuitSocket.h"
#include "PathTraitor.h"
#include "FlatVertex.h"
#include "MainCircuit.h"
#include "SimplifierEngine.h"
#include "Cofactor.h"
#include "ComponentPath.h"
#include "CompRedefiner.h"
#include "MetaComponente.h"

typedef _CModelVerticesList::iterator MODEL_LIST_IT;

_CComponent::~_CComponent(void)
{
}
_CGraphState& _CComponent::GetGraphState(const _CModelVertex& Vertex) const
{
	return *Vertex.m_pInputState;
}
_CGraphState* _CComponent::PrepareNewState(_CModelVertex& Vertex) const
{
	return Vertex.GetInputState();
}
void _CComponent::CheckNextStep(_CPredictionResult& PredictionResult, const _CGraphState& GraphState, size_t MinInternal, size_t MaxInternal) 
{
	string msg=typeid(this).name();
	msg+=" has not get implemented the function CheckNextStep";
	RISEPDD(eNotImplementedOperation,msg.c_str());
}

//void _CComponent::ConsiderComponentInCircuit()
//{
//	_CModelVerticesList& VertInAnalysis=m_pBaseCircuit->m_VerticesInAnalysis;
//	for(MODEL_LIST_IT it=VertInAnalysis.begin();it!=VertInAnalysis.end();it++)
//		ProcessCMVertex(**it);
//}

void _CComponent::ProcessCMVertex(_CModelVertex& Vertex,unsigned ReduceToNoNodes)
{
	_CGraphState* pGraphState=PrepareNewState(Vertex);
	//if(!m_pBaseCircuit->IsPromising(pGraphState))
	//{
	//}
	_CGraphTable* pGraphTable=pGraphState->GetGraphTable();
	_CIntegrityTable* pIntegrityTable=pGraphState->GetIntegrityTable();
	TWO_GRAPH_PINS Pins;
	CurrentPins(*pGraphTable,Pins);
	pIntegrityTable->UnPlugComponent(Pins);
	pGraphState->CheckCompressionToNode();
//	Vertex.m_pComponent=this;
//	CreateLeaves(Vertex,IntegrityTable);
	ProcessModel(Vertex,pGraphState,ReduceToNoNodes);
	UnPrepareNewState(pGraphState);
	//Vertex.RealiseState();
}
_CGraphTable* _CComponent::PushIntoCache(_CGraphTable* pObject) const
{
	return m_pBaseCircuit->PushIntoCache(pObject);
}

_CIntegrityTable* _CComponent::PushIntoCache(_CIntegrityTable* pObject) const
{
	return m_pBaseCircuit->PushIntoCache(pObject);
}

_CGraphState* _CComponent::PushIntoCache(_CGraphState* pObject) const
{
	return m_pBaseCircuit->PushIntoCache(pObject);
}

_CModelVertex* _CComponent::PushIntoCache(_CModelVertex* pObject) const
{
	return m_pBaseCircuit->PushIntoCache(pObject);
}
	
void _CComponent::SetComponent2Vertex(_CModelVertex& Vertex,_CComponent* pComponent) const
{
	Vertex.m_pComponent=pComponent;
//	Vertex.m_Descendants.resize(pComponent->NoOfDescendants());
	Vertex.ResizeDescendants(pComponent->NoOfDescendants());
}

void _CComponent::SetOwnerCircuit(_CCircuit *pOwner)
{
	m_pBaseCircuit=pOwner;
}

//const NumericType& _CComponent::GetParameterValueInSocket(const _CSubCircuitSocket* pSocket) const
//{
//	return pSocket==NULL?GetParameterValue():pSocket->GetParameterCurrentValue(this);
//}

void _CComponent::GetCompleteOfDeletions(DESC_DELETIONS& DescDeletions)
{
	auto DescNo=NoOfEffevtiveDescendants();
	DescDeletions.resize(DescNo);
	for(unsigned i=0;i<DescNo;i++)
		GetDescendantDeletions(i,DescDeletions[i]);
}


_CModelVertex* _CComponent::GetRegistredVertex(_CGraphState* pUnregisteredState) const
{
	return m_pBaseCircuit->GetRegisteredMVertex(pUnregisteredState);
}

bool _CComponent::IsEqualTo(const _CComponent& RightComp)
{
	if(strcmp(GetClassNameId(),RightComp.GetClassNameId())!=0)
		return false;
	return IsEqualIfSameType(RightComp);
}

void _CComponent::GetSimplyVertex(_CModelVertex* pBaseVertex,_CSExpandedVertices& Vertices,_CSExpandedDescendends& Descendents,
								  short& TopSgn, const _CDescSigns& BottomSgn, bool Approximated, const NumericType& Accuracy)
//_CSimpleVertexContainer _CComponent::GetSimplyVertex( _CSimpleVertexVector& Descendents) 
{
	//if(Descendents.size()!=NoOfDescendants())
	//	eIllegalOperation("No of Descendants is incompatible in _CComponent::GetSimplyVertex");
	_CSExpandedVertices& Desc1=*Descendents.at(0);
	_CSExpandedVertices& Desc0=*Descendents.at(1);
	short Bottom1Sgn=BottomSgn[0],
		  Bottom0Sgn=BottomSgn[1];
	unsigned short S=GetSShift();
	auto _size=max(Desc1.size()+S,Desc0.size());
	for(int i=0;i<_size;i++)
		Vertices.SetVertex(	m_pBaseCircuit->GetSimpleVertexId(this,Desc1.GetVertex(i-S),Desc0.GetVertex(i),Bottom1Sgn,Bottom0Sgn,TopSgn),i);
}
void _CComponent::RaportUniqConnections(COMPONENT_PINS_LIST& Pins) const
{
	COMPONENT_PINS VPins;
	RaportConnections(VPins);
	Pins.assign(VPins.begin(),VPins.end());
	Pins.sort();
	Pins.unique();
}

void _CComponent::WirteToTextStream(iostream& stream)
{
	WriteType(stream);
	stream<<GetParameterName();
	WritePos(stream);
	WriteValue(stream);
	stream<<"\n";
}

unsigned _CComponent::GetSShift()
{
	unsigned short S=sPower();
	if(S==0) return 0;
	return IsSExpanded()?S:0;
}
bool _CComponent::IsSExpanded() const
{
	return m_pBaseCircuit->IsSExpanded();
}

long _CComponent::GetMyIndex() const
{ 
	return m_pBaseCircuit->GetComponentIndex(this); 
}

bool _CComponent::IsTheSameType(const _CComponent* pComp)
{
	return typeid(this) == typeid(pComp);
}

bool _CComponent::IsTheSameCompType(const _CComponent* pOtherComp) const
{
	return strcmp(GetClassNameId(), pOtherComp->GetClassNameId())==0;
}


//******************************_CSimplyComponent***********************
size_t _CSimplyComponent::GetHash(size_t Core) const
{
	auto lCore=Core;
	lCore=Hash_value(lCore,GetClassNameId());
	lCore*=HASH_FACTOR;
	lCore=Hash_value(lCore,m_Name);
	return (long)lCore;
}
_CGraphState* _CTreeForkComponent::PrepareNewState(_CModelVertex& Vertex) const
{
	//static char* Msg="Bad alocation in _CTreeForkComponent::PrepareNewState";

	_CGraphState* pNewState=new _CGraphState(GetGraphState(Vertex),true);
	if(pNewState==NULL)
		throw bad_alloc();
	return pNewState;
}

void _CTreeForkComponent::UnPrepareNewState(_CGraphState* pGraphState) const
{
	if(!pGraphState->DestroyIfUnregistered())
	{
		//pGraphState->pIntegrityTable=NULL;
	}
}

void _CTreeForkComponent::ProcessModel(_CModelVertex& ParentVertex, _CGraphState* pNewState,unsigned ReduceToNoNodes)
{
	SetComponent2Vertex(ParentVertex,this);
	for(unsigned i=0;i<NoOfDescendants();i++)
	{
		short Multiplier=1;
		_CModelVertex* pRegisteredDescendant=NULL;
		if(IsAllways0(i) /*|| !m_pBaseCircuit->IsPromising(pNewState)*/)
		{
			pRegisteredDescendant=GetGlobal0Vertex();
			Multiplier=0;
		}
		else
		{
			_CGraphState* pCurrState=new _CGraphState(*pNewState,true);
			ProcessDescendant(i,pCurrState);
			m_pBaseCircuit->IsPromising(pCurrState);
			if(pCurrState->GetState()==IN_USE)
				CheckDisconnection(pCurrState);
			Multiplier=pCurrState->ExtractMultiplier();
			pCurrState->ReduceToNoOfNodes(ReduceToNoNodes);
			pRegisteredDescendant=GetRegistredVertex(pCurrState);
			if(pRegisteredDescendant==GetGlobalM1Vertex())
			{
				Multiplier*=(-1);
				pRegisteredDescendant=GetGlobal1Vertex();
			}
		}
		ParentVertex.SetDescendant(i,pRegisteredDescendant,Multiplier);
//		pCurrState->DestroyIfUnregistered();
	}
}

void _CTreeForkComponent::CheckNextStep(_CPredictionResult& PredictionResult, const _CGraphState& GraphState, size_t MinInternal, size_t MaxInternal)
{
	_CGraphState TestState(GraphState,true);
	_CGraphTable* pGraphTable=TestState.GetGraphTable();
	_CIntegrityTable* pIntegrityTable=TestState.GetIntegrityTable();
	TWO_GRAPH_PINS Pins;
	CurrentPins(*pGraphTable,Pins);
	pIntegrityTable->UnPlugComponent(Pins);
	TestState.CheckCompressionToNode();
	int ZeroOut=HollowOutput();
	for(unsigned long i=0;i<NoOfDescendants();i++)
	{
		if(IsAllways0(i))
		{
			PredictionResult.NotifyNotActive();
			continue;
		}
		_CGraphState* pCurrState=new _CGraphState(TestState,true);
		try
		{
			ProcessDescendant(i,pCurrState);
			if(pCurrState->GetState()==IN_USE)
				CheckDisconnection(pCurrState);
			RES_STATE State=pCurrState->GetState();
			if(State==IN_USE || State==TERM_ONE)
			{
				Pins.clear();
				CurrentPins(*pCurrState->GetGraphTable(),Pins);
				PredictionResult.TestExistance(pCurrState,Pins);
				PredictionResult.NotifyActiveDescendent(i);
			}
			else
				PredictionResult.NotifyNotActive();			
			if(pCurrState!=NULL && !pCurrState->IsRegistered())
				delete pCurrState;
		}
		catch(exception)
		{
			if(pCurrState!=NULL && !pCurrState->IsRegistered())
				delete pCurrState;
			throw;
		}
	}
}

void _CSimplyComponent::Load(_binary_filer& Filer)
{
	Filer>>m_Name>>m_Value;
}

void _CSimplyComponent::Store(_binary_filer& Filer)
{
	Filer<<m_Name<<m_Value;
}
bool _CSimplyComponent::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CSimplyComponent& RC=*dynamic_cast<const _CSimplyComponent*>(&RightComp);
	return (m_Name==RC.m_Name && m_Value==RC.m_Value);
}

void _CSimplyComponent::ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials,
	const _CDescModelNumericPattern& DescendantsNumResuls,
	const _CDescSignsVector& DescendantsSigns,
	_CCompRedefEntry& CompRedefEntry,
	const string* Context)
{
	const NumericType& Value = CompRedefEntry.IsNumeric() ? CompRedefEntry.GetNumericValue() : m_Value;
	_CSparsePolynomial Mult0;
	Mult0.put(sPower(), Value*DescendantsSigns[0]);
	short Sgn0 = DescendantsSigns[1];
	if (Sgn0 != 1)
	{
		_CSparsePolynomial Mult1;
		Mult1.put(0, Sgn0);
		ResPolynomials = (const _CSubModelNumericPattern)((*DescendantsNumResuls[0])*Mult0 + (*DescendantsNumResuls[1])*Mult1);
	}
	else
		ResPolynomials = (const _CSubModelNumericPattern)((*DescendantsNumResuls[0])*Mult0 + (*DescendantsNumResuls[1]));
}

//void _CSimplyComponent::ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials,
//	const _CDescModelNumericPattern& DescendantsNumResuls,
//	const _CDescSignsVector& DescendantsSigns,
//	_CSubCircuitSocket* pOwnerSocket,
//	const string* Context)
//{
//	//size_t DescNo=DescendantsNumResuls.size();
//	//ASSERTPDD(DescNo==NoOfDescendants());
//	//size_t OutNo=DescendantsNumResuls[0]->size();
//	//ResPolynomials.resize(OutNo);
//	const NumericType& Value =/*pOwnerSocket==NULL?m_Value:*/GetParameterValueInSocket(pOwnerSocket);
//	_CSparsePolynomial Mult0;
//	Mult0.put(sPower(), Value*DescendantsSigns[0]);
//	short Sgn0 = DescendantsSigns[1];
//	if (Sgn0 != 1)
//	{
//		_CSparsePolynomial Mult1;
//		Mult1.put(0, Sgn0);
//		ResPolynomials = (const _CSubModelNumericPattern)((*DescendantsNumResuls[0])*Mult0 + (*DescendantsNumResuls[1])*Mult1);
//	}
//	else
//		ResPolynomials = (const _CSubModelNumericPattern)((*DescendantsNumResuls[0])*Mult0 + (*DescendantsNumResuls[1]));
//	//if(DescLocalSgn!=1)
//	//	ResPolynomials*=DescLocalSgn;
//	//for(size_t i=0;i<OutNo;i++)
//	//{
//	//	//ResPolynomials[i]=DescendantsNumResuls[0]->at(i)*Mult+DescendantsNumResuls[1]->at(i);
//	//	ResPolynomials[i]=(*DescendantsNumResuls[0])[i]*Mult+(*DescendantsNumResuls[1])[i];
//	//}
//}

void _CSimplyComponent::GenerateMyNumPattern(const NumericType& NumericValue, _CSubModelNumericPattern& NumericPattern)
{
	_CSubModelNumericPattern::element_poly LDesc, HDesc;
	LDesc.put(0, 1);
	HDesc.put(sPower(), NumericValue);
	NumericPattern.put(1, HDesc);
	NumericPattern.put(0, LDesc);
}

const _CSubModelNumericPattern* _CSimplyComponent::GetMyBaseNumPattern(_CCompRedefEntry& RedefEntry)
{
	if (RedefEntry.IsNumeric())
		return RedefEntry.GetNumericPattern();
	if (m_pCompPattern == nullptr)
	{
		m_pCompPattern = make_unique<_CSubModelNumericPattern>();
		GenerateMyNumPattern(m_Value, *m_pCompPattern);
		//_CSubModelNumericPattern::element_poly LDesc, HDesc;
		//LDesc.put(0, 1);
		//HDesc.put(sPower(), m_Value);
		//m_pCompPattern->put(0, HDesc);
		//m_pCompPattern->put(1, LDesc);
	}
	return m_pCompPattern.get();
}

//const _CSubModelNumericPattern* _CSimplyComponent::GetMyBaseNumPattern(_CSubCircuitSocket* pOwnerSocket)
//{
//	NumericType Value=0.0;
//	_CSubModelNumericPattern* pResValue=NULL;
//	if(pOwnerSocket!=NULL)
//	{
//		const NumericType* pValue=NULL;
//		pResValue=pOwnerSocket->GetNumPatIfRedefined(this,pValue);
//		if(pValue!=NULL)
//			Value=*pValue;
//	}
//	if(pResValue==NULL)
//	{
//		if(m_pCompPatter==NULL)
//			m_pCompPatter=new _CSubModelNumericPattern;
//		pResValue=m_pCompPatter;
//		Value=m_Value;
//	}
//	if(pResValue->empty())
//	{
//		_CSubModelNumericPattern::element_poly LDesc,HDesc;
//		LDesc.put(0,1);
//		HDesc.put(sPower(),Value);
//		pResValue->put(0,HDesc);
//		pResValue->put(1,LDesc);
//	}
//	
//	return pResValue;
//}

void _CSimplyComponent::GetSimplyFlatVertex2(const _CModelVertex* pCallerVertex,
	_CModelVerticesPath& VerticesPath,
	_CPathTraitor& PathTraitor,
	unsigned long Power,
	NumericType& CurrentAllowedInaccuracy,
	const _CFlatVertex*& pResultVertex,
	short& TopSgn,
	const _CDescModelNumericPattern& DescNumValues,
	_CCompRedefEntry& RedefEntry) 
{
	const _CSubModelNumericPattern* pScaler=VerticesPath.GetTopSubModelNumericPatter();
	_CSparsePolynomial DescPat0,DescPat1; 
	unsigned long Power1=sPower();
	const _CSubModelNumericPattern* pNumPat1=DescNumValues[0];
	if(pNumPat1!=NULL)
		pNumPat1->ScalarProduct(DescPat0,*pScaler,Power);
	if(Power1>=Power)
	{
		Power1=Power-Power1;
		const _CSubModelNumericPattern* pNumPat0=DescNumValues[1];
		if(pNumPat0!=NULL)
			pNumPat0->ScalarProduct(DescPat1,*pScaler,Power1);
	}
	else
		Power1=0;
	unsigned long DescPowers[2]={Power,Power1};
	NumericType DescNum0=DescPat0.get(Power), DescNum1=DescPat1.get(Power1);
	typedef pair<unsigned char/*Index*/,NumericType/*Error?*/> DESC_WEIGHT;
	NumericType Value=GetParameterValue();
	DESC_WEIGHT Weights[2]={DESC_WEIGHT(0,DescPat0.get(Power)*Value),
							DESC_WEIGHT(1,DescPat1.get(Power1))};
	if(Weights[0].second>Weights[1].second)
		swap(Weights[0],Weights[1]);
	typedef pair<const _CFlatVertex*,short> SIGNED_FLAT_V;
	SIGNED_FLAT_V Out[2]={SIGNED_FLAT_V(NULL,0),
						  SIGNED_FLAT_V(NULL,0)};
	for(size_t i=0;i<2;i++)
	{
		unsigned char RealIndex=Weights[i].first;
		NumericType& Weight=Weights[i].second;
		if(Weight<=CurrentAllowedInaccuracy)
			CurrentAllowedInaccuracy-=Weight;
		else
		{
			if(RealIndex==0)
				CurrentAllowedInaccuracy/=Value;
			_CDescendantContainer& ModelDesc=pCallerVertex->GetDescendantNo(RealIndex);
			const _CFlatVertex*& ResDesc=Out[RealIndex].first;
			short& ResSgn=Out[RealIndex].second;
			ResSgn=ModelDesc.m_Multiplier;
			ModelDesc.m_pDescendant->GetSimpleVertexApprox2(ResDesc,ResSgn,DescPowers[RealIndex],CurrentAllowedInaccuracy,PathTraitor,VerticesPath,RedefEntry[this]);
			if(RealIndex==0)
				CurrentAllowedInaccuracy*=Value;
		}
	}
	if(Out[0].first==NULL)
	{
		pResultVertex=Out[1].first;
		TopSgn=Out[1].second;
	}
	else
	{
		short& Sgn0=Out[0].second;
		short& Sgn1=Out[1].second;
		if(Sgn0==0)
		{
			TopSgn=Sgn1;
			Sgn1=1;
		}
		else if(Sgn1!=1 && Sgn0!=1)
		{
			TopSgn=1;
			if(Sgn1<0)
			{
				Sgn1=-Sgn1;
				Sgn0=-Sgn0;
				TopSgn=-TopSgn;
			}
			while((Sgn1&(short)1)==0 && (Sgn0&(short)1)==0)
			{
				Sgn1/=2;
				Sgn0/=2;
				TopSgn*=2;
			}
		}
		VerticesPath.Step2NextVertex(pCallerVertex);
		DESCENDANTS Descs(2);
		Descs[1] = Out[0].first;
		Descs[0] = Out[1].first;
		MULTIPLIERS Mults(2);
		Mults[1] = Sgn1;
		Mults[0] = Sgn0;
		const COMPONENTS_PATH& CPUQ=VerticesPath.GetBaseCircuit()->GetUnique(COMPONENTS_PATH(VerticesPath));
		_CFlatVertex* pResVertexN = new _CFlatVertex(Descs, Mults, CPUQ);
		//_CFlatVertex* pResVertexN = new _CFlatVertex(Out[0].first,
		//	Out[1].first,
		//	Sgn1,
		//	Sgn0,
		//	CPUQ);
		pResultVertex = VerticesPath.GetBaseCircuit()->RegisterFlatVertex(pResVertexN);
		VerticesPath.StepOutOneVertex();
	}
}

bool /*NewPath*/ _CTreeForkComponent::PerformSimplyFlatVertexTravers(
		_CModelVertex* pCallerVertex,
		unsigned long SPower,
		_CPreFlatVertexContainer*& pResultVertex,
		short& TopSgn,
		_CNewSimplifierData& Data,
		_CCompRedefEntry& RedefEntry
		)
{
	unsigned long s_Power = TraversSPower();
	size_t NoOfDescs = NoOfDescendants();
	unsigned short CompPower = NoOfDescendants() - 1;
	unsigned short MaxCompPower = 0;
	for (unsigned short p = CompPower;p != 0; p--)
		if ((size_t)p*s_Power<=SPower)
		{
			MaxCompPower = p;
			break;
		}
	bool Result=true;
	_CPreFlatMap& PreFlatMap=Data.PreFlatMap();
	bool IsExact=Data.PathTraitor.GetApproxCriterion()->IsExact();
	if(IsExact)
	{
		if(pResultVertex!=NULL) return false;
		vector<_CPreFlatVertexContainer*> PreFlatDescs(NoOfDescs, nullptr);
		MULTIPLIERS TopSgns(NoOfDescs, 1);
		bool IsEmpty = true;
		for (unsigned long i = 0; i <= MaxCompPower; i++)
		{
			_CPreFlatVertexContainer*& pPreFlat = PreFlatDescs[i];
			short& TopSgn = TopSgns[i];
			pCallerVertex->GetDescendant(i)->PerformNewSimplyfyApprox(pPreFlat, TopSgn, SPower - i*s_Power, Data, RedefEntry);
			if (pPreFlat != nullptr)
				IsEmpty = false;
			TopSgn *= pCallerVertex->GetMultiplier(i);
		}
		if (IsEmpty)
		{
			TopSgn = 0;
			pResultVertex = nullptr;
			return false;
		}
		if (IsHollow(PreFlatDescs))
		{
			TopSgn *= TopSgns[0];
			pResultVertex = PreFlatDescs[0];
			return Result;
		}
		_CModelVerticesPath& VerticesPath = Data.ModelVerticesPath;
		VerticesPath.Step2NextVertex(pCallerVertex);
		PreFlatMap.PreparePreFlatVC(VerticesPath,SPower,Data.MaxPower(),pResultVertex,RedefEntry);
		VerticesPath.StepOutOneVertex();
		if(!pResultVertex->IsNew()) return false;
		for (short i = 0; i <= MaxCompPower; i++)
		{
			pResultVertex->CurrentSgn(i) = TopSgns[i];
			pResultVertex->CurrentDesc(i) = PreFlatDescs[i];
		}
		if(pResultVertex!=NULL)
			Data.RegisterParent(pResultVertex);
	}
	else
	{
		if(MaxCompPower == 0)
		{
			TopSgn*=pCallerVertex->GetMultiplier(0);
			PreFlatMap.DetermineTraceResult(pResultVertex==NULL);
			Result=pCallerVertex->GetDescendant(0)->PerformNewSimplyfyApprox(pResultVertex,TopSgn,SPower,Data,RedefEntry);
			if(Data.IsForward() && (pResultVertex==nullptr || pResultVertex->IsTerminal()))
			{
				Data.SartBack();
			}
			return Result;
		}
		_CModelVerticesPath& VerticesPath=Data.ModelVerticesPath;
		VerticesPath.Step2NextVertex(pCallerVertex);
		_CSubModelNumericParrernStorage& NumericParentStorage=Data.SubModelNumericParrernStorage;
		_CSubModelNumericPattern* pNumDesc=NULL;
		bool IsNew=Data.SubModelNumericParrernStorage.ForcePattern(VerticesPath,pNumDesc);
		if(IsNew)
		{
			ASSERTPDD(pNumDesc!=NULL && pNumDesc->empty());
			_CDescModelNumericPattern DescNumValues;
			_CDescSigns2 Sgns;
			_CDirNumContainers Locker;
			pCallerVertex->DetermineNumericDescendants(DescNumValues,Sgns,Locker,Data,RedefEntry);
			const _CSubModelNumericPattern* pScaller=VerticesPath.GetTopSubModelNumericPatter();
			pNumDesc->NewScaler(pScaller,DescNumValues,Sgns,Data.MaxPower());
		}
		_CPreFlatMap::TRACE_RESULTS TrRes=_CPreFlatMap::EMPTY;
		if(pResultVertex!=NULL)
		{
			TrRes=PreFlatMap.DetermineTraceResult(false);
		}
		else
		{
			TrRes=PreFlatMap.PreparePreFlatVC(VerticesPath,SPower,Data.MaxPower(),pResultVertex,RedefEntry);
		}
		if(TrRes==_CPreFlatMap::STILL_OLD)
		{	
			if(pResultVertex->NotifyToBeUpdated())
				Data.NotifyParentsToBeUpdated(pResultVertex);
		}
		VerticesPath.StepOutOneVertex();
		if(TrRes==_CPreFlatMap::TERMINAL_REACHED)
		{
			Data.SartBack();
			return false;
		}
		ASSERTPDD(MaxCompPower > 0);
		vector<NumericType> CurrDescValues(MaxCompPower+1,0.0);
		const NumericType& ParValue = pResultVertex->m_ParamValue;
		NumericType CurrParValue(1.0);
		auto CurrSPower = SPower;
#ifdef _DEBUG
		NumericType TopRes(0.0);
#endif
		for (size_t i = 0; i <= MaxCompPower; i++, CurrParValue *= ParValue,CurrSPower-=s_Power)
		{
			ASSERTPDD(CurrSPower >= 0);
#ifdef _DEBUG
			TopRes += CurrDescValues[i] = pNumDesc->get2(i, CurrSPower)*CurrParValue;
#else
			CurrDescValues[i] = pNumDesc->get2(i, CurrSPower)*CurrParValue;
#endif
		}
#ifdef _DEBUG
		TopRes *= TopSgn;
		NumericType TopDiff = abs(TopRes - Data.m_PrevResult);
		bool TestDBG = !(TopDiff.GetMantise() == 0 && TopDiff.GetBase()<numeric_limits<long double>::epsilon());
		TestDBG = TestDBG;
#endif
		if (TrRes == _CPreFlatMap::STILL_OLD && !IsZero(CurrDescValues))
		{
			CurrParValue = 1.0;
			for (size_t i = 0; i <= MaxCompPower; i++, CurrParValue *= ParValue)
				CurrDescValues[i] -= pResultVertex->m_CurrentDescValues[i] * CurrParValue;
		}
		vector<bool> ConsiderVector(MaxCompPower + 1, false);

#ifdef _GREEDY
		if(Data.IsGready())
		{
			if (!SelectIfBigEnough(CurrDescValues, Data.GetAccuracy(),ConsiderVector))
			{
				Data.SartBack();
				return false;
			}
		}
		else
		{
#endif
			ConsiderVector[ChooseTheBiggestOne(CurrDescValues)] = true;
#ifdef _GREEDY
		}
#endif
		for (unsigned long i = 0; i <= MaxCompPower;i++)
			if (ConsiderVector[i])
			{
				unsigned long Power0 = SPower - i*s_Power;
#ifdef _DEBUG
				Data.m_PrevResult = pNumDesc->get2(i, Power0);
#endif
				pResultVertex->m_Sgns[i] = pCallerVertex->GetMultiplier(i);
				Result = pCallerVertex->GetDescendant(i)->PerformNewSimplyfyApprox(pResultVertex->m_CurrDescs[i], pResultVertex->m_Sgns[i], Power0, Data, RedefEntry);
			}

		Data.RegisterParentAndUpdateValues(pResultVertex,TrRes);
	}
	return Result;
}

//bool /*NewPath*/ _CSimplyComponent::PerformSimplyFlatVertexTravers(
//		_CModelVertex* pCallerVertex,
//		size_t SPower,
//		_CPreFlatVertexContainer*& pResultVertex,
//		short& TopSgn,
//		_CNewSimplifierData& Data
//		)
//{
//	bool Power1Allowed=SPower>=sPower();
// 	size_t Power0=SPower,Power1=Power1Allowed?SPower-sPower():0;
//	bool Result=true;
//	_CPreFlatMap& PreFlatMap=Data.PreFlatMap();
//	if(!Power1Allowed)
//	{
//		TopSgn*=pCallerVertex->GetMultiplier(1);
//		PreFlatMap.DetermineTraceResult(pResultVertex==NULL);
//		//PreFlatMap.ReturnedToOld(pResultVertex==NULL);
//		Result=pCallerVertex->GetDescendant(1)->PerformNewSimplyfyApprox(pResultVertex,TopSgn,SPower,Data);
//		if(Data.IsForward() && pResultVertex->IsTerminal())
//		{
//			Data.SartBack();
//		}
//		return Result;
//	}
//	if(Data.PathTraitor.GetApproxCriterion()->IsExact())
//	{
//	}
//	_CModelVerticesPath& VerticesPath=Data.ModelVerticesPath;
//	VerticesPath.Step2NextVertex(pCallerVertex);
//	//_CPreFlatMap& PreFlatMap=Data.PreFlatMap;
//	_CSubModelNumericParrernStorage& NumericParentStorage=Data.SubModelNumericParrernStorage;
//	_CSubModelNumericPattern* pNumDesc=NULL;
//	bool IsNew=Data.SubModelNumericParrernStorage.ForcePattern(VerticesPath,pNumDesc);
//	if(IsNew)
//	{
//		ASSERTPDD(pNumDesc!=NULL && pNumDesc->empty());
//		_CDescModelNumericPattern DescNumValues;
//		_CDescSigns2 Sgns;
//		_CDirNumContainers Locker;
//		pCallerVertex->DetermineNumericDescendants(DescNumValues,Sgns,Locker,Data);
//		const _CSubModelNumericPattern* pScaller=VerticesPath.GetTopSubModelNumericPatter();
//		pNumDesc->NewScaler(pScaller,DescNumValues,Sgns,Data.MaxPower());
//	}
//	//ASSERTPDD(pNumDesc->size()==2);
//	_CPreFlatMap::TRACE_RESULTS TrRes=_CPreFlatMap::EMPTY;
//	if(pResultVertex!=NULL)
//	{
//		TrRes=PreFlatMap.DetermineTraceResult(false);
//	}
//	else
//	{
//		TrRes=PreFlatMap.PreparePreFlatVC(VerticesPath,SPower,Data.MaxPower(),pResultVertex);
//	}
//	//_CPreFlatMap::TRACE_RESULTS TrRes=PreFlatMap.PreparePreFlatVC(Data,pPreFlatVertexContainer);
//	if(TrRes==_CPreFlatMap::STILL_OLD)
//	{	
//		if(pResultVertex->NotifyToBeUpdated())
//			Data.NotifyParentsToBeUpdated(pResultVertex);
//	}
//	VerticesPath.StepOutOneVertex();
//	if(TrRes==_CPreFlatMap::TERMINAL_REACHED)
//	{
//		Data.SartBack();
//		return false;
//	}
//	//NumericType ParValue=GetParameterValueInSocket(VerticesPath.GetParentSocket());
//	const NumericType& ParValue=pResultVertex->m_ParamValue;
//	NumericType Value1=Power1Allowed?pNumDesc->get2(0,Power1)*ParValue:0.0;
//#ifdef _DEBUG
//	NumericType Value0=pNumDesc->get2(1,Power0);
//	NumericType TopRes=Value1+Value0;
//	TopRes*=TopSgn;
//	NumericType TopDiff=abs(TopRes-Data.m_PrevResult);
//	bool TestDBG=!(TopDiff.GetMantise()==0 && TopDiff.GetBase()<numeric_limits<long double>::epsilon());
//	TestDBG=TestDBG;
//#else
//	NumericType Value0=Power1Allowed?pNumDesc->get2(1,Power0):0.0;
//#endif
//	if(TrRes==_CPreFlatMap::STILL_OLD && !IsZero(Value0) && !IsZero(Value1))
//	{
//		Value1-=(pResultVertex->m_1CurrentValue*ParValue);
//		Value0-=pResultVertex->m_0CurrentValue;
//	}
//	bool Consider1=false;
//	bool Consider0=false;
//	NumericType AbsValue1=abs(Value1);
//	NumericType AbsValue0=abs(Value0);
//#ifdef _GREEDY
//	if(Data.IsGready())
//	{
//		const NumericType& Accuracy=Data.GetAccuracy();
//		Consider1=Power1Allowed && Value1!=0.0;
//		Consider0=Value0!=0.0;
//		if(Consider1 && Consider0)
//		{
//			NumericType Total=AbsValue1+AbsValue0;
//			Consider1=AbsValue1/AbsValue0>=Accuracy;
//			Consider0=AbsValue0/AbsValue1>=Accuracy;
//		}
//	}
//	else
//	{
//#endif
//		Consider1=AbsValue1>AbsValue0;
//		Consider0=!Consider1;
//#ifdef _GREEDY
//	}
//#endif
//	//bool Consider1=Power1Allowed && (AbsValue1/Total>=Accuracy);
//	//bool Consider0=AbsValue0/Total>=Accuracy;
//	//_CNewSimplifierData::_CTraceStatusStore StatStore;
//	//if(Power1Allowed && abs(Value1)>abs(Value0))
//	if(Consider1)
//	{
//#ifdef _DEBUG
//		Data.m_PrevResult=pNumDesc->get2(0,Power1);
//#endif
//		pResultVertex->m_1Sgn/***/=pCallerVertex->GetMultiplier(0);
//		Result=pCallerVertex->GetDescendant(0)->PerformNewSimplyfyApprox(pResultVertex->m_Curr1Desc,pResultVertex->m_1Sgn,Power1,Data);
//		if(Consider0)
//		{
//#ifdef _DEBUG
//			Data.m_PrevResult=Value0;
//#endif
//			pResultVertex->m_0Sgn/***/=pCallerVertex->GetMultiplier(1);
//			Result=pCallerVertex->GetDescendant(1)->PerformNewSimplyfyApprox(pResultVertex->m_Curr0Desc,pResultVertex->m_0Sgn,Power0,Data);
//		}
//	}
//	else
//	{
//#ifdef _DEBUG
//		Data.m_PrevResult=Value0;
//#endif
//		pResultVertex->m_0Sgn/***/=pCallerVertex->GetMultiplier(1);
//		Result=pCallerVertex->GetDescendant(1)->PerformNewSimplyfyApprox(pResultVertex->m_Curr0Desc,pResultVertex->m_0Sgn,Power0,Data);
//		if(Consider1)
//		{
//#ifdef _DEBUG
//			Data.m_PrevResult=pNumDesc->get2(0,Power1);
//#endif
//			pResultVertex->m_1Sgn/***/=pCallerVertex->GetMultiplier(0);
//			Result=pCallerVertex->GetDescendant(0)->PerformNewSimplyfyApprox(pResultVertex->m_Curr1Desc,pResultVertex->m_1Sgn,Power1,Data);
//		}
//	}
////	if(Consider1)
////	{
////#ifdef _DEBUG
////		Data.m_PrevResult=pNumDesc->get2(0,Power1);
////#endif
////		pPreFlatVertexContainer->m_1Sgn/***/=pCallerVertex->GetMultiplier(0);
////		Result=pCallerVertex->GetDescendant(0)->PerformNewSimplyfyApprox(pPreFlatVertexContainer->m_Curr1Desc,pPreFlatVertexContainer->m_1Sgn,Power1,Data);
////	}
////	//else
////	if(Consider0)
////	{
////#ifdef _DEBUG
////		Data.m_PrevResult=Value0;
////#endif
////		pPreFlatVertexContainer->m_0Sgn/***/=pCallerVertex->GetMultiplier(1);
////		Result=pCallerVertex->GetDescendant(1)->PerformNewSimplyfyApprox(pPreFlatVertexContainer->m_Curr0Desc,pPreFlatVertexContainer->m_0Sgn,Power0,Data);
////	}
//	bool IsStillEmpty=pResultVertex->IsEmpty();
//	Data.RegisterParentAndUpdateValues(pResultVertex,TrRes);
//	//VerticesPath.StepOutOneVertex();
//	return Result;
//}

//bool /*NewPath*/ _CSimplyComponent::PerformSimplyFlatVertexTravers(
//		_CModelVertex* pCallerVertex,
//		size_t SPower,
//		_CPreFlatVertexContainer*& pResultVertex,
//		short& TopSgn,
//		_CNewSimplifierData& Data
//		)
//{
//	bool Power1Allowed=SPower>=sPower();
// 	size_t Power0=SPower,Power1=Power1Allowed?SPower-sPower():0;
//	bool Result=true;
//	_CPreFlatMap& PreFlatMap=Data.PreFlatMap();
//	if(!Power1Allowed)
//	{
//		TopSgn*=pCallerVertex->GetMultiplier(1);
//		PreFlatMap.ReturnedToOld(pResultVertex==NULL);
//		Result=pCallerVertex->GetDescendant(1)->PerformNewSimplyfyApprox(pResultVertex,TopSgn,SPower,Data);
//		if(Data.IsForward() && pResultVertex->IsTerminal())
//		{
//			Data.SartBack();
//		}
//		return Result;
//	}
//	_CModelVerticesPath& VerticesPath=Data.ModelVerticesPath;
//	VerticesPath.Step2NextVertex(pCallerVertex);
//	//_CPreFlatMap& PreFlatMap=Data.PreFlatMap;
//	_CSubModelNumericParrernStorage& NumericParentStorage=Data.SubModelNumericParrernStorage;
//	_CSubModelNumericPattern* pNumDesc=NULL;
//	bool IsNew=Data.SubModelNumericParrernStorage.ForcePattern(VerticesPath,pNumDesc);
//	if(IsNew)
//	{
//		ASSERTPDD(pNumDesc!=NULL && pNumDesc->empty());
//		_CDescModelNumericPattern DescNumValues;
//		_CDescSigns2 Sgns;
//		_CDirNumContainers Locker;
//		pCallerVertex->DetermineNumericDescendants(DescNumValues,Sgns,Locker,Data);
//		const _CSubModelNumericPattern* pScaller=VerticesPath.GetTopSubModelNumericPatter();
//		pNumDesc->NewScaler(pScaller,DescNumValues,Sgns,Data.MaxPower());
//	}
//	//ASSERTPDD(pNumDesc->size()==2);
//	_CPreFlatVertexContainer* pPreFlatVertexContainer=NULL;
//	_CPreFlatMap::TRACE_RESULTS TrRes=PreFlatMap.PreparePreFlatVC(VerticesPath,SPower,Data.MaxPower(),pPreFlatVertexContainer);
//	//_CPreFlatMap::TRACE_RESULTS TrRes=PreFlatMap.PreparePreFlatVC(Data,pPreFlatVertexContainer);
//	if(TrRes==_CPreFlatMap::STILL_OLD)
//	{	
//		if(pPreFlatVertexContainer->NotifyToBeUpdated())
//			Data.NotifyParentsToBeUpdated(pPreFlatVertexContainer);
//	}
//	VerticesPath.StepOutOneVertex();
//	pResultVertex=pPreFlatVertexContainer;
//	if(TrRes==_CPreFlatMap::TERMINAL_REACHED)
//	{
//		Data.SartBack();
//		return false;
//	}
//	//NumericType ParValue=GetParameterValueInSocket(VerticesPath.GetParentSocket());
//	const NumericType& ParValue=pResultVertex->m_ParamValue;
//	NumericType Value1=Power1Allowed?pNumDesc->get2(0,Power1)*ParValue:0.0;
//#ifdef _DEBUG
//	NumericType Value0=pNumDesc->get2(1,Power0);
//	NumericType TopRes=Value1+Value0;
//	TopRes*=TopSgn;
//	NumericType TopDiff=abs(TopRes-Data.m_PrevResult);
//	bool TestDBG=!(TopDiff.GetMantise()==0 && TopDiff.GetBase()<numeric_limits<long double>::epsilon());
//	TestDBG=TestDBG;
//#else
//	NumericType Value0=Power1Allowed?pNumDesc->get2(1,Power0):0.0;
//#endif
//	if(TrRes==_CPreFlatMap::STILL_OLD && !IsZero(Value0) && !IsZero(Value1))
//	{
//		Value1-=(pPreFlatVertexContainer->m_1CurrentValue*ParValue);
//		Value0-=pPreFlatVertexContainer->m_0CurrentValue;
//	}
//	bool Consider1=false;
//	bool Consider0=false;
//	NumericType AbsValue1=abs(Value1);
//	NumericType AbsValue0=abs(Value0);
//#ifdef _GREEDY
//	if(Data.IsGready())
//	{
//		const NumericType& Accuracy=Data.GetAccuracy();
//		Consider1=Power1Allowed && Value1!=0.0;
//		Consider0=Value0!=0.0;
//		if(Consider1 && Consider0)
//		{
//			NumericType Total=AbsValue1+AbsValue0;
//			Consider1=AbsValue1/AbsValue0>=Accuracy;
//			Consider0=AbsValue0/AbsValue1>=Accuracy;
//		}
//	}
//	else
//	{
//#endif
//		Consider1=AbsValue1>AbsValue0;
//		Consider0=!Consider1;
//#ifdef _GREEDY
//	}
//#endif
//	//bool Consider1=Power1Allowed && (AbsValue1/Total>=Accuracy);
//	//bool Consider0=AbsValue0/Total>=Accuracy;
//	//_CNewSimplifierData::_CTraceStatusStore StatStore;
//	//if(Power1Allowed && abs(Value1)>abs(Value0))
//	if(Consider1)
//	{
//#ifdef _DEBUG
//		Data.m_PrevResult=pNumDesc->get2(0,Power1);
//#endif
//		pPreFlatVertexContainer->m_1Sgn/***/=pCallerVertex->GetMultiplier(0);
//		Result=pCallerVertex->GetDescendant(0)->PerformNewSimplyfyApprox(pPreFlatVertexContainer->m_Curr1Desc,pPreFlatVertexContainer->m_1Sgn,Power1,Data);
//		if(Consider0)
//		{
//#ifdef _DEBUG
//			Data.m_PrevResult=Value0;
//#endif
//			pPreFlatVertexContainer->m_0Sgn/***/=pCallerVertex->GetMultiplier(1);
//			Result=pCallerVertex->GetDescendant(1)->PerformNewSimplyfyApprox(pPreFlatVertexContainer->m_Curr0Desc,pPreFlatVertexContainer->m_0Sgn,Power0,Data);
//		}
//	}
//	else
//	{
//#ifdef _DEBUG
//		Data.m_PrevResult=Value0;
//#endif
//		pPreFlatVertexContainer->m_0Sgn/***/=pCallerVertex->GetMultiplier(1);
//		Result=pCallerVertex->GetDescendant(1)->PerformNewSimplyfyApprox(pPreFlatVertexContainer->m_Curr0Desc,pPreFlatVertexContainer->m_0Sgn,Power0,Data);
//		if(Consider1)
//		{
//#ifdef _DEBUG
//			Data.m_PrevResult=pNumDesc->get2(0,Power1);
//#endif
//			pPreFlatVertexContainer->m_1Sgn/***/=pCallerVertex->GetMultiplier(0);
//			Result=pCallerVertex->GetDescendant(0)->PerformNewSimplyfyApprox(pPreFlatVertexContainer->m_Curr1Desc,pPreFlatVertexContainer->m_1Sgn,Power1,Data);
//		}
//	}
////	if(Consider1)
////	{
////#ifdef _DEBUG
////		Data.m_PrevResult=pNumDesc->get2(0,Power1);
////#endif
////		pPreFlatVertexContainer->m_1Sgn/***/=pCallerVertex->GetMultiplier(0);
////		Result=pCallerVertex->GetDescendant(0)->PerformNewSimplyfyApprox(pPreFlatVertexContainer->m_Curr1Desc,pPreFlatVertexContainer->m_1Sgn,Power1,Data);
////	}
////	//else
////	if(Consider0)
////	{
////#ifdef _DEBUG
////		Data.m_PrevResult=Value0;
////#endif
////		pPreFlatVertexContainer->m_0Sgn/***/=pCallerVertex->GetMultiplier(1);
////		Result=pCallerVertex->GetDescendant(1)->PerformNewSimplyfyApprox(pPreFlatVertexContainer->m_Curr0Desc,pPreFlatVertexContainer->m_0Sgn,Power0,Data);
////	}
//	bool IsStillEmpty=pPreFlatVertexContainer->IsEmpty();
//	Data.RegisterParentAndUpdateValues(pPreFlatVertexContainer,TrRes);
//	//VerticesPath.StepOutOneVertex();
//	return Result;
//}
//
//void _CSimplyComponent::GetSimplyFlatVertex(_CModelVerticesTraitor& VerticesTraitor,
void _CSimplyComponent::_GetSimplyFlatVertex(_CModelVerticesPath& VerticesPath,
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
	ASSERTPDD(Descendents.size()==2);
	//COMPONENTS_PATH Path(VerticesTraitor);
	const _CSPowerLimiter& SPowerLimiter1=DescLimiters.GetSmartEntryLimiter(0); 
	const _CSPowerLimiter& SPowerLimiter0=DescLimiters.GetSmartEntryLimiter(1); 
	const _CSExpFlatVertices* pDesc1=Descendents.get_smart(0);
	const _CSExpFlatVertices* pDesc0=Descendents.get_smart(1);
	//size_t _level_size=pDesc1->size();
	//ASSERTPDD(pDesc0->size()==_level_size);
	unsigned short S=sPower();
	unsigned long MP1=pDesc1==NULL?0:(unsigned long)pDesc1->size();
	unsigned long MP0=pDesc0==NULL?0:(unsigned long)pDesc0->size();
	unsigned long MaxPower=max(MP1+S,MP0);
	for(unsigned long n=0;n<=MaxPower;n++)
	{
		if(!TopLimiter.IsAllowed(n))
			continue;
		const _CSExpFlatVertices::_CVertexContainer* p1Cont=NULL;
		const _CSExpFlatVertices::_CVertexContainer* p0Cont=NULL;
		_CSExpFlatVertices::_CVertexContainer OutCont(1);
		//OutCont.Sgn=1;
		//const _CFlatVertex* p1Desc=NULL;
		//const _CFlatVertex* p0Desc=NULL;
		if(n>=S && pDesc1!=NULL && SPowerLimiter1.IsAllowed(n-S))
			p1Cont=pDesc1->sPowerCont[(size_t)n-S];
			//p1Desc=pDesc1->sPowerCoeff[n-S];
		if(pDesc0!=NULL && SPowerLimiter0.IsAllowed(n))
			p0Cont=pDesc0->sPowerCont[n];
			//p0Desc=pDesc0->sPowerCoeff[n];
		//const _CFlatVertex* pResVertex=NULL;
		if(p1Cont==NULL || p1Cont->pVertex==NULL)
		{
			if(p0Cont!=NULL)
			{
				OutCont.pVertex=p0Cont->pVertex;
				OutCont.Sgn=p0Cont->Sgn*BottomSgn[1];
				//VerticesPath.GetBaseCircuit()->
				//	NotifySkipped(&VerticesPath.GetBaseCircuit()->GetUnique(COMPONENTS_PATH(VerticesPath)));
			}
		}
		else
		{
			//size_t Sgn1=BottomSgn[0];
			//size_t Sgn0=BottomSgn[1];
			const _CFlatVertex* p1Desc=p1Cont->pVertex;
			const _CFlatVertex* p0Desc=(p0Cont==NULL)?0:p0Cont->pVertex;
			short Sgn1=(p1Cont->Sgn)*BottomSgn[0];
			short Sgn0=((p0Cont==NULL)?0:p0Cont->Sgn)*BottomSgn[1];
			//if(p0Desc==NULL)
			//{
			//	TopSgn*=Sgn1;
			//	Sgn1=1;
			//	Sgn0=0;
			//}
			if(Sgn0==0)
			{
				OutCont.Sgn=Sgn1;
				Sgn1=1;
			}
			else if(Sgn1!=1 && Sgn0!=1)
			{
				OutCont.Sgn=1;
				if(Sgn1<0)
				{
					Sgn1=-Sgn1;
					Sgn0=-Sgn0;
					OutCont.Sgn=-OutCont.Sgn;
				}
				while((Sgn1&(short)1)==0 && (Sgn0&(short)1)==0)
				{
					Sgn1/=2;
					Sgn0/=2;
					OutCont.Sgn*=2;
				}
			}
			const COMPONENTS_PATH& CPUQ=VerticesPath.GetBaseCircuit()->GetUnique(COMPONENTS_PATH(VerticesPath));
			DESCENDANTS Descs(2);
			Descs[1] = p1Desc;
			Descs[0] = p0Desc;
			MULTIPLIERS Mults(2);
			Mults[1] = Sgn1;
			Mults[0] = Sgn0;
			_CFlatVertex* pResVertexN = new _CFlatVertex(Descs, Mults,CPUQ);
			//_CFlatVertex* pResVertexN = new _CFlatVertex(p1Desc,
			//	p0Desc,
			//	Sgn1,
			//	Sgn0,
			//	CPUQ);
			//pResVertex=VerticesTraitor.GetBaseCircuit()->RegisterFlatVertex(pResVertexN);
			OutCont.pVertex=VerticesPath.GetBaseCircuit()->RegisterFlatVertex(pResVertexN);
		}
		ResVertices.sPowerCont[n]=&OutCont;
	}
}


// const _CSubModelNumericPattern* _CSimplyComponent::GetMyNumPattern(_CSubCircuitSocket* pOwnerSocket, _CPathTraitor& PathTraitor)
//{
//	_CPathTraitor::COMP_RESULT CompRes=PathTraitor.Compare(this);
//	short& Comp=CompRes.first;
//	short& Oper=CompRes.second;
//	if(Comp<0)
//		return GetMyBaseNumPattern(pOwnerSocket);
//	else
//		if(Comp==0)
//		{
//			if(Oper==_CPathTraitor::DIVIDER)
//				return &HDescFilterPattern;
//			if(Oper==_CPathTraitor::REMINDER)
//				return &LDescFilterPattern;
//		}
//		else
//		{
//			if(Oper==_CPathTraitor::DIVIDER)
//				return &ZeroPattern;
//			if(Oper==_CPathTraitor::REMINDER)
//				return GetMyBaseNumPattern(pOwnerSocket);
//		}
//		RISEPDD(eIllegalOperation,"Oops! I shouldn't be here");
//}

 const _CSubModelNumericPattern* _CSimplyComponent::GetMyNumPattern(_CCompRedefEntry& RedefEntry, _CPathTraitor& PathTraitor)
{
	_CPathTraitor::COMP_RESULT CompRes = PathTraitor.Compare(this);
	short& Comp = CompRes.first;
	short& Oper = CompRes.second;
	if (Comp<0)
		return GetMyBaseNumPattern(RedefEntry[this]);
	else
		if (Comp == 0)
		{
			if (Oper == _CPathTraitor::DIVIDER)
				return &HDescFilterPattern;
			if (Oper == _CPathTraitor::REMINDER)
				return &LDescFilterPattern;
		}
		else
		{
			if (Oper == _CPathTraitor::DIVIDER)
				return &ZeroPattern;
			if (Oper == _CPathTraitor::REMINDER)
				return GetMyBaseNumPattern(RedefEntry[this]);
		}
	RISEPDD(eIllegalOperation, "Oops! I shouldn't be here");
 }

//void _CSimplyComponent::DetermineNumericalSPolynomial(_CModelNumericPattern& ResPolynomials, _CSubCircuitSocket* pOwnerSocket)
//{
//	ResPolynomials.resize(2);
//	ResPolynomials[1].InitSPower(GetParameterValueInSocket(pOwnerSocket),sPower());
//	ResPolynomials[0].InitSPower(1.0,0);
//}

_CComponents::~_CComponents()
{
	for(reverse_iterator it=rbegin();it!=rend();it++)
		delete (*it);
	if(m_pComp2Index!=NULL)
		delete m_pComp2Index;
	//if(m_pIndex2Comp!=NULL)
	//	delete m_pIndex2Comp;
}

bool find_next_macro_pred(const _CComponent* pComponent)
{
	return pComponent->IsMacroDependant();
}
_CComponents::iterator _CComponents::find_next_macro(_CComponents::iterator first)
{
	return find_if(first,end(),&find_next_macro_pred);
}

void _CComponents::SetOwner(_CCircuit* pOwner)
{
	if(m_pOwnerCircuit!=NULL)
		RISEPDD(eIllegalOperation,"_CComponents::SetOwner: Operation for allready initialized value is prohibited");
	m_pOwnerCircuit=pOwner;
}

void _CComponents::Store(_binary_filer& Filer)
{
	Filer<<size();
	for (iterator it = begin(); it != end(); it++)
	{
		Filer << **it;
		auto pit = m_Comp2Priority.find(*it);
		if (pit == m_Comp2Priority.end())
			Filer << (size_t) numeric_limits<size_t>::max();
		else
			Filer << pit->second->first;
	}
	_streamable::Store(Filer);
}

void _CComponents::Load(_binary_filer& Filer)
{
	clear();
	m_Comp2Priority.clear();
	m_Priority2Comp.clear();
	_CComponents::size_type size;
	Filer>>size;
	for(_CComponents::size_type i=0;i<size;i++)
	{
		_CComponent* pComponent=dynamic_cast<_CComponent*>(Filer.RetriveDynamicClass(this));
//		pComponent->SetOwnerCircuit(m_pOwnerCircuit);
		push_back(pComponent);
		size_t Priority = 0;
		Filer >> Priority;
		auto Res = m_Priority2Comp.insert(PRIORITY2COMP::value_type(Priority, pComponent));
		m_Comp2Priority.insert(COMP2PRIORITY::value_type(pComponent, Res));
	}
	_streamable::Load(Filer);//Dodaæ
}
void _CComponents::DoSomeExtras(_streamable* pstrObj)
{
	dynamic_cast<_CComponent*>(pstrObj)->SetOwnerCircuit(m_pOwnerCircuit);
}

void _CComponents::InitMaps()
{
	if(m_pComp2Index!=NULL /*&& m_pIndex2Comp!=NULL*/) return;
	m_pComp2Index=new COMP_TO_INDEX_MAP;
	//m_pIndex2Comp=new INDEX_TO_COMP_MAP;
	unsigned long Index=1;
	iterator it=begin(), _end=end();
	for(;it!=_end;it++,Index++)
	{
		_CComponent* pComponent=*it;
		m_pComp2Index->insert(COMP_TO_INDEX_TYPE(pComponent,Index));
		//m_pIndex2Comp->insert(INDEX_TO_COMP_TYPE(Index,pComponent));
	}
}

long _CComponents::Comp2Index(const _CComponent* pComponent) const
{
	if(pComponent==NULL)
		return -1;
	long Res=0;
	for(const_iterator it=begin();it!=end();it++,Res++)
		if(**it==*pComponent)
			return Res;
	RISEPDD(out_of_range,"_CComponents::Comp2Index do not found the index of the component");
}

_CComponent* _CComponents::Index2Comp(long Index) const
{
	if(Index<0)
		return NULL;
	if(Index>=(long)size())
		RISEPDD(out_of_range,"_CComponents::Index2Comp out of range");
	const_iterator it=begin();
	for(long i=0;i<Index;i++)
		it++;
	return *it;
}
short _CComponents::ComparePosition(const _CComponent* pLeft, const _CComponent* pRight) 
{
	if(pLeft==pRight)
		return 0;
	InitMaps();
	ASSERTPDD(m_pComp2Index!=NULL);
	COMP_TO_INDEX_ITERATOR lit=m_pComp2Index->find(pLeft),
		rit=m_pComp2Index->find(pRight),_end=m_pComp2Index->end();
	if(lit==_end)
		RISEPDD(eIllegalOperation,"pLeft does not point to the circuit's component");
	if(rit==_end)
		RISEPDD(eIllegalOperation,"pRight does not point to the circuit's component");
	unsigned long l=lit->second, r=rit->second;
	if(l>r) return 1;
	else if (l<r) return -1;
	else return 0;
}
void _CComponents::PushToEnd(iterator it)
{
	push_back(*it);
	erase(it);
}

void _CComponents::PushToEnd(_CComponent* pComp)
{
	remove(pComp);
	push_back(pComp);
}

//void _CComponents::PushToEnd(size_t Index)
//{
//}

void _CComponents::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	for(iterator it=begin(),_e=end();it!=_e;it++)
		(*it)->ExchangeNumbers(Numbers2Exchange);
}

void _CComponents::Move2Me(_CComponent* pComponent,_CComponents& Source)
{
	auto pit = Source.m_Comp2Priority.find(pComponent);
	ASSERTPDD(pit != Source.m_Comp2Priority.end());
	size_t Priority = pit->second->first;
	Source.remove(pComponent);
	Source.m_Priority2Comp.erase(pit->second);
	Source.m_Comp2Priority.erase(pit);
	push_back(pComponent);
	auto p2cit = m_Priority2Comp.insert(PRIORITY2COMP::value_type(Priority, pComponent));
	m_Comp2Priority.insert(COMP2PRIORITY::value_type(pComponent, p2cit));
	Source.ClearMaps();
	ClearMaps();
}

void _CComponents::Move2Me(_CComponents& Source)
{
	for(_CComponents::iterator it=Source.begin(),_e=Source.end();it!=_e;)
	{
		_CComponent*  pComponent = *it;
		auto pit = Source.m_Comp2Priority.find(pComponent);
		ASSERTPDD(pit != Source.m_Comp2Priority.end());
		size_t Priority = pit->second->first;
		Source.m_Priority2Comp.erase(pit->second);
		Source.m_Comp2Priority.erase(pit);
		push_back(pComponent);
		auto p2cit = m_Priority2Comp.insert(PRIORITY2COMP::value_type(Priority, pComponent));
		m_Comp2Priority.insert(COMP2PRIORITY::value_type(pComponent, p2cit));
		it = Source.erase(it);
	}
	Source.ClearMaps();
	ClearMaps();
}

void _CComponents::Interpcept(_CComponents& Source)
{
	assign(Source.begin(),Source.end());
	Source.clear();
	Source.ClearMaps();
	ClearMaps();
}
unsigned long _CComponents::MaxPower() const
{
	unsigned Res=0;
	for(const_iterator it=begin(),_e=end();it!=_e;it++)
	{
		Res+=(*it)->sPower();
	}
	return Res;
}

_CMetaComponent* _CComponents::GetMetaIfAlreadyExist(const string& CompName, const type_info& CompTypeInfo)
{
	iterator fit = FindByName(CompName);
	if (fit == end())
		return nullptr;
	_CComponent *pComp = *fit;
	const type_info& pCompInfor = typeid(pComp);
	if (pCompInfor == typeid(_CMetaComponent))
		return dynamic_cast<_CMetaComponent*> (pComp);
	else if (pCompInfor == CompTypeInfo)
	{
		_CMetaComponent* pNewMetaComp = new _CMetaComponent(m_pOwnerCircuit);
		push_back(pNewMetaComp);
		return pNewMetaComp;
	}
	else
		RISEPDD(eWrongName, "Coupled components should has the same type");
	return nullptr;
}

bool _CComponents::AddAnotherComponent(_CComponent* pComponent, size_t Priority)
{
	iterator fit = FindByName(pComponent->GetParameterName());
	if (fit == end())
	{
		push_back(pComponent);
		//pair<PRIORITY2COMP::iterator,bool> 
		auto Res = m_Priority2Comp.insert(PRIORITY2COMP::value_type(Priority, pComponent));
		m_Comp2Priority.insert(COMP2PRIORITY::value_type(pComponent, Res));
		return true;
	}
	_CComponent *pComp = *fit;
	if (pComp->IsMacroDependant())
		return false;
	const type_info& pCompInfor = typeid(*fit);
	_CMetaComponent* pMetaComp = dynamic_cast<_CMetaComponent*> (pComp);
	if (pMetaComp != nullptr)
	{
		if (!pMetaComp->IsCompSetOfType(pComponent))
			return false;
		pMetaComp->AddAnotherSimilarComponent(pComponent);
		auto c2pit = m_Comp2Priority.find(pComp);
		ASSERTPDD(c2pit != m_Comp2Priority.end());
		size_t PriorityOld = c2pit->second->first;
		if (PriorityOld > Priority)
		{
			m_Priority2Comp.erase(c2pit->second);
			m_Comp2Priority.erase(c2pit);
			auto Res = m_Priority2Comp.insert(PRIORITY2COMP::value_type(Priority, pMetaComp));
			m_Comp2Priority.insert(COMP2PRIORITY::value_type(pMetaComp, Res));
		}
		return true;
	}
	if (dynamic_cast<_CSimplyComponent*> (pComp) == nullptr)
		return false;
	if (typeid(pComponent) != pCompInfor)
		return false;
	_CMetaComponent* pNewMetaComp = new _CMetaComponent(pComp,pComponent);
	auto c2pit = m_Comp2Priority.find(pComp);
	ASSERTPDD(c2pit!=m_Comp2Priority.end());
	size_t PriorityOld = c2pit->second->first;
	m_Priority2Comp.erase(c2pit->second);
	m_Comp2Priority.erase(c2pit);
	//pNewMetaComp->AddAnotherSimilarComponent(pComponent);
	push_back(pNewMetaComp);
	auto Res = m_Priority2Comp.insert(PRIORITY2COMP::value_type(min(Priority,PriorityOld), pNewMetaComp));
	m_Comp2Priority.insert(COMP2PRIORITY::value_type(pNewMetaComp, Res));
	erase(fit);
	return true;
}

void _CComponents::ReduceLowerPriority(LIST_OF_COMPONENTS& List)
{
	ASSERTPDD(!m_Priority2Comp.empty());
	size_t Priority = m_Priority2Comp.begin()->first;
	if (Priority == numeric_limits<size_t>::max())
		return;
	for (auto it = List.begin(), _e = List.end();it!=_e;)
	{
		auto pComp = *it;
		auto c2pit = m_Comp2Priority.find(pComp);
		ASSERTPDD(c2pit != m_Comp2Priority.end());
		if (c2pit->second->first == Priority)
			it++;
		else
			it = List.erase(it);
	}
	if (List.empty())
	{
		auto Range = m_Priority2Comp.equal_range(Priority);
		for_each(Range.first, Range.second, [&](PRIORITY2COMP::value_type& Elem)
		{
			List.push_back(Elem.second);
		}
		);
	}
}

bool _CComponents::IsTheMostImportant(_CComponent* pComp)
{
	ASSERTPDD(!m_Priority2Comp.empty());
	size_t TopPriority = (m_Priority2Comp.begin())->first;
	auto fit=m_Comp2Priority.find(pComp);
	ASSERTPDD(fit != m_Comp2Priority.end());
	size_t CompPriority = fit->second->first;
	return CompPriority <= TopPriority;
}


_CComponents::iterator _CComponents::FindByName(const string& CompName)
{
	return find_if(begin(), end(), [&](_CComponent* pComp)
	{
		return CompName == pComp->GetParameterName();
	});
}

IMPLEMENT_DYNAMIC_CREATION(_CComponents);
