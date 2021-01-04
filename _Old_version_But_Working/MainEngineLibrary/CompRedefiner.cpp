#include "stdafx.h"
#include "CompRedefiner.h"
#include "SubCircuitSocket.h"
#include "SimplifierEngine.h"

//size_t _CCompEntryOrdinator::m_Counter = 0;
//
//_CCompEntryOrdinator::ID2ENTRY _CCompEntryOrdinator::m_Id2Entry;
//_CCompEntryOrdinator::ENTRY2ID _CCompEntryOrdinator::m_Entry2Id;
//
//_CCompEntryOrdinator::_CCompEntryOrdinator()
//{
//}
//
//_CCompEntryOrdinator::~_CCompEntryOrdinator()
//{
//	auto& fit1 = m_Entry2Id.find(this);
//	if (fit1 == m_Entry2Id.end())
//		return;
//	auto& fit2 = m_Id2Entry.find(fit1->second);
//	m_Entry2Id.erase(fit1);
//	m_Id2Entry.erase(fit2);
//}
//
//void _CCompEntryOrdinator::RegisterMe()
//{
//	m_Entry2Id.insert(ENTRY2ID::value_type(this,++m_Counter));
//	m_Id2Entry.insert(ID2ENTRY::value_type(m_Counter, this));
//}
//
//size_t _CCompEntryOrdinator::GetMyId() const
//{
//	auto& fit1 = m_Entry2Id.find(this);
//	if (fit1 == m_Entry2Id.end())
//		return non_register_id();
//	return fit1->second;
//}

_CCompRedefEntry::_CCompRedefEntry(_CCircuit* pOwner, _CCompRedefEntry* pTopEntry) : /*_CCompEntryOrdinator(), */
m_pOwner(pOwner), m_pTopEntry(pTopEntry), m_IsMerged(false), m_pSimilarRedefEntry(nullptr)
{

}
_CCompRedefEntry::~_CCompRedefEntry()
{

}

_CCompRedefEntry::_CCompRedefEntry(_CComponent* pComponent, NumericType Value, _CCompRedefEntry* pTopEntry) : 
m_IsMerged(false), m_pTopEntry(pTopEntry), m_pSimilarRedefEntry(nullptr)
{
	if (pComponent->IsMacroDependant())
		RISEPDD(eWrongCondition, "Only for SimplyComponents");
	m_pOwner = pComponent->GetOwnerCircuit();
	m_pNumericPatern = make_unique<NumericPattern>();
	m_pNumericPatern->first = Value;
	pComponent->GenerateMyNumPattern(Value, m_pNumericPatern->second);
	//_CSubModelNumericPattern::element_poly LDesc, HDesc;
	//LDesc.put(0, 1);
	//HDesc.put(pComponent->sPower(), Value);
	//m_pNumericPatern->second.put(0, HDesc);
	//m_pNumericPatern->second.put(1, LDesc);

}
void _CCompRedefEntry::SetRedefValue(_CComponent* pComponent, NumericType Value)
{
	ASSERTPDD(m_pSubMap == nullptr);
	if (pComponent->IsMacroDependant())
		RISEPDD(eWrongCondition, "Only for SimplyComponents");
	if (m_pNumericPatern == nullptr)
		m_pNumericPatern = make_unique<NumericPattern>();
	else
		m_pNumericPatern->second.ClearAll();
	ASSERTPDD(m_pOwner == pComponent->GetOwnerCircuit());//?
	m_pNumericPatern->first = Value;
	pComponent->GenerateMyNumPattern(Value, m_pNumericPatern->second);
	//_CSubModelNumericPattern::element_poly LDesc, HDesc;
	//LDesc.put(0, 1);
	//HDesc.put(pComponent->sPower(), Value);
	//m_pNumericPatern->second.put(0, HDesc);
	//m_pNumericPatern->second.put(1, LDesc);
}

_CSubModelNumericPattern* _CCompRedefEntry::GetNumericPattern()
{
	if (IsNumeric())
		return &m_pNumericPatern->second;
	if (IsSubList())
		RISEPDD(eWrongCondition, "Not numeric pattern");
	return nullptr;
}

const NumericType& _CCompRedefEntry::GetNumericValue()
{
	if (IsNumeric())
		return m_pNumericPatern->first;
	RISEPDD(eWrongCondition, "Not numeric value");
}

