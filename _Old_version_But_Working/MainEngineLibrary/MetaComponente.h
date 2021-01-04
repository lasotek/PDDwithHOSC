#pragma once
#include "Components.h"
#include "IdealComponents.h"
class _CMetaComponent :
	public _CTreeForkComponent
{
public:
	virtual ~_CMetaComponent();
	//using _CComponent::_CComponent;
	_CMetaComponent(_CCircuit* pParentCircuit = nullptr);
	_CMetaComponent(_CComponent* p1stComponent, _CComponent* p2ndComponent); 
	void AddAnotherSimilarComponent(_CComponent *pComponent);

	virtual int GetNoOfPorts() const override;
	virtual bool HasParameterValue() const override { return true; }
	virtual unsigned NoOfDescendants() const override;
	virtual unsigned NoOfEffevtiveDescendants() override;
	//virtual unsigned GreedyNoOfDescendants() { return NoOfDescendants(); }
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	virtual const string& GetParameterName() const override { return m_CommonParamName; }
	virtual const NumericType& GetParameterValue() const override;
	virtual size_t MaxDescRank() override;
	virtual void PrepareInnerStructure(_CPathTraitor& Traitor) {}
	virtual long GetHash(long Core = HASH_CORE) const override;
	//virtual int GetNextParamID() const override { return -1; }
	virtual bool IsMacroDependant() const override { return false; }
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CMetaComponent& RightComp) { return IsEqualTo(RightComp); }
	bool operator!=(const _CMetaComponent& RightComp) { return !operator==(RightComp); }
	virtual unsigned short sPower() const override;
	//virtual bool Translate2RealDescendant(size_t& Index) const { return true; }
	virtual void ModifyValue(long double NewValue) override;
	//virtual void CheckNextStep(_CPredictionResult& PredictionResult, const _CGraphState& GraphState, size_t MinInternal, size_t MaxInternal);
	virtual bool IsControledSource() override { return true; }
	//virtual void CleanGarbage() {}
	//virtual int HollowOutput() override { return m_CurrPinsSet.size()-1; }
	//virtual size_t Desc2RealDesc(size_t Index) { return Index; }
	virtual const _CSubModelNumericPattern* GetMyNumPattern(_CCompRedefEntry& RedefEntry, _CPathTraitor& PathTraitor) override;
	virtual void GetSimplyFlatVertex2(const _CModelVertex* pCallerVertex,
		_CModelVerticesPath& VerticesPath,
		_CPathTraitor& PathTraitor,
		size_t SPower,
		NumericType& CurrentAllowedInaccuracy,
		const _CFlatVertex*& pResultVertex,
		short& TopSgn,
		const _CDescModelNumericPattern& DescNumValues,
		_CCompRedefEntry& RedefEntry) override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange);
	virtual bool DeletionsAreForced() const { return false; }
	virtual void GenerateMyNumPattern(const NumericType& NumericValue, _CSubModelNumericPattern& NumericPattern) override;
	const _CSubModelNumericPattern* GetMyBaseNumPattern(_CCompRedefEntry& RedefEntry);
	bool IsCompSetOfType(const _CComponent* pComp);
	virtual short TraversSPower() override 
	{
		ASSERTPDD(!m_CompList.empty());
		return m_CompList.front()->sPower(); 
	}
	virtual void WirteToTextStream(iostream& stream) override;

	//***Obsolate*************************************************************************************
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions)
	{
		RISEPDD(eIllegalOperation, "Only _CTreeForkComponents");
	}
	virtual void PlugInComponent(_CIntegrityTable& IntegrityTable)
	{
		RISEPDD(eIllegalOperation, "Funtion GetDescendantDeletions obsolete.");
	}
	virtual void GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions)
	{
		RISEPDD(eIllegalOperation, "Funtion GetDescendantDeletions obsolete.");
	}
	virtual void ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials,
		const _CDescModelNumericPattern& DescendantsNumResuls,
		const _CDescSignsVector& DescendantsSigns,
		_CCompRedefEntry& CompRedefEntry,
		const string* Context) override
	{
		RISEPDD(eIllegalOperation, "Funtion ApplyMe2Result obsolete.");
	}
	virtual size_t RealIndex(size_t Index) const
	{
		RISEPDD(eIllegalOperation, "Only for MetaVertices");
	}
	virtual void CurrentPins(_CGraphTable& Tables, TWO_GRAPH_PINS& Pins) const
	{
		RISEPDD(eIllegalOperation, "Obsolate function not for meta components");
	}
	virtual void ProcessDescendant(unsigned Desc, _CGraphState* pState) override
	{
		RISEPDD(eIllegalOperation, "Obsolate function not for meta components");
	}
	virtual void CheckDisconnection(_CGraphState* pState) const override
	{
		RISEPDD(eIllegalOperation, "Obsolate function not for meta components");
	}
	//***Obsolate*************************************************************************************
