#include "stdafx.h"
#include "Deletions.h"
#include "Cofactor.h"
#include "NewCore.h"

_CBasicSetOfDeletions EmptyDeletionsSet;

_CMultiBasicSetOfDeletions EmptyMultiDeletionsSet(1, EmptyDeletionsSet);

void _CBasicPairOfDeletion::Translate(const PINS_COLOCATIONS& PinsColocations)
{
	PINS_COLOCATIONS::const_iterator it=PinsColocations.find(m_p);
	if(it==PinsColocations.end())
		RISEPDD(eWrongPins,(ulong2str(m_p)+" is not a boundary pin").c_str());
	m_p=it->second;
	it=PinsColocations.find(m_r);
	if(it==PinsColocations.end())
		RISEPDD(eWrongPins,(ulong2str(m_r)+" is not a boundary pin").c_str());
	m_r=it->second;
	it=PinsColocations.find(m_k);
	if(it==PinsColocations.end())
		RISEPDD(eWrongPins,(ulong2str(m_k)+" is not a boundary pin").c_str());
	m_k=it->second;
	it=PinsColocations.find(m_l);
	if(it==PinsColocations.end())
		RISEPDD(eWrongPins,(ulong2str(m_l)+" is not a boundary pin").c_str());
	m_l=it->second;
}

_CBasicSetOfDeletions::_CBasicSetOfDeletions(const _CDeletions& Dels) : m_Sgn(Dels.Sgn())
{
	for(_CDeletions::const_iterator it=Dels.begin(),_e=Dels.end();it!=_e;it++)
	{
		const _CDeletion* pDel=*it;
		push_back(_CBasicPairOfDeletion(pDel->P(),pDel->R(),pDel->K(),pDel->L())); //pDel->
	}
}

_CBasicSetOfDeletions::_CBasicSetOfDeletions(const _CBasicSetOfDeletions& Source1, const _CBasicSetOfDeletions& Source2) :
m_Sgn(1)
{
	_CComplexDeletionsSimplifier Simplifier(Source1);
	if (Simplifier.IsSelfLoop())
	{
		m_Sgn = 0;
		return;
	}
	if (!Simplifier.AddDeletions(Source2))
	{
		m_Sgn = 0;
		return;
	}
	Simplifier.Translate(*this);
}

void _CBasicSetOfDeletions::Translate(const PINS_COLOCATIONS& PinsColocations)
{
	_CBasicSetOfDeletions NewOne;
	for(iterator pit=begin(),_e=end();pit!=_e;pit++)
	{
		int p=pit->m_p;
		PINS_COLOCATIONS::const_iterator it=PinsColocations.find(p);
		if(it==PinsColocations.end())
			RISEPDD(eWrongPins,(ulong2str(p)+" is not a boundary pin").c_str());
		p=it->second;
		int r=pit->m_r;
		it=PinsColocations.find(r);
		if(it==PinsColocations.end())
			RISEPDD(eWrongPins,(ulong2str(r)+" is not a boundary pin").c_str());
		r=it->second;
		int k=pit->m_k;
		it=PinsColocations.find(k);
		if(it==PinsColocations.end())
			RISEPDD(eWrongPins,(ulong2str(k)+" is not a boundary pin").c_str());
		k=it->second;
		int l=pit->m_l;
		it=PinsColocations.find(l);
		if(it==PinsColocations.end())
			RISEPDD(eWrongPins,(ulong2str(l)+" is not a boundary pin").c_str());
		l=it->second;
		NewOne.push_back(_CBasicPairOfDeletion(p,r,k,l));
	}
	clear();
	assign(NewOne.begin(),NewOne.end());
}
