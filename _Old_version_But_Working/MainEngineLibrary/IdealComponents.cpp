#include "StdAfx.h"
#include "IdealComponents.h"
#include "Circuit.h"
#include "Vertex.h"

_CIdealComponent::_CIdealComponent(_CCircuit* pOwner):_CComponent(pOwner)
{
}

_CIdealComponent::~_CIdealComponent(void)
{
}
_CNullor::_CNullor(_CCircuit* pOwner,int p, int r, int k, int l):_CIdealComponent(pOwner),m_p(p),m_r(r),m_k(k),m_l(l)
{
	AddDeletions();
}

size_t _CNullor::GetHash(size_t Core) const
{
	Core*=HASH_FACTOR;
	Core^=m_p;
	Core*=HASH_FACTOR;
	Core^=m_r;
	Core*=HASH_FACTOR;
	Core^=m_k;
	Core*=HASH_FACTOR;
	Core^=m_l;
	return Core;
}
//void _CNullor::PrepareLState(_CGraphState& LState,int ParamId) const
//{
//	LState.DoShortCircuit(m_p,m_r,m_k,m_l, true);
//}

void _CNullor::ProcessModel(_CModelVertex& ParentVertex, _CGraphState* pNewState,unsigned ReduceToNoNodes)
{
	pNewState->AddDeletion(m_p,m_r,m_k,m_l);
	pNewState->ReduceToNoOfNodes(ReduceToNoNodes);
}

void _CNullor::Load(_binary_filer& Filer)
{
	Filer>>m_p>>m_r>>m_k>>m_l;
}

void _CNullor::Store(_binary_filer& Filer)
{
	Filer<<m_p<<m_r<<m_k<<m_l;
}

bool _CNullor::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CNullor& RC=*dynamic_cast<const _CNullor*>(&RightComp);
	return (m_p==RC.m_p && m_r==RC.m_r && m_k==RC.m_k && m_l==RC.m_l);
}

void _CNullor::WritePos(iostream& stream)
{
	stream<<"(O+="<<m_pBaseCircuit->ConvertNode(m_p)<<", O-="<<m_pBaseCircuit->ConvertNode(m_r)<<
		", I+="<<m_pBaseCircuit->ConvertNode(m_k)<<", I-="<<m_pBaseCircuit->ConvertNode(m_l)<<")";
}

void _CNullor::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) 
{
	Numbers2Exchange.ExchangeIfNeede(m_p);
	Numbers2Exchange.ExchangeIfNeede(m_r);
	Numbers2Exchange.ExchangeIfNeede(m_k);
	Numbers2Exchange.ExchangeIfNeede(m_l);
	m_SetDeletion.clear();
	AddDeletions();
}

void _CNullor::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(4);
	Pins[0]=iAbs(m_p);
	Pins[1]=iAbs(m_r);
	Pins[2]=iAbs(m_k);
	Pins[3]=iAbs(m_l);
}

IMPLEMENT_DYNAMIC_CREATION(_CNullor);

_COpAmp::_COpAmp(_CCircuit* pOwner, int II, int NI, int Out):
	_CNullor(pOwner,Out,0,II,NI)
{

}

void _COpAmp::WritePos(iostream& stream)
{
	stream<<"(O="<<m_pBaseCircuit->ConvertNode(m_p)<<", Ref="<<m_pBaseCircuit->ConvertNode(m_r)<<
		", I-="<<m_pBaseCircuit->ConvertNode(m_k)<<", I+="<<m_pBaseCircuit->ConvertNode(m_l)<<")";
}

IMPLEMENT_DYNAMIC_CREATION(_COpAmp);

//void _CCurrentConveyor::PlugInComponent(_CIntegrityTable& IntegrityTable)
//{
//	COMPONENT_PINS Pins;
//	Pins.push_back(m_X);
//	Pins.push_back(m_Y);
//	Pins.push_back(m_Z);
//	IntegrityTable.PlugComponents(Pins);
//}

//void _CCurrentConveyor::PrepareLState(_CGraphState& LState, int ParamId) const
//{
//	LState.DoShortCircuit(m_X,(m_Type==CCIIp || m_Type==ICCIIp)?-m_Z:m_Z,m_X,(m_Type==ICCIIm || m_Type==ICCIIp)?-m_Y:m_Y,true);
//}
size_t _CCurrentConveyor::GetHash(size_t Core) const
{
	Core*=HASH_FACTOR;
	Core^=m_Type;
	Core*=HASH_FACTOR;
	Core^=m_X;
	Core*=HASH_FACTOR;
	Core^=m_Y;
	Core*=HASH_FACTOR;
	Core^=m_Z;
	return Core;
}
void _CCurrentConveyor::ProcessModel(_CModelVertex& ParentVertex, _CGraphState* pNewState,unsigned ReduceToNoNodes)
{
	pNewState->AddDeletion(m_X,(m_Type==CONV_CCIIp || m_Type==CONV_ICCIIp)?-m_Z:m_Z,m_X,(m_Type==CONV_ICCIIm || m_Type==CONV_ICCIIp)?-m_Y:m_Y);
	pNewState->ReduceToNoOfNodes(ReduceToNoNodes);
}

void _CCurrentConveyor::AddDeletions()
{
	_CBasicSetOfDeletions Set;
	Set.push_back(_CBasicPairOfDeletion(m_X, (m_Type == CONV_CCIIp || m_Type == CONV_ICCIIp) ? -m_Z : m_Z, m_X, (m_Type == CONV_ICCIIm || m_Type == CONV_ICCIIp) ? -m_Y : m_Y));
	m_SetDeletion.push_back(Set);
}

void _CCurrentConveyor::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) 
{
	Numbers2Exchange.ExchangeIfNeede(m_X);
	Numbers2Exchange.ExchangeIfNeede(m_Y);
	Numbers2Exchange.ExchangeIfNeede(m_Z);
}
void _CCurrentConveyor::RaportConnections(COMPONENT_PINS& Pins) const 
{
	Pins.resize(4);
	Pins[0]=0;
	Pins[1]=m_X;
	Pins[2]=m_Y;
	Pins[3]=m_Z;
}

void _CCurrentConveyor::Load(_binary_filer& Filer)
{
	Filer>>m_X>>m_Y>>m_Z>>m_Type;
}

void _CCurrentConveyor::Store(_binary_filer& Filer)
{
	Filer<<m_X<<m_Y<<m_Z<<m_Type;
}

bool _CCurrentConveyor::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CCurrentConveyor& RC=*dynamic_cast<const _CCurrentConveyor*>(&RightComp);
	return (m_X==RC.m_X && m_Y==RC.m_Y && m_Z==RC.m_Z && m_Type==RC.m_Type);
}

void _CCurrentConveyor::WritePos(iostream& stream)
{
	stream<<"(X="<<m_pBaseCircuit->ConvertNode(m_X)<<", Y="<<m_pBaseCircuit->ConvertNode(m_Y)<<
		", Z="<<m_pBaseCircuit->ConvertNode(m_Z)<<")";
}

char* _CCurrentConveyor::m_TypeStr[]={"CCII+","CCII-","ICCII+","ICCII-"};
IMPLEMENT_DYNAMIC_CREATION(_CCurrentConveyor);