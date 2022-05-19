#pragma once
#include "Components.h"
#include "Vertex.h"
#include "StateGraphTables.h"
#include "PredictionResult.h"

class _CNextComponentOptimizer
{
public:
	_CNextComponentOptimizer(/*const*/ _CComponents& InitComponentList,
		_CModelVerticesList& AnalyzeList,size_t MinInternal, size_t MaxInternal):
		m_CurrentComponents(InitComponentList), m_AnalyzeList(AnalyzeList),
			m_MinInternal(MinInternal),m_MaxInternal(MaxInternal) {}
	~_CNextComponentOptimizer(void);
	_CComponent* GetNextComponent(bool& IsLastOne);
protected:
	_CModelVerticesList& m_AnalyzeList;
	_CComponents& m_CurrentComponents;
	//_CComponents m_CurrentComponents;
	//_CGraphStateCache m_StateCache;
	size_t m_MinInternal;
	size_t m_MaxInternal;
	class _CData
	{
	public:
		_CData(_CComponents::iterator CompIterator,
		size_t NoOfActiveDescendants,
		size_t NoOfNotActiveDescendants,
		size_t TheSmallestIncidence,
		size_t TheSmallestIncidenceRep):
				m_CompIterator(CompIterator),
				m_NoOfActiveDescendants(NoOfActiveDescendants),
				m_NoOfNotActiveDescendants(NoOfNotActiveDescendants),
				m_TheSmallestIncidence(TheSmallestIncidence),
				m_TheSmallestIncidenceRep(TheSmallestIncidenceRep){}
		_CData(const _CData& Source): 
				m_CompIterator(Source.m_CompIterator),
				m_NoOfActiveDescendants(Source.m_NoOfActiveDescendants),
				m_NoOfNotActiveDescendants(Source.m_NoOfNotActiveDescendants),
				m_TheSmallestIncidence(Source.m_TheSmallestIncidence),
				m_TheSmallestIncidenceRep(Source.m_TheSmallestIncidenceRep){}
		//_CData() {}
		_CComponents::iterator m_CompIterator;
		size_t m_NoOfActiveDescendants;
		size_t m_NoOfNotActiveDescendants;
		size_t m_TheSmallestIncidence;
		size_t m_TheSmallestIncidenceRep;
		bool operator<(const _CData& Right);
	};
	class __CurrentList : public list<_CData>
	{
	public:
		__CurrentList():m_CurrentMinDesc(numeric_limits<unsigned int>::max()),m_IsCommon(true) {}
		size_t m_CurrentMinDesc;
		bool m_IsCommon;
		inline void clear()
		{
			list<_CData>::clear();
			m_CurrentMinDesc=numeric_limits<unsigned int>::max();
			m_IsCommon=true;
		}
		iterator GetTheMostOptimal();
	};
	__CurrentList m_CurrentOptimal;
};

//Nowoœæ
class _CNextComponentOptimizer2
{
public:
	_CNextComponentOptimizer2(const _CComponents& InitComponentList,
		_CModelVerticesList& AnalyzeList,size_t MinInternal, size_t MaxInternal,bool Multileaves):
		m_CurrentComponents(InitComponentList), m_AnalyzeList(AnalyzeList),
			m_MinInternal(MinInternal),m_MaxInternal(MaxInternal),m_Multileaves(Multileaves),
		m_LastSelectedComponent(NULL) {}
	_CComponent* GetNextComponent(bool& IsLastOne);
	typedef pair<bool,const _CGraphState*> GET_GRAPH_STATE_RES; 
	GET_GRAPH_STATE_RES GetDeterminedGraphState( _CModelVertex* pAncestorVertex,
		size_t NoOfDescendant,  _CComponent* pComponent=NULL/*LastSelected*/);
protected:
	void StoreDeterminedGraphState( _CModelVertex* pAncestorVertex,
		size_t NoOfDescendant,  _CComponent* pComponent,const _CGraphState* pGraphState);
	_CModelVerticesList& m_AnalyzeList;
	_CComponents m_CurrentComponents;
	_CGraphStateCache m_LocalStateCache;
	size_t m_MinInternal;
	size_t m_MaxInternal;
	class DESC_STATE : public unordered_map<size_t/*Desc Index*/,const _CGraphState* /*Expected state modification*/>
	{
	};
	//typedef unordered_map<size_t/*Desc Index*/,_CGraphState* /*Expected state modification*/> DESC_STATE;
	class MVERTEX_STATE : public unordered_map<_CModelVertex* /*Ancestor*/,DESC_STATE>
	{
	};
	//typedef unordered_map<_CModelVertex* /*Ancestor*/,DESC_STATE> MVERTEX_STATE;
	class COMP_REL_MAP : public unordered_map<_CComponent*,MVERTEX_STATE>
	{
	};
	//typedef unordered_map<_CComponent*,MVERTEX_STATE> COMP_REL_MAP;
	COMP_REL_MAP m_CompStateMap;
	bool m_Multileaves;
	_CComponent* m_LastSelectedComponent;
};