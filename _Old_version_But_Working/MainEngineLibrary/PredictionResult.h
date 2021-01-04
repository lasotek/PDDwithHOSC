#pragma once
#include "StateGraphTables.h"
#include <set>
#include <limits.h>
class _CPredictionResult
{
public:
	~_CPredictionResult()
	{
	}
	_CPredictionResult(size_t Min, size_t Max,int HollowDescendent):
		m_NoActiveDescendants(0), m_NoNotActiveDescendants(0),
		m_SmallestIncidentToGet(numeric_limits<size_t>::max()), m_SmallestIncidentRepetiotion(0),m_Min(Min),m_Max(Max),
		m_CommonComponent(true), m_CommonDescendant(numeric_limits<size_t>::max()),m_HollowDescendent(HollowDescendent) {}
	size_t GetNoActiveDescendants() {return m_NoActiveDescendants;}
	size_t GetNoNotActiveDescendants() {return m_NoNotActiveDescendants;}
	size_t GetSmallestIncidentToGet() {return m_SmallestIncidentToGet;}
	size_t GetSmallestIncidentRepetiotion() {return m_SmallestIncidentRepetiotion;}
	void NotifyNotActive() {m_NoNotActiveDescendants++;}
	void TestExistance(_CGraphState* pState,TWO_GRAPH_PINS& Pins);
	void FindTheSmolestIncidence(_CGraphState& State,TWO_GRAPH_PINS& Pins);
	void NotifyActiveDescendent(size_t NoOfDescentant);
	typedef set<size_t> RC_SET;
	bool IsCommon() const {return m_CommonComponent;}
protected:
	size_t m_NoActiveDescendants;
	size_t m_NoNotActiveDescendants;
	size_t m_SmallestIncidentToGet;
	size_t m_SmallestIncidentRepetiotion;
	size_t m_Max;
	size_t m_Min;
	int m_HollowDescendent;
	bool m_CommonComponent;
	size_t m_CommonDescendant;
	RC_SET m_RSet,m_CSet;
	_CGraphStateCache m_StateCache;
};