protected:
	virtual void WriteType(iostream& stream) override;
	//virtual void WritePos(iostream& stream) override {}
	//virtual void WriteValue(iostream& stgream) override {}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;

	void GenerateAllDeletins();

	void CheckPins(_CComponent* pComponent);
	void CheckDeletins(_CComponent* pComponent);
	typedef list<_CComponent*> COMP_LIST;
	COMP_LIST m_CompList;
	string m_CommonParamName;
	COMPONENT_PINS_SET m_CurrPinsSet;
	typedef vector<_CMultiBasicSetOfDeletions> DESC_DELS;
	DESC_DELS m_DescDels;
	void PrepareDescDels();
	unique_ptr<_CSubModelNumericPattern> m_pCompPattern;
	DECLARE_DYNAMIC_CREATION(_CMetaComponent);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
};


class _CMataPathologicComponent : public _CIdealComponent
{
public:
	_CMataPathologicComponent(_CCircuit* pOwner = nullptr) :_CIdealComponent(pOwner) {}
	virtual size_t MaxDescRank() override { return 1; }
	virtual void ProcessModel(_CModelVertex& ParentVertex, _CGraphState* pNewState, unsigned ReduceToNoNodes) override
	{
		RISEPDD(eIllegalOperation, "Obsolate function");
	}
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		if (m_SetDeletion.empty())
			PrepareDeletions();
		pDeletions = &m_SetDeletion;
	}
protected:
	virtual void PrepareDeletions() = 0;
};

class _CIdealFollowers : public _CMataPathologicComponent
{
public:
	_CIdealFollowers(_CCircuit* pOwner = nullptr);
	_CIdealFollowers(_CCircuit* pOwner, int OUTp, int OUTn, int INp, int INn);
	_CIdealFollowers(const _CIdealFollowers& Source);
	virtual int GetNoOfPorts() const override { return 4; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CIdealFollowers& Other) const
	{
		return m_INp == Other.m_INp && m_INn == Other.m_INn && m_OUTp == Other.m_OUTp && m_OUTn == Other.m_OUTn;
	}
protected:
	virtual void WriteType(iostream& stream) override { stream << "Generic IdealFollowers : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_INp, m_INn, m_OUTp, m_OUTn;
};

class _CValtageFollower : public _CIdealFollowers
{
public:
	//using _CIdealFollowers::_CIdealFollowers;
	_CValtageFollower(_CCircuit* pOwner = nullptr) :_CIdealFollowers(pOwner) {}
	_CValtageFollower(const _CIdealFollowers& Source) : _CIdealFollowers(Source) {}
	_CValtageFollower(_CCircuit* pOwner, int INp, int INn, int OUTp, int OUTn) :_CIdealFollowers(pOwner, INp, INn, OUTp, OUTn) 
	{
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Voltage Follower : "; }
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CValtageFollower);
};

class _CCurrentFollower : public _CIdealFollowers
{
public:
	_CCurrentFollower(_CCircuit* pOwner = nullptr) :_CIdealFollowers(pOwner) {}
	_CCurrentFollower(const _CIdealFollowers& Source) : _CIdealFollowers(Source) {}
	_CCurrentFollower(_CCircuit* pOwner, int INp, int INn, int OUTp, int OUTn) :_CIdealFollowers(pOwner, INp, INn, OUTp, OUTn) 
	{
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Current Follower : "; }
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CCurrentFollower);
};

class _CConveyors : public _CMataPathologicComponent
{
public:
	_CConveyors(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner), m_x(0), m_y(0),m_gnd(0) {}
	_CConveyors(const _CConveyors& Source) :_CMataPathologicComponent(Source.m_pBaseCircuit), m_x(Source.m_x), m_y(Source.m_y), m_gnd(Source.m_gnd),
		m_Z(Source.m_Z)
	{
	}
	_CConveyors(_CCircuit* pOwner, int X, int Y, int Z, int gnd = 0) :_CMataPathologicComponent(pOwner),m_x(X), m_y(Y), m_gnd(gnd), m_Z(1,Z) 
	{
	}
	_CConveyors(_CCircuit* pOwner, int X, int Y, vector<int> Zs, int gnd = 0) :_CMataPathologicComponent(pOwner), m_x(X), m_y(Y), m_gnd(gnd), m_Z(Zs)
	{
	}
	virtual int GetNoOfPorts() const override { return m_Z.size() + 3; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CConveyors& Other) const 
	{
		return m_x == Other.m_x && m_y == Other.m_y && m_gnd == Other.m_gnd && m_Z == Other.m_Z;
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Generic Conveyor : "; }
	virtual void WritePos(iostream& stream) override;
	//DECLARE_DYNAMIC_CREATION(_CConveyors);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_x, m_y, m_gnd;
	vector<int> m_Z;
};

class _CCCIIp : public _CConveyors
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "CCII+ : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_CCCIIp);
};

class _CCCIp : public _CCCIIp
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "CCI+ : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_CCCIp);
};

