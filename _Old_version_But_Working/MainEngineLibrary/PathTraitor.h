#pragma once

#include "stdafx.h"
//#include <set>
//#include <utility>
//#include "ComponentPath.h"
//#include "Circuit.h"
#include "Components.h"
#include "SubCircuitSocket.h"
//#include <set>
//#include <limits.h>
//#include <float.h>
//#include "MainCircuit.h"
#include <ostream>

using namespace std;
using namespace stdext;

class _CApproxCriterion
{
public:
	_CApproxCriterion(bool Exact,
					  const NumericType& Accurracy=DBL_EPSILON,
		              const NumericType& HighFrequency=1e6, 
					  const NumericType& LowFrequency=0.0,
					  short MaxPower=numeric_limits<short>::max(),
					  short MinPower=0): 
						    m_Accurracy(Accurracy),
							m_HighFreguency(HighFrequency),
							m_LowFrequency(LowFrequency),
							m_MaxPower(MaxPower),
							m_MinPower(MinPower),
							m_Exact(Exact) {}
	_CApproxCriterion(const _CApproxCriterion& Source): 
							m_Accurracy(Source.m_Accurracy),
							m_HighFreguency(Source.m_HighFreguency),
							m_LowFrequency(Source.m_LowFrequency),
							m_MaxPower(Source.m_MaxPower),
							m_MinPower(Source.m_MinPower),
							m_Exact(Source.m_Exact) {}
	const NumericType& GetAccurracy() const {return m_Accurracy;}
	const NumericType& GetLowFrequency() const {return m_LowFrequency;}
	const NumericType& GetHighFrequency() const {return m_HighFreguency;}
	short GetMaxPower() const {return m_MaxPower;}
	short GetMinPower() const {return m_MinPower;}
	string GetContext() const;
	string GetDescString() const;
	void WriteToTextFile(txt_stream& stream) const;
	bool IsExact() const {return m_Exact;}
protected:
	NumericType m_Accurracy;
	NumericType m_LowFrequency;
	NumericType m_HighFreguency;
	short m_MaxPower;
	short m_MinPower;
	bool m_Exact;
};
txt_stream& operator<<(txt_stream& stream, const _CApproxCriterion& Appr);

class _CPathTraitor
{
public:
	enum Oper{DIVIDER=1,REMINDER=2,BOTH=3};
	typedef set<const string*> CONTEXTS;
	_CPathTraitor(_CMainCircuit* pBaseCircuit,const string& BaseContext);
	_CPathTraitor(const _CPathTraitor& Source);
	~_CPathTraitor();
	const _CComponent* AddDivider(const string& ElementPath, _CCompRedefEntry& Entry, const _CSubModelNumericPattern** ppCompPatt = nullptr);
	const _CComponent* AddReminder(const string& ElementPath, _CCompRedefEntry& Entry, const _CSubModelNumericPattern** ppCompPatt = nullptr);
	const string& GetBaseContext() {return m_BaseContext; }
	bool ShouldBeApproximated() {return m_pApproximationCriterion!=NULL;}
	void ClearApproximation();
	void SetApproximationCriterion(bool Exact=true,
								   const NumericType& Accurracy=DBL_EPSILON,
								   const NumericType& HighFrequency=1e6, 
								   const NumericType& LowFrequency=0.0,
								   short MaxPower=numeric_limits<short>::max(),
								   short MinPower=0);
protected:
	typedef enum{Divider,Reminder} OP_TYPE; 
	const _CComponent* AddOperation(OP_TYPE Type,const string& strPath,_CCompRedefEntry& Redef, const _CSubModelNumericPattern** ppCompPatt=NULL);


