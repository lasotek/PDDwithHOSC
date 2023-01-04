#pragma once
#include "Components.h"
#include "..\PDDErrorsAndConst.h"

class _CIdealComponent : public _CComponent
{
public:
	_CIdealComponent(_CCircuit* pOwner);
	virtual ~_CIdealComponent(void);
	virtual int GetNoOfParametes() const {return 0;}
	virtual const string& GetParameterName() const {return EmptyString;}
	virtual const NumericType& GetParameterValue() const {return EmptyFloat;}
	//virtual long GetHash() {return 0;}
	//virtual void PrepareHState(_CGraphState& HState, int ParamId) const {}
	virtual void PlugInComponent(_CIntegrityTable& IntegrityTable) {}
	//virtual void PredictLHStates(_CGraphState& AscState, _CComponentPredicator& PredictedState,int ParamId) const {}
	unsigned short sPower() const {return 0;}
	void CurrentPins(_CGraphTable& Tables,TWO_GRAPH_PINS& Pins) const {}
	//virtual void GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions) override
	//{
	//	pDeletions = &m_SetDeletion;
	//}
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		pDeletions = &m_SetDeletion;
	}
	virtual bool DeletionsAreForced() override { return true; }
protected:
	//_CBasicSetOfDeletions m_SetDeletion;
	_CMultiBasicSetOfDeletions m_SetDeletion;
};

class _CNullor : public _CIdealComponent
{
public:
	~_CNullor()
	{
	}
	_CNullor(_CCircuit* pOwner=NULL):_CIdealComponent(pOwner) 
	{
		AddDeletions();
	}
	_CNullor(_CCircuit* pOwner,int p, int r, int k, int l);
//	virtual void PlugInComponent(_CIntegrityTable& IntegrityTable);
//	virtual void PrepareLState(_CGraphState& LState, int ParamId) const;
	virtual unsigned GetNoOfPorts() const override {return 4;};
	virtual size_t GetHash(size_t Core=HASH_CORE) const override;
	virtual unsigned MaxDescRank() override {return 1;}
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
protected:
	void AddDeletions()
	{
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_p, m_r, m_k, m_l)));
	}
	virtual void WriteType(iostream& stream) override {stream<<"Nullor: ";}
	virtual void WritePos(iostream& stream) override;
	DECLARE_DYNAMIC_CREATION(_CNullor);
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	void Load(_binary_filer& Filer);
	void Store(_binary_filer& Filer);
	virtual void ProcessModel(_CModelVertex& ParentVertex, _CGraphState* pNewState,unsigned ReduceToNoNodes) override;
	int m_p,m_r,m_k,m_l;
};

class _COpAmp : public _CNullor
{
public:
	~_COpAmp()
	{
	}
	_COpAmp(_CCircuit* pOwner=NULL):_CNullor(pOwner) {}
	_COpAmp(_CCircuit* pOwner, int II, int NI, int Out);
	virtual unsigned GetNoOfPorts() const {return 3;}
protected:
	void WriteType(iostream& stream) {stream<<"Ideal op-amp: ";}
	void WritePos(iostream& stream);
	DECLARE_DYNAMIC_CREATION(_COpAmp);
};

class _CCurrentConveyor : public _CIdealComponent
{
public:
	~_CCurrentConveyor()
	{
	}
	_CCurrentConveyor(_CCircuit* pOwner=NULL):_CIdealComponent(pOwner) 
	{
		AddDeletions();
	}
	_CCurrentConveyor(_CCircuit* pOwner,int X, int Y, int Z, short Type):
	_CIdealComponent(pOwner),m_X(X),m_Y(Y),m_Z(Z),m_Type(Type) 
	{
		AddDeletions();
	}
//	virtual void PrepareLState(_CGraphState& LState, int ParamId) const;
	virtual unsigned GetNoOfPorts() const override{return 3;}
	virtual size_t GetHash(size_t Core=HASH_CORE) const override;
	virtual unsigned MaxDescRank() override {return 1;}
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
protected:
	void AddDeletions();
	static char* m_TypeStr[]; 
	void WriteType(iostream& stream) {stream<<"Ideal "<<m_TypeStr[m_Type]<<": ";}
	void WritePos(iostream& stream);
	DECLARE_DYNAMIC_CREATION(_CCurrentConveyor);
	bool IsEqualIfSameType(const _CComponent& RightComp);
	void Load(_binary_filer& Filer);
	void Store(_binary_filer& Filer);
	void ProcessModel(_CModelVertex& ParentVertex, _CGraphState* pNewState,unsigned ReduceToNoNodes);
	int m_X,m_Y,m_Z;
	short m_Type;
};