class _CCCIIIp : public _CCCIIp
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "CCIII+ : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_CCCIIIp);
};

class _CCCIIm : public _CConveyors
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "CCII- : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_CCCIIm);
};

class _CCCIm : public _CCCIIm
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "CCI- : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_CCCIm);
};

class _CCCIIIm : public _CCCIIm
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "CCIII- : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_CCCIIIm);
};

class _ICCCIIp : public _CConveyors
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "ICCII+ : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_ICCCIIp);
};

class _ICCCIp : public _ICCCIIp
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "ICCI+ : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_ICCCIp);
};

class _ICCCIIIp : public _ICCCIIp
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "ICCIII+ : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_ICCCIIIp);
};

class _ICCCIIm : public _CConveyors
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "ICCII- : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_ICCCIIm);
};

class _ICCCIm : public _ICCCIIm
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "ICCI- : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_ICCCIm);
};

class _ICCCIIIm : public _ICCCIIm
{
public:
	using _CConveyors::_CConveyors;
protected:
	virtual void WriteType(iostream& stream) override { stream << "ICCIII- : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_ICCCIIIm);
};

class _CBOCC : public _CConveyors {
public:
	_CBOCC(_CCircuit* pOwner = nullptr) :_CConveyors(pOwner) {}
	_CBOCC(const _CBOCC& Source) :_CConveyors(Source), m_Type(Source.m_Type) {}
	_CBOCC(_CCircuit* pOwner, int X, int Y, int Zp, int Zm, int Type, int gnd = 0) :
		_CConveyors(pOwner, X, Y, { Zp, Zm }, gnd),m_Type(Type) {}
protected:
	virtual void WritePos(iostream& stream) override;
	virtual void WriteType(iostream& stream) override;
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	DECLARE_DYNAMIC_CREATION(_CBOCC);
	int m_Type = CONV_MOCCII;
};

class _CMOICCGen : public _CConveyors
{
public:
	_CMOICCGen(_CCircuit* pOwner = nullptr) :_CConveyors(pOwner) {}
	_CMOICCGen(const _CConveyors& Source) :_CConveyors(Source) {}
	_CMOICCGen(_CCircuit* pOwner, int X, int Y, int Z1p, int Z1m, int Z2p, int Z2m, int gnd = 0) :
		_CConveyors(pOwner, X, Y, {Z1p,Z1m,Z2p,Z1m}, gnd) {}
protected:
	virtual void WritePos(iostream& stream) override;
	virtual void WriteType(iostream& stream) override { stream << "MOICCGen : "; }
	//virtual void PrepareDeletions() override;
	//virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	//DECLARE_DYNAMIC_CREATION(_CMOICCGen);
};

class _CMOICCI : public _CMOICCGen
{
public:
	using _CMOICCGen::_CMOICCGen;
protected:
	virtual void WriteType(iostream& stream) override { stream << "MOICCI : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_CMOICCI);
};

class _CMOICCII : public _CMOICCGen
{
public:
	using _CMOICCGen::_CMOICCGen;
protected:
	virtual void WriteType(iostream& stream) override { stream << "MOICCII : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_CMOICCII);
};

class _CMOICCIII : public _CMOICCGen
{
public:
	using _CMOICCGen::_CMOICCGen;
protected:
	virtual void WriteType(iostream& stream) override { stream << "MOICCIII : "; }
	virtual void PrepareDeletions() override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	DECLARE_DYNAMIC_CREATION(_CMOICCIII);
};

class _CFDCFOA_RM_INF : public _CMataPathologicComponent
{
public:
	_CFDCFOA_RM_INF(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner),
		m_Vyp(0), m_Vyn(0), m_Ixp(0), m_Ixn(0), m_Izp(0), m_Izn(0), m_Vwp(0), m_Vwn(0) {}
	_CFDCFOA_RM_INF(const _CFDCFOA_RM_INF& Source) :_CMataPathologicComponent(Source.m_pBaseCircuit),
		m_Vyp(Source.m_Vyp), m_Vyn(Source.m_Vyn), m_Ixp(Source.m_Ixp), m_Ixn(Source.m_Ixn), 
		m_Izp(Source.m_Izp), m_Izn(Source.m_Izn), m_Vwp(Source.m_Vwp), m_Vwn(Source.m_Vwn)
	{
	}
	_CFDCFOA_RM_INF(_CCircuit* pOwner, int Vyp, int Vyn, int Ixp, int Ixn, int Izp, int Izn, int Vwp, int Vwn) :_CMataPathologicComponent(pOwner),
		m_Vyp(Vyp), m_Vyn(Vyn), m_Ixp(Ixp), m_Ixn(Ixn), m_Izp(Izp), m_Izn(Izn), m_Vwp(Vwp), m_Vwn(Vwn)
	{
	}
	virtual int GetNoOfPorts() const override { return 8; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CFDCFOA_RM_INF& Other) const
	{
		return
			m_Vyp == Other.m_Vyp &&
			m_Vyn == Other.m_Vyn &&
			m_Ixp == Other.m_Izp &&
			m_Ixn == Other.m_Izn &&
			m_Izp == Other.m_Izp &&
			m_Izn == Other.m_Izn &&
			m_Vwp == Other.m_Vwp &&
			m_Vwn == Other.m_Vwn;
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Differential Difference Operational Mirrored Amplifier : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CFDCFOA_RM_INF);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_Vyp, m_Vyn, m_Ixp, m_Ixn, m_Izp, m_Izn, m_Vwp, m_Vwn;
};

class _CDDOMA : public _CMataPathologicComponent
{
public:
	_CDDOMA(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner),
		m_V1(0), m_V2(0), m_V3(0), m_V4(0), m_Von(0), m_Vop(0), m_gnd(0) {}
	_CDDOMA(const _CDDOMA& Source) :_CMataPathologicComponent(Source.m_pBaseCircuit),
		m_V1(Source.m_V1), m_V2(Source.m_V2), m_V3(Source.m_V3), m_V4(Source.m_V4),
		m_Von(Source.m_Von), m_Vop(Source.m_Vop), m_gnd(0)
	{
	}
	_CDDOMA(_CCircuit* pOwner, int V1, int V2, int V3, int V4, int Von, int Vop, int gnd = 0) :_CMataPathologicComponent(pOwner),
		m_V1(V1), m_V2(V2), m_V3(V3), m_V4(V4), m_Von(Von), m_Vop(Vop), m_gnd(gnd)
	{
	}
	virtual int GetNoOfPorts() const override { return 7; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CDDOMA& Other) const
	{
		return m_V1 == Other.m_V1 &&
			m_V2 == Other.m_V2 &&
			m_V3 == Other.m_V3 &&
			m_V4 == Other.m_V4 &&
			m_Von == Other.m_Von &&
			m_Vop == Other.m_Vop &&
			m_gnd == Other.m_gnd;
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Differential Difference Operational Mirrored Amplifier : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CDDOMA);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_V1, m_V2, m_V3, m_V4, m_Von, m_Vop, m_gnd;
};

class _CFBDDA : public _CMataPathologicComponent
{
public:
	_CFBDDA(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner),
		m_V1(0), m_V2(0), m_V3(0), m_V4(0), m_Ion(0), m_Iop(0), m_gnd(0) {}
	_CFBDDA(const _CFBDDA& Source) :_CMataPathologicComponent(Source.m_pBaseCircuit),
		m_V1(Source.m_V1), m_V2(Source.m_V2), m_V3(Source.m_V3), m_V4(Source.m_V4),
		m_Ion(Source.m_Ion), m_Iop(Source.m_Iop), m_gnd(0)
	{
	}
	_CFBDDA(_CCircuit* pOwner, int V1, int V2, int V3, int V4, int Ion, int Iop, int gnd = 0) :_CMataPathologicComponent(pOwner),
		m_V1(V1), m_V2(V2), m_V3(V3), m_V4(V4), m_Ion(Ion), m_Iop(Iop), m_gnd(gnd)
	{
	}
	virtual int GetNoOfPorts() const override { return 7; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CFBDDA& Other) const
	{
		return m_V1 == Other.m_V1 &&
			m_V2 == Other.m_V2 &&
			m_V3 == Other.m_V3 &&
			m_V4 == Other.m_V4 &&
			m_Ion == Other.m_Ion &&
			m_Iop == Other.m_Iop &&
			m_gnd == Other.m_gnd;
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Fully Balanced Differential Difference Amplifier : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CFBDDA);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_V1, m_V2, m_V3, m_V4, m_Ion, m_Iop, m_gnd;
};

class _CDVCFOA : public _CMataPathologicComponent
{
public://m_Vx, m_Vyp, m_Vyn, m_Iz, m_Vw, m_gnd
	_CDVCFOA(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner),
		m_Vx(0), m_Vyp(0), m_Vyn(0), m_Iz(0), m_Vw(0), m_gnd(0) {}
	_CDVCFOA(const _CDVCFOA& Source) :_CMataPathologicComponent(Source.m_pBaseCircuit),
		m_Vx(Source.m_Vx), m_Vyp(Source.m_Vyp), m_Vyn(Source.m_Vyn), m_Iz(Source.m_Iz),
		m_Vw(Source.m_Vw), m_gnd(Source.m_gnd)
	{
	}
	_CDVCFOA(_CCircuit* pOwner, int Vx, int Vyp, int Vyn, int Iz, int Vw, int gnd = 0) :_CMataPathologicComponent(pOwner),
		m_Vx(Vx), m_Vyp(Vyp), m_Vyn(Vyn), m_Iz(Iz), m_Vw(Vw), m_gnd(gnd)
	{
	}
	virtual int GetNoOfPorts() const override { return 6; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CDVCFOA& Other) const
	{
		return m_Vx == Other.m_Vx &&
			m_Vyp == Other.m_Vyp &&
			m_Vyn == Other.m_Vyn &&
			m_Iz == Other.m_Iz &&
			m_Vw == Other.m_Vw &&
			m_gnd == Other.m_gnd;
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Differential Voltage Current Feedback Operational Amplifier : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CDVCFOA);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_Vx, m_Vyp, m_Vyn, m_Iz, m_Vw, m_gnd;
};


class _CDDCCFOA : public _CMataPathologicComponent
{
public://m_Vx, m_Vy1, m_Vy2, m_Vy3, m_Iz1,m_Iz2, m_Vw1, m_Vw2, m_gnd
	_CDDCCFOA(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner),
		m_Vx(0), m_Vy1(0), m_Vy2(0), m_Vy3(0), m_Iz1(0), m_Iz2(0), m_Vw1(0), m_Vw2(0), m_gnd(0) {}
	_CDDCCFOA(const _CDDCCFOA& Source) :_CMataPathologicComponent(Source.m_pBaseCircuit),
		m_Vx(Source.m_Vx), m_Vy1(Source.m_Vy1), m_Vy2(Source.m_Vy2), m_Vy3(Source.m_Vy3), 
		m_Iz1(Source.m_Iz1), m_Iz2(Source.m_Iz2), m_Vw1(Source.m_Vw1), m_Vw2(Source.m_Vw2), m_gnd(Source.m_gnd)
	{
	}
	_CDDCCFOA(_CCircuit* pOwner, int Vx, int Vy1, int Vy2, int Vy3, int Iz1, int Iz2, int Vw1, int Vw2, int gnd=0) :_CMataPathologicComponent(pOwner),
		m_Vx(Vx), m_Vy1(Vy1), m_Vy2(Vy2), m_Vy3(Vy3), m_Iz1(Iz1), m_Iz2(Iz2), m_Vw1(Vw1), m_Vw2(Vw2), m_gnd(gnd)
	{
	}
	virtual int GetNoOfPorts() const override { return 9; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CDDCCFOA& Other) const
	{
		return m_Vx == Other.m_Vx &&
			m_Vy1 == Other.m_Vy1 &&
			m_Vy2 == Other.m_Vy2 &&
			m_Vy3 == Other.m_Vy3 &&
			m_Iz1 == Other.m_Iz1 &&
			m_Iz2 == Other.m_Iz2 &&
			m_Vw1 == Other.m_Vw1 &&
			m_Vw2 == Other.m_Vw2 &&
			m_gnd == Other.m_gnd;
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Differential Difference Complementary Current Feedback Operational Amplifier : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CDDCCFOA);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_Vx, m_Vy1, m_Vy2, m_Vy3, m_Iz1,m_Iz2, m_Vw1, m_Vw2, m_gnd;
};

class _CDDCCpm : public _CMataPathologicComponent
{
public://m_Vx, m_Vy1, m_Vy2, m_Vy3, m_Iz1,m_Iz2, m_Vw1, m_Vw2, m_gnd
	_CDDCCpm(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner),
		m_Vx(0), m_Vy1(0), m_Vy2(0), m_Vy3(0), m_Iz1(0), m_Iz2(0),  m_gnd(0) {}
	_CDDCCpm(const _CDDCCpm& Source) :_CMataPathologicComponent(Source.m_pBaseCircuit),
		m_Vx(Source.m_Vx), m_Vy1(Source.m_Vy1), m_Vy2(Source.m_Vy2), m_Vy3(Source.m_Vy3),
		m_Iz1(Source.m_Iz1), m_Iz2(Source.m_Iz2), m_gnd(Source.m_gnd)
	{
	}
	_CDDCCpm(_CCircuit* pOwner, int Vx, int Vy1, int Vy2, int Vy3, int Iz1, int Iz2, int gnd = 0) :_CMataPathologicComponent(pOwner),
		m_Vx(Vx), m_Vy1(Vy1), m_Vy2(Vy2), m_Vy3(Vy3), m_Iz1(Iz1), m_Iz2(Iz2), m_gnd(gnd)
	{
	}
	virtual int GetNoOfPorts() const override { return 7; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CDDCCpm& Other) const
	{
		return m_Vx == Other.m_Vx &&
			m_Vy1 == Other.m_Vy1 &&
			m_Vy2 == Other.m_Vy2 &&
			m_Vy3 == Other.m_Vy3 &&
			m_Iz1 == Other.m_Iz1 &&
			m_Iz2 == Other.m_Iz2 &&
			m_gnd == Other.m_gnd;
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Differential Difference Current Conveyor : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CDDCCpm);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_Vx, m_Vy1, m_Vy2, m_Vy3, m_Iz1, m_Iz2, m_gnd;
};


class _CDXCCII : public _CMataPathologicComponent
{
public:
	_CDXCCII(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner),
		m_Vx1(0), m_Vx2(0), m_Vy(0), m_Iz1(0), m_Iz2(0), m_gnd(0) {}
	_CDXCCII(const _CDXCCII& Source) :_CMataPathologicComponent(Source.m_pBaseCircuit),
		m_Vx1(Source.m_Vx1), m_Vx2(Source.m_Vx2), m_Vy(Source.m_Vy), 
		m_Iz1(Source.m_Iz1), m_Iz2(Source.m_Iz2), m_gnd(Source.m_gnd)
	{
	}
	_CDXCCII(_CCircuit* pOwner, int Vx1, int Vx2, int Vy, int Iz1, int Iz2, int gnd = 0) :_CMataPathologicComponent(pOwner),
		m_Vx1(Vx1), m_Vx2(Vx2), m_Vy(Vy), m_Iz1(Iz1), m_Iz2(Iz2), m_gnd(gnd)
	{
	}
	virtual int GetNoOfPorts() const override { return 6; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CDXCCII& Other) const
	{
		return m_Vx1 == Other.m_Vx1 &&
			m_Vx2 == Other.m_Vx2 &&
			m_Vy == Other.m_Vy &&
			m_Iz1 == Other.m_Iz1 &&
			m_Iz2 == Other.m_Iz2 &&
			m_gnd == Other.m_gnd;
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Dual-X Second Generation Current Conveyor : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CDXCCII);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_Vx1, m_Vx2, m_Vy, m_Iz1, m_Iz2, m_gnd;
};

class _CFDVDCCIIpm : public _CMataPathologicComponent
{
public: //	int m_Vx1, m_Vx2, m_Vy1, m_V_y2, m_Iz1, m_Iz2, m_gnd;
	_CFDVDCCIIpm(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner),
		m_Vx1(0), m_Vx2(0), m_Vy1(0), m_Vy2(0), m_Iz1(0), m_Iz2(0), m_gnd(0) {}
	_CFDVDCCIIpm(const _CFDVDCCIIpm& Source) :_CMataPathologicComponent(Source.m_pBaseCircuit),
		m_Vx1(Source.m_Vx1), m_Vx2(Source.m_Vx2), m_Vy1(Source.m_Vy1), m_Vy2(Source.m_Vy2),
		m_Iz1(Source.m_Iz1), m_Iz2(Source.m_Iz2), m_gnd(Source.m_gnd)
	{
	}
	_CFDVDCCIIpm(_CCircuit* pOwner, int Vx1, int Vx2, int Vy1, int Vy2, int Iz1, int Iz2, int gnd = 0) :_CMataPathologicComponent(pOwner),
		m_Vx1(Vx1), m_Vx2(Vx2), m_Vy1(Vy1), m_Vy2(Vy2), m_Iz1(Iz1), m_Iz2(Iz2), m_gnd(gnd)
	{
	}
	virtual int GetNoOfPorts() const override { return 7; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	bool operator==(const _CFDVDCCIIpm& Other) const
	{
		return m_Vx1 == Other.m_Vx1 &&
			m_Vx2 == Other.m_Vx2 &&
			m_Vy1 == Other.m_Vy1 &&
			m_Vy2 == Other.m_Vy2 &&
			m_Iz1 == Other.m_Iz1 &&
			m_Iz2 == Other.m_Iz2 &&
			m_gnd == Other.m_gnd;
	}
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Fully Differential Voltage Current Conveyor : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CFDVDCCIIpm);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_Vx1, m_Vx2, m_Vy1, m_Vy2, m_Iz1, m_Iz2, m_gnd;
};


class _CFBCCII : public _CFDVDCCIIpm
{
public: //	int m_Vx1, m_Vx2, m_Vy1, m_V_y2, m_Iz1, m_Iz2, m_gnd;
	using _CFDVDCCIIpm::_CFDVDCCIIpm;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Fully Balanced Second Generation Current Conveyor : "; }
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CFBCCII);
};

//class _CFBCCI : public _CFDVDCCIIpm
//{
//public: //	int m_Vx1, m_Vx2, m_Vy1, m_V_y2, m_Iz1, m_Iz2, m_gnd;
//	using _CFDVDCCIIpm::_CFDVDCCIIpm;
//	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
//protected:
//	virtual void WriteType(iostream& stream) override { stream << "Fully Balanced First Generation Current Conveyor : "; }
//	virtual void PrepareDeletions() override;
//	DECLARE_DYNAMIC_CREATION(_CFBCCI);
//};

class _CDVCCII : public _CMataPathologicComponent
{
public:
	_CDVCCII(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner) {}
	_CDVCCII(_CCircuit* pOwner, int x, int y1, int y2, int z1, int z2, int gnd = 0);
	_CDVCCII(const _CDVCCII& Source);
	virtual int GetNoOfPorts() const override { return 6; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Differential Voltage Current Conveyo : "; }
	virtual void WritePos(iostream& stream) override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CDVCCII);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_x, m_y1, m_y2, m_z1, m_z2, m_gnd;
};

class _CFDCCII : public _CMataPathologicComponent
{
public:
	_CFDCCII(_CCircuit* pOwner = nullptr) :_CMataPathologicComponent(pOwner) {}
	_CFDCCII(_CCircuit* pOwner, int x1, int x2, int y1, int y2, int y3, int y4, int z1, int z2, int gnd = 0);
	_CFDCCII(const _CFDCCII& Source);
	virtual int GetNoOfPorts() const override { return 9; };
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
protected:
	virtual void WriteType(iostream& stream) override { stream << "Fully-Differential Current Conveyor : "; }
	virtual void WritePos(iostream& stream) override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CFDCCII);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	int m_x1, m_x2, m_y1, m_y2, m_y3, m_y4, m_z1, m_z2, m_gnd;
};


class _CMultiPortOneParam : public _CSimplyComponent
{
public:
	_CMultiPortOneParam(_CCircuit* pOwnerCircuit = nullptr) :_CSimplyComponent(pOwnerCircuit)  {}
	_CMultiPortOneParam(_CCircuit* pOwnerCircuit, string Name, const vector<int>& Ports, long double Value = 0.0);
	virtual int GetNoOfPorts() const { return m_Ports.size(); }
	virtual long GetHash(long Core = HASH_CORE) const;
	virtual void PlugInComponent(_CIntegrityTable& IntegrityTable)
	{
		RISEPDD(eIllegalOperation, "Obsolate");
	}
	bool IsControledSource() { return true; }
	void CurrentPins(_CGraphTable& Tables, TWO_GRAPH_PINS& Pins) const
	{
		RISEPDD(eIllegalOperation, "Obsolate");
	}
	virtual void CheckDisconnection(_CGraphState* pState) const override
	{
		RISEPDD(eIllegalOperation, "Obsolate");
	}
	virtual void ProcessDescendant(unsigned Desc, _CGraphState* pState) override
	{
		RISEPDD(eIllegalOperation, "Obsolate");
	}
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override
	{
		Pins.assign(m_Ports.begin(), m_Ports.end());
	}
	bool operator==(const _CMultiPortOneParam& Other)
	{
		return m_Name == Other.m_Name && m_Value == Other.m_Value && m_Ports==Other.m_Ports;
	}
	virtual bool DeletionsAreForced()  override 
	{ 
		if (m_1Deletions.empty() || m_0Deletions.empty())
			PrepareDeletions();
		return m_1Deletions.front().size()>0 && m_0Deletions.front().size()>0; 
	}
protected:
	virtual void PrepareDeletions() = 0;
	//virtual void WritePos(iostream& stream) override;
	//virtual void WriteType(iostream& stream) override;
	//bool IsEqualIfSameType(const _CComponent& RightComp);
	//virtual size_t MaxDescRank() override { return 2; }
	virtual void Load(_binary_filer& Filer) override;
	virtual void Store(_binary_filer& Filer) override;
	vector<int> m_Ports;
	_CMultiBasicSetOfDeletions m_1Deletions;
	_CMultiBasicSetOfDeletions m_0Deletions;
	//DECLARE_DYNAMIC_CREATION(_CMultiPortOneParam);
};

class _CCustomOneParameterIdealizedComponent : public _CMultiPortOneParam {
public:
	typedef pair < string, string > COF_DELETION;
	typedef pair< COF_DELETION, COF_DELETION> COF_DELETION_PAIR;
	typedef vector<COF_DELETION_PAIR> DELETIONS;
	_CCustomOneParameterIdealizedComponent(_CCircuit* pOwner = nullptr);
	_CCustomOneParameterIdealizedComponent(_CCircuit* pOwner, string parName, const DELETIONS& multDels, const DELETIONS& remDels, long double Value = 0.0);
	_CCustomOneParameterIdealizedComponent(const _CCustomOneParameterIdealizedComponent& Source);
	virtual int GetNoOfPorts() const override;
	virtual long GetHash(long Core = HASH_CORE) const override;
	virtual void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) override;
	virtual void RaportConnections(COMPONENT_PINS& Pins) const override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	virtual size_t MaxDescRank() override { return 4; }
	virtual unsigned short sPower() const override { return 0; }
protected:
	virtual void WriteType(iostream& stream) override { stream << "Generic Custom_One_Parameter_Idealized_Component : "; }
	virtual void WritePos(iostream& stream) override;
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	virtual void PrepareDeletions() override;
	DECLARE_DYNAMIC_CREATION(_CCustomOneParameterIdealizedComponent);
};

