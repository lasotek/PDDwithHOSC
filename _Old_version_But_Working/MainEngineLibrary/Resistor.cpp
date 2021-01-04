#include "StdAfx.h"
#include "Resistor.h"
#include "Circuit.h"


_CResistor::~_CResistor(void)
{
}

long _CResistor::GetHash(int ParamId)
{
	long Res=::GetHash(m_Name);
	Res*=31;
	Res+=m_NodeA;
	Res*=31;
	Res+=m_NodeB;
	Res*=31;
	Res+=(long)m_AsConductance;
	return Res;
}

void _CResistor::PlugInComponent(_CIntegrityTable& IntegrityTable)
{
	COMPONENT_PINS Pins;
	Pins.push_back(m_NodeA);
	Pins.push_back(m_NodeB);
	IntegrityTable.PlugComponents(Pins);
}
void _CResistor::PrepareLHStates(_CGraphState& HState, _CGraphState& LState)
{
}