const NumericType& _CCompRedefEntry::GetcompNumericValue(const _CComponent* pComp)
{
	ASSERTPDD(!IsNumeric());
	_CCompRedefEntry& SupRedef = (*this)[pComp];
	if (SupRedef.IsNumeric())
		return SupRedef.GetNumericValue();
	return pComp->GetParameterValue();
}
const _CSubModelNumericPattern& _CCompRedefEntry::GetPaternValue()
{
	if(!IsNumeric())
		RISEPDD(eWrongCondition, "Value defined only for numeric _CSubModelNumericPattern");
	return m_pNumericPatern->second;
}

_CCompRedefEntry::_CCompRedefEntry(const _CComponentPath& CompPath, NumericType Value, _CCompRedefEntry* pTopEntry) : /*_CCompEntryOrdinator(),*/ m_pOwner(CompPath.GetBaseCircuit()), 
m_IsMerged(false), m_pTopEntry(pTopEntry), m_pSimilarRedefEntry(nullptr)
{
	LIST2COMPONENT MyList(CompPath.GetPathList());
	AddRedefValue(MyList, Value);
}
_CCompRedefEntry::_CCompRedefEntry(LIST2COMPONENT& CompPath, NumericType Value, _CCircuit* pOwner,_CCompRedefEntry* pTopEntry) : 
m_pOwner(pOwner), m_IsMerged(false), m_pTopEntry(pTopEntry), m_pSimilarRedefEntry(nullptr)
{
	AddRedefValue(CompPath, Value);
}
_CCompRedefEntry::_CCompRedefEntry(const _CCompRedefEntry& Source, _CCompRedefEntry* pTopEntry) : /*_CCompEntryOrdinator(),*/ m_IsMerged(Source.m_IsMerged),
m_pSimilarRedefEntry(Source.m_pSimilarRedefEntry), m_pTopEntry(pTopEntry==nullptr?Source.m_pTopEntry:pTopEntry)
{
	m_pOwner = Source.m_pOwner;
	if (Source.m_pNumericPatern != nullptr)
		m_pNumericPatern = make_unique<NumericPattern>(*Source.m_pNumericPatern);
	else if (Source.m_pSubMap != nullptr)
	{
		m_pSubMap = make_unique<SUB_MAP>();
		for (auto& SubEntry : *Source.m_pSubMap)
		{
			m_pSubMap->insert(SUB_MAP::value_type(SubEntry.first, make_unique<_CCompRedefEntry>(*SubEntry.second, this)));
		}
	}
	else
		RISEPDD(eWrongCondition, "CompRedEntry cannot be Numeric and list at the same time");
}

_CCompRedefEntry& _CCompRedefEntry::operator = (const _CCompRedefEntry& Source)
{
	m_pTopEntry = Source.m_pTopEntry; 
	m_pSimilarRedefEntry = Source.m_pSimilarRedefEntry;
	m_IsMerged = Source.m_IsMerged;
	if (Source.m_pNumericPatern != nullptr)
		m_pNumericPatern = make_unique<NumericPattern>(*Source.m_pNumericPatern);
	else if (Source.m_pSubMap != nullptr)
	{
		m_pSubMap = make_unique<SUB_MAP>();
		for (auto& SubEntry : *Source.m_pSubMap)
		{
			m_pSubMap->insert(SUB_MAP::value_type(SubEntry.first, make_unique<_CCompRedefEntry>(*SubEntry.second, this)));
		}
	}
	else
		RISEPDD(eWrongCondition, "CompRedEntry cannot be Numeric and list at the same time");
	return *this;
}

_CCompRedefEntry& _CCompRedefEntry::operator[](const _CComponent* pComp)
{
	if (m_pNumericPatern!=nullptr)
		RISEPDD(eWrongCondition, "Opearotor [] for _CCompRedefEntry list only");
	if (m_pSubMap == nullptr)
		m_pSubMap = make_unique<SUB_MAP>();
	auto fit = m_pSubMap->find(pComp);
	if (fit == m_pSubMap->end())
	{
		_CCompRedefEntry& Res = *m_pSubMap->insert(SUB_MAP::value_type(pComp, make_shared<_CCompRedefEntry>(pComp->GetOwnerCircuit(),this))).first->second;
		//Res.RegisterMe();
		return Res;
	}
	else
		return *fit->second;
}

_CCompRedefEntry& _CCompRedefEntry::operator[](const _CModelVertex* pModelVertex)
{
	return (*this)[pModelVertex->GetPComponent()];
}

_CCompRedefEntry& _CCompRedefEntry::operator[](const MVerticesList& List)
{
	_CCompRedefEntry* pResEntry = this;
	for (auto pVertex : List)
		pResEntry = &(*pResEntry)[pVertex];
	return *pResEntry;
}

