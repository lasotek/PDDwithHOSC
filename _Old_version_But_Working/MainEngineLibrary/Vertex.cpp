#include "StdAfx.h"
#include "Vertex.h"
#include "Circuit.h"
#include "MainCircuit.h"
#include "SubCircuit.h"
#include "SubCircuitSocket.h"
#include "norm_float.h"
#include <iostream>
#include "PathTraitor.h"
#include "SimplifierEngine.h"
#include "NewCore.h"
//#include "CSimpleVertexContainer.h"

//_CStreamRegistry _streamable::m_CRegistry;

//const _CTerminalVertex Vertex0(0);
//const _CTerminalVertex Vertex1(1);

//const _CTerminalVertex* GetVertex0()
//{
//	return &Vertex0;
//}
//
//const _CTerminalVertex* GetVertex1()
//{
//	return &Vertex1;
//}
//
//bool _CTerminalVertex::IsEqual(const _CAbstractVertex& Operand)
//{
//	try
//	{
//		const _CTerminalVertex& OpVertex=*dynamic_cast<const _CTerminalVertex*>(&Operand);
//		return m_TermValue==OpVertex.m_TermValue;
//	}
//	catch(bad_cast)
//	{
//		return false;
//	}
//}
//************************************_CVertex*****************************************
_CVertex::_CVertex(_CCircuit* pOwner,_CComponent* pComponent, 
		const _CSimpleVertexContainer& Succ1,
		const _CSimpleVertexContainer& Succ0,
		bool Negated):m_pParameter(pComponent),
		m_1Succ(Succ1),
		m_0Succ(Succ0),
		m_IsNegated(Negated),m_pOwnerCircuit(pOwner)
{
	if(m_1Succ.IsLocal())
		m_pOwnerCircuit->TouchVertex(m_1Succ.GetLocalVertex());
	if(m_0Succ.IsLocal())
	m_pOwnerCircuit->TouchVertex(m_0Succ.GetLocalVertex());
}
_CVertex::~_CVertex()
{
	if(m_pOwnerCircuit==NULL || m_pOwnerCircuit->IsDestroyingInProgress())
		return;
	if(m_1Succ.IsLocal())
		m_pOwnerCircuit->UnTouchVertex(m_1Succ.GetLocalVertex());
	if(m_0Succ.IsLocal())
		m_pOwnerCircuit->UnTouchVertex(m_0Succ.GetLocalVertex());
}

long _CVertex::DetermineHashKey(long Core)
{
	register long long Res=m_pParameter->GetHash(Core);
	Res=m_1Succ.GetHash(Res);
	Res=m_0Succ.GetHash(Res);
	Res*=HASH_FACTOR;
	Res^=(long long)m_IsNegated;
	return (long)Res;
}

bool _CVertex::operator==(const _CVertex& Right) const
{
	if(*m_pParameter!=*(Right.m_pParameter))
		return false;
	if(m_IsNegated!=Right.m_IsNegated)
		return false;
	if(m_1Succ!=Right.m_1Succ)
		return false;
	if(m_0Succ!=Right.m_0Succ)
		return false;
	return true;
}

const char* NAP="Ther is no associated parameter with vertex"; 
string _CVertex::GetAssociatedParamName()
{
	if(m_pParameter==NULL)
		RISEPDD(eIllegalOperation,NAP);
	return m_pParameter->GetParameterName();
}

short _CVertex::GetAssociatedSPower()
{
	if(m_pParameter==NULL)
		RISEPDD(eIllegalOperation,NAP);
	return m_pParameter->sPower();
}

const NumericType& _CVertex::GetAssociatedValue()
{
	if(m_pParameter==NULL)
		RISEPDD(eIllegalOperation,NAP);
	return m_pParameter->GetParameterValue();
}

short _CVertex::GlobalMultiplier()
{
	short Mult=1;
	if(m_1Succ.GetLocalMultiplier()==m_0Succ.GetLocalMultiplier() && m_1Succ.GetLocalMultiplier()!=1)
	{
		Mult=(short)m_1Succ.GetLocalMultiplier();
		m_1Succ.ReduceMultiplier();
		m_0Succ.ReduceMultiplier();
	}
	return Mult;
}
void _CVertex::WriteToTextStream(iostream& stream, _CSimpleVerticesMapsSet& Set)
{
	WriteGlobalParameterName(stream);
	if(m_IsNegated)
		stream<<"!";
	stream<<"(";
	m_1Succ.WriteToStream(stream,true,Set);
	stream<<", ";
	m_0Succ.WriteToStream(stream,true,Set);
	stream<<")\n";
}
void _CVertex::WriteGlobalParameterName(iostream& stream)
{
	stream<<""<<m_pOwnerCircuit->GetMyId()<<"."<<m_pParameter->GetParameterName();
}


//****************************_CModelVertex*****************************************************

_CModelVertex Global0Vertex(TERM_SELFLOOP);
_CModelVertex Global1Vertex(TERM_ONE);
_CModelVertex GlobalM1Vertex(TERM_ONE);

_CModelVertex* GetGlobal0Vertex()
{
	return &Global0Vertex;
}

_CModelVertex* GetGlobal1Vertex()
{
	return &Global1Vertex;
}

_CModelVertex* GetGlobalM1Vertex()
{
	return &GlobalM1Vertex;
}

_CModelVertex::_CModelVertex(_CGraphState* pInState,_CCircuit* Owner,bool ForceNewStateCopy):m_OwnerCircuit(Owner),
m_pComponent(NULL),m_pInputState(NULL),/*m_pDescendants(NULL),
m_NoOfDescendants(0),*/m_WasGarbageCleaned(false)/*, m_OptionalMultiplier(0)*/
{
	if(ForceNewStateCopy)
		pInputState=new _CGraphState(*pInState,true);
	else
		pInputState=pInState;
	ASSERTPDD(m_pInputState!=NULL);
}


_CModelVertex::_CModelVertex(RES_STATE ForcedState):m_OwnerCircuit(NULL),
m_pComponent(NULL),m_pInputState(NULL),
/*m_pDescendants(NULL),m_NoOfDescendants(0),*/m_WasGarbageCleaned(false)/*, m_OptionalMultiplier(0)*/
{
	m_IsRegistered=true;
}

_CModelVertex::_CModelVertex(const _CModelVertex& Source, _CCircuit* Owner):m_WasGarbageCleaned(false)/*,
m_OptionalMultiplier(Source.m_OptionalMultiplier)*/
{
	Copy(Source);
	if(Owner!=NULL)
		m_OwnerCircuit=Owner;
}

_CModelVertex::~_CModelVertex()
{
	if(!m_Descendants.empty())
	{
		for(unsigned i=0;i<NoOfDescendants();i++)
			if(m_Descendants[i].m_pDescendant!=NULL)
				m_Descendants[i].m_pDescendant->UnTouchMe();
	}
	if(m_pInputState!=NULL)
	{
		m_pInputState->UnTouchMe();
		if(!m_pInputState->IsRegistered())
			delete m_pInputState;
	}
}

//void _CModelVertex::SetMultiplier(short Multiplier)
//{
//	if(!IsLeaf())
//		RISEPDD(eIllegalOperation,"Only for leaves this operation is legal");
//	m_OptionalMultiplier=Multiplier;
//}

_CDescendantContainer _CModelVertex::GetDescendantNo(size_t Index)  const
{
	if(Index>=NoOfDescendants())
		RISEPDD(out_of_range,"Wrong descendant Index");
	return m_Descendants[Index];
}

void _CModelVertex::RealiseState() 
{
	if(pInputState!=NULL)
		pInputState->FreeTables();
	//pInputState->pIntegrityTable=NULL;
	//pInputState->pGraphTable=NULL;
	pInputState=NULL;
}

void _CModelVertex::RealiseIntTable()
{
	pInputState->pIntegrityTable=NULL;
}

short _CModelVertex::DetermineCommonDivider() 
{
	list<short> Sgns;
	for (auto& Desc : m_Descendants)
		Sgns.push_back(Desc.m_Multiplier);
	short nwd = NWD(Sgns);
	if (nwd != 1)
		for (auto& Desc : m_Descendants)
			Desc.m_Multiplier /= nwd;
	return nwd;

	//short Sgn=1,Divider=1;
	//bool FirstNoZero=true;
	//for(int i=0;i<(int)NoOfDescendants();i++)
	//{
	//	short DesDiv=m_Descendants[i].m_Multiplier;
	//	if(DesDiv==0)
	//		continue;
	//	if(FirstNoZero)
	//	{	
	//		if(DesDiv<0)
	//		{
	//			Sgn=-1;
	//			DesDiv=-DesDiv;
	//		}
	//		FirstNoZero=false;
	//		Divider=Max2PowerDiv(DesDiv);
	//	}
	//	else
	//		Divider=min(Divider,Max2PowerDiv(DesDiv));
	//	if(Divider==1)
	//		break;
	//}
	//Divider*=Sgn;
	//if(Divider!=1)
	//	for(int i=0;i<(int)NoOfDescendants();i++)
	//	{
	//		m_Descendants[i].m_Multiplier/=Divider;
	//	}
	//return Divider;
}

_CModelVertex::_CModelVertex(const _CModelVertex& Source,unsigned NoOfBoundaryNodes):
m_OwnerCircuit(Source.m_OwnerCircuit),m_pComponent(NULL),m_pInputState(NULL),/*m_pDescendants(NULL),
	m_NoOfDescendants(0),*/m_WasGarbageCleaned(true)
{
	ASSERTPDD(Source.m_pComponent==NULL && Source.NoOfDescendants()==0 &&
		!Source.m_Descendants.empty());
	pInputState=new _CGraphState(*Source.m_pInputState,NoOfBoundaryNodes);
	RegisterMembers();
}