	_CMainCircuit* m_pBaseCircuit;
	const string& m_BaseContext;
	class _AbstractTreeElement;
	class _CTreeElement;
	//===============
	class _AbstractPredicator
	{
	public:
		_AbstractPredicator():m_ParentTreeElement(NULL) 
		{
		}
		bool operator() (const _CTreeElement& Left,const _CTreeElement& Right) const;
		_AbstractTreeElement* m_ParentTreeElement;
	protected:
	};
	//===============
	class _AbstractTreeElement
	{
	public:
		_AbstractTreeElement():m_Operation(0) 
		{
			m_Descendants.comp.m_ParentTreeElement=this;
		}
		_AbstractTreeElement(const _AbstractTreeElement& Source):m_Operation(Source.m_Operation),
			m_Descendants(Source.m_Descendants)
		{
			m_Descendants.comp.m_ParentTreeElement=this;
		}
		virtual _CCircuit* GetParentCircuit()=0;
	protected:
		typedef set<_CTreeElement,_AbstractPredicator> SET;
		SET m_Descendants;
		short m_Operation;
		friend class _CPathTraitor;
	public:
		typedef SET::iterator DESC_OPERATOR;
		//DESC_OPERATOR GetBegin() {m_Descendants.begin();}
		inline void SetDivider() {m_Operation|=(short)DIVIDER; }
		inline void SetReminder() {m_Operation|=(short)REMINDER; }
		inline void ResetDivider() {m_Operation&=(~(short)DIVIDER); }
		inline void ResetReminder() {m_Operation&=(~(short)REMINDER); }
		inline bool IsDivider() {return (m_Operation & (short)DIVIDER)!=0;}
		inline bool IsReminder() {return (m_Operation & (short)REMINDER)!=0;}
		inline bool IsBoth() {return (m_Operation & (short)BOTH)!=0;}
		_CComponent* AddOperPath(OP_TYPE Type, _string_list::iterator B, const _string_list::iterator& E, _CCompRedefEntry& Redef,const _CSubModelNumericPattern** ppCompPatt=NULL);
		inline void Clear() 
		{
			m_Descendants.clear();
		}
	};
	//===============
	class _CTreeElement : public _AbstractTreeElement
	{
	public:
		_CTreeElement(_CComponent* pComponent):m_pRootComponent(pComponent) 
		{
		}
		_CTreeElement(const _CTreeElement& Source):_AbstractTreeElement(Source),
			m_pRootComponent(Source.m_pRootComponent) 
		{
		}
		virtual _CCircuit* GetParentCircuit();
		_CComponent* GetRootComponent() const { return m_pRootComponent;}
		string GetStrPath() const;
	protected:
		_CComponent* m_pRootComponent;
		friend class _CPathTraitor;
		void GetOperStrSymbol(string& Oper);
	};
	//===============

	class _CTreeRoot : public _AbstractTreeElement
	{
	public:
		_CTreeRoot(_CMainCircuit* pMainCircuit):m_pMainCircuit(pMainCircuit) 
		{
		}
		_CTreeRoot(const _CTreeRoot& Source):_AbstractTreeElement(Source),
			m_pMainCircuit(Source.m_pMainCircuit) 
		{
		}
		virtual _CCircuit* GetParentCircuit();
	protected:
		_CMainCircuit* m_pMainCircuit;
	};
	//===============

	_CTreeRoot m_Tree;
	typedef pair<_AbstractTreeElement*, _AbstractTreeElement::SET::iterator> FOLLOWER;
	void ReStartFollower();
public:
	//typedef pair<_AbstractTreeElement*, _AbstractTreeElement::SET::iterator> FOLLOWER;
	typedef pair<const _CComponent*,short> OPER_DATA;
	typedef pair<short,short> COMP_RESULT; //first - comp result, second - OPER
	//FOLLOWER GetStartFollower(); 
	//const _CComponent* Follower2Component(const FOLLOWER& Follower);
	//short Follower2Oper(const FOLLOWER& Follower);
	//OPER_DATA Follower2Data(const FOLLOWER& Follower);
	//COMP_RESULT Compare(const FOLLOWER& Follower,const _CComponent* pComponent);
	//bool GetNextFollower(FOLLOWER& Follower);
	//bool GetSubFollower(FOLLOWER& Follower);
	//const string& GetStrContext(const FOLLOWER& Follower);

	const _CComponent* CurrentComponent();
	short CurrentOper();
	OPER_DATA CurrentData();
	COMP_RESULT Compare(const _CComponent* pComponent);
	inline bool IsOperatorComponent(const _CComponent* pComponent)
	{
		return Compare(pComponent).first==0;
	}
	bool NextFollower();
	bool SubFollower();
	void SideFollower();
	//const string& StrContext();
	const string& StrContext2();
	const string& StrApproxContext();
	bool PopLastLevel();
	void Clear();
protected:
	typedef list<FOLLOWER> FOLLOWER_LIST;
	FOLLOWER_LIST m_List;
	_CApproxCriterion* m_pApproximationCriterion;
public:
	const _CApproxCriterion* GetApproxCriterion() {return m_pApproximationCriterion;}
};