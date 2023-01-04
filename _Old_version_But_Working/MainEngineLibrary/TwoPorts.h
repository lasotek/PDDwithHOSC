#pragma once
#include "Components.h"

//class _CModelVertex;

class _CTwoPort : public _CSimplyComponent
{
public:
	_CTwoPort(_CCircuit* pCircuit):_CSimplyComponent(pCircuit), m_NodeA(0),m_NodeB(0) 
	{
		PrepareSet();
	}
	_CTwoPort(_CCircuit* pCircuit, 
		const string& Name, 
		int NodeA, 
		int NodeB, 
		long double Value):_CSimplyComponent(pCircuit,Name,Value),
		m_NodeA(NodeA),m_NodeB(NodeB) 
	{
		PrepareSet();
	}
	virtual ~_CTwoPort(void);
	virtual size_t GetHash(size_t Core=HASH_CORE) const override;
	/*virtual*/ void PlugInComponent(_CIntegrityTable& IntegrityTable);
	///*virtual*/ void PrepareLState(_CGraphState& LState, int ParamId) const;
	///*virtual*/ void PrepareHState(_CGraphState& HState, int ParamId) const;
	///*virtual*/ void PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const;
	unsigned short sPower() const {return 0;}
	void CurrentPins(_CGraphTable& Tables,TWO_GRAPH_PINS& Pins) const;
	virtual void ProcessDescendant(unsigned Desc, _CGraphState* pState) override;
	virtual void CheckDisconnection(_CGraphState* pState) const override;
	virtual unsigned MaxDescRank() override {return 1;}
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
protected:
	void PrepareSet()
	{
		m_SetOfDeletions.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_NodeA, m_NodeB, m_NodeA, m_NodeB)));
	}
	void WritePos(iostream& stream);
	void Load(_binary_filer& Filer);
	void Store(_binary_filer& Filer);
	bool IsEqualIfSameType(const _CComponent& RightComp);
	virtual bool IsAdmittance() const=0;
	int m_NodeA;
	int m_NodeB;
	//_CBasicSetOfDeletions m_SetOfDeletions;
	_CMultiBasicSetOfDeletions m_SetOfDeletions;
};

class _CResistance : public _CTwoPort
{
public:
	~_CResistance()
	{
	}
	_CResistance(_CCircuit* pCircuit=NULL):_CTwoPort(pCircuit) {}
	_CResistance(_CCircuit* pCircuit, 
		const string& Name, 
		int NodeA, 
		int NodeB, 
		long double Value):_CTwoPort(pCircuit,Name,NodeA,NodeB,Value) {}
	//virtual void GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions) override
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		if(Descendant==0)
			pDeletions=&m_SetOfDeletions;
		else
			pDeletions=&EmptyMultiDeletionsSet;
	}
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions) override
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		if(ImportanceLevel==0)
			pDeletions=&m_SetOfDeletions;
		else
			pDeletions = &EmptyMultiDeletionsSet;
	}
protected:
	void WriteType(iostream& stream) {stream<<"Resistance: ";}
	DECLARE_DYNAMIC_CREATION(_CResistance);
	virtual bool IsAdmittance() const {return false;}
};

class _CConductance : public _CTwoPort
{
public:
	~_CConductance()
	{
	}
	_CConductance(_CCircuit* pCircuit=NULL):_CTwoPort(pCircuit) {}
	_CConductance(_CCircuit* pCircuit, 
		const string& Name, 
		int NodeA, 
		int NodeB, 
		long double Value):_CTwoPort(pCircuit,Name,NodeA,NodeB,Value) {}
	//virtual void GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions) override
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		if(Descendant==1)
			pDeletions=&m_SetOfDeletions;
		else
			pDeletions=&EmptyMultiDeletionsSet;
	}
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions) override
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		if(ImportanceLevel==0)
			pDeletions=&m_SetOfDeletions;
		else
			pDeletions = &EmptyMultiDeletionsSet;
	}
protected:
	void WriteType(iostream& stream) {stream<<"Conductance: ";}
	DECLARE_DYNAMIC_CREATION(_CConductance);
	virtual bool IsAdmittance() const {return true;}
};

class _CCapacitance : public _CTwoPort
{
public:
	~_CCapacitance()
	{
	}
	_CCapacitance(_CCircuit* pCircuit=NULL):_CTwoPort(pCircuit) {}
	_CCapacitance(_CCircuit* pCircuit, 
		const string& Name, 
		int NodeA, 
		int NodeB, 
		long double Value):_CTwoPort(pCircuit,Name,NodeA,NodeB,Value) {}
	unsigned short sPower() const {return 1;}
	//virtual void GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions) override
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		if(Descendant==1)
			pDeletions=&m_SetOfDeletions;
		else
			pDeletions=&EmptyMultiDeletionsSet;
	}
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions) override
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		if(ImportanceLevel==0)
			pDeletions=&m_SetOfDeletions;
		else
			pDeletions = &EmptyMultiDeletionsSet;
	}
protected:
	void WriteType(iostream& stream) {stream<<"Capacitance: ";}
	DECLARE_DYNAMIC_CREATION(_CCapacitance);
	virtual bool IsAdmittance() const {return true;}
};

class _CInductance : public _CTwoPort
{
public:
	~_CInductance() 
	{
	}
	_CInductance(_CCircuit* pCircuit=NULL):_CTwoPort(pCircuit) {}
	_CInductance(_CCircuit* pCircuit, 
		const string& Name, 
		int NodeA, 
		int NodeB, 
		long double Value):_CTwoPort(pCircuit,Name,NodeA,NodeB,Value) {}
	unsigned short sPower() const {return 1;}
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		if (Descendant == 0)
			pDeletions = &m_SetOfDeletions;
		else
			pDeletions = &EmptyMultiDeletionsSet;
	}
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		if (ImportanceLevel == 0)
			pDeletions = &m_SetOfDeletions;
		else
			pDeletions = &EmptyMultiDeletionsSet;
	}
	//virtual void GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions) override
	//{
	//	if (Descendant == 1)
	//		pDeletions = &m_SetOfDeletions;
	//	else
	//		pDeletions = &EmptyDeletionsSet;
	//}
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions) override
	//{
	//	if (ImportanceLevel == 0)
	//		pDeletions = &m_SetOfDeletions;
	//	else
	//		pDeletions = &EmptyDeletionsSet;
	//}
protected:
	void WriteType(iostream& stream) {stream<<"Inductance: ";}
	DECLARE_DYNAMIC_CREATION(_CInductance);
	virtual bool IsAdmittance() const {return false;}
};