void _CModelVertex::SetInputState(_CGraphState* pState)
{
	if(m_pInputState==pState)
		return;
	if(m_pInputState!=NULL)
	{
		m_pInputState->UnTouchMe();
		if(!m_pInputState->IsRegistered())
			delete m_pInputState;
	}
	m_pInputState=pState;
	if(m_pInputState!=NULL)
		m_pInputState->TouchMe();
}
//void _CModelVertex::SetP1Succ(_CModelVertex* P1Succ)
//{
//	if(m_p1Succ==P1Succ)
//		return;
//	if(m_p1Succ!=NULL)
//		m_p1Succ->UnTouchMe();
//	m_p1Succ=P1Succ;
//	if(m_p1Succ!=NULL)
//		m_p1Succ->TouchMe();
//}

//void _CModelVertex::SetP0Succ(_CModelVertex* P0Succ)
//{
//	if(m_p0Succ==P0Succ)
//		return;
//	if(m_p0Succ!=NULL)
//		m_p0Succ->UnTouchMe();
//	m_p0Succ=P0Succ;
//	if(m_p0Succ!=NULL)
//		m_p0Succ->TouchMe();
//}

//void _CModelVertex::AddComponent(const _CComponent& Component,int ParamId)
//{
//	if(IsTerminal()) return;
//	m_pComponent=&Component;
//	_CGraphState* pHSt=new _CGraphState(*m_pInputState),*pNewHSt=NULL;
//	try
//	{
//		_CGraphState* pLSt=new _CGraphState(*m_pInputState),*pNewLSt=NULL;
//		try
//		{
//			m_pComponent->PrepareLHStates(*pHSt,*pLSt,ParamId);
//			pNewHSt=m_OwnerCircuit->PushIntoCache(pHSt);
//			pNewLSt=m_OwnerCircuit->PushIntoCache(pLSt);
//			p1Succ=m_OwnerCircuit->PushIntoCache(new _CModelVertex(pNewHSt,m_OwnerCircuit));
//			p0Succ=m_OwnerCircuit->PushIntoCache(new _CModelVertex(pNewLSt,m_OwnerCircuit));
//		}
//		catch(exception)
//		{
//			if(pNewLSt==NULL)
//				delete pLSt;
//			throw;
//		}
//	}
//	catch(exception)
//	{
//		if(pNewHSt==NULL)
//			delete pHSt;
//		throw;
//	}
//}

void _CModelVertex::AssignComponent(_CComponent* pComponent)
{
	m_pComponent=pComponent;
	ResizeDescendants(pComponent->NoOfDescendants());
//	pComponent->ProcessCMVertex(*this);
}

void _CModelVertex::RegisterMembers()
{
	pInputState->RegisterMembers();
	pInputState=m_OwnerCircuit->PushIntoCache(pInputState);
}

void _CModelVertex::SetOwnerCircuit(_CCircuit* pOwner)
{
	if(m_OwnerCircuit!=NULL)
		RISEPDD(eIllegalOperation,"Only for uninitialized Vertex owner circuit could be set");
	m_OwnerCircuit=pOwner;
}
void _CModelVertex::ClearStateIfPossible()
{
	if(m_pInputState==NULL) return;
	if(HashIsDetermined())
	{
		if(!m_pInputState->IsRegistered())
		{
			delete m_pInputState;
			m_pInputState=NULL;
		}
		else
			pInputState=NULL;
	}
}

//void _CModelVertex::ResizeDescendants(unsigned NewSize)
//{
//	//static char* ErrInfo="Allocation problem in _CModelVertex::ResiszeDescendants";
//	if(!m_Descendants.empty())
//		if(NewSize<=m_NoOfDescendants)
//		{
//			memset(m_pDescendants+NewSize,0,(m_NoOfDescendants-NewSize)*sizeof(_CDescendantContainer));
//			m_NoOfDescendants=NewSize;
//			return;
//		}
//		else
//		{
//			_CDescendantContainer* pNew=new _CDescendantContainer[NewSize];
//			if(pNew==NULL)
//				throw bad_alloc();
//			unsigned size=NewSize*sizeof(_CDescendantContainer);
//			memset(pNew,0,size);
//			memcpy_s(pNew,size,m_pDescendants,m_NoOfDescendants*sizeof(_CDescendantContainer));
//			delete []m_pDescendants;
//			m_pDescendants=pNew;
//		}
//	else
//	{
//		if(NewSize==0)
//			return;
//		m_NoOfDescendants=NewSize;
//		m_pDescendants=new _CDescendantContainer[m_NoOfDescendants];
//		if(m_pDescendants==NULL)
//			throw bad_alloc();
//		memset(m_pDescendants,0,NewSize*sizeof(_CDescendantContainer));
//	}
//
//}

long _CModelVertex::DetermineHashKey(long Core)
{
	if(IsTerminal())
		return GetTerminalValue();
	return pInputState->DetermineHashKey(Core);
}

void _CModelVertex::GetNumberOfOperations(size_t& NoOfMults, size_t& NoOfAdds, size_t& NoOfMultsNonOne)
{
	NoOfMults = NoOfAdds = NoOfMultsNonOne = 0;
	bool First = true;
	for (size_t i = 0; i < m_Descendants.size(); i++)
	{
		_CDescendantContainer& Desc = m_Descendants[i];
		if (Desc.m_Multiplier == 0)
			continue;
		if (Desc.m_Multiplier != 1)
			NoOfMults++;
		if (i != m_pComponent->HollowOutput())
		{
			if (Desc.m_pDescendant == NULL || !Desc.m_pDescendant->Is1())
				NoOfMultsNonOne++;
			NoOfMults++;
		}
		if (First)
			First = false;
		else
			NoOfAdds++;
	}
}

bool _CModelVertex::IsEqual( _CModelVertex& SOperand)
{
	if(HashIsFixed())
	{
		ASSERTPDD(SOperand.HashIsFixed());
		return false;
	}
	if(IsTerminal()!=SOperand.IsTerminal()) return false;
	if(pInputState==NULL || SOperand.pInputState==NULL) return false;//Different level
	if(IsTerminal())
		return GetTerminalValue()==SOperand.GetTerminalValue();
	return (*pInputState)==(*SOperand.pInputState);
}

void _CModelVertex::Copy(const _CModelVertex& SSource) 
{
	m_OwnerCircuit=SSource.m_OwnerCircuit;
	pInputState=SSource.pInputState;
	m_pComponent=SSource.m_pComponent;
	//p1Succ=SSource.p1Succ;
	//p0Succ=SSource.p0Succ;
	ResizeDescendants(SSource.NoOfDescendants());
	//unsigned size=m_NoOfDescendants*sizeof(_CDescendantContainer);
	//memcpy_s(m_pDescendants,size,SSource.m_pDescendants,size);
	m_Descendants.assign(SSource.m_Descendants.begin(),SSource.m_Descendants.end());
}

IMPLEMENT_DYNAMIC_CREATION(_CModelVertex);

void _CModelVertex::Store(_binary_filer& Filer)
{
	_CInUseCheckpoint::Store(Filer);
	_CHashable::Store(Filer);
	long cIndex=m_OwnerCircuit->GetComponentIndex(m_pComponent);
	Filer<<cIndex;
	if(!Filer.IsShortMode())
	{
		long long sIndex=m_OwnerCircuit->GetStateTableIndex(m_pInputState);
		Filer<<sIndex;
	}
	Filer<<NoOfDescendants();
	for(int i=(int)NoOfDescendants()-1;i>=0;i--)
	{
		_CDescendantContainer& Cont=m_Descendants[i];
		long long sIndex=m_OwnerCircuit->GetModelVertexIndex(Cont.m_pDescendant);
		Filer<<Cont.m_Multiplier;
		Filer<<sIndex;
	}
	_streamable::Store(Filer);
}

void _CModelVertex::Load(_binary_filer& Filer)
{
	_CInUseCheckpoint::Load(Filer);
	_CHashable::Load(Filer);
	m_WasGarbageCleaned=true;
	long cIndex;
	Filer>>cIndex;
	m_pComponent=m_OwnerCircuit->GetComponentFromIndex(cIndex);
	if(!Filer.IsShortMode())
	{
		long long sIndex;
		Filer>>sIndex;
		m_pInputState=m_OwnerCircuit->GetStateTableFromIndex(sIndex);
	}
	size_t tNoOfDescendants;
	Filer >> tNoOfDescendants;
	ResizeDescendants(tNoOfDescendants);
	for (int i = (int)tNoOfDescendants - 1; i >= 0; i--)
	{
		_CDescendantContainer& Cont=m_Descendants[i];
		long long sIndex;
		Filer>>Cont.m_Multiplier;
		Filer>>sIndex;
		Cont.m_pDescendant=m_OwnerCircuit->GetModelVertexFromIndex(sIndex);
	}
	_streamable::Load(Filer);
}

_CComponentPredicator _CModelVertex::TestComponent(const _CComponent& Component)
{
	_CComponentPredicator Res;
	return Res;
}

bool _CModelVertex::Is0() const
{
	if(this==GetGlobal0Vertex())
		return true;
	return m_pInputState!=NULL && 
		(m_pInputState->GetState()==TERM_SELFLOOP || m_pInputState->GetState()==TERM_DISCONNECTED);
}

bool _CModelVertex::Is1() const 
{
	if(this==GetGlobal1Vertex())
		return true; 
	return m_pInputState!=NULL && (m_pInputState->GetState()==TERM_ONE && m_pInputState->GetSgn()==1);
}

