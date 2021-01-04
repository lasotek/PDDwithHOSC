#pragma once
#include "Components.h"

class _CControledSource : public _CSimplyComponent
{
public:
	_CControledSource(_CCircuit* pOwnerCircuit);
	_CControledSource(_CCircuit* pOwnerCircuit,string Name,int p, int r, int k, int l, long double Value,int sPower=0);
	virtual ~_CControledSource(void);
	virtual int GetNoOfPorts() const {return 4;}
	virtual long GetHash(long Core=HASH_CORE) const;
	virtual void PlugInComponent(_CIntegrityTable& IntegrityTable);
	//virtual void PrepareHState(_CGraphState& HState, int ParamId) const;
	//virtual void PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState) const;
	unsigned short sPower() const {return m_sPower;}
	bool IsControledSource() {return true;}
	void CurrentPins(_CGraphTable& Tables,TWO_GRAPH_PINS& Pins) const;
	virtual void CheckDisconnection(_CGraphState* pState) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	//virtual void GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions) override;
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
protected:
	unsigned GetReal(unsigned DescNo);
	//void Add1Deletions() 
	//{
	//	//m_1Deletions.push_back(_CBasicPairOfDeletion(m_p, m_r, m_k, m_l));
	//	m_1Deletions.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_p, m_r, m_k, m_l)));
	//}
	void WritePos(iostream& stream);
	bool IsEqualIfSameType(const _CComponent& RightComp);
	void Load(_binary_filer& Filer);
	void Store(_binary_filer& Filer);
	int m_p,m_r,m_k,m_l;
	unsigned short m_sPower;
	bool m_IsInverted;
	_CMultiBasicSetOfDeletions m_1Deletions;
	_CMultiBasicSetOfDeletions m_0Deletions;
	//_CBasicSetOfDeletions m_1Deletions;
	//_CBasicSetOfDeletions m_0Deletions;
};

class _CTransconductance : public _CControledSource
{
public:
	~_CTransconductance()
	{
	}
	_CTransconductance(_CCircuit* pOwnerCircuit=NULL):_CControledSource(pOwnerCircuit) {}
	_CTransconductance(_CCircuit* pOwnerCircuit,string Name,int p, int r, int k, int l, long double Value,int sPower=0):
		_CControledSource(pOwnerCircuit,Name,p,r,k,l,Value,sPower) {}
	//virtual void PrepareLState(_CGraphState& LState, int ParamId) const;
	//virtual void PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const;
	virtual void ProcessDescendant(unsigned Desc, _CGraphState* pState) override;
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions) override;
	virtual size_t MaxDescRank() override { return 1; }
protected:
	void WriteType(iostream& stream) {stream<<"Transconductance: ";}
	DECLARE_DYNAMIC_CREATION(_CTransconductance);
};

class _CTransimpedance : public _CControledSource
{
public:
	~_CTransimpedance() 
	{
		Add0Deletions();
	}
	_CTransimpedance(_CCircuit* pOwnerCircuit=NULL):_CControledSource(pOwnerCircuit) 
	{
		Add0Deletions();
	}
	_CTransimpedance(_CCircuit* pOwnerCircuit,string Name,int p, int r, int k, int l, long double Value,int sPower=0):
		_CControledSource(pOwnerCircuit,Name,r,p,k,l,Value,sPower) 
	{
		Add0Deletions();
	}
	//virtual void PrepareLState(_CGraphState& LState, int ParamId) const;
	//virtual void PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const;
	virtual void ProcessDescendant(unsigned Desc, _CGraphState* pState) override;
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions) override;
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	virtual size_t MaxDescRank() override { return 2; }
	virtual bool DeletionsAreForced()  override {return true;}
protected:
	void Add0Deletions() 
	{
		//m_0Deletions.push_back(_CBasicSetOfDeletions());
		auto& Set = m_0Deletions.back();
		Set.push_back(_CBasicPairOfDeletion(m_p, m_r, m_p, m_r));
		Set.push_back(_CBasicPairOfDeletion(m_k, m_l, m_k, m_l));
	}
	void WriteType(iostream& stream) {stream<<"Transimpedance: ";}
	DECLARE_DYNAMIC_CREATION(_CTransimpedance);
};

class _CVoltageGain : public _CControledSource
{
public:
	~_CVoltageGain()
	{
	}
	_CVoltageGain(_CCircuit* pOwnerCircuit=NULL):_CControledSource(pOwnerCircuit) 
	{
		Add0Deletions();
	}
	_CVoltageGain(_CCircuit* pOwnerCircuit,string Name,int r, int p, int k, int l, long double Value,int sPower=0):
		_CControledSource(pOwnerCircuit,Name,p,r,k,l,Value,sPower) 
	{
		Add0Deletions();
	}
	//virtual void PrepareLState(_CGraphState& LState, int ParamId) const;
	//virtual void PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const;
	virtual void ProcessDescendant(unsigned Desc, _CGraphState* pState) override;
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions) override;
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	virtual size_t MaxDescRank() override { return 1; }
	virtual bool DeletionsAreForced()  override {return true;}
protected:
	void Add0Deletions() 
	{
		auto& Set = m_0Deletions.back();
		Set.push_back(_CBasicPairOfDeletion(m_p, m_r, m_p, m_r));
		//m_0Deletions.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_p, m_r, m_p, m_r)));
	}
	void WriteType(iostream& stream) {stream<<"Voltage gain: ";}
	DECLARE_DYNAMIC_CREATION(_CVoltageGain);
};

class _CCurrentGain : public _CControledSource
{
public:
	~_CCurrentGain()
	{
	}
	_CCurrentGain(_CCircuit* pOwnerCircuit=NULL):_CControledSource(pOwnerCircuit) 
	{
		Add0Deletions();
	}
	_CCurrentGain(_CCircuit* pOwnerCircuit,string Name,int p, int r, int k, int l, long double Value,int sPower=0):
		_CControledSource(pOwnerCircuit,Name,p,r,k,l,Value,sPower) 
	{
		Add0Deletions();
	}

	//virtual void PrepareLState(_CGraphState& LState, int ParamId) const;
	//virtual void PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const;
	virtual void ProcessDescendant(unsigned Desc, _CGraphState* pState) override;
	//virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions) override;
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	virtual size_t MaxDescRank() override { return 1; }
	virtual bool DeletionsAreForced()  override {return true;}
protected:
	void Add0Deletions() 
	{
		auto& Set = m_0Deletions.back();
		Set.push_back(_CBasicPairOfDeletion(m_k, m_l, m_k, m_l));
		//m_0Deletions.push_back(_CBasicPairOfDeletion(m_k, m_l, m_k, m_l));
	}
	void WriteType(iostream& stream) {stream<<"Current gain: ";}
	DECLARE_DYNAMIC_CREATION(_CCurrentGain);
};




