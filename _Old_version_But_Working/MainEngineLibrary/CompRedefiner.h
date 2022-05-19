#pragma once
#include<memory>
#include"Components.h"
#include "ComponentPath.h"
#include"norm_float.h"
#include <limits>

class _CCompRedefEntry;
typedef shared_ptr<_CCompRedefEntry> _CCompRedefEntryShrdPtr;

class _CModelVertex;
typedef list<const _CModelVertex*> MVerticesList;


//class _CCompEntryOrdinator
//{
//public:
//	_CCompEntryOrdinator();
//	virtual ~_CCompEntryOrdinator();
//	size_t GetMyId() const;
//	static size_t non_register_id() { return numeric_limits<size_t>::max(); }
//protected:
//	void RegisterMe();
//	static size_t m_Counter;
//	typedef unordered_map<size_t, _CCompEntryOrdinator*> ID2ENTRY;
//	typedef unordered_map<const _CCompEntryOrdinator*, size_t> ENTRY2ID;
//	static ID2ENTRY m_Id2Entry;
//	static ENTRY2ID m_Entry2Id;
//};

class _CCompRedefEntry /*: public _CCompEntryOrdinator*/
{
public:
	_CCompRedefEntry(_CCircuit* pOwner, _CCompRedefEntry* pTopEntry);
	virtual ~_CCompRedefEntry();
	_CCompRedefEntry(_CComponent* pComponent, NumericType Value, _CCompRedefEntry* pTopEntry);
	_CCompRedefEntry(const _CComponentPath& CompPath, NumericType Value, _CCompRedefEntry* pTopEntry);
	_CCompRedefEntry(const _CCompRedefEntry& Source, _CCompRedefEntry* pTopEntry = nullptr);
	_CCompRedefEntry& operator=(const _CCompRedefEntry& Source);
	_CCompRedefEntry& operator[](const _CComponent* pComp);
	_CCompRedefEntry& operator[](const _CModelVertex* pModelVertex);
	_CCompRedefEntry& operator[](const MVerticesList& List);
	bool IsAbstract() const { return m_pNumericPatern == nullptr && m_pSubMap == nullptr; }
	bool IsNumeric() const { return m_pNumericPatern != nullptr && m_pSubMap == nullptr; }
	bool IsSubList() const { return m_pNumericPatern == nullptr && m_pSubMap != nullptr; }
	void WeekMergeSubtree(const _CCompRedefEntry& SubtreeRoot);
	const _CSubModelNumericPattern& GetPaternValue();
	void AddRedefValue(const _CComponentPath& CompPath, NumericType Value);
	void SetRedefValue(_CComponent* pComponent, NumericType Value);
	typedef _CComponentPath::PATH LIST2COMPONENT;
	_CCompRedefEntry(LIST2COMPONENT& CompPath, NumericType Value, _CCircuit* pOwner, _CCompRedefEntry* pTopEntry);
	_CSubModelNumericPattern* GetNumericPattern();
	const NumericType& GetNumericValue();
	const NumericType& GetcompNumericValue(const _CComponent* pComp);
	long long GetHash(long long Core = HASH_CORE) const;
	bool operator==(const _CCompRedefEntry& Right) const;
	bool operator!=(const _CCompRedefEntry& Right) const { return !operator==(Right); }
	void Store(_binary_filer& Filer) const;
	void Load(_binary_filer& Filer);
	void WriteToText(iostream& stream, size_t TabBefore = 0) const;
	bool SkipMerging() { return m_IsMerged || IsNumeric(); }
	const _CCompRedefEntry* GetPtrUniq();
	//const _CCompRedefEntryShrdPtr GetPtrUniq();
	_CCompRedefEntry& MyTopEntry();
	//_CCompRedefEntry* FindFromId(size_t Id) const;
	void AddNumberValueIfNotRedefined(_CComponent* pComponent,const NumericType& Value);
protected:
	typedef pair<NumericType, _CSubModelNumericPattern> NumericPattern;
	typedef unique_ptr<NumericPattern> PNumericPattarn;
	PNumericPattarn m_pNumericPatern;
	//typedef unique_ptr<NumericType> PNumericType;
	//PNumericType m_pNumeric;
	typedef unordered_map<const _CComponent*, _CCompRedefEntryShrdPtr> SUB_MAP;
	typedef unique_ptr<SUB_MAP> PSubMap;
	PSubMap m_pSubMap;
	void AddRedefValue(LIST2COMPONENT& CompPath, NumericType Value);
	_CCircuit* m_pOwner;
	bool m_IsMerged;
	friend class _CCircuit;
	void WriteToTextTab(iostream& stream,size_t NoOfTabs);
	_CCompRedefEntry* m_pSimilarRedefEntry;
	//_CCompRedefEntryShrdPtr m_pSimilarRedefEntry;
	friend class _CUniqCompRedefEntry;
	_CCompRedefEntry* m_pTopEntry;
};

class _CUniqCompRedefEntry
{
public:
	//_CCompRedefEntryShrdPtr GetUniq(_CCompRedefEntryShrdPtr pOrgRedefEntry);
	const _CCompRedefEntry* GetUniq(_CCompRedefEntry* pOrgRedefEntry);
protected:
	//typedef hash_multimap<long long, _CCompRedefEntryShrdPtr> UNQ_MAP;
	typedef unordered_multimap<long long, _CCompRedefEntry*> UNQ_MAP;
	UNQ_MAP m_Map;
};