bool _CModelVertex::IsM1() const 
{

	if(this==GetGlobalM1Vertex())
		return true;
	return m_pInputState!=NULL && (m_pInputState->GetState()==TERM_ONE && m_pInputState->GetSgn()==-1);
}

bool _CModelVertex::IsLeaf() const
{
	return this!=GetGlobal0Vertex() && this!=GetGlobal1Vertex() && this!=GetGlobalM1Vertex() && NoOfDescendants()==0;
}

bool _CModelVertex::IsTerminal() const {
	if(this==GetGlobal0Vertex() || this==GetGlobal1Vertex() || this==GetGlobalM1Vertex())
		return true;
	if(m_pInputState!=NULL)
		return m_pInputState->GetState()!=IN_USE;
	else
		return false;
}
bool _CModelVertex::IsDegenerated(int& HollowOutput) const
{
	if(NoOfDescendants()==0 || m_pComponent==NULL /*|| m_pComponent->IsMacroDependant()*/)
		return false;
	HollowOutput=m_pComponent->HollowOutput();
	if(HollowOutput<0)
		return false;
	for(int i=0;i<(int)NoOfDescendants();i++)
		if(i!=HollowOutput)
			if(!m_Descendants[i].m_pDescendant->Is0())
				return false;
	//return m_pDescendants[0].m_pDescendant->Is0();
	return true;
}
bool _CModelVertex::IsEmptyMacro() const
{
	if(m_pComponent!=NULL && m_pComponent->IsMacroDependant())
	{
		for(unsigned i=0;i<NoOfDescendants();i++)
			if(m_Descendants[i].m_Multiplier!=0 || m_Descendants[i].m_pDescendant!=GetGlobal0Vertex())
				return false;
		return true;
	}
	else
		return false;
}

short _CModelVertex::GetTerminalValue() const
{
	if(m_pInputState==NULL || this==GetGlobal0Vertex())
		return 0;
	if(m_pInputState->GetState()==TERM_SELFLOOP || m_pInputState->GetState()==TERM_DISCONNECTED)
		return 0;
	if(m_pInputState->GetState()==TERM_ONE)
		return m_pInputState->GetGraphTable()->GetSgn();
	RISEPDD(eIllegalOperation,"GetTerminalValue can be use only with terminal vertices");
}

void _CModelVertex::DefineBoundaryParam(unsigned NoOfBoundaries)
{
}

const char* IOR="Index out of range";
void _CModelVertex::SetDescendant(unsigned Index,_CModelVertex* pVertex,short Multiplier)
{
	if(Index>=NoOfDescendants())
		RISEPDD(out_of_range,IOR);
	_CDescendantContainer& DescendanContainer=m_Descendants[Index];
	if(pVertex!=DescendanContainer.m_pDescendant)
	{
		if(DescendanContainer.m_pDescendant!=NULL)
		{
			DescendanContainer.m_pDescendant->UnTouchMe();
			if(!DescendanContainer.m_pDescendant->IsRegistered())
				delete DescendanContainer.m_pDescendant;
		}
		DescendanContainer.m_pDescendant=pVertex;
		if(DescendanContainer.m_pDescendant!=NULL)
			DescendanContainer.m_pDescendant->TouchMe();
	}
	DescendanContainer.m_Multiplier=Multiplier;
}
void _CModelVertex::ReSetDescendant(unsigned Index,_CModelVertex* pVertex)
{
	if(Index>=NoOfDescendants())
		RISEPDD(out_of_range,IOR);
	_CDescendantContainer& DescendanContainer=m_Descendants[Index];
	if(DescendanContainer.m_pDescendant==pVertex)
		return;
	if(DescendanContainer.m_pDescendant!=NULL)
	{
		DescendanContainer.m_pDescendant->UnTouchMe();
		if(!DescendanContainer.m_pDescendant->IsRegistered())
			delete DescendanContainer.m_pDescendant;
	}
	DescendanContainer.m_pDescendant=pVertex;
	if(DescendanContainer.m_pDescendant!=NULL)
		DescendanContainer.m_pDescendant->TouchMe();
}

_CModelVertex* _CModelVertex::GetDescendant(unsigned Index) const
{
	if(Index>=NoOfDescendants())
		RISEPDD(out_of_range,IOR);
	return m_Descendants[Index].m_pDescendant;
}
short _CModelVertex::GetMultiplier(unsigned Index)
{
	if(Index>=NoOfDescendants())
		RISEPDD(out_of_range,IOR);
	return m_Descendants[Index].m_Multiplier;
}

void _CModelVertex::ClearDescendants()
{
	if(!m_Descendants.empty())
	{
		for(unsigned i=0;i<NoOfDescendants();i++)
		{
			_CModelVertex* pVertex=m_Descendants[i].m_pDescendant;
			if(pVertex!=NULL)
				pVertex->UnTouchMe();
		}
		m_Descendants.clear();
		//delete []m_pDescendants;
		//m_pDescendants=NULL;
	}
}

_CModelVerticesCache::~_CModelVerticesCache()
{
	for(auto rit=m_Index2ObjectMap.begin();rit!=m_Index2ObjectMap.end();rit++)
		rit->second->ClearDescendants();

}

void _CModelVerticesCache::SetOwnerCircuit(_CCircuit* pNewOwner)
{
	if(m_pOwnerCircuit!=NULL)
		RISEPDD(eIllegalOperation,"_CModelVerticesCache::SetOwnerCircuit: Operation for allready initialized value is prohibited");
	m_pOwnerCircuit=pNewOwner;
}
void _CModelVerticesCache::DoSomeExtras(_streamable* pMe) 
{
	dynamic_cast<_CModelVertex*>(pMe)->SetOwnerCircuit(m_pOwnerCircuit);
}

void _CModelVerticesCache::Store(_binary_filer& Filer)
{
	PARENT_CLASS::Store(Filer);
	m_PosCurrentContinue=Filer.pos();
	long long TempNextPos=-1;
	Filer<<TempNextPos;
	m_StoredId=m_Id;
}

void _CModelVerticesCache::Load(_binary_filer& Filer)
{
	PARENT_CLASS::Load(Filer);
	long long TempNextPos=0;
	m_PosCurrentContinue=Filer.pos();
	Filer>>TempNextPos;
	long long TempRetard=Filer.pos();
	while(TempNextPos>0)
	{
		Filer.set(TempNextPos);
		PARENT_CLASS::Load(Filer);
		m_PosCurrentContinue=Filer.pos();
		Filer>>TempNextPos;
	}
	Filer.set(TempRetard);
	m_StoredId=m_Id;
}

void _CModelVerticesCache::AppendUnstored(_binary_filer& Filer)
{
	Filer.end();
	long long Pos=Filer.pos();
	//SetLowerBound(m_StoredId+1);
	PARENT_CLASS::Store(Filer,m_StoredId+1);
	long long PostCurrentContinue=Filer.pos();
	long long M1=-1;
	Filer<<M1;
	long long TempRetard=Filer.pos();
	Filer.set(m_PosCurrentContinue);
	Filer<<PostCurrentContinue;
	m_PosCurrentContinue=PostCurrentContinue;
	Filer.set(TempRetard);
	m_StoredId=m_Id;
}

//void _CModelVerticesCache::DoSomethingElse(_CModelVertex* pVertex)
//{
//	pVertex->SetOwnerCircuit(m_pOwnerCircuit);
//}

IMPLEMENT_DYNAMIC_CREATION(_CModelVerticesCache);

void _CModelVerticesList::SetOwner(_CCircuit* pOwner)
{
	if(m_pOwnerCircuit!=NULL)
		RISEPDD(eIllegalOperation,"_CModelVerticesList::SetOwner: Operation for allready initialized value is prohibited");
	m_pOwnerCircuit=pOwner;
}

void _CModelVerticesList::Store(_binary_filer& Filer)
{
	Filer<<size();
	for(iterator it=begin();it!=end();it++)
	{
		long long Index=m_pOwnerCircuit->GetModelVertexIndex(*it);
		Filer<<Index;
	}
	_streamable::Store(Filer);
}
void _CModelVerticesList::FreeStatestIfPosibble()
{
	for each (_CModelVertex* pVertex in *this)
		pVertex->ClearStateIfPossible();
}

void _CModelVerticesList::Load(_binary_filer& Filer)
{
	clear();
	size_type size;
	Filer>>size;
	for(size_type i=0;i<size;i++)
	{
		long long Index;
		Filer>>Index;
		push_back(m_pOwnerCircuit->GetModelVertexFromIndex(Index));
	}
	_streamable::Load(Filer);
}

