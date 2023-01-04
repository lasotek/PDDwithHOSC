#include "StdAfx.h"
#include "TwoPorts.h"
#include "StateGraphTables.h"
#include "Vertex.h"
#include "Circuit.h"

_CTwoPort::~_CTwoPort(void)
{
}

size_t _CTwoPort::GetHash(size_t Core) const
{
	auto Res=_CSimplyComponent::GetHash(Core);
	Res*=HASH_FACTOR;
	Res^=m_NodeA;
	Res*=HASH_FACTOR;
	Res^=m_NodeB;
	Res*=HASH_FACTOR;
	Res^=(unsigned long)IsAdmittance();
	Res*=HASH_FACTOR;
	Res^=(unsigned long)sPower();
	return Res;
}

void _CTwoPort::PlugInComponent(_CIntegrityTable& IntegrityTable)
{
	COMPONENT_PINS Pins;
	Pins.push_back(m_NodeA);
	Pins.push_back(m_NodeB);
	IntegrityTable.PlugComponents(Pins);
}
//void _CTwoPort::PrepareLState(_CGraphState& LState, int ParamId) const
//{
//	if(IsAdmittance())
//		LState.DoOpenCircuit(m_NodeA,m_NodeB,m_NodeA,m_NodeB);
//	else
//		LState.DoShortCircuit(m_NodeA,m_NodeB,m_NodeA,m_NodeB);
//}
//
//void _CTwoPort::PrepareHState(_CGraphState& HState,int ParamId) const
//{
//	if(IsAdmittance())
//		HState.DoShortCircuit(m_NodeA,m_NodeB,m_NodeA,m_NodeB);
//	else
//		HState.DoOpenCircuit(m_NodeA,m_NodeB,m_NodeA,m_NodeB);
//}

//void _CTwoPort::PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const
//{
//}

void _CTwoPort::CurrentPins(_CGraphTable& Tables,TWO_GRAPH_PINS& Pins) const
{
	int ar=abs(Tables.RealRow(m_NodeA)),
		br=abs(Tables.RealRow(m_NodeB)),
		ac=abs(Tables.RealColumn(m_NodeA)),
		bc=abs(Tables.RealColumn(m_NodeB));
	Pins.push_back(TWO_GRAPH_PIN(ar,ac));
	Pins.push_back(TWO_GRAPH_PIN(br,bc));
}

void _CTwoPort::ProcessDescendant(unsigned Desc, _CGraphState* pState)
{
	ASSERTPDD(Desc==0 || Desc==1);
	if((Desc==0 && IsAdmittance()) || (Desc==1 && !IsAdmittance()))
		pState->AddDeletion(m_NodeA,m_NodeB,m_NodeA,m_NodeB);
}

void _CTwoPort::CheckDisconnection(_CGraphState* pState) const
{
	pState->IsSeparatedNode(m_NodeA) || pState->IsSeparatedNode(m_NodeB);
}

void _CTwoPort::Load(_binary_filer& Filer)
{
	_CSimplyComponent::Load(Filer);
	Filer>>m_NodeA>>m_NodeB;
}

void _CTwoPort::Store(_binary_filer& Filer)
{
	_CSimplyComponent::Store(Filer);
	Filer<<m_NodeA<<m_NodeB;
}
bool _CTwoPort::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CTwoPort& RC=*dynamic_cast<const _CTwoPort*> (&RightComp);
	return (m_NodeA==RC.m_NodeA && m_NodeB==RC.m_NodeB &&
		_CSimplyComponent::IsEqualIfSameType(RightComp));
}
void _CTwoPort::WritePos(iostream& stream)
{
	stream<<"(NodeA="<<m_pBaseCircuit->ConvertNode(m_NodeA)<<", NodeB="<<m_pBaseCircuit->ConvertNode(m_NodeB)<<")";
}
void _CTwoPort::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) 
{
	Numbers2Exchange.ExchangeIfNeede(m_NodeA);
	Numbers2Exchange.ExchangeIfNeede(m_NodeB);
}

void _CTwoPort::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(2);
	Pins[0]=m_NodeA;
	Pins[1]=m_NodeB;
}

IMPLEMENT_DYNAMIC_CREATION(_CCapacitance);
IMPLEMENT_DYNAMIC_CREATION(_CConductance);
IMPLEMENT_DYNAMIC_CREATION(_CInductance);
IMPLEMENT_DYNAMIC_CREATION(_CResistance);