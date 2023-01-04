#include "StdAfx.h"
#include "ControledSources.h"
#include "StateGraphTables.h"
#include "Circuit.h"

_CControledSource::_CControledSource(_CCircuit* pOwnerCircuit) :_CSimplyComponent(pOwnerCircuit), m_p(0), m_r(0), m_k(0), m_l(0),
m_1Deletions(1, _CBasicSetOfDeletions(_CBasicPairOfDeletion(0, 0, 0, 0))), m_0Deletions(1, _CBasicSetOfDeletions())
{
	//Add1Deletions();
}


_CControledSource::_CControledSource(_CCircuit* pOwnerCircuit,string Name,int p, int r, int k, int l, long double Value,int sPower):
_CSimplyComponent(pOwnerCircuit, Name, Value), m_p(p), m_r(r), m_k(k), m_l(l), m_sPower(sPower<0 ? -sPower : sPower), m_IsInverted(sPower<0),
m_1Deletions(1, _CBasicSetOfDeletions(_CBasicPairOfDeletion(m_p, m_r, m_k, m_l))), m_0Deletions(1, _CBasicSetOfDeletions())
{
	//Add1Deletions(); 
	if (m_sPower != 0)
	{
		string str_power = ulong2str(sPower);
		if (m_IsInverted)
			m_Name = "[" + Name + "^("+str_power+")]";
		else
			m_Name = "[" + Name + "^" + str_power + "]";
	}
}

_CControledSource::~_CControledSource(void)
{
}

size_t _CControledSource::GetHash(size_t Core) const
{
	auto Res=_CSimplyComponent::GetHash(Core);
	Res*=HASH_FACTOR;
	Res^=m_p;
	Res*=HASH_FACTOR;
	Res^=m_r;
	Res*=HASH_FACTOR;
	Res^=m_k;
	Res*=HASH_FACTOR;
	Res^=m_l;
	Res*=HASH_FACTOR;
	Res^=m_sPower;
	Res*=HASH_FACTOR;
	Res ^= (size_t)m_IsInverted;
	return Res;
}

void _CControledSource::PlugInComponent(_CIntegrityTable& IntegrityTable)
{
	COMPONENT_PINS Pins;
	Pins.push_back(m_p);
	Pins.push_back(m_r);
	Pins.push_back(m_k);
	Pins.push_back(m_l);
	IntegrityTable.PlugComponents(Pins);
}

//void _CControledSource::PrepareHState(_CGraphState& HState, int ParamId)  const
//{
//	HState.DoShortCircuit(m_p,m_r,m_k,m_l);
//}

//void _CControledSource::PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState) const
//{
//
//}

void _CControledSource::CurrentPins(_CGraphTable& Tables,TWO_GRAPH_PINS& Pins) const
{
	int pr=abs(Tables.RealRow(m_p)),
		rr=abs(Tables.RealRow(m_r)),
		kr=abs(Tables.RealRow(m_k)),
		lr=abs(Tables.RealRow(m_l)),
		pc=abs(Tables.RealColumn(m_p)),
		rc=abs(Tables.RealColumn(m_r)),
		kc=abs(Tables.RealColumn(m_k)),
		lc=abs(Tables.RealColumn(m_l));
	Pins.push_back(TWO_GRAPH_PIN(pr,pc));
	Pins.push_back(TWO_GRAPH_PIN(rr,rc));
	Pins.push_back(TWO_GRAPH_PIN(kr,kc));
	Pins.push_back(TWO_GRAPH_PIN(lr,lc));
}

void _CControledSource::CheckDisconnection(_CGraphState* pState) const
{
	pState->IsSeparatedNode(m_p)||
		pState->IsSeparatedNode(m_r) ||
		pState->IsSeparatedNode(m_k) ||
		pState->IsSeparatedNode(m_l);
}
void _CControledSource::Load(_binary_filer& Filer)
{
	_CSimplyComponent::Load(Filer);
	Filer>>m_p>>m_r>>m_k>>m_l>>m_IsInverted;
}

void _CControledSource::Store(_binary_filer& Filer)
{
	_CSimplyComponent::Store(Filer);
	Filer<<m_p<<m_r<<m_k<<m_l<<m_IsInverted;
}

bool _CControledSource::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CControledSource& RC=*dynamic_cast<const _CControledSource*> (&RightComp);
	return (m_p==RC.m_p && m_r==RC.m_r && m_k==RC.m_k && m_l==RC.m_l && m_IsInverted==RC.m_IsInverted &&
		_CSimplyComponent::IsEqualIfSameType(RightComp));
}
void _CControledSource::WritePos(iostream& stream)
{
	stream<<"(P="<<m_pBaseCircuit->ConvertNode(m_p)<<", R="<<m_pBaseCircuit->ConvertNode(m_r)<<
		", K="<<m_pBaseCircuit->ConvertNode(m_k)<<", L="<<m_pBaseCircuit->ConvertNode(m_l)<<")";
}
void _CControledSource::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) 
{
	Numbers2Exchange.ExchangeIfNeede(m_p);
	Numbers2Exchange.ExchangeIfNeede(m_r);
	Numbers2Exchange.ExchangeIfNeede(m_k);
	Numbers2Exchange.ExchangeIfNeede(m_l);
}
void _CControledSource::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(4);
	Pins[0]=m_p;
	Pins[1]=m_r;
	Pins[2]=m_k;
	Pins[3]=m_l;
}