//void _CModelVertex::GetSimpleVertexApprox( _CMultLevelSExpFlatVertices*& pOutResVertices, 
//								           _CFlatVerticesResContainer*& pCacheContainer,
//										   _CFlatVerticesSPowerLimiter& Limiter,
//										   _CLeafsSExpVertices& TerminalVertices, 
//										   short& TopSgn, 
//										   const _CDescSigns& BottomSgns,
//										   _CPathTraitor& MainTraitor,
//										   _CModelVerticesTraitor& VerticesTraitor)
//{
//	//if(this==GetGlobal0Vertex())
//	//{
//	//	pOutResVertices=&ZeroMultLevelSExpFlatVertices; TopSgn=0; return;
//	//}
//	//if(this==GetGlobal1Vertex())
//	//{
//	//	pOutResVertices=&OneMultLevelSExpFlatVertices; TopSgn=1; return;
//	//}
//	//if(this==GetGlobalM1Vertex())
//	//{
//	//	pOutResVertices=&OneMultLevelSExpFlatVertices; TopSgn=-1; return;
//	//}
//	//_CMainCircuit* pMainCircuit=VerticesTraitor.GetBaseCircuit();
//	//VerticesTraitor.Step2NextVertex(this);
//	//_CFlatVerticesResContainer* pNewContainer=new _CFlatVerticesResContainer(VerticesTraitor,&MainTraitor.StrApproxContext());
//	//_CFlatVerticesResContainer* pExistedContainer=pMainCircuit->FindOrRegisterResult(pNewContainer);
//	//if(pExistedContainer==NULL)
//	//{
//	//	pCacheContainer=pExistedContainer;
//	//	if(IsLeaf())
//	//	{
//	//		size_t Index=m_OwnerCircuit->GetOuterVertexIndex(this);
//	//		if(Index>=TerminalVertices.size())
//	//			RISEPDD(out_of_range,"TerminalVerticeses are too short in _CModelVertex::GetSimpleVertexApprox");
//	//		pOutResVertices=TerminalVertices[Index];
//	//		delete pNewContainer;
//	//	}
//	//	else
//	//	{
//	//		_CLeafsSExpVertices LocalTerminals(m_NoOfDescendants);
//	//		_CFlatVerticesResCacheKiller FlatVerticesResCacheKiller(m_NoOfDescendants);
//	//		_CMultLevelSExpFlatVertices& Result=pNewContainer->GetResult();
//	//		pOutResVertices=&Result;
//	//		_CSubCircuitSocket* pParentSocket=VerticesTraitor.GetParentSocket();
//	//		const _CSubModelNumericPattern* pInternalPatern=m_pComponent->GetMyNumPattern(pParentSocket,MainTraitor);
//	//		const _CSubModelNumericPattern* pTopNumericPattern=NULL;
//	//		_CDirNumContainerWrapper TopDirNumContainer;
//	//		PrepareDirectNumericalDivRem(pTopNumericPattern,TopDirNumContainer,MainTraitor,pParentSocket);
//	//		bool IsOperComp=MainTraitor.IsOperatorComponent(m_pComponent);
//	//		if(IsOperComp)
//	//			MainTraitor.NextFollower();
//	//		_CFlatVerticesSPowerLimiterDesc DescLimiters(m_NoOfDescendants);
//	////		_CFlatVerticesSPowerLimiter DescLimiter;
//	//		_CDescSigns BottomSgns(m_NoOfDescendants);
//	//		for(size_t i=0;i<m_NoOfDescendants;i++)
//	//		{
//	//			if(pInternalPatern->get(i).empty())
//	//				continue;
//	//			_CDescendantContainer& Desc=m_pDescendants[i];
//	//			short Mult=Desc.m_Multiplier;
//	//			if(Mult==0)
//	//				continue;
//	//			const _CSubModelNumericPattern* pDescNumericPattern=NULL;
//	//			_CDirNumContainerWrapper LocalDirNumContainer;
//	//			Desc.m_pDescendant->PrepareDirectNumericalDivRem(pDescNumericPattern,LocalDirNumContainer,MainTraitor,pParentSocket);
//	//			_CFlatVerticesSPowerLimiter& LocalLimiter=DescLimiters[i];
//	////			_CSPowerLimiter& LocalLimiter=DescLimiter.GetEntryLimiter(i);
//	////			LocalLimiter.WorkOutNew(Limiter,pTopNumericPattern->get(Level),pInternalPatern->get(i),
//	////				pDescNumericPattern->get(Level),*MainTraitor.GetApproxCriterion());
//	//			LocalLimiter.WorkOutNew(Limiter,*pTopNumericPattern,pInternalPatern->get(i),
//	//				*pDescNumericPattern,*MainTraitor.GetApproxCriterion());
//	//			if(LocalLimiter.IsEmpty())
//	//				continue;
//	//			Desc.m_pDescendant->GetSimpleVertexApprox(LocalTerminals[i],FlatVerticesResCacheKiller[i],LocalLimiter,TerminalVertices,TopSgn,BottomSgns,MainTraitor,VerticesTraitor);
//	//		}
//	//		TopSgn=BottomSgns.ExtractCommonFactor();
//	//		m_pComponent->GetSimplyFlatVertex(VerticesTraitor,
//	//										  DescLimiters,
//	//										  Result,
//	//										  LocalTerminals,
//	//										  TopSgn,
//	//										  BottomSgns);
//	//		//juchu
//	//		if(IsOperComp)
//	//			MainTraitor.PopLastLevel();
//	//	}
//	//}
//	//else
//	//{
//	////	pOutResVertices=&pExistedContainer->GetResult();
//	////	delete pNewContainer;
//	//}
//	//VerticesTraitor.StepOutOneVertex();
//}
//void _CModelVertex::GetSimpleVertexApprox( _CSExpFlatVertices*& pOutResVertices, 
//		                                   _CFlatVerticesResContainer*& pCacheContainer,
//										   const _CSparsePolynomial& TopNumValue,
//										   const _CSPowerLimiter& Limiter,
//										   const _CSubModelNumericPattern& Scaler,
//										   _CMultLevelSExpFlatVertices& TerminalVertices, 
//										   //short& TopSgn, 
//										   //const _CDescSigns& BottomSgns,
//										   _CPathTraitor& MainTraitor,
//										   _CModelVerticesPath& VerticesPath)
//										   //_CModelVerticesTraitor& VerticesTraitor)
//{
//	if(this==GetGlobal0Vertex())
//	{
//		pOutResVertices=&ZeroFlatVertices; /*TopSgn=0;*/ return;
//	}
//	if(this==GetGlobal1Vertex())
//	{
//		pOutResVertices=&OneFlatVertices; /*TopSgn=1;*/ return;
//	}
//	if(this==GetGlobalM1Vertex())
//	{
//		pOutResVertices=&MinusOneFlatVertices; /*TopSgn=-1;*/ return;
//	}
//	//_CMainCircuit* pMainCircuit=VerticesTraitor.GetBaseCircuit();
//	_CMainCircuit* pMainCircuit=VerticesPath.GetBaseCircuit();
//	//VerticesTraitor.Step2NextVertex(this);
//	//VerticesPath.Step2NextVertex(this);
//	//_CFlatVerticesResContainer* pNewContainer=new _CFlatVerticesResContainer(VerticesTraitor,&MainTraitor.StrApproxContext());
//	_CFlatVerticesResContainer* pNewContainer=new _CFlatVerticesResContainer(this,VerticesPath,Limiter,&MainTraitor.StrApproxContext());
//	_CFlatVerticesResContainer* pExistedContainer=pMainCircuit->FindExisting(pNewContainer);
//	if(pExistedContainer==NULL)
//	{
//		if(IsLeaf())
//		{
//			size_t Index=m_OwnerCircuit->GetOuterVertexIndex(this);
//			if(Index>=TerminalVertices.size())
//				RISEPDD(out_of_range,"TerminalVerticeses are too short in _CModelVertex::GetSimpleVertexApprox");
//			//pOutResVertices=&TerminalVertices[Index];
//			pOutResVertices=TerminalVertices[Index];
//			delete pNewContainer;
//		}
//		else
//		{
//			_CMultLevelSExpFlatVertices LocalTerminals(m_NoOfDescendants);
//			_CFlatVerticesResCacheKiller FlatVerticesResCacheKiller(m_NoOfDescendants);
//			_CDescModelNumericPattern TerminalNumericPatterns(m_NoOfDescendants);
//			_CSExpFlatVertices& Result=pNewContainer->GetResult();
//			pOutResVertices=&Result;
//			//_CSubCircuitSocket* pParentSocket=VerticesTraitor.GetParentSocket();
//			_CSubCircuitSocket* pParentSocket=VerticesPath.GetParentSocket();
//			const _CSubModelNumericPattern* pInternalPatern=m_pComponent->GetMyNumPattern(pParentSocket,MainTraitor);
//			//bool IsOperComp=Traitor.IsOperatorComponent(m_pComponent);
//			//if(IsOperComp)
//			//	Traitor.NextFollower();
//			//const _CSubModelNumericPattern* pTopNumericPattern=NULL;
//			//_CDirNumContainerWrapper TopDirNumContainer;
//			//PrepareDirectNumericalDivRem(pTopNumericPattern,TopDirNumContainer,MainTraitor,pParentSocket);
//			//_CSparsePolynomial TopSparsePoly;
//			//pTopNumericPattern->ScalarProduct(TopSparsePoly,Scaler);
//			bool IsOperComp=MainTraitor.IsOperatorComponent(m_pComponent);
//			if(IsOperComp)
//				MainTraitor.NextFollower();
//			//_CFlatVerticesSPowerLimiterDesc DescLimiters(m_NoOfDescendants);
//			_CFlatVerticesSPowerLimiter DescLimiter;
//			_CDescSigns2 AuxSgns(m_NoOfDescendants);
//			_CDirNumContainers DirNumContainers(m_NoOfDescendants);
//			for(size_t i=0;i<m_NoOfDescendants;i++)
//			{
//				if(pInternalPatern->get(i).empty())
//					continue;
//				_CDescendantContainer& Desc=m_pDescendants[i];
//				short Mult=Desc.m_Multiplier;
//				if(Mult==0)
//					continue;
//				const _CSubModelNumericPattern*& pDescNumericPattern=TerminalNumericPatterns[i];
//				//_CDirNumContainerWrapper LocalDirNumContainer;
//				Desc.m_pDescendant->PrepareDirectNumericalDivRem(pDescNumericPattern,DirNumContainers[i],MainTraitor,pParentSocket);
//				_CSparsePolynomial DescSparsePoly;
//				pDescNumericPattern->ScalarProduct(DescSparsePoly,Scaler);//????????????????
//	//			_CFlatVerticesSPowerLimiter& LocalLimiter=DescLimiters[i];
//				_CSPowerLimiter& LocalLimiter=DescLimiter.GetEntryLimiter(i);
//				LocalLimiter.WorkOutNew(Limiter,TopNumValue,pInternalPatern->get(i),
//					DescSparsePoly,*MainTraitor.GetApproxCriterion());//a³a
//	//			LocalLimiter.WorkOutNew(Limiter,*pTopNumericPattern,pInternalPatern->get(i),
//	//				*pDescNumericPattern,*MainTraitor.GetApproxCriterion());
//				if(LocalLimiter.IsEmpty())
//				{
//					DescLimiter.RemoveEntryLimiter(i);
//					continue;
//				}
//				//short& LocalDescSgn=AuxSgns[i]; 
//				Desc.m_pDescendant->GetSimpleVertexApprox(
//					LocalTerminals[i],
//					FlatVerticesResCacheKiller[i],
//					DescSparsePoly,
//					LocalLimiter,
//					Scaler,
//					TerminalVertices,
//					//LocalDescSgn,
//					//BottomSgns,
//					MainTraitor,
//					VerticesPath);
//					//VerticesTraitor);
//				AuxSgns[i]=Mult;
//				//(*LocalTerminals[i])*=Desc.m_Multiplier;
//			}
//			//TopSgn=AuxSgns.ExtractCommonFactor();
//			//m_pComponent->GetSimplyFlatVertex(VerticesTraitor,
//			m_pComponent->GetSimplyFlatVertex(this,
//											  VerticesPath,
//											  MainTraitor,
//											  TopNumValue,
//											  Limiter,
//											  DescLimiter,
//											  Result,
//											  LocalTerminals,
//											  Scaler,
//											  TerminalNumericPatterns,
//											  //TopSgn,
//											  AuxSgns);
//	//		//juchu
//			if(IsOperComp)
//				MainTraitor.PopLastLevel();
//			pMainCircuit->RegisterResult(pNewContainer);
//			pCacheContainer=pNewContainer;
//		}
//	}
//	else
//	{
//		pOutResVertices=&pExistedContainer->GetResult();
//		delete pNewContainer;
//	}
//	//VerticesTraitor.StepOutOneVertex();
//	//VerticesPath.StepOutOneVertex();
//}