void _CCompRedefEntry::AddRedefValue(const _CComponentPath& CompPath, NumericType Value) 
{
	LIST2COMPONENT MyList(CompPath.GetPathList());
	AddRedefValue(MyList, Value);
}

void _CCompRedefEntry::AddRedefValue(LIST2COMPONENT& CompPath, NumericType Value)
{
	ASSERTPDD(m_pNumericPatern == nullptr);
	if (m_pSubMap == nullptr)
		m_pSubMap = make_unique<SUB_MAP>();
	_CComponent* pComp = CompPath.front();
	if (pComp->IsMacroDependant())
	{
		CompPath.pop_front();
		auto fit = m_pSubMap->find(pComp);
		if (fit == m_pSubMap->end())
			m_pSubMap->insert(SUB_MAP::value_type(pComp, make_shared<_CCompRedefEntry>(CompPath, Value,dynamic_cast<_CSubCircuitSocket*>(pComp)->GetModelCircuit(),this)))/*.first->second->RegisterMe()*/;
		else
			fit->second->AddRedefValue(CompPath, Value);
		CompPath.push_front(pComp);
	}
	else
	{
		auto fit = m_pSubMap->find(pComp);
		if (fit == m_pSubMap->end())
			m_pSubMap->insert(SUB_MAP::value_type(pComp, make_shared<_CCompRedefEntry>(pComp, Value,this)))/*.first->second->RegisterMe()*/;
		else
			RISEPDD(eWrongCondition, (string("The same value ") + dynamic_cast<_CSimplyComponent*>(pComp)->GetParameterName() + " modified ath the same level").c_str());
	}
}

void _CCompRedefEntry::WeekMergeSubtree(const _CCompRedefEntry& SubtreeRoot)
{
	if (m_IsMerged || IsNumeric())
		return;
	ASSERTPDD(!SubtreeRoot.IsNumeric());
	if (SubtreeRoot.IsSubList())
	{
		if (m_pSubMap == nullptr)
			m_pSubMap = make_unique<SUB_MAP>();
		for (auto& ListEntry : (*SubtreeRoot.m_pSubMap))
		{
			const _CComponent* pSubElement = ListEntry.first;
			SUB_MAP::iterator fit = m_pSubMap->find(pSubElement);
			if (fit == m_pSubMap->end())
				m_pSubMap->insert(SUB_MAP::value_type(pSubElement, make_shared<_CCompRedefEntry>(*ListEntry.second, this)));
			else if (pSubElement->IsMacroDependant())
				fit->second->WeekMergeSubtree(*ListEntry.second);
		}
	}
	m_IsMerged = true;
}

long long _CCompRedefEntry::GetHash(long long Core) const
{
	if (IsAbstract())
		return Core;
	if (IsNumeric())
	{
		return GetTypedHash(Core, m_pNumericPatern->second);
	}
	for (auto& SubEntry : *m_pSubMap)
	{
		Core *= HASH_CORE; 
		Core ^= SubEntry.first->GetMyIndex();
		Core = SubEntry.second->GetHash(Core);
	}
	return Core;
}

bool _CCompRedefEntry::operator == (const _CCompRedefEntry& Right) const
{
	if (IsAbstract() && Right.IsAbstract())
		return true;
	if (m_pNumericPatern != nullptr && Right.m_pNumericPatern != nullptr)
		return (*m_pNumericPatern) == (*Right.m_pNumericPatern);
	if (m_pSubMap != nullptr && Right.m_pSubMap != nullptr)
	{
		if (m_pSubMap->size() != Right.m_pSubMap->size())
			return false;
		auto& LIt = m_pSubMap->begin(), &_e=m_pSubMap->end();
		auto& RIt = Right.m_pSubMap->begin();
		for (; LIt != _e; LIt++, RIt++)
		{
			if (LIt->first != RIt->first)
				return false;
			if (*LIt->second != *RIt->second)
				return false;
		}
		return true;
	}
	return false;
}

void _CCompRedefEntry::Store(_binary_filer& Filer) const
{
	Filer << m_IsMerged;
	Filer << m_pOwner->GetMyId();
	Filer << IsNumeric();
	if (IsNumeric())
	{
		Filer << m_pNumericPatern->first;
		Filer << m_pNumericPatern->second;
	}
	Filer << IsSubList();
	if (IsSubList())
	{
		Filer << m_pSubMap->size();
		for (auto& Entry : *m_pSubMap)
		{
			Filer << Entry.first->GetMyIndex();
			Entry.second->Store(Filer);
		}
	}
}

