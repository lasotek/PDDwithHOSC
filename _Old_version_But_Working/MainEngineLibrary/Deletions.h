#pragma once

class _CDeletion;
class _CDeletions;
class _CCofactor;
class _CBasicPairOfDeletion
{
public:
	_CBasicPairOfDeletion(int p, int r, int k, int l):m_p(p), m_r(r), m_k(k),m_l(l) {}
	_CBasicPairOfDeletion(const _CDeletion& Del);
	void Translate(const PINS_COLOCATIONS& PinsColocations);
	int m_p, m_r, m_k, m_l;
};

//typedef list<const _CBasicPairOfDeletion> _CBasicSetOfDeletions;
class _CBasicSetOfDeletions : public list<const _CBasicPairOfDeletion>
{
public:
	_CBasicSetOfDeletions() :list<const _CBasicPairOfDeletion>(), m_Sgn(1) {}
	_CBasicSetOfDeletions(const _CBasicPairOfDeletion& Set,short Sgn = 1) :list<const _CBasicPairOfDeletion>(1,Set), m_Sgn(Sgn) {}
	_CBasicSetOfDeletions(const _CBasicSetOfDeletions& Source) :
		list<const _CBasicPairOfDeletion>(Source),m_Sgn(Source.m_Sgn) {}
	_CBasicSetOfDeletions(const _CDeletions& Dels);
	_CBasicSetOfDeletions(const _CBasicSetOfDeletions& Source1, const _CBasicSetOfDeletions& Source2);
	short Sgn() const { return m_Sgn; }
	void Clear()
	{
		clear();
		m_Sgn=1;
	}
	short MultSgn(short Mult) {m_Sgn*=Mult; return m_Sgn;}
	void Translate(const PINS_COLOCATIONS& PinsColocations);
protected:
	short m_Sgn;
};

typedef list<_CBasicSetOfDeletions> _CMultiBasicSetOfDeletions;

typedef map<size_t, _CBasicSetOfDeletions> BASIC_SET_OF_DELETIONS;

typedef map<size_t, _CMultiBasicSetOfDeletions> MULTI_BASIC_SET_OF_DELETIONS;

extern _CBasicSetOfDeletions EmptyDeletionsSet;

extern _CMultiBasicSetOfDeletions EmptyMultiDeletionsSet;