typedef pair<size_t/*Index*/,NumericType/*Error?*/> DESC_WEIGHT;
class _CDescWeightComp
{
public:
	bool operator()(const DESC_WEIGHT& Left,const DESC_WEIGHT& Right) const
	{
		less<NumericType> Less;
		return Less(Left.second,Right.second);
	}
};

typedef vector<DESC_WEIGHT> DESC_VECTOR;

void Sort(DESC_VECTOR& List2Sort)
{
	_CDescWeightComp Comp;
	sort(List2Sort.begin(),List2Sort.end(),Comp);
}
void _CModelVertex::GetSimpleVertexApprox2(const _CFlatVertex*& pResultVertex,
								short& ResultSgn,
								//const _CSparsePolynomial& TopNumValue,
								size_t Power,
								NumericType& AbsolutInaccuracyAllowed,  
								//const _CSubModelNumericPattern& Scaler,
								_CPathTraitor& MainTraitor,
								_CModelVerticesPath& VerticesPath,
								_CCompRedefEntry& RedefEntry)
{
	if(this==GetGlobal0Vertex())
	{
		pResultVertex=NULL; ResultSgn=0; return;
	}
	if(this==GetGlobal1Vertex())
	{
		pResultVertex=&OneFlatVertex; ResultSgn=1; return;
	}
	if(this==GetGlobalM1Vertex())
	{
		pResultVertex=&OneFlatVertex; ResultSgn=-1; return;
	}
	_CMainCircuit* pMainCircuit=VerticesPath.GetBaseCircuit();
	_CFlatVerticesResContainer2* pNewContainer=new _CFlatVerticesResContainer2(this,VerticesPath,Power,&MainTraitor.StrApproxContext(),AbsolutInaccuracyAllowed);
	_CFlatVerticesResContainer2* pExistedContainer=pMainCircuit->FindExisting(pNewContainer);
	if(pExistedContainer==NULL)
	{
		if(IsLeaf())
		{
			const _CModelVertex* pParentMetaVertex=VerticesPath.GetParentMetaVertex();
			ASSERTPDD(pParentMetaVertex!=NULL);
			size_t Index=m_OwnerCircuit->GetOuterVertexIndex(this);
			_CDescendantContainer& RealDesc=pParentMetaVertex->GetDescendantNo(Index);
			ResultSgn*=RealDesc.m_Multiplier;
			_CModelVerticesPath::LIST HeepPath;
			const _CSubModelNumericPattern* pPrevScaler=NULL;
			VerticesPath.LeaveMetaVertex2(HeepPath,pPrevScaler);
			RealDesc.m_pDescendant->GetSimpleVertexApprox2(pResultVertex,ResultSgn,/*TopNumValue,*/
				Power,AbsolutInaccuracyAllowed/*,*pPrevScaler*/,MainTraitor,VerticesPath,RedefEntry.MyTopEntry());
			VerticesPath.Return2MetaVertex(HeepPath);
		}
		else
		{
			bool IsOperComp=MainTraitor.IsOperatorComponent(m_pComponent);
			if(IsOperComp)
				MainTraitor.NextFollower();
			//_CSubCircuitSocket* pParentSocket=VerticesPath.GetParentSocket();
			_CDescModelNumericPattern TerminalNumericPatterns(NoOfDescendants());
			const _CSubModelNumericPattern* pInternalPatern=m_pComponent->GetMyNumPattern(RedefEntry,MainTraitor);
			_CDirNumContainers DirNumContainers(NoOfDescendants());
			//_CDescSigns2 AuxSgns(m_NoOfDescendants);
			for(size_t i=0;i<NoOfDescendants();i++)
			{
				if(pInternalPatern->get(i).empty())
					continue;
				_CDescendantContainer& Desc=m_Descendants[i];
				short Mult=/*AuxSgns[i]=*/Desc.m_Multiplier;
				if(Mult==0)
					continue;
				const _CSubModelNumericPattern*& pDescNumericPattern=TerminalNumericPatterns[i];
				Desc.m_pDescendant->PrepareDirectNumericalDivRem(pDescNumericPattern,DirNumContainers[i],MainTraitor,RedefEntry);
			}
			m_pComponent->GetSimplyFlatVertex2(this,VerticesPath,MainTraitor,Power,AbsolutInaccuracyAllowed,pResultVertex,ResultSgn,
				TerminalNumericPatterns,RedefEntry);
			if(IsOperComp)
				MainTraitor.PopLastLevel();
		}
		pNewContainer->SetResults(pResultVertex,ResultSgn,AbsolutInaccuracyAllowed);	
		pMainCircuit->RegisterFlatVerticesResContainer2(pNewContainer);
	}
	else
	{
		pResultVertex=pExistedContainer->GetVertex();
		ResultSgn=pExistedContainer->GetSgn();
		AbsolutInaccuracyAllowed-=pExistedContainer->GetInaccuracy();
	}
}

void _CModelVertex::DetermineNumericDescendants(_CDescModelNumericPattern& DescPattern, 
	_CDescSigns2& Sgns, _CDirNumContainers& Locker, _CNewSimplifierData& Data, _CCompRedefEntry& RedefParamPtr)
{
	DescPattern.resize(NoOfDescendants());
	Locker.resize(NoOfDescendants());
	Sgns.resize(NoOfDescendants());
	//_CSubCircuitSocket* pParentSocket=Data.ModelVerticesPath.GetParentSocket();
	//const _CSubModelNumericPattern* pInternalPatern =
	//	m_pComponent->GetMyNumPattern(pParentSocket, Data.PathTraitor);
	const _CSubModelNumericPattern* pInternalPatern =
		m_pComponent->GetMyNumPattern(RedefParamPtr, Data.PathTraitor);
	for (size_t i = 0; i<NoOfDescendants(); i++)
	{
		if(pInternalPatern->get(i).empty())
			continue;
		_CDescendantContainer& Desc=m_Descendants[i];
		Sgns[i]=Desc.m_Multiplier;
		if(Desc.m_Multiplier==0)
			continue;

		//const _CSubModelNumericPattern*& pDescNumericPattern=DescPattern[i];
		//Desc.m_pDescendant->PrepareDirectNumericalDivRem(DescPattern[i],
		//	Locker[i], Data.PathTraitor, pParentSocket);
		Desc.m_pDescendant->PrepareDirectNumericalDivRem(DescPattern[i],
			Locker[i], Data.PathTraitor, RedefParamPtr);
	}
}