//void _CControledSource::GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions)
void _CControledSource::GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	switch (GetReal(Descendant))
	{
	case 0: pDeletions = &m_0Deletions; break;
	case 1: pDeletions = &m_1Deletions; break;
	default:
		RISEPDD(eWrongCondition,"Wrong no of descendant");
	}

}

unsigned _CControledSource::GetReal(unsigned DescNo)
{
	ASSERTPDD(DescNo==0 || DescNo==1);
	return m_IsInverted ? (DescNo==1?0:1) : DescNo;
}

//void _CTransconductance::PrepareLState(_CGraphState& LState,int ParamId) const
//{
//	LState.DoOpenCircuit(m_p,m_r,m_k,m_l);
//}
//void _CTransconductance::PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const
//{
//}

void _CTransconductance::ProcessDescendant(unsigned Desc, _CGraphState* pState)
{
	switch (GetReal(Desc))
	{
	case 0: pState->AddDeletion(m_p,m_r,m_k,m_l);
	case 1: break;
	default:
		RISEPDD(eWrongCondition, "Wrong no of descendant");
	}
}
//void _CTransconductance::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions)
void _CTransconductance::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	switch(ImportanceLevel)
	{
	case 1: pDeletions=&m_0Deletions; break;
	case 0: pDeletions=&m_1Deletions; break;
	default:
		RISEPDD(eWrongCondition, "Wrong no of descendant");
	}
}
IMPLEMENT_DYNAMIC_CREATION(_CTransconductance);
//void _CTransimpedance::PrepareLState(_CGraphState& LState,int ParamId) const
//{
//	LState.DoShortCircuit(m_p,m_r,m_p,m_r);
//	LState.DoShortCircuit(m_k,m_l,m_k,m_l);
//}
//void _CTransimpedance::PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const
//{
//}

void _CTransimpedance::ProcessDescendant(unsigned Desc, _CGraphState* pState)
{
	switch(GetReal(Desc))
	{
	case 0: pState->AddDeletion(m_p,m_r,m_k,m_l); break;
	case 1: if(pState->AddDeletion(m_p,m_r,m_p,m_r)!=TERM_SELFLOOP)
				pState->AddDeletion(m_k,m_l,m_k,m_l);
		break;
	default:
		RISEPDD(eWrongCondition, "Wrong no of descendant");
	}
}

//void _CTransimpedance::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions)
void _CTransimpedance::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	switch(ImportanceLevel)
	{
	case 0: pDeletions=&m_0Deletions; break;
	case 1: pDeletions=&m_1Deletions; break;
	default:
		RISEPDD(eWrongCondition, "Wrong no of descendant");
	}
}

IMPLEMENT_DYNAMIC_CREATION(_CTransimpedance);
//void _CVoltageGain::PrepareLState(_CGraphState& LState,int ParamId) const
//{
//	LState.DoShortCircuit(m_p,m_r,m_p,m_r);
//	LState.DoOpenCircuit(m_k,m_l,m_k,m_l);
//}
//void _CVoltageGain::PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const
//{
//}

void _CVoltageGain::ProcessDescendant(unsigned Desc, _CGraphState* pState)
{
	switch(GetReal(Desc))
	{
	case 0:pState->AddDeletion(m_p,m_r,m_k,m_l); break;
	case 1:pState->AddDeletion(m_p,m_r,m_p,m_r); break;
	default:
		RISEPDD(eWrongCondition, "Wrong no of descendant");
	}
}
//void _CVoltageGain::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions)
void _CVoltageGain::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	switch(ImportanceLevel)
	{
	case 0: pDeletions=&m_0Deletions; break;
	case 1: pDeletions=&m_1Deletions; break;
	default:
		RISEPDD(eWrongCondition, "Wrong no of descendant");
	}
}

IMPLEMENT_DYNAMIC_CREATION(_CVoltageGain);
//void _CCurrentGain::PrepareLState(_CGraphState& LState,int ParamId) const
//{
//	LState.DoShortCircuit(m_k,m_l,m_k,m_l);
//	LState.DoOpenCircuit(m_p,m_r,m_p,m_r);
//}
//void _CCurrentGain::PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const
//{
//}

void _CCurrentGain::ProcessDescendant(unsigned Desc, _CGraphState* pState)
{
	switch(GetReal(Desc))
	{
	case 0: pState->AddDeletion(m_p,m_r,m_k,m_l); break;
	case 1: pState->AddDeletion(m_k,m_l,m_k,m_l); break;
	default:
		RISEPDD(eWrongCondition, "Wrong no of descendant");
	}
}

//void _CCurrentGain::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions)
void _CCurrentGain::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	switch(ImportanceLevel)
	{
	case 0: pDeletions=&m_0Deletions; break;
	case 1: pDeletions=&m_1Deletions; break;
	default:
		RISEPDD(eWrongCondition, "Wrong no of descendant");
	}
}

IMPLEMENT_DYNAMIC_CREATION(_CCurrentGain);