void _CCompRedefEntry::Load(_binary_filer& Filer)
{
	long CirId;
	Filer >> m_IsMerged;
	Filer >> CirId;
	_CCircuit* pMyRealCir = m_pOwner->GetCircuit(CirId);
	m_pOwner = pMyRealCir;
	bool IsNumeric = false;
	Filer >> IsNumeric;
	if (IsNumeric)
	{
		if (m_pNumericPatern == nullptr)
			m_pNumericPatern = make_unique<NumericPattern>();
		Filer >> m_pNumericPatern->first;
		Filer >> m_pNumericPatern->second;
	}
	else
		m_pNumericPatern.reset();
	bool IsSubList = false;
	Filer >> IsSubList;
	if (IsSubList)
	{
		if (m_pSubMap == nullptr)
			m_pSubMap = make_unique<SUB_MAP>();
		else
			m_pSubMap->clear();
		size_t Size = 0;
		Filer >> Size;
		for (size_t i = 0; i < Size; i++)
		{
			long CompId = 0;
			Filer >> CompId;
			_CComponent* pComp = m_pOwner->GetComponentFromIndex(CompId);
			_CCompRedefEntry& NewEntry=*m_pSubMap->insert(SUB_MAP::value_type(pComp, make_shared<_CCompRedefEntry>(m_pOwner,this))).first->second;
			NewEntry.Load(Filer);
		}
	}
	else
		m_pSubMap.reset();
}
void _CCompRedefEntry::WriteToText(iostream& stream, size_t TabBefore) const
{
	if (IsAbstract())
		stream << " has default value\n";
	else if (IsNumeric())
		stream << " = " << m_pNumericPatern->first << "\n";
	else if (IsSubList())
	{
		for (auto& Entry : *m_pSubMap)
		{
			for (size_t i = 0; i < TabBefore; i++) stream << "\t";
			const _CComponent* pComp = Entry.first;
			stream << pComp->GetParameterName();
			if (pComp->IsMacroDependant()) stream << ".\t";
			Entry.second->WriteToText(stream, TabBefore + 1);
		}
	}
	else
	RISEPDD(eWrongCondition, "Illegal _CCompRedefEntry");
}

//const _CCompRedefEntryShrdPtr _CCompRedefEntry::GetPtrUniq()
const _CCompRedefEntry* _CCompRedefEntry::GetPtrUniq()
{
	if (m_pSimilarRedefEntry != nullptr)
		return m_pSimilarRedefEntry;
	else
		return nullptr;
	//return _CCompRedefEntryShrdPtr(this);
}

_CCompRedefEntry& _CCompRedefEntry::MyTopEntry()
{
	if (m_pTopEntry == nullptr)
		RISEPDD(eWrongCondition, "Main RedefEntry has no top value");
	return *m_pTopEntry;
}

void _CCompRedefEntry::AddNumberValueIfNotRedefined(_CComponent* pComponent, const NumericType& Value)
{
	if (IsSubList())
		RISEPDD(eWrongCondition, "Lists only for SubModels available");
	if (IsNumeric())
		return;
	SetRedefValue(pComponent, Value);
}

//_CCompRedefEntry* _CCompRedefEntry::FindFromId(size_t Id) const
//{
//	auto& fit = m_Id2Entry.find(Id);
//	if (fit == m_Id2Entry.end())
//		RISEPDD(eWrongCondition,"Wrong CompRedef Id");
//	return dynamic_cast<_CCompRedefEntry*>(fit->second);
//}

//_CCompRedefEntryShrdPtr _CUniqCompRedefEntry::GetUniq(_CCompRedefEntryShrdPtr pOrgRedefEntry)
const _CCompRedefEntry* _CUniqCompRedefEntry::GetUniq(_CCompRedefEntry* pOrgRedefEntry)
{
	const _CCompRedefEntry* pUniq = pOrgRedefEntry->GetPtrUniq();
	if (pUniq != nullptr)
		return pUniq;
	long long Hash = pOrgRedefEntry->GetHash();
	auto& Range = m_Map.equal_range(Hash);
	for (auto it = Range.first; it != Range.second; it++)
		if (*it->second == *pOrgRedefEntry)
		{
			pOrgRedefEntry->m_pSimilarRedefEntry = it->second;
			return it->second;
		}
	return m_Map.insert(UNQ_MAP::value_type(Hash, pOrgRedefEntry))->second;
}