bool/*NewAdded*/ _CModelVertex::PerformNewSimplyfyApprox(_CPreFlatVertexContainer*& pResultVertex,
	short& ResultSgn, size_t Power, _CNewSimplifierData& Data,_CCompRedefEntry& RedefEntry)
{
	if(this==GetGlobal0Vertex())
	{
		pResultVertex=NULL; ResultSgn=0; return false;
	}
	if(this==GetGlobal1Vertex())
	{
		if(Power>0)
		{
			pResultVertex=NULL; ResultSgn=0; return false;
		}
		else
			pResultVertex=&OnePreFlatVertex; /*ResultSgn=1;*/ return true;
	}
	if(this==GetGlobalM1Vertex())
	{
		if(Power>0)
		{
			pResultVertex=NULL; ResultSgn=0; return false;
		}
		else
		{
			pResultVertex=&OnePreFlatVertex; ResultSgn*=-1; return true;
		}
	}
	_CPathTraitor& PathTraitor=Data.PathTraitor;
	_CModelVerticesPath& ModelVerticesPath=Data.ModelVerticesPath;
	_CPreFlatMap& PreFlatMap=Data.PreFlatMap();
	bool Result=false;
	if(IsLeaf())
	{
			const _CModelVertex* pParentMetaVertex=ModelVerticesPath.GetParentMetaVertex();
			ASSERTPDD(pParentMetaVertex!=NULL);
			size_t Index=m_OwnerCircuit->GetOuterVertexIndex(this);
			//Index=pParentMetaVertex->RealIndex(Index);
			_CDescendantContainer& RealDesc=pParentMetaVertex->GetDescendantNo(Index);
			ResultSgn*=RealDesc.m_Multiplier;
			//_CModelVerticesPath::LIST HeepPath; 
			const _CModelVertex* pTopVertex=NULL;
			const _CSubModelNumericPattern* pPrevScaler=NULL;
			ModelVerticesPath.LeaveMetaVertex(pTopVertex,pPrevScaler);
			Result=RealDesc.m_pDescendant->PerformNewSimplyfyApprox(pResultVertex,ResultSgn,Power,Data,RedefEntry.MyTopEntry());
			ModelVerticesPath.Return2MetaVertex(pTopVertex,pPrevScaler);
	}
	else
	{
			bool IsOperComp=PathTraitor.IsOperatorComponent(m_pComponent);
			if(IsOperComp)
				PathTraitor.NextFollower();
			//_CSubCircuitSocket* pParentSocket=ModelVerticesPath.GetParentSocket();
			//_CDescModelNumericPattern TerminalNumericPatterns(m_NoOfDescendants);
			//const _CSubModelNumericPattern* pInternalPatern=m_pComponent->GetMyNumPattern(pParentSocket,PathTraitor);
			//_CDirNumContainers DirNumContainers(m_NoOfDescendants);
			m_pComponent->PerformSimplyFlatVertexTravers(this,Power,pResultVertex,ResultSgn,Data,RedefEntry);
			if(IsOperComp)
				PathTraitor.PopLastLevel();
	}
	return Result;
}


void _CModelVertex::GetSimpleVertex(_CSExpandedVertices*& pSEVertices,unsigned CofId, const string& SocketName,_CSExpandedDescendends& TerminalVertices,
									short& TopSgn, const _CDescSigns& BottomSgns, bool Approximated, const NumericType& Accuracy)
{
	if(this==GetGlobal0Vertex())
	{
		pSEVertices=&StaticZeroVertices; TopSgn=0; return;
	}
	if(this==GetGlobal1Vertex())
	{
		pSEVertices=&StaticOneVertices; TopSgn=1; return;
	}
	if(this==GetGlobalM1Vertex())
	{
		pSEVertices=&StaticOneVertices; TopSgn=-1; return;
	}
	_CRealGraphDevelopement Container(CofId,SocketName,TerminalVertices,this);
	_CSExpandedVertices* pResVertices=NULL;
	_CRealGraphDevelopement* pRealContainer=m_OwnerCircuit->FindExisting(&Container);//wszelakie zmiany
	if(pRealContainer==NULL)
	{
		_CSExpandedDescendends LocalTerminals(NoOfDescendants());
		_CDescSigns AuxSigns((short)NoOfDescendants());
		for(unsigned i=0;i<NoOfDescendants();i++)
		{
			_CDescendantContainer& Desc=m_Descendants[i];
			_CSExpandedVertices*& pVertices=LocalTerminals[i];
			short& LocalDescSgn=AuxSigns[i]; 
			if(Desc.m_pDescendant->IsLeaf())
			{
				m_OwnerCircuit->GetOuterVertex(pVertices,Desc.m_pDescendant,TerminalVertices);
				LocalDescSgn=1;
			}
			else
				Desc.m_pDescendant->GetSimpleVertex(pVertices,CofId,SocketName,TerminalVertices,LocalDescSgn,BottomSgns,Approximated,Accuracy);
			LocalDescSgn*=Desc.m_Multiplier;
		}
		_CSExpandedVertices ResVertices;
		TopSgn=AuxSigns.ExtractCommonFactor();
		m_pComponent->GetSimplyVertex(this,ResVertices,LocalTerminals,TopSgn,AuxSigns,Approximated,Accuracy);
		pResVertices=m_OwnerCircuit->GetUniqueSEExpandedVertices(ResVertices);
		Container.SetResult(pResVertices,TopSgn);
		m_OwnerCircuit->RegisterResult(&Container);
	}
	else
	{
		pResVertices=pRealContainer->GetVertexResult();
		TopSgn=pRealContainer->GetTopSgnResult();
	}
	pSEVertices=pResVertices;
}

//void _CModelVertex::PrepareDirectNumericValue(_CSPolynomial& ResPolynomial,/*unsigned CofId, const string& SocketName,*/ _CModelNumericPattern& NumericInputPatern,
//void _CModelVertex::PrepareDirectNumericValue(_CSparsePolynomial& ResPolynomial,/*unsigned CofId, const string& SocketName,*/ _CSubModelNumericPattern& NumericInputPatern,
//	bool Approximated, const NumericType& Accuracy)

const _CSubModelNumericPattern ZeroPattern;
const _CSubModelNumericPattern OnePattern(_CSubModelNumericPattern::ONE_TERMINAL);
const _CSubModelNumericPattern MOnePattern(_CSubModelNumericPattern::MONE_TERMINAL);
const _CSubModelNumericPattern HDescFilterPattern(_CSubModelNumericPattern::H_DESC);
const _CSubModelNumericPattern LDescFilterPattern(_CSubModelNumericPattern::L_DESC);

void _CModelVertex::PrepareDirectNumericValue(const _CSubModelNumericPattern*& pOutputPatern, _CDirectNumContainer*& pCacheContainer, 
	const string* Context, _CCompRedefEntry& RedefParam)
{
	if(this==GetGlobal0Vertex())
	{
		pOutputPatern=&ZeroPattern; 
		pCacheContainer=NULL;
		return;
	}
	if(this==GetGlobal1Vertex())
	{
		pOutputPatern=&OnePattern; 
		pCacheContainer=NULL;
		return;
	}
	if(this==GetGlobalM1Vertex())
	{
		pOutputPatern=&MOnePattern; 
		pCacheContainer=NULL;
		return;
	}
	_CDirectNumContainer* pNewContainer=new _CDirectNumContainer(&RedefParam,this,Context);
	_CDirectNumContainer* pExistedContainer=m_OwnerCircuit->FindExisting(pNewContainer);
	if(pExistedContainer==NULL)
	{
		pCacheContainer=pNewContainer;
		_CSubModelNumericPattern* pPatern=pNewContainer->GetVertexResult();
		if(IsLeaf())
		{
			size_t Index=m_OwnerCircuit->GetOuterVertexIndex(this);
			pPatern->SetLeaf(true,Index,1);
		}
		else
		{
			_CDescModelNumericPattern DescModelNumPatt(NoOfDescendants());
			_CDirNumContainers DirNumContainers(NoOfDescendants());
			_CDescSignsVector DescSignsVector(NoOfDescendants());
			for(size_t i=0;i<NoOfDescendants();i++)
			{
				_CDescendantContainer& Desc=m_Descendants[i];
				DescSignsVector[i]=Desc.m_Multiplier;
				//Desc.m_pDescendant->PrepareDirectNumericValue(DescModelNumPatt[i], DirNumContainers[i], Context, pMasterSocket);//Dodatkowy znak
				Desc.m_pDescendant->PrepareDirectNumericValue(DescModelNumPatt[i], DirNumContainers[i], Context, RedefParam);
			}
			m_pComponent->ApplyMe2Result(*pPatern, DescModelNumPatt,DescSignsVector,RedefParam,Context);///Bulba
		}
		m_OwnerCircuit->RegisterResult(pNewContainer);
		pOutputPatern=pPatern;
	}
	else
	{
		pCacheContainer=pExistedContainer;
		pOutputPatern=pExistedContainer->GetVertexResult();
		delete pNewContainer;
	}
}