class _CFDCFOA_RM : public _CMultiPortOneParam
{
public:
	_CFDCFOA_RM(_CCircuit* pOwnerCircuit = nullptr) :_CMultiPortOneParam(pOwnerCircuit)  {}
	_CFDCFOA_RM(_CCircuit* pOwnerCircuit, string Name, int Vyp, int Vyn, int Ixp, int Ixn, int Izp, int Izn, int Vwp, int Vwn, long double Value = 0.0);
	virtual unsigned short sPower() const override { return 0; }
protected:
	virtual void PrepareDeletions() override;
	virtual void WritePos(iostream& stream) override;
	virtual void WriteType(iostream& stream) override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	virtual size_t MaxDescRank() override { return 4; }
	inline int& Vyp() { return m_Ports[0]; }
	inline int& Vyn() { return m_Ports[1]; }
	inline int& Ixp() { return m_Ports[2]; }
	inline int& Ixn() { return m_Ports[3]; }
	inline int& Izp() { return m_Ports[4]; }
	inline int& Izn() { return m_Ports[5]; }
	inline int& Vwp() { return m_Ports[6]; }
	inline int& Vwn() { return m_Ports[7]; }
	DECLARE_DYNAMIC_CREATION(_CFDCFOA_RM);
};

class _CFDOTRA : public _CMultiPortOneParam
{
public:
	_CFDOTRA(_CCircuit* pOwnerCircuit = nullptr) :_CMultiPortOneParam(pOwnerCircuit)  {}
	_CFDOTRA(_CCircuit* pOwnerCircuit, string Name, int Ip, int In, int Vp, int Vn, int gnd = 0, long double Value = 0.0);
	virtual unsigned short sPower() const override { return 0; }
protected:
	virtual void PrepareDeletions() override;
	virtual void WritePos(iostream& stream) override;
	virtual void WriteType(iostream& stream) override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	virtual size_t MaxDescRank() override { return 4; }
	inline int& gnd() { return m_Ports[0]; }
	inline int& Ip() { return m_Ports[1]; }
	inline int& In() { return m_Ports[2]; }
	inline int& Vp() { return m_Ports[3]; }
	inline int& Vn() { return m_Ports[4]; }
	DECLARE_DYNAMIC_CREATION(_CFDOTRA);
};

class _COTRA : public _CMultiPortOneParam
{
public:
	_COTRA(_CCircuit* pOwnerCircuit = nullptr) :_CMultiPortOneParam(pOwnerCircuit)  {}
	_COTRA(_CCircuit* pOwnerCircuit, string Name, int Ip, int In, int Vp, int gnd = 0, long double Value = 0.0);
	virtual unsigned short sPower() const override { return 0; }
	virtual void GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions) override
	{
		GetDescendantDeletions(ImportanceLevel, pDeletions);
	}
protected:
	virtual void PrepareDeletions() override;
	virtual void WritePos(iostream& stream) override;
	virtual void WriteType(iostream& stream) override;
	virtual bool IsEqualIfSameType(const _CComponent& RightComp) override;
	virtual size_t MaxDescRank() override { return 3; }
	inline int& gnd() { return m_Ports[0]; }
	inline int& Ip() { return m_Ports[1]; }
	inline int& In() { return m_Ports[2]; }
	inline int& Vp() { return m_Ports[3]; }
	DECLARE_DYNAMIC_CREATION(_COTRA);
};