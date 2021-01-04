#include "StdAfx.h"
#include "NextComponentOptimizer.h"


_CNextComponentOptimizer::~_CNextComponentOptimizer(void)
{
}

_CNextComponentOptimizer2::GET_GRAPH_STATE_RES _CNextComponentOptimizer2::GetDeterminedGraphState( _CModelVertex* pAncestorVertex,
		size_t NoOfDescendant,  _CComponent* pComponent)
{
	if(pComponent==NULL)
		pComponent=m_LastSelectedComponent;
	if(pComponent==NULL)
		return GET_GRAPH_STATE_RES(false,NULL);
	COMP_REL_MAP::iterator _comp_it=m_CompStateMap.find(pComponent);
	if(_comp_it!=m_CompStateMap.end())
	{
		MVERTEX_STATE& MVState=_comp_it->second;
		MVERTEX_STATE::iterator _mv_it=MVState.find(pAncestorVertex);
		if(_mv_it!=MVState.end())
		{
			DESC_STATE& DescState=_mv_it->second;
			DESC_STATE::iterator _st_it=DescState.find(NoOfDescendant);
			if(_st_it!=DescState.end())
				return GET_GRAPH_STATE_RES(true,_st_it->second);
		}
	}
	return GET_GRAPH_STATE_RES(false,NULL);
}
void _CNextComponentOptimizer2::StoreDeterminedGraphState( _CModelVertex* pAncestorVertex,
		size_t NoOfDescendant,  _CComponent* pComponent,const _CGraphState* pGraphState)
{
	MVERTEX_STATE& MVState=m_CompStateMap[pComponent];
	DESC_STATE& DescState=MVState[pAncestorVertex];
	DescState[NoOfDescendant]=pGraphState;
}

_CComponent* _CNextComponentOptimizer2::GetNextComponent(bool& IsLastOne)
{
	m_LastSelectedComponent=NULL;
	switch(m_CurrentComponents.size())
	{
	case 0: return NULL;
	case 1:
		{
			_CComponent* pCom=m_CurrentComponents.back();
			m_CurrentComponents.pop_back();
			IsLastOne=true;
			m_CompStateMap.clear();
			m_LastSelectedComponent=pCom;
			return pCom;
		}
	default:
		{
			m_CompStateMap.clear();
			IsLastOne=false;
			for(_CComponents::iterator cit=m_CurrentComponents.begin();cit!=m_CurrentComponents.end();cit++)
			{
				//_CComponent* pComonent=(*cit);//do treeFork
				_CTreeForkComponent* pComonent=dynamic_cast<_CTreeForkComponent*>(*cit);
				if(pComonent==NULL)
					RISEPDD(eWrongComponentType,"Empty ot unexpected type of component");
				size_t NoOfDesc=pComonent->NoOfDescendants();
				for(_CModelVerticesList::iterator vit=m_AnalyzeList.begin();vit!=m_AnalyzeList.end();vit++)
				{
					_CModelVertex* pVertex=(*vit);
					_CGraphState TopState(*pVertex->GetInputState());
					TWO_GRAPH_PINS Pins;
					pComonent->CurrentPins(*TopState.GetGraphTable(),Pins);
					TopState.GetIntegrityTable()->UnPlugComponent(Pins);
					for(size_t DescNo=0;DescNo<NoOfDesc;DescNo++)
					{
						_CGraphState* pNewState=new _CGraphState(TopState,true);//niebezpiecznie
						pComonent->ProcessDescendant(DescNo,pNewState);
						//_CGraphTable* pGraphTable=pNewState->GetGraphTable();
						//_CIntegrityTable* pIntegrityTable=pNewState->GetIntegrityTable();
						//pIntegrityTable->UnPlugComponent(Pins);
					}
				}
			}
		}
	}
	return NULL;
}

_CComponent* _CNextComponentOptimizer::GetNextComponent(bool& IsLastOne)
{
	_CComponent* pRes=NULL;
	switch(m_CurrentComponents.size())
	{
	case 0: break;
	case 1:
		pRes=m_CurrentComponents.front();
		m_CurrentComponents.pop_back();
		IsLastOne=true;
		break;
	default:
		IsLastOne=false;
		m_CurrentOptimal.clear();
		for(_CComponents::iterator cit=m_CurrentComponents.begin();cit!=m_CurrentComponents.end();cit++)
		{
			_CPredictionResult Res(m_MinInternal,m_MaxInternal,(*cit)->HollowOutput());
			size_t NoOfNewDescendatns=0;
			size_t NoOfInvalidNewDescendants=0;
			size_t TheSmallestIncidence=numeric_limits<size_t>::max();
			size_t TheSmallestIncidenceRep=0;
			for(_CModelVerticesList::iterator vit=m_AnalyzeList.begin();vit!=m_AnalyzeList.end();vit++)
			{
				(*cit)->CheckNextStep(Res,*(*vit)->GetInputState(),m_MinInternal,m_MaxInternal);
				NoOfNewDescendatns=Res.GetNoActiveDescendants();
				NoOfInvalidNewDescendants=Res.GetNoNotActiveDescendants();
				size_t TSI=Res.GetSmallestIncidentToGet();
				if(TSI==numeric_limits<size_t>::max())
					continue;
				if(TSI==TheSmallestIncidence)
					TheSmallestIncidenceRep+=Res.GetSmallestIncidentRepetiotion();
				else if(TSI<TheSmallestIncidence)
				{
					TheSmallestIncidence=TSI;
					TheSmallestIncidenceRep=Res.GetSmallestIncidentRepetiotion();
				}
			}
			if(m_CurrentOptimal.m_CurrentMinDesc<NoOfNewDescendatns)
				continue;
			if(!m_CurrentOptimal.m_IsCommon && Res.IsCommon())
				continue;
			if(m_CurrentOptimal.m_CurrentMinDesc>NoOfNewDescendatns)
			{
				m_CurrentOptimal.clear();
				m_CurrentOptimal.m_CurrentMinDesc=NoOfNewDescendatns;
				m_CurrentOptimal.m_IsCommon=Res.IsCommon();
			}
			m_CurrentOptimal.push_back(_CData(cit,NoOfNewDescendatns,NoOfInvalidNewDescendants,
				TheSmallestIncidence,TheSmallestIncidenceRep));
		}
		_CComponents::iterator FinalResIt=(*m_CurrentOptimal.GetTheMostOptimal()).m_CompIterator;
		pRes=(*FinalResIt);
		m_CurrentComponents.erase(FinalResIt);
	}
	return pRes;
}
bool _CNextComponentOptimizer::_CData::operator<(const _CData& Right)
{
	if(m_TheSmallestIncidence<Right.m_TheSmallestIncidence)
		return true;
	if(m_TheSmallestIncidence==Right.m_TheSmallestIncidence && 
		m_TheSmallestIncidenceRep>Right.m_TheSmallestIncidenceRep)
		return true;
	_CComponent &Comp=**m_CompIterator,
		&RComp=**Right.m_CompIterator;
	if(Comp.IsControledSource() && !RComp.IsControledSource())
		return true;
	if(Comp.IsMacroDependant() && !RComp.IsMacroDependant())
		return true;
	return false;
}

_CNextComponentOptimizer::__CurrentList::iterator 
_CNextComponentOptimizer::__CurrentList::GetTheMostOptimal()
{
	iterator Res=begin();
	iterator it=begin();
	if(it!=end()) 
		it++;
	for(;it!=end();it++)
		if((*it)<(*Res))
			Res=it;
	return Res;
}