void _CModelVertex::PrepareDirectNumericalDivRem(const _CSubModelNumericPattern*& pOutputPatern, _CDirectNumContainer*& pCacheContainer, _CPathTraitor& Traitor,
	_CCompRedefEntry& RedefParam)
{
	if(this==GetGlobal0Vertex())
	{
		pOutputPatern=&ZeroPattern; 
		pCacheContainer=NULL;
		return;
	}
	if(this==GetGlobal1Vertex())
	{
		pOutputPatern=&OnePattern; 
		pCacheContainer=NULL;
		return;
	}
	if(this==GetGlobalM1Vertex())
	{
		pOutputPatern=&MOnePattern; 
		pCacheContainer=NULL;
		return;
	}
	_CDirectNumContainer* pNewContainer=new _CDirectNumContainer(&RedefParam,this,&Traitor.StrContext2());
	_CDirectNumContainer* pExistedContainer=m_OwnerCircuit->FindExisting(pNewContainer);
	if(pExistedContainer==NULL)
	{
		pCacheContainer=pNewContainer;
		_CSubModelNumericPattern* pPatern=pNewContainer->GetVertexResult();
		if(IsLeaf())
		{
			size_t Index=m_OwnerCircuit->GetOuterVertexIndex(this);
			pPatern->SetLeaf(true,Index,1);
		}
		else
		{
			const _CSubModelNumericPattern* pInternalPatern=m_pComponent->GetMyNumPattern(RedefParam,Traitor);
			bool IsOperComp=Traitor.IsOperatorComponent(m_pComponent);
			if(IsOperComp)
				Traitor.NextFollower();
			_CDirNumContainers DirNumContainers(NoOfDescendants());
			if (m_Descendants.empty())
				RISEPDD(eWrongCondition, "Invalid descendants");
			for(size_t i=0;i<NoOfDescendants();i++)
			{
				if(pInternalPatern->get(i).empty())
					continue;
				_CDescendantContainer& Desc=m_Descendants[i];
				short Mult=Desc.m_Multiplier;
				if(Mult==0)
					continue;
				const _CSubModelNumericPattern* pDescPattern=NULL;
				Desc.m_pDescendant->PrepareDirectNumericalDivRem(pDescPattern,DirNumContainers[i],Traitor,RedefParam);
				if(Mult==1)
					(*pPatern)+=((*pDescPattern)*(pInternalPatern->get(i)));
				else
				{
					_CSparsePolynomial MultP;
					MultP.put(0,Mult);
					(*pPatern)+=((*pDescPattern)*(pInternalPatern->get(i))*MultP);
				}
			}
			if(IsOperComp)
				Traitor.PopLastLevel();
		}
		m_OwnerCircuit->RegisterResult(pNewContainer);
		pOutputPatern=pPatern;
	}
	else
	{
		pCacheContainer=pExistedContainer;
		pOutputPatern=pExistedContainer->GetVertexResult();
		delete pNewContainer;
	}
}

//void _CModelVertex::PrepareDirectNumericValue(const _CSubModelNumericPattern*& pOutputPatern, _CDirectNumContainer*& pCacheContainer,
//	const string* Context, _CSubCircuitSocket* pMasterSocket)
//{
//	if (this == GetGlobal0Vertex())
//	{
//		pOutputPatern = &ZeroPattern;
//		pCacheContainer = NULL;
//		return;
//	}
//	if (this == GetGlobal1Vertex())
//	{
//		pOutputPatern = &OnePattern;
//		pCacheContainer = NULL;
//		return;
//	}
//	if (this == GetGlobalM1Vertex())
//	{
//		pOutputPatern = &MOnePattern;
//		pCacheContainer = NULL;
//		return;
//	}
//	try
//	{
//		_CDirectNumContainer* pNewContainer = new _CDirectNumContainer(this, Context);
//		if (pNewContainer == NULL)
//			throw bad_alloc();
//		_CDirectNumContainer* pExistedContainer = m_OwnerCircuit->FindExisting(pNewContainer, pMasterSocket);
//		if (pExistedContainer == NULL)
//		{
//			pCacheContainer = pNewContainer;
//			_CSubModelNumericPattern* pPatern = pNewContainer->GetVertexResult();
//			if (IsLeaf())
//			{
//				size_t Index = m_OwnerCircuit->GetOuterVertexIndex(this);
//				pPatern->SetLeaf(true, Index, 1);
//			}
//			else
//			{
//				_CDescModelNumericPattern DescModelNumPatt(NoOfDescendants());
//				_CDirNumContainers DirNumContainers(NoOfDescendants());
//				_CDescSignsVector DescSignsVector(NoOfDescendants());
//				for (size_t i = 0; i<NoOfDescendants(); i++)
//				{
//					_CDescendantContainer& Desc = m_Descendants[i];
//					DescSignsVector[i] = Desc.m_Multiplier;
//					Desc.m_pDescendant->PrepareDirectNumericValue(DescModelNumPatt[i], DirNumContainers[i], Context, pMasterSocket);//Dodatkowy znak
//				}
//				m_pComponent->ApplyMe2Result(*pPatern, DescModelNumPatt, DescSignsVector, pMasterSocket, Context);///Bulba
//			}
//			m_OwnerCircuit->RegisterResult(pNewContainer, pMasterSocket);
//			pOutputPatern = pPatern;
//		}
//		else
//		{
//			pCacheContainer = pExistedContainer;
//			pOutputPatern = pExistedContainer->GetVertexResult();
//			delete pNewContainer;
//		}
//	}
//	catch (bad_alloc)
//	{
//	}
//}
//
//void _CModelVertex::PrepareDirectNumericalDivRem(const _CSubModelNumericPattern*& pOutputPatern, _CDirectNumContainer*& pCacheContainer, _CPathTraitor& Traitor,
//	_CSubCircuitSocket* pMasterSocket)
//{
//	if (this == GetGlobal0Vertex())
//	{
//		pOutputPatern = &ZeroPattern;
//		pCacheContainer = NULL;
//		return;
//	}
//	if (this == GetGlobal1Vertex())
//	{
//		pOutputPatern = &OnePattern;
//		pCacheContainer = NULL;
//		return;
//	}
//	if (this == GetGlobalM1Vertex())
//	{
//		pOutputPatern = &MOnePattern;
//		pCacheContainer = NULL;
//		return;
//	}
//	try
//	{
//		_CDirectNumContainer* pNewContainer = new _CDirectNumContainer(this, &Traitor.StrContext2());
//		if (pNewContainer == NULL)
//			throw bad_alloc();
//		_CDirectNumContainer* pExistedContainer = m_OwnerCircuit->FindExisting(pNewContainer, pMasterSocket);
//		if (pExistedContainer == NULL)
//		{
//			pCacheContainer = pNewContainer;
//			_CSubModelNumericPattern* pPatern = pNewContainer->GetVertexResult();
//			if (IsLeaf())
//			{
//				size_t Index = m_OwnerCircuit->GetOuterVertexIndex(this);
//				pPatern->SetLeaf(true, Index, 1);
//			}
//			else
//			{
//				const _CSubModelNumericPattern* pInternalPatern = m_pComponent->GetMyNumPattern(pMasterSocket, Traitor);
//				bool IsOperComp = Traitor.IsOperatorComponent(m_pComponent);
//				if (IsOperComp)
//					Traitor.NextFollower();
//				_CDirNumContainers DirNumContainers(NoOfDescendants());
//				if (m_Descendants.empty())
//					RISEPDD(eWrongCondition, "Invalid descendants");
//				for (size_t i = 0; i<NoOfDescendants(); i++)
//				{
//					if (pInternalPatern->get(i).empty())
//						continue;
//					_CDescendantContainer& Desc = m_Descendants[i];
//					short Mult = Desc.m_Multiplier;
//					if (Mult == 0)
//						continue;
//					const _CSubModelNumericPattern* pDescPattern = NULL;
//					Desc.m_pDescendant->PrepareDirectNumericalDivRem(pDescPattern, DirNumContainers[i], Traitor, pMasterSocket);
//					if (Mult == 1)
//						(*pPatern) += ((*pDescPattern)*(pInternalPatern->get(i)));
//					else
//					{
//						_CSparsePolynomial MultP;
//						MultP.put(0, Mult);
//						(*pPatern) += ((*pDescPattern)*(pInternalPatern->get(i))*MultP);
//					}
//				}
//				if (IsOperComp)
//					Traitor.PopLastLevel();
//			}
//			m_OwnerCircuit->RegisterResult(pNewContainer, pMasterSocket);
//			pOutputPatern = pPatern;
//		}
//		else
//		{
//			pCacheContainer = pExistedContainer;
//			pOutputPatern = pExistedContainer->GetVertexResult();
//			delete pNewContainer;
//		}
//	}
//	catch (bad_alloc)
//	{
//	}
//}

bool _CModelVertex::DescDescription(iostream& stream, _CDescendantContainer& Desc)
{
	short ExtSgn=1;
	long long OutParam=0;
	bool IsLeaf=Desc.m_pDescendant->IsLeaf();
	if(IsLeaf)
	{
		_CSubCircuit& SubCr=*dynamic_cast<_CSubCircuit*>(m_OwnerCircuit);
		//ExtSgn=SubCr.GetOuterVertexSign(Desc.m_pDescendant);
		OutParam=SubCr.GetBoundaryParamIndex(Desc.m_pDescendant);
	}
	else
	{
		OutParam=m_OwnerCircuit->GetModelVertexIndex(Desc.m_pDescendant);
	}
	if(OutParam==0 && !IsLeaf)
		return false;
	ExtSgn*=Desc.m_Multiplier;
	if(ExtSgn!=1)
		if(ExtSgn>=0)
			stream<<""<<ExtSgn<<"*";
		else
			stream<<"("<<ExtSgn<<")"<<"*";
		//stream<<""<<ExtSgn<<"*";
	if(IsLeaf)
		stream<<"EP";
	if(OutParam==-1)
		stream<<"(-1)";
	else
		stream<<""<<OutParam;
	return true;
}

void _CModelVertex::WriteToTextStream(iostream& stream)
{
	if(IsLeaf())
		return;
	else
	{
		if (m_pComponent == NULL)
		{
			stream << "-";
			return;
		}
		else
			stream<<m_pComponent->GetParameterName();
		stream<<"(";
		if(m_pComponent->IsMacroDependant())
		{
			bool IsInit=true;
			//PARAM_CONNECTIONS Connections=dynamic_cast<_CSubCircuitSocket*>(m_pComponent)->GetConnections();
			//for(unsigned i=0;i<Connections.size();i++)
			for(unsigned i=0;i<NoOfDescendants();i++)
			{
				//int C=Connections[i];
				//if(C<0)
				//	continue;
				//else
				//{
					//_CDescendantContainer& Desc=m_pDescendants[C];
					_CDescendantContainer& Desc=m_Descendants[i];
					if(Desc.m_pDescendant->Is0())
						continue;
					if(IsInit)
						IsInit=false;
					else
						stream<<", ";
					stream<<""<<i<<" = ";
					DescDescription(stream,Desc);
				//}
			}
		}
		else for(unsigned i=0;i<NoOfDescendants();i++)
		{
			if(i>0)
				stream<<", ";
			_CDescendantContainer& Desc=m_Descendants[i];
			if(!DescDescription(stream,Desc))
				stream<<"0";
		}
		stream<<")";
	}
	stream<<"\n";
}

