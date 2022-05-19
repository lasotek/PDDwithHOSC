#include "stdafx.h"
#include "MetaComponente.h"
#include "NewCore.h"
#include "PathTraitor.h"
#include "FlatVertex.h"
#include "MainCircuit.h"
#include "binary_filer.h"

IMPLEMENT_DYNAMIC_CREATION(_CMetaComponent);

_CMetaComponent::_CMetaComponent(_CCircuit* pParentCircuit) :_CTreeForkComponent(pParentCircuit), 
m_DescDels(1, _CMultiBasicSetOfDeletions(1,_CBasicSetOfDeletions()))
{
	//PrepareDescDels();
}


void _CMetaComponent::PrepareDescDels()
{
	m_DescDels.resize(1);
	m_DescDels[0].push_back(_CBasicSetOfDeletions());
}

_CMetaComponent::_CMetaComponent(_CComponent* p1stComponent, _CComponent* p2ndComponent) : _CTreeForkComponent(p1stComponent->GetOwnerCircuit()),
m_CommonParamName(p1stComponent->GetParameterName()), m_DescDels(1, _CMultiBasicSetOfDeletions(1, _CBasicSetOfDeletions()))
{
	ASSERTPDD(p1stComponent->GetParameterName() == p2ndComponent->GetParameterName());
	ASSERTPDD(typeid(p1stComponent) == typeid(p2ndComponent));
	m_CompList.push_back(p1stComponent);
	CheckPins(p1stComponent);
	//CheckDeletins(p1stComponent);
	m_CompList.push_back(p2ndComponent);
	CheckPins(p2ndComponent);
	//CheckDeletins(p2ndComponent);
}

void _CMetaComponent::AddAnotherSimilarComponent(_CComponent *pComponent)
{
	if (!m_CompList.empty())
	{
		ASSERTPDD(pComponent->GetParameterName() == m_CommonParamName);
		if (!m_CompList.empty() && typeid(pComponent) != typeid(m_CompList.front()))
		{
			string ErrInfo(pComponent->GetParameterName() + ": each components with the common name should be the same type");
			RISEPDD(eWrongComponentType, ErrInfo.c_str());
		}
	}
	m_CompList.push_back(pComponent);
	CheckPins(pComponent);
	//CheckDeletins(pComponent);
}


_CMetaComponent::~_CMetaComponent()
{
	while (!m_CompList.empty())
	{
		auto pComp = m_CompList.back();
		m_CompList.pop_back();
		delete pComp;
	}
}

void _CMetaComponent::CheckPins(_CComponent* pComponent)
{
	COMPONENT_PINS Pins;
	pComponent->RaportConnections(Pins);
	m_CurrPinsSet.insert(Pins.begin(), Pins.end());
}



void _CMetaComponent::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.assign(m_CurrPinsSet.begin(), m_CurrPinsSet.end());
}


void _CMetaComponent::CheckDeletins(_CComponent* pComponent)
{
	size_t NoOfDescs = pComponent->NoOfDescendants();
	DESC_DELS AuxDescDels;
	AuxDescDels.resize(m_DescDels.size() + NoOfDescs - 1);
	//int HollowDesc = pComponent->HollowOutput();
	for (size_t i = 0; i < NoOfDescs; i++)
	{
		const _CMultiBasicSetOfDeletions* pMultiSet = NULL;
		pComponent->GetDescendantDeletions(i, pMultiSet);
		for (const _CBasicSetOfDeletions& BasicSet : *pMultiSet)
		{
			for (size_t j = 0; j < m_DescDels.size();j++)
				for (_CBasicSetOfDeletions& PrevSet : m_DescDels[j])
					{
						_CBasicSetOfDeletions NewDeletions(PrevSet,BasicSet);
						if (NewDeletions.Sgn() != 0)
							AuxDescDels[i + j].push_back(NewDeletions);
					}
		}
	}
	for (auto& MultSetOfDel : AuxDescDels)
		if (MultSetOfDel.size()>1)
		{
			_ComplexDeletionCompressor Compressor(MultSetOfDel);
			Compressor.Simplify();
			Compressor.TranslateBack(MultSetOfDel);
		}
	m_DescDels = AuxDescDels;
}

void _CMetaComponent::GenerateAllDeletins()
{
	ASSERTPDD(m_CompList.size() > 1);
	auto cit = m_CompList.begin(), _ce = m_CompList.end();
	m_DescDels.resize(m_CompList.size() + 1);
	_CComponent* pBaseComp = *cit++;
	ASSERTPDD(pBaseComp->NoOfDescendants() == 2);
	const _CMultiBasicSetOfDeletions* pBaseDels=nullptr;
	pBaseComp->GetDescendantDeletions(0, pBaseDels);
	m_DescDels[0] = *pBaseDels;
	pBaseComp->GetDescendantDeletions(1, pBaseDels);
	m_DescDels[1] = *pBaseDels;
	size_t RealMaxSize = 2;
	for (; cit != _ce; cit++)
	{
		_CComponent* pComponent = *cit;
		DESC_DELS AuxDescDels(m_CompList.size() + 1);
		for (size_t i = 0; i < 2; i++)
		{
			const _CMultiBasicSetOfDeletions* pMultiSet = NULL;
			pComponent->GetDescendantDeletions(i, pMultiSet);
			for (const _CBasicSetOfDeletions& BasicSet : *pMultiSet)
			{
				for (size_t j = 0; j < m_DescDels.size(); j++)
					for (_CBasicSetOfDeletions& PrevSet : m_DescDels[j])
					{
						_CBasicSetOfDeletions NewDeletions(PrevSet, BasicSet);
						if (NewDeletions.Sgn() != 0)
						{
							AuxDescDels[i + j].push_back(NewDeletions);
							RealMaxSize = max(RealMaxSize, i + j);
						}
					}
			}
		}
		AuxDescDels.resize(RealMaxSize+1);
		m_DescDels = AuxDescDels;
	}
	for (auto& MultSetOfDel : m_DescDels)
		if (MultSetOfDel.size()>1)
		{
			_ComplexDeletionCompressor Compressor(MultSetOfDel);
			if(Compressor.Simplify())
				Compressor.TranslateBack(MultSetOfDel);
		}
}

void _CMetaComponent::GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	if (m_DescDels.empty())
		GenerateAllDeletins();
	if (Descendant >= m_DescDels.size())
		RISEPDD(out_of_range, "Too big No of Descendant");
	pDeletions = &m_DescDels[Descendant];
}

int _CMetaComponent::GetNoOfPorts() const
{ 
	return m_CurrPinsSet.size();
}

unsigned _CMetaComponent::NoOfDescendants() const
{ 
	ASSERTPDD(!m_CompList.empty());
	if (m_DescDels.size()<=1)
		return m_CompList.size() + 1;
	else
		return m_DescDels.size();
}

unsigned _CMetaComponent::NoOfEffevtiveDescendants()
{
	if (m_DescDels.size()<=1)
		GenerateAllDeletins();
	return m_DescDels.size();
}

const NumericType& _CMetaComponent::GetParameterValue() const
{
	ASSERTPDD(!m_CompList.empty());
	return m_CompList.back()->GetParameterValue();
}

size_t _CMetaComponent::MaxDescRank()
{
	size_t Rank = 0;
	for (auto& MultiSet : m_DescDels)
	{
		ASSERTPDD(!MultiSet.empty());
		auto& BaseSetOfDels = MultiSet.front();
		Rank = max(Rank, BaseSetOfDels.size());
	}
	return Rank;
}

long _CMetaComponent::GetHash(long Core) const
{
	for (auto& pBComp : m_CompList)
		Core = pBComp->GetHash(Core);
	return Core;
}

unsigned short _CMetaComponent::sPower() const
{
	ASSERTPDD(!m_CompList.empty());
	return (unsigned short)m_CompList.size()*m_CompList.front()->sPower();
}

void _CMetaComponent::ModifyValue(long double NewValue)
{
	for (auto& pBaseComp : m_CompList)
		pBaseComp->ModifyValue(NewValue);
}

const _CSubModelNumericPattern* _CMetaComponent::GetMyNumPattern(_CCompRedefEntry& RedefEntry, _CPathTraitor& PathTraitor)
{
	_CPathTraitor::COMP_RESULT CompRes = PathTraitor.Compare(this);
	short& Comp = CompRes.first;
	short& Oper = CompRes.second;
	if (Comp<0)
		return GetMyBaseNumPattern(RedefEntry[this]);
	else
		if (Comp == 0)
		{
			if (Oper == _CPathTraitor::DIVIDER)
				return &HDescFilterPattern;
			if (Oper == _CPathTraitor::REMINDER)
				return &LDescFilterPattern;
		}
		else
		{
			if (Oper == _CPathTraitor::DIVIDER)
				return &ZeroPattern;
			if (Oper == _CPathTraitor::REMINDER)
				return GetMyBaseNumPattern(RedefEntry[this]);
		}
	RISEPDD(eIllegalOperation, "Oops! I shouldn't be here");
}

void _CMetaComponent::GenerateMyNumPattern(const NumericType& NumericValue, _CSubModelNumericPattern& NumericPattern)
{
	NumericType CurrNumValue(1.0);
	unsigned short Power = m_CompList.front()->sPower(), CurrPower = 0;
	for (size_t i = 0; i < m_DescDels.size(); i++, CurrPower += Power, CurrNumValue *= NumericValue)
	{
		_CSubModelNumericPattern::element_poly HOrderDesc;
		HOrderDesc.put(CurrPower, CurrNumValue);
		NumericPattern.put(i, HOrderDesc);
	}
}

const _CSubModelNumericPattern* _CMetaComponent::GetMyBaseNumPattern(_CCompRedefEntry& RedefEntry)
{
	if (RedefEntry.IsNumeric())
		return RedefEntry.GetNumericPattern();
	if (m_pCompPattern == nullptr)
	{
		m_pCompPattern = make_unique<_CSubModelNumericPattern>();
		GenerateMyNumPattern(GetParameterValue(), *m_pCompPattern);
	}
	return m_pCompPattern.get();
}

void _CMetaComponent::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	for (auto& pSimpComp : m_CompList)
		pSimpComp->ExchangeNumbers(Numbers2Exchange);
	COMPONENT_PINS_SET AuxSet(m_CurrPinsSet);
	m_CurrPinsSet.clear();
	for (int Pin : m_CurrPinsSet)
	{
		Numbers2Exchange.ExchangeIfNeede(Pin);
		m_CurrPinsSet.insert(Pin);
	}
	m_pCompPattern.release();
}


