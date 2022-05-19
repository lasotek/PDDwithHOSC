#pragma once
#include "stdafx.h"
//#include <vector>
//#include <unordered_map>

using namespace std;
using namespace stdext;


template<class ResTableType>
class _CAbstractResTables
{
public:
	~_CAbstractResTables()
	{
	}
	void ClearAll() {m_Map.ClearAll();}
	const ResTableType* smart_at(const string& Context,unsigned long long CofId, unsigned short sPower) const
	{
		const _CSCoffTab* pCoffTab=m_Map.smart_at(&Context);
		if(pCoffTab==NULL)
			return NULL;
		const _CSCoeffsTab* pCoeffsTab=pCoffTab->smart_at((size_t)CofId);
		if(pCoeffsTab==NULL)
			return NULL;
		return pCoeffsTab->smart_at(sPower);
	}
	ResTableType& force_at(const string& Context,unsigned long long CofId, unsigned short sPower)
	{
		return m_Map[&Context].force_at((size_t)CofId).force_at((size_t)sPower);
	}
	//ResTableType& GetResult(const string& Context,unsigned long long CofId, unsigned short sPower)
	//{
	//	return m_Map[&Context].force_at((size_t)CofId).force_at((size_t)sPower);
	//}
protected:
	//<_CSCoeffsTab
	class _CSCoeffsTab : public self_exp_vector<ResTableType>
	{
	public:
	};
	//_CSCoeffsTab>
	//<_CSCoffTab
	class _CSCoffTab : public self_exp_vector<_CSCoeffsTab>
	{
	public:
	};
	//_CSCoffTab>
	//<_CContextMap
	class _CContextMap : public unordered_map<const string*,_CSCoffTab>
	{
	public:
		const _CSCoffTab* smart_at(const string* pContext) const
		{
			const_iterator it=find(pContext);
			if(it==end())
				return NULL;
			else
				return &(it->second);
		}
		void ClearAll()
		{
			for(iterator it=begin();it!=end();it++)
				it->second.ClearAll();
		}
	};
	//_CContextMap>

	_CContextMap m_Map;
public:
	_CSCoffTab& EnsureMap(const string& Context)
	{
		return m_Map[&Context];
	}
};

template<class NumType>
class _CNumericalContainer
{
public:
	~_CNumericalContainer()
	{
	}
	_CNumericalContainer():m_IsValid(false), m_Value(0) {}
	_CNumericalContainer(const NumType& Value):m_Value(Value),m_IsValid(true) {} 
	_CNumericalContainer(const _CNumericalContainer& Source): m_IsValid(Source.m_IsValid),
		m_Value(Source.m_Value) {}
	_CNumericalContainer& operator=(const NumType& Value)
	{
		m_Value=Value;
		m_IsValid=true;
		return *this;
	}
	_CNumericalContainer& operator=(const _CNumericalContainer& Source)
	{
		m_Value=Source.m_Value;
		m_IsValid=Source.m_IsValid;
		return *this;
	}
	operator NumType() const {return m_Value;}
	bool IsEmpty() const {return !m_IsValid;}
	void ClearAll() {m_IsValid=false;}
protected:
	bool m_IsValid;
	NumType m_Value;
};

template<class NumType>
class _CNumResTable : public _CAbstractResTables<_CNumericalContainer<NumType>>
{
public:
	~_CNumResTable()
	{
	}
};

