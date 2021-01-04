#include "StdAfx.h"
#include "PredictionResult.h"

void _CPredictionResult::TestExistance(_CGraphState* pState,TWO_GRAPH_PINS& Pins)
{
	//if(m_StateCache.FindExisting(pState)!=NULL)
	//{
	//	delete pState;
	//}
	//else
	if(m_StateCache.FindExisting(pState)==NULL)
	{
		m_StateCache.ForceIntoCache(pState);
		FindTheSmolestIncidence(*pState,Pins);
		pState->m_IsRegistered=true;
		m_NoActiveDescendants++;
	}
}

void _CPredictionResult::FindTheSmolestIncidence(_CGraphState& State,TWO_GRAPH_PINS& Pins)
{
	size_t TheSmallest;
	size_t TheSmallestRep;
	State.FindTheSmallestIncidence(m_RSet, m_CSet, Pins, m_Min,m_Max,TheSmallest,TheSmallestRep);
	if(m_SmallestIncidentToGet>TheSmallest)
	{
		m_SmallestIncidentToGet=TheSmallest;
		m_SmallestIncidentRepetiotion=TheSmallestRep;
	}
	else if(m_SmallestIncidentToGet==TheSmallest)
		m_SmallestIncidentRepetiotion+=TheSmallestRep;
}

void _CPredictionResult::NotifyActiveDescendent(size_t NoOfDescentant)
{
	if(!m_CommonComponent) return ;
	if(NoOfDescentant==m_HollowDescendent)
		m_CommonComponent=false;
	if(m_CommonDescendant==numeric_limits<size_t>::max())
		m_CommonDescendant=NoOfDescentant;
	if(m_CommonDescendant!=NoOfDescentant)
		m_CommonComponent=false;
}