void _CMetaComponent::GetSimplyFlatVertex2(const _CModelVertex* pCallerVertex,
	_CModelVerticesPath& VerticesPath,
	_CPathTraitor& PathTraitor,
	size_t Power,
	NumericType& CurrentAllowedInaccuracy,
	const _CFlatVertex*& pResultVertex,
	short& TopSgn,
	const _CDescModelNumericPattern& DescNumValues,
	_CCompRedefEntry& RedefEntry)
{
	RISEPDD(eIllegalOperation, "Obsolate function");
}

void _CMetaComponent::WriteType(iostream& stream)
{
	stream << "Meta set of ";
	m_CompList.front()->WriteType(stream);
	stream << " : ";
}

void _CMetaComponent::WirteToTextStream(iostream& stream)
{
	stream << "Meta set of ";
	m_CompList.front()->WriteType(stream);
	stream << " : ";
	bool first = true;
	for (auto pComp : m_CompList)
	{
		if (first)
			first = false;
		else
			stream << " || ";
		stream << pComp->GetParameterName();
		pComp->WritePos(stream);
	}
	m_CompList.back()->WriteValue(stream);
	stream << "\n";
}

bool _CMetaComponent::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CMetaComponent* pRC = dynamic_cast<const _CMetaComponent*>(&RightComp);
	if (pRC == nullptr)
		return false;
	if(m_CommonParamName != pRC->m_CommonParamName) 
		return false;
	if (typeid(m_CompList.front()) != typeid(pRC->m_CompList.front()))
		return false;
	return m_CompList == pRC->m_CompList;

}

bool _CMetaComponent::IsCompSetOfType(const _CComponent* pComp)
{
	ASSERTPDD(!m_CompList.empty());
	return m_CompList.front()->IsTheSameCompType(pComp);
}

void _CMetaComponent::Store(_binary_filer& Filer)
{
	Filer << m_CurrPinsSet.size();
	for (unsigned Pin : m_CurrPinsSet)
		Filer << Pin;
	Filer << m_CompList.size();
	for (_CComponent* pComp : m_CompList)
		Filer << *pComp;
	_streamable::Store(Filer);
}

void _CMetaComponent::Load(_binary_filer& Filer)
{
	m_DescDels.clear();
	m_CurrPinsSet.clear();
	m_CompList.clear();

	size_t Size;
	Filer >> Size;
	for (size_t i = 0; i < Size; i++)
	{
		unsigned Pin;
		Filer >> Pin;
		m_CurrPinsSet.insert(Pin);
	}
	
	Filer >> Size;
	for (size_t i = 0; i < Size; i++)
	{
		_CComponent* pComponent = dynamic_cast<_CComponent*>(Filer.RetriveDynamicClass());
		m_CompList.push_back(pComponent);
	}
	_streamable::Store(Filer);
}
//****************************_CMetaComponent***************************
//****************************_CIdealFollowers***************************

_CIdealFollowers::_CIdealFollowers(_CCircuit* pOwner) :_CMataPathologicComponent(pOwner), m_INp(0), m_INn(0), m_OUTp(0), m_OUTn(0) {}
_CIdealFollowers::_CIdealFollowers(_CCircuit* pOwner, int OUTp, int OUTn, int INp, int INn) :
_CMataPathologicComponent(pOwner), m_INp(INp), m_INn(INn), m_OUTp(OUTp), m_OUTn(OUTn)
{
	//Prepare();
}
_CIdealFollowers::_CIdealFollowers(const _CIdealFollowers& Source) :
_CMataPathologicComponent(Source.m_pBaseCircuit),
m_INp(Source.m_INp), m_INn(Source.m_INn), m_OUTp(Source.m_OUTp), m_OUTn(Source.m_OUTn)
{
	//Prepare();
}

long _CIdealFollowers::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_INp;
	Core *= HASH_FACTOR;
	Core ^= m_INn;
	Core *= HASH_FACTOR;
	Core ^= m_OUTp;
	Core *= HASH_FACTOR;
	Core ^= m_OUTn;
	return Core;
}
void _CIdealFollowers::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_INp);
	Numbers2Exchange.ExchangeIfNeede(m_INn);
	Numbers2Exchange.ExchangeIfNeede(m_OUTp);
	Numbers2Exchange.ExchangeIfNeede(m_OUTn);
	m_SetDeletion.clear();
}
void _CIdealFollowers::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(4);
	Pins[0] = m_INp;
	Pins[1] = m_INn;
	Pins[2] = m_OUTp;
	Pins[3] = m_OUTn;
}
void _CIdealFollowers::Store(_binary_filer& Filer)
{
	Filer << m_INp;
	Filer << m_INn;
	Filer << m_OUTp;
	Filer << m_OUTn;
}
void _CIdealFollowers::Load(_binary_filer& Filer)
{
	Filer >> m_INp;
	Filer >> m_INn;
	Filer >> m_OUTp;
	Filer >> m_OUTn;

}

void _CIdealFollowers::WritePos(iostream& stream)
{
	stream << "(In_p = " << m_pBaseCircuit->ConvertNode(m_INp) <<
		", In_n = " << m_pBaseCircuit->ConvertNode(m_INn) <<
		", Out_p = " << m_pBaseCircuit->ConvertNode(m_OUTp) <<
		", Out_n = " << m_pBaseCircuit->ConvertNode(m_OUTn);
}


//****************************_CIdealFollowers***************************
//****************************_CValtageFollower*************************

IMPLEMENT_DYNAMIC_CREATION(_CValtageFollower);

bool _CValtageFollower::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CValtageFollower* pRR = dynamic_cast<const _CValtageFollower*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

void _CValtageFollower::PrepareDeletions()
{
	int& k = m_INp;
	int& l = m_INn;
	int& p = m_OUTp;
	int& r = m_OUTn;
	m_SetDeletion.clear();
	if (l == r)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(p, l, p, k)));
	else if (k == p)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(r, k, r, l)));
	else if (l==0 && p==0)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(r, l, k, -r)));
	else if (k == 0 && r == 0)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(p, k, l, -p)));
	else
	{
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(r, p, k, l)));
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(r, p, r, p)));
	}
}

//****************************_CValtageFollower*************************
//****************************_CCurrentFollower**************************

IMPLEMENT_DYNAMIC_CREATION(_CCurrentFollower);

bool _CCurrentFollower::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CCurrentFollower* pRR = dynamic_cast<const _CCurrentFollower*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

void _CCurrentFollower::PrepareDeletions()
{
	int& k = m_INp;
	int& l = m_INn;
	int& p = m_OUTp;
	int& r = m_OUTn;
	m_SetDeletion.clear();
	if (l == p)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(r, k, l, k),-1));
	else if (k == r)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(p, l, k, l)));
	else if (l == 0 && r == 0)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(p, -k, l, k),-1));
	else if (k == 0 && p == 0)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(r, -l, k, l),-1));
	else
	{
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(p, r, k, l)));
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(k, l, k, l)));
	}

}

//****************************_CCurrentollower**************************
//****************************_CConveyors*******************************
//IMPLEMENT_DYNAMIC_CREATION(_CConveyors);

long _CConveyors::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_x;
	Core *= HASH_FACTOR;
	Core ^= m_y;
	Core *= HASH_CORE;
	Core ^= HASH_CORE;
	for (int z : m_Z)
	{
		Core *= HASH_CORE;
		Core ^= z;
	}
	return Core;
}

void _CConveyors::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_x);
	Numbers2Exchange.ExchangeIfNeede(m_y);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	for (int& z : m_Z)
		Numbers2Exchange.ExchangeIfNeede(z);
	m_SetDeletion.clear();
	PrepareDeletions();

}

void _CConveyors::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(GetNoOfPorts());
	Pins[0] = m_x;
	Pins[1] = m_y;
	Pins[2] = m_gnd;
	size_t i = 3;
	for (int Z : m_Z)
		Pins[i++] = Z;
}

void _CConveyors::Store(_binary_filer& Filer)
{
	Filer << m_x;
	Filer << m_y;
	Filer << m_gnd;
	size_t Size = m_Z.size();
	Filer << Size;
	for (size_t i = 0; i < Size; i++)
		Filer << m_Z[i];
}
void _CConveyors::Load(_binary_filer& Filer)
{
	Filer >> m_x;
	Filer >> m_y;
	Filer >> m_gnd;
	size_t Size = 0;
	Filer >> Size;
	m_Z.resize(Size);
	for (size_t i = 0; i < Size; i++)
		Filer >> m_Z[i];
}

void _CConveyors::WritePos(iostream& stream)
{
	stream << "(I_x = " << m_pBaseCircuit->ConvertNode(m_x) <<
		", V_y = " << m_pBaseCircuit->ConvertNode(m_y);
	if (m_Z.size() == 1)
		stream << ", I_z = " << m_pBaseCircuit->ConvertNode(m_Z[0]);
	else
		for (size_t i = 0; i < m_Z.size(); i++)
			stream << ", I_z" << i << " = " << m_pBaseCircuit->ConvertNode(m_Z[i]);
	stream<< ")";
}

bool _CConveyors::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CConveyors* pRR = dynamic_cast<const _CConveyors*>(&RightComp);
	if (pRR == nullptr) return false;
	//return *dynamic_cast<const _CConveyors*>(this) == *dynamic_cast<const _CConveyors*> (pRR);
	return *this == *pRR;
}

//****************************_CConveyors*******************************
//****************************_CBOCC*******************************
void _CBOCC::WritePos(iostream& stream)
{
	stream << "(I_x = " << m_pBaseCircuit->ConvertNode(m_x) <<
		", V_y = " << m_pBaseCircuit->ConvertNode(m_y);
	if (m_Z.size() == 1)
		stream << ", I_z = " << m_pBaseCircuit->ConvertNode(m_Z[0]);
	else
		for (size_t i = 0; i < m_Z.size(); i++)
			stream << ", I_z" << i << " = " << m_pBaseCircuit->ConvertNode(m_Z[i]);
	stream << ")";

}
void _CBOCC::WriteType(iostream& stream) 
{ 
	switch (m_Type)
	{
	case CONV_MOCCI:stream << "MOCCI : "; break;
	case CONV_IMOCCI:stream << "IMOCCI : "; break;
	case CONV_MOCCII:stream << "MOCCII : "; break;
	case CONV_IMOCCII:stream << "IMOCCII : "; break;
	case CONV_MOCCIII:stream << "MOCCIII : "; break;
	case CONV_IMOCCIII:stream << "IMOCCIII : "; break;
	default:
		RISEPDD(eIllegalModelName, "Illegal conveyor type");
	}
}

void _CBOCC::PrepareDeletions()
{
	int a = 1, b = 1, g = 1;
	if (m_Type == CONV_IMOCCII || m_Type == CONV_MOCCII)
		g = 0;
	if (m_Type == CONV_IMOCCIII || m_Type == CONV_MOCCIII)
		g = -1;
	if (m_Type == CONV_IMOCCI || m_Type == CONV_IMOCCII || m_Type == CONV_IMOCCIII)
		a = -1;
	VERIFYPDD(m_Z.size() == 2);
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, g*m_y, m_x, a*m_y)));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[0], m_Z[1], m_x, a*m_y)));
}