void _CModelVertex::Skip0Suppressed()
{
	if(m_WasGarbageCleaned || this==GetGlobal0Vertex() || this==GetGlobal1Vertex())
		return;
	if(NoOfDescendants()==0)
	{
		m_WasGarbageCleaned=true;
		return;
	}
	for(unsigned i=0;i<NoOfDescendants();i++)
	{
		_CModelVertex* &pDesc=m_Descendants[i].m_pDescendant;
		pDesc->Skip0Suppressed(/*CutLeaves*/);
		int HollowOutput;
		if(pDesc->IsDegenerated(HollowOutput))
		{
			short NewMult=m_Descendants[i].m_Multiplier*pDesc->GetMultiplier(HollowOutput);
			_CModelVertex* pNewDesc=pDesc->GetDescendant(HollowOutput);
			pDesc->SetDescendant(HollowOutput,NULL,0);
			SetDescendant(i,pNewDesc,NewMult);
		}
	}
	m_WasGarbageCleaned=true;
}

//void _CModelVertex::SkipDuplicated(_CDuplicatedVertexCatcher& Catcher)
//{
//	for(unsigned short i=0;i<m_NoOfDescendants;i++)
//	{
//		_CModelVertex* pDescVertex=GetDescendant(i);
//		if(!pDescVertex->IsTerminal() && !pDescVertex->IsLeaf())
//		{
//			pDescVertex->SkipDuplicated(Catcher);
//			ReSetDescendant(i,Catcher.GetUnique(pDescVertex));
//		}
//	}
//}
void _CModelVertex::Skip0SupressedAndDuplicated(_CDuplicatedVertexCatcher& Catcher)
{
	if(m_WasGarbageCleaned || IsTerminal() || IsLeaf())
		return;
	for(unsigned short i=0;i<NoOfDescendants();i++)
	{
#ifdef _DEBUG
		long long MyIndex=m_OwnerCircuit->ModelVertex2Index(this);
#endif
		_CModelVertex* pDescVertex=m_Descendants[i].m_pDescendant;
		if(pDescVertex->IsTerminal() /*|| pDescVertex->IsLeaf()*/)
			continue;
		if(m_OwnerCircuit->IsDeletionAdmissibleDef() && pDescVertex->IsLeaf() && Catcher.IsSuppressedLeaf(pDescVertex))
		{
			SetDescendant(i,&Global0Vertex,0);
			continue;
		}
#ifdef _DEBUG
		long long Index=m_OwnerCircuit->ModelVertex2Index(pDescVertex);
#endif
		pDescVertex->Skip0SupressedAndDuplicated(Catcher);
		int HollowOutput;
		if(pDescVertex->IsDegenerated(HollowOutput))
		{
			short NewMult=m_Descendants[i].m_Multiplier*pDescVertex->GetMultiplier(HollowOutput);
			_CModelVertex* pNewDesc=pDescVertex->GetDescendant(HollowOutput);
			//pDescVertex->SetDescendant(1,NULL,0);
			SetDescendant(i,pNewDesc,NewMult);
			pDescVertex=pNewDesc;
		}
		else if(pDescVertex->IsEmptyMacro())
		{
			SetDescendant(i,&Global0Vertex,0);
		}
		//else if(!pDescVertex->IsTerminal() && !pDescVertex->IsLeaf())
		//{
		//	ReSetDescendant(i,Catcher.GetUnique(pDescVertex));
		//}
		else if(!pDescVertex->IsTerminal() || pDescVertex->WasGarbageCleaned()/*&& !pDescVertex->IsLeaf()*/)
		{
			_CModelVertex* pNewDescVertex=NULL;
			short Sgn=1;
			Catcher.GetUnique(pDescVertex,pNewDescVertex,Sgn);
			SetDescendant(i,pNewDescVertex,Sgn*m_Descendants[i].m_Multiplier);
		}
	}
	RealiseState();
	m_WasGarbageCleaned=true;
}

bool _CModelVertex::IsEqualDsc(const _CModelVertex* pOperand) const
{
	if (pOperand == NULL)
		RISEPDD(eWrongCondition, "Invalid Operrand");
	if(this==pOperand)
		return true;
	if(IsTerminal() || IsLeaf() || pOperand->IsTerminal() || pOperand->IsLeaf())
		return false;
	if(m_pComponent != pOperand->m_pComponent)
		return false;
	for(unsigned short i=0;i<NoOfDescendants();i++)
	{
		/*_CModelVertex::*/const _CDescendantContainer &LCont=m_Descendants[i],
			&PCont=pOperand->m_Descendants[i];
		if(LCont.m_pDescendant!=PCont.m_pDescendant || LCont.m_Multiplier!=PCont.m_Multiplier)
			return false;
	}
	return true;
}

unsigned long long _CModelVertex::GetHashDsc() const
{
	unsigned long long Res=HASH_CORE;
	Res*=HASH_FACTOR;
	Res^=(unsigned long long)m_pComponent;
	Res*=HASH_FACTOR;
	Res^=(unsigned long long)NoOfDescendants();
	for(unsigned short i=0;i<NoOfDescendants();i++)
	{
		/*_CModelVertex::*/const _CDescendantContainer& Cont=m_Descendants[i];
		Res*=HASH_FACTOR;
		Res^=(unsigned long long)Cont.m_pDescendant;
		Res*=HASH_FACTOR;
		Res^=(unsigned long long)Cont.m_Multiplier;
	}
	return Res;
}

_CModelVertex* _CModelVertex::GetNextHollowVertex()
{
	if(IsTerminal())
		return NULL;
	if(IsLeaf())
		return this;
	int HollowDesc=m_pComponent->HollowOutput();
	if(HollowDesc<0)
		return NULL;
	ASSERTPDD(HollowDesc<(int)NoOfDescendants());
	return m_Descendants[HollowDesc].m_pDescendant->GetNextHollowVertex();
}

long long _CModelVertex::GetMyId()
{
	ASSERTPDD(m_OwnerCircuit!=NULL);
	return m_OwnerCircuit->ModelVertex2Index(this);
}

IMPLEMENT_DYNAMIC_CREATION(_CModelVerticesList);

short _CDescSigns::ExtractCommonFactor()
{
	short ComFactor=1;
	while(!IsZero() && IsDevidedBy2())
	{
		ComFactor*=2;
		DevideBy2();
	}
	if(IsEachNegative())
	{
		ChangeSgn();
		ComFactor=-ComFactor;
	}
	return ComFactor;
}
bool _CDescSigns::IsZero()
{
	for(iterator it=begin(),_e=end();it!=_e;it++)
		if((*it)!=0)
			return false;
	return true;
}

bool _CDescSigns::IsDevidedBy2()
{
	for(iterator it=begin(),_e=end();it!=_e;it++)
		if(((*it)&(short)1)!=0)
			return false;
	return true;
}

void _CDescSigns::DevideBy2()
{
	for(iterator it=begin(),_e=end();it!=_e;it++)
		(*it)/=2;
}

bool _CDescSigns::IsEachNegative()
{
	for(iterator it=begin(),_e=end();it!=_e;it++)
		if((*it)>=0)
			return false;
	return true;
}

void _CDescSigns::ChangeSgn()
{
	for(iterator it=begin(),_e=end();it!=_e;it++)
		(*it)=-(*it);
}


void _CDuplicatedVertexCatcher::GetUnique(_CModelVertex* pInObject, _CModelVertex*& pOutObject, short& RootSgn)
{
	DUP_MAP::iterator dm_it=m_DupMap.find(pInObject);
	if(dm_it!=m_DupMap.end())
	{
		pOutObject=dm_it->second.first;
		RootSgn=dm_it->second.second;
		return;
	}
	if(pInObject->IsLeaf())
	{
		ASSERTPDD(m_pMVertex2Sgn!=NULL);
		m_OwnerCircuit.FindDuplicateLeaf(pInObject,pOutObject,RootSgn,m_pMVertex2Sgn);
	}
	else
	{
		if(!m_IgnoreSgnPropagation)
			RootSgn=pInObject->DetermineCommonDivider();
		pOutObject=_CDuplicationCatcher<_CModelVertex,_CDuplicatedVertexCatcher>::GetUnique(pInObject);
	}
	m_DupMap.insert(DUP_MAP::value_type(pInObject,DUP_RESULT(pOutObject,RootSgn)));
}

short _CDuplicatedVertexCatcher::GetOutSgn(_CModelVertex* pVertex)
{
	DUP_MAP::iterator it=m_DupMap.find(pVertex);
	if(it==m_DupMap.end())
		return 0;
	else
		return it->second.second;
}

bool _CDuplicatedVertexCatcher::IsSuppressedLeaf(_CModelVertex* pLeafVertex)
{
	if(!pLeafVertex->IsLeaf())
		return false;
	_CSubCircuit* pSubCircuit=dynamic_cast<_CSubCircuit*>(&m_OwnerCircuit);
	if(pSubCircuit==NULL)
		RISEPDD(eWrongCondition,"m_OwnerCircuit isnot a subcircuit");
	return pSubCircuit->IsSuppresedLeaf(pLeafVertex);
}