bool _CBOCC::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CBOCC* pRR = dynamic_cast<const _CBOCC*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

void _CBOCC::Store(_binary_filer& Filer)
{
	_CConveyors::Store(Filer);
	Filer << m_Type;
}

void _CBOCC::Load(_binary_filer& Filer)
{
	_CConveyors::Load(Filer);
	Filer >> m_Type;
}

IMPLEMENT_DYNAMIC_CREATION(_CBOCC);
//****************************_CBOCC*******************************
//****************************_CCCIIp*******************************
IMPLEMENT_DYNAMIC_CREATION(_CCCIIp);

void _CCCIIp::PrepareDeletions()
{
	ASSERTPDD(m_Z.size() >= 1);
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, -m_Z[0], m_x, m_y)));
	for (size_t i = 1; i < m_Z.size();i++)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[i], m_gnd, m_x, m_y)));
}

bool _CCCIIp::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CCCIIp* pRR = dynamic_cast<const _CCCIIp*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_CCCIIp*******************************

//****************************_CCCIp********************************
IMPLEMENT_DYNAMIC_CREATION(_CCCIp);

void _CCCIp::PrepareDeletions()
{
	_CCCIIp::PrepareDeletions();
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_y, m_gnd, m_x, m_y)));
}

bool _CCCIp::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CCCIp* pRR = dynamic_cast<const _CCCIp*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_CCCIp********************************

//****************************_CCCIIIp******************************
IMPLEMENT_DYNAMIC_CREATION(_CCCIIIp);

void _CCCIIIp::PrepareDeletions()
{
	_CCCIIp::PrepareDeletions();
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_y, m_gnd, m_x, m_y),-1));
}

bool _CCCIIIp::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CCCIIIp* pRR = dynamic_cast<const _CCCIIIp*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_CCCIIIp******************************

//****************************_CCCIIm*******************************
IMPLEMENT_DYNAMIC_CREATION(_CCCIIm);

void _CCCIIm::PrepareDeletions()
{
	ASSERTPDD(m_Z.size() >= 1);
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, m_Z[0], m_x, m_y)));
	for (size_t i = 1; i < m_Z.size(); i++)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[i], m_gnd, m_x, m_y),-1));
}

bool _CCCIIm::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CCCIIm* pRR = dynamic_cast<const _CCCIIm*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_CCCIIm*******************************

//****************************_CCCIm********************************
IMPLEMENT_DYNAMIC_CREATION(_CCCIm);

void _CCCIm::PrepareDeletions()
{
	_CCCIIm::PrepareDeletions();
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_y, m_gnd, m_x, m_y)));
}

bool _CCCIm::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CCCIm* pRR = dynamic_cast<const _CCCIm*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_CCCIm********************************

//****************************_CCCIIIm******************************
IMPLEMENT_DYNAMIC_CREATION(_CCCIIIm);

void _CCCIIIm::PrepareDeletions()
{
	_CCCIIm::PrepareDeletions();
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_y, m_gnd, m_x, m_y),-1));
}

bool _CCCIIIm::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CCCIIIm* pRR = dynamic_cast<const _CCCIIIm*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_CCCIIIm******************************

//****************************_ICCCIIp******************************
IMPLEMENT_DYNAMIC_CREATION(_ICCCIIp);

void _ICCCIIp::PrepareDeletions()
{
	ASSERTPDD(m_Z.size() >= 1);
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, -m_Z[0], m_x, -m_y)));
	for (size_t i = 1; i < m_Z.size(); i++)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[i], m_gnd, m_x, -m_y)));
}

bool _ICCCIIp::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _ICCCIIp* pRR = dynamic_cast<const _ICCCIIp*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_ICCCIIp******************************

//****************************_ICCCIp*******************************
IMPLEMENT_DYNAMIC_CREATION(_ICCCIp);

void _ICCCIp::PrepareDeletions()
{
	_ICCCIIp::PrepareDeletions();
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_y, m_gnd, m_x, m_y)));
}

bool _ICCCIp::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _ICCCIp* pRR = dynamic_cast<const _ICCCIp*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_ICCCIp*******************************

//****************************_ICCCIIIp*****************************
IMPLEMENT_DYNAMIC_CREATION(_ICCCIIIp);

void _ICCCIIIp::PrepareDeletions()
{
	_ICCCIIp::PrepareDeletions();
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_y, m_gnd, m_x, m_y),-1));
}

bool _ICCCIIIp::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _ICCCIIIp* pRR = dynamic_cast<const _ICCCIIIp*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_ICCCIIIp*****************************

//****************************_ICCCIIm******************************
IMPLEMENT_DYNAMIC_CREATION(_ICCCIIm);

void _ICCCIIm::PrepareDeletions()
{
	ASSERTPDD(m_Z.size() >= 1);
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, m_Z[0], m_x, -m_y)));
	for (size_t i = 1; i < m_Z.size(); i++)
		m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[i], m_gnd, m_x, -m_y), -1));
}

bool _ICCCIIm::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _ICCCIIm* pRR = dynamic_cast<const _ICCCIIm*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_ICCCIIm******************************

//****************************_ICCCIm*******************************
IMPLEMENT_DYNAMIC_CREATION(_ICCCIm);

void _ICCCIm::PrepareDeletions()
{
	_ICCCIIm::PrepareDeletions();
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_y, m_gnd, m_x, m_y)));
}

bool _ICCCIm::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _ICCCIm* pRR = dynamic_cast<const _ICCCIm*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_ICCCIm*******************************

//****************************_ICCCIIIm*****************************
IMPLEMENT_DYNAMIC_CREATION(_ICCCIIIm);

void _ICCCIIIm::PrepareDeletions()
{
	_ICCCIIm::PrepareDeletions();
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_y, m_gnd, m_x, m_y),-1));
}

bool _ICCCIIIm::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _ICCCIIIm* pRR = dynamic_cast<const _ICCCIIIm*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_ICCCIIIm*****************************

//****************************_CMOICCGen****************************
//IMPLEMENT_DYNAMIC_CREATION(_CMOICCGen);

void _CMOICCGen::WritePos(iostream& stream)
{
	stream << "(I_x = " << m_pBaseCircuit->ConvertNode(m_x) <<
		", V_y = " << m_pBaseCircuit->ConvertNode(m_y) <<
		", V_z1_p = " << m_pBaseCircuit->ConvertNode(m_Z[0]) <<
		", V_z1_m = " << m_pBaseCircuit->ConvertNode(m_Z[0]) <<
		", V_z2_p = " << m_pBaseCircuit->ConvertNode(m_Z[0]) <<
		", V_z2_m = " << m_pBaseCircuit->ConvertNode(m_Z[0]) << ")";
}

//void _CMOICCGen::PrepareDeletions()
//{
//	ASSERTPDD(m_Z.size() >= 1);
//	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, m_Z[0], m_x, -m_y)));
//}
//
//bool _CMOICCGen::IsEqualIfSameType(const _CComponent& RightComp)
//{
//	const _CMOICCGen* pRR = dynamic_cast<const _CMOICCGen*>(&RightComp);
//	if (pRR == nullptr) return false;
//	return *this == *pRR;
//}

//****************************_CMOICCGen****************************

//****************************_CMOICCI******************************
IMPLEMENT_DYNAMIC_CREATION(_CMOICCI);


void _CMOICCI::PrepareDeletions()
{
	ASSERTPDD(m_Z.size() == 4);
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[0], m_Z[1], m_x, -m_y),-1));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[2], m_Z[3], m_x, -m_y),-1));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, -m_y, m_x, -m_y)));
}

bool _CMOICCI::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CMOICCI* pRR = dynamic_cast<const _CMOICCI*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_CMOICCI******************************

//****************************_CMOICCII*****************************
IMPLEMENT_DYNAMIC_CREATION(_CMOICCII);


void _CMOICCII::PrepareDeletions()
{
	ASSERTPDD(m_Z.size() == 4);
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[0], m_Z[1], m_x, m_y),-1));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[2], m_Z[3], m_x, m_y),-1));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, m_gnd, m_x, m_y)));
}

bool _CMOICCII::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CMOICCII* pRR = dynamic_cast<const _CMOICCII*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_CMOICCII*****************************

//****************************_CMOICCIII****************************
IMPLEMENT_DYNAMIC_CREATION(_CMOICCIII);


void _CMOICCIII::PrepareDeletions()
{
	ASSERTPDD(m_Z.size() == 4);
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[0], m_Z[1], m_x, m_y),-1));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Z[2], m_Z[3], m_x, m_y),-1));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, m_y, m_x, m_y)));
}

bool _CMOICCIII::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CMOICCIII* pRR = dynamic_cast<const _CMOICCIII*>(&RightComp);
	if (pRR == nullptr) return false;
	return *this == *pRR;
}

//****************************_CMOICCIII****************************
//****************************_CFDCFOA_RM_INF***********************
IMPLEMENT_DYNAMIC_CREATION(_CFDCFOA_RM_INF);

void _CFDCFOA_RM_INF::PrepareDeletions()
{
	_CBasicSetOfDeletions Set;
	Set.push_back(_CBasicPairOfDeletion(m_Ixn, m_Ixp, m_Vyp, m_Ixp));
	Set.push_back(_CBasicPairOfDeletion(m_Izp, m_Ixp, m_Vyn, m_Ixn));
	Set.push_back(_CBasicPairOfDeletion(m_Vwp, m_Ixp, m_Vwp, m_Izp));
	Set.push_back(_CBasicPairOfDeletion(m_Vwn, m_Ixp, m_Vwn, m_Izn));
	m_SetDeletion.push_back(Set);

	Set.clear();
	Set.push_back(_CBasicPairOfDeletion(m_Ixn, m_Ixp, m_Vyp, m_Ixp));
	Set.push_back(_CBasicPairOfDeletion(m_Izn, m_Ixp, m_Vyn, m_Ixn));
	Set.push_back(_CBasicPairOfDeletion(m_Vwp, m_Ixp, m_Vwp, m_Izp));
	Set.push_back(_CBasicPairOfDeletion(m_Vwn, m_Ixp, m_Vwn, m_Izn));
	m_SetDeletion.push_back(Set);

	Set.clear();
	Set.MultSgn(-1);
	Set.push_back(_CBasicPairOfDeletion(m_Izp, m_Ixp, m_Vyp, m_Ixp));
	Set.push_back(_CBasicPairOfDeletion(m_Izn, m_Ixp, m_Vyn, m_Ixn));
	Set.push_back(_CBasicPairOfDeletion(m_Vwp, m_Ixp, m_Vwp, m_Izp));
	Set.push_back(_CBasicPairOfDeletion(m_Vwn, m_Ixp, m_Vwn, m_Izn));
	m_SetDeletion.push_back(Set);

	Set.clear();
	Set.push_back(_CBasicPairOfDeletion(m_Izp, m_Ixn, m_Vyp, m_Ixp));
	Set.push_back(_CBasicPairOfDeletion(m_Izn, m_Ixn, m_Vyn, m_Ixn));
	Set.push_back(_CBasicPairOfDeletion(m_Vwp, m_Ixn, m_Vwp, m_Izp));
	Set.push_back(_CBasicPairOfDeletion(m_Vwn, m_Ixn, m_Vwn, m_Izn));
	m_SetDeletion.push_back(Set);
}

long _CFDCFOA_RM_INF::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_Vyp;
	Core *= HASH_FACTOR;
	Core ^= m_Vyn;
	Core *= HASH_FACTOR;
	Core ^= m_Ixp;
	Core *= HASH_FACTOR;
	Core ^= m_Ixn;
	Core *= HASH_FACTOR;
	Core ^= m_Izp;
	Core *= HASH_FACTOR;
	Core ^= m_Izn;
	Core *= HASH_FACTOR;
	Core ^= m_Vwp;
	Core *= HASH_FACTOR;
	Core ^= m_Vwn;
	return Core;
}
void _CFDCFOA_RM_INF::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_Vyp);
	Numbers2Exchange.ExchangeIfNeede(m_Vyn);
	Numbers2Exchange.ExchangeIfNeede(m_Ixp);
	Numbers2Exchange.ExchangeIfNeede(m_Ixn);
	Numbers2Exchange.ExchangeIfNeede(m_Izp);
	Numbers2Exchange.ExchangeIfNeede(m_Izn);
	Numbers2Exchange.ExchangeIfNeede(m_Vwp);
	Numbers2Exchange.ExchangeIfNeede(m_Vwn);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CFDCFOA_RM_INF::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(8);
	Pins[0] = m_Vyp;
	Pins[1] = m_Vyp;
	Pins[2] = m_Ixp;
	Pins[3] = m_Ixn;
	Pins[4] = m_Izp;
	Pins[5] = m_Izn;
	Pins[6] = m_Vwp;
	Pins[7] = m_Vwn;
}

void _CFDCFOA_RM_INF::WritePos(iostream& stream)
{
	stream << "(V_y_p = " << m_pBaseCircuit->ConvertNode(m_Vyp) <<
		", V_y_n = " << m_pBaseCircuit->ConvertNode(m_Vyn) <<
		", I_x_p = " << m_pBaseCircuit->ConvertNode(m_Ixp) <<
		", I_x_n = " << m_pBaseCircuit->ConvertNode(m_Ixn) <<
		", I_z_p = " << m_pBaseCircuit->ConvertNode(m_Izp) <<
		", I_z_n = " << m_pBaseCircuit->ConvertNode(m_Izn) <<
		", V_w_p = " << m_pBaseCircuit->ConvertNode(m_Vwp) <<
		", V_w_n = " << m_pBaseCircuit->ConvertNode(m_Vwn) << ")";
}

bool _CFDCFOA_RM_INF::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CFDCFOA_RM_INF* pDV = dynamic_cast<const _CFDCFOA_RM_INF*> (&RightComp);
	if (pDV == nullptr) return false;
	return *pDV == *this;
}


void _CFDCFOA_RM_INF::Store(_binary_filer& Filer)
{
	Filer << m_Vyp;
	Filer << m_Vyn;
	Filer << m_Ixp;
	Filer << m_Ixn;
	Filer << m_Izp;
	Filer << m_Izn;
	Filer << m_Vwp;
	Filer << m_Vwn;
}

void _CFDCFOA_RM_INF::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_Vyp;
	Filer >> m_Vyn;
	Filer >> m_Ixp;
	Filer >> m_Ixn;
	Filer >> m_Izp;
	Filer >> m_Izn;
	Filer >> m_Vwp;
	Filer >> m_Vwn;
	PrepareDeletions();
}

//****************************_CFDCFOA_RM_INF***********************
//****************************_CDDOMA*******************************

IMPLEMENT_DYNAMIC_CREATION(_CDDOMA);

void _CDDOMA::PrepareDeletions()
{
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Von, -m_Vop, m_V2, m_V1)));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Von, -m_Vop, m_V3, m_V4)));
}

long _CDDOMA::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_V1;
	Core *= HASH_FACTOR;
	Core ^= m_V2;
	Core *= HASH_FACTOR;
	Core ^= m_V3;
	Core *= HASH_FACTOR;
	Core ^= m_V4;
	Core *= HASH_FACTOR;
	Core ^= m_Von;
	Core *= HASH_FACTOR;
	Core ^= m_Vop;
	Core *= HASH_FACTOR;
	Core ^= m_gnd;
	return Core;
}
void _CDDOMA::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_V1);
	Numbers2Exchange.ExchangeIfNeede(m_V2);
	Numbers2Exchange.ExchangeIfNeede(m_V3);
	Numbers2Exchange.ExchangeIfNeede(m_V4);
	Numbers2Exchange.ExchangeIfNeede(m_Von);
	Numbers2Exchange.ExchangeIfNeede(m_Vop);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CDDOMA::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(7);
	Pins[0] = m_V1;
	Pins[1] = m_V2;
	Pins[2] = m_V3;
	Pins[3] = m_V4;
	Pins[4] = m_Von;
	Pins[5] = m_Vop;
	Pins[6] = m_gnd;
}

void _CDDOMA::WritePos(iostream& stream)
{
	stream << "(V1 = " << m_pBaseCircuit->ConvertNode(m_V1) <<
		", V2 = " << m_pBaseCircuit->ConvertNode(m_V2) <<
		", V3 = " << m_pBaseCircuit->ConvertNode(m_V3) <<
		", V4 = " << m_pBaseCircuit->ConvertNode(m_V4) <<
		", V_o_n = " << m_pBaseCircuit->ConvertNode(m_Von) <<
		", V_o_p = " << m_pBaseCircuit->ConvertNode(m_Vop) << ")";
}

bool _CDDOMA::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CDDOMA* pDV = dynamic_cast<const _CDDOMA*> (&RightComp);
	if (pDV == nullptr) return false;
	return *pDV == *this;
}


void _CDDOMA::Store(_binary_filer& Filer)
{
	Filer << m_V1;
	Filer << m_V2;
	Filer << m_V3;
	Filer << m_V4;
	Filer << m_Von;
	Filer << m_Vop;
}

void _CDDOMA::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_V1;
	Filer >> m_V2;
	Filer >> m_V3;
	Filer >> m_V4;
	Filer >> m_Von;
	Filer >> m_Vop;
	PrepareDeletions();
}

//****************************_CDDOMA*******************************

//****************************_CFBDDA******************************

IMPLEMENT_DYNAMIC_CREATION(_CFBDDA);

void _CFBDDA::PrepareDeletions()
{
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Ion, m_Iop, m_V2, m_V1)));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Ion, m_Iop, m_V3, m_V4)));
}

long _CFBDDA::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_V1;
	Core *= HASH_FACTOR;
	Core ^= m_V2;
	Core *= HASH_FACTOR;
	Core ^= m_V3;
	Core *= HASH_FACTOR;
	Core ^= m_V4;
	Core *= HASH_FACTOR;
	Core ^= m_Ion;
	Core *= HASH_FACTOR;
	Core ^= m_Iop;
	Core *= HASH_FACTOR;
	Core ^= m_gnd;
	return Core;
}
void _CFBDDA::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_V1);
	Numbers2Exchange.ExchangeIfNeede(m_V2);
	Numbers2Exchange.ExchangeIfNeede(m_V3);
	Numbers2Exchange.ExchangeIfNeede(m_V4);
	Numbers2Exchange.ExchangeIfNeede(m_Ion);
	Numbers2Exchange.ExchangeIfNeede(m_Iop);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CFBDDA::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(7);
	Pins[0] = m_V1;
	Pins[1] = m_V2;
	Pins[2] = m_V3;
	Pins[3] = m_V4;
	Pins[4] = m_Ion;
	Pins[5] = m_Iop;
	Pins[6] = m_gnd;
}

void _CFBDDA::WritePos(iostream& stream)
{
	stream << "(V1 = " << m_pBaseCircuit->ConvertNode(m_V1) <<
		", V2 = " << m_pBaseCircuit->ConvertNode(m_V2) <<
		", V3 = " << m_pBaseCircuit->ConvertNode(m_V3) <<
		", V4 = " << m_pBaseCircuit->ConvertNode(m_V4) <<
		", I_o_n = " << m_pBaseCircuit->ConvertNode(m_Ion) <<
		", I_o_p = " << m_pBaseCircuit->ConvertNode(m_Iop) << ")";
}

bool _CFBDDA::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CFBDDA* pDV = dynamic_cast<const _CFBDDA*> (&RightComp);
	if (pDV == nullptr) return false;
	return *pDV == *this;
}


void _CFBDDA::Store(_binary_filer& Filer)
{
	Filer << m_V1;
	Filer << m_V2;
	Filer << m_V3;
	Filer << m_V4;
	Filer << m_Ion;
	Filer << m_Iop;
}

void _CFBDDA::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_V1;
	Filer >> m_V2;
	Filer >> m_V3;
	Filer >> m_V4;
	Filer >> m_Ion;
	Filer >> m_Iop;
	PrepareDeletions();
}

//****************************_CFBDDA******************************

//****************************_CDVCFOA*****************************

IMPLEMENT_DYNAMIC_CREATION(_CDVCFOA);

void _CDVCFOA::PrepareDeletions()
{
	_CBasicPairOfDeletion BD(m_Vw, m_gnd, m_Vw, m_Iz);
	_CBasicSetOfDeletions Set1(BD);
	_CBasicSetOfDeletions Set2(BD,-1);
	Set1.push_back(_CBasicPairOfDeletion(m_Iz, -m_Vx, m_Vyp, m_Vyn));
	Set2.push_back(_CBasicPairOfDeletion(m_Iz, -m_Vx, m_Vx, m_gnd));
	m_SetDeletion.push_back(Set1);
	m_SetDeletion.push_back(Set2);
}

long _CDVCFOA::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_Vx;
	Core *= HASH_FACTOR;
	Core ^= m_Vyp;
	Core *= HASH_FACTOR;
	Core ^= m_Vyn;
	Core *= HASH_FACTOR;
	Core ^= m_Iz;
	Core *= HASH_FACTOR;
	Core ^= m_Vw;
	Core *= HASH_FACTOR;
	Core ^= m_gnd;
	return Core;
}
void _CDVCFOA::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_Vx);
	Numbers2Exchange.ExchangeIfNeede(m_Vyp);
	Numbers2Exchange.ExchangeIfNeede(m_Vyn);
	Numbers2Exchange.ExchangeIfNeede(m_Iz);
	Numbers2Exchange.ExchangeIfNeede(m_Vw);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CDVCFOA::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(6);
	Pins[0] = m_Vx;
	Pins[1] = m_Vyp;
	Pins[2] = m_Vyn;
	Pins[3] = m_Iz;
	Pins[4] = m_Vw;
	Pins[5] = m_gnd;
}

void _CDVCFOA::WritePos(iostream& stream)
{
	stream << "(V_x = " << m_pBaseCircuit->ConvertNode(m_Vx) <<
		", V_y_p = " << m_pBaseCircuit->ConvertNode(m_Vyp) <<
		", V_y_n = " << m_pBaseCircuit->ConvertNode(m_Vyn) <<
		", I_z = " << m_pBaseCircuit->ConvertNode(m_Iz) <<
		", V_w = " << m_pBaseCircuit->ConvertNode(m_Vw) << ")";
}

bool _CDVCFOA::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CDVCFOA* pDV = dynamic_cast<const _CDVCFOA*> (&RightComp);
	if (pDV == nullptr) return false;
	return *pDV == *this;
}


void _CDVCFOA::Store(_binary_filer& Filer)
{
	Filer << m_Vx;
	Filer << m_Vyp;
	Filer << m_Vyn;
	Filer << m_Iz;
	Filer << m_Vw;
}

void _CDVCFOA::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_Vx;
	Filer >> m_Vyp;
	Filer >> m_Vyn;
	Filer >> m_Iz;
	Filer >> m_Vw;
	PrepareDeletions();
}
//****************************_CDVCFOA*****************************

//****************************_CDDCCFOA****************************

IMPLEMENT_DYNAMIC_CREATION(_CDDCCFOA);

void _CDDCCFOA::PrepareDeletions()
{
	//D(x+0)(y2+y1),(w1+0)(w1+z1),(w2+0)(w2+z2)
	//-D(z2+z1)(y2+y1),(w1+0)(w1+z1),(w2+0)(w2+z2)
	//D(x+0)(x+y3),(w1+0)(w1+z1),(w2+0)(w2+z2)
	//-D(z2+z1)(x+y3),(w1+0)(w1+z1),(w2+0)(w2+z2)
	_CBasicSetOfDeletions Set1(_CBasicPairOfDeletion(m_Vw1,m_gnd,m_Vw1,m_Iz1));
	Set1.push_back(_CBasicPairOfDeletion(m_Vw2, m_gnd, m_Vw2, m_Iz2));

	_CBasicSetOfDeletions Set2(Set1), Set3(Set1), Set4(Set1);
	Set2.MultSgn(-1); Set4.MultSgn(-1);
	Set1.push_back(_CBasicPairOfDeletion(m_Vx, m_gnd, m_Vy2, m_Vy1));
	Set2.push_back(_CBasicPairOfDeletion(m_Iz2, m_Iz1, m_Vy2, m_Vy1));
	Set3.push_back(_CBasicPairOfDeletion(m_Vx, m_gnd, m_Vx, m_Vy3));
	Set4.push_back(_CBasicPairOfDeletion(m_Iz2, m_Iz1, m_Vx, m_Vy3));
	m_SetDeletion.push_back(Set1);
	m_SetDeletion.push_back(Set2);
	m_SetDeletion.push_back(Set3);
	m_SetDeletion.push_back(Set4);
}

long _CDDCCFOA::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_Vx;
	Core *= HASH_FACTOR;
	Core ^= m_Vy1;
	Core *= HASH_FACTOR;
	Core ^= m_Vy2;
	Core *= HASH_FACTOR;
	Core ^= m_Vy3;
	Core *= HASH_FACTOR;
	Core ^= m_Iz1;
	Core *= HASH_FACTOR;
	Core ^= m_Iz2;
	Core *= HASH_FACTOR;
	Core ^= m_Vw1;
	Core *= HASH_FACTOR;
	Core ^= m_Vw2;
	Core *= HASH_FACTOR;
	Core ^= m_gnd;
	return Core;
}
void _CDDCCFOA::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_Vx);
	Numbers2Exchange.ExchangeIfNeede(m_Vy1);
	Numbers2Exchange.ExchangeIfNeede(m_Vy2);
	Numbers2Exchange.ExchangeIfNeede(m_Vy2);
	Numbers2Exchange.ExchangeIfNeede(m_Iz1);
	Numbers2Exchange.ExchangeIfNeede(m_Iz2);
	Numbers2Exchange.ExchangeIfNeede(m_Vw1);
	Numbers2Exchange.ExchangeIfNeede(m_Vw2);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CDDCCFOA::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(9);
	Pins[0] = m_Vx;
	Pins[1] = m_Vy1;
	Pins[2] = m_Vy2;
	Pins[3] = m_Vy3;
	Pins[4] = m_Iz1;
	Pins[5] = m_Iz2;
	Pins[6] = m_Vw1;
	Pins[7] = m_Vw2;
	Pins[8] = m_gnd;
}

void _CDDCCFOA::WritePos(iostream& stream)
{
	stream << "(V_x = " << m_pBaseCircuit->ConvertNode(m_Vx) <<
		", V_y1 = " << m_pBaseCircuit->ConvertNode(m_Vy1) <<
		", V_y2 = " << m_pBaseCircuit->ConvertNode(m_Vy2) <<
		", V_y3 = " << m_pBaseCircuit->ConvertNode(m_Vy3) <<
		", I_z1 = " << m_pBaseCircuit->ConvertNode(m_Iz1) <<
		", I_z2 = " << m_pBaseCircuit->ConvertNode(m_Iz2) <<
		", V_w1 = " << m_pBaseCircuit->ConvertNode(m_Vw1) <<
		", V_w2 = " << m_pBaseCircuit->ConvertNode(m_Vw2) << ")";
}

bool _CDDCCFOA::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CDDCCFOA* pDV = dynamic_cast<const _CDDCCFOA*> (&RightComp);
	if (pDV == nullptr) return false;
	return *pDV == *this;
}


void _CDDCCFOA::Store(_binary_filer& Filer)
{
	Filer << m_Vx;
	Filer << m_Vy1;
	Filer << m_Vy2;
	Filer << m_Vy3;
	Filer << m_Iz1;
	Filer << m_Iz2;
	Filer << m_Vw1;
	Filer << m_Vw2;
}

void _CDDCCFOA::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_Vx;
	Filer >> m_Vy1;
	Filer >> m_Vy2;
	Filer >> m_Vy3;
	Filer >> m_Iz1;
	Filer >> m_Iz2;
	Filer >> m_Vw1;
	Filer >> m_Vw2;
	PrepareDeletions();
}
//****************************_CDDCCFOA****************************
//****************************_CCustomOneParameterIdealizedComponent****************************

IMPLEMENT_DYNAMIC_CREATION(_CCustomOneParameterIdealizedComponent);


_CCustomOneParameterIdealizedComponent::_CCustomOneParameterIdealizedComponent(_CCircuit* pOwner ) {
}
_CCustomOneParameterIdealizedComponent::_CCustomOneParameterIdealizedComponent(const _CCustomOneParameterIdealizedComponent& Source) {
}
_CCustomOneParameterIdealizedComponent::_CCustomOneParameterIdealizedComponent(_CCircuit* pOwner, string parName, 
	const DELETIONS& multDels, const DELETIONS& remDels, long double Value ) {
}

int _CCustomOneParameterIdealizedComponent::GetNoOfPorts() const { return 1; }
long _CCustomOneParameterIdealizedComponent::GetHash(long Core ) const { return 0; }
void _CCustomOneParameterIdealizedComponent::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange) {

}
void _CCustomOneParameterIdealizedComponent::RaportConnections(COMPONENT_PINS& Pins) const {

}
bool _CCustomOneParameterIdealizedComponent::IsEqualIfSameType(const _CComponent& RightComp) {
	return false;
}
//protected:
void _CCustomOneParameterIdealizedComponent::WritePos(iostream& stream) {

}
void _CCustomOneParameterIdealizedComponent::Store(_binary_filer& Filer) {

}
void _CCustomOneParameterIdealizedComponent::Load(_binary_filer& Filer) {

}
void _CCustomOneParameterIdealizedComponent::PrepareDeletions() {

}
//};
//****************************_CCustomOneParameterIdealizedComponent****************************

//****************************_CDDCCpm*****************************

IMPLEMENT_DYNAMIC_CREATION(_CDDCCpm);

void _CDDCCpm::PrepareDeletions()
{
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Vx, m_gnd, m_Vy2, m_Vy1)));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Iz1, m_Iz2, m_Vy2, m_Vy1)));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Vx, m_gnd, m_Vx, m_Vy3)));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Iz1, m_Iz2, m_Vx, m_Vy3)));
}

long _CDDCCpm::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_Vx;
	Core *= HASH_FACTOR;
	Core ^= m_Vy1;
	Core *= HASH_FACTOR;
	Core ^= m_Vy2;
	Core *= HASH_FACTOR;
	Core ^= m_Vy3;
	Core *= HASH_FACTOR;
	Core ^= m_Iz1;
	Core *= HASH_FACTOR;
	Core ^= m_Iz2;
	Core *= HASH_FACTOR;
	Core ^= m_gnd;
	return Core;
}
void _CDDCCpm::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_Vx);
	Numbers2Exchange.ExchangeIfNeede(m_Vy1);
	Numbers2Exchange.ExchangeIfNeede(m_Vy2);
	Numbers2Exchange.ExchangeIfNeede(m_Vy2);
	Numbers2Exchange.ExchangeIfNeede(m_Iz1);
	Numbers2Exchange.ExchangeIfNeede(m_Iz2);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CDDCCpm::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(7);
	Pins[0] = m_Vx;
	Pins[1] = m_Vy1;
	Pins[2] = m_Vy2;
	Pins[3] = m_Vy3;
	Pins[4] = m_Iz1;
	Pins[5] = m_Iz2;
	Pins[6] = m_gnd;
}

void _CDDCCpm::WritePos(iostream& stream)
{
	stream << "(V_x = " << m_pBaseCircuit->ConvertNode(m_Vx) <<
		", V_y1 = " << m_pBaseCircuit->ConvertNode(m_Vy1) <<
		", V_y2 = " << m_pBaseCircuit->ConvertNode(m_Vy2) <<
		", V_y3 = " << m_pBaseCircuit->ConvertNode(m_Vy3) <<
		", I_z1 = " << m_pBaseCircuit->ConvertNode(m_Iz1) <<
		", I_z2 = " << m_pBaseCircuit->ConvertNode(m_Iz2) << ")";
}

bool _CDDCCpm::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CDDCCpm* pDV = dynamic_cast<const _CDDCCpm*> (&RightComp);
	if (pDV == nullptr) return false;
	return *pDV == *this;
}


void _CDDCCpm::Store(_binary_filer& Filer)
{
	Filer << m_Vx;
	Filer << m_Vy1;
	Filer << m_Vy2;
	Filer << m_Vy3;
	Filer << m_Iz1;
	Filer << m_Iz2;
}

void _CDDCCpm::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_Vx;
	Filer >> m_Vy1;
	Filer >> m_Vy2;
	Filer >> m_Vy3;
	Filer >> m_Iz1;
	Filer >> m_Iz2;
	PrepareDeletions();
}
//****************************_CDDCCpm******************************

//****************************_CDXCCII*****************************

IMPLEMENT_DYNAMIC_CREATION(_CDXCCII);

void _CDXCCII::PrepareDeletions()
{
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Iz1, -m_Vx1, m_Vx1, m_Vy)));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_Iz2, m_Vx2, m_Vx2, -m_Vy)));
}

long _CDXCCII::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_Vx1;
	Core *= HASH_FACTOR;
	Core ^= m_Vx2;
	Core *= HASH_FACTOR;
	Core ^= m_Vy;
	Core *= HASH_FACTOR;
	Core ^= m_Iz1;
	Core *= HASH_FACTOR;
	Core ^= m_Iz2;
	Core *= HASH_FACTOR;
	Core ^= m_gnd;
	return Core;
}
void _CDXCCII::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_Vx1);
	Numbers2Exchange.ExchangeIfNeede(m_Vx2);
	Numbers2Exchange.ExchangeIfNeede(m_Vy);
	Numbers2Exchange.ExchangeIfNeede(m_Iz1);
	Numbers2Exchange.ExchangeIfNeede(m_Iz2);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CDXCCII::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(7);
	Pins[0] = m_Vx1;
	Pins[1] = m_Vx2;
	Pins[2] = m_Vy;
	Pins[3] = m_Iz1;
	Pins[4] = m_Iz2;
	Pins[5] = m_gnd;
}

void _CDXCCII::WritePos(iostream& stream)
{
	stream << "(V_x1 = " << m_pBaseCircuit->ConvertNode(m_Vx1) <<
		", V_x2 = " << m_pBaseCircuit->ConvertNode(m_Vx2) <<
		", V_y = " << m_pBaseCircuit->ConvertNode(m_Vy) <<
		", I_z1 = " << m_pBaseCircuit->ConvertNode(m_Iz1) <<
		", I_z2 = " << m_pBaseCircuit->ConvertNode(m_Iz2) << ")";
}

bool _CDXCCII::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CDXCCII* pDV = dynamic_cast<const _CDXCCII*> (&RightComp);
	if (pDV == nullptr) return false;
	return *pDV == *this;
}


void _CDXCCII::Store(_binary_filer& Filer)
{
	Filer << m_Vx1;
	Filer << m_Vx2;
	Filer << m_Vy;
	Filer << m_Iz1;
	Filer << m_Iz2;
}

void _CDXCCII::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_Vx1;
	Filer >> m_Vx2;
	Filer >> m_Vy;
	Filer >> m_Iz1;
	Filer >> m_Iz2;
	PrepareDeletions();
}
//****************************_CDXCCII******************************

//****************************_CFDVDCCIIpm**************************

IMPLEMENT_DYNAMIC_CREATION(_CFDVDCCIIpm);

void _CFDVDCCIIpm::PrepareDeletions()
{
	_CBasicSetOfDeletions Set1(_CBasicPairOfDeletion(m_Iz1, m_Vx1, m_Vy1, m_Vy2));
	Set1.push_back(_CBasicPairOfDeletion(m_Iz2, m_Vx2, m_Vx1, m_Vx2));
	_CBasicSetOfDeletions Set2(_CBasicPairOfDeletion(m_Iz1, m_Vx1, m_Vx1, m_gnd));
	Set1.push_back(_CBasicPairOfDeletion(m_Iz2, m_Vx2, m_Vx2, m_gnd));
	m_SetDeletion.push_back(Set1);
	m_SetDeletion.push_back(Set2);
}

long _CFDVDCCIIpm::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_Vx1;
	Core *= HASH_FACTOR;
	Core ^= m_Vx2;
	Core *= HASH_FACTOR;
	Core ^= m_Vy1;
	Core *= HASH_FACTOR;
	Core ^= m_Vy2;
	Core *= HASH_FACTOR;
	Core ^= m_Iz1;
	Core *= HASH_FACTOR;
	Core ^= m_Iz2;
	Core *= HASH_FACTOR;
	Core ^= m_gnd;
	return Core;
}
void _CFDVDCCIIpm::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_Vx1);
	Numbers2Exchange.ExchangeIfNeede(m_Vx2);
	Numbers2Exchange.ExchangeIfNeede(m_Vy1);
	Numbers2Exchange.ExchangeIfNeede(m_Vy2);
	Numbers2Exchange.ExchangeIfNeede(m_Iz1);
	Numbers2Exchange.ExchangeIfNeede(m_Iz2);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CFDVDCCIIpm::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(7);
	Pins[0] = m_Vx1;
	Pins[1] = m_Vx2;
	Pins[2] = m_Vy1;
	Pins[3] = m_Vy2;
	Pins[4] = m_Iz1;
	Pins[5] = m_Iz2;
	Pins[6] = m_gnd;
}

void _CFDVDCCIIpm::WritePos(iostream& stream)
{
	stream << "(V_x1 = " << m_pBaseCircuit->ConvertNode(m_Vx1) <<
		", V_x2 = " << m_pBaseCircuit->ConvertNode(m_Vx2) <<
		", V_y1 = " << m_pBaseCircuit->ConvertNode(m_Vy1) <<
		", V_y2 = " << m_pBaseCircuit->ConvertNode(m_Vy2) <<
		", I_z1 = " << m_pBaseCircuit->ConvertNode(m_Iz1) <<
		", I_z2 = " << m_pBaseCircuit->ConvertNode(m_Iz2) << ")";
}

bool _CFDVDCCIIpm::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CFDVDCCIIpm* pDV = dynamic_cast<const _CFDVDCCIIpm*> (&RightComp);
	if (pDV == nullptr) return false;
	return *pDV == *this;
}


void _CFDVDCCIIpm::Store(_binary_filer& Filer)
{
	Filer << m_Vx1;
	Filer << m_Vx2;
	Filer << m_Vy1;
	Filer << m_Vy2;
	Filer << m_Iz1;
	Filer << m_Iz2;
}

void _CFDVDCCIIpm::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_Vx1;
	Filer >> m_Vx2;
	Filer >> m_Vy1;
	Filer >> m_Vy2;
	Filer >> m_Iz1;
	Filer >> m_Iz2;
	PrepareDeletions();
}
//****************************_CFDVDCCIIpm**************************

//****************************_CFBCCII******************************

IMPLEMENT_DYNAMIC_CREATION(_CFBCCII);

void _CFBCCII::PrepareDeletions()
{
	_CBasicSetOfDeletions Set1(_CBasicPairOfDeletion(m_Vx1, m_gnd, m_Vx1, m_Vx2));
	Set1.push_back(_CBasicPairOfDeletion(m_Vx2, m_gnd, m_Vx1, m_Vx2));

	_CBasicSetOfDeletions Set2(_CBasicPairOfDeletion(m_Vx1, m_Vx2, m_Vx1, m_Vx2),-1);
	Set2.push_back(_CBasicPairOfDeletion(m_Iz1, m_Iz2, m_Vx1, m_Vx2));

	_CBasicSetOfDeletions Set3(_CBasicPairOfDeletion(m_Vx1, m_gnd, m_Vx1, m_gnd), -1);
	Set3.push_back(_CBasicPairOfDeletion(m_Vx2, m_gnd, m_Vx2, m_gnd));

	_CBasicSetOfDeletions Set4(_CBasicPairOfDeletion(m_Vx1, m_Vx2, m_Vx1, m_gnd), -1);
	Set4.push_back(_CBasicPairOfDeletion(m_Iz1, m_Iz2, m_Vx2, m_gnd));

	m_SetDeletion.push_back(Set1);
	m_SetDeletion.push_back(Set2);
	m_SetDeletion.push_back(Set3);
	m_SetDeletion.push_back(Set4);
}

bool _CFBCCII::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CFBCCII* pDV = dynamic_cast<const _CFBCCII*> (&RightComp);
	if (pDV == nullptr) return false;
	return *pDV == *this;
}

//****************************_CFBCCII******************************

//****************************_CFBCCI*******************************

//IMPLEMENT_DYNAMIC_CREATION(_CFBCCI);
//
//void _CFBCCI::PrepareDeletions()
//{
//	// D(V_y_2-V_y_1)(V_y_2+V_y_1),(V_x_2+V_x_1)(V_x_2+V_x_1) 
//	// D(V_y_2-V_y_1)(V_x_1+0),(V_x_2+V_x_1)(V_x_2+0)
//	_CBasicSetOfDeletions Set1(_CBasicPairOfDeletion(m_Vx1, -m_Vy2, m_Vy1, m_Vy2));
//	Set1.push_back(_CBasicPairOfDeletion(m_Vx1, m_Vx2, m_Vx1, m_Vx2));
//
//	_CBasicSetOfDeletions Set2(_CBasicPairOfDeletion(m_Vy1, -m_Vy2, m_Vx1, m_gnd));
//	Set2.push_back(_CBasicPairOfDeletion(m_Vx1, m_Vx2, m_Vx2, m_gnd));
//
//	_CBasicSetOfDeletions Set3(_CBasicPairOfDeletion(m_Vx1, m_gnd, m_Vx1, m_gnd), -1);
//	Set3.push_back(_CBasicPairOfDeletion(m_Vx2, m_gnd, m_Vx2, m_gnd));
//
//	_CBasicSetOfDeletions Set4(_CBasicPairOfDeletion(m_Vx1, m_Vx2, m_Vx1, m_gnd), -1);
//	Set4.push_back(_CBasicPairOfDeletion(m_Iz1, m_Iz2, m_Vx2, m_gnd));
//
//	m_SetDeletion.push_back(Set1);
//	m_SetDeletion.push_back(Set2);
//	m_SetDeletion.push_back(Set3);
//	m_SetDeletion.push_back(Set4);
//}
//
//bool _CFBCCI::IsEqualIfSameType(const _CComponent& RightComp)
//{
//	const _CFBCCI* pDV = dynamic_cast<const _CFBCCI*> (&RightComp);
//	if (pDV == nullptr) return false;
//	return *pDV == *this;
//}

//****************************_CFBCCI*******************************

//****************************_CDVCCII******************************

IMPLEMENT_DYNAMIC_CREATION(_CDVCCII);

_CDVCCII::_CDVCCII(_CCircuit* pOwner, int x, int y1, int y2, int z1, int z2, int gnd) : 
_CMataPathologicComponent(pOwner), m_x(x), m_y1(y1), m_y2(y2), m_z1(z1), m_z2(z2), m_gnd(gnd)
{
	PrepareDeletions();
}

_CDVCCII::_CDVCCII(const _CDVCCII& Source) : 
_CMataPathologicComponent(Source.m_pBaseCircuit), m_x(Source.m_x), m_y1(Source.m_y1), m_y2(Source.m_y2), m_z1(Source.m_z1), m_z2(Source.m_z2)
{
	PrepareDeletions();
}

void _CDVCCII::PrepareDeletions()
{
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, m_gnd, m_x, m_gnd), -1));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_z1, m_z2, m_x, m_gnd), -1));

	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_x, m_gnd, m_y1, m_y2)));
	m_SetDeletion.push_back(_CBasicSetOfDeletions(_CBasicPairOfDeletion(m_z1, m_z2, m_y1, m_y2)));
}

long _CDVCCII::GetHash(long Core ) const
{
	Core *= HASH_FACTOR;
	Core ^= m_x;
	Core *= HASH_FACTOR;
	Core ^= m_y1;
	Core *= HASH_FACTOR;
	Core ^= m_y2;
	Core *= HASH_FACTOR;
	Core ^= m_z1;
	Core *= HASH_FACTOR;
	Core ^= m_z2;
	Core *= HASH_FACTOR;
	Core ^= m_gnd;
	return Core;
}
void _CDVCCII::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_x);
	Numbers2Exchange.ExchangeIfNeede(m_y1);
	Numbers2Exchange.ExchangeIfNeede(m_y2);
	Numbers2Exchange.ExchangeIfNeede(m_z1);
	Numbers2Exchange.ExchangeIfNeede(m_z2);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CDVCCII::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(6);
	Pins[0] = m_x;
	Pins[1] = m_y1;
	Pins[2] = m_y2;
	Pins[3] = m_z1;
	Pins[4] = m_z2;
	Pins[5] = m_gnd;
}

void _CDVCCII::WritePos(iostream& stream)
{
	stream << "(I_x = " << m_pBaseCircuit->ConvertNode(m_x) <<
		", I_y1 = " << m_pBaseCircuit->ConvertNode(m_y1) <<
		", I_y2 = " << m_pBaseCircuit->ConvertNode(m_y2) <<
		", I_z1 = " << m_pBaseCircuit->ConvertNode(m_z1) <<
		", I_z2 = " << m_pBaseCircuit->ConvertNode(m_z2) << ")";
}

bool _CDVCCII::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CDVCCII* pDV = dynamic_cast<const _CDVCCII*> (&RightComp);
	if (pDV == nullptr) return false;
	return m_x == pDV->m_x && m_y1 == pDV->m_y1 && m_y2 == pDV->m_y2 && m_z1 == pDV->m_z1 && m_z2 == pDV->m_z2;
}


void _CDVCCII::Store(_binary_filer& Filer)
{
	Filer << m_x;
	Filer << m_y1;
	Filer << m_y2;
	Filer << m_z1;
	Filer << m_z2;
}

void _CDVCCII::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_x;
	Filer >> m_y1;
	Filer >> m_y2;
	Filer >> m_z1;
	Filer >> m_z2;
	PrepareDeletions();
}

//****************************_CDVCCII**********************************

//****************************_CFDCCII**********************************

_CFDCCII::_CFDCCII(_CCircuit* pOwner, int x1, int x2, int y1, int y2, int y3, int y4, int z1, int z2, int gnd) :
_CMataPathologicComponent(pOwner), m_x1(x1), m_x2(x2), m_y1(y1), m_y2(y2), m_y3(y3), m_y4(y4), m_z1(z1), m_z2(z2), m_gnd(gnd)
{
	PrepareDeletions();
}

_CFDCCII::_CFDCCII(const _CFDCCII& Source) : 
_CMataPathologicComponent(Source.m_pBaseCircuit), m_x1(Source.m_x1), m_x2(Source.m_x2),
m_y1(Source.m_y1), m_y2(Source.m_y2), m_y3(Source.m_y3), m_y4(Source.m_y4), 
m_z1(Source.m_z1), m_z2(Source.m_z2)
{
	PrepareDeletions();
}

void _CFDCCII::PrepareDeletions()
{
	_CBasicSetOfDeletions Set;
	Set.push_back(_CBasicPairOfDeletion(m_z1, -m_x1, m_y1, m_y2));
	Set.push_back(_CBasicPairOfDeletion(m_z2, -m_x2, m_x1, -m_x2));
	m_SetDeletion.push_back(Set);
	Set.clear();
	Set.push_back(_CBasicPairOfDeletion(m_z1, -m_x1, m_y1, m_y2));
	Set.push_back(_CBasicPairOfDeletion(m_z2, -m_x2, m_y3, -m_y4));
	m_SetDeletion.push_back(Set);
	Set.clear();
	Set.push_back(_CBasicPairOfDeletion(m_z1, -m_x1, m_x1, -m_y3));
	Set.push_back(_CBasicPairOfDeletion(m_z2, -m_x2, m_x2, -m_y4));
	m_SetDeletion.push_back(Set);
}

long _CFDCCII::GetHash(long Core) const
{
	Core *= HASH_FACTOR;
	Core ^= m_x1;
	Core *= HASH_FACTOR;
	Core ^= m_x2;
	Core *= HASH_FACTOR;
	Core ^= m_y1;
	Core *= HASH_FACTOR;
	Core ^= m_y2;
	Core *= HASH_FACTOR;
	Core ^= m_y3;
	Core *= HASH_FACTOR;
	Core ^= m_y4;
	Core *= HASH_FACTOR;
	Core ^= m_z1;
	Core *= HASH_FACTOR;
	Core ^= m_z2;
	Core *= HASH_FACTOR;
	Core ^= m_gnd;
	return Core;
}
void _CFDCCII::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	Numbers2Exchange.ExchangeIfNeede(m_x1);
	Numbers2Exchange.ExchangeIfNeede(m_x2);
	Numbers2Exchange.ExchangeIfNeede(m_y1);
	Numbers2Exchange.ExchangeIfNeede(m_y2);
	Numbers2Exchange.ExchangeIfNeede(m_y3);
	Numbers2Exchange.ExchangeIfNeede(m_y4);
	Numbers2Exchange.ExchangeIfNeede(m_z1);
	Numbers2Exchange.ExchangeIfNeede(m_z2);
	Numbers2Exchange.ExchangeIfNeede(m_gnd);
	m_SetDeletion.clear();
	PrepareDeletions();
}

void _CFDCCII::RaportConnections(COMPONENT_PINS& Pins) const
{
	Pins.resize(9);
	Pins[0] = m_x1;
	Pins[1] = m_x2;
	Pins[2] = m_y1;
	Pins[3] = m_y2;
	Pins[4] = m_y3;
	Pins[5] = m_y4;
	Pins[6] = m_z1;
	Pins[7] = m_z2;
	Pins[8] = m_gnd;
}

void _CFDCCII::WritePos(iostream& stream)
{
	stream << "(V_x1 = " << m_pBaseCircuit->ConvertNode(m_x1) <<
		", V_x2 = " << m_pBaseCircuit->ConvertNode(m_x2) <<
		", V_y1 = " << m_pBaseCircuit->ConvertNode(m_y1) <<
		", V_y2 = " << m_pBaseCircuit->ConvertNode(m_y2) <<
		", V_y3 = " << m_pBaseCircuit->ConvertNode(m_y3) <<
		", V_y4 = " << m_pBaseCircuit->ConvertNode(m_y4) <<
		", I_z1 = " << m_pBaseCircuit->ConvertNode(m_z1) <<
		", I_z2 = " << m_pBaseCircuit->ConvertNode(m_z2) << ")";
}

bool _CFDCCII::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CFDCCII* pDV = dynamic_cast<const _CFDCCII*> (&RightComp);
	if (pDV == nullptr) return false;
	return m_x1 == pDV->m_x1 && m_x2 == pDV->m_x2 && 
		m_y1 == pDV->m_y1 && m_y2 == pDV->m_y2 && m_y3 == pDV->m_y3 && m_y4 == pDV->m_y4 &&
		m_z1 == pDV->m_z1 && m_z2 == pDV->m_z2;
}


void _CFDCCII::Store(_binary_filer& Filer)
{
	Filer << m_x1;
	Filer << m_x2;
	Filer << m_y1;
	Filer << m_y2;
	Filer << m_y3;
	Filer << m_y4;
	Filer << m_z1;
	Filer << m_z2;
}

void _CFDCCII::Load(_binary_filer& Filer)
{
	m_SetDeletion.clear();
	Filer >> m_x1;
	Filer >> m_x2;
	Filer >> m_y1;
	Filer >> m_y2;
	Filer >> m_y3;
	Filer >> m_y4;
	Filer >> m_z1;
	Filer >> m_z2;
	PrepareDeletions();
}

//****************************_CFDCCII**********************************

//****************************_CMultiPortOneParam***********************

_CMultiPortOneParam::_CMultiPortOneParam(_CCircuit* pOwnerCircuit, string Name, const vector<int>& Ports, long double Value):
_CSimplyComponent(pOwnerCircuit, Name, Value), m_Ports(Ports)
{
}

long _CMultiPortOneParam::GetHash(long Core) const
{
	Core = _CSimplyComponent::GetHash(Core);
	for (auto Pin : m_Ports)
	{
		Core *= HASH_FACTOR;
		Core ^= Pin;
	}
	return Core;
}

void _CMultiPortOneParam::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	for (auto& Pin : m_Ports)
		Numbers2Exchange.ExchangeIfNeede(Pin);
	PrepareDeletions();
}

void _CMultiPortOneParam::GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	if (m_0Deletions.empty() && m_1Deletions.empty())
		PrepareDeletions();
	switch (Descendant)
	{
	case 0:
		pDeletions=&m_0Deletions;
		break;
	case 1:
		pDeletions=&m_1Deletions;
		break;
	default:
		RISEPDD(eIllegalOperation,"There are only 2 descendants");
	}
}
void _CMultiPortOneParam::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	if (m_0Deletions.empty() && m_1Deletions.empty())
		PrepareDeletions();
	int Comp = m_0Deletions.front().size() - m_1Deletions.front().size();
	switch (ImportanceLevel)
	{
	case 0:
		pDeletions = Comp < 0 ? &m_1Deletions : &m_0Deletions;
		break;
	case 1:
		pDeletions = Comp < 0 ? &m_0Deletions : &m_1Deletions;
		break;
	default:
		RISEPDD(eIllegalOperation, "There are only 2 descendants");
	}
}

void _CMultiPortOneParam::Load(_binary_filer& Filer)
{
	_CSimplyComponent::Load(Filer);
	size_t Size = m_Ports.size();
	Filer << Size;
	for (size_t i = 0; i < Size; i++)
		Filer << m_Ports[i];
}

void _CMultiPortOneParam::Store(_binary_filer& Filer)
{
	_CSimplyComponent::Store(Filer);
	size_t Size;
	Filer >> Size;
	m_Ports.resize(Size);
	for (size_t i = 0; i < Size; i++)
		Filer >> m_Ports[i];
}

//****************************_CMultiPortOneParam***********************

//****************************_CFDCFOA_RM*******************************
IMPLEMENT_DYNAMIC_CREATION(_CFDCFOA_RM);


_CFDCFOA_RM::_CFDCFOA_RM(_CCircuit* pOwnerCircuit, string Name, int Vyp, int Vyn, int Ixp, int Ixn, int Izp, int Izn, int Vwp, int Vwn, long double Value) :
_CMultiPortOneParam(pOwnerCircuit, Name, { Vyp, Vyn, Ixp, Ixn, Izp, Izn, Vwp,  Vwn }, Value)
{
}

void _CFDCFOA_RM::PrepareDeletions()
{
	m_1Deletions.clear();
	m_0Deletions.clear();
	int m_Vyp = Vyp();
	int m_Vyn = Vyn();
	int m_Ixp = Ixp();
	int m_Ixn = Ixn();
	int m_Izp = Izp();
	int m_Izn = Izn();
	int m_Vwp = Vwp();
	int m_Vwn = Vwn();

	_CBasicSetOfDeletions Set;
	Set.push_back(_CBasicPairOfDeletion(m_Ixn, m_Ixp, m_Vyp, m_Ixp));
	Set.push_back(_CBasicPairOfDeletion(m_Izp, m_Ixp, m_Vyn, m_Ixn));
	Set.push_back(_CBasicPairOfDeletion(m_Vwp, m_Ixp, m_Vwp, m_Izp));
	Set.push_back(_CBasicPairOfDeletion(m_Vwn, m_Ixp, m_Vwn, m_Izn));
	m_1Deletions.push_back(Set);

	Set.clear();
	Set.push_back(_CBasicPairOfDeletion(m_Ixn, m_Ixp, m_Vyp, m_Ixp));
	Set.push_back(_CBasicPairOfDeletion(m_Izn, m_Ixp, m_Vyn, m_Ixn));
	Set.push_back(_CBasicPairOfDeletion(m_Vwp, m_Ixp, m_Vwp, m_Izp));
	Set.push_back(_CBasicPairOfDeletion(m_Vwn, m_Ixp, m_Vwn, m_Izn));
	m_1Deletions.push_back(Set);

	Set.clear();
	Set.MultSgn(-1);
	Set.push_back(_CBasicPairOfDeletion(m_Izp, m_Ixp, m_Vyp, m_Ixp));
	Set.push_back(_CBasicPairOfDeletion(m_Izn, m_Ixp, m_Vyn, m_Ixn));
	Set.push_back(_CBasicPairOfDeletion(m_Vwp, m_Ixp, m_Vwp, m_Izp));
	Set.push_back(_CBasicPairOfDeletion(m_Vwn, m_Ixp, m_Vwn, m_Izn));
	m_1Deletions.push_back(Set);

	Set.clear();
	Set.push_back(_CBasicPairOfDeletion(m_Izp, m_Ixn, m_Vyp, m_Ixp));
	Set.push_back(_CBasicPairOfDeletion(m_Izn, m_Ixn, m_Vyn, m_Ixn));
	Set.push_back(_CBasicPairOfDeletion(m_Vwp, m_Ixn, m_Vwp, m_Izp));
	Set.push_back(_CBasicPairOfDeletion(m_Vwn, m_Ixn, m_Vwn, m_Izn));
	m_1Deletions.push_back(Set);

	Set.Clear();
	//(V_X_n+V_X_p)(V_Y_p+V_X_p)
	Set.push_back(_CBasicPairOfDeletion(m_Ixn, m_Ixp, m_Vyp, m_Ixp));
	//(I_Z_p+V_X_p)(V_Y_n+V_X_n)
	Set.push_back(_CBasicPairOfDeletion(m_Izp, m_Ixp, m_Vyn, m_Ixn));
	//(I_Z_n+V_X_p)(I_Z_n+I_Z_p)
	Set.push_back(_CBasicPairOfDeletion(m_Izn, m_Ixp, m_Izn, m_Izp));
	//(V_W_p+V_X_p)(V_W_p+I_Z_p)
	Set.push_back(_CBasicPairOfDeletion(m_Vwp, m_Ixp, m_Vwp, m_Izp));
	//(V_W_n+V_X_p)(V_W_n+I_Z_p)
	Set.push_back(_CBasicPairOfDeletion(m_Vwn, m_Ixp, m_Vwn, m_Izp));
	m_0Deletions.push_back(Set);
}

void _CFDCFOA_RM::WritePos(iostream& stream)
{
	stream << "(V_y_p = " << m_pBaseCircuit->ConvertNode(Vyp()) <<
		", V_y_n = " << m_pBaseCircuit->ConvertNode(Vyn()) <<
		", I_x_p = " << m_pBaseCircuit->ConvertNode(Ixp()) <<
		", I_x_n = " << m_pBaseCircuit->ConvertNode(Ixn()) <<
		", I_z_p = " << m_pBaseCircuit->ConvertNode(Izp()) <<
		", I_z_n = " << m_pBaseCircuit->ConvertNode(Izn()) <<
		", V_w_p = " << m_pBaseCircuit->ConvertNode(Vwp()) <<
		", V_w_n = " << m_pBaseCircuit->ConvertNode(Vwn()) << ")";
}

void _CFDCFOA_RM::WriteType(iostream& stream)
{
	stream << "Fully-Differential Current Reedback Operational Transimpedance Amplifier with Rm:";
}

bool _CFDCFOA_RM::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CFDCFOA_RM* pDV = dynamic_cast<const _CFDCFOA_RM*> (&RightComp);
	if (pDV == nullptr) return false;
	return *this == *pDV;
}


//****************************_CFDCFOA_RM*******************************

//****************************_CFDOTRA**********************************
IMPLEMENT_DYNAMIC_CREATION(_CFDOTRA);


_CFDOTRA::_CFDOTRA(_CCircuit* pOwnerCircuit, string Name, int Ip, int In, int Vp, int Vn, int gnd, long double Value) :
_CMultiPortOneParam(pOwnerCircuit, Name, {gnd, Ip, In, Vp, Vn }, Value)
{
}

void _CFDOTRA::PrepareDeletions()
{
	m_1Deletions.clear();
	m_0Deletions.clear();
	int Gnd = gnd();
	int IP = Ip();
	int IN = In();
	int VP = Vp();
	int VN = Vn();
	_CBasicSetOfDeletions Set(_CBasicPairOfDeletion(IP,-IN,IP,Gnd));
	Set.push_back(_CBasicPairOfDeletion(VP, Gnd, IN, Gnd));
	Set.push_back(_CBasicPairOfDeletion(VN, Gnd, VP, -VN));
	m_1Deletions.push_back(Set);
	Set.clear();
	Set.push_back(_CBasicPairOfDeletion(IP, Gnd, IP, Gnd));
	Set.push_back(_CBasicPairOfDeletion(IN, Gnd, IN, Gnd));
	Set.push_back(_CBasicPairOfDeletion(VP, Gnd, VP, Gnd));
	Set.push_back(_CBasicPairOfDeletion(VN, Gnd, VN, Gnd));

	m_0Deletions.push_back(Set);
}

void _CFDOTRA::WritePos(iostream& stream)
{
	stream << "(I_p = " << m_pBaseCircuit->ConvertNode(Ip()) <<
		", I_n = " << m_pBaseCircuit->ConvertNode(In()) <<
		", V_p = " << m_pBaseCircuit->ConvertNode(Vp()) <<
		", V_n = " << m_pBaseCircuit->ConvertNode(Vn()) <<
		", Gnd = " << m_pBaseCircuit->ConvertNode(gnd()) << ")";
}

void _CFDOTRA::WriteType(iostream& stream)
{
	stream << "Fully-Differential Operational Transimpedance Amplifier :";
}

bool _CFDOTRA::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CFDOTRA* pDV = dynamic_cast<const _CFDOTRA*> (&RightComp);
	if (pDV == nullptr) return false;
	return *this==*pDV;
}


//****************************_CFDOTRA**********************************

//****************************_COTRA************************************
IMPLEMENT_DYNAMIC_CREATION(_COTRA);


_COTRA::_COTRA(_CCircuit* pOwnerCircuit, string Name, int Ip, int In, int Vp, int gnd, long double Value) :
_CMultiPortOneParam(pOwnerCircuit, Name, { gnd, Ip, In, Vp }, Value)
{
}

void _COTRA::PrepareDeletions()
{
	m_1Deletions.clear();
	m_0Deletions.clear();
	int Gnd = gnd();
	int IP = Ip();
	int IN = In();
	int VP = Vp();
	_CBasicSetOfDeletions Set(_CBasicPairOfDeletion(IP, -IN, IP, Gnd));
	Set.push_back(_CBasicPairOfDeletion(VP, Gnd, IN, Gnd));
	m_1Deletions.push_back(Set);
	Set.clear();
	Set.push_back(_CBasicPairOfDeletion(IP, Gnd, IP, Gnd));
	Set.push_back(_CBasicPairOfDeletion(IN, Gnd, IN, Gnd));
	Set.push_back(_CBasicPairOfDeletion(VP, Gnd, VP, Gnd));

	m_0Deletions.push_back(Set);
}

void _COTRA::WritePos(iostream& stream)
{
	stream << "(I_p = " << m_pBaseCircuit->ConvertNode(Ip()) <<
		", I_n = " << m_pBaseCircuit->ConvertNode(In()) <<
		", V_p = " << m_pBaseCircuit->ConvertNode(Vp()) <<
		", Gnd = " << m_pBaseCircuit->ConvertNode(gnd()) << ")";
}

void _COTRA::WriteType(iostream& stream)
{
	stream << "Operational Transimpedance Amplifier :";
}

bool _COTRA::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _COTRA* pDV = dynamic_cast<const _COTRA*> (&RightComp);
	if (pDV == nullptr) return false;
	return *this == *pDV;
}

//****************************_COTRA************************************
