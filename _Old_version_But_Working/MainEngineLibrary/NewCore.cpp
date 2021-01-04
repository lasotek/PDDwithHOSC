#include "StdAfx.h"
#include "NewCore.h"
#include <algorithm>
#include "Circuit.h"
#include <limits>

COMPONENT_PINS_LIST EmptyPinList;


//*******************************_CGlobalIncidenceTable***********************
void _CGlobalIncidenceTable::AddPins(const COMPONENT_PINS_SET& Pins,bool TouchOnly)
{
	for(COMPONENT_PINS_SET::const_iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
	{
		unsigned Pin=*it;
		if(Pin>=m_MainVector.size())
		{
			m_MainVector.resize(Pin+1,0);
			m_TestVector.resize(Pin+1,0);
		}
		if(!TouchOnly)
			m_MainVector[Pin]++;
	}
}
void _CGlobalIncidenceTable::AddPins(const COMPONENT_PINS& Pins,bool TouchOnly)
{
	for(COMPONENT_PINS::const_iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
	{
		unsigned Pin=*it;
		if(Pin>=m_MainVector.size())
		{
			m_MainVector.resize(Pin+1,0);
			m_TestVector.resize(Pin+1,0);
		}
		if(!TouchOnly)
			m_MainVector[Pin]++;
	}
}
void _CGlobalIncidenceTable::AddCompPins(const _CComponent& Component,bool TouchOnly)
{
	//ResetTestRemoval();
	COMPONENT_PINS Pins;
	Component.RaportConnections(Pins);
	AddPins(Pins,TouchOnly);
}

void _CGlobalIncidenceTable::AddCompPins(const _CComponents& Components,bool TouchOnly,const COMPONENT_PINS_LIST** ppNewSeparateNodes)
{
	//ResetTestRemoval();
	for(_CComponents::const_iterator it=Components.begin(),_e=Components.end();it!=_e;it++)
		AddCompPins(**it,TouchOnly);
	if(ppNewSeparateNodes!=NULL)
		RaportAllSeparateNodes(*ppNewSeparateNodes);
}

//void _CGlobalIncidenceTable::ResetTest()
//{
////	if(!m_IsInTest) return;
////	m_IsInTest=false;
////	m_TestVectors.clear();
////	m_ComponentsInTest.clear();
////	m_TestNewSeparatePins.clear();
//}

void _CGlobalIncidenceTable::RaportAllSeparateNodes(const COMPONENT_PINS_LIST*& pNewSeparateNodes)
{
	COMPONENT_PINS_LIST* pNewSeparateNodes1=CurrentNewSeparatePins();
	pNewSeparateNodes=pNewSeparateNodes1;
	COMPONENT_PINS* pCompPins=CurrentCompPins();
	pNewSeparateNodes1->clear();
	for(size_t i=0;i<pCompPins->size();i++)
		if(pCompPins->at(i)==0)
			pNewSeparateNodes1->push_back(i);
}

bool _CGlobalIncidenceTable::TestCompProduceSeparatedPins(const _CComponent& Component)
{
	COMPONENT_PINS Pins;
	Component.RaportConnections(Pins);
	auto AuxVector(m_MainVector);
	transform(AuxVector.begin(), AuxVector.end(), m_TestVector.begin(), AuxVector.begin(),
		[](unsigned& Left, unsigned& Right) -> unsigned { return Left - Right; });
	//for (size_t i = 0; i < AuxVector.size(); i++)
		//AuxVector[i] -= m_TestVector[i];
	for (auto Pin : Pins)
		if (--AuxVector[Pin] == 0)
			return true;
	return false;
}


void _CGlobalIncidenceTable::RemoveCompPins(const _CComponent& Component, const COMPONENT_PINS_LIST*& pNewSeparateNodes)
{
	//ResetTestRemoval();
	COMPONENT_PINS Pins;
	Component.RaportConnections(Pins);
	m_MainNewSeparatePins.clear();
	for(COMPONENT_PINS::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
	{
		unsigned Pin=*it;
		ASSERTPDD(Pin<m_MainVector.size());
		unsigned& In=m_MainVector.at(Pin);
		ASSERTPDD(In>0);
		if(--In==0 /*&& Pin!=0*/)
			m_MainNewSeparatePins.push_back(Pin);
	}
	//if(m_MainVector[0]==0/* && (m_MainNewSeparatePins.empty() || m_MainNewSeparatePins.front()!=0)*/)
	//	m_MainNewSeparatePins.push_back(0);
}

void _CGlobalIncidenceTable::CorrectCompPins(COMPONENT_PINS_LIST& SeparateNodes)
{
	if(m_MainVector[0]==0 && (SeparateNodes.empty() || SeparateNodes.front()!=0))
		SeparateNodes.push_back(0);
}

//void _CGlobalIncidenceTable::TestRemovalCompPins(const _CComponent& Component, COMPONENT_PINS_LIST& NewSeparateNodes) 
//{
//	m_ComponentsInTest.push_back(&Component);
//	m_NoOfElelements.push_back(1);
//
//	//NewSeparateNodes.clear();
//	//size_t IncdSize=m_MainVector.size();
//	//COMPONENT_PINS Incidences(IncdSize,0);
//	//COMPONENT_PINS Pins;
//	//Component.RaportConnections(Pins);
//	//for(COMPONENT_PINS::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
//	//	Incidences[*it]++;
//	//for(COMPONENT_PINS::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
//	//{
//	//	size_t i=*it;
//	//	size_t CurIncds=m_MainVector[i], ToCut=Incidences[i];
//	//	ASSERTPDD(CurIncds>=ToCut);
//	//	if(CurIncds==ToCut)
//	//		NewSeparateNodes.push_back(i);
//	//}
//}

//void _CGlobalIncidenceTable::TestRemovalCompPins(const _CComponent& Component1, const _CComponent& Component2, COMPONENT_PINS_LIST& NewSeparateNodes) 
//{
//	m_ComponentsInTest.push_back(&Component1);
//	m_ComponentsInTest.push_back(&Component2);
//	m_NoOfElelements.push_back(2);
//
//
//	NewSeparateNodes.clear();
//	size_t IncdSize=m_MainVector.size();
//	COMPONENT_PINS Incidences(IncdSize,0);
//	COMPONENT_PINS Pins;
//	Component1.RaportConnections(Pins);
//	for(COMPONENT_PINS::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
//		Incidences[*it]++;
//	COMPONENT_PINS_SET Comm(Pins.begin(),Pins.end());
//	Pins.clear();
//	Component2.RaportConnections(Pins);
//	for(COMPONENT_PINS::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
//		Incidences[*it]++;
//	Comm.insert(Pins.begin(),Pins.end());
//	for(COMPONENT_PINS_SET::iterator it=Comm.begin(),_e=Comm.end();it!=_e;it++)
//	{
//		size_t i=*it;
//		size_t CurIncds=m_MainVector[i], ToCut=Incidences[i];
//		ASSERTPDD(CurIncds>=ToCut);
//		if(CurIncds==ToCut)
//			NewSeparateNodes.push_back(i);
//	}
//}

//void _CGlobalIncidenceTable::TestRemovalCompPins(const _CComponent& Component, const COMPONENT_PINS_LIST*& pNewSeparateNodes)
//{
//	COMPONENT_PINS* pCompPins=CurrentCompPins();
//	m_IsInTest=true;
//	m_TestVectors.push_back(COMPONENT_PINS(*pCompPins));
//	pCompPins=&m_TestVectors.back();
//	COMPONENT_PINS_LIST* pNewSNodes=NULL;
//	m_TestNewSeparatePins.push_back(COMPONENT_PINS_LIST());
//	pNewSeparateNodes=pNewSNodes=&m_TestNewSeparatePins.back();
//	COMPONENT_PINS Pins;
//	Component.RaportConnections(Pins);
//	m_ComponentsInTest.push_back(&Component);
//	for(COMPONENT_PINS::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
//	{
//		unsigned Pin=*it;
//		ASSERTPDD(Pin<pCompPins->size());
//		unsigned& In=pCompPins->at(Pin);
//		ASSERTPDD(In>0);
//		if(--In==0)
//			pNewSNodes->push_back(Pin);
//	}
//}


//bool _CGlobalIncidenceTable::RemoveTest(size_t Counter)
//{
//	//for(size_t i=Counter;i>0;i--)
//	//{
//	//	if(!m_IsInTest)
//	//		return false;
//	//	m_TestNewSeparatePins.pop_back();
//	//	m_TestVectors.pop_back();
//	//	m_ComponentsInTest.pop_back();
//	//	m_IsInTest=!m_ComponentsInTest.empty();
//	//	ASSERTPDD(m_IsInTest || (m_TestVectors.empty() && m_TestNewSeparatePins.empty()));
//	//}
//	return true;
//}

size_t _CGlobalIncidenceTable::GetCurrentIncidences(size_t Node) const
{
	const COMPONENT_PINS* pPins=CurrentCompPins();
	if(Node>=pPins->size())
	{
		int i=1;
		i*=1;
	}
	ASSERTPDD(Node<pPins->size());
	return pPins->at(Node);
}

//void _CGlobalIncidenceTable::PreparPins(COMPONENT_PINS& Pins)
//{
//	if(m_Wrapers.empty())
//		Pins.assign(m_MainVector.begin(),m_MainVector.end());
//	else
//	{
//		COMPONENT_PINS& Top=m_Wrapers.back()->m_LocalPins;
//		Pins.assign(Top.begin(),Top.end());
//	}
//}

//*******************************_CGlobalIncidenceTable***********************
//*******************************_CTestIncidentsWraper************************

_CTestIncidentsWraper::_CTestIncidentsWraper(_CGlobalIncidenceTable& GlobalTable):
	m_GlobalTable(GlobalTable),
	m_TestVector(GlobalTable.m_TestVector)
{
}

//_CTestIncidentsWraper::_CTestIncidentsWraper(const _CTestIncidentsWraper& TopTable):
//	m_GlobalTable(TopTable.m_GlobalTable),
//	m_TestVector(TopTable.m_TestVector)
//{
//	RISEPDD(eWrongCondition,"Coping prohibited");
//}

_CTestIncidentsWraper::~_CTestIncidentsWraper()
{
	for (auto Pin : m_2CompPins)
		m_TestVector[Pin]--;
	for (auto Pin : m_1CompPins)
		m_TestVector[Pin]--;
}

void _CTestIncidentsWraper::TestRemovalCompPins(const _CComponent& Component, COMPONENT_PINS_LIST& NewSeparateNodes)
{
	Component.RaportConnections(m_1CompPins);
	for (auto Pin : m_1CompPins)
	{
		ASSERTPDD(Pin < m_TestVector.size() && m_TestVector[Pin] <= m_GlobalTable.m_MainVector[Pin]);
		if (++m_TestVector[Pin] == m_GlobalTable.m_MainVector[Pin])
			NewSeparateNodes.push_back(Pin);
	}
}

//void _CTestIncidentsWraper::TestRemovalCompPinsFast(const _CComponent& Component, COMPONENT_PINS_LIST& NewSeparateNodes) const
//{
//	COMPONENT_PINS Pins;
//	Component.RaportConnections(Pins);
//	for(COMPONENT_PINS::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
//	{
//		size_t Pin=*it;
//		ASSERTPDD(Pin<m_TestVector.size() && m_TestVector[Pin]<=m_GlobalTable.m_MainVector[Pin]);
//		if(m_TestVector[Pin]+1==m_GlobalTable.m_MainVector[Pin])
//			NewSeparateNodes.push_back(Pin);
//	}
//}

void _CTestIncidentsWraper::TestRemovalCompPins(const _CComponent& Component1, const _CComponent& Component2, COMPONENT_PINS_LIST& NewSeparateNodes)
{
	Component1.RaportConnections(m_1CompPins);
	for (auto Pin : m_1CompPins)
	{
		ASSERTPDD(Pin<m_TestVector.size() && m_TestVector[Pin]<=m_GlobalTable.m_MainVector[Pin]);
		if(++m_TestVector[Pin]==m_GlobalTable.m_MainVector[Pin])
			NewSeparateNodes.push_back(Pin);
	}
	Component2.RaportConnections(m_2CompPins);
	for (auto Pin : m_2CompPins)
	{
		ASSERTPDD(Pin<m_TestVector.size() && m_TestVector[Pin]<=m_GlobalTable.m_MainVector[Pin]);
		if(++m_TestVector[Pin]==m_GlobalTable.m_MainVector[Pin])
			NewSeparateNodes.push_back(Pin);
	}
}
//void _CTestIncidentsWraper::TestRemovalCompPinsFast(const _CComponent& Component1, const _CComponent& Component2, COMPONENT_PINS_LIST& NewSeparateNodes) const
//{
//	map<size_t,size_t> Map;
//	
//	COMPONENT_PINS_LIST Pins;
//	Component1.RaportUniqConnections(Pins);
//	for(COMPONENT_PINS_LIST::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
//		Map[*it]++;
//}

//*******************************_CTestIncidentsWraper************************
//*******************************_CTestIncidentsWRAPER************************
//_CTestIncidentsWRAPER::_CTestIncidentsWRAPER(_CGlobalIncidenceTable& GlobalTable) :
//m_TestVector(GlobalTable.m_MainVector)
//{
//}
//
//_CTestIncidentsWRAPER::_CTestIncidentsWRAPER(const _CTestIncidentsWRAPER& TopTable) :
//m_TestVector(TopTable.m_TestVector)
//{
//}
//
//
//void _CTestIncidentsWRAPER::TestRemovalCompPins(const _CComponent& Component, COMPONENT_PINS_LIST& NewSeparateNodes)
//{
//	COMPONENT_PINS Pins;
//	Component.RaportConnections(Pins);
//	for (auto Pin : Pins)
//	{
//		ASSERTPDD(m_TestVector[Pin] > 0);
//		if (--m_TestVector[Pin] == 0)
//			NewSeparateNodes.push_back(Pin);
//	}
//}
//void _CTestIncidentsWRAPER::TestRemovalCompPins(const SET_OF_COMPONENTS& Components, COMPONENT_PINS_LIST& NewSeparateNodes)
//{
//	for (auto pComp : Components)
//		TestRemovalCompPins(*pComp, NewSeparateNodes);
//}
//
//void _CTestIncidentsWRAPER::TestRemovalCompPins(const _CComponent& Component1, const _CComponent& Component2, COMPONENT_PINS_LIST& NewSeparateNodes)
//{
//	COMPONENT_PINS Pins;
//	Component1.RaportConnections(Pins);
//	for (auto Pin : Pins)
//	{
//		ASSERTPDD(m_TestVector[Pin] > 0);
//		if (--m_TestVector[Pin] == 0)
//			NewSeparateNodes.push_back(Pin);
//	}
//	Pins.clear();
//	Component2.RaportConnections(Pins);
//	for (auto Pin : Pins)
//	{
//		ASSERTPDD(m_TestVector[Pin] > 0);
//		if (--m_TestVector[Pin] == 0)
//			NewSeparateNodes.push_back(Pin);
//	}
//}
//
//bool _CTestIncidentsWRAPER::TestCompProduceSeparatedPins(const _CComponent& Component)
//{
//	COMPONENT_PINS Pins;
//	Component.RaportConnections(Pins);
//	auto AuxVector(m_TestVector);
//	for (auto Pin : Pins)
//		if (--AuxVector[Pin] == 0)
//			return true;
//	return false;
//}

//*******************************_CTestIncidentsWRAPER************************

//*******************************_CCancelledNodesMap**************************

//_CCancelledNodesMap::_iterator _CCancelledNodesMap::_find2insert(int NodeNo,int& Pos)//throws if already exists
//{
//	Pos=0;
//	_iterator it=m_List.begin(),_e=m_List.end(),_prev=it;
//	if(it==_e)
//		return it;
//	while(it!=_e && (it->first<NodeNo))
//	{
//		_prev=it;
//		it++; Pos++;
//	}
//	if(it!=_e && it->first<=NodeNo)
//		RISEPDD(eWrongCondition,"Unexpected duplication in deletions");
//	Pos--;
//	return _prev;
//}
//
//_CCancelledNodesMap::_iterator _CCancelledNodesMap::_find_existed(int NodeNo)//return end if not exists
//{
//	_iterator it=m_List.begin(),_e=m_List.end();
//	while(it!=_e && (it->first<NodeNo))
//		it++;
//	if(it!=_e || it->first>NodeNo)
//		return _e;
//	return it;
//}
//_CCancelledNodesMap::_iterator _CCancelledNodesMap::_find_existed(int NodeNo,int& Pos)//return end if not exists
//{
//	Pos=0;
//	_iterator it=m_List.begin(),_e=m_List.end();
//	while(it!=_e && (it->first<NodeNo))
//	{
//		it++; Pos++;
//	}
//	if(it!=_e || it->first>NodeNo)
//		return _e;
//	return it;
//}
//
//int _CCancelledNodesMap::_translate(int NodeNo)
//{
//	_iterator it=_find_existed(NodeNo);
//	if(it==m_List.end())
//		return NodeNo;
//	else
//		return it->second;
//}
//
//void _CCancelledNodesMap::_translate_and_update(int& NodeNo)
//{
//	_iterator it=_find_existed(iAbs(NodeNo));
//	if(it!=m_List.end())
//	{
//		short sgn=NodeNo<0?-1:1;
//		int& Next=it->second;
//		_translate_and_update(Next);
//		NodeNo=Next*sgn;
//	}
//}
//
//int _CCancelledNodesMap::_insert(int NodeA, int NodeB)
//{
//	int Pos=0;
//	_iterator it2in=_find2insert(NodeA,Pos);
//	m_List.insert(it2in,CONTRACTION(NodeA,NodeB));
//	return Pos;
//}
//int _CCancelledNodesMap::_remove(int NodeA)
//{
//	int Pos=0;
//	_iterator it=_find_existed(NodeA,Pos);
//	m_List.erase(it);
//	return Pos;
//}
//void _CCancelledNodesMap::Flatten()
//{
//	for(_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
//		_translate_and_update(it->second);
//}
//
//short _CCancelledNodesMap::_reverse(_iterator& It)//sgn change
//{
//	short sgn=1;
//	int A=It->first,B=It->second;
//	int OldPos=it2pos(It);
//	m_List.erase(It);
//	int NewPos=_insert(B,A);
//	Flatten();
//	return OldPos-NewPos;
//}
//
//int _CCancelledNodesMap::it2pos(_iterator& It)
//{
//	int Pos=0;
//	_iterator it=m_List.begin(),_e=m_List.end();
//	while(it!=_e && it!=It)
//	{
//		Pos++;it++;
//	}
//	if(it==_e)
//		RISEPDD(eWrongCondition,"Unproper iterator");
//	return Pos;
//}
//
//short _CCancelledNodesMap::IgnoreNode(int Node)
//{
//	short sgn=1;
//	int Pos=0;
//	_iterator It=_find_existed(Node,Pos);
//	if(It!=m_List.end())
//	{
//	}
//	return sgn;
//}

//*******************************_CCancelledNodesMap**************************
//*******************************_CSubNodeContractionsSimpl*******************

_CSubNodeContractionsSimpl::_iterator _CSubNodeContractionsSimpl::_find2insert(int NodeNo,int& Pos)//throws if already exists
{
	Pos=0;
	_iterator it=m_List.begin(),_e=m_List.end()/*,_prev=it*/;
	if(it==_e)
		return it;
	while(it!=_e && (it->first<NodeNo))
	{
		//_prev=it;
		it++; Pos++;
	}
	if(it!=_e && it->first<=NodeNo)
		RISEPDD(eWrongCondition,"Unexpected duplication in deletions");
	//Pos--;
	return it;
	//return _prev;
}

_CSubNodeContractionsSimpl::_iterator _CSubNodeContractionsSimpl::_find_existed(int NodeNo)//return end if not exists
{
	_iterator it=m_List.begin(),_e=m_List.end();
	while(it!=_e && (it->first<NodeNo))
		it++;
	if(it==_e || it->first>NodeNo)
		return _e;
	return it;
}
_CSubNodeContractionsSimpl::_const_iterator _CSubNodeContractionsSimpl::_find_existed(int NodeNo) const//return end if not exists
{
	_const_iterator it=m_List.begin(),_e=m_List.end();
	while(it!=_e && (it->first<NodeNo))
		it++;
	if(it==_e || it->first>NodeNo)
		return _e;
	return it;
}

_CSubNodeContractionsSimpl::_iterator _CSubNodeContractionsSimpl::_find_existed(int NodeNo,int& Pos)//return end if not exists
{
	Pos=0;
	_iterator it=m_List.begin(),_e=m_List.end();
	while(it!=_e && (it->first<NodeNo))
	{
		it++; Pos++;
	}
	if(it==_e || it->first>NodeNo)
		return _e;
	return it;
}
_CSubNodeContractionsSimpl::_iterator _CSubNodeContractionsSimpl::_find_first_into(int NodeNo,int& Pos)//return end if not exists
{
	Pos=0;
	_iterator it=m_List.begin(),_e=m_List.end();
	for(;it!=_e;it++,Pos++)
		if(iAbs(it->second)==NodeNo)
			return it;
	return _e;
}
_CSubNodeContractionsSimpl::_iterator _CSubNodeContractionsSimpl::_search_and_prepare_first_negative(int& Pos)
{
	Pos=0;
	_iterator it=m_List.begin(),_e=m_List.end();
	for(;it!=_e;it++,Pos++)
	{
		int A=it->first;
		int B=it->second;
		if(B>=0)
			continue;
		_iterator it_ext=it;
		it_ext++;
		for(;it_ext!=_e;it_ext++)
		{
			int& B1=it_ext->second;
			if(B1>=0)
				continue;
			if(B1!=B)
				return _e;
			B1=-A;
		}
		return it;
	}
	return _e;
}

int _CSubNodeContractionsSimpl::_translate(int NodeNo) const
{
	_const_iterator it=_find_existed(iAbs(NodeNo));
	if(it==m_List.end())
		return NodeNo;
	else
		return it->second*(NodeNo<0?-1:1);
}

int _CSubNodeContractionsSimpl::_translate_and_update(int& NodeNo)
{
	_iterator it=_find_existed(iAbs(NodeNo));
	if(it!=m_List.end())
	{
		short sgn=NodeNo<0?-1:1;
		int& Next=it->second;
		_translate_and_update(Next);
		NodeNo=Next*sgn;
	}
	if(m_CurrZero!=0 && NodeNo==-m_CurrZero)
		NodeNo=m_CurrZero;
	return NodeNo;
}

int _CSubNodeContractionsSimpl::_insert(int NodeA, int NodeB)
{
	int Pos=0;
	_iterator it2in=_find2insert(NodeA,Pos);
	m_List.insert(it2in,CONTRACTION(NodeA,NodeB));
	return Pos;
}
int _CSubNodeContractionsSimpl::_remove(int NodeA)
{
	int Pos=0;
	_iterator it=_find_existed(NodeA,Pos);
	m_List.erase(it);
	return Pos;
}
void _CSubNodeContractionsSimpl::_Flatten()
{
	m_HasNegativeDels=false;
	for(_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		//_translate_and_update(it->second);
		m_HasNegativeDels= (_translate_and_update(it->second)<0) || m_HasNegativeDels;
}

short _CSubNodeContractionsSimpl::_reverse(_iterator& It)//sgn change
{
	short sgn=1;
	int A=It->first,B=It->second;
	int OldPos=_it2pos(It);
	m_List.erase(It);
	int NewPos=_insert(B,A);
	_Flatten();
	return OldPos-NewPos;
}

int _CSubNodeContractionsSimpl::_it2pos(_iterator& It)
{
	int Pos=0;
	_iterator it=m_List.begin(),_e=m_List.end();
	while(it!=_e && it!=It)
	{
		Pos++;it++;
	}
	if(it==_e)
		RISEPDD(eWrongCondition,"Unproper iterator");
	return Pos;
}
_CSubNodeContractionsSimpl::CAN_REM _CSubNodeContractionsSimpl::CanRemove(int Node)
{
	bool IsCurrZero=Node==m_CurrZero;
	int Pos=0;
	CONTRACTION_LIST::iterator it=m_List.begin(),_e=m_List.end();
	m_Pos2Rem=-1;
	m_RemIt=_e;
	m_ModifyList.clear();
	TO_MODIFY AuxNegModifyList;
	CONTRACTION_LIST::iterator AuxNegRemIt=_e;
	int PosNeg=-1;
	bool CanBeRedByNeg=true, MoreThenOneMinus=false,HasPlusAlso=false;
	for(;it!=_e;it++,Pos++)
	{
		if(it->first==Node )
		{
			m_Pos2Rem=Pos;
			m_RemIt=it;
			int A=it->first,B=it->second;
			if(A==m_CurrZero)
			{
				for(CONTRACTION_LIST::iterator oit=m_List.begin();oit!=_e;oit++)
					if(oit!=it && iAbs(oit->second)==m_CurrZero)
						m_ModifyList.push_back(oit);
			}
			return CAN;
		}
		int B=it->second;
		if(iAbs(B)==Node)
		{
			m_Pos2Rem=Pos;
			m_RemIt=it;
			for(CONTRACTION_LIST::iterator oit=m_List.begin();oit!=_e;oit++)
				if(oit!=it && iAbs(oit->second)==Node)
					m_ModifyList.push_back(oit);
			//CONTRACTION_LIST::iterator oit=it;
			//oit++;
			//for(;oit!=_e;oit++,Pos++)
			//	if(iAbs(oit->second)==A)
			//		m_ModifyList.push_back(oit);
			return CAN;
		}
		if(IsCurrZero && CanBeRedByNeg)
		{
			if(B<0 && PosNeg<0)
			{
				AuxNegRemIt=it;
				PosNeg=Pos;
			}
			else
			{
				if(PosNeg>=0)
				{
					if(B==AuxNegRemIt->second /*|| B==-AuxNegRemIt->second*/)
					{
						AuxNegModifyList.push_back(it);
						MoreThenOneMinus=true;
					}
					else if(B<0)
						CanBeRedByNeg=false;
					//else if(-B==AuxNegRemIt->second)
					//	HasPlusAlso=true;
					//if(MoreThenOneMinus && HasPlusAlso)
					//	CanBeRedByNeg=false;
				}
			}
		}
		//if(IsCurrZero && CanBeRedByNeg)
		//{
		//	if(B<0 && PosNeg<0)
		//	{
		//		AuxNegRemIt=it;
		//		PosNeg=Pos;
		//	}
		//	else
		//	{
		//		if(PosNeg>=0)
		//		{
		//			if(B==AuxNegRemIt->second)
		//			{
		//				AuxNegModifyList.push_back(it);
		//				MoreThenOneMinus=true;
		//			}
		//			else if(B<0)
		//				CanBeRedByNeg=false;
		//			else if(-B==AuxNegRemIt->second)
		//				HasPlusAlso=true;
		//			if(MoreThenOneMinus && HasPlusAlso)
		//				CanBeRedByNeg=false;
		//		}
		//	}
		//}
	}
	if(IsCurrZero && PosNeg>=0)
		if(CanBeRedByNeg)
		{
			//if(MoreThenOneMinus)
			//{
			//	CONTRACTION_LIST::iterator ait=m_List.begin();
			//	int APos=0,&B=AuxNegRemIt->second;
			//	for(;APos<PosNeg;ait++,APos++)
			//		if(ait->second=-B)
			//			return DELAY;
			//}
			m_Pos2Rem=PosNeg;
			m_RemIt=AuxNegRemIt;
			m_ModifyList=AuxNegModifyList;
			return CAN;
		}
		else
			return NEED2SPLIT;
	else
		return DISCONNECT;
}
_CSubNodeContractionsSimpl::_NODE_RES _CSubNodeContractionsSimpl::_IgnoreNode(int Node)
{
	ASSERTPDD(m_Pos2Rem>=0);
	short sgn=1;
	int A=m_RemIt->first, B=m_RemIt->second;
	bool NegRemoving = B < 0;
	//TO_MODIFY CheckPositionList;
	if(A==Node)
	{
		if(A==m_CurrZero)
		{
			sgn*=iSgn(B);
			m_CurrZero=iAbs(B);
			if(m_CurrZero!=0)
				for(TO_MODIFY::iterator mit=m_ModifyList.begin(),_me=m_ModifyList.end();mit!=_me;mit++)
				{
					int& MA=(*mit)->first;
					int& MB=(*mit)->second;
					MB=m_CurrZero;
					//if(MB>MA)
					//	CheckPositionList.push_back(*mit);
				}
				//for(_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
				//	if(it->second==-m_CurrZero)
				//		it->second*=-1;
		}
	}
	else if(iAbs(B)==Node)
	{
		if(B==m_CurrZero)
			m_CurrZero=A;
		else if(-B==m_CurrZero)
		{
			sgn*=-1;
			m_CurrZero=-B;
		}
		if(B<0)
		{
			A*=-1;
			B*=-1;
		}
		else
			sgn*=-1;
		int BPos=A,BNeg=-A;
		if(BPos==-m_CurrZero) BPos*=-1;
		if(BNeg==-m_CurrZero) BNeg*=-1;
		for(TO_MODIFY::iterator mit=m_ModifyList.begin(),_me=m_ModifyList.end();mit!=_me;mit++)
		{
			int& MA=(*mit)->first;
			int& MB=(*mit)->second;
			if(MB==B)
				MB=BPos;
			else if(MB==-B)
				MB=BNeg;
			//if((int)iAbs(MB)>MA)
			//	CheckPositionList.push_back(*mit);
		}
	}
	else
	{
		ASSERTPDD(Node==m_CurrZero && m_RemIt->second<0);
		sgn*=-2;
		m_CurrZero=-m_RemIt->second;
		for(TO_MODIFY::iterator mit=m_ModifyList.begin(),_me=m_ModifyList.end();mit!=_me;mit++)
		{
			(*mit)->second=m_RemIt->first;
			//(*mit)->second=-m_RemIt->second;
			//if((*mit)->second>(*mit)->first)
			//	CheckPositionList.push_back(*mit);
		}
	}
	//size_t SCPL=CheckPositionList.size();
	//SCPL*=1;
	m_List.erase(m_RemIt);
	int Pos2Rem=m_Pos2Rem;
	DelayRemove();
	if (NegRemoving)
		UpdateNegatives();
	return _NODE_RES(sgn,Pos2Rem);
}

void _CSubNodeContractionsSimpl::UpdateNegatives()
{
	m_HasNegativeDels = find_if(m_List.begin(), m_List.end(), [](const CONTRACTION& Pair) {return Pair.second < 0; }) != m_List.end();
}
//_CSubNodeContractionsSimpl::_NODE_RES _CSubNodeContractionsSimpl::_IgnoreNode(int Node)
//{
//	short sgn=1;//Tu poprawiæ
//	int Pos=0;
//	bool NodeIsZero=Node==m_CurrZero,NeedEnum=false;
//	int NewZero=-1;
//	_iterator _e=m_List.end(),It=_find_existed(Node,Pos);
//	if(It==_e)
//	{
//		It=_find_first_into(Node,Pos);
//		if(It==_e)
//		{
//			if(Node==m_CurrZero)
//			{
//				It=_search_and_prepare_first_negative(Pos);
//				if(It==_e)
//					return _NODE_RES(0,0);
//				sgn*=-2;
//				NewZero=-It->second;
//			}
//			else
//				return _NODE_RES(0,0);
//		}
//		else
//		{
//			int A=It->first,B=It->second;
//			if(B<0)
//			{
//				A=NegNode(A);
//				B=NegNode(B);
//			}
//			else
//				sgn=-1;
//			//m_HasNegativeDels=false;
//			if(NodeIsZero)
//			{
//				NewZero=It->first;
//				if(NewZero<0)
//					NewZero=-NewZero;
//				else
//					sgn=-sgn;
//			}
//			for(_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
//				if(it!=It)
//				{
//					int& V=it->second;
//					if(V==B)
//						V=A;
//					else if(V==-B)
//						V=-A;
//					if(V==-NewZero)
//						V=NewZero;
//				}
//		}
//
//	}
//	else
//		if(NodeIsZero)
//		{
//			NewZero=It->second;
//			if(NewZero<0)
//				NewZero=-NewZero;
//			else
//				sgn=-sgn;
//			for(_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
//				if(it!=It)
//				{
//					int& V=it->second;
//					if(V==-NewZero)
//						V=NewZero;
//				}
//		}
//	m_List.erase(It);
//	if(NewZero)
//		m_CurrZero=NewZero;
//	return _NODE_RES(sgn,Pos);
//}
//_CSubNodeContractionsSimpl::IGNORE_NODE_RES _CSubNodeContractionsSimpl::IgnoreNode(int Node,int& Subst/*only for 0*/)//the sign change and needs of S rebuild
//{
//	short Sgn=1; bool RebuildS=false;
//	//Flatten();
//	CANCELLED_NODES_MAP::iterator dit=m_CancelledNodesMap.find(Node),_e=m_CancelledNodesMap.end();
//	if (dit==_e)
//	{
//		dit=m_CancelledNodesMap.begin();
//		for(;dit!=_e;dit++)
//			if(iAbs(dit->second)==Node)
//				break;
//		if(dit==_e)
//		{
//			Subst=Node;
//			return IGNORE_NODE_RES(1,false);
//		}
//		int A=dit->first,B=dit->second;
//		if(B<0)
//		{
//			A=-A;
//			B=-B;
//			//Sgn=-Sgn;
//		}
//		//else
//		//	Sgn=-Sgn;
//		m_CancelledNodesMap.erase(dit);
//		dit=m_CancelledNodesMap.insert(CANCELLED_NODES_MAP::value_type(B,A)).first;
//		//if(Node==m_CurrZero)
//		//	m_CurrZero=iAbs(dit->second);
//		Flatten();
//		RebuildS=true;
//		Subst=A;
//	}
//	else
//		Subst=Node;
//	//else
//	//	if(Node==m_CurrZero)
//	//		m_CurrZero=iAbs(dit->second);
//	bool SNeg=dit->second<0;
//	m_CancelledNodesMap.erase(dit);
//	if(SNeg)
//		TestNegative();
//	return IGNORE_NODE_RES(Sgn,RebuildS);
//}

//void _CSubNodeContractionsSimpl::Flatten()
//{
//	m_HasNegativeDels=false;
//	for(CANCELLED_NODES_MAP::iterator it=m_CancelledNodesMap.begin(),_e=m_CancelledNodesMap.end();it!=_e;it++)
//		m_HasNegativeDels=(GetContractedNodeAndRebuild(it->second)<0) || m_HasNegativeDels;
//}
//void _CSubNodeContractionsSimpl::TestNegative()
//{
//	m_HasNegativeDels=false;
//	//for(CANCELLED_NODES_MAP::iterator it=m_CancelledNodesMap.begin(),_e=m_CancelledNodesMap.end();it!=_e;it++)
//	for(_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
//		if(it->second<0)
//		{
//			m_HasNegativeDels=true;
//			return;
//		}
//}

int _CSubNodeContractionsSimpl::GetContractedNodeFast(int Node) const
{
	return _translate(Node);
	//int Mult=Node<0?-1:1;
	//if(Node<0) Node=-Node;
	//CANCELLED_NODES_MAP::iterator fit=m_CancelledNodesMap.find(Node);
	//if(fit==m_CancelledNodesMap.end())
	//	Node*=Mult;
	//else
	//	Node=fit->second*Mult;
	//return Node;

}

int _CSubNodeContractionsSimpl::GetContractedNodeAndRebuild(int& Node)
{
	return _translate_and_update(Node);
	//int Mult=Node<0?-1:1;
	//if(Node<0) Node=-Node;
	//CANCELLED_NODES_MAP::iterator fit=m_CancelledNodesMap.find(Node);
	//if(fit==m_CancelledNodesMap.end())
	//	Node*=Mult;
	//else
	//	Node=GetContractedNodeAndRebuild(fit->second)*Mult;
	//return Node;
}

//int _CSubNodeContractionsSimpl::GetContractedNode(int& Node)
//{
//	if(iAbs(Node)==m_CurrZero) 
//	{	Node=m_CurrZero;
//		return Node;
//	}
//	int Mult=Node<0?-1:1;
//	if(Node<0) Node=-Node;
//	CANCELLED_NODES_MAP::iterator fit=m_CancelledNodesMap.find(Node);
//	if(fit==m_CancelledNodesMap.end())
//		Node*=Mult;
//	else
//		Node=GetContractedNode(fit->second)*Mult;
//	if(iAbs(Node)==m_CurrZero) 
//		Node=m_CurrZero;
//	return Node;
//}

void _CSubNodeContractionsSimpl::Store(_binary_filer& Filer) 
{
	Filer<<m_CurrZero;
	//Filer<<m_CancelledNodesMap.size();
	//for(CANCELLED_NODES_MAP::iterator it=m_CancelledNodesMap.begin(),_e=m_CancelledNodesMap.end();it!=_e;it++)
	Filer<<m_List.size();
	for(_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		Filer<<it->first;
		Filer<<it->second;
	}
}

void _CSubNodeContractionsSimpl::Load(_binary_filer& Filer) 
{
	m_List.clear();
	Filer>>m_CurrZero;
	size_t Size=0;
	Filer>>Size;
	for(size_t i=0;i<Size;i++)
	{
		int a=0,b=0;
		Filer>>a;
		Filer>>b;
		//m_CancelledNodesMap[a]=b;
		m_List.push_back(CONTRACTION(a,b));
	}
}
bool _CSubNodeContractionsSimpl::ContainNode(int Node)
{
	//for(CANCELLED_NODES_MAP::iterator it=m_CancelledNodesMap.begin(),_e=m_CancelledNodesMap.end();it!=_e;it++)
	for(_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		if(it->first==Node || it->second==Node)
			true;
	return false;
}

bool _CSubNodeContractionsSimpl::ContainNodes(const DONGLE_NODES& Nodes)
{
	for(DONGLE_NODES::const_iterator it=Nodes.begin(),_e=Nodes.end();it!=_e;it++)
		if(!ContainNode(*it))
			return false;
	return true;
}

short _CSubNodeContractionsSimpl::TranslateMe(const NODES_CONNECTION& NodesConnections,POS_LIST& PosList/*,short& Sgn*/)
{
	CONTRACTION_LIST AuxList(m_List);
	m_List.clear();
	short Sgn=1;
	for(_iterator it=AuxList.begin(),_e=AuxList.end();it!=_e;it++)
	{
		NODES_CONNECTION::const_iterator ait=NodesConnections.find(it->first),_nce=NodesConnections.end();
		NODES_CONNECTION::const_iterator bit=NodesConnections.find(iAbs(it->second));
		if(ait==_nce || bit==_nce)
			RISEPDD(eWrongCondition,"Unexpected nodes collocation");
		int ANode=ait->second, BNode=bit->second*sgn(it->second);
		if(ANode==BNode)
			return 0;
		ANode=_translate(ANode);
		BNode=_translate(BNode);
		if(ANode==BNode)
			return 0;
		unsigned UANode=iAbs(ANode);
		unsigned UBNode=iAbs(BNode);
		if(UANode<UBNode)
		{
			swap(ANode,BNode); swap(UANode,UBNode); Sgn=-Sgn;
		}
		if(ANode<0)
		{
			ANode*=-1;
			BNode*=-1;
			Sgn*=-1;
		}
		if(ANode==-BNode)
		{
			BNode=UBNode=m_CurrZero;
			if(BNode>ANode)
			{
				Sgn*=-1;
				swap(ANode,BNode);
				swap(UANode,UBNode);
			}
			Sgn<<=1;
		}
		int Pos=_insert(ANode,BNode);
		_Flatten();
		if(ANode==m_CurrZero)
		{
			m_CurrZero=UBNode;
			if(BNode<0)
				Sgn*=-1;
		}
		PosList.push_back(Pos);
	}
	return Sgn;
}

unsigned long long _CSubNodeContractionsSimpl::GetHash(unsigned long long Core) 
{
	//Core*=HASH_FACTOR;
	//Core^=(unsigned long long)m_HasNegativeDels;
	//if(m_HasNegativeDels)
	//{
	//	Core*=HASH_FACTOR;
	//	Core^=m_CurrZero;
	//}
	//for(CANCELLED_NODES_MAP::iterator it=m_CancelledNodesMap.begin(),_e=m_CancelledNodesMap.end();it!=_e;it++)
	m_HasNegativeDels=false;
	for(_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		int A = it->second;
		Core*=HASH_FACTOR;
		Core ^= (unsigned long long)it->first;
		Core*=HASH_FACTOR;
		Core ^= (unsigned long long)it->second;
		m_HasNegativeDels=m_HasNegativeDels || it->second<0;
	}
	Core*=HASH_FACTOR;
	Core^=(unsigned long long)(m_HasNegativeDels?m_CurrZero:0);
	return Core;
}

bool _CSubNodeContractionsSimpl::IsEqual(const _CSubNodeContractionsSimpl& Other)  const
{
	if(m_HasNegativeDels!=Other.m_HasNegativeDels)
		return false;
	if(m_HasNegativeDels && (m_CurrZero!=Other.m_CurrZero))
		return false;
	return m_List==Other.m_List;
	////if(m_HasNegativeDels!=Other.m_HasNegativeDels) return false;
	//if(m_CancelledNodesMap.size()!=Other.m_CancelledNodesMap.size())
	//	return false;
	////if(m_HasNegativeDels)
	////	if(m_CurrZero!=Other.m_CurrZero)
	////		return false;
	//CANCELLED_NODES_MAP::iterator it=m_CancelledNodesMap.begin(),_e=m_CancelledNodesMap.end(),
	//	oit=Other.m_CancelledNodesMap.begin();
	//for(;it!=_e;it++,oit++)
	//{
	//	if(it->first!=oit->first)
	//		return false;
	//	//if(GetContractedNode(it->second)!=GetContractedNode(oit->second))
	//	if(it->second!=oit->second)
	//		return false;
	//}
	//return true;
}
//void _CSubNodeContractionsSimpl::RemoveTouched(INT_SET& Set)
//{
//	for(CONTRACTION_LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
//	{
//		Set.erase(it->first);
//		Set.erase(iAbs(it->second));
//	}
//}

void _CSubNodeContractionsSimpl::SetTouched(INT_LIST& ExpectedList, CONTR_LIST& ContrList) const
{
	INT_LIST DoneList(1,m_CurrZero);
	ContrList[m_CurrZero].push_back(m_CurrZero);
	for(CONTRACTION_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		int a=it->first;
		int b=iAbs(it->second);
		DoneList.push_back(a);
		INT_LIST& List=ContrList[b];
		if(List.empty())
			List.push_back(b);
		List.push_back(a);
	}
	DoneList.sort();
	DoneList.unique();
	list_differ(ExpectedList,DoneList);
}

bool _CSubNodeContractionsSimpl::GlueItIfCanBeGluedTogether(const _CSubNodeContractionsSimpl& Another, short Sgn, short& OutSgn)
{
	return false;
}

IMPLEMENT_DYNAMIC_CREATION(_CSubNodeContractionsSimpl);

_CSubNodeContractionsSimpl::_NODE_RES _CSubNodeContractionsSimpl::Contract(int ANode, int BNode)
//int _CSubNodeContractions::Contract(int &ANode, int BNode/*,const COMPONENT_PINS_LIST& NewSeparateNodes*/)
{
	short Sgn = 1;
	unsigned UANode = iAbs(ANode);
	unsigned UBNode = iAbs(BNode);
	if (UANode<UBNode || (m_HasNegativeDels && (UANode == m_CurrZero)))
	{
		swap(ANode, BNode); swap(UANode, UBNode); Sgn = -Sgn;
	}
	if(IsBoundaryNode(UANode) && !IsBoundaryNode(UBNode))
	{
		swap(ANode, BNode); swap(UANode, UBNode); Sgn = -Sgn;
	}
	if (ANode<0)
	{
		ANode *= -1;
		BNode *= -1;
		Sgn *= -1;
	}
	if (ANode == -BNode)
	{
		BNode = UBNode = m_CurrZero;
		if (BNode>ANode)
		{
			Sgn *= -1;
			swap(ANode, BNode);
			swap(UANode, UBNode);
		}
		Sgn <<= 1;
	}
	int Pos = _insert(ANode, BNode);
	if (ANode == m_CurrZero)
	{
		m_CurrZero = UBNode;
		if (BNode<0)
			Sgn *= -1;
	}
	return _NODE_RES(Sgn, Pos);
}

bool _CSubNodeContractionsSimpl::FastLoopTest(int& ANode, int& BNode) const
{
	ANode = GetContractedNodeFast(ANode);
	BNode = GetContractedNodeFast(BNode);
	return ANode == BNode;
}

short _CSubNodeContractionsSimpl::CompOrderWith(const _CSubNodeContractionsSimpl& Right) const
{
	ASSERTPDD(m_List.size() == Right.m_List.size());
	auto mit = m_List.begin(), _me = m_List.end();
	auto rit = Right.m_List.begin();
	for (; mit != _me; mit++, rit++)
	{
		if (mit->first != rit->first)
			return (short)(mit->first - rit->first);
		if (mit->second != rit->second)
			return (short)(mit->second - rit->second);
	}
	if (m_HasNegativeDels)
		if (Right.m_HasNegativeDels)
		{
			if (m_CurrZero < Right.m_CurrZero)
				return -1;
			else if (m_CurrZero > Right.m_CurrZero)
				return 1;
		}
		else
		{
			if (m_CurrZero > 0)
				return 1;
		}
	else if (Right.m_HasNegativeDels)
		if (Right.m_CurrZero > 0)
			return -1;
	return 0;
}
//SL 2nd Comparer
_CSubNodeContractionsSimpl::COMP_RESULT _CSubNodeContractionsSimpl::Compare2Compress(_CSubNodeContractionsSimpl& Other, DIFFERS_RES& MyRes, DIFFERS_RES& OtherRes)
{
	ASSERTPDD(m_List.size() == Other.m_List.size());
	bool CZDonotCare = CurrZeroDontCare() && Other.CurrZeroDontCare();
	if (!CZDonotCare && m_CurrZero != Other.m_CurrZero)
		return TOO_MUCH_DIFFERENT;
	auto mit = m_List.begin(), _me = m_List.end();
	auto oit = Other.m_List.begin(), _oe = Other.m_List.end();
	size_t Pos = 0;
	bool MyDifFound = false, OtherDifFound = false;
	vector<bool> MyFounAnalyzed(m_List.size(), false), OthFounAnalyzed(m_List.size(), false);
	for (; mit != _me && oit != _oe; mit++, oit++, Pos++)
	{
		if (*mit == *oit)
		{
			MyFounAnalyzed[Pos] = true;
			OthFounAnalyzed[Pos] = true;
			continue;
		}
		auto next_oit = oit;
		bool MyIsOdd = !MyFounAnalyzed[Pos];
		size_t LocalPos = Pos;
		while (MyIsOdd && ++next_oit != _oe)
		{
			LocalPos++;
			if (*next_oit == *mit)
			{
				MyIsOdd = false;
				OthFounAnalyzed[LocalPos] = true;
				//break;
			}
		}
		if (MyIsOdd)
		{
			if (MyDifFound) return TOO_MUCH_DIFFERENT;
			MyDifFound = true;
			MyRes.first = Pos;
			MyRes.second = mit;
		}

		auto next_mit = mit;
		bool OthIsOdd = !OthFounAnalyzed[Pos];
		LocalPos = Pos;
		while (OthIsOdd && ++next_mit != _me)
		{
			LocalPos++;
			if (*next_mit == *oit)
			{
				OthIsOdd = false;
				MyFounAnalyzed[LocalPos] = true;
				break;
			}
		}
		if (OthIsOdd)
		{
			if (OtherDifFound) return TOO_MUCH_DIFFERENT;
			OtherDifFound = true;
			OtherRes.first = Pos;
			OtherRes.second = oit;
		}
	}

	//for (; mit != _me && oit != _oe; mit++, oit++, Pos++)
	//{
	//	if (*mit == *oit)
	//		continue;
	//	auto next_oit = oit;
	//	bool MyIsOdd = true;
	//	while(++next_oit != _oe) 
	//		if (*next_oit == *mit)
	//		{
	//			MyIsOdd = false;
	//			break;
	//		}
	//	auto next_mit = mit;
	//	bool OthIsOdd = true;
	//	while (++next_mit != _me)
	//		if (*next_mit == *oit)
	//		{
	//			OthIsOdd = false;
	//			break;
	//		}
	//	if (MyIsOdd)
	//	{
	//		if (MyDifFound) return TOO_MUCH_DIFFERENT;
	//		MyDifFound = true;
	//		MyRes.first = Pos;
	//		MyRes.second = mit;
	//	}
	//	if (OthIsOdd)
	//	{
	//		if (OtherDifFound) return TOO_MUCH_DIFFERENT;
	//		OtherDifFound = true;
	//		OtherRes.first = Pos;
	//		OtherRes.second = oit;
	//	}
	//	//if (MyRes.second->first != OtherRes.second->first || MyRes.second->second != -OtherRes.second->second)
	//	//{
	//	//	if (!CZDonotCare && mit->first != m_CurrZero && mit->second != m_CurrZero)
	//	//		return TOO_MUCH_DIFFERENT;
	//	//	if (!CZDonotCare && oit->first != m_CurrZero && oit->second != m_CurrZero)
	//	//		return TOO_MUCH_DIFFERENT;
	//	//}
	//}
	if (MyDifFound != OtherDifFound)
		return TOO_MUCH_DIFFERENT;
	if (MyDifFound)
		return TWO_DEFFERENTS;
	else 
		return EQUAL;
}
//SL to 2nd

_CSubNodeContractionsSimpl::ContrCompartator::ContrCompartator(_CSubNodeContractionsSimpl& me) : m_me(me), m_Result(TOO_MUCH_DIFFERENT), 
m_pMineDif(nullptr), m_pTheirDif(nullptr) {
}

void _CSubNodeContractionsSimpl::ContrCompartator::UpdateModyfication() {
	for (auto entry : m_ExchangePairList) {
		entry.first->second = entry.second;
	}
}

_CSubNodeContractionsSimpl::COMP_RESULT _CSubNodeContractionsSimpl::ContrCompartator::CompareWith(_CSubNodeContractionsSimpl& other){
	ASSERTPDD(m_me.m_List.size() == other.m_List.size());
	bool CZDonotCare = m_me.CurrZeroDontCare() && other.CurrZeroDontCare();
	if (!CZDonotCare && m_me.m_CurrZero != other.m_CurrZero)
		return (m_Result = TOO_MUCH_DIFFERENT);
	clear();
	InitList(other);
	auto mit = m_MineList.begin();
	auto _me = m_MineList.end();
	auto oit = m_Other_List.begin();
	auto _oe = m_Other_List.end();
	while (mit != _me || oit != _oe) {
		if (mit == _me) {
			if (oit == _oe)
				break;
			if (m_pTheirDif == nullptr) {
				m_pTheirDif = &*oit;
				break;
			}
			else
				return (m_Result = TOO_MUCH_DIFFERENT);
		}
		else if (oit == _oe) {
			if (m_pMineDif == nullptr) {
				m_pMineDif = &*mit;
				break;
			}
			else
				return (m_Result = TOO_MUCH_DIFFERENT);
		}
		else if (mit->second->first > oit->second->first) {
			if (m_pTheirDif == nullptr) {
				m_pTheirDif = &*oit++;
				continue;
			} else
				return (m_Result = TOO_MUCH_DIFFERENT);
		}
		else if (mit->second->first < oit->second->first) {
			if (m_pMineDif == nullptr) {
				m_pMineDif = &*mit++;
				continue;
			}
			else
				return (m_Result = TOO_MUCH_DIFFERENT);
		}
		else if (mit->second->second != oit->second->second) {
			////
			bool finallyOk = false;
			if (m_pMineDif != nullptr && abs(mit->second->second) == abs(m_pMineDif->second->second)) {
				int newMyTarget = sgn(mit->second->second)*sgn(m_pMineDif->second->second)*m_pMineDif->second->first;
				if (newMyTarget == oit->second->second) {
					m_ExchangePairList.push_back(EXCHANGE_PAIR(mit->second, newMyTarget));
					finallyOk = true;
				}
				else if (m_pTheirDif != nullptr && abs(oit->second->second) == abs(m_pTheirDif->second->second)) {
					int newTheirTarget = sgn(oit->second->second)*sgn(m_pTheirDif->second->second)*m_pTheirDif->second->first;
					if (newTheirTarget != newMyTarget)
						return (m_Result = TOO_MUCH_DIFFERENT);
					m_ExchangePairList.push_back(EXCHANGE_PAIR(mit->second, newMyTarget));
					finallyOk = true;
				}
			}
			else if (m_pTheirDif != nullptr && abs(oit->second->second) == abs(m_pTheirDif->second->second)) {
				int newTheirTarget = sgn(oit->second->second)*sgn(m_pTheirDif->second->second)*m_pTheirDif->second->first;
				if (newTheirTarget != mit->second->second)
					return (m_Result = TOO_MUCH_DIFFERENT);
				finallyOk = true;
			}

			if (!finallyOk){
				if (m_pMineDif == nullptr) {
					m_pMineDif = &*mit++;
					if (m_pTheirDif == nullptr)
						m_pTheirDif = &*oit++;
					continue;
				}
				else if (m_pTheirDif == nullptr) {
					m_pTheirDif = &*oit++;
					continue;
				}
				else {
					return (m_Result = TOO_MUCH_DIFFERENT);
				}

			}

			//else if (m_pTheirDif != nullptr || m_pMineDif != nullptr) {
			//	return (m_Result = TOO_MUCH_DIFFERENT);
			//}
			//else {
			//	m_pMineDif = &*mit;
			//	m_pTheirDif = &*oit;
			//}
		}
		mit++; oit++;
	}
	ASSERTPDD((m_pMineDif == nullptr && m_pTheirDif == nullptr) || (m_pMineDif != nullptr && m_pTheirDif != nullptr));
	return (m_Result = (m_pMineDif ==nullptr ? EQUAL: TWO_DEFFERENTS));
}

void _CSubNodeContractionsSimpl::ContrCompartator::InitList(_CSubNodeContractionsSimpl& other)
{
	
	auto itm = m_me.m_List.begin();
	auto _me = m_me.m_List.end();
	auto ito = other.m_List.begin();
	auto _oe = other.m_List.end();
	for (size_t i = 0; itm != _me; i++, itm++, ito++) {
		m_MineList.push_back(DIFFERS_RES(i, itm));
		m_Other_List.push_back(DIFFERS_RES(i, ito));
	}
}


bool _CSubNodeContractionsSimpl::CompressIt(COMP_RESULT Operation, _CSubNodeContractionsSimpl& Other, short& MySgn, short& OtherSgn, 
	DIFFERS_RES& MyRes, DIFFERS_RES& OtherRes)
{
	if (Operation == EQUAL)
	{
		MySgn -= OtherSgn;
		if (MySgn == 0)
			Clear();
		return true;
	}
	else if (Operation == TWO_DEFFERENTS)
	{
		if (!CurrZeroDontCare() && m_CurrZero != Other.m_CurrZero)
			return false;
		//_CSubNodeContractionsSimpl& A = Other;
		//A = *this;
		if (abs(MySgn) > abs(OtherSgn))
		{
			short SgnCorrect = sgn(MySgn) * sgn(OtherSgn);
			short Borrowed = SgnCorrect*OtherSgn;
			if(Other.CompressIt(Operation, *this, OtherSgn, Borrowed, OtherRes,MyRes))
				MySgn -= Borrowed;
			return false;
		}
		else if (abs(MySgn) < abs(OtherSgn))
		{
			short SgnCorrect = sgn(MySgn) * sgn(OtherSgn);
			short Borrowed = SgnCorrect*MySgn;
			if(CompressIt(Operation, Other, MySgn, Borrowed, MyRes, OtherRes))
				OtherSgn -= Borrowed;
			return false;
		}
		//if (MySgn != OtherSgn && MySgn != -OtherSgn)
		//	return false;
		int Mult = abs(MySgn);
		ASSERTPDD(Mult==abs(OtherSgn));
		int pureMySgn = MySgn / Mult;
		int pureOtherSgn = OtherSgn / Mult;
		if (IsOdd(MyRes.first - OtherRes.first))
			pureOtherSgn *= -1;

		CONTRACTION Old = *MyRes.second;
		CONTRACTION ChgOld = *OtherRes.second;
		int a1 = Old.first;
		int b1 = Old.second;
		int a2 = ChgOld.first;
		int b2 = ChgOld.second;
		int Common = {};
		
		if (abs(a1) == abs(b2)) {
			pureMySgn *= -1;
			swap(a1, b1);
		}
		else if (abs(b1) == abs(a2)) {
			pureOtherSgn *= -1;
			swap(a2, b2);
		}
		else if (abs(b1) == abs(b2) || abs(a1) == abs(a2)) {
			//do nothing
		}
		else return false;
		auto _sgn_multip = [this](int& a, int Sgn) {
			if (a != this->CurrentZero())
				a *= Sgn;
		};
		_sgn_multip(a1, pureMySgn);
		_sgn_multip(b1, pureMySgn);
		_sgn_multip(a2, -pureOtherSgn);
		_sgn_multip(b2, -pureOtherSgn);

		CONTRACTION NewContr;
		if (a1 == a2) {
			if (b1 > 0) {
				pureMySgn = -1;
			}
			else {
				pureMySgn = 1;
				b1 *= -1; b2 *= -1;
			}
			NewContr.first = b1;
			NewContr.second = b2;
		}
		else if (b1 == b2) {
			if (a1 < 0) {
				pureMySgn = -1; a1 *= -1; a2 *= -1;
			}
			else {
				pureMySgn = 1;
			}
			NewContr.first = a1;
			NewContr.second = a2;
		}
		else return false;

		MySgn = Mult * pureMySgn;

		int OldPos = _remove(Old.first);
		_NODE_RES Res = Contract(NewContr.first, NewContr.second);
		MySgn *= Res.first;
		if (IsOdd(OldPos - Res.second))
			MySgn *= -1;
		_Flatten();
		return true;
	}
	return false;
}

bool _CSubNodeContractionsSimpl::CompressIt2(COMP_RESULT Operation, _CSubNodeContractionsSimpl& Other, short& MySgn, short& OtherSgn,
	ContrCompartator& CompResults, bool testOnly)
{
	if (Operation == EQUAL)
	{
		MySgn -= OtherSgn;
		if (MySgn == 0)
			Clear();
		return true;
	}
	else if (Operation == TWO_DEFFERENTS)
	{
		if (!CurrZeroDontCare() && m_CurrZero != Other.m_CurrZero)
			return false;
		//_CSubNodeContractionsSimpl& A = Other;
		//A = *this;
		if (abs(MySgn) > abs(OtherSgn))
		{
			//VERIFYPDD(false);
			short SgnCorrect = sgn(MySgn) * sgn(OtherSgn);
			short Borrowed = SgnCorrect*OtherSgn;
			if (Other.CompressIt2(Operation, *this, OtherSgn, Borrowed, CompResults, true))
				MySgn -= Borrowed;
			return false;
		}
		else if (abs(MySgn) < abs(OtherSgn))
		{
			//VERIFYPDD(false);
			short SgnCorrect = sgn(MySgn) * sgn(OtherSgn);
			short Borrowed = SgnCorrect*MySgn;
			if (CompressIt2(Operation, Other, MySgn, Borrowed, CompResults, true))
				OtherSgn -= Borrowed;
			return false;
		}
		//if (MySgn != OtherSgn && MySgn != -OtherSgn)
		//	return false;
		int Mult = abs(MySgn);
		ASSERTPDD(Mult == abs(OtherSgn));
		int pureMySgn = MySgn / Mult;
		int pureOtherSgn = OtherSgn / Mult;
		if (IsOdd(CompResults.pMyyPairToCompres()->first - CompResults.pThierPairToCompres()->first))
			pureOtherSgn *= -1;

		CONTRACTION Old = *(CompResults.pMyyPairToCompres()->second);
		CONTRACTION ChgOld = *(CompResults.pThierPairToCompres()->second);
		int a1 = Old.first;
		int b1 = Old.second;
		int a2 = ChgOld.first;
		int b2 = ChgOld.second;

		if (abs(a1) == abs(b2)) {
			pureMySgn *= -1;
			swap(a1, b1);
		}
		else if (abs(b1) == abs(a2)) {
			pureOtherSgn *= -1;
			swap(a2, b2);
		}
		else if (abs(b1) == abs(b2) || abs(a1) == abs(a2)) {
			//do nothing
		}
		else return false;
		auto _sgn_multip = [this](int& a, int Sgn) {
			if (a != this->CurrentZero())
				a *= Sgn;
		};
		_sgn_multip(a1, pureMySgn);
		_sgn_multip(b1, pureMySgn);
		_sgn_multip(a2, -pureOtherSgn);
		_sgn_multip(b2, -pureOtherSgn);

		CONTRACTION NewContr;
		if (a1 == a2) {
			if (b1 > 0) {
				pureMySgn = -1;
			}
			else {
				pureMySgn = 1;
				b1 *= -1; b2 *= -1;
			}
			NewContr.first = b1;
			NewContr.second = b2;
		}
		else if (b1 == b2) {
			if (a1 < 0) {
				pureMySgn = -1; a1 *= -1; a2 *= -1;
			}
			else {
				pureMySgn = 1;
			}
			NewContr.first = a1;
			NewContr.second = a2;
		}
		else return false;
		if (!testOnly) {
			if (CompResults.needsModification()) {
				CompResults.UpdateModyfication();
			}
			MySgn = Mult * pureMySgn;

			int OldPos = _remove(Old.first);
			_NODE_RES Res = Contract(NewContr.first, NewContr.second);
			MySgn *= Res.first;
			if (IsOdd(OldPos - Res.second))
				MySgn *= -1;
			_Flatten();
		}
		return true;
	}
	return false;
}

bool _CSubNodeContractionsSimpl::Split1IfNeeded(_CSubNodeContractionsSimpl& Second, short& Sgn)
{
	if (!m_HasNegativeDels)
		return false;
	Second.m_AuxTestList.clear();
	Second.m_CurrZero = m_CurrZero;
	Second.m_ModifyList.clear();
	Second.m_HasNegativeDels = false;
	bool IsFirst = false;
	int Pos = 0, OldPos = 0;
	_CSubNodeContractionsSimpl::CONTRACTION NewContact;
	for (auto it = m_List.begin(), _e = m_List.end(); it != _e; it++, Pos++)
	{
		auto& Contract = *it;
		if (Contract.second < 0)
		{
			if (!IsFirst)
			{
				IsFirst = true;
				NewContact.first = -Contract.second;
				Contract.second = m_CurrZero;
				NewContact.second = m_CurrZero;
				OldPos = Pos;
				m_HasNegativeDels = Second.m_HasNegativeDels = false;
			}
			else
			{
				m_HasNegativeDels = Second.m_HasNegativeDels = true;
				Second.m_List.push_back(Contract);
			}
		}
		else
			Second.m_List.push_back(Contract);
	}
	_Flatten();
	_NODE_RES Res = Second.Contract(NewContact.first, NewContact.second);//dokiñcz
	Second._Flatten();
	Sgn *= Res.first;
	if (IsOdd(Res.second - OldPos))
		Sgn *= -1;
	return true;
}

//*******************************_CSubNodeContractionsSimpl*******************
//*******************************_CSubNodeContractions************************


void _CSubNodeContractions::AddTestContract(int& ANode, int& BNode)
{
	if(iAbs(ANode)==0 || (m_pAnalyser->IsBoundary(iAbs(ANode)) && !m_pAnalyser->IsBoundary(iAbs(BNode))))
	{
		swap(ANode,BNode); 
	}
	if(ANode==-BNode)
		BNode=m_CurrZero;
	if(iAbs(BNode)>iAbs(ANode))
		swap(ANode,BNode);
	if(ANode<0)
	{
		ANode*=-1;
		BNode*=-1;
	}
	_insert(ANode,BNode);
	if(m_CurrZero==ANode)
		m_CurrZero=iAbs(BNode);
}

bool _CSubNodeContractions::GreedyTestContract(int& ANode, int& BNode)
{
	_translate_and_update(ANode);
	_translate_and_update(BNode);
	return ANode==BNode;
}

//_CSubNodeContractions::_CConnectedNodes::_CConnectedNodes(const CANCELLED_NODES_MAP& Source)
_CSubNodeContractions::_CConnectedNodes::_CConnectedNodes(const CONTRACTION_LIST& Source,int CurrZero)
{
	//for(CANCELLED_NODES_MAP::const_iterator it=Source.begin(),_e=Source.end();it!=_e;it++)
	for(_const_iterator it=Source.begin(),_e=Source.end();it!=_e;it++)
	{
		int B=it->second;
		insert(_CConnectedNodes::value_type(iAbs(B),it->first));
		if(B<0)
			insert(_CConnectedNodes::value_type(CurrZero,it->first));
	}
}



bool _CSubNodeContractions::IsSeparateNodeDisconneced(const COMPONENT_PINS_LIST& SeparateNodes,const _CGlobalIncidenceTable& GlobalTable) const
{
	if(SeparateNodes.empty())
		return false;
	//COMPONENT_PINS_LIST Copy(SeparateNodes);
	COMPONENT_PINS_SET CopySet;
	for(COMPONENT_PINS_LIST::const_iterator pit=SeparateNodes.begin(),_pe=SeparateNodes.end();
		pit!=_pe;pit++)
	{
		int SeparateNode=*pit;
		//if(m_HasNegativeDels && SeparateNode==m_CurrZero)
		//if(SeparateNode==0 && m_HasNegativeDels)
		//	continue;
		//CANCELLED_NODES_MAP::const_iterator cit=m_CancelledNodesMap.find(SeparateNode);
		_const_iterator cit=_find_existed(SeparateNode);
		//if(cit==m_CancelledNodesMap.end())
		if(cit==m_List.end())
			CopySet.insert(SeparateNode);
		else
			if(GlobalTable.GetCurrentIncidences(iAbs(cit->second))==0)
				CopySet.insert(iAbs(cit->second));
	}
	if(CopySet.empty())
		return false;
	//_CConnectedNodes ConNodes(m_CancelledNodesMap);
	_CConnectedNodes ConNodes(m_List,m_CurrZero);
	for(COMPONENT_PINS_SET::iterator pit=CopySet.begin(),_pe=CopySet.end();
		pit!=_pe;pit++)
	{
		int SeparateNode=*pit;
		pair<_CConnectedNodes::iterator,_CConnectedNodes::iterator> Range=
			ConNodes.equal_range(SeparateNode);
		if(Range.first==Range.second)
			return true;
		bool Found=false;
		for(_CConnectedNodes::iterator it=Range.first;it!=Range.second;it++)
			if(GlobalTable.GetCurrentIncidences(iAbs(it->second))>0)
			{
				Found=true;
				break;
			}
		if(!Found)
			return true;
	}
	return false;
}


bool _CSubNodeContractions::IsPinConnected(int Node) const
{
	//for(CANCELLED_NODES_MAP::const_iterator it=m_CancelledNodesMap.begin(),_e=m_CancelledNodesMap.end();it!=_e;it++)
	for(_const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		if(it->first==Node || iAbs(it->second)==Node)
			return true;
	return false;
}

bool _CSubNodeContractions::IsPinConnected(const COMPONENT_PINS_SET& DisconnnectedPins) const
{
	COMPONENT_PINS_SET Copy(DisconnnectedPins);
	//for(CANCELLED_NODES_MAP::const_iterator it=m_CancelledNodesMap.begin(),_e=m_CancelledNodesMap.end();it!=_e;it++)
	for(_const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		Copy.erase(it->first);
		Copy.erase(it->second);
		if(Copy.empty())
			return true;
	}
	return false;
}

bool _CSubNodeContractions::IsPinConnected(const COMPONENT_PINS_LIST& DisconnnectedPins) const
{
	COMPONENT_PINS_LIST Copy(DisconnnectedPins);
	//for(CANCELLED_NODES_MAP::const_iterator it=m_CancelledNodesMap.begin(),_e=m_CancelledNodesMap.end();it!=_e;it++)
	for(_const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		Copy.remove(it->first);
		Copy.remove(it->second);
		if(Copy.empty())
			return true;
	}
	return false;
}

bool _CSubNodeContractions::IsEnough() const
{
	for(CONTRACTION_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		if(!m_pAnalyser->IsBoundary(iAbs(it->first)) || !m_pAnalyser->IsBoundary(iAbs(it->second)))
			return false;
	}

	return true;
}

bool _CSubNodeContractions::IsBoundaryNode(size_t Node) 
{ 
	return m_pAnalyser->IsBoundary(Node); 
}


//*******************************_CSubNodeContractions************************

//*******************************_CContractionPairSimpl***********************
_CContractionPairSimpl::_CContractionPairSimpl(const _CContractionPair& Source)
	:m_Sgn(Source.m_Sgn), m_R(Source.m_R), m_C(Source.m_C), m_State(Source.m_State)
{

}
//SL 2nd Comparer
//bool _CContractionPairSimpl::Try2Compress(_CContractionPairSimpl& Other)
//{
//	//SL New Implementation
//	_CSubNodeContractionsSimpl::DIFFERS_RES MyResR, OtherResR;
//	_CSubNodeContractionsSimpl::COMP_RESULT ResR = m_R.Compare2Compress(Other.m_R, MyResR, OtherResR);
//	if (ResR == _CSubNodeContractionsSimpl::TOO_MUCH_DIFFERENT)
//		return false;
//	_CSubNodeContractionsSimpl::DIFFERS_RES MyResC, OtherResC;
//	_CSubNodeContractionsSimpl::COMP_RESULT ResC = m_C.Compare2Compress(Other.m_C, MyResC, OtherResC);
//	if (ResC == _CSubNodeContractionsSimpl::TOO_MUCH_DIFFERENT)
//		return false;
//	if (ResR == _CSubNodeContractionsSimpl::EQUAL)
//		if (ResC == _CSubNodeContractionsSimpl::EQUAL)
//		{
//			//m_C.CompressIt(ResC, Other.m_C, m_Sgn, Other.m_Sgn, MyResC, OtherResC);
//			//m_R.CompressIt(ResR, Other.m_R, m_Sgn, Other.m_Sgn, MyResR, OtherResR);
//			m_Sgn += Other.m_Sgn;
//			if (m_Sgn == 0)
//				m_State = AD_LOOP;
//			return true;
//		}
//		else if (ResC == _CSubNodeContractionsSimpl::TWO_DEFFERENTS)
//		{
//			return m_C.CompressIt(ResC, Other.m_C, m_Sgn, Other.m_Sgn, MyResC, OtherResC);
//		}
//		else
//			return false;
//	else
//		if (ResC == _CSubNodeContractionsSimpl::EQUAL)
//		{
//			return m_R.CompressIt(ResR, Other.m_R, m_Sgn, Other.m_Sgn, MyResR, OtherResR);
//		}
//		else
//			return false;
//}
bool _CContractionPairSimpl::Try2Compress2(_CContractionPairSimpl& Other)
{
	//SL New Implementation
	if (abs(m_Sgn) != abs(Other.m_Sgn))
		return false;
	_CSubNodeContractionsSimpl::ContrCompartator rowComp(m_R);
	auto ResR = rowComp.CompareWith(Other.m_R);
	if (ResR == _CSubNodeContractionsSimpl::TOO_MUCH_DIFFERENT)
		return false;

	_CSubNodeContractionsSimpl::ContrCompartator colComp(m_C);
	auto ResC = colComp.CompareWith(Other.m_C);
	if (ResC == _CSubNodeContractionsSimpl::TOO_MUCH_DIFFERENT)
		return false;

	if (ResR == _CSubNodeContractionsSimpl::EQUAL)
		if (ResC == _CSubNodeContractionsSimpl::EQUAL)
		{
			//m_C.CompressIt(ResC, Other.m_C, m_Sgn, Other.m_Sgn, MyResC, OtherResC);
			//m_R.CompressIt(ResR, Other.m_R, m_Sgn, Other.m_Sgn, MyResR, OtherResR);
			m_Sgn += Other.m_Sgn;
			if (m_Sgn == 0)
				m_State = AD_LOOP;
			return true;
		}
		else if (ResC == _CSubNodeContractionsSimpl::TWO_DEFFERENTS)
		{
			return m_C.CompressIt2(ResC, Other.m_C, m_Sgn, Other.m_Sgn, colComp);
		}
		else
			return false;
	else
		if (ResC == _CSubNodeContractionsSimpl::EQUAL)
		{
			return m_R.CompressIt2(ResR, Other.m_R, m_Sgn, Other.m_Sgn, rowComp);
		}
		else
			return false;
}

bool _CContractionPairSimpl::Try2Split(_CContractionPairSimpl& NewReminder)
{
	NewReminder.m_Sgn = m_Sgn;
	if (m_C.Split1IfNeeded(NewReminder.m_C, NewReminder.m_Sgn))
	{
		NewReminder.m_R = m_R;
		return true;
	}
	else if (m_R.Split1IfNeeded(NewReminder.m_R, NewReminder.m_Sgn))
	{
		NewReminder.m_C = m_C;
		return true;
	}
	return false;
}

bool _CContractionPairSimpl::SplitRows(_CContractionPairSimpl& NewReminder)
{
	NewReminder.m_Sgn = m_Sgn;
	if (m_R.m_HasNegativeDels && m_R.Split1IfNeeded(NewReminder.m_R, NewReminder.m_Sgn))
	{
		NewReminder.m_C = m_C;
		return true;
	}
	return false;
}

bool _CContractionPairSimpl::SplitCols(_CContractionPairSimpl& NewReminder)
{
	NewReminder.m_Sgn = m_Sgn;
	if (m_C.m_HasNegativeDels && m_C.Split1IfNeeded(NewReminder.m_C, NewReminder.m_Sgn))
	{
		NewReminder.m_R = m_R;
		return true;
	}
	return false;
}

unsigned long long _CContractionPairSimpl::GetHash(unsigned long long Core)
{
	Core *= HASH_FACTOR;
	Core ^= m_Sgn;
	Core = m_R.GetHash(Core);
	Core = m_C.GetHash(Core);
	return Core;
}

//AD_DELETION _CContractionPairSimpl::AddDeletionPair(int P, int R, int K, int L,
//	short Mult, const COMPONENT_PINS_LIST& NewSeparateNodes)
//{
//	return AD_LOOP;
//}

AD_DELETION _CContractionPairSimpl::RemoveNode(int Node)
{
	CAN_REM RCan = m_R.CanRemove(Node);
	CAN_REM CCan = m_C.CanRemove(Node);
	if (RCan == _CSubNodeContractionsSimpl::DISCONNECT || CCan == _CSubNodeContractionsSimpl::DISCONNECT)
		return (m_State = AD_DISCONNECTION);
	if (RCan == _CSubNodeContractionsSimpl::CAN && CCan == _CSubNodeContractionsSimpl::CAN)
	{
		_NODE_RES RR = m_R._IgnoreNode(Node);
		_NODE_RES CR = m_C._IgnoreNode(Node);
		m_Sgn *= RR.first;
		m_Sgn *= CR.first;
		if (IsOdd(RR.second - CR.second))
			m_Sgn *= (-1);
		//m_HighestOrder--;
	}
	return (m_State = AD_OK);
}

AD_DELETION _CContractionPairSimpl::RemoveNodes(const COMPONENT_PINS_LIST& NewSeparateNodes)
{
	for (int Node : NewSeparateNodes)
		if (RemoveNode(Node) != AD_OK)
			break;
	return m_State;
}

AD_DELETION _CContractionPairSimpl::Translate(const NODES_CONNECTION& NodesConnections)
{
	if (!IsOkOrComplete())
		return m_State;
	_CSubNodeContractionsSimpl::POS_LIST RList;
	short RSgn = m_R.TranslateMe(NodesConnections, RList);
	if (RSgn == 0)
		return (m_State = AD_LOOP);
	_CSubNodeContractionsSimpl::POS_LIST CList;
	short CSgn = m_C.TranslateMe(NodesConnections, CList);
	if (CSgn == 0)
		return (m_State = AD_LOOP);
	m_Sgn *= RSgn;
	m_Sgn *= CSgn;
	for (_CSubNodeContractionsSimpl::POS_LIST::iterator rit = RList.begin(), _re = RList.end(),
		cit = CList.begin(), _ce = CList.end(); rit != _re && cit != _ce; rit++, cit++)
		if (IsOdd(*rit - *cit))
			m_Sgn *= -1;
	return m_State;
}

void _CContractionPairSimpl::_Store(_binary_filer& Filer)
{
	Filer << m_Sgn;
	Filer << (unsigned)m_State;
	Filer << m_R;
	Filer << m_C;
}

void _CContractionPairSimpl::_Load(_binary_filer& Filer)
{
	Filer >> m_Sgn;
	unsigned state;
	Filer >> state;
	m_State=(AD_DELETION)state;
	Filer >> m_R;
	Filer >> m_C;
}

void _CContractionPairSimpl::ReverseCofactor(_CCofactor& OutCofactor, short GlobalSgn, size_t Index) const
{
	ASSERTPDD(m_C.NoOfDel() == m_R.NoOfDel());
	const _CSubNodeContractions::CONTRACTION_LIST& RList = m_R.GetNodesList();
	const _CSubNodeContractions::CONTRACTION_LIST& CList = m_C.GetNodesList();
	auto Rit = RList.begin(), _Re = RList.end();
	auto Cit = CList.begin(), _Ce = CList.end();
	for (; Rit != _Re; Rit++, Cit++)
	{
		int P = Rit->first, R = Rit->second, K = Cit->first, L = Cit->second;
		OutCofactor.AddDeletions(P, R, K, L, false, false, Index);
	}
	OutCofactor.MultSgn(m_Sgn*GlobalSgn); 
}

void _CContractionPairSimpl::ReverseCofactor(_CBasicSetOfDeletions& OutCofactor, short GlobalSgn) const
{
	ASSERTPDD(m_C.NoOfDel() == m_R.NoOfDel());
	const _CSubNodeContractions::CONTRACTION_LIST& RList = m_R.GetNodesList();
	const _CSubNodeContractions::CONTRACTION_LIST& CList = m_C.GetNodesList();
	auto Rit = RList.begin(), _Re = RList.end();
	auto Cit = CList.begin(), _Ce = CList.end();
	for (; Rit != _Re; Rit++, Cit++)
	{
		int P = Rit->first, R = Rit->second, K = Cit->first, L = Cit->second;
		OutCofactor.push_back(_CBasicPairOfDeletion(P, R, K, L));
	}
	OutCofactor.MultSgn(m_Sgn*GlobalSgn);
}

void _CContractionPairSimpl::ReverseCofactor(_CDeletions& OutCofactor, short GlobalSgn) const
{
	ASSERTPDD(m_C.NoOfDel() == m_R.NoOfDel());
	const _CSubNodeContractions::CONTRACTION_LIST& RList = m_R.GetNodesList();
	const _CSubNodeContractions::CONTRACTION_LIST& CList = m_C.GetNodesList();
	auto Rit = RList.begin(), _Re = RList.end();
	auto Cit = CList.begin(), _Ce = CList.end();
	for (; Rit != _Re; Rit++, Cit++)
	{
		int P = Rit->first, R = Rit->second, K = Cit->first, L = Cit->second;
		OutCofactor.push_in_order(new _CDeletion(NULL, P, R, K, L));
	}
	OutCofactor.SetMultiplier(m_Sgn*GlobalSgn);
}


//*******************************_CContractionPairSimpl***********************
//*******************************_CContractionPair****************************

_CContractionPair::_CContractionPair(_CCircuitAnalyzer* pAnalyzer) :m_Sgn(1), m_State(AD_OK), m_R(pAnalyzer), m_C(pAnalyzer), m_pAnalyzer(pAnalyzer) 
{
	m_HighestOrder = m_pAnalyzer->HighestNoOfNodes();
}

_CContractionPair::_CContractionPair(const _CSubNodeContractions& C, const _CSubNodeContractions& R, short Sgn) :
m_C(C), m_R(R), m_Sgn(Sgn), m_State(AD_OK)
{
	m_HighestOrder = C.GetAnalyser()->HighestNoOfNodes();
}

_CContractionPair::_CContractionPair(const _CSubNodeContractionsSimpl& C, const _CSubNodeContractionsSimpl& R, short Sgn, _CCircuitAnalyzer* pAnalyzer) :
m_C(C, pAnalyzer), m_R(R, pAnalyzer), m_Sgn(Sgn), m_State(AD_OK)
{
	m_HighestOrder = m_pAnalyzer->HighestNoOfNodes();
}

_CContractionPair::_CContractionPair(const _CContractionPairSimpl& Source, _CCircuitAnalyzer* pAnalyzer)
	:m_Sgn(Source.m_Sgn), m_R(Source.m_R, pAnalyzer), m_C(Source.m_C, pAnalyzer), m_pAnalyzer(pAnalyzer), m_State(Source.m_State)
{
	m_HighestOrder = m_pAnalyzer->HighestNoOfNodes();
}

bool _CContractionPair::Try2Compress(_CContractionPair& Other)
{
	_CSubNodeContractionsSimpl::DIFFERS_RES MyResR, OtherResR;
	_CSubNodeContractionsSimpl::COMP_RESULT ResR = m_R.Compare2Compress(Other.m_R, MyResR, OtherResR);
	if (ResR == _CSubNodeContractionsSimpl::TOO_MUCH_DIFFERENT)
		return false;
	_CSubNodeContractionsSimpl::DIFFERS_RES MyResC, OtherResC;
	_CSubNodeContractionsSimpl::COMP_RESULT ResC = m_C.Compare2Compress(Other.m_C, MyResC, OtherResC);
	if (ResC == _CSubNodeContractionsSimpl::TOO_MUCH_DIFFERENT)
		return false;
	if (ResR == _CSubNodeContractionsSimpl::EQUAL)
		if (ResC == _CSubNodeContractionsSimpl::EQUAL)
		{
			//m_C.CompressIt(ResC, Other.m_C, m_Sgn, Other.m_Sgn, MyResC, OtherResC);
			//m_R.CompressIt(ResR, Other.m_R, m_Sgn, Other.m_Sgn, MyResR, OtherResR);
			m_Sgn += Other.m_Sgn;
			if (m_Sgn == 0)
				m_State = AD_LOOP;
			return true;
		}
		else if (ResC == _CSubNodeContractionsSimpl::TWO_DEFFERENTS)
		{
			return m_C.CompressIt(ResC, Other.m_C, m_Sgn, Other.m_Sgn, MyResC, OtherResC);
		}
		else
			return false;
	else
		if (ResC == _CSubNodeContractionsSimpl::EQUAL)
		{
			return m_R.CompressIt(ResR, Other.m_R, m_Sgn, Other.m_Sgn, MyResR, OtherResR);
		}
		else
			return false;
}

bool _CContractionPair::Try2Compress2(_CContractionPair& Other)
{
	//SL New Implementation
	//if (abs(m_Sgn) != abs(Other.m_Sgn))
	//	return false;
	_CSubNodeContractionsSimpl::ContrCompartator rowComp(m_R);
	auto ResR = rowComp.CompareWith(Other.m_R);
	if (ResR == _CSubNodeContractionsSimpl::TOO_MUCH_DIFFERENT)
		return false;

	_CSubNodeContractionsSimpl::ContrCompartator colComp(m_C);
	auto ResC = colComp.CompareWith(Other.m_C);
	if (ResC == _CSubNodeContractionsSimpl::TOO_MUCH_DIFFERENT)
		return false;

	if (ResR == _CSubNodeContractionsSimpl::EQUAL)
		if (ResC == _CSubNodeContractionsSimpl::EQUAL)
		{
			//m_C.CompressIt(ResC, Other.m_C, m_Sgn, Other.m_Sgn, MyResC, OtherResC);
			//m_R.CompressIt(ResR, Other.m_R, m_Sgn, Other.m_Sgn, MyResR, OtherResR);
			m_Sgn += Other.m_Sgn;
			if (m_Sgn == 0)
				m_State = AD_LOOP;
			return true;
		}
		else if (ResC == _CSubNodeContractionsSimpl::TWO_DEFFERENTS)
		{
			return m_C.CompressIt2(ResC, Other.m_C, m_Sgn, Other.m_Sgn, colComp);
		}
		else
			return false;
	else
		if (ResC == _CSubNodeContractionsSimpl::EQUAL)
		{
			return m_R.CompressIt2(ResR, Other.m_R, m_Sgn, Other.m_Sgn, rowComp);
		}
		else
			return false;
}

bool _CContractionPair::Try2Split(_CContractionPair& NewReminder)
{
	NewReminder.m_Sgn = m_Sgn;
	if (m_C.m_HasNegativeDels && m_C.Split1IfNeeded(NewReminder.m_C, NewReminder.m_Sgn))
	{
		NewReminder.m_R = m_R;
		return true;
	}
	else if (m_R.m_HasNegativeDels && m_R.Split1IfNeeded(NewReminder.m_R, NewReminder.m_Sgn))
	{
		NewReminder.m_C = m_C;
		return true;
	}
	return false;
}

bool _CContractionPair::SplitRows(_CContractionPair& NewReminder)
{
	NewReminder.m_Sgn = m_Sgn;
	NewReminder.m_HighestOrder = m_HighestOrder;
	if (m_R.m_HasNegativeDels && m_R.Split1IfNeeded(NewReminder.m_R, NewReminder.m_Sgn))
	{
		NewReminder.m_C = m_C;
		return true;
	}
	return false;
}

bool _CContractionPair::SplitCols(_CContractionPair& NewReminder)
{
	NewReminder.m_Sgn = m_Sgn;
	NewReminder.m_HighestOrder = m_HighestOrder;
	if (m_C.m_HasNegativeDels && m_C.Split1IfNeeded(NewReminder.m_C, NewReminder.m_Sgn))
	{
		NewReminder.m_R = m_R;
		return true;
	}
	return false;
}
unsigned long long _CContractionPair::GetHash(unsigned long long Core)
{
	Core *= HASH_FACTOR;
	Core ^= m_Sgn;
	Core *= HASH_FACTOR;
	Core ^= m_HighestOrder;
	Core = m_R.GetHash(Core);
	Core = m_C.GetHash(Core);
	return Core;
}

AD_DELETION _CContractionPair::AddDeletionPair(int P, int R, int K, int L)
{
	if (m_State == AD_COMPLETE || m_R.FastLoopTest(P, R) || m_C.FastLoopTest(K, L))
		return (m_State = AD_LOOP);
	_NODE_RES MultR = m_R.Contract(P, R);
	_NODE_RES MultC = m_C.Contract(K, L);
	//m_Sgn *= Mult;
	m_Sgn *= MultR.first;
	m_Sgn *= MultC.first;
	if (IsOdd(MultR.second - MultC.second))
		m_Sgn *= (-1);
	m_R._Flatten();
	m_C._Flatten();
	if (IsCompleteTest())
		return (m_State = AD_COMPLETE);
	//if (m_R.IsSeparateNodeDisconneced(NewSeparateNodes, m_pAnalyzer->m_IncidentTable))
	//	return (m_State = AD_DISCONNECTION);
	//if (m_C.IsSeparateNodeDisconneced(NewSeparateNodes, m_pAnalyzer->m_IncidentTable))
	//	return (m_State = AD_DISCONNECTION);
	return (m_State = AD_OK);
}

bool _CContractionPair::IsEnoughTest() const
{
	if (m_pAnalyzer->IsSubmodel())
		return IsEnough();
	else
		return IsCompleteTest();
}

AD_DELETION _CContractionPair::RemoveNode(int Node)
{
	if (m_HighestOrder == 0)
		return (m_State = AD_COMPLETE);
	_CSubNodeContractionsSimpl::CAN_REM RCan = m_R.CanRemove(Node);
	if (RCan == _CSubNodeContractionsSimpl::DISCONNECT)
		return (m_State = AD_DISCONNECTION);
	_CSubNodeContractionsSimpl::CAN_REM CCan = m_C.CanRemove(Node);
	if (CCan == _CSubNodeContractionsSimpl::DISCONNECT)
		return (m_State = AD_DISCONNECTION);
	if (RCan == _CSubNodeContractionsSimpl::NEED2SPLIT)
		if (CCan == _CSubNodeContractionsSimpl::NEED2SPLIT)
			return (m_State = AD_NEED_SPLIY_BOTH);
		else
			return (m_State = AD_NEED_SPLIT_ROW);
	else if (CCan == _CSubNodeContractionsSimpl::NEED2SPLIT)
		return (m_State = AD_NEED_SPLIT_COL);
	_NODE_RES RR = m_R._IgnoreNode(Node);
	_NODE_RES CR = m_C._IgnoreNode(Node);
	m_Sgn *= RR.first;
	m_Sgn *= CR.first;
	if (IsOdd(RR.second - CR.second))
		m_Sgn *= (-1);
	m_HighestOrder--;
	return (m_State=IsCompleteTest()?AD_COMPLETE:AD_OK);
}

AD_DELETION _CContractionPair::RemoveNodes(const COMPONENT_PINS_LIST& NewSeparateNodes, SPLITED_LIST& SplitedList)
{
	bool Need2SplitRow = false, Need2SplitCol = false;
	for (int Node : NewSeparateNodes)
	{
		m_State = RemoveNode(Node);
		if (m_State == AD_NEED_SPLIT_ROW)
		{
			Need2SplitRow = true;
			//SeparateNodes2Repeat.push_back(Node);
		} 
		else if (m_State == AD_NEED_SPLIT_COL)
		{
			Need2SplitCol = true;
			//SeparateNodes2Repeat.push_back(Node);
		}
		else if (m_State == AD_NEED_SPLIY_BOTH)
		{
			Need2SplitRow = Need2SplitCol = true;
			//SeparateNodes2Repeat.push_back(Node);
		}
		else if (!IsOkOrComplete())
			break;
	}
	if (m_State != AD_LOOP && m_State != AD_DISCONNECTION)
		if (Need2SplitRow)
			if (Need2SplitCol)
				m_State = AD_NEED_SPLIY_BOTH;
			else
				m_State = AD_NEED_SPLIT_ROW;
		else
			m_State = AD_NEED_SPLIT_COL;
	return m_State;
}

void _CContractionPair::ReverseCofactor(_CCofactor& OutCofactor, short GlobalSgn, size_t Index) const
{
	ASSERTPDD(m_C.NoOfDel() == m_R.NoOfDel());
	const _CSubNodeContractions::CONTRACTION_LIST& RList = m_R.GetNodesList();
	const _CSubNodeContractions::CONTRACTION_LIST& CList = m_C.GetNodesList();
	auto Rit = RList.begin(), _Re = RList.end();
	auto Cit = CList.begin(), _Ce = CList.end();
	for (; Rit != _Re; Rit++, Cit++)
	{
		int P = Rit->first, R = Rit->second, K = Cit->first, L = Cit->second;
		OutCofactor.AddDeletions(P, R, K, L, false, false,Index);
	}
	OutCofactor.MultSgn(m_Sgn*GlobalSgn);
}

void _CContractionPair::ReverseCofactor(_CBasicSetOfDeletions& OutCofactor, short GlobalSgn) const
{
	ASSERTPDD(m_C.NoOfDel()==m_R.NoOfDel());
	const _CSubNodeContractions::CONTRACTION_LIST& RList=m_R.GetNodesList();
	const _CSubNodeContractions::CONTRACTION_LIST& CList=m_C.GetNodesList();
	auto Rit=RList.begin(),_Re=RList.end();
	auto Cit=CList.begin(),_Ce=CList.end();
	for(;Rit!=_Re;Rit++,Cit++)
	{
		int P=Rit->first, R=Rit->second, K=Cit->first, L=Cit->second;
		OutCofactor.push_back(_CBasicPairOfDeletion(P,R,K,L));
	}
	OutCofactor.MultSgn(m_Sgn*GlobalSgn);
}
void _CContractionPair::ReverseCofactor(_CDeletions& OutCofactor, short GlobalSgn) const
{
	ASSERTPDD(m_C.NoOfDel() == m_R.NoOfDel());
	const _CSubNodeContractions::CONTRACTION_LIST& RList = m_R.GetNodesList();
	const _CSubNodeContractions::CONTRACTION_LIST& CList = m_C.GetNodesList();
	auto Rit = RList.begin(), _Re = RList.end();
	auto Cit = CList.begin(), _Ce = CList.end();
	for (; Rit != _Re; Rit++, Cit++)
	{
		int P = Rit->first, R = Rit->second, K = Cit->first, L = Cit->second;
		OutCofactor.push_in_order(new _CDeletion(m_pAnalyzer->m_pBaseCircuit, P, R, K, L));
	}
	OutCofactor.SetMultiplier(m_Sgn*GlobalSgn);
}

//*******************************_CContractionPair****************************
//*******************************_CContractionListImpl************************

_CContractionListImpl::_CContractionListImpl(const _CContractionList& Source)
	:m_GlobalSgn(Source.m_GlobalSgn), m_State(Source.m_State)
{
	m_List.assign(Source.m_List.begin(), Source.m_List.end());
}

void _CContractionListImpl::insert_in_order(_CContractionPairSimpl& Source)
{
	LIST AuxList(1, Source);
	m_List.merge(AuxList, 
		[](const _CContractionPairSimpl& Left, const _CContractionPairSimpl& Right)
	{
		short C = Left.m_C.CompOrderWith(Right.m_C);
		if (C != 0)
			return C < 0;
		C = Left.m_R.CompOrderWith(Right.m_R);
		if (C != 0)
			return C < 0;
		return Left.m_Sgn < Right.m_Sgn;
	});
}

void _CContractionListImpl::Compress()
{
	if (m_List.size() > 1)
	{
		bool WasSimplification = false;
		do
		{
			WasSimplification = false;
			for (auto fit = m_List.begin(), _e = m_List.end(); fit != _e;)
			{
				if (fit == _e)
					break;
				auto sit = fit;
				sit++;
				for (; sit != _e;)
				{
					if (fit->Try2Compress2(*sit))
					{
						WasSimplification = true;
						sit = m_List.erase(sit);
						if (fit->IsSelfLoop())
							break;
					}
					else
						sit++;
				}
				if (fit->IsSelfLoop())
					fit = m_List.erase(fit);
				else
					fit++;
			}
		} while (WasSimplification);
		if (m_List.empty())
			m_State = AD_DISCONNECTION;
		else
			RetriveCommonSgn();
	}
}

void _CContractionListImpl::Split()
{
	bool ElementWasSplited = false;
	do
	{
		for (auto fit = m_List.begin(), _e = m_List.end(); fit != _e; fit++)
		{
			_CContractionPairSimpl NewReminder;
			if (fit->Try2Split(NewReminder))
			{
				insert_in_order(NewReminder);
				ElementWasSplited = true;
				break;
			}
		}
	} while (ElementWasSplited);
	RetriveCommonSgn();
}

void _CContractionListImpl::RetriveCommonSgn()
{
	list<int> Sgns;
	for (auto& Contr : m_List)
		Sgns.push_back(Contr.m_Sgn);
	short nwd = NWD(Sgns);
	if (nwd != 1)
	{
		m_GlobalSgn *= nwd;
		for (auto& Contr : m_List)
			Contr.m_Sgn /= nwd;
	}


	//if (m_List.empty()) return;
	//if (m_List.front().m_Sgn < 0)
	//{
	//	m_GlobalSgn *= -1;
	//	for (auto& Contr : m_List)
	//		Contr.m_Sgn *= -1;
	//}
	//const short Stamp = 1;
	//bool IsDividedBy2 = true;
	//do
	//{
	//	for (auto& Contr : m_List)
	//		if ((Contr.m_Sgn & Stamp) != 0)
	//		{
	//			IsDividedBy2 = false;
	//			break;
	//		}
	//	if (IsDividedBy2)
	//	{
	//		m_GlobalSgn <<= 1;
	//		for (auto& Contr : m_List)
	//			Contr.m_Sgn >>= 1;
	//	}
	//} while (IsDividedBy2);
}

void _CContractionListImpl::Translate(int p, int r, int k, int l, MULTI_BASIC_PAIR_OF_DELETIONS& Result) const 
{
	for (auto& Contr : m_List)
	{
		auto& R = Contr.m_R;
		auto& C = Contr.m_C;
		int pc = p, rc = r, kc = k, lc = l;
		if (R.FastLoopTest(pc, rc) && C.FastLoopTest(kc, lc))
			Result.push_back(_CBasicPairOfDeletion(pc, rc, kc, lc));
	}
}

unsigned long long _CContractionListImpl::GetHash(unsigned long long Core)
{
	Core *= HASH_FACTOR;
	Core ^= m_GlobalSgn;
	Core *= HASH_FACTOR;
	Core ^= (unsigned long long)m_State;
	Core *= HASH_FACTOR;
	Core ^= m_List.size();
	for (auto& Contr : m_List)
		Core = Contr.GetHash(Core);
	return Core;
}

AD_DELETION _CContractionListImpl::Translate(const NODES_CONNECTION& NodesConnections)
{
	for (LIST::iterator it = m_List.begin(), _e = m_List.end(); it != _e;)
	{
		it->Translate(NodesConnections);
		if (!it->IsOkOrComplete())
			it = m_List.erase(it);
		else
			it++;
	}
	return (m_State = m_List.empty() ? AD_LOOP : AD_OK);
}

bool _CContractionListImpl::ContainNegatives()
{
	for (auto& Contr : m_List)
		if (Contr.ContainNegatives())
			return true;
	return false;
}


AD_DELETION _CContractionListImpl::RemoveNode(int Node)
{
	if (Node == 0 && ContainNegatives())
		Split();
	for (auto& it = m_List.begin(), _e = m_List.end(); it != _e;)
	{
		AD_DELETION Res = it->RemoveNode(Node);
		if (Res == AD_OK)
			it++;
		else
			it = m_List.erase(it);
	}
	return AD_OK;
}
AD_DELETION _CContractionListImpl::RemoveNodes(const COMPONENT_PINS_LIST& NewSeparateNodes)
{
	for (int Node : NewSeparateNodes)
	{
		LIST AdditionalList;
		for (auto& it = m_List.begin(), _e = m_List.end(); it != _e;)
		{
			auto& AuxPair = *it;
			AD_DELETION RemRes = AuxPair.RemoveNode(Node);
			switch (RemRes)
			{
			case AD_NEED_SPLIT_ROW:
			{
				_CContractionPairSimpl AddPair;
				AuxPair.SplitRows(AddPair);
				AD_DELETION AddRes = AddPair.RemoveNode(Node);
				if (AddRes == AD_OK || AddRes == AD_COMPLETE)
					AdditionalList.push_back(AddPair);
				else if (AddRes == AD_NEED_SPLIT_COL || AddRes == AD_NEED_SPLIT_ROW || AddRes == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");
				AD_DELETION AuxRes = AuxPair.RemoveNode(Node);
				if (AuxRes == AD_NEED_SPLIT_COL || AuxRes == AD_NEED_SPLIT_ROW || AuxRes == AD_NEED_SPLIY_BOTH)
				{ RISEPDD(eWrongCondition, "Why it does not work?"); }
				else if (AuxRes == AD_DISCONNECTION || AuxRes == AD_LOOP)
					it=m_List.erase(it);
				else it++;
				break;
			}
			case AD_NEED_SPLIT_COL:
			{
				_CContractionPairSimpl AddPair;
				AuxPair.SplitCols(AddPair);
				AD_DELETION AddRes = AddPair.RemoveNode(Node);
				if (AddRes == AD_OK || AddRes == AD_COMPLETE)
					AdditionalList.push_back(AddPair);
				else if (AddRes == AD_NEED_SPLIT_COL || AddRes == AD_NEED_SPLIT_ROW || AddRes == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");
				AD_DELETION AuxRes = AuxPair.RemoveNode(Node);
				if (AuxRes == AD_NEED_SPLIT_COL || AuxRes == AD_NEED_SPLIT_ROW || AuxRes == AD_NEED_SPLIY_BOTH)
				{
					RISEPDD(eWrongCondition, "Why it does not work?");
				}
				else if (AuxRes == AD_DISCONNECTION || AuxRes == AD_LOOP)
					it = m_List.erase(it);
				else it++;
				break;
			}
			case AD_NEED_SPLIY_BOTH:
			{
				_CContractionPairSimpl AddPair;
				AuxPair.SplitCols(AddPair);
				_CContractionPairSimpl AddPair2;
				AuxPair.SplitRows(AddPair2);
				_CContractionPairSimpl AddPair3;
				AddPair.SplitCols(AddPair3);
				AD_DELETION AddRes = AddPair.RemoveNode(Node);
				if (AddRes == AD_OK || AddRes == AD_COMPLETE)
					AdditionalList.push_back(AddPair);
				else if (AddRes == AD_NEED_SPLIT_COL || AddRes == AD_NEED_SPLIT_ROW || AddRes == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");

				AD_DELETION AddRes2 = AddPair2.RemoveNode(Node);
				if (AddRes2 == AD_OK || AddRes2 == AD_COMPLETE)
					AdditionalList.push_back(AddPair2);
				else if (AddRes2 == AD_NEED_SPLIT_COL || AddRes2 == AD_NEED_SPLIT_ROW || AddRes2 == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");

				AD_DELETION AddRes3 = AddPair3.RemoveNode(Node);
				if (AddRes3 == AD_OK || AddRes3 == AD_COMPLETE)
					AdditionalList.push_back(AddPair);
				else if (AddRes3 == AD_NEED_SPLIT_COL || AddRes3 == AD_NEED_SPLIT_ROW || AddRes3 == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");

				AD_DELETION AuxRes = AuxPair.RemoveNode(Node);
				if (AuxRes == AD_NEED_SPLIT_COL || AuxRes == AD_NEED_SPLIT_ROW || AuxRes == AD_NEED_SPLIY_BOTH)
				{
					RISEPDD(eWrongCondition, "Why it does not work?");
				}
				else if (AuxRes == AD_DISCONNECTION || AuxRes == AD_LOOP)
					it = m_List.erase(it);
				else it++;
				break;
			}
			case AD_DISCONNECTION:
			case AD_LOOP:
				it = m_List.erase(it);
				break;
			default:
				it++;
			}
		}
		for (auto& ExtraPairs : AdditionalList)
			insert_in_order(ExtraPairs);
	}
	Compress();
	return (m_State=m_List.empty() ? AD_DISCONNECTION : AD_OK);
}


void _CContractionListImpl::_Store(_binary_filer& Filer)
{
	Filer << m_GlobalSgn;
	Filer << (unsigned)m_State;
	Filer << m_List.size();
	for (_CContractionPairSimpl& Contr : m_List)
		Filer << Contr;
}
void _CContractionListImpl::_Load(_binary_filer& Filer)
{
	m_List.clear();
	Filer >> m_GlobalSgn;
	unsigned Counter;
	Filer >> Counter;
	m_State = (AD_DELETION)Counter;
	Filer >> Counter;
	for (size_t i = 0; i < Counter; i++)
	{
		_CContractionPairSimpl Contr;
		Filer >> Contr;
		m_List.push_back(Contr);
	}
}

void _CContractionListImpl::ReverseCofactor(_CCofactor& OutCofactor) const
{
	size_t Index = 0;
	for (auto& Contr : m_List)
		Contr.ReverseCofactor(OutCofactor, m_GlobalSgn, Index++);
}


void _CContractionListImpl::ReverseCofactor(_CMultiBasicSetOfDeletions& OutCofactor) const
{
	for (auto& Contr : m_List)
	{
		OutCofactor.push_back(_CBasicSetOfDeletions());
		Contr.ReverseCofactor(OutCofactor.back(),m_GlobalSgn);
	}
}

void _CContractionListImpl::ReverseCofactor(_CMultiDeletions& OutCofactor) const
{
	size_t Index = 0;
	for (auto& Contr : m_List)
		Contr.ReverseCofactor(OutCofactor[Index++],m_GlobalSgn);
}


//*******************************_CContractionListImpl************************
//*******************************_CContractionList****************************

_CContractionList::_CContractionList(const _CContractionListImpl& Source, _CCircuitAnalyzer* pAnalyzer)
	:m_GlobalSgn(Source.m_GlobalSgn), m_pAnalyzer(pAnalyzer), m_State(AD_OK)
{
	for (auto& Contr : Source.m_List)
		m_List.push_back(_CContractionPair(Contr.m_R, Contr.m_C, Contr.m_Sgn, pAnalyzer));
	if (!m_List.empty())
		m_State = AD_OK;
}

void _CContractionList::insert_in_order(_CContractionPair& Source)
{
	LIST AuxList(1, Source);
	m_List.merge(AuxList,
		[](const _CContractionPair& Left, const _CContractionPair& Right)
	{
		short C = Left.m_C.CompOrderWith(Right.m_C);
		if (C != 0)
			return C < 0;
		C = Left.m_R.CompOrderWith(Right.m_R);
		if (C!=0)
			return C < 0;
		return Left.m_Sgn < Right.m_Sgn;
	});
	m_State = AD_OK;
}

void _CContractionList::sort()
{
	m_List.sort([](const _CContractionPair& Left, const _CContractionPair& Right)
	{
		short C = Left.m_C.CompOrderWith(Right.m_C);
		if (C != 0)
			return C < 0;
		C = Left.m_R.CompOrderWith(Right.m_R);
		if (C != 0)
			return C < 0;
		return Left.m_Sgn < Right.m_Sgn;
	});
}

void _CContractionList::Compress()
{
	if (m_List.size() > 1)
	{
		bool WasSimplification = false;
		do
		{
			WasSimplification = false;
			for (auto fit = m_List.begin(), _e = m_List.end(); fit != _e;)
			{
				if (fit == _e)
					break;
				auto sit = fit;
				sit++;
				for (; sit != _e;)
				{
					if (fit->Try2Compress2(*sit))
					{
						WasSimplification = true;
						sit = m_List.erase(sit);
						if (fit->IsSelfLoop())
							break;
					}
					else
						sit++;
				}
				if (fit->IsSelfLoop())
					fit = m_List.erase(fit);
				else
					fit++;
			}
		} while (WasSimplification);
	}
	if (m_List.empty())
		m_State = AD_DISCONNECTION;
	else
	{
		if (m_List.size() > 1)
			sort();
		RetriveCommonSgn();
	}
}

void _CContractionList::Split()
{
	bool ElementWasSplited = false;
	do
	{
		for (auto fit = m_List.begin(), _e = m_List.end(); fit != _e; fit++)
		{
			_CContractionPair NewReminder(fit->m_pAnalyzer);
			if (fit->Try2Split(NewReminder))
			{
				insert_in_order(NewReminder);
				ElementWasSplited = true;
				break;
			}
		}
	} while (ElementWasSplited);
	RetriveCommonSgn();
}


void _CContractionList::RetriveCommonSgn()
{
	if (m_List.size() == 1)
	{
		short& Sgn = m_List.front().m_Sgn;
		m_GlobalSgn *= Sgn;
		Sgn = 1;
		return;
	}
	list<short> Sgns;
	for (auto& Contr : m_List)
		Sgns.push_back(Contr.m_Sgn);
	short nwd = NWD(Sgns);
	if (nwd != 1)
	{
		m_GlobalSgn *= nwd;
		for (auto& Contr : m_List)
			Contr.m_Sgn /= nwd;
	}

	//if (m_List.front().m_Sgn < 0)
	//{
	//	m_GlobalSgn *= -1;
	//	for (auto& Contr : m_List)
	//		Contr.m_Sgn *= -1;
	//}
	//const short Stamp = 1;
	//bool IsDividedBy2 = true;
	//do
	//{
	//	for (auto& Contr : m_List)
	//		if ((Contr.m_Sgn & Stamp) != 0)
	//		{
	//			IsDividedBy2 = false;
	//			break;
	//		}
	//	if (IsDividedBy2)
	//	{
	//		m_GlobalSgn <<= 1;
	//		for (auto& Contr : m_List)
	//			Contr.m_Sgn >>= 1;
	//	}
	//} while (IsDividedBy2);
}

void _CContractionList::DenormalizeSgn()
{
	for (auto& Contr : m_List)
		Contr.m_Sgn *= m_GlobalSgn;
	m_GlobalSgn = 1;
}

void _CContractionList::Translate(int p, int r, int k, int l, MULTI_BASIC_PAIR_OF_DELETIONS& Result) const
{
	for (auto& Contr : m_List)
	{
		auto& R = Contr.m_R;
		auto& C = Contr.m_C;
		int pc = p, rc = r, kc = k, lc = l;
		if (!R.FastLoopTest(pc, rc) && !C.FastLoopTest(kc, lc))
			Result.push_back(_CBasicPairOfDeletion(pc, rc, kc, lc));
	}
}

unsigned long long _CContractionList::GetHash(unsigned long long Core)
{
	Core *= HASH_FACTOR;
	Core ^= (unsigned long long)m_State;
	Core *= HASH_FACTOR;
	Core ^= m_List.size();
	for (auto& Contr : m_List)
		Core = Contr.GetHash(Core);
	return Core;
}

bool _CContractionList::ContainNegatives()
{
	for (auto& Contr : m_List)
		if (Contr.ContainNegatives())
			return true;
	return false;
}

AD_DELETION _CContractionList::RemoveNodes(const COMPONENT_PINS_LIST& NewSeparateNodes)
{
	if (NewSeparateNodes.empty() || (m_State!=AD_OK && m_State!=AD_COMPLETE))
		return m_State;
	for (int Node : NewSeparateNodes)
	{
		LIST AdditionalList;
		for (auto& it = m_List.begin(), _e = m_List.end(); it != _e;)
		{
			auto& AuxPair = *it;
			AD_DELETION RemRes = AuxPair.RemoveNode(Node);
			switch (RemRes)
			{
			case AD_NEED_SPLIT_ROW:
			{
				_CContractionPair AddPair(AuxPair.m_pAnalyzer);
				AuxPair.SplitRows(AddPair);
				AD_DELETION AddRes = AddPair.RemoveNode(Node);
				if (AddRes == AD_OK || AddRes == AD_COMPLETE)
					AdditionalList.push_back(AddPair);
				else if (AddRes == AD_NEED_SPLIT_COL || AddRes == AD_NEED_SPLIT_ROW || AddRes == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");
				AD_DELETION AuxRes = AuxPair.RemoveNode(Node);
				if (AuxRes == AD_NEED_SPLIT_COL || AuxRes == AD_NEED_SPLIT_ROW || AuxRes == AD_NEED_SPLIY_BOTH)
				{
					RISEPDD(eWrongCondition, "Why it does not work?");
				}
				else if (AuxRes == AD_DISCONNECTION || AuxRes == AD_LOOP)
					it = m_List.erase(it);
				else it++;
				break;
			}
			case AD_NEED_SPLIT_COL:
			{
				_CContractionPair AddPair(AuxPair.m_pAnalyzer);
				AuxPair.SplitCols(AddPair);
				AD_DELETION AddRes = AddPair.RemoveNode(Node);
				if (AddRes == AD_OK || AddRes == AD_COMPLETE)
					AdditionalList.push_back(AddPair);
				else if (AddRes == AD_NEED_SPLIT_COL || AddRes == AD_NEED_SPLIT_ROW || AddRes == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");
				AD_DELETION AuxRes = AuxPair.RemoveNode(Node);
				if (AuxRes == AD_NEED_SPLIT_COL || AuxRes == AD_NEED_SPLIT_ROW || AuxRes == AD_NEED_SPLIY_BOTH)
				{
					RISEPDD(eWrongCondition, "Why it does not work?");
				}
				else if (AuxRes == AD_DISCONNECTION || AuxRes == AD_LOOP)
					it = m_List.erase(it);
				else it++;
				break;
			}
			case AD_NEED_SPLIY_BOTH:
			{
				_CContractionPair AddPair(AuxPair.m_pAnalyzer);
				AuxPair.SplitCols(AddPair);
				_CContractionPair AddPair2(AuxPair.m_pAnalyzer);
				AuxPair.SplitRows(AddPair2);
				_CContractionPair AddPair3(AuxPair.m_pAnalyzer);
				AddPair.SplitCols(AddPair3);
				AD_DELETION AddRes = AddPair.RemoveNode(Node);
				if (AddRes == AD_OK || AddRes == AD_COMPLETE)
					AdditionalList.push_back(AddPair);
				else if (AddRes == AD_NEED_SPLIT_COL || AddRes == AD_NEED_SPLIT_ROW || AddRes == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");

				AD_DELETION AddRes2 = AddPair2.RemoveNode(Node);
				if (AddRes2 == AD_OK || AddRes2 == AD_COMPLETE)
					AdditionalList.push_back(AddPair2);
				else if (AddRes2 == AD_NEED_SPLIT_COL || AddRes2 == AD_NEED_SPLIT_ROW || AddRes2 == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");

				AD_DELETION AddRes3 = AddPair3.RemoveNode(Node);
				if (AddRes3 == AD_OK || AddRes3 == AD_COMPLETE)
					AdditionalList.push_back(AddPair);
				else if (AddRes3 == AD_NEED_SPLIT_COL || AddRes3 == AD_NEED_SPLIT_ROW || AddRes3 == AD_NEED_SPLIY_BOTH)
					RISEPDD(eWrongCondition, "Why it does not work?");

				AD_DELETION AuxRes = AuxPair.RemoveNode(Node);
				if (AuxRes == AD_NEED_SPLIT_COL || AuxRes == AD_NEED_SPLIT_ROW || AuxRes == AD_NEED_SPLIY_BOTH)
				{
					RISEPDD(eWrongCondition, "Why it does not work?");
				}
				else if (AuxRes == AD_DISCONNECTION || AuxRes == AD_LOOP)
					it = m_List.erase(it);
				else it++;
				break;
			}
			case AD_DISCONNECTION:
			case AD_LOOP:
				it = m_List.erase(it);
				break;
			default:
				it++;
			}
		}
		if (m_List.size() > 1)
			sort();
		for (auto& ExtraPairs : AdditionalList)
			insert_in_order(ExtraPairs);
	}
	if (m_List.empty())
		return (m_State = AD_DISCONNECTION);
	return m_State;
}


	//if(Deletions.empty())
	//{
	//	if(m_State!=AD_COMPLETE &&
	//		(m_R.IsSeparateNodeDisconneced(NewSeparateNodes,m_pAnalyzer->m_IncidentTable) || 
	//		 m_C.IsSeparateNodeDisconneced(NewSeparateNodes,m_pAnalyzer->m_IncidentTable)))
	//		return (m_State=AD_DISCONNECTION);
	//	else
	//		return m_State;
	//}
	//_CBasicSetOfDeletions::const_iterator it=Deletions.begin(),_le=(--Deletions.end());
	//for(;it!=_le;it++)
	//{
	//	const _CBasicPairOfDeletion& Del=(*it);
	//	AddDeletionPair(Del.m_p,Del.m_r,Del.m_k,Del.m_l,1/*,EmptyPinList*/);
	//	if(m_State==AD_LOOP || m_State==AD_DISCONNECTION)
	//		return m_State;
	//}
	//const _CBasicPairOfDeletion& Del=(*it);
	//AddDeletionPair(Del.m_p,Del.m_r,Del.m_k,Del.m_l,Deletions.Sgn(),NewSeparateNodes);
	//return m_State;

bool _CContractionList::RemoveNodesFromPair(const COMPONENT_PINS_LIST& NewSeparateNodes, _CContractionPair& MainPair, LIST& AdditionalList)
{
	COMPONENT_PINS_LIST Nodes2Repeat;
	if (!MainPair.ContainNegatives())
	{
		MainPair.RemoveNodes(NewSeparateNodes,AdditionalList);
		return false;
	}
	bool IsZeroNode = false;
	for (auto Node : NewSeparateNodes)
		if (Node == 0)
			IsZeroNode = true;
		else if (MainPair.RemoveNode(Node) != AD_OK)
			return false;
	if (IsZeroNode)
	{
		AdditionalList.push_back(MainPair);
		bool ElementWasSplited = false;
		do
		{
			ElementWasSplited = false;
			for (auto fit = AdditionalList.begin(), _e = AdditionalList.end(); fit != _e; fit++)
			{
				_CContractionPair NewReminder(fit->m_pAnalyzer);
				if (fit->Try2Split(NewReminder))
				{
					AdditionalList.push_back(NewReminder);
					ElementWasSplited = true;
					break;
				}
			}
		} while (ElementWasSplited);
		LIST::iterator it = AdditionalList.begin(), _e = AdditionalList.end();
		for (; it != _e;)
			if (it->RemoveNode(0) != AD_OK)
				it = AdditionalList.erase(it);
			else
				it++;
		return true;
	}
	else
		return false;
}

AD_DELETION _CContractionList::AddDeletions(const _CMultiBasicSetOfDeletions& SetOfDeletions, const COMPONENT_PINS_LIST& NewSeparateNodes)
{
	if (SetOfDeletions.empty())
		return (m_State = AD_LOOP);
	if (SetOfDeletions.size() > 1)
		if (m_List.size() > 1)
		{
			//OK
			LIST AuxList(m_List);
			m_List.clear();
			for (auto& BasicSet : SetOfDeletions)
			{
				for (auto& Contr : AuxList)
				{
					_CContractionPair AuxPair(Contr);
					for (auto& PairOfDels : BasicSet)
					{
						AD_DELETION Res = AuxPair.AddDeletionPair(PairOfDels.m_p, PairOfDels.m_r, PairOfDels.m_k, PairOfDels.m_l);
						if (Res != AD_OK && Res != AD_COMPLETE)
							break;
					}
					if (AuxPair.IsOkOrComplete())
					{
						AuxPair.m_Sgn *= BasicSet.Sgn();
						m_List.push_back(AuxPair);
					}
				}
			}
			if (m_List.size() == 0)
				return (m_State = AD_LOOP);
			else
				m_State = m_List.front().GetState();
			if (!NewSeparateNodes.empty())
				m_State = RemoveNodes(NewSeparateNodes);
			if (m_List.size() > 1)
			{
				Compress();
			}
			if (m_State == AD_OK || m_State == AD_COMPLETE)
				RetriveCommonSgn();
			return m_State;
		}
		else
		{
			auto it = SetOfDeletions.begin(), _e = SetOfDeletions.end();
			auto& FirstContrPair = m_List.front();
			_CContractionPair AuxContrPair(FirstContrPair);
			auto& FirstBasicSet = *it++;
			for (auto& PairOfDels : FirstBasicSet)
			{
				AD_DELETION Res = FirstContrPair.AddDeletionPair(PairOfDels.m_p, PairOfDels.m_r, PairOfDels.m_k, PairOfDels.m_l);
				if (Res != AD_OK && Res != AD_COMPLETE)
					break;
			}
			if (!FirstContrPair.IsOkOrComplete())
			{
				m_List.pop_front();
			}
			else
				FirstContrPair.m_Sgn *= FirstBasicSet.Sgn();
			for (; it != _e; it++)
			{
				_CContractionPair AuxPair(AuxContrPair);
				auto& BasicSet = *it;
				for (auto& PairOfDels : BasicSet)
				{
					AD_DELETION Res = AuxPair.AddDeletionPair(PairOfDels.m_p, PairOfDels.m_r, PairOfDels.m_k, PairOfDels.m_l);
					if (Res != AD_OK && Res != AD_COMPLETE)
						break;
				}
				if (AuxPair.IsOkOrComplete())
				{
					AuxPair.m_Sgn *= BasicSet.Sgn();
					m_List.push_back(AuxPair);
				}
			}
			if (m_List.size() == 0)
				return (m_State = AD_LOOP);
			else
				m_State = m_List.front().GetState();
			if (!NewSeparateNodes.empty())
				m_State = RemoveNodes(NewSeparateNodes);
			if (m_List.size() > 1)
			{
				Compress();
			}
			if (m_State == AD_OK || m_State == AD_COMPLETE)
				RetriveCommonSgn();
			return m_State;
		}
	else
		if (m_List.size() > 1)
		{
			auto& BasicSet = SetOfDeletions.front();
			for (auto it = m_List.begin(), _e = m_List.end(); it != _e;)
			{
				auto& Contr = *it;
				for (auto& PairOfDels : BasicSet)
				{
					AD_DELETION Res = Contr.AddDeletionPair(PairOfDels.m_p, PairOfDels.m_r, PairOfDels.m_k, PairOfDels.m_l);
					if (Res != AD_OK && Res != AD_COMPLETE)
						break;
				}
				if (Contr.IsOkOrComplete())
				{
					Contr.m_Sgn *= BasicSet.Sgn();
					it++;
				}
				else
					it = m_List.erase(it);
			}
			if (m_List.size() == 0)
				return (m_State = AD_LOOP);
			else
				m_State = m_List.front().GetState();
			if (!NewSeparateNodes.empty())
				m_State = RemoveNodes(NewSeparateNodes);
			if (m_List.size() > 1)
			{
				Compress();
			}
			if (m_State == AD_OK || m_State == AD_COMPLETE)
				RetriveCommonSgn();
			return m_State;
		}
		else
		{
			_CContractionPair& MainPair = m_List.front();
			const _CBasicSetOfDeletions& MainSetOfDels = SetOfDeletions.front();
			for (auto& PairOfDels : MainSetOfDels)
					{
						AD_DELETION Res = MainPair.AddDeletionPair(PairOfDels.m_p, PairOfDels.m_r, PairOfDels.m_k, PairOfDels.m_l);
						if (Res != AD_OK && Res != AD_COMPLETE)
							break;
					}
			m_State = MainPair.GetState();
			if (!MainPair.IsOkOrComplete())
				return m_State;
			MainPair.m_Sgn *= MainSetOfDels.Sgn();
			if (!NewSeparateNodes.empty())
				m_State = RemoveNodes(NewSeparateNodes);
			if (m_List.size() > 1)
			{
				Compress();
			}
			if (m_State==AD_OK || m_State==AD_COMPLETE)
				RetriveCommonSgn();
			return m_State;
		}
}

//AD_DELETION _CContractionList::AddDeletions(const _CMultiBasicSetOfDeletions& SetOfDeletions, const COMPONENT_PINS_LIST& NewSeparateNodes)
//{
//	LIST AuxList(m_List);
//	bool IsMultiLIst = m_List.size() > 1;
//	m_List.clear();
//	bool WasTasted = false, WasAdded = false;
//	for (auto& BasicSet : SetOfDeletions)
//	{
//		for (auto& Contr : AuxList)
//		{
//			bool MultDelIs = BasicSet.size() > 1;
//			WasTasted = true;
//			_CContractionPair AuxPair(Contr);
//			for (auto& PairOfDels : BasicSet)
//				if (AuxPair.AddDeletionPair(PairOfDels.m_p, PairOfDels.m_r, PairOfDels.m_k, PairOfDels.m_l) != AD_OK)
//					break;
//			if (AuxPair.IsOkOrComplete())
//			{
//				AuxPair.m_Sgn *= BasicSet.Sgn();
//				if (!NewSeparateNodes.empty())
//				{
//					bool RowDelayed = false, ColDelayed = false;
//					COMPONENT_PINS_LIST DelayedNNodes;
//					for (int Node : NewSeparateNodes)
//					{
//						AD_DELETION RemRes = AuxPair.RemoveNode(Node);
//						switch (RemRes)
//						{
//						case AD_NEED_SPLIT_ROW:
//							RowDelayed = true;
//							break;
//						case AD_NEED_SPLIT_COL:
//							ColDelayed = true;
//							break;
//						case AD_NEED_SPLIY_BOTH:
//							RowDelayed = ColDelayed = true;
//							break;
//						case AD_DISCONNECTION:
//						case AD_LOOP:
//							goto OUT_LOOP; 
//						default:
//							continue;
//						}
//						DelayedNNodes.push_front(Node);
//					}
//				OUT_LOOP:
//					if (RowDelayed || ColDelayed)
//					{
//						LIST AuxList2(1, AuxPair);
//						if (RowDelayed)
//							SplitRowsOnce(AuxList2);
//						if (ColDelayed)
//							SplitColsOnce(AuxList2);
//						for (int RepNode : DelayedNNodes)
//							RemoveNodeFrom(AuxList2, RepNode);
//						for (auto& AuxPair2 : AuxList2)
//						{
//							ASSERTPDD(AuxPair2.IsOkOrComplete());
//							WasAdded = true;
//							insert_in_order(AuxPair2);
//						}
//					}
//					else
//					{
//						if (!AuxPair.IsOkOrComplete())
//							continue;
//						WasAdded = true;
//						insert_in_order(AuxPair);
//					}
//				}
//				else
//				{
//					WasAdded = true;
//					insert_in_order(AuxPair);
//				}
//			}
//		}
//	}
//	if (!WasAdded)
//		return (m_State = AD_LOOP);
//	Compress();
//	return (m_State = m_List.empty() ? AD_LOOP : m_List.front().GetState());
//}

void _CContractionList::SplitRows(LIST& ContractionPairs)
{
	bool WasSplited = false;
	do
	{
		for (auto& ContrPair : ContractionPairs)
		{
			_CContractionPair NewPair(ContrPair.m_pAnalyzer);
			if (ContrPair.SplitRows(NewPair))
			{
				ContractionPairs.push_back(NewPair);
				WasSplited = true;
				break;
			}
		}
	} while (WasSplited);
}

void _CContractionList::SplitCols(LIST& ContractionPairs)
{
	bool WasSplited = false;
	do
	{
		WasSplited = false;
		for (auto& ContrPair : ContractionPairs)
		{
			_CContractionPair NewPair(ContrPair.m_pAnalyzer);
			if (ContrPair.SplitCols(NewPair))
			{
				ContractionPairs.push_back(NewPair);
				WasSplited = true;
				break;
			}
		}
	} while (WasSplited);
}


void _CContractionList::SplitRowsOnce(LIST& ContractionPairs)
{
	LIST AUXList;
	for (auto& ContrPair : ContractionPairs)
	{
		_CContractionPair NewPair(ContrPair.m_pAnalyzer);
		if (ContrPair.SplitRows(NewPair))
			AUXList.push_back(NewPair);
	}
	ContractionPairs.insert(ContractionPairs.end(),AUXList.begin(), AUXList.end());
}

void _CContractionList::SplitColsOnce(LIST& ContractionPairs)
{
	LIST AUXList;
	for (auto& ContrPair : ContractionPairs)
	{
		_CContractionPair NewPair(ContrPair.m_pAnalyzer);
		if (ContrPair.SplitCols(NewPair))
			AUXList.push_back(NewPair);
	}
	ContractionPairs.insert(ContractionPairs.end(), AUXList.begin(), AUXList.end());

}

void _CContractionList::RemoveNodeFrom(LIST& ContractionPairs, int Node)
{
	for (auto& it = ContractionPairs.begin(), _e = ContractionPairs.end(); it != _e;)
	{
		AD_DELETION Res = it->RemoveNode(Node);
		if (Res == AD_OK)
			it++;
		else
			it = ContractionPairs.erase(it);
	}
}

//AD_DELETION _CContractionList::RemoveNode(int Node)
//{
//	LIST AuxList;
//	for (auto& ContrPair : m_List)
//	{
//		AD_DELETION FRes = ContrPair.RemoveNode(Node);
//		switch (FRes)
//		{
//		case AD_NEED_SPLIT_ROW:
//		case AD_NEED_SPLIT_COL:
//		case AD_NEED_SPLIY_BOTH:
//			case
//		}
//	}
//}

AD_DELETION _CContractionList::RemoveNode(int Node)
{
	if (Node == 0 && ContainNegatives())
		Split();
	for (auto& it = m_List.begin(), _e = m_List.end();it!=_e;)
	{
		AD_DELETION Res = it->RemoveNode(Node);
		if (Res == AD_OK)
			it++;
		else
			it = m_List.erase(it);
	}
	return AD_OK;
}

bool _CContractionList::IsCompleteTest()
{
	if (m_List.size() == 0)
		return false;
	return m_List.front().IsCompleteTest();
}

bool _CContractionList::IsEnoughTest() const
{
	if (m_List.size() == 0)
		return false;
	return m_List.front().IsEnoughTest();
}


void _CContractionList::ReverseCofactor(_CCofactor& OutCofactor) const
{
	size_t Index = 0;
	for (auto& Contr : m_List)
		Contr.ReverseCofactor(OutCofactor, m_GlobalSgn,Index++);
}


void _CContractionList::ReverseCofactor(_CMultiBasicSetOfDeletions& OutCofactor) const
{
	for (auto& Contr : m_List)
	{
		OutCofactor.push_back(_CBasicSetOfDeletions());
		Contr.ReverseCofactor(OutCofactor.back(),m_GlobalSgn);
	}
}

void _CContractionList::ReverseCofactor(_CMultiDeletions& OutCofactor) const
{
	size_t Index = 0;
	for (auto& Contr : m_List)//Generowaæ
	{
		_CDeletions& Dels = OutCofactor[Index++];
		if (Dels.GetOwner() == nullptr)
			Dels.SetOwner(m_pAnalyzer->GetOwner());
		Contr.ReverseCofactor(Dels,m_GlobalSgn);
	}
}

void _CContractionList::SetTouched(INT_LIST& ExpectedList, CONTR_LIST& ContrList) const
{
	for (auto& Contr : m_List)
		Contr.SetTouched(ExpectedList, ContrList);
}

//*******************************_CContractionList****************************
//*******************************_CDeletionsCompresor*************************
bool _CDeletionsCompresor::Try2CompressWith(const _CDeletionsCompresor& OtherContractions)
{
	//_CSubNodeContractionsSimpl::_NODE_RES RowRes = ResultContractions.m_R.Contract(My_rit->first, My_rit->second);
	//_CSubNodeContractionsSimpl::_NODE_RES ColRes = ResultContractions.m_C.Contract(My_cit->first, My_cit->second);
	if (m_C.m_HasNegativeDels != OtherContractions.m_C.m_HasNegativeDels) return false;
	if (m_R.m_HasNegativeDels != OtherContractions.m_R.m_HasNegativeDels) return false;
	if (m_C.m_HasNegativeDels && m_C.GetCurrZero() != OtherContractions.m_C.GetCurrZero()) return false;
	if (m_R.m_HasNegativeDels && m_R.GetCurrZero() != OtherContractions.m_R.GetCurrZero()) return false;
	if (m_C.m_List == OtherContractions.m_C.m_List && m_R.m_List == OtherContractions.m_R.m_List)
	{
		int NewSgn = m_Sgn + OtherContractions.m_Sgn;
		if (NewSgn == 0)
		{
			m_C.Clear();
			m_R.Clear();
			m_Sgn = 0;
		}
		else
		{
			m_Sgn = NewSgn;
		}
		return true;
	}
	int RowCount = 0;
	int ColCount = 0;
	const _CSubNodeContractionsSimpl::CONTRACTION_LIST& My_ListC = m_C.GetNodesList();
	const _CSubNodeContractionsSimpl::CONTRACTION_LIST& My_ListR = m_R.GetNodesList();
	const _CSubNodeContractionsSimpl::CONTRACTION_LIST& Other_ListC = OtherContractions.m_C.GetNodesList();
	const _CSubNodeContractionsSimpl::CONTRACTION_LIST& Other_ListR = OtherContractions.m_R.GetNodesList();
	auto My_cit = My_ListC.begin(), _my_ce = My_ListC.end();
	auto My_rit = My_ListR.begin(), _my_re = My_ListR.end();
	auto Oth_cit = Other_ListC.begin(), _Oth_ce = Other_ListC.end();
	auto Oth_rit = Other_ListC.begin(), _Oth_re = Other_ListC.end();
	auto R2Chg = _my_re, C2Chg = _my_ce,RSrc=_Oth_re,CSrc=_Oth_ce;
	bool RowFound = false, ColFound = false;
	for (; My_cit != _my_ce && My_rit != _my_re && Oth_cit != _Oth_ce && Oth_rit != _Oth_re; 
		My_cit++, My_rit++, Oth_cit++, Oth_rit++,RowCount++,ColCount++)
	{
		if (*My_cit == *Oth_cit)
			if (*My_rit == *Oth_rit)
				continue;
			else
			{
				if (ColFound) return false;
				auto next_My_rit = My_rit;
				auto next_Oth_rit = Oth_rit;
				next_My_rit++;
				next_Oth_rit++;
				if (*next_My_rit == *Oth_rit)
				{
				}
				else if (*My_rit == *next_Oth_rit)
				{
				}
				else return false;
			}
		else
			if (*My_rit == *Oth_rit)
			{
				if (RowFound) return false;
			}
			else return false;
	}
	return true;
}

//*******************************_CDeletionsCompresor*************************
//*******************************_CComplexDeletionsSimplifier*****************

_CComplexDeletionsSimplifier::_CComplexDeletionsSimplifier(const _CBasicSetOfDeletions& SetOfDeletions):
_CComplexDeletionsSimplifier()
{
	AddDeletions(SetOfDeletions);
}

bool _CComplexDeletionsSimplifier::AddDeletions(const _CBasicSetOfDeletions& SetOfDeletions)
{
	m_Sgn *= SetOfDeletions.Sgn();
	for (auto& Pair : SetOfDeletions)
		if(!AddPair(Pair))
			return false;
	return true;
}

bool _CComplexDeletionsSimplifier::Translate(_CBasicSetOfDeletions& SetOfDeletions)
{
	if (m_SelfLoop)
		return false;
	SetOfDeletions.MultSgn(m_Sgn);
	const _CSubNodeContractionsSimpl::CONTRACTION_LIST& ListC = m_C.GetNodesList();
	const _CSubNodeContractionsSimpl::CONTRACTION_LIST& ListR = m_R.GetNodesList();
	ASSERTPDD(ListC.size() == ListR.size());
	auto cit = ListC.begin(), _ce = ListC.end();
	auto rit = ListR.begin(), _re = ListR.end();
	for (; rit != _re; cit++, rit++)
	{
		SetOfDeletions.push_back(_CBasicPairOfDeletion(rit->first, rit->second, cit->first, cit->second));
	}
	return true;
}

bool _CComplexDeletionsSimplifier::AddPair(const _CBasicPairOfDeletion& Pair)
{
	int p = Pair.m_p, r = Pair.m_r, k = Pair.m_k, l = Pair.m_l;
	if (!m_R.FastLoopTest(p, r) && !m_C.FastLoopTest(k, l))
	{
		_CSubNodeContractions::_NODE_RES RR = m_R.Contract(p, r);
		_CSubNodeContractions::_NODE_RES RC = m_C.Contract(k, l);
		m_Sgn *= RC.first;
		m_Sgn *= RR.first;
		if (IsOdd(RC.second - RR.second))
			m_Sgn *= -1;
		return true;
	}
	else
	{
		m_SelfLoop = true;
		return false;
	}
}


//*******************************_CComplexDeletionsSimplifier*****************
//*******************************_ComplexDeletionCompressor*******************
_ComplexDeletionCompressor::_ComplexDeletionCompressor(const _CMultiBasicSetOfDeletions& MulSetOfDel)
{
	for (auto& BasicSet : MulSetOfDel)
	{
		_CComplexDeletionsSimplifier Simplifier(BasicSet);
		if (!Simplifier.IsSelfLoop())
			m_List.push_back(BasicSet);
	}
}

bool _ComplexDeletionCompressor::Simplify()
{
	bool  WasGlobalSimplification = false;
	if (m_List.size() > 1)
	{
		bool WasSimplification = false;
		do
		{
			int Counter = 0;
			WasSimplification = false;
			for (auto fit = m_List.begin(), _e = m_List.end(); fit != _e;)
			{
				if (fit == _e)
					break;
				auto sit = fit;
				sit++;
				for (; sit != _e;)
				{
					Counter++;
					if (fit->Try2Compress2(*sit))
					{
						WasGlobalSimplification = WasSimplification = true;
						sit = m_List.erase(sit);
						if (fit->IsSelfLoop())
							break;
					}
					else
						sit++;
				}
				if (fit->IsSelfLoop())
					fit = m_List.erase(fit);
				else
					fit++;
			}
		} while (WasSimplification);
	}
	return WasGlobalSimplification;
}


void _ComplexDeletionCompressor::TranslateBack(_CMultiBasicSetOfDeletions& MulSetOfDel)
{
	if (MulSetOfDel.size() == m_List.size())
		return;
	MulSetOfDel.clear();
	for (auto& Simplifier : m_List)
	{
		_CBasicSetOfDeletions SetOfDel;
		Simplifier.Translate(SetOfDel);
		MulSetOfDel.push_back(SetOfDel);
	}
}

//*******************************_ComplexDeletionCompressor*******************
//*******************************_CSignAidBuffer******************************


//short _CSignAidBuffer::TestSgnChange(int NodeR, int NodeC)
//{
//	ASSERTPDD(NodeR>0 && NodeC>0);
//	if(NodeR==NodeC)
//		return 1;
//	PAIRS_MAP::iterator Rit=m_PairsMap.find(NodeR),
//						Cit=m_PairsMap.find(NodeC),
//						_e=m_PairsMap.end();
//	if(Rit==_e)
//		if(Cit==_e)
//		{
//			m_PairsMap[NodeR]=NodeC;
//			m_PairsMap[NodeC]=NodeR;
//			return -1;
//		}
//		else
//		{
//			int OldNodeC=Cit->second;
//			m_PairsMap[NodeR]=OldNodeC;
//			m_PairsMap[OldNodeC]=NodeR;
//			m_PairsMap.erase(Cit);
//			return -1;
//		}
//	else
//		if(Cit==_e)
//		{
//			int OldNodeC=Rit->second;
//			m_PairsMap[NodeC]=OldNodeC;
//			m_PairsMap[OldNodeC]=NodeC;
//			m_PairsMap.erase(Rit);
//			return -1;
//		}
//	int OldNodeR=Rit->second;
//	int OldNodeC=Cit->second;
//	int Res=1;
//	if(OldNodeR!=NodeC)
//	{
//		Res=-1;
//		m_PairsMap[OldNodeR]=OldNodeC;
//		m_PairsMap[OldNodeC]=OldNodeR;
//	}
//	m_PairsMap.erase(Rit);
//	m_PairsMap.erase(Cit);
//	return Res;
//}
//
//int _CSignAidBuffer::GetChainValue(int Node)
//{
//	PAIRS_MAP::iterator it=m_PairsMap.find(Node);
//	return it==m_PairsMap.end()?Node:it->second;
//}
//
//short _CSignAidBuffer::Recreate(const _CSubNodeContractions::CANCELLED_NODES_MAP& R,
//		const _CSubNodeContractions::CANCELLED_NODES_MAP& C)
//{
//	short Res=1;
//	ASSERTPDD(R.size()==C.size());
//	m_PairsMap.clear();
//	for(_CSubNodeContractions::CANCELLED_NODES_MAP::const_iterator rit=R.begin(), cit=C.begin(),
//		_re=R.end();rit!=_re;rit++,cit++)
//		Res*=TestSgnChange(rit->first,cit->first);
//	return Res;
//}

//void _CSignAidBuffer::Store(_binary_filer& Filer) 
//{
//	Filer<<m_PairsMap.size();
//	for(PAIRS_MAP::iterator it=m_PairsMap.begin(),_e=m_PairsMap.end();it!=_e;it++)
//	{
//		Filer<<it->first;
//		Filer<<it->second;
//	}
//}
//
//void _CSignAidBuffer::Load(_binary_filer& Filer) 
//{
//	m_PairsMap.clear();
//	size_t Size=0;
//	Filer>>Size;
//	for(size_t i=0;i<Size;i++)
//	{
//		int a=0,b=0;
//		Filer>>a;
//		Filer>>b;
//		m_PairsMap.insert(PAIRS_MAP::value_type(a,b));
//	}
//}
//
//IMPLEMENT_DYNAMIC_CREATION(_CSignAidBuffer);

//*******************************_CSignAidBuffer******************************

//*******************************_CCommonStateCofactor************************

//_CCommonStateCofactor::_CCommonStateCofactor(_CCircuitAnalyzer* pAnalyzer):m_State(AD_OK),
//	m_pAnalyzer(pAnalyzer),m_R(pAnalyzer),m_C(pAnalyzer)
_CCommonStateCofactor::_CCommonStateCofactor(_CCircuitAnalyzer* pAnalyzer) :/*m_State(AD_OK),*/
m_pAnalyzer(pAnalyzer), m_Contractions(pAnalyzer)
{
	ASSERTPDD(m_pAnalyzer!=NULL);
	//m_HighestOrder=m_pAnalyzer->HighestNoOfNodes();
}

inline bool _CCommonStateCofactor::IsCompleteTest() 
{
	return m_Contractions.IsCompleteTest();
	//return GetNoOfDeletions() == m_HighestOrder;
}

bool _CCommonStateCofactor::IsEnoughTest() const
{
	return m_Contractions.IsEnoughTest();
	//if(m_pAnalyzer->IsSubmodel())
	//	return m_Contractions.IsEnough();

	//else
	//	return GetNoOfDeletions()==m_HighestOrder; 
}

void _CCommonStateCofactor::Translate(int p, int r, int k, int l, MULTI_BASIC_PAIR_OF_DELETIONS& Results) const
{
	m_Contractions.Translate(p, r, k, l, Results);
}

unsigned long long _CCommonStateCofactor::GetHash(unsigned long long Core) 
{
	//unsigned long long Res=m_R.GetHash(m_C.GetHash(Core));
	return  m_Contractions.GetHash(Core);
	//Core *= HASH_FACTOR;
	//Core ^= (unsigned long long)m_State;
	//Core *= HASH_FACTOR;
	//Core ^= m_HighestOrder;
	//return Core;
}

long _CCommonStateCofactor::DetermineHashKey(long Core)
{
	return (long)GetHash(Core);
}

//bool _CCommonStateCofactor::IsEqualIgnoreSgn(_CCommonStateCofactor& Other)
//{
//	return m_R.IsEqual(Other.m_R) && m_C.IsEqual(Other.m_C) && 
//		m_State==Other.m_State && 
//		m_HighestOrder==Other.m_HighestOrder;
//}

//*******************************_CCommonStateCofactor************************
//*******************************_CGreedyStateCofactor************************

//_CGreedyStateCofactor::AD_DELETION _CGreedyStateCofactor::TestGreedyAddDeletionPair(const _CBasicSetOfDeletions& SetOfDeletions)
//{
//	for(_CBasicSetOfDeletions::const_iterator it=SetOfDeletions.begin(), _e=SetOfDeletions.end();it!=_e;it++)
//	{
//		const _CBasicPairOfDeletion& Del=*it;
//		int p=Del.m_p, r=Del.m_r, k=Del.m_k, l=Del.m_l;
//		if(m_R.GreedyTestContract(p,r) || m_C.GreedyTestContract(k,l))
//			return (m_State=AD_LOOP);
//		m_R.AddTestContract(p,r);
//		m_C.AddTestContract(k,l);
//	}
//	return (m_State=IsEnoughTest()? AD_ENOUGH:AD_OK);
//}
//*******************************_CGreedyStateCofactor************************
//*******************************_CGreedyResContainer*************************
//_CGreedyResContainer::_CGreedyResContainer(const _CCommonStateCofactor& SourceCof, 
//										   _CComponents& InitComponentList,
//										   SET_OF_COMPONENTS& IgnoreComponents,bool IgnoreCache):
//											/*m_AdvancedComOnPath(InitComponentList),*/
//											m_CanBeComplate(false),
//												m_IgnoreCache(IgnoreCache),
//												m_StateCof(SourceCof),
//												m_CompOnAPath(InitComponentList.begin(),InitComponentList.end())
//{
//	for(SET_OF_COMPONENTS::iterator it=IgnoreComponents.begin(),_e=IgnoreComponents.end();it!=_e;it++)
//		m_CompOnAPath.erase(*it);
//}
//
//long _CGreedyResContainer::DetermineHashKey(long Core)
//{
//	unsigned long long iCore=m_StateCof.GetHash((unsigned long long)Core);
//	for(COMP_ON_A_PATH::iterator it=m_CompOnAPath.begin(),_e=m_CompOnAPath.end();
//		it!=_e;it++)
//	{
//		iCore*=HASH_FACTOR;
//		iCore^=(unsigned long long)*it;
//	}
//	return (long)iCore;
//}
//bool _CGreedyResContainer::operator==(_CGreedyResContainer& Other)
//{
//	return m_StateCof==Other.m_StateCof && m_CompOnAPath==Other.m_CompOnAPath;
//}
//
//_CGreedyResContainer::_CGreedyResContainer(const _CCommonStateCofactor& SourceCof,
//										   COMP_ON_A_PATH& PrevPath,
//										   SET_OF_COMPONENTS& IgnoreComponents,bool IgnoreCache):
//											m_StateCof(SourceCof),m_CompOnAPath(PrevPath),m_IgnoreCache(IgnoreCache)/*,
//												m_AdvancedComOnPath(PrevPath)*/
//{
//	for(SET_OF_COMPONENTS::iterator it=IgnoreComponents.begin(),_e=IgnoreComponents.end();it!=_e;it++)
//		m_CompOnAPath.erase(*it);
//}
//
//bool _CGreedyResContainer::ResolveAmIFeasible()
//{
//	if ((m_StateCof.IsEnough() || m_StateCof.IsComplete() || m_StateCof.IsEnoughTest()) &&
//		(m_CompOnAPath.empty() || (*m_CompOnAPath.begin())->DeletionsAreForced()))
//	{
//		m_CanBeComplate = true;
//		return true;
//	}
//	if (m_StateCof.IsLoop() || m_CompOnAPath.empty())
//	{
//		m_CanBeComplate = false;
//		return false;
//	}
//	_CComponent* pComponent=*m_CompOnAPath.begin();
//	for(size_t i=0;i<pComponent->GreedyNoOfDescendants();i++)
//	{
//		const _CMultiBasicSetOfDeletions* pDels=NULL;
//		pComponent->GetDescendantGreedyDeletion(i,pDels);
//		ASSERTPDD(pDels!=NULL);
//		if((m_StateCof.GetNoOfDeletions()+pDels->size())>m_StateCof.m_HighestOrder)
//			continue;
//		_CGreedyResContainer NewDesc(*this);
//		NewDesc.m_CompOnAPath.erase(pComponent);
//		NewDesc.m_StateCof.TestGreedyAddDeletionPair(*pDels);
//		if(NewDesc.m_StateCof.IsEnough() && 
//			(NewDesc.m_CompOnAPath.empty() 
//			|| !(*NewDesc.m_CompOnAPath.begin())->DeletionsAreForced()))
//			return (m_CanBeComplate=true);
//		if(NewDesc.m_StateCof.IsInProgres()|| NewDesc.m_StateCof.IsEnough())
//			if(NewDesc.m_StateCof.m_pAnalyzer->IsGreedyFeasible(NewDesc))
//				return (m_CanBeComplate=true);
//	}
//	m_CanBeComplate = false;
//	return false;
//}



//*******************************_CGreadyResCache*****************************
//bool _CGreadyResCache::GetExistingResult(_CGreedyResContainer& ResContainer,bool*& pResult)
//{
//	long Hash=(long)ResContainer;
//	pair<MAP::iterator,MAP::iterator> Range=m_Map.equal_range(Hash);
//	for(MAP::iterator it=Range.first;it!=Range.second;it++)
//		if(it->second==ResContainer)
//		{
//			pResult=&it->second.m_CanBeComplate;
//			return true;
//		}
//	pResult=&m_Map.insert(MAP::value_type(Hash,ResContainer))->second.m_CanBeComplate;
//	return false;
//}
//bool _CGreadyResCache::IsFeasible(_CGreedyResContainer& ResContainer)
//{
//	if(ResContainer.IgnoreCache())
//		return ResContainer.ResolveAmIFeasible();
//	long Hash=(long)ResContainer;
//	pair<MAP::iterator,MAP::iterator> Range=m_Map.equal_range(Hash);
//	for(MAP::iterator it=Range.first;it!=Range.second;it++)
//		if(it->second==ResContainer)
//			return it->second.m_CanBeComplate;
//	MAP::iterator n_it=m_Map.insert(MAP::value_type(Hash,ResContainer));
//	return n_it->second.ResolveAmIFeasible();
//}

//*******************************_CGreadyResCache*****************************
//*******************************_CStateCofactor******************************
//IMPLEMENT_DYNAMIC_CREATION(_CStateCofactor);

//AD_DELETION _CStateCofactor::AddDeletionPair(int P, int R, int K, int L, 
//	short Mult,const COMPONENT_PINS_LIST& NewSeparateNodes)
//{
//	if(m_State==AD_COMPLETE || m_R.FastLoopTest(P,R) || m_C.FastLoopTest(K,L))
//		return (m_State=AD_LOOP);
//	_NODE_RES MultR=m_R.Contract(P,R);
//	_NODE_RES MultC=m_C.Contract(K,L);
//	m_Sgn*=Mult;
//	m_Sgn*=MultR.first;
//	m_Sgn*=MultC.first;
//	if(IsOdd(MultR.second-MultC.second))
//		m_Sgn*=(-1);
//	m_R._Flatten();
//	m_C._Flatten();
//	if(m_DelayedZero>=0)
//	{
//		CAN_REM RCan=m_R.CanRemove(m_DelayedZero);
//		CAN_REM CCan=m_C.CanRemove(m_DelayedZero);
//		if(RCan==_CSubNodeContractionsSimpl::DISCONNECT || CCan==_CSubNodeContractionsSimpl::DISCONNECT)
//			return (m_State=AD_DISCONNECTION);
//		if(RCan==_CSubNodeContractionsSimpl::CAN && CCan==_CSubNodeContractionsSimpl::CAN)
//		{
//			_NODE_RES RR=m_R._IgnoreNode(m_DelayedZero);
//			_NODE_RES CR=m_C._IgnoreNode(m_DelayedZero);
//			m_DelayedZero=-1;
//			m_Sgn*=RR.first;
//			m_Sgn*=CR.first; 
//			if(IsOdd(RR.second-CR.second))
//				m_Sgn*=(-1);
//			m_HighestOrder--;
//		}
//	}
//	if(IsCompleteTest())
//		return (m_State=AD_COMPLETE);
//	if(m_R.IsSeparateNodeDisconneced(NewSeparateNodes,m_pAnalyzer->m_IncidentTable))
//		return (m_State=AD_DISCONNECTION);
//	if(m_C.IsSeparateNodeDisconneced(NewSeparateNodes,m_pAnalyzer->m_IncidentTable))
//		return (m_State=AD_DISCONNECTION);
//	return (m_State=AD_OK);
//}


AD_DELETION _CStateCofactor::AddDeletions(const _CMultiBasicSetOfDeletions& Deletions,
		const COMPONENT_PINS_LIST& NewSeparateNodes)
{
	return m_Contractions.AddDeletions(Deletions, NewSeparateNodes);
	//if(Deletions.empty())
	//{
	//	if(m_State!=AD_COMPLETE &&
	//		(m_R.IsSeparateNodeDisconneced(NewSeparateNodes,m_pAnalyzer->m_IncidentTable) || 
	//		 m_C.IsSeparateNodeDisconneced(NewSeparateNodes,m_pAnalyzer->m_IncidentTable)))
	//		return (m_State=AD_DISCONNECTION);
	//	else
	//		return m_State;
	//}
	//_CBasicSetOfDeletions::const_iterator it=Deletions.begin(),_le=(--Deletions.end());
	//for(;it!=_le;it++)
	//{
	//	const _CBasicPairOfDeletion& Del=(*it);
	//	AddDeletionPair(Del.m_p,Del.m_r,Del.m_k,Del.m_l,1/*,EmptyPinList*/);
	//	if(m_State==AD_LOOP || m_State==AD_DISCONNECTION)
	//		return m_State;
	//}
	//const _CBasicPairOfDeletion& Del=(*it);
	//AddDeletionPair(Del.m_p,Del.m_r,Del.m_k,Del.m_l,Deletions.Sgn(),NewSeparateNodes);
	//return m_State;
}


void _CStateCofactor::ReverseCofactor(_CCofactor& OutCofactor) const
{
	OutCofactor.ResetDeletions();
	m_Contractions.ReverseCofactor(OutCofactor);
}

//void _CStateCofactor::ReverseCofactor(_CCofactor& OutCofactor) const
//{
//	OutCofactor.ResetDeletions();
//	ASSERTPDD(m_C.NoOfDel()==m_R.NoOfDel());
//	const _CSubNodeContractions::CONTRACTION_LIST& RList=m_R.GetNodesList();
//	const _CSubNodeContractions::CONTRACTION_LIST& CList=m_C.GetNodesList();
//	_const_iterator Rit=RList.begin(),_Re=RList.end();
//	_const_iterator Cit=CList.begin(),_Ce=CList.end();
//	for(;Rit!=_Re;Rit++,Cit++)
//	{
//		int P=Rit->first, R=Rit->second, K=Cit->first, L=Cit->second;
//		OutCofactor.AddDeletions(P,R,K,L,false,false);
//	}
//	OutCofactor.MultSgn(m_Sgn);
//}

//void _CStateCofactor::ReverseCofactor(_CMultiDeletions& OutCofactor) const
//{
//	OutCofactor.Clear();
//	ASSERTPDD(m_C.NoOfDel() == m_R.NoOfDel());
//	const _CSubNodeContractions::CONTRACTION_LIST& RList = m_R.GetNodesList();
//	const _CSubNodeContractions::CONTRACTION_LIST& CList = m_C.GetNodesList();
//	_const_iterator Rit = RList.begin(), _Re = RList.end();
//	_const_iterator Cit = CList.begin(), _Ce = CList.end();
//	for (; Rit != _Re; Rit++, Cit++)
//	{
//		int P = Rit->first, R = Rit->second, K = Cit->first, L = Cit->second;
//		OutCofactor.push_in_order(new _CDeletion(NULL, P, R, K, L));
//	}
//	OutCofactor.SetMultiplier(m_Sgn);
//}

void _CStateCofactor::ReverseCofactor(_CMultiDeletions& OutCofactor) const
{
	OutCofactor.clear();
	m_Contractions.ReverseCofactor(OutCofactor);
}

//void _CStateCofactor::ReverseCofactor(_CMultiBasicSetOfDeletions& OutCofactor) const
//{
//	OutCofactor.Clear();
//	ASSERTPDD(m_C.NoOfDel()==m_R.NoOfDel());
//	const _CSubNodeContractions::CONTRACTION_LIST& RList=m_R.GetNodesList();
//	const _CSubNodeContractions::CONTRACTION_LIST& CList=m_C.GetNodesList();
//	_const_iterator Rit=RList.begin(),_Re=RList.end();
//	_const_iterator Cit=CList.begin(),_Ce=CList.end();
//	for(;Rit!=_Re;Rit++,Cit++)
//	{
//		int P=Rit->first, R=Rit->second, K=Cit->first, L=Cit->second;
//		OutCofactor.push_back(_CBasicPairOfDeletion(P,R,K,L));
//	}
//	OutCofactor.MultSgn(m_Sgn);
//}

void _CStateCofactor::ReverseCofactor(_CMultiBasicSetOfDeletions& OutCofactor) const
{
	OutCofactor.clear();
	m_Contractions.ReverseCofactor(OutCofactor);
}

//bool _CStateCofactor::ReDisconnectNodes(const COMPONENT_PINS_LIST& ReDisconnectedPins)
//{
//	if(ReDisconnectedPins.empty())
//		return true;
//	if(!m_R.IsPinConnected(ReDisconnectedPins))
//		return false;
//	if(!m_C.IsPinConnected(ReDisconnectedPins))
//		return false;
//	ReduceSeparateNodes(ReDisconnectedPins);
//	return true;
//}

unsigned long long _CStateCofactor::GetHash(unsigned long long Core) 
{
	Core=_CCommonStateCofactor::GetHash(Core);
	Core*=HASH_FACTOR;
	Core^=m_DelayedZero;
	return Core;
}

//bool _CStateCofactor::IsEqualIgnoreSgn(_CCommonStateCofactor& Other) 
//{
//	_CStateCofactor* pOther=dynamic_cast<_CStateCofactor*>(&Other);
//	if(m_DelayedZero!=pOther->m_DelayedZero)
//		return false;
//	return _CCommonStateCofactor::IsEqualIgnoreSgn(Other);
//}


//bool _CStateCofactor::TestIsDisconnected(const COMPONENT_PINS_LIST& NewSeparateNodes)
//{
//
//	return m_R.IsSeparateNodeDisconneced(NewSeparateNodes,m_pAnalyzer->m_IncidentTable) || 
//		m_C.IsSeparateNodeDisconneced(NewSeparateNodes,m_pAnalyzer->m_IncidentTable);
//}

//AD_DELETION _CStateCofactor::ReduceSeparateNode(int Node)
//{
//	CAN_REM RCan=m_R.CanRemove(Node);
//	CAN_REM CCan=m_C.CanRemove(Node);
//	if(RCan==_CSubNodeContractions::DISCONNECT || CCan==_CSubNodeContractions::DISCONNECT)
//	{
//		return (m_State=AD_DISCONNECTION);
//	}
//	if(RCan==_CSubNodeContractions::DELAY || CCan==_CSubNodeContractions::DELAY)
//	{
//		m_R.DelayRemove();
//		m_C.DelayRemove();
//		m_DelayedZero=Node;
//		return m_State;
//	}
//	else if(Node==m_DelayedZero)
//		m_DelayedZero=-1;
//	_NODE_RES RR=m_R._IgnoreNode(Node);
//	_NODE_RES CR=m_C._IgnoreNode(Node);
//	m_Sgn*=RR.first;
//	m_Sgn*=CR.first; 
//	if(IsOdd(RR.second-CR.second))
//		m_Sgn*=(-1);
//	m_HighestOrder--;
//	return m_State;
//}
//
AD_DELETION _CStateCofactor::ReduceSeparateNodes(const COMPONENT_PINS_LIST& SeparateNodes)
{
	return m_Contractions.RemoveNodes(SeparateNodes);
	//for(COMPONENT_PINS_LIST::const_iterator it=SeparateNodes.begin(),_e=SeparateNodes.end();it!=_e;it++)
	//{
	//	if(m_HighestOrder==0)
	//	{
	//		ASSERTPDD(m_R.empty() && m_C.empty()/* && it==(--SeparateNodes.end())*/);
	//		return m_State;
	//	}
	//	bool ReCheckDelayed=m_DelayedZero>=0;
	//	ReduceSeparateNode(*it);
	//	if(ReCheckDelayed)
	//		ReduceSeparateNode(m_DelayedZero);
	//}
	//return m_State;
}
//bool _CStateCofactor::IsEqualWithSgn(_CStateCofactor& Other, short& Sgn)
//{
//	bool Res = IsEqualIgnoreSgn(Other);
//	if (Res)
//		Sgn = m_Sgn;
//	return Res;
//}

//bool _CStateCofactor::IsEqualWithSgn(_CStateCofactor& Other, short& Sgn)
//{
//	return m_Contractions.IsEqualWithSgn(Other, Sgn);
//	//bool Res=IsEqualIgnoreSgn(Other);
//	//if(Res)
//	//	Sgn=m_Sgn;
//	//return Res;
//}

//*******************************_CStateCofactor******************************
//*******************************_CMultiStateCofactor******************************

void _CMultiStateCofactor::CompressCofactors()
{

	for (auto it1 = m_List.begin(), _e = m_List.end(); it1 != _e; it1++)
	{
		auto MianState = *it1;
		auto it2 = it1;
		it2++;
		for (; it2 != _e; it2++)
		{

		}
	}
}

//*******************************_CMultiStateCofactor******************************
//*******************************_CStateCofactorCache*************************

short ExtractHigherPower(short& Source)
{
	short Res = Source < 0 ? -Source : Source;
	if (Res != 1)
		Source /= Res;
	return Res;
	//short Out=1;
	//while((Source & (short)1)==0)
	//{
	//	Source>>=1;
	//	Out<<=1;
	//}		
	//return Out;
		}

//_CStateCofactorCache::_CStateCofactorCache(_CStateCofactorCache& Source):
//m_CurrOrder(Source.m_CurrOrder),m_PrevOrder(Source.m_PrevOrder),m_Map(Source.m_Map)
//{
//	for(MAP::iterator it=Source.m_Map.begin(),_e=Source.m_Map.end();it!=_e;it++)
//	{
//		LOCAL_CACHE& Cache=it->second;
//		Cache.Clear(false);
//	}
//}

_CStateCofactor* _CStateCofactorCache::RegisterUnique(_CStateCofactor* pStateCofactor, short& SgnChange)
{
	size_t HighestOrder=pStateCofactor->HighestOrder();
	m_CurrOrder=max(m_CurrOrder,HighestOrder);
	short& OutSgn=pStateCofactor->Sgn();
	SgnChange=ExtractHigherPower(OutSgn);
	LOCAL_CACHE& Cache=m_Map[HighestOrder];
	_CStateCofactor* pStoredState=Cache.FindExisting(pStateCofactor);
	if(pStoredState!=NULL)
	{
		short TargetSgn=pStoredState->Sgn();
		SgnChange*=OutSgn;
		SgnChange*=TargetSgn;
		delete pStateCofactor;
		return pStoredState;
	}
	else
	{
		Cache.ProcessIndex(Cache.ForceIntoCache(pStateCofactor),pStateCofactor);
		return pStateCofactor;
	}
}
void _CStateCofactorCache::KillOutOfDate()
{
	if(m_CurrOrder<m_PrevOrder)
	{
		for(MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;)
			if(it->first>m_CurrOrder)
				it=m_Map.erase(it);
			else
				it++;
	}
	m_PrevOrder=m_CurrOrder;
	m_CurrOrder=0;
}

void _CStateCofactorCache::Clear()
{
	//for(MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
	//	it->second.Clear();
	m_Map.clear();
}
void _CStateCofactorCache::DetachCache()
{
	for(MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
		it->second.Clear(false);
}

//*******************************_CStateCofactorCache*************************
//*******************************_CBoundaryState******************************
IMPLEMENT_DYNAMIC_CREATION(_CBoundaryState);

_CBoundaryState::_CBoundaryState(const _CStateCofactor& Source) : /*m_State(AD_OK),*/ m_Contractions(Source.m_Contractions)
{
}

_CBoundaryState::_CBoundaryState(const _CBoundaryState& Source,
	const NODES_CONNECTION& NodesConnection, const DONGLE_NODES& Dongles) :
	m_Contractions(Source.m_Contractions)
{
	m_Contractions.RemoveNodes(Dongles);
	m_Contractions.Translate(NodesConnection);
	//ReduceSeparateNodes(Dongles);
	//if(m_State!=AD_OK)
	//	return;
	//_CSubNodeContractionsSimpl::POS_LIST RList;
	//short RSgn=m_R.TranslateMe(NodesConnection,RList);
	//if(RSgn==0)
	//{
	//	m_State=AD_LOOP;
	//	return;
	//}
	//_CSubNodeContractionsSimpl::POS_LIST CList;
	//short CSgn=m_C.TranslateMe(NodesConnection,CList);
	//if(CSgn==0)
	//{
	//	m_State=AD_LOOP;
	//	return;
	//}
	//m_Sgn*=RSgn;
	//m_Sgn*=CSgn;
	//for(_CSubNodeContractionsSimpl::POS_LIST::iterator rit=RList.begin(),_re=RList.end(),
	//	cit=CList.begin(),_ce=CList.end();rit!=_re && cit!=_ce;rit++,cit++)
	//	if(IsOdd(*rit-*cit))
	//		m_Sgn*=-1;
	//if(!Dongles.empty() && (!m_R.ContainNodes(Dongles) || m_C.ContainNodes(Dongles)))
	//{
	//	m_State=AD_DISCONNECTION;
	//	return;
	//}
	//_CSignAidBuffer m_S;
	//m_Sgn*=m_S.Recreate(m_R.GetNodesMap(),m_C.GetNodesMap());
	//if(!Dongles.empty())
	//	ReduceSeparateNodes(Dongles);
	//if(!m_R.TranslateMe(NodesConnection,m_Sgn))
	//{
	//	m_State=AD_LOOP;
	//	return;
	//}
	//if(!m_C.TranslateMe(NodesConnection,m_Sgn))
	//{
	//	m_State=AD_LOOP;
	//	return;
	//}
	//m_Sgn*=m_S.Recreate(m_R.GetNodesMap(),m_C.GetNodesMap());
}
void _CBoundaryState::ReduceSeparateNodes(const DONGLE_NODES& Nodes)
{
	m_Contractions.RemoveNodes(Nodes);
	//for(DONGLE_NODES::const_iterator it=Nodes.begin(),_e=Nodes.end();it!=_e;it++)
	//{
	//	unsigned Node=*it;
	//	CAN_REM RCan=m_R.CanRemove(Node);
	//	if(RCan==_CSubNodeContractionsSimpl::DISCONNECT)
	//	{
	//		m_State=AD_DISCONNECTION;
	//		return;
	//	}
	//	CAN_REM CCan=m_C.CanRemove(Node);
	//	if (CCan == _CSubNodeContractionsSimpl::DISCONNECT)
	//	{
	//		m_State=AD_DISCONNECTION;
	//		return;
	//	}
	//	if(RCan==_CSubNodeContractionsSimpl::CAN && CCan==_CSubNodeContractionsSimpl::CAN)
	//	{
	//		_CSubNodeContractions::_NODE_RES R_I=m_R._IgnoreNode(Node);
	//		m_Sgn*=R_I.first;
	//		_CSubNodeContractions::_NODE_RES C_I=m_C._IgnoreNode(Node);
	//		m_Sgn*=C_I.first;
	//		if(IsOdd(R_I.second-C_I.second))
	//			m_Sgn*=(-1);
	//	}
	//}
	//for(DONGLE_NODES::const_iterator it=Nodes.begin(),_e=Nodes.end();it!=_e;it++)
	//{
	//	unsigned Node=*it;
	//	int SubR=-1,SubC=-1;
	//	_CSubNodeContractions::IGNORE_NODE_RES R_I=m_R.IgnoreNode(Node,SubR);
	//	m_Sgn*=R_I.first;
	//	_CSubNodeContractions::IGNORE_NODE_RES C_I=m_C.IgnoreNode(Node,SubC);
	//	m_Sgn*=C_I.first;
	//}
//Zdecydowanie poprawiæ
}

void _CBoundaryState::Store(_binary_filer& Filer) 
{
	//Filer<<m_Sgn;
	////Filer<<m_S;
	//Filer<<m_R;
	//Filer<<m_C;
	Filer << m_Contractions;
}

void _CBoundaryState::Load(_binary_filer& Filer) 
{
	Filer >> m_Contractions;
	//Filer>>m_Sgn;
	////Filer>>m_S;
	//Filer>>m_R;
	//Filer>>m_C;
	//m_State=AD_OK;
}
unsigned long long _CBoundaryState::GetHash(unsigned long long Core)
{
	return m_Contractions.GetHash(Core);
	//Core=m_R.GetHash(Core);
	//Core=m_C.GetHash(Core);
	//Core*=HASH_FACTOR;
	//Core^=(unsigned long long)m_Sgn;
	//return Core;
}

void _CBoundaryState::ReverseCofactor(_CCofactor& OutCofactor) const
{
	m_Contractions.ReverseCofactor(OutCofactor);
	//OutCofactor.ResetDeletions();
	//ASSERTPDD(m_C.NoOfDel()==m_R.NoOfDel());
	//const _CSubNodeContractions::CONTRACTION_LIST& RList=m_R.GetNodesList();
	//const _CSubNodeContractions::CONTRACTION_LIST& CList=m_C.GetNodesList();
	//_const_iterator Rit=RList.begin(),_Re=RList.end();
	//_const_iterator Cit=CList.begin(),_Ce=CList.end();
	//for(;Rit!=_Re;Rit++,Cit++)
	//{
	//	int P=Rit->first, R=Rit->second, K=Cit->first, L=Cit->second;
	//	OutCofactor.AddDeletions(P,R,K,L,false,false);
	//}
	//OutCofactor.MultSgn(m_Sgn);
}

void _CBoundaryState::ReverseCofactor(_CMultiDeletions& OutCofactor) const
{
	m_Contractions.ReverseCofactor(OutCofactor);
	//OutCofactor.Clear();
	//ASSERTPDD(m_C.NoOfDel()==m_R.NoOfDel());
	//const _CSubNodeContractions::CONTRACTION_LIST& RList=m_R.GetNodesList();
	//const _CSubNodeContractions::CONTRACTION_LIST& CList=m_C.GetNodesList();
	//_const_iterator Rit=RList.begin(),_Re=RList.end();
	//_const_iterator Cit=CList.begin(),_Ce=CList.end();
	//for(;Rit!=_Re;Rit++,Cit++)
	//{
	//	int P=Rit->first, R=Rit->second, K=Cit->first, L=Cit->second;
	//	OutCofactor.push_in_order(new _CDeletion(NULL,P,R,K,L));
	//}
	//OutCofactor.SetMultiplier(m_Sgn);
}

void _CBoundaryState::ReverseCofactor(_CMultiBasicSetOfDeletions& OutCofactor) const
{
	m_Contractions.ReverseCofactor(OutCofactor);
	//OutCofactor.Clear();
	//ASSERTPDD(m_C.NoOfDel()==m_R.NoOfDel());
	//const _CSubNodeContractions::CONTRACTION_LIST& RList=m_R.GetNodesList();
	//const _CSubNodeContractions::CONTRACTION_LIST& CList=m_C.GetNodesList();
	//_const_iterator Rit=RList.begin(),_Re=RList.end();
	//_const_iterator Cit=CList.begin(),_Ce=CList.end();
	//for(;Rit!=_Re;Rit++,Cit++)
	//{
	//	int P=Rit->first, R=Rit->second, K=Cit->first, L=Cit->second;
	//	OutCofactor.push_back(_CBasicPairOfDeletion(P,R,K,L));
	//}
	//OutCofactor.MultSgn(m_Sgn);
}

//*******************************_CBoundaryState******************************
//*******************************_CBounadryStatesVector***********************

IMPLEMENT_DYNAMIC_CREATION(_CBounadryStatesVector);

_CBounadryStatesVector::_CBounadryStatesVector(const ORIGINAL_DESC_STATES& Source) 
{
	m_EnumIt=m_UniqMap.end();
	for(size_t i=0;i<Source.size();i++)
	{
		//const _CStateCofactor& OrgStCof=Source[i];
		_CBoundaryState OrgStCof(Source[i]);
		unsigned long long Hash=OrgStCof.GetHash();
		_CBoundaryState& SrcCofState=m_UniqMap.insert(UNIQ_MAP::value_type(Hash,OrgStCof))->second;
		m_State2Index.insert(STATE2INDEX::value_type(&SrcCofState,i));
		m_Index2State.insert(INDEX2STATE::value_type(i,&SrcCofState));
		//m_Map.insert(MAP::value_type(i,OrgStCof));
	}
}
_CBounadryStatesVector::_CBounadryStatesVector(const _CBounadryStatesVector& Source,
		const NODES_CONNECTION& NodesConnection,const DONGLE_NODES& Dongles)
{
	m_EnumIt=m_UniqMap.end();
	for(UNIQ_MAP::const_iterator it=Source.m_UniqMap.begin(),_e=Source.m_UniqMap.end();it!=_e;it++)
	{
		_CBoundaryState NewState(it->second,NodesConnection,Dongles);
		_CBoundaryState* pNewUniqState=NewState.IsOk()?&GetUnique(NewState):NULL;
		pair<STATE2INDEX::const_iterator,STATE2INDEX::const_iterator> Range=Source.m_State2Index.equal_range(&it->second);
		for(STATE2INDEX::const_iterator iit=Range.first;iit!=Range.second;iit++)
		{
			m_State2Index.insert(STATE2INDEX::value_type(pNewUniqState,iit->second));
			m_Index2State.insert(INDEX2STATE::value_type(iit->second,pNewUniqState));
		}
	}
	//for(MAP::const_iterator it=Source.m_Map.begin(),_e=Source.m_Map.end();it!=_e;it++)
	//{
	//	_CBoundaryState NewState(it->second,NodesConnection,Dongles);
	//	if(NewState.IsOk())
	//		m_Map.insert(MAP::value_type(it->first,NewState));
	//}
}

_CBoundaryState& _CBounadryStatesVector::GetUnique(_CBoundaryState& Source)
{
	unsigned long long Hash=Source.GetHash();
	pair<UNIQ_MAP::iterator,UNIQ_MAP::iterator> Range=m_UniqMap.equal_range(Hash);
	for(UNIQ_MAP::iterator it=Range.first;it!=Range.second;it++)
		if(it->second==Source)
			return it->second;
	return m_UniqMap.insert(UNIQ_MAP::value_type(Hash,Source))->second;
}

void _CBounadryStatesVector::Store(_binary_filer& Filer) 
{
	Filer<<m_UniqMap.size();
	for(UNIQ_MAP::iterator it=m_UniqMap.begin(),_e=m_UniqMap.end();it!=_e;it++)
	{
		Filer<<it->first;
		_CBoundaryState& State=it->second;
		Filer<<State;
		pair<STATE2INDEX::iterator,STATE2INDEX::iterator> Range=m_State2Index.equal_range(&State);
		size_t Index=0;
		long long IndexPos=Filer.pos();
		Filer<<Index;
		for(STATE2INDEX::iterator rit=Range.first;rit!=Range.second;rit++,Index++)
			Filer<<rit->second;
		long long EndPos=Filer.pos();
		Filer.set(IndexPos);
		Filer<<Index;
		Filer.set(EndPos);
	}
}

void _CBounadryStatesVector::Load(_binary_filer& Filer) 
{
	m_UniqMap.clear();
	m_Index2State.clear();
	m_State2Index.clear();
	size_t NoOfStates=0, NoOfIndices=0;
	Filer>>NoOfStates;
	for(size_t i=0;i<NoOfStates;i++)
	{
		unsigned long long Hash=0;
		Filer>>Hash;
		_CBoundaryState State;
		Filer>>State;
		unsigned long long TestHash=State.GetHash();
		ASSERTPDD(TestHash==Hash);
		_CBoundaryState& ResState=m_UniqMap.insert(UNIQ_MAP::value_type(Hash,State))->second;
		Filer>>NoOfIndices;
		for(size_t j=0;j<NoOfIndices;j++)
		{
			size_t Index;
			Filer>>Index;
			m_Index2State.insert(INDEX2STATE::value_type(Index,&ResState));
			m_State2Index.insert(STATE2INDEX::value_type(&ResState,Index));
		}
	}
	//m_Map.clear();
	//size_t Size=0;
	//Filer>>Size;
	//for(size_t i=0;i<Size;i++)
	//{
	//	size_t Pos=0;
	//	Filer>>Pos;
	//	_CBoundaryState State;
	//	Filer>>State;
	//	m_Map.insert(MAP::value_type(Pos,State));
	//}
}
bool _CBounadryStatesVector::ReverseDeletions(size_t Index, _CMultiDeletions& Dels)
{
	INDEX2STATE::iterator fit=m_Index2State.find(Index);
	if(fit==m_Index2State.end() || fit->second==NULL)
		return false;
	fit->second->ReverseCofactor(Dels);
	return true;
}
bool _CBounadryStatesVector::GetNext(_CBoundaryState*& pNextState, TERMINALS_LIST& TerminalsLits)
{
	if(m_EnumIt==m_UniqMap.end())
		m_EnumIt=m_UniqMap.begin();
	else
		++m_EnumIt;
	if(m_EnumIt==m_UniqMap.end())
		return false;
	pNextState=&m_EnumIt->second;
	pair<STATE2INDEX::iterator,STATE2INDEX::iterator> Range=m_State2Index.equal_range(pNextState);
	TerminalsLits.clear();
	for(STATE2INDEX::iterator it=Range.first;it!=Range.second;it++)
		TerminalsLits.push_back(it->second);
	return true;
}

//*******************************_CBounadryStatesVector***********************
//*******************************_CTempVertex*********************************
_CTempVertex::DESC_DATA& _CTempVertex::GetDescData(size_t DescId)
{
	if(DescId>=m_Descendants.size())
		RISEPDD(out_of_range,"Descendant index out of range");
	return m_Descendants[DescId];
}
const _CTempVertex::DESC_DATA& _CTempVertex::GetDescData(size_t DescId) const
{
	if(DescId>=m_Descendants.size())
		RISEPDD(out_of_range,"Descendant index out of range");
	return m_Descendants[DescId];
}

bool _CTempVertex::Is0() 
{
	return this == &(m_pCircuitAnalyzer->m_0_Vertex);
}
bool _CTempVertex::Is1()
{
	return this==&(m_pCircuitAnalyzer->m_1_Vertex);
}

unsigned long long _CTempVertex::GetHash(unsigned long long Core) const
{
	Core=m_pComponent->GetHash(HASH_CORE);
	Core*=HASH_FACTOR;
	size_t NoOfDesc=m_Descendants.size();
	Core^=NoOfDesc;
	for(DESCENDANT_VECTOR::const_iterator it=m_Descendants.begin(),_e=m_Descendants.end();it!=_e;it++)
	{
		Core*=HASH_FACTOR;
		Core^=(unsigned long long)it->first;
		Core*=HASH_FACTOR;
		Core^=(unsigned long long)it->second;
	}
	return Core;
}

bool _CTempVertex::operator==(_CTempVertex& Right)
{
	if(m_pComponent!=Right.m_pComponent) return false;
	if(m_Descendants.size()!=Right.m_Descendants.size()) return false;
	for(DESCENDANT_VECTOR::iterator it1=m_Descendants.begin(),it2=Right.m_Descendants.begin(),
		_e=m_Descendants.end();it1!=_e;it1++,it2++)
	{
		if(it1->second!=it2->second) return false;
		if(it1->first==NULL && it2->first==NULL)
		{
			if(m_pCircuitAnalyzer->DonglIndex((const _CTempVertex**)&(it1->first))
				!= m_pCircuitAnalyzer->DonglIndex((const _CTempVertex**)&(it2->first)))
				return false;
		}
		else if(it1->first!=it2->first) return false;
	}
	return true;
}

short _CTempVertex::RetriveComMult()
{
	list<short> Sgns;
	for (auto& Desc : m_Descendants)
		Sgns.push_back(Desc.second);
	short nwd = NWD(Sgns);
	if (nwd != 1 && nwd!=0)
		for (auto& Desc : m_Descendants)
			Desc.second /= nwd;
	return nwd;


	//short Res=1,Mask=1,sgn=1;
	//int dg_max=numeric_limits<short>::digits;
	//size_t _s=m_Descendants.size();
	//int Shift=0;
	//bool SgnDetermined=false,ShiftDetermined=false;
	//for(;Shift<dg_max;Shift++,Mask<<=1)
	//{
	//	if(!SgnDetermined && Shift>0)
	//		return 0;
	//	for(size_t i=0;i<_s;i++)
	//	{
	//		short Sgn=m_Descendants[i].second;
	//		if(!SgnDetermined && Sgn!=0) 
	//		{
	//			sgn=Sgn<0?-1:1;
	//			SgnDetermined=true;
	//		}
	//		if((Sgn & Mask)!=0)
	//		{
	//			ShiftDetermined=true;
	//			break;
	//		}
	//	}
	//	if(ShiftDetermined)
	//		break;
	//}
	//Res<<=Shift;
	//if(Shift>0 || sgn!=1)
	//	for(DESCENDANT_VECTOR::iterator it=m_Descendants.begin(),_e=m_Descendants.end();it!=_e;it++)
	//	{
	//		short& M=it->second;
	//		M>>=Shift;
	//		M*=sgn;
	//	}
	//return Res*sgn;
}

bool _CTempVertex::IsHollow()
{
	int HollowDesc=m_pComponent->HollowOutput();
	if(HollowDesc<0)
		return false;
	for(size_t i=0,_s=m_Descendants.size();i<_s;i++)
		if(i!=HollowDesc && (m_Descendants[i].first==NULL /*|| !m_Descendants[i].first->Is0()*/))
			return false;
	return true;
}
const _CTempVertex::DESC_DATA* _CTempVertex::GetHollowPinData()
{
	int HollowPin=m_pComponent->HollowOutput();
	if(HollowPin<0 || HollowPin>(int)m_Descendants.size())
		return NULL;
	return &m_Descendants[HollowPin];
}

const _CTempVertex::DESC_DATA* _CTempVertex::GetPinData(size_t Index) const 
{
	if (Index >= m_Descendants.size())
		return nullptr;
	return &m_Descendants[Index];
}


//*******************************_CTempVertex*********************************
//*******************************_CBasicCofactorVector************************
_CBasicCofactorVector::_CBasicCofactorVector(const _CCofactorsVector& Source)
{
	resize(Source.size());
	for (size_t i = 0; i < Source.size(); i++)
	{
		_CMultiBasicSetOfDeletions& MSet = at(i);
		Source[i]->GetDeletions(MSet);
		//const _CMultiDeletions& MDels = Source[i]->GetDeletions();
		//for (auto& DelsPairs : MDels)
		//	MSet.push_back(DelsPairs.second);				
	}
	//for (size_t i = 0; i<Source.size(); i++)
	//	at(i) = _CMultiBasicSetOfDeletions(Source[i]->GetDeletions());
}

//*******************************_CBasicCofactorVector************************

//*******************************_CCircuitAnalyzer****************************
_CCircuitAnalyzer::_CCircuitAnalyzer(
	_CCircuit* pTopCircuit,
	_CComponents& IdealComponentsList,
	_CComponents& MainComponentsList,
	bool OrderOotimisation,
	COMPONENT_PINS_SET* pBoundaryPins,
	_CCofactorsVector& TopCofactors):
		m_TopCofactors(TopCofactors),
		m_IdealComponentsList(IdealComponentsList),
		m_MainComponentsList(MainComponentsList),
		//m_pCurrentStateCache(&m_Cache1), 
		//m_pPreveStateCache(NULL),
		m_pBaseCircuit(pTopCircuit),
		m_TopStateCofVector(TopCofactors.size(),NULL),
		m_TopTempVertexVector(TopCofactors.size()),
		m_pSeparateNodes2Test(NULL),
		m_pCurrentTempVerts2Redefine(&m_TempVerts2Redefine1),
		m_pPrevTempVerts2Redefine(&m_TempVerts2Redefine2),
		m_pPrevStateCofactors(&m_StateCofactorSet1),
		m_pCurrentStateCofactors(&m_StateCofactorSet2),
		m_IsSubcircuit(pBoundaryPins!=NULL),
		//m_pSubcircuitOutData(NULL),
		m_pBoundaryPins(pBoundaryPins==NULL?NULL:new COMPONENT_PINS_SET(*pBoundaryPins)),
		//m_pOptimizationMap(NULL),
		//m_pOptimizationMapH(NULL),
		//m_pOptimizationMapC(NULL),
		//_CMultLevelInterface(0,NULL),
		m_HollowState(NULL),
		//m_pGreadyResContainer(NULL),
		//m_GreadyResAnalyser(MainComponentsList),
		m_CanonizationMode(false),
		//m_pFastOptimizer(NULL),
		m_FastGreedyAnalyser(m_MainComponentsList)
{
	m_1_Vertex.m_pCircuitAnalyzer=this;
	m_0_Vertex.m_pCircuitAnalyzer=this;
	if(m_pBoundaryPins!=NULL)
	{
		m_IncidentTable.AddPins(*m_pBoundaryPins,false);
		m_TouchedPinSet.insert(m_pBoundaryPins->begin(),m_pBoundaryPins->end());
		TouchPin(m_TopCofactors);
	}
	m_IncidentTable.AddCompPins(m_IdealComponentsList,true,&m_pSeparateNodes2Test);
	m_IncidentTable.AddCompPins(m_MainComponentsList,false,&m_pSeparateNodes2Test);
	//m_GreadyResAnalyser.m_pAnalyzer=this;
	m_FastGreedyAnalyser.m_pAnlyser=this;
}

_CCircuitAnalyzer::~_CCircuitAnalyzer()
{
	//delete m_pSubcircuitOutData;
	//delete m_pBoundaryPins;
	//delete m_pOptimizationMap;
	//delete m_pOptimizationMapH;
	//delete m_pOptimizationMapC;
	//delete m_pGreadyResContainer;
	//delete m_pFastOptimizer;
}

void _CCircuitAnalyzer::PrepareCofactorsStates()
{
	_CStateCofactor IdealBaseState(this);
	for(_CComponents::iterator it=m_IdealComponentsList.begin(),_e=m_IdealComponentsList.end();
		it!=_e;it++)
	{
		TouchPin(**it);
		const _CMultiBasicSetOfDeletions* pDeletions=NULL;
		(*it)->GetDescendantDeletions(0,pDeletions);
		ASSERTPDD(pDeletions!=NULL);
		IdealBaseState.AddDeletions(*pDeletions);
		if(!IdealBaseState.IsInProgres())
			break;
	}
	//if(!m_pSeparateNodes2Test->empty())
	//	IdealBaseState.ReduceSeparateNodes(*m_pSeparateNodes2Test);
	if(m_TopCofactors.empty())
	{
		_CStateCofactor* pTopState=new _CStateCofactor(IdealBaseState);
		try
		{
			AD_DELETION Res=pTopState->GetState();
			if(Res==AD_OK)
			{
				if(!m_pSeparateNodes2Test->empty())
					Res=pTopState->ReduceSeparateNodes(*m_pSeparateNodes2Test);
				if(Res==AD_OK && 
					!CanBeDifferentThenZero(*pTopState,IGNORE_COMP_SET(),false))
					Res=AD_DISCONNECTION;
			}
			m_TopTempVertexVector.resize(1);
			switch(Res)
			{
			case AD_OK: 
			case AD_COMPLETE: 
				{
					short Sgn=1;
					//_CStateCofactor* pNewTopState=m_pCurrentStateCache->RegisterUnique(pTopState,Sgn);
					_CStateCofactor* pNewTopState=m_StateCache.RegisterUnique(pTopState,Sgn);
					m_pCurrentStateCofactors->insert(pNewTopState);
					//m_pCurrentStateCofactors->insert(pNewTopState);
					m_TopStateCofVector.push_back(pNewTopState);
					m_HollowState=pNewTopState;
					VERTEX_ROOT& VertexRoot=m_TopTempVertexVector[0];
					VertexRoot.first=NULL;
					VertexRoot.second=Sgn;
					m_pCurrentTempVerts2Redefine->insert(TEMP_VERTS_2_REDEFINE::value_type(pNewTopState,
						&VertexRoot));
					break;
				}
			default:
				{
					VERTEX_ROOT& VertexRoot=m_TopTempVertexVector[0];
					VertexRoot.first = &m_0_Vertex;
					//VertexRoot.first = nullptr;
					VertexRoot.second = 0;
					if(!pTopState->IsRegistered())
						delete pTopState;
				}
			}
		}
		catch(...)
		{
			if(!pTopState->IsRegistered())
				delete pTopState;
			throw;
		}
	}
	else
		for(size_t i=0;i<m_TopCofactors.size();i++)
		{
			_CCofactor* pCof=m_TopCofactors[i]; 
			TouchPin(*pCof);
			_CMultiBasicSetOfDeletions Dels;
			pCof->GetDeletions(Dels);
			//_CBasicSetOfDeletions Dels(pCof->GetDeletions());
			_CStateCofactor* pTopState=new _CStateCofactor(IdealBaseState);
			try
			{
				
				AD_DELETION Res=pTopState->AddDeletions(Dels,*m_pSeparateNodes2Test);
				if(Res==AD_OK)
				{
					//if(!m_pSeparateNodes2Test->empty())
					//	Res=pTopState->ReduceSeparateNodes(*m_pSeparateNodes2Test);
					if(Res==AD_OK && 
						!CanBeDifferentThenZero(*pTopState,IGNORE_COMP_SET(),false))
						Res=AD_DISCONNECTION;
				}
				switch(Res)
				{
				case AD_OK: 
				case AD_COMPLETE: 
					{
						short Sgn=1;
						//_CStateCofactor* pNewTopState=m_pCurrentStateCache->RegisterUnique(pTopState,Sgn);
						_CStateCofactor* pNewTopState=m_StateCache.RegisterUnique(pTopState,Sgn);
						//if(pNewTopState==pTopState)
						m_pCurrentStateCofactors->insert(pNewTopState);
						m_TopStateCofVector[i]=pNewTopState;
						VERTEX_ROOT& VertexRoot=m_TopTempVertexVector[i];
						VertexRoot.first=NULL;
						VertexRoot.second=Sgn;
						m_pCurrentTempVerts2Redefine->insert(TEMP_VERTS_2_REDEFINE::value_type(pNewTopState,
							&VertexRoot));
						break;
					}
				default:
					{
						VERTEX_ROOT& VertexRoot=m_TopTempVertexVector[i];
						VertexRoot.first = &m_0_Vertex;
						//VertexRoot.first = nullptr;
						VertexRoot.second = 0;
						if(!pTopState->IsRegistered())
							delete pTopState;
					}
				}
			}
			catch(...)
			{
				if(!pTopState->IsRegistered())
					delete pTopState;
				throw;
			}
		}
	if(!m_pSeparateNodes2Test->empty())
	{
		m_IgnoredPins.insert(m_pSeparateNodes2Test->begin(),m_pSeparateNodes2Test->end());
		//m_GreedyResCache.ClearCache();
		RemoveTouches(*m_pSeparateNodes2Test);
		//m_StateCache.Clear();
	}
}
void _CCircuitAnalyzer::AddAnotherComponent(_CComponent& NextComponent, bool Try2OptimizeOrder)
{
	SwitchAll();
	m_StateCache.KillOutOfDate();
	int HollowDesc=NextComponent.HollowOutput();
	//ASSERTPDD(m_pPreveStateCache!=NULL && m_pPrevS2VMap!=NULL && m_pPrevV2SMap!=NULL);
	m_IncidentTable.RemoveCompPins(NextComponent,m_pSeparateNodes2Test);
	typedef vector<const _CMultiBasicSetOfDeletions*> DESC_SETS_OFDELETIONS;
	size_t NoOfDescendants=NextComponent.NoOfEffevtiveDescendants();
	DESC_SETS_OFDELETIONS DescSetsOfDeletions(NextComponent.NoOfEffevtiveDescendants());
	for(size_t i=0;i<NoOfDescendants;i++)
	{
		NextComponent.GetDescendantDeletions(i,DescSetsOfDeletions[i]);
	}
	unsigned Counter=0;
	for(STATE_COFACTOR_SET::iterator sc_it=m_pPrevStateCofactors->begin(),
		sc_e=m_pPrevStateCofactors->end();
		sc_it!=sc_e;
		sc_it++,Counter++)
	{
		_CStateCofactor* pTopCofactor=*sc_it;
		typedef pair<TEMP_VERTS_2_REDEFINE::iterator,TEMP_VERTS_2_REDEFINE::iterator> RANGE;
		RANGE Range=m_pPrevTempVerts2Redefine->equal_range(pTopCofactor);
		bool IsHollow=true, IsEmpty=true;
		//typedef pair<_CStateCofactor*,short> DESC_COFACTORS;
		//typedef vector<DESC_COFACTORS> DESC_COFACTORS_V;
		DESC_COFACTORS_V DescCofactorsV(NoOfDescendants,DESC_COFACTORS(NULL,0));
		IGNORE_COMP_SET IgnoreSet;
		IgnoreSet.insert(&NextComponent);
		for(size_t i=0;i<NoOfDescendants;i++)
		{
			_CStateCofactor* pNewTopCofactor=new _CStateCofactor(*pTopCofactor);
			try
			{
				AD_DELETION Res=pNewTopCofactor->AddDeletions(*DescSetsOfDeletions[i],*m_pSeparateNodes2Test);
				if(Res==AD_OK ||
					Res==AD_COMPLETE)
				{
					//COMPONENT_PINS_LIST LocalNodes2Test(*m_pSeparateNodes2Test);
					//if(!m_pSeparateNodes2Test->empty())
					//	Res=pNewTopCofactor->ReduceSeparateNodes(LocalNodes2Test);
					if(Try2OptimizeOrder && Res==AD_OK &&
							!CanBeDifferentThenZero(*pNewTopCofactor,IgnoreSet,false))
						Res=AD_DISCONNECTION;
				}
				switch(Res)
				{
				case AD_OK:
				case AD_COMPLETE:
					{
						//COMPONENT_PINS_LIST LocalNodes2Test(*m_pSeparateNodes2Test);
						////m_IncidentTable.CorrectCompPins(LocalNodes2Test);
						//if(!m_pSeparateNodes2Test->empty())
						//	pNewTopCofactor->ReduceSeparateNodes(LocalNodes2Test);
						_CStateCofactor* pCurTopCofactor=DescCofactorsV[i].first=
							m_StateCache.RegisterUnique(pNewTopCofactor,DescCofactorsV[i].second);
							//m_pCurrentStateCache->RegisterUnique(pNewTopCofactor,DescCofactorsV[i].second);
						//if(pCurTopCofactor==pNewTopCofactor)
						m_pCurrentStateCofactors->insert(pCurTopCofactor);
						size_t SS = m_pCurrentStateCofactors->size();
						if(HollowDesc<0 || HollowDesc!=i)
							IsHollow=false;
						IsEmpty=false;
						break;
					}
				default:
					delete pNewTopCofactor;
				}
			} 
			catch(...)
			{
				if(pNewTopCofactor!=NULL && !pNewTopCofactor->IsRegistered())
					delete pNewTopCofactor;
				throw;
			}
		}
		if(IsEmpty)
		{
			for(TEMP_VERTS_2_REDEFINE::iterator it=Range.first;it!=Range.second;it++)
			{
				it->second->first = &m_0_Vertex;
				//it->second->first = nullptr;
				it->second->second = 0;
			}
		}
		else
		{
			if(IsHollow)
			{
				DESC_COFACTORS& DescState=DescCofactorsV[HollowDesc];
				for(TEMP_VERTS_2_REDEFINE::iterator it=Range.first;it!=Range.second;it++)
				{
					VERTEX_ROOT* pOldVertexRoot=it->second;
					pOldVertexRoot->second*=DescState.second;
					m_pCurrentTempVerts2Redefine->insert(
						TEMP_VERTS_2_REDEFINE::value_type(DescState.first,pOldVertexRoot));
				}
			}
			else
			{
				_CTempVertex* pVertex=NULL;
				short TopMultiplier=CommMultiplierExtract(DescCofactorsV);
				bool IsNew=GetNewOrExistedVertex(pVertex,&NextComponent,DescCofactorsV);
				//_CTempVertex& NewVertex=GetNewVertex(&NextComponent);
				for(TEMP_VERTS_2_REDEFINE::iterator it=Range.first;it!=Range.second;it++)
				{
					//it->second->first=&NewVertex;
					it->second->first=pVertex;
					it->second->second*=TopMultiplier;
				}
				if(IsNew)
					for(size_t i=0;i<NoOfDescendants;i++)
					{
						DESC_COFACTORS& DescCof=DescCofactorsV[i];
						VERTEX_ROOT& VertRoot=pVertex->GetDescData(i);
						VertRoot.second=DescCof.second;
						if(DescCof.first==NULL)
							//VertRoot.first = nullptr;
							VertRoot.first = &m_0_Vertex;
						else
							m_pCurrentTempVerts2Redefine->insert(
								TEMP_VERTS_2_REDEFINE::value_type(DescCof.first,&VertRoot));
					}
			}
		}
	}
	//m_AnalyzedComponents.Move2Me(&NextComponent,m_MainComponentsList);
	if(!m_pSeparateNodes2Test->empty())
	{
		m_IgnoredPins.insert(m_pSeparateNodes2Test->begin(),m_pSeparateNodes2Test->end());
		//m_GreedyResCache.ClearCache();
		//m_StateCache.Clear();
	}
	TouchPin(NextComponent);
	if (!m_pSeparateNodes2Test->empty())
		RemoveTouches(*m_pSeparateNodes2Test);
}
#ifdef _ALLOW_REDUNDANCY
bool _CCircuitAnalyzer::CanBeDifferentThenZero(const _CStateCofactor& InputState,IGNORE_COMP_SET& IgnoreSet,bool IgnoreCache)
{
	//return true;
	return m_FastGreedyAnalyser.CanBeComplete(InputState,IgnoreSet);
	//return m_FastGreedyAnalyser.CanBeComplete(_CNewGreedyStateCofactor(InputState,m_GreadyResAnalyser),IgnoreSet);

	//m_GreadyResAnalyser.TempRemoveComponent(IgnoreSet);
	//bool Res=m_GreadyResAnalyser.CanBeComplete(_CNewGreedyStateCofactor(InputState,m_GreadyResAnalyser));
	//m_GreadyResAnalyser.RetryTempRemovedComponents();
	//return Res;
}

#else

bool _CCircuitAnalyzer::CanBeDifferentThenZero(const _CStateCofactor& InputState,IGNORE_COMP_SET& IgnoreSet,bool IgnoreCache)
{
	//return true;
	m_GreadyResAnalyser.Store();
	m_GreadyResAnalyser.TempRemoveComponent(IgnoreSet);
#ifdef _INCREMENT_TOP
	INT_LIST NodesList;
	m_GreadyResAnalyser.PrepareNodesInPorgres(NodesList);
	bool Res=m_GreadyResAnalyser.CanBeComplete(_CNewGreedyStateCofactor(InputState,m_GreadyResAnalyser,NodesList));
#endif
	bool Res=m_GreadyResAnalyser.CanBeComplete(_CNewGreedyStateCofactor(InputState,m_GreadyResAnalyser));
	m_GreadyResAnalyser.Restore();
	return Res;
}
#endif


inline unsigned _CCircuitAnalyzer::NoOfNodes() const
{
	ASSERTPDD(m_pBaseCircuit!=NULL);
	return m_pBaseCircuit->NoOfNodes();
}

inline unsigned _CCircuitAnalyzer::HighestNoOfNodes(bool Row) const
{
	ASSERTPDD(m_pBaseCircuit!=NULL);
	return m_pBaseCircuit->HighestNoOfNode()-(Row?0:m_IgnoredPins.size());
}

void _CCircuitAnalyzer::SwitchTempVert2Redef()
{
	if(m_pPrevTempVerts2Redefine!=NULL)
		m_pPrevTempVerts2Redefine->clear();
	swap(m_pPrevTempVerts2Redefine,m_pCurrentTempVerts2Redefine);
}

//void _CCircuitAnalyzer::SwitchCache()
//{
//	if(m_pPreveStateCache!=NULL)
//		m_pPreveStateCache->Clear();
//	m_pPreveStateCache=m_pCurrentStateCache;
//	m_pCurrentStateCache=m_pCurrentStateCache==&m_Cache1?&m_Cache2:&m_Cache1;
//}

_CTempVertex& _CCircuitAnalyzer::GetNewVertex(_CComponent* pComponent)
{
	m_TempVerticesList.push_back(_CTempVertex(this,pComponent));
	return m_TempVerticesList.back();
}
short _CCircuitAnalyzer::CommMultiplierExtract(DESC_COFACTORS_V& Desc) const
{
	list<int> Sgns;
	for (auto& D : Desc)
		Sgns.push_back(D.second);
	int ComDiv = NWD(Sgns);
	if (ComDiv!=1)
		for (auto& D : Desc)
			D.second/=ComDiv;
	return ComDiv;

	//short ComM=1;
	//DESC_COFACTORS_V::iterator _e=Desc.end();
	//bool IsNeg=true;
	//short Max2Power=numeric_limits<short>::max();
	//for(DESC_COFACTORS_V::iterator it=Desc.begin();it!=_e;it++)
	//{
	//	short LocalMult=it->second;
	//	if(LocalMult==0)
	//		continue;
	//	IsNeg=IsNeg && (LocalMult<0);
	//	short LocalMax2Power=0;
	//	while((LocalMult & (short)1)==0)
	//	{
	//		LocalMult>>=1;
	//		LocalMax2Power++;
	//	}
	//	Max2Power=min(Max2Power,LocalMax2Power);
	//	if(!IsNeg && Max2Power==0)
	//		break;
	//}
	//if(IsNeg || Max2Power>0)
	//	for(DESC_COFACTORS_V::iterator it=Desc.begin();it!=_e;it++)
	//	{
	//		short& LocalMult=it->second;
	//		for(short i=0;i<Max2Power;i++)
	//			LocalMult>>=1;
	//		if(IsNeg)
	//			LocalMult*=-1;
	//	}
	//for(short i=0;i<Max2Power;i++)
	//	ComM<<=1;
	//return IsNeg?-ComM:ComM;
}

bool _CCircuitAnalyzer::GetNewOrExistedVertex(_CTempVertex*& PVertex,
						   _CComponent* pComponent,
						   const DESC_COFACTORS_V& Desc)//true if is new
{
	_CUniqueCacheContainer AuxContainer(pComponent,Desc);
	unsigned long long Hash=AuxContainer.GetHash();
	pair<AUX_TEMP_VERTEX_MAP::iterator,AUX_TEMP_VERTEX_MAP::iterator> Range=m_AuxTempVertexMap.equal_range(Hash);
	for(AUX_TEMP_VERTEX_MAP::iterator it=Range.first;it!=Range.second;it++)
		if(it->second==AuxContainer)
		{
			PVertex=it->second.m_pVertex;
			return false;
		}
	PVertex=&GetNewVertex(pComponent);
	AuxContainer.m_pVertex=PVertex;
	m_AuxTempVertexMap.insert(AUX_TEMP_VERTEX_MAP::value_type(Hash,AuxContainer));
	return true;
}

void _CCircuitAnalyzer::FinishAnalysis()
{
	//SwitchAll();
	if(m_IsSubcircuit)
	{
		m_pSubcircuitOutData = make_unique<_CSubciruitOutsData>(*this);
		//m_pSubcircuitOutData = new _CSubciruitOutsData(*this);
	}
	else
	{
		_CStateCofactor* pState2Remove=NULL;
		for(TEMP_VERTS_2_REDEFINE::iterator it=m_pCurrentTempVerts2Redefine->begin(),
			_e=m_pCurrentTempVerts2Redefine->end();it!=_e;)
			if(it->first->IsComplete())
			{
				it->second->first=&m_1_Vertex;
				short CorSgn=it->first->Sgn();
				it->second->second*=CorSgn;
				if(pState2Remove!=it->first)
				{
					pState2Remove=it->first;
					m_pCurrentStateCofactors->erase(pState2Remove);
				}
				it=m_pCurrentTempVerts2Redefine->erase(it);
			}
			else
				it++;
	}
	//czyszczenie
}

size_t _CCircuitAnalyzer::NoOfModelDescendants() const
{
	ASSERTPDD(m_IsSubcircuit && m_pSubcircuitOutData!=NULL);
	return m_pSubcircuitOutData->GetNoOfModelDescendants();
}

//void _CCircuitAnalyzer::GetOriginalDesndantDeltions(size_t Index,_CBasicSetOfDeletions& Deletions) const
//{
//	ASSERTPDD(m_IsSubcircuit && m_pSubcircuitOutData!=NULL);
//	_CStateCofactor* pStateCof=m_pSubcircuitOutData->GetDescCofactor(Index);
//	pStateCof->ReverseCofactor(Deletions);
//}
//void _CCircuitAnalyzer::GetOriginalDesndantDeltions(size_t Index,_CCofactor& Cofactors) const
//{
//	ASSERTPDD(m_IsSubcircuit && m_pSubcircuitOutData!=NULL);
//	_CStateCofactor* pStateCof=m_pSubcircuitOutData->GetDescCofactor(Index);
//	pStateCof->ReverseCofactor(Cofactors);
//}

//void _CCircuitAnalyzer::GetTranslatedAllDesndantDeltions(const PINS_COLOCATIONS& PinsColocations,
//	BASIC_SET_OF_DELETIONS& SetOfDeletions) const
//{
//	SetOfDeletions.clear();
//	ASSERTPDD(m_IsSubcircuit && m_pSubcircuitOutData!=NULL);
//	COMPONENT_PINS_LIST DisocnectedPins;
//	FIndNotConnected(PinsColocations,DisocnectedPins);
//	bool AreDisconnections=!DisocnectedPins.empty();
//	_CBasicSetOfDeletions Res;
//	for(size_t i=0;i<m_pSubcircuitOutData->GetNoOfModelDescendants();i++)
//	{
//		const _CStateCofactor& OrgBoundCof=m_pSubcircuitOutData->GetOrginalStateCofactor(i);
//		if(AreDisconnections)
//		{
//			_CStateCofactor CopyBoundCof(OrgBoundCof);
//			if(CopyBoundCof.ReDisconnectNodes(DisocnectedPins))
//			{
//				CopyBoundCof.ReverseCofactor(Res);
//				Res.Translate(PinsColocations);
//				SetOfDeletions.insert(BASIC_SET_OF_DELETIONS::value_type(i,Res));
//			}
//		}
//		else
//		{
//			OrgBoundCof.ReverseCofactor(Res);
//			Res.Translate(PinsColocations);
//			SetOfDeletions.insert(BASIC_SET_OF_DELETIONS::value_type(i,Res));
//		}
//	}
//}

void _CCircuitAnalyzer::GetTempVerticesVector(CONST_VERTICES_VECTOR& VertVector)
{
	VertVector.resize(m_TempVerticesList.size());
	VERTICES_LIST::iterator it=m_TempVerticesList.begin(),_e=m_TempVerticesList.end();
	size_t i=0;
	for(;it!=_e;it++,i++)
		VertVector[i]=&*it;
}

void _CCircuitAnalyzer::RegisterTempVertex2ModelVertex(_CTempVertex* const * ppTempVertex,_CModelVertex** ppModelVertex)
{
	ASSERTPDD(m_pSubcircuitOutData!=NULL);
	m_pSubcircuitOutData->RegisterTempVertex2ModelVertex(ppTempVertex,ppModelVertex);
}

const _CCircuitAnalyzer::BOUNDARIES_MAP& _CCircuitAnalyzer::GetBoundariesMap()
{
	ASSERTPDD(m_pSubcircuitOutData!=NULL);
	return m_pSubcircuitOutData->GetBoundariesMap();
}

size_t _CCircuitAnalyzer::GetNoOfSubciruitDongles() const
{
	ASSERTPDD(m_pSubcircuitOutData!=NULL);
	return m_pSubcircuitOutData->GetNoOfSubciruitDongles();
}

void _CCircuitAnalyzer::PrepareDongle(size_t DongleNo, _CMultiDeletions& Deletions, _CModelVertex& ModelVertex, unsigned long& Counter)
{
	ASSERTPDD(m_pSubcircuitOutData!=NULL);
	m_pSubcircuitOutData->PrepareDongle(DongleNo,Deletions,ModelVertex,Counter);
}

void _CCircuitAnalyzer::PrepareVertItList(VERTICES_IT_LIST& List)
{
	//List.assign(m_TempVerticesList.begin(), m_TempVerticesList.end());
	List.clear();
	for(VERTICES_LIST::iterator it=m_TempVerticesList.begin(),_e=m_TempVerticesList.end();it!=_e;it++)
		List.push_front(it);
}

bool _CCircuitAnalyzer::IsVertexHollow(const _CTempVertex& Vertex)
{
	int HollowPin = Vertex.GetHollowPin();
	if (HollowPin < 0)
		return false;
	size_t Len = Vertex.GetDescDataSize();
	if (HollowPin >= (long)Len)
		return false;
	//const _CTempVertex::DESC_DATA* pDescData = Vertex.GetPinData(HollowPin);
	//if (IsDescEmpty(pDescData))
	//	return false;
	for (int i = 0; i < (int)Len; i++)
		if (i != HollowPin)
			if (!IsDescEmpty(Vertex.GetPinData(i)))
				return false;
	return true;
}

bool _CCircuitAnalyzer::IsDescEmpty(const _CTempVertex::DESC_DATA* pDescData)
{
	if (pDescData->first != nullptr)
		if (pDescData->first->Is0() || pDescData->second == 0)
			return true;
		else
			return false;
	//if (pDescData->first != nullptr || pDescData->second == 0)
	//	return false;
	if (m_pSubcircuitOutData == nullptr)
		return false;

	return (long)m_pSubcircuitOutData->DongleIndex((const _CTempVertex**)&(pDescData->first))<0;
}


void _CCircuitAnalyzer::SimplifyToBeCanonic()
{
	//return; 
	m_CanonizationMode=true;
	VERTICES_IT_LIST MyList,Vertices2Remove;
	PrepareVertItList(MyList);
	typedef pair<_CTempVertex*,short> EQUIV;
	typedef hash_map<_CTempVertex*,EQUIV> EQUIV_MAP;
	EQUIV_MAP EquivMap;
	typedef hash_multimap<unsigned long long,_CTempVertex*> LOCAL_CACHE;
	LOCAL_CACHE LocalCache;
	for(VERTICES_IT_LIST::iterator rit=MyList.begin(),_re=MyList.end();rit!=_re;rit++)
	{
		VERTICES_LIST::iterator it=*rit;
		_CTempVertex& Vertex=*it;
		for(size_t i=0;i<Vertex.GetNoOfDescendants();i++)
		{
			_CTempVertex::DESC_DATA& Data=Vertex.GetDescData(i);
			if (Data.first != nullptr && Data.first->Is0())
				Data.second = 0;
			EQUIV_MAP::iterator fit=EquivMap.find(Data.first);
			if(fit!=EquivMap.end())
			{
				Data.first=fit->second.first;
				Data.second*=fit->second.second;
			}
		}
		short TopMult = Vertex.RetriveComMult();
		const _CTempVertex::DESC_DATA* pDescData = Vertex.GetHollowPinData();
		//short TopMult = 1;
		//if (Vertex.IsHollow())
		if (IsVertexHollow(Vertex))
		{
			const _CTempVertex::DESC_DATA* pDescData=Vertex.GetHollowPinData();
			EquivMap.insert(EQUIV_MAP::value_type(&Vertex,EQUIV(pDescData->first,TopMult)));
			Vertices2Remove.push_back(it);
		}
		else
		{
			unsigned long long hash=Vertex.GetHash();
			pair<LOCAL_CACHE::iterator,LOCAL_CACHE::iterator> Range=
				LocalCache.equal_range(hash);
			_CTempVertex* pOldVertex=NULL;
			for(LOCAL_CACHE::iterator cit=Range.first;cit!=Range.second;cit++)
				if(*(cit->second)==Vertex)
				{
					pOldVertex=cit->second;
					break;
				}
			if(pOldVertex==NULL)
			{
				LocalCache.insert(LOCAL_CACHE::value_type(hash,&Vertex));
				if(TopMult!=1)
					EquivMap.insert(EQUIV_MAP::value_type(&Vertex,EQUIV(&Vertex,TopMult)));

			}
			else
			{
				EquivMap.insert(EQUIV_MAP::value_type(&Vertex,EQUIV(pOldVertex,TopMult)));
				Vertices2Remove.push_back(it);
			}
		}
	}
	for(TEMP_VERTEX_VECTOR::iterator it=m_TopTempVertexVector.begin(),_e=m_TopTempVertexVector.end();
		it!=_e;it++)
	{
		_CTempVertex::DESC_DATA &Data=*it;
		EQUIV_MAP::iterator fit=EquivMap.find(Data.first);
		if(fit!=EquivMap.end())
		{
			Data.first=fit->second.first;
			Data.second*=fit->second.second;
		}
	}
	for(VERTICES_IT_LIST::iterator rit=Vertices2Remove.begin(),_re=Vertices2Remove.end();rit!=_re;rit++)
		m_TempVerticesList.erase(*rit);
	m_CanonizationMode=false;
}

const ORIGINAL_DESC_STATES& _CCircuitAnalyzer::GetOriginalDescState()
{
	ASSERTPDD(m_pSubcircuitOutData!=NULL);
	return m_pSubcircuitOutData->GetOriginalDescState();
}
void _CCircuitAnalyzer::ClearGarbage()
{
	//m_ColMap.clear();
	m_StateCache.Clear();
	//m_GreedyResCache.ClearCache();
	m_pCurrentStateCofactors=NULL;
	m_pPrevStateCofactors=NULL;
	m_StateCofactorSet1.clear();
	m_StateCofactorSet2.clear();
	m_pCurrentTempVerts2Redefine=NULL;
	m_pPrevTempVerts2Redefine=NULL;
	m_TempVerts2Redefine1.clear();
	m_TempVerts2Redefine2.clear();
	m_TouchedPinSet.clear();
	m_IncidentTable.Clear();
	m_IgnoredPins.clear();
}

void _CCircuitAnalyzer::ReportMinIncidences(_CComponent& Component,unsigned& MinValue, unsigned& MinNumber)
{
	COMPONENT_PINS Pins;
	Component.RaportConnections(Pins);
	MinNumber=0;
	MinValue=numeric_limits<int>::max();
	for(COMPONENT_PINS::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
	{
		unsigned Pin=*it;
		int Incd=DetermineNoOfIcidenceInNode((int)Pin);
		if(Incd<0)
			continue;
		if(MinValue>(unsigned)Incd)
		{
			MinValue=(unsigned)Incd;
			MinNumber=1;
		}
		else if(MinValue==(unsigned)Incd)
			MinNumber++;
	}
}

int _CCircuitAnalyzer::DetermineNoOfIcidenceInNode(int Node)//-1 means boundary
{
	ASSERTPDD(Node>=0);
	if(m_pBoundaryPins!=NULL && IsIn(*m_pBoundaryPins,(unsigned)Node))
		return -1;
	return m_IncidentTable.GetIncidentComponents((unsigned)Node);
}

void _CCircuitAnalyzer::FIndNotConnected(const PINS_COLOCATIONS& PinsColocations, COMPONENT_PINS_LIST& DisconnectedPins) const
{
	ASSERTPDD(m_IsSubcircuit && m_pSubcircuitOutData!=NULL);
	COMPONENT_PINS_SET AuxSet(*m_pBoundaryPins);
	for(PINS_COLOCATIONS::const_iterator it=PinsColocations.begin(),_e=PinsColocations.end();it!=_e;it++)
	{
		COMPONENT_PINS_SET::iterator s_it=AuxSet.find(it->first);
		if(s_it==AuxSet.end())
			RISEPDD(eWrongPins,(ulong2str(it->first)+" is not a boundary pin").c_str());
		AuxSet.erase(s_it);
	}
	if(!AuxSet.empty())
		DisconnectedPins.assign(AuxSet.begin(),AuxSet.end());
}

bool _CCircuitAnalyzer::IsBoundary(size_t Node)
{
	return m_pBoundaryPins!=NULL && IsIn(*m_pBoundaryPins,Node);
}

_CComponent* _CCircuitAnalyzer::GetNextOptimalComponentFast()
{
	//if (m_pFastOptimizer == NULL)
	//	m_pFastOptimizer = new _CFastCompOptimizer(this, m_MainComponentsList);
	if(!m_pFastOptimizer )
		m_pFastOptimizer = make_unique<_CFastCompOptimizer> (this, m_MainComponentsList,m_TouchedPinSet);
	_CComponent* pComp = m_pFastOptimizer->GetNextComponent();
	if(pComp==NULL)
	{
		m_pFastOptimizer.release();
		//delete m_pFastOptimizer;
		//m_pFastOptimizer = NULL;
	}
	return pComp;
}

_CComponent* _CCircuitAnalyzer::GetNextOptimalComponent()
{
	if(!m_pFastOptimizer/*==NULL*/)
		m_pFastOptimizer = make_unique< _CFastCompOptimizer>(this, m_MainComponentsList,m_TouchedPinSet);
	//m_pFastOptimizer = new _CFastCompOptimizer(this, m_MainComponentsList);

	size_t LocalNoOfCrestedVertices=numeric_limits<size_t>::max();
	size_t LocalNoOfRelDescs=numeric_limits<size_t>::max();
	if(m_MainComponentsList.empty())
	{
		m_pFastOptimizer.release();
		//delete m_pFastOptimizer;
		//m_pFastOptimizer = NULL;
		return NULL;
	}
	if(m_pOptimizationMap!=NULL && !m_pOptimizationMap->empty() && m_pOptimizationMap->begin()->second.GetCreatedVertices()==0)
	{
		OPTIMIZATION_MAP::iterator it=m_pOptimizationMap->begin();
		_CComponent* pComp=it->first;
		m_pOptimizationMap->erase(it);
		m_pFastOptimizer->RemovComp(pComp);
		return pComp;
	}
	LIST_OF_COMPONENTS FeasableComponents;
	m_pFastOptimizer->GetTouchedAndLast(FeasableComponents);
	m_MainComponentsList.ReduceLowerPriority(FeasableComponents);
	if (FeasableComponents.empty())
		FeasableComponents.assign(m_MainComponentsList.begin(), m_MainComponentsList.end());
	//m_pFastOptimizer->GetSmallestIncds(FeasableComponents);
	//if (m_MainComponentsList.size() == 1)
	if(FeasableComponents.size()==1)
	{
		//_CComponent* pComp=m_MainComponentsList.front();
		_CComponent* pComp=FeasableComponents.front();
		m_pFastOptimizer->RemovComp(pComp);
		return pComp;
	}
	//if (m_pOptimizationMap == NULL)
	//	m_pOptimizationMap = new OPTIMIZATION_MAP;
	if (!m_pOptimizationMap)
		m_pOptimizationMap = make_unique<OPTIMIZATION_MAP>();
	//else if(!m_pOptimizationMap->empty() && m_pOptimizationMap->begin()->second.GetCreatedVertices()==0)
	else
		m_pOptimizationMap->clear();

	//if (m_pOptimizationMapH == NULL)
	//	m_pOptimizationMapH = new OPTIMIZATION_MAP;
	if (!m_pOptimizationMapH)
		m_pOptimizationMapH = make_unique<OPTIMIZATION_MAP>();
	else
		m_pOptimizationMapH->clear();

	//if (m_pOptimizationMapC == NULL)
	//	m_pOptimizationMapC = new OPTIMIZATION_MAP;
	if (!m_pOptimizationMapC)
		m_pOptimizationMapC = make_unique<OPTIMIZATION_MAP>();
	else
		m_pOptimizationMapC->clear();
	bool ContainSubCirc=false;
#ifdef _REORDER
	for(_CNewGreedyAnalyser::COMPONENTS_LIST::iterator it=m_OrderedComponentsList.begin(),_e=m_OrderedComponentsList.end();it!=_e;it++)
#else
	//for(_CComponents::iterator it=m_MainComponentsList.begin(),_e=m_MainComponentsList.end();it!=_e;it++)
	for(LIST_OF_COMPONENTS::iterator it=FeasableComponents.begin(),_e=FeasableComponents.end();it!=_e;it++)
#endif
	{
#ifdef _REORDER
		_CComponent* pComp=it->first;
#else
		_CComponent* pComp=*it;
#endif
		//if(!CoincidentWithProcessed(*pComp))
		//	continue;
		STATE_COFACTOR_SET LocalStateSet;
		_CStateCofactorCache LocalCache;
		//_COptimizationComponentData Data(*this,*pComp,m_Level,this,LocalCache,LocalStateSet);
		_COptimizationComponentData Data(*this,*pComp,LocalCache,LocalStateSet);

		Data.TestInfluence();
		size_t CreatedVertices=Data.GetCreatedVertices();
		//size_t RelDesc = Data.GetRelativeDescendantsInNextLevel();
		size_t RelDesc = Data.GetDescendantsInNextLevel();
		if (LocalNoOfCrestedVertices >= CreatedVertices)
		{
			if(LocalNoOfCrestedVertices>CreatedVertices)
			{
				m_pOptimizationMapC->clear();
				LocalNoOfCrestedVertices=CreatedVertices;
				ContainSubCirc=false;
			}
			if(pComp->IsMacroDependant())
				ContainSubCirc=true;
			m_pOptimizationMapC->insert(OPTIMIZATION_MAP::value_type(pComp,Data));
		}
		if(LocalNoOfRelDescs>=RelDesc)
		{
			if(LocalNoOfRelDescs>RelDesc)
			{
				m_pOptimizationMapH->clear();
				LocalNoOfRelDescs=RelDesc;
			}
			m_pOptimizationMapH->insert(OPTIMIZATION_MAP::value_type(pComp,Data));
		}
	}
	//if (ContainSubCirc)
	//	m_pOptimizationMap = m_pOptimizationMapH->size()<m_pOptimizationMapC->size() ?
	//m_pOptimizationMapH : m_pOptimizationMapC;
	////(*m_pOptimizationMap)=(m_pOptimizationMapH->size()<m_pOptimizationMapC->size())?
	////	(*m_pOptimizationMapH):(*m_pOptimizationMapC);
	//else
	//	m_pOptimizationMap = m_pOptimizationMapC->size()<m_pOptimizationMapH->size() ?
	//m_pOptimizationMapC : m_pOptimizationMapH;
	////(*m_pOptimizationMap)=(m_pOptimizationMapC->size()<m_pOptimizationMapH->size())?
	////	(*m_pOptimizationMapC):(*m_pOptimizationMapH);
	if (m_pOptimizationMapH->size() < m_pOptimizationMapC->size())
		m_pOptimizationMap->insert(m_pOptimizationMapH->begin(), m_pOptimizationMapH->end());
	else
		if (m_pOptimizationMapC->size() < m_pOptimizationMapH->size())
			m_pOptimizationMap->insert(m_pOptimizationMapC->begin(), m_pOptimizationMapC->end());
		else if (m_pOptimizationMapH->size() == 1 && m_pOptimizationMapH->begin()->first!=m_pOptimizationMapC->begin()->first)
		{
			_COptimizationComponentData& HData = m_pOptimizationMapH->begin()->second;
			size_t HCreatedVertices = HData.GetCreatedVertices();
			size_t HDescs = HData.Get1Descendants();
			_COptimizationComponentData& CData = m_pOptimizationMapC->begin()->second;
			size_t CCreatedVertices = CData.GetCreatedVertices();
			size_t CDescs = CData.Get1Descendants();
			size_t H1stSupremacy = CDescs - HDescs;
			size_t C1stSupremacy = HCreatedVertices - CCreatedVertices;
			_CComponent* pComp = (H1stSupremacy>C1stSupremacy) ? m_pOptimizationMapH->begin()->first : m_pOptimizationMapC->begin()->first;
			m_pFastOptimizer->RemovComp(pComp);
			return pComp;
		}
		else
		{
			m_pOptimizationMap->insert(m_pOptimizationMapH->begin(), m_pOptimizationMapH->end());
			m_pOptimizationMap->insert(m_pOptimizationMapC->begin(), m_pOptimizationMapC->end());
		}
	if (m_pOptimizationMap->size() == 0)
		return NULL;//There is disconnection in the circuit
	if (m_pOptimizationMap->size() > 1)
		FilterTheGreatest(m_pOptimizationMap, _COptimizationComponentData::NodesRemoved);
//Insert the closest prev
	if(m_pOptimizationMap->size()!=1)
	{
		if (m_pOptimizationMap->size() != 1)
		{
			if (m_pOptimizationMap->size() == 2)
				//if (m_pOptimizationMap->size() <= 8)
				{
				_CComponent* pComp = GetHigherOrderOptimization();
				if (pComp != nullptr)
				{
					m_pFastOptimizer->RemovComp(pComp);
					return pComp;
				}
			}

			FilterTheSmallest(m_pOptimizationMap, _COptimizationComponentData::VerticesCreated);
			if (m_pOptimizationMap->size() != 1)
			{
				FilterTheSmallest(m_pOptimizationMap, _COptimizationComponentData::RelativeDescendantsInNextLevel);
				if (m_pOptimizationMap->size() != 1)
				{
					FilterClosed2Prev(m_pOptimizationMap);
					if (m_pOptimizationMap->size() != 1)
						FilterTheSmallest(m_pOptimizationMap, _COptimizationComponentData::LastIncidence);
				}
			}
		}
	}
	if (m_pOptimizationMap->size() > 1)
	{
		fstream file("Comps.txt", ios::out | ios::app);
		file << "Arbitrary chosen from " << m_pOptimizationMap->size() << "  - ";
	}
	OPTIMIZATION_MAP::iterator it = m_pOptimizationMap->begin();
	while (it!=m_pOptimizationMap->end() && it->first->IsMacroDependant())
		it++;
	if (it == m_pOptimizationMap->end())
		it = m_pOptimizationMap->begin();
	_CComponent* pComp =  it->first;
	m_pOptimizationMap->erase(it);
	m_pFastOptimizer->RemovComp(pComp);
	return pComp;
}

_CComponent* _CCircuitAnalyzer::GetHigherOrderOptimization()
{
#ifdef _NON_HIGHER_ORDER
	return m_pOptimizationMap->begin()->first;
#else
	COMP_LIST_DELS CompListDels;
	PrepareCompListDels(*m_pOptimizationMap,CompListDels);
	typedef map<_CComponent*,short> P_ROW;
	typedef map<_CComponent*,P_ROW> P_MATRIX;
	P_MATRIX PMatrix;
	_CComponent *pResComp=NULL;
	for(OPTIMIZATION_MAP::iterator lit=m_pOptimizationMap->begin();
		lit!=m_pOptimizationMap->end();lit++)
	{
		OPTIMIZATION_MAP::iterator rit=lit;rit++;
		OPTIMIZATION_MAP::iterator FirstIt,
								   LastIt;
		_CComponent* pLeft=lit->first;
		for(;rit!=m_pOptimizationMap->end();rit++)
		{
			_CComponent* pRight=rit->first;
			if(!ComponentsOverlaps(CompListDels[pLeft],CompListDels[pRight]))
				continue;
			_C2ndOrderOptimizationComponentData Data2(*this,*pLeft,*pRight);
			int CmpRes=Data2.Test2ndInfluence();
			//int CmpRes=TestOrder(pLeft,pRight);
			if(CmpRes==0)
				continue;
			PMatrix[pLeft][pRight]=CmpRes;
			PMatrix[pRight][pLeft]=-CmpRes;
		}
	}
	if(!PMatrix.empty())
	{
		P_ROW& Row=PMatrix.begin()->second;
		ASSERTPDD(!Row.empty());
		_CComponent *pAuxComp=NULL;
		if(Row.begin()->second<0)
			pResComp=PMatrix.begin()->first;
		else
			pResComp=Row.begin()->first;
		pAuxComp=pResComp;
		while(pAuxComp!=NULL)
		{
			P_ROW& RFirst=PMatrix[pAuxComp];
			pAuxComp=NULL;
			for(P_ROW::iterator pit=RFirst.begin(),p_e=RFirst.end();pit!=p_e;pit++)
				if(pit->second>0)
				{
					pAuxComp=pit->first;
					pResComp=pAuxComp;
					break;
				}
					
		}
	}
	//if (pResComp == NULL)
	//{
	//	OPTIMIZATION_MAP::iterator it = m_pOptimizationMap->begin();
	//	while (it!=m_pOptimizationMap->end() && it->first->IsMacroDependant())
	//		it++;
	//	return it == m_pOptimizationMap->end() ? m_pOptimizationMap->begin()->first : it->first;
	//}
	return pResComp/*!=NULL?pResComp:m_pOptimizationMap->begin()->first*/;
#endif
}

//int _CCircuitAnalyzer::TestOrder(_CComponent* pPred,_CComponent* pSucc)
//{
//	//OPTIMIZATION_MAP::iterator Pred_it=m_pOptimizationMap->find(pPred);
//	//OPTIMIZATION_MAP::iterator Succ_it=m_pOptimizationMap->find(pSucc);
//	//ASSERTPDD(Pred_it!=m_pOptimizationMap->end());
//	//ASSERTPDD(Succ_it!=m_pOptimizationMap->end());
//	//_COptimizationComponentData& PredSuccData=Pred_it->second.GetHigherLevelData(pSucc);
//	//_COptimizationComponentData& SuccPredData=Succ_it->second.GetHigherLevelData(pPred);
//	//return PredSuccData.Comp(SuccPredData);
//	return 0;
//}


//void _CCircuitAnalyzer::PrepareCompListDels(OPTIMIZATION_MAP& OptMap, COMP_LIST_DELS& CompListDels)
//{
//	CompListDels.clear();
//	for (OPTIMIZATION_MAP::iterator om_it = OptMap.begin(), om_e = OptMap.end(); om_it != om_e; om_it++)
//	{
//		_CComponent* Comp = om_it->first;
//		_CComponent::DESC_DELETIONS Deletions;
//		Comp->GetCompleteOfDeletions(Deletions);
//		_COptimizationComponentData& Data = om_it->second;
//		const STATE_COFACTOR_SET* pTopSet = Data.GetMyTopStates();
//		STATE_DEP_LIST_DELS& StateDepListDels = CompListDels[Comp];
//		for (STATE_COFACTOR_SET::const_iterator sit = pTopSet->begin(), _se = pTopSet->end(); sit != _se; sit++)
//		{
//			_CStateCofactor* pState = *sit;
//			PAIR_LIST& PairList = StateDepListDels[pState];
//			MULTI_LIST_DELS& RowDels = PairList.first;
//			MULTI_LIST_DELS& ColDels = PairList.second;
//			for (_CComponent::DESC_DELETIONS::iterator dn_it = Deletions.begin(), dn_e = Deletions.end(); dn_it != dn_e; dn_it++)
//			{
//				//const _CBasicSetOfDeletions* pDels = *dn_it;
//				const _CMultiBasicSetOfDeletions* pDels = *dn_it;
//				//for (_CBasicSetOfDeletions::const_iterator it = pDels->begin(), _e = pDels->end(); it != _e; it++)
//				for (_CMultiBasicSetOfDeletions::const_iterator it = pDels->begin(), _e = pDels->end(); it != _e; it++)
//				{
//					int p = it->m_p, r = it->m_r, k = it->m_k, l = it->m_l;
//					if (pState->Translate(p, r, k, l))
//					{
//						if (iAbs(p)<iAbs(r))
//							swap(p, r);
//						if (p<0)
//						{
//							p *= -1;
//							r *= -1;
//						}
//						if (p == -r)
//							r = 0;
//						RowDels.push_back(DEL(p, r));
//						if (iAbs(k)<iAbs(l))
//							swap(k, l);
//						if (k<0)
//						{
//							k *= -1;
//							l *= -1;
//						}
//						if (k == -l)
//							l = 0;
//						ColDels.push_back(DEL(k, l));
//					}
//				}
//			}
//			ColDels.sort();
//			ColDels.unique();
//			RowDels.sort();
//			RowDels.unique();
//		}
//
//	}
//}

void _CCircuitAnalyzer::PrepareCompListDels(OPTIMIZATION_MAP& OptMap,COMP_LIST_DELS& CompListDels)
{
	CompListDels.clear();
	for(OPTIMIZATION_MAP::iterator om_it=OptMap.begin(),om_e=OptMap.end();om_it!=om_e;om_it++)
	{
		_CComponent* Comp=om_it->first; 
		_CComponent::DESC_DELETIONS Deletions;
		Comp->GetCompleteOfDeletions(Deletions);
		_COptimizationComponentData& Data=om_it->second;
		const STATE_COFACTOR_SET* pTopSet=Data.GetMyTopStates();
		STATE_DEP_LIST_DELS& StateDepListDels=CompListDels[Comp];
		for(STATE_COFACTOR_SET::const_iterator sit=pTopSet->begin(),_se=pTopSet->end();sit!=_se;sit++)
		{
			_CStateCofactor* pState=*sit; 
			PAIR_LIST& PairList=StateDepListDels[pState];
			LIST_DELS& RowDels=PairList.first;
			LIST_DELS& ColDels = PairList.second;
			for (auto* pMultDels : Deletions)
			{
				for (auto& SetOfDels : *pMultDels)
				{
					for (auto& Dels : SetOfDels)
					{
						int p = Dels.m_p, r = Dels.m_r, k = Dels.m_k, l = Dels.m_l;
						MULTI_BASIC_PAIR_OF_DELETIONS ParResults;
						pState->Translate(p, r, k, l, ParResults);
						for (auto& BasicPairOfDels : ParResults)
						{
							p = BasicPairOfDels.m_p;
							r = BasicPairOfDels.m_r;
							k = BasicPairOfDels.m_k;
							l = BasicPairOfDels.m_l;
							if (iAbs(p)<iAbs(r))
								swap(p, r);
							if (p<0)
							{
								p *= -1;
								r *= -1;
							}
							if (p == -r)
								r = 0;
							RowDels.push_back(DEL(p, r));
							if (iAbs(k)<iAbs(l))
								swap(k, l);
							if (k<0)
							{
								k *= -1;
								l *= -1;
							}
							if (k == -l)
								l = 0;
							ColDels.push_back(DEL(k, l));
						}
					}

				}
			}
			ColDels.sort();
			ColDels.unique();
			RowDels.sort();
			RowDels.unique();
		}

	}
}

bool _CCircuitAnalyzer::ComponentsOverlaps(STATE_DEP_LIST_DELS& Left,STATE_DEP_LIST_DELS& Right)
{
	ASSERTPDD(Left.size()==Right.size());
	for(STATE_DEP_LIST_DELS::iterator lit=Left.begin(),rit=Right.begin(),_le=Left.end(),_re=Right.end();
		lit!=_le && rit!=_re;lit++,rit++)
	{
		ASSERTPDD(lit->first==rit->first);
		PAIR_LIST& LList=lit->second;
		PAIR_LIST& RList=rit->second;
		if(has_intersec(LList.first,RList.first) || has_intersec(LList.second,RList.second))
			return true;
	}
	return false;
}

//bool _CCircuitAnalyzer::ComponentsOverlaps(OPTIMIZATION_MAP::iterator it_Left,OPTIMIZATION_MAP::iterator it_Right)
//{
//	_CComponent* pLeft=it_Left->first;
//	_CComponent* pRight=it_Right->first;
//	_CComponent::DESC_DELETIONS LeftD,RightD;
//	pLeft->GetCompleteOfDeletions(LeftD);
//	pRight->GetCompleteOfDeletions(RightD);
//	_COptimizationComponentData& LeftData=it_Left->second;
//	const STATE_COFACTOR_SET* pTopSet=LeftData.GetMyTopState();
//	typedef pair<int,int> DEL;
//	typedef list<DEL> LIST_DELS;
//	LIST_DELS ColDelsLeft,ColDelsRight,RowDelsLeft,RowDelsRight;
//	for(STATE_COFACTOR_SET::const_iterator sit=pTopSet->begin(),_se=pTopSet->end();sit!=_se;sit++)
//	{
//		_CStateCofactor* pState=*sit; 
//		ColDelsLeft.clear(),ColDelsRight.clear(),RowDelsLeft.clear(),RowDelsRight.clear();
//		for(_CComponent::DESC_DELETIONS::iterator dn_it=LeftD.begin(),dn_e=LeftD.end();dn_it!=dn_e;dn_it++)
//		{
//			const _CBasicSetOfDeletions* pDels=*dn_it;
//			for(_CBasicSetOfDeletions::const_iterator it=pDels->begin(),_e=pDels->end();it!=_e;it++)
//			{
//				int p=it->m_p,r=it->m_r,k=it->m_k,l=it->m_l;
//				if(pState->Translate(p,r,k,l))
//				{
//					RowDelsLeft.push_back(DEL(p,r));
//					ColDelsLeft.push_back(DEL(k,l));
//				}
//			}
//		}
//		ColDelsLeft.sort();
//		ColDelsLeft.unique();
//		RowDelsLeft.sort();
//		RowDelsLeft.unique();
//
//		for(_CComponent::DESC_DELETIONS::iterator dn_it=RightD.begin(),dn_e=RightD.end();dn_it!=dn_e;dn_it++)
//		{
//			const _CBasicSetOfDeletions* pDels=*dn_it;
//			for(_CBasicSetOfDeletions::const_iterator it=pDels->begin(),_e=pDels->end();it!=_e;it++)
//			{
//				int p=it->m_p,r=it->m_r,k=it->m_k,l=it->m_l;
//				if(pState->Translate(p,r,k,l))
//				{
//					RowDelsRight.push_back(DEL(p,r));
//					ColDelsRight.push_back(DEL(k,l));
//				}
//			}
//		}
//
//		ColDelsRight.sort();
//		ColDelsRight.unique();
//		RowDelsRight.sort();
//		RowDelsRight.unique();
//		if(has_intersec(RowDelsLeft,RowDelsRight) || has_intersec(ColDelsLeft,ColDelsRight))
//			return true;
//	}
//	return false;
//}

void _CCircuitAnalyzer::FilterTheSmallest(P_OPTIMIZATION_MAP& m_pOptimizationMap,_COptimizationComponentData::VERT_TYPE Type) const
{
	size_t Counter=numeric_limits<size_t>::max();
	for(OPTIMIZATION_MAP::iterator it=m_pOptimizationMap->begin(),_e=m_pOptimizationMap->end();it!=_e;)
	{
		size_t NewValue=it->second.Criterion(Type);
		if(NewValue>Counter)
		{
			it=m_pOptimizationMap->erase(it);
		}
		else
		{
			Counter=NewValue;
			it++;
		}
	}
	for(OPTIMIZATION_MAP::iterator it=m_pOptimizationMap->begin(),_e=m_pOptimizationMap->end();it!=_e;)
	{
		if(it->second.Criterion(Type)>Counter)
			it=m_pOptimizationMap->erase(it);
		else
			it++;
	}
}

void _CCircuitAnalyzer::FilterClosed2Prev(P_OPTIMIZATION_MAP& pOptimizationMap) const
{
	OPTIMIZATION_MAP AuxMap;
	auto r_it = m_AnalyzedComponents.rbegin();
	while (r_it != m_AnalyzedComponents.rend() && AuxMap.empty())
	{
		const _CComponent* pPrevComp = *r_it++;
		COMPONENT_PINS_LIST Pins;
		pPrevComp->RaportUniqConnections(Pins);
		//copy_if(pOptimizationMap->begin,);
		for each (auto OME in *pOptimizationMap)
		{
			const _CComponent* pFutureComp = OME.first;
			COMPONENT_PINS_LIST FuturePins;
			pFutureComp->RaportUniqConnections(FuturePins);
			COMPONENT_PINS ComPins(min(FuturePins.size(),Pins.size()),-1);
			set_intersection(Pins.begin(), Pins.end(), FuturePins.begin(), FuturePins.end(), ComPins.begin());
			if (ComPins.front() != -1)
				AuxMap.insert(OPTIMIZATION_MAP::value_type(OME.first,OME.second));

		}
	}
	if (!AuxMap.empty())
		(*pOptimizationMap) = AuxMap;

}

void _CCircuitAnalyzer::FilterTheGreatest(P_OPTIMIZATION_MAP& m_pOptimizationMap,_COptimizationComponentData::VERT_TYPE Type) const
{
	size_t Counter=0;
	for(OPTIMIZATION_MAP::iterator it=m_pOptimizationMap->begin(),_e=m_pOptimizationMap->end();it!=_e;)
	{
		size_t NewValue=it->second.Criterion(Type);
		if(NewValue<Counter)
		{
			it=m_pOptimizationMap->erase(it);
		}
		else
		{
			Counter=NewValue;
			it++;
		}
	}
	for(OPTIMIZATION_MAP::iterator it=m_pOptimizationMap->begin(),_e=m_pOptimizationMap->end();it!=_e;)
	{
		if(it->second.Criterion(Type)<Counter)
			it=m_pOptimizationMap->erase(it);
		else
			it++;
	}
}

void _CCircuitAnalyzer::PerformAnalysis(bool Try2OptimizeOrder)
{
 	PrepareCofactorsStates();
	while(!m_MainComponentsList.empty())
	{
#ifdef _REORDER
		m_GreadyResAnalyser.GetFixedOrderComponents(m_OrderedComponentsList);
#endif
		_CComponent* pNextComponent=Try2OptimizeOrder?GetNextOptimalComponent():GetNextOptimalComponentFast()/*m_MainComponentsList.front()*/;
		//m_GreadyResAnalyser.PermRemoveComponent(pNextComponent);
		fstream file("Comps.txt", ios::out | ios::app);
		file << "Component " << pNextComponent->GetParameterName()<< " was selected" << endl;
		AddAnotherComponent(*pNextComponent, Try2OptimizeOrder);
		m_AnalyzedComponents.Move2Me(pNextComponent,m_MainComponentsList);
	}
 	FinishAnalysis();
	m_MainComponentsList.Move2Me(m_AnalyzedComponents);
}

void _CCircuitAnalyzer::TouchPin(const _CComponent& Component)
{
	COMPONENT_PINS Pins;
	Component.RaportConnections(Pins);
	m_TouchedPinSet.insert(Pins.begin(),Pins.end());
}
bool _CCircuitAnalyzer::CoincidentWithProcessed(const _CComponent& Component)
{
	COMPONENT_PINS Pins;
	Component.RaportConnections(Pins);
	for(COMPONENT_PINS::iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
		if(IsTouchedPin(*it))
			return true;
	return false;
}

//bool _CCircuitAnalyzer::StoreInComMap(_CComponent* pLeft,_CComponent* pRight,short CompRes)
//{
//	COMP_COORDS CordsStright(pLeft,pRight);
//	COMP_COORDS CordsReversed(pRight,pLeft);
//	if(m_ComMap.find(CordsStright)!=m_ComMap.end()||
//		m_ComMap.find(CordsReversed)!=m_ComMap.end())
//		return false;
//	m_ComMap.insert(COMP_MAP::value_type(CordsStright,CompRes));
//	return true;
//}
//short _CCircuitAnalyzer::GetFromComMap(_CComponent* pLeft,_CComponent* pRight)
//{
//	COMP_MAP::iterator it;
//	if((it=m_ComMap.find(COMP_COORDS(pLeft,pRight)))!=m_ComMap.end())
//		return it->second;
//	if((it=m_ComMap.find(COMP_COORDS(pRight,pLeft)))!=m_ComMap.end())
//		return -it->second;
//	return 0;//unresolved
//}

//*******************************_CSubciruitOutsData****************************
void _CCircuitAnalyzer::_CSubciruitOutsData::PrepareMe()
{
	m_OriginalDescStates.resize(m_Analyzer.m_pCurrentStateCofactors->size(),_CStateCofactor(&m_Analyzer));
	size_t Index=0;
	for(STATE_COFACTOR_SET::iterator it=m_Analyzer.m_pCurrentStateCofactors->begin(),
		_e=m_Analyzer.m_pCurrentStateCofactors->end();it!=_e;it++,Index++)
	{
		_CStateCofactor* pOrgCof=*it;
		m_OriginalDescStates[Index]=*pOrgCof;
		pair<TEMP_VERTS_2_REDEFINE::iterator,TEMP_VERTS_2_REDEFINE::iterator>
			Range=m_Analyzer.m_pCurrentTempVerts2Redefine->equal_range(pOrgCof);
		size_t& Counter=m_Counter[Index];
		for(TEMP_VERTS_2_REDEFINE::iterator it2=Range.first;it2!=Range.second;it2++)
		{
			m_Vertex2OriginalDescIndex.insert(VERTEX_2_ORIGINAL_DESC_INDEX::value_type(
				(const _CTempVertex**) &(it2->second->first),Index));
			Counter++;
		}
	}
}
size_t _CCircuitAnalyzer::_CSubciruitOutsData::DongleIndex(const _CTempVertex** ppDongleAddres) const
{
	VERTEX_2_ORIGINAL_DESC_INDEX::const_iterator fit=m_Vertex2OriginalDescIndex.find(ppDongleAddres);
	if(fit==m_Vertex2OriginalDescIndex.end())
		return (size_t)(-1);
	else
		return fit->second;
}

void _CCircuitAnalyzer::_CSubciruitOutsData::
		RegisterTempVertex2ModelVertex(_CTempVertex* const * ppTempVertex,_CModelVertex** ppModelVertex)
{
	
	VERTEX_2_ORIGINAL_DESC_INDEX::iterator it=m_Vertex2OriginalDescIndex.find((const _CTempVertex**)ppTempVertex);
	ASSERTPDD(it!=m_Vertex2OriginalDescIndex.end());
	_CStateCofactor& EqState=m_OriginalDescStates[it->second];
	m_BoundariesMap.insert(BOUNDARIES_MAP::value_type(&EqState,ppModelVertex));
}

void _CCircuitAnalyzer::_CSubciruitOutsData::PrepareDongle(size_t DongleNo,
	_CMultiDeletions& Deletions, _CModelVertex& ModelVertex, unsigned long& Counter)
{
	ASSERTPDD(DongleNo<m_OriginalDescStates.size());
	_CStateCofactor& State=m_OriginalDescStates[DongleNo];
	State.ReverseCofactor(Deletions);
	ModelVertex.ForceHash((long)State.GetHash());
	pair<BOUNDARIES_MAP::iterator,BOUNDARIES_MAP::iterator> Range=m_BoundariesMap.equal_range(&State);
	for(BOUNDARIES_MAP::iterator it=Range.first;it!=Range.second;it++)
	{
		(*it->second)=&ModelVertex;
		ModelVertex.TouchMe();
	}
	Counter=m_Counter[DongleNo];
}

//*******************************_CSubciruitOutsData****************************

//*******************************_COptimizationComponentData********************

_CCircuitAnalyzer::_COptimizationComponentData::_COptimizationComponentData(const _COptimizationComponentData& Source):
	//_CMultLevelInterface(Source),
	m_Analyzer(Source.m_Analyzer),
	m_MyComponent(Source.m_MyComponent),
	m_HollowedVertices(Source.m_HollowedVertices),
	m_CreatedVertices(Source.m_CreatedVertices),
	m_DescendantsInNextLevel(Source.m_DescendantsInNextLevel),
	m_GlobalPossibleDescendants(Source.m_GlobalPossibleDescendants),
	m_1Descendants(Source.m_1Descendants),
	m_pNextLevelOptimizationMap(Source.m_pNextLevelOptimizationMap==NULL?NULL:new OPTIMIZATION_MAP(*Source.m_pNextLevelOptimizationMap)),
	m_pMyTopStates(Source.m_pMyTopStates),
	m_MinIncidenceAfterReduction(Source.m_MinIncidenceAfterReduction),
	m_NoOfPinsWithMinIncidence(Source.m_NoOfPinsWithMinIncidence),
	m_StateCache(Source.m_StateCache),
	m_MyProductStates(Source.m_MyProductStates),
	m_NoOfRemovedPins(Source.m_NoOfRemovedPins)
{
	//for(STATE_COFACTOR_SET::const_iterator it=Source.m_MyProductStates.begin(),
	//	_e=Source.m_MyProductStates.end();it!=_e;it++)
	//	m_MyStates.push_back(**it);
	//for(H_LEVEL_STATES::iterator it=m_MyStates.begin(),_e=m_MyStates.end();it!=_e;it++)
	//	m_MyProductStates.insert(&*it);
}

void _CCircuitAnalyzer::_COptimizationComponentData::TestInfluence()
{
	//const COMPONENT_PINS_LIST* pPinsList=NULL;
	_CTestIncidentsWraper TestTable(m_Analyzer.m_IncidentTable);
	COMPONENT_PINS_LIST PinsList;
	TestTable.TestRemovalCompPins(m_MyComponent,PinsList);
	m_NoOfRemovedPins = PinsList.size();
	//m_Analyzer.m_IncidentTable.TestRemovalCompPins(m_MyComponent,PinsList);
	//PrevTestRemovalCompPins(pPinsList);
	//m_Analyzer.m_IncidentTable.TestRemovalCompPins(m_MyComponent,pPinsList);
	m_Analyzer.ReportMinIncidences(m_MyComponent,m_MinIncidenceAfterReduction,m_NoOfPinsWithMinIncidence);
	size_t NoOfDesc=m_MyComponent.NoOfEffevtiveDescendants();
	int HollowDesc=m_MyComponent.HollowOutput(); 
	IGNORE_COMP_SET IgnoreSet;
	IgnoreSet.insert(&m_MyComponent);
	//PrepareIgnoreCompSet(IgnoreSet);
	typedef vector<const _CMultiBasicSetOfDeletions*> DESC_SETS_OFDELETIONS;
	DESC_SETS_OFDELETIONS DescSetsOfDeletions(NoOfDesc);
	for(size_t i=0;i<NoOfDesc;i++)
	{
		m_MyComponent.GetDescendantDeletions(i,DescSetsOfDeletions[i]);
	}
	//STATE_COFACTOR_SET* pTopCofactorSet
	//m_pMyTopState=GetPrevLevelInterface()->GetMyCofState(); 
	m_pMyTopStates=m_Analyzer.GetBaseStates(); 
	for(STATE_COFACTOR_SET::iterator it=m_pMyTopStates->begin(),_e=m_pMyTopStates->end();
		it!=_e;it++)
	{
		bool IsHollow=true;
		bool IsNonZero=false;
		for(size_t i=0;i<NoOfDesc;i++)
		{
			_CStateCofactor* pNewState=new _CStateCofactor(**it);
			m_GlobalPossibleDescendants++;
			try
			{
				//_CCommonStateCofactor::AD_DELETION Res=pNewState->AddDeletions(*DescSetsOfDeletions[i],*pPinsList);
				AD_DELETION Res=pNewState->AddDeletions(*DescSetsOfDeletions[i],PinsList);
				if(Res==AD_OK)
				{
					//if(!pPinsList->empty())
						//Res=pNewState->ReduceSeparateNodes(*pPinsList);
					//if(!PinsList.empty())
					//	Res=pNewState->ReduceSeparateNodes(PinsList);
					if(Res==AD_OK &&
						!m_Analyzer.CanBeDifferentThenZero(*pNewState,IgnoreSet,true))
					Res=AD_DISCONNECTION;
				}
				switch(Res)
				{
				case AD_COMPLETE:
					m_1Descendants++;
				case AD_OK:
					{
						short Sgn=1;
						IsNonZero=true;
						if(i!=HollowDesc)
							IsHollow=false;
						//if(!pPinsList->empty())
						//	pNewState->ReduceSeparateNodes(*pPinsList);
						_CStateCofactor* pCurState=
							/*m_Analyzer.*/m_StateCache.RegisterUnique(pNewState,Sgn);
						if(m_MyProductStates.insert(pCurState).second)
							m_DescendantsInNextLevel++;
						break;
					}
				default:
					delete pNewState;
				}
			}
			catch(...)
			{
				if(!pNewState->IsRegistered())
					delete pNewState;
				throw;
			}
		}
		if(IsNonZero)
			if(IsHollow)
				m_HollowedVertices++;
			else
				m_CreatedVertices++;
	}
	//m_Analyzer.m_IncidentTable.RemoveTest();
	//PrevTestRemoveTest();
	//m_Analyzer.m_IncidentTable.RemoveTest();
}

int _CCircuitAnalyzer::_COptimizationComponentData::Comp(const _COptimizationComponentData& Right) const
{
	int Res=GetCreatedVertices()-Right.GetCreatedVertices();
	if(Res!=0)
		return sgn(Res);
	Res=GetRelativeDescendantsInNextLevel()-Right.GetRelativeDescendantsInNextLevel();
	if(Res!=0)
		return sgn(Res);
	return 0;
}

//_CCircuitAnalyzer::_COptimizationComponentData& _CCircuitAnalyzer::_COptimizationComponentData::GetHigherLevelData(_CComponent* pComopnent)
//{
//	//if(m_pNextLevelOptimizationMap==NULL)
//	//	m_pNextLevelOptimizationMap=new OPTIMIZATION_MAP;
//	//else
//	//{
//	//	OPTIMIZATION_MAP::iterator it=m_pNextLevelOptimizationMap->find(pComopnent);
//	//	if(it!=m_pNextLevelOptimizationMap->end())
//	//		return it->second;
//	//}
//	//_CStateCofactorCache LocalCache;
//	//_COptimizationComponentData& Data=
//	//	m_pNextLevelOptimizationMap->insert(OPTIMIZATION_MAP::value_type(pComopnent,
//	//	_COptimizationComponentData(m_Analyzer,*pComopnent,m_Level,this,LocalCache))).first->second;
//	//Data.TestInfluence();
//	//return Data;
//}


//*******************************_COptimizationComponentData********************
//*******************************_C2ndOrderOptimizationComponentData************
int _CCircuitAnalyzer::_C2ndOrderOptimizationComponentData::Test2ndInfluence()
{
	m_CreatedVertices1_2=m_CreatedVertices2_1=0;
	typedef vector<bool> RES_VECTOR;
	typedef vector<RES_VECTOR> RES_MATRIX;
	size_t NoOfDesc1=m_My1stComponent.NoOfEffevtiveDescendants();
	int HollowDesc1=m_My1stComponent.HollowOutput(); 
	size_t NoOfDesc2=m_My2nComponent.NoOfEffevtiveDescendants();
	int HollowDesc2=m_My2nComponent.HollowOutput(); 
	//RES_MATRIX ResMatrix12(NoOfDesc1,RES_VECTOR(NoOfDesc2));

	typedef vector<const _CMultiBasicSetOfDeletions*> DESC_SETS_OFDELETIONS;
	DESC_SETS_OFDELETIONS DescSetsOfDeletions1(NoOfDesc1);
	for(size_t i=0;i<NoOfDesc1;i++)
	{
		m_My1stComponent.GetDescendantDeletions(i,DescSetsOfDeletions1[i]);
	}

	DESC_SETS_OFDELETIONS DescSetsOfDeletions2(NoOfDesc2);
	for(size_t i=0;i<NoOfDesc2;i++)
	{
		m_My2nComponent.GetDescendantDeletions(i,DescSetsOfDeletions2[i]);
	}
	m_pMyTopStates=m_Analyzer.GetBaseStates(); 

	IGNORE_COMP_SET IgnoreSet12;
	IgnoreSet12.insert(&m_My1stComponent);
	IgnoreSet12.insert(&m_My2nComponent);

	//const COMPONENT_PINS_LIST* pPinsList1=NULL,*pPinsList2=NULL;
	//m_Analyzer.m_IncidentTable.TestRemovalCompPins(m_My1stComponent,pPinsList1);
	//m_Analyzer.m_IncidentTable.TestRemovalCompPins(m_My1stComponent,pPinsList1);
	//m_Analyzer.m_IncidentTable.TestRemovalCompPins(m_My2nComponent,pPinsList2);
	//COMPONENT_PINS_LIST CommonPinsList(*pPinsList1);
	//CommonPinsList.sort();
	//COMPONENT_PINS_LIST AuxList(*pPinsList2);
	//AuxList.sort();
	//CommonPinsList.merge(AuxList);

	//COMPONENT_PINS_LIST CommonPinsList1;
	COMPONENT_PINS_LIST CommonPinsList12;
	_CTestIncidentsWraper TestTable(m_Analyzer.m_IncidentTable);
	//TestTable.TestRemovalCompPins(m_My1stComponent, CommonPinsList1);
	TestTable.TestRemovalCompPins(m_My1stComponent, m_My2nComponent, CommonPinsList12);
	//m_Analyzer.m_IncidentTable.TestRemovalCompPins(m_My1stComponent,m_My2nComponent,CommonPinsList);
	//STATE_COFACTOR_SET MyProductStates;
	//_CStateCofactorCache StateCache;


	for(STATE_COFACTOR_SET::iterator it=m_pMyTopStates->begin(),_e=m_pMyTopStates->end();
		it!=_e;it++)
	{
		RES_MATRIX ResMatrix12(NoOfDesc1, RES_VECTOR(NoOfDesc2));
		//bool IsHollow1 = true;
		bool IsNonZero1 = false;
		for (size_t l = 0; l<NoOfDesc1; l++)
		{
			//bool IsHollow2=true;
			bool IsNonZero2=false;
			for(size_t r=0;r<NoOfDesc2;r++)
			{
				_CStateCofactor NewState(**it);
				//NewState.AddDeletions(*DescSetsOfDeletions1[l]);
				AD_DELETION Res = NewState.AddDeletions(*DescSetsOfDeletions1[l]);
				if (Res == AD_LOOP || Res == AD_DISCONNECTION)
					break;
				else
					Res=NewState.AddDeletions(*DescSetsOfDeletions2[r],CommonPinsList12);
				//AD_DELETION Res = NewState.AddDeletions(*DescSetsOfDeletions2[r], CommonPinsList12);
				if (Res ==AD_OK)
				{
					//if(!CommonPinsList12.empty())
					//	Res=NewState.ReduceSeparateNodes(CommonPinsList12);
					if(Res==AD_OK &&
						!m_Analyzer.CanBeDifferentThenZero(NewState,IgnoreSet12,true))
					Res=AD_DISCONNECTION;
				}
				bool bRes=(Res==AD_OK || Res==AD_ENOUGH ||
					Res==AD_COMPLETE);
				ResMatrix12[l][r]=bRes;
				if(bRes)
				{
					if (r != HollowDesc2)
						IsNonZero2 = true;
					if (l != HollowDesc1)
						IsNonZero1 = true;
					//if(r!=HollowDesc2)
						//IsHollow2=false;
				}
			}
			if (IsNonZero2 /*&& !IsHollow2*/)
			{
				m_CreatedVertices1_2++;
				//if (l != HollowDesc1)
					//IsNonZero1 = true;
				//if (l != HollowDesc1)
				//	IsHollow1 = false;
			}
		}
		if (IsNonZero1 /*&& !IsHollow1*/)
			m_CreatedVertices1_2++;
		//IsHollow1 = true;
		IsNonZero1 = false;
		for (size_t r = 0; r<NoOfDesc2; r++)
		{
			//bool IsHollow=true;
			bool IsNonZero2=false;
			for(size_t l=0;l<NoOfDesc1;l++)
			{
				if(ResMatrix12[l][r])
				{
					if (l != HollowDesc1)
						IsNonZero2 = true;
					if (r != HollowDesc2)
						IsNonZero1 = true;
					//if(l!=HollowDesc1)
						//IsHollow=false;
				}
			}
			if (IsNonZero2 /*&& !IsHollow*/)
			{
				m_CreatedVertices2_1++;
				//if (r != HollowDesc1)
				//	IsNonZero1 = true;
				//if (r != HollowDesc1)
					//IsHollow1 = false;
			}
		}
		if (IsNonZero1 /*&& !IsHollow1*/)
			m_CreatedVertices2_1++;
	}
	//m_Analyzer.m_IncidentTable.RemoveTest(2);

	return CompResults();
}

int _CCircuitAnalyzer::_C2ndOrderOptimizationComponentData::CompResults()
{
	if(m_CreatedVertices1_2==m_CreatedVertices2_1) return 0;
	if(m_CreatedVertices1_2>m_CreatedVertices2_1) return 1;
	return -1;
}

//*******************************_C2ndOrderOptimizationComponentData************
//*******************************_CUniqueCacheContainer*************************
unsigned long long _CCircuitAnalyzer::_CUniqueCacheContainer::GetHash(unsigned long long Core)
{
	Core*=HASH_FACTOR;
	Core^=(unsigned long long) m_pComponent;
	for(DESC_COFACTORS_V::const_iterator it=m_DescDesc.begin(),_e=m_DescDesc.end();it!=_e;it++)
	{
		const DESC_COFACTORS& Desc=*it;
		Core*=HASH_FACTOR;
		Core^=(unsigned long long) Desc.first;
		Core*=HASH_FACTOR;
		Core^=Desc.second;
	}
	return Core;
}
//*******************************_CUniqueCacheContainer*************************

//*******************************_CCircuitAnalyzer******************************

//*******************************_CMultLevelInterface***************************

_CMultLevelInterface* _CMultLevelInterface::GetLevelInterface(size_t Level)
{
	ASSERTPDD(Level<=m_Level);
	if(Level==m_Level)
		return this;
	else
		return m_pUpperInterface->GetLevelInterface(Level);
}

//*******************************_CMultLevelInterface***************************

//*******************************_CFuzzyOrderedCompList*************************
//bool _CFuzzyOrderedCompList::InertInOrder(_CComponent* pPredComp,_CComponent* pSuccComp)
//{
//	AUX_MAP::iterator itPred=m_AuxMap.find(pPredComp);
//	AUX_MAP::iterator itSucc=m_AuxMap.find(pSuccComp);
//	SEGMENT_AUX_DATA* pDataPred=itPred==m_AuxMap.end()?NULL:&itPred->second;
//	SEGMENT_AUX_DATA* pDataSucc=itSucc==m_AuxMap.end()?NULL:&itSucc->second;
//	if(pDataPred==NULL)
//	{
//		if(pDataSucc==NULL)
//		{
//			m_ListOfLists.push_back(SEGMENT());
//			SEGMENT& SG=m_ListOfLists.back();
//			SG.push_back(pPredComp);
//			SG.push_back(pSuccComp);
//		}
//		else
//		{
//		}
//	}
//	else
//	{
//		if(pDataSucc==NULL)
//		{
//		}
//		else
//		{
//		}
//	}
//	return true;
//}
//*******************************_CFuzzyOrderedCompList*************************

//*******************************_CNewGreedyStateCofactor***********************

//bool _CNewGreedyStateCofactor::TestDeletionPair(int& P, int& R, int& K, int& L) const
//{
//	bool Res = m_State==AD_COMPLETE || m_R.FastLoopTest(P,R) || m_C.FastLoopTest(K,L);
//	if(!Res && P==-R) R=m_R.m_CurrZero;
//	if(!Res && K==-L) L=m_C.m_CurrZero;
//	return Res;
//}
//
//AD_DELETION _CNewGreedyStateCofactor::AddDeletionPair(int P, int R, int K, int L, 
//	const COMPONENT_PINS_LIST& NewSeparateNodes)
//{
//	//if(m_State==AD_COMPLETE || m_R.FastLoopTest(P,R) || m_C.FastLoopTest(K,L))
//	if(TestDeletionPair(P,R,K,L))
//		return (m_State=AD_LOOP);
//	//if(ord_list_remove(m_MNR,P))
//	//	m_JAR.push_back(P);
//	//int aR=iAbs(R);
//	//if(ord_list_remove(m_MNR,aR))
//	//	m_JAR.push_back(aR);
//
//	//if(ord_list_remove(m_MNC,K))
//	//	m_JAC.push_back(K);
//	//int aL=iAbs(L);
//	//if(ord_list_remove(m_MNC,aL))
//	//	m_JAC.push_back(aL);
//	//m_JAR.sort();
//	//m_JAC.sort();
//
//	//m_TNR.merge(m_JAR);
//	//m_TNC.merge(m_JAC);
//	NODES_SET JustRemovedNodes;
//	m_R.AddTestContract(P,R);
//	m_C.AddTestContract(K,L);
//	m_R._Flatten();
//	m_C._Flatten();
//	bool JustDeleyed=false;
//	for(COMPONENT_PINS_LIST::const_iterator cit=NewSeparateNodes.begin(),_ce=NewSeparateNodes.end();
//		cit!=_ce;cit++)
//	{
//		if(m_HighestOrder==0)
//		{
//			ASSERTPDD(m_R.empty() && m_C.empty());
//			return (m_State=AD_ENOUGH);
//		}
//		int Node=*cit;
//		CAN_REM RCan=m_R.CanRemove(Node);
//		CAN_REM CCan=m_C.CanRemove(Node);
//		if(RCan==_CSubNodeContractionsSimpl::DISCONNECT || CCan==_CSubNodeContractionsSimpl::DISCONNECT)
//			return (m_State=AD_DISCONNECTION);
//		if(RCan==_CSubNodeContractions::DELAY || CCan==_CSubNodeContractions::DELAY)
//		{
//			m_R.DelayRemove();
//			m_C.DelayRemove();
//			m_DelayedZero=Node;
//			JustDeleyed=true;
//			continue;
//		}
//		if(RCan==_CSubNodeContractionsSimpl::CAN && CCan==_CSubNodeContractionsSimpl::CAN)
//		{
//			_NODE_RES RR=m_R._IgnoreNode(Node);
//			_NODE_RES CR=m_C._IgnoreNode(Node);
//			JustRemovedNodes.push_back(Node);
//			m_HighestOrder--;
//		}
//	}
//	if(!JustDeleyed && m_DelayedZero>=0)
//	{
//		CAN_REM RCan=m_R.CanRemove(m_DelayedZero);
//		CAN_REM CCan=m_C.CanRemove(m_DelayedZero);
//		if(RCan==_CSubNodeContractionsSimpl::DISCONNECT || CCan==_CSubNodeContractionsSimpl::DISCONNECT)
//			return (m_State=AD_DISCONNECTION);
//		if(RCan==_CSubNodeContractionsSimpl::CAN && CCan==_CSubNodeContractionsSimpl::CAN)
//		{
//			_NODE_RES RR=m_R._IgnoreNode(m_DelayedZero);
//			_NODE_RES CR=m_C._IgnoreNode(m_DelayedZero);
//			JustRemovedNodes.push_back(m_DelayedZero);
//			m_DelayedZero=-1;
//			m_HighestOrder--;
//		}
//	}
//#ifdef _INCREMENT_TOP
//
//	if(!JustRemovedNodes.empty())
//	{
//		JustRemovedNodes.sort();
//		m_ErasedNodes.merge(JustRemovedNodes);
//	}
//#endif
//
//	if(IsEnoughTest())
//		return (m_State=AD_ENOUGH);
//	return (m_State=AD_OK);
//}

AD_DELETION _CNewGreedyStateCofactor::AddDeletions(const _CMultiBasicSetOfDeletions& Deletions,
	const COMPONENT_PINS_LIST& NewSeparateNodes)
{
	m_Contractions.AddDeletions(Deletions, NewSeparateNodes);

#ifdef _INCREMENT_TOP
	if (!NewSeparateNodes.empty())
		{
			NODES_SET JustRemovedNodes;
			JustRemovedNodes.assign(NewSeparateNodes.begin(),NewSeparateNodes.end());
			JustRemovedNodes.sort();
			m_ErasedNodes.merge(JustRemovedNodes);
		}
#endif
		if (IsEnoughTest())
			return (m_Contractions.m_State = AD_ENOUGH);
	return m_Contractions.m_State;
}

//AD_DELETION _CNewGreedyStateCofactor::AddDeletions(const _CBasicSetOfDeletions& Deletions,
//	const COMPONENT_PINS_LIST& NewSeparateNodes)
//{
//	//m_JAR.clear();
//	//m_JAC.clear();
//
//	if (!Deletions.empty())
//	{
//		_CBasicSetOfDeletions::const_iterator it = Deletions.begin(), _le = (--Deletions.end());
//		for (; it != _le; it++)
//		{
//			const _CBasicPairOfDeletion& Del = (*it);
//			AddDeletionPair(Del.m_p, Del.m_r, Del.m_k, Del.m_l/*,EmptyPinList*/);
//			if (m_State == AD_LOOP || m_State == AD_DISCONNECTION)
//				return m_State;
//		}
//		const _CBasicPairOfDeletion& Del = (*it);
//		AddDeletionPair(Del.m_p, Del.m_r, Del.m_k, Del.m_l, NewSeparateNodes);
//	}
//	else
//	{
//		NODES_SET JustRemovedNodes;
//		for (COMPONENT_PINS_LIST::const_iterator cit = NewSeparateNodes.begin(), _ce = NewSeparateNodes.end();
//			cit != _ce; cit++)
//		{
//			if (m_HighestOrder == 0)
//			{
//				ASSERTPDD(m_R.empty() && m_C.empty());
//				return (m_State = AD_ENOUGH);
//			}
//			int Node = *cit;
//			CAN_REM RCan = m_R.CanRemove(Node);
//			if (RCan == _CSubNodeContractionsSimpl::DISCONNECT)
//				return (m_State = AD_DISCONNECTION);
//			CAN_REM CCan = m_C.CanRemove(Node);
//			if (CCan == _CSubNodeContractionsSimpl::DISCONNECT)
//				return (m_State = AD_DISCONNECTION);
//			if (RCan == _CSubNodeContractionsSimpl::CAN && CCan == _CSubNodeContractionsSimpl::CAN)
//			{
//				_NODE_RES RR = m_R._IgnoreNode(Node);
//				_NODE_RES CR = m_C._IgnoreNode(Node);
//				JustRemovedNodes.push_back(Node);
//				m_HighestOrder--;
//			}
//		}
//#ifdef _INCREMENT_TOP
//		if (!JustRemovedNodes.empty())
//		{
//			JustRemovedNodes.sort();
//			m_ErasedNodes.merge(JustRemovedNodes);
//		}
//#endif
//		if (IsEnoughTest())
//			return (m_State = AD_ENOUGH);
//	}
//	return m_State;
//}
//
//AD_DELETION _CNewGreedyStateCofactor::ReduceSeparateNodes(const COMPONENT_PINS_LIST& SeparateNodes)
//{
//	for(COMPONENT_PINS_LIST::const_iterator it=SeparateNodes.begin(),_e=SeparateNodes.end();it!=_e;it++)
//	{
//		if(m_HighestOrder==0)
//		{
//			ASSERTPDD(m_R.empty() && m_C.empty());
//			return m_State;
//		}
//		bool ReCheckDelayed=m_DelayedZero>=0;
//		ReduceSeparateNode(*it);
//		if(ReCheckDelayed)
//			ReduceSeparateNode(m_DelayedZero);
//	}
//	return m_State;
//}

//AD_DELETION _CNewGreedyStateCofactor::ReduceSeparateNode(int Node)
//{
//	CAN_REM RCan=m_R.CanRemove(Node);
//	if(RCan==_CSubNodeContractions::DISCONNECT)
//	{
//		return (m_State=AD_DISCONNECTION);
//	}
//	CAN_REM CCan=m_C.CanRemove(Node);
//	if(CCan==_CSubNodeContractions::DISCONNECT)
//	{
//		return (m_State=AD_DISCONNECTION);
//	}
//	if(RCan==_CSubNodeContractions::DELAY || CCan==_CSubNodeContractions::DELAY)
//	{
//		m_R.DelayRemove();
//		m_C.DelayRemove();
//		m_DelayedZero=Node;
//		return m_State;
//	}
//	else if(Node==m_DelayedZero)
//		m_DelayedZero=-1;
//	_NODE_RES RR=m_R._IgnoreNode(Node);
//	_NODE_RES CR=m_C._IgnoreNode(Node);
//	m_HighestOrder--;
//	return m_State;
//}

unsigned long long _CNewGreedyStateCofactor::GetHash(unsigned long long Core)
{
	Core=_CCommonStateCofactor::GetHash(Core);
	Core*=HASH_FACTOR;
	Core^=(unsigned long long )m_ZeroWasRemoved;
	Core*=HASH_FACTOR;
	Core^=(unsigned long long )m_DelayedZero;
	return Core;
}
#ifdef _INCREMENT_TOP
void _CNewGreedyStateCofactor::SetTouched(INT_LIST& MNR, CONTR_LIST& MRCTRL) const
{
	list_differ(MNR,m_ErasedNodes);
	m_Contractions.SetTouched(MNR, MRCTRL);
	//m_R.SetTouched(MNR,MRCTRL);
}
#endif
//*******************************_CNewGreedyStateCofactor***********************

//*************************_CNewGreedyAnalyser*****************************************

//*******************************_CAdvancedComOnPath**************************
//_CNewGreedyAnalyser::_CAdvancedComOnPath::_CAdvancedComOnPath(_CComponents& InitComponentList,_CCircuitAnalyzer*& pAnalyser) : m_pAnalyzer(pAnalyser)
//{
//	for(_CComponents::iterator it=InitComponentList.begin(),_e=InitComponentList.end();
//		it!=_e;it++)
//			AddComponent(*it,false);
//#ifndef _ALLOW_REDUNDANCY
//#ifndef _INCREMENT_TOP
//	PrepareSortedList();
//#endif
//#endif
//}

//_CNewGreedyAnalyser::_CAdvancedComOnPath::_CAdvancedComOnPath(const COMP_ON_A_PATH& Source,_CCircuitAnalyzer*& pAnalyser) : m_pAnalyzer(pAnalyser)
//{
//	for(COMP_ON_A_PATH::const_iterator it=Source.begin(),_e=Source.end();
//		it!=_e;it++)
//			AddComponent(*it,false);
//#ifndef _ALLOW_REDUNDANCY
//#ifndef _INCREMENT_TOP
//	PrepareSortedList();
//#endif
//#endif
//}

#ifndef _ALLOW_REDUNDANCY
#ifndef _INCREMENT_TOP
void _CNewGreedyAnalyser::_CAdvancedComOnPath::PrepareSortedList()
{
	m_List.clear();
	INCIDENCE_LIST AuxList;
	for(PIN_COMP_MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
	{
		it->second.second=m_List.insert(m_List.end(),INCIDENCE(it->first,it->second.first.size()));
	}
	m_List.sort(_sort());
}
#endif
#endif

#ifdef _INCREMENT_TOP
void _CNewGreedyAnalyser::_CAdvancedComOnPath::AddComponent(_CComponent* pComponent,bool Reorder)
{
	if(pComponent->DeletionsAreForced())
		m_ForcedDeletionsComps.insert(pComponent);
	//INCIDENCE_LIST AuxList(m_List._Alval);
	COMPONENT_PINS_LIST PinsList;
	pComponent->RaportUniqConnections(PinsList);
	for(COMPONENT_PINS_LIST::iterator nit=PinsList.begin(),_ne=PinsList.end();nit!=_ne;nit++)
	{
		size_t Pin=*nit;
		PIN_COMP_SET& Set=m_Map[Pin];
		Set.insert(pComponent);
		//PIN_COMP_PAIR& Pair=m_Map[Pin];
		//Pair.first.insert(pComponent);
		//if(!Reorder) continue;
		//INCIDENCE_LIST::iterator lit=Pair.second;
		//lit->second++;
		//if(!IsInList(AuxList,*Pair.second))
		//AuxList._Splice(AuxList.begin(),m_List,Pair.second,++lit,1,true);
	}
	//if(Reorder)
	//{
	//	AuxList.sort(_sort());
	//	m_List.merge(AuxList,_sort());
	//}
}

void _CNewGreedyAnalyser::_CAdvancedComOnPath::_RemoveComponent(_CComponent* pComponent,COMPONENT_PINS_LIST& RemovedPins,bool PermanentRemove,bool Reorder)
{
	if(pComponent->DeletionsAreForced())
		m_ForcedDeletionsComps.erase(pComponent);
	//INCIDENCE_LIST AuxList(m_List._Alval);
	RemovedPins.clear();			
	COMPONENT_PINS_LIST PinsList;
	pComponent->RaportUniqConnections(PinsList);
	for(COMPONENT_PINS_LIST::iterator nit=PinsList.begin(),_ne=PinsList.end();nit!=_ne;nit++)
	{
		size_t Pin=*nit;
		PIN_COMP_SET& Set=m_Map[Pin];
		//PIN_COMP_PAIR& Pair=m_Map[Pin];
		//PIN_COMP_SET& Set=Pair.first;
		Set.erase(pComponent);
		//INCIDENCE_LIST::iterator lit=Pair.second;
 	//	lit->second--;
		if(Set.empty())
		{
			if(!m_pAnalyzer->IsBoundary(Pin))
				RemovedPins.push_back(Pin);
			if(PermanentRemove)
			{
				m_Map.erase(Pin);
				//ASSERTPDD(lit->second==0);
				//m_List.erase(lit);
			}
			//else if(Reorder/* & !IsInList(AuxList,*Pair.second)*/)
			//		AuxList._Splice(AuxList.begin(),m_List,Pair.second,++lit,1,true);
		}
		//else
		//	if(Reorder/* && !IsInList(AuxList,*Pair.second)*/)
		//		AuxList._Splice(AuxList.begin(),m_List,Pair.second,++lit,1,true);
		//		//m_List.splice(--AuxList.end(),AuxList,Pair.second,++lit);
	}
	//if(Reorder)
	//{
	//	AuxList.sort(_sort());
	//	m_List.merge(AuxList,_sort());
	//}
}
#else
#ifdef _ALLOW_REDUNDANCY
//void _CNewGreedyAnalyser::_CAdvancedComOnPath::AddComponent(_CComponent* pComponent,bool Reorder)
//{
//	if(pComponent->DeletionsAreForced())
//		m_ForcedDeletionsComps.insert(pComponent);
//}
//
//void _CNewGreedyAnalyser::_CAdvancedComOnPath::_RemoveComponent(_CComponent* pComponent,COMPONENT_PINS_LIST& RemovedPins,bool PermanentRemove,bool Reorder)
//{
//	if(pComponent->DeletionsAreForced())
//		m_ForcedDeletionsComps.erase(pComponent);
//}
#else
void _CNewGreedyAnalyser::_CAdvancedComOnPath::AddComponent(_CComponent* pComponent,bool Reorder)
{
	if(pComponent->DeletionsAreForced())
		m_ForcedDeletionsComps.insert(pComponent);
	INCIDENCE_LIST AuxList(m_List._Alval);
	COMPONENT_PINS_LIST PinsList;
	pComponent->RaportUniqConnections(PinsList);
	for(COMPONENT_PINS_LIST::iterator nit=PinsList.begin(),_ne=PinsList.end();nit!=_ne;nit++)
	{
		size_t Pin=*nit;
		PIN_COMP_PAIR& Pair=m_Map[Pin];
		Pair.first.insert(pComponent);
		if(!Reorder) continue;
		INCIDENCE_LIST::iterator lit=Pair.second;
		lit->second++;
		if(!IsInList(AuxList,*Pair.second))
		AuxList._Splice(AuxList.begin(),m_List,Pair.second,++lit,1,true);
	}
	if(Reorder)
	{
		AuxList.sort(_sort());
		m_List.merge(AuxList,_sort());
	}
}

void _CNewGreedyAnalyser::_CAdvancedComOnPath::_RemoveComponent(_CComponent* pComponent,COMPONENT_PINS_LIST& RemovedPins,bool PermanentRemove,bool Reorder)
{
	if(pComponent->DeletionsAreForced())
		m_ForcedDeletionsComps.erase(pComponent);
	INCIDENCE_LIST AuxList(m_List._Alval);
	RemovedPins.clear();			
	COMPONENT_PINS_LIST PinsList;
	pComponent->RaportUniqConnections(PinsList);
	for(COMPONENT_PINS_LIST::iterator nit=PinsList.begin(),_ne=PinsList.end();nit!=_ne;nit++)
	{
		size_t Pin=*nit;
		PIN_COMP_PAIR& Pair=m_Map[Pin];
		PIN_COMP_SET& Set=Pair.first;
		Set.erase(pComponent);
		INCIDENCE_LIST::iterator lit=Pair.second;
 		lit->second--;
		if(Set.empty())
		{
			if(!m_pAnalyzer->IsBoundary(Pin))
				RemovedPins.push_back(Pin);
			if(PermanentRemove)
			{
				m_Map.erase(Pin);
				ASSERTPDD(lit->second==0);
				m_List.erase(lit);
			}
			else if(Reorder/* & !IsInList(AuxList,*Pair.second)*/)
					AuxList._Splice(AuxList.begin(),m_List,Pair.second,++lit,1,true);
		}
		else
			if(Reorder/* && !IsInList(AuxList,*Pair.second)*/)
				AuxList._Splice(AuxList.begin(),m_List,Pair.second,++lit,1,true);
				//m_List.splice(--AuxList.end(),AuxList,Pair.second,++lit);
	}
	if(Reorder)
	{
		AuxList.sort(_sort());
		m_List.merge(AuxList,_sort());
	}
}
#endif
#endif 
#ifdef  _ALLOW_REDUNDANCY
//_CComponent* _CNewGreedyAnalyser::_CAdvancedComOnPath::GetTopComponent(const _CNewGreedyStateCofactor& CurrCof) 
//{
//	if(!m_ForcedDeletionsComps.empty())
//		return *m_ForcedDeletionsComps.begin();
//	else
//		return NULL;
//}

#else
_CComponent* _CNewGreedyAnalyser::_CAdvancedComOnPath::GetTopComponent(const _CNewGreedyStateCofactor& CurrCof) 
{
	if(!m_ForcedDeletionsComps.empty())
		return *m_ForcedDeletionsComps.begin();
#ifdef _INCREMENT_TOP
	NODES_SET MNR(CurrCof.m_OriginalNodes);
	CONTR_LIST MRCTRS;
	CurrCof.SetTouched(MNR,MRCTRS);

	for(NODES_SET::const_iterator it=MNR.begin(),_e=MNR.end();it!=_e;it++)
	{
		int PrefNode1=*it;
		INT_LIST& ContrList=MRCTRS[PrefNode1];
		if(ContrList.empty())
			ContrList.push_back(PrefNode1);
		for(INT_LIST::iterator lit=ContrList.begin(),_le=ContrList.end();lit!=_le;lit++)
		{
			int PrefNode=*lit;
			PIN_COMP_SET* pSet=&m_Map[PrefNode];
			while(pSet->empty() && ++it!=_e)
				//pSet=&m_Map[(PrefNode=*it)].first;
				pSet=&m_Map[(PrefNode=*it)];
			if(it==_e)
				break;
			PIN_COMP_SET::iterator sit=pSet->begin(),_se=pSet->end();
			for(;sit!=_se;sit++)
			{
				_CComponent* pPrefComp=*sit;
				const _CBasicSetOfDeletions* pDels;
				pPrefComp->GetDescendantGreedyDeletion(0,pDels);
				ASSERTPDD(pDels!=NULL);
				if(pDels->size()>1)
					return pPrefComp;
				const _CBasicPairOfDeletion& Pair=pDels->front();
				int P=Pair.m_p;
				int R=Pair.m_r;
				int K=Pair.m_k;
				int L=Pair.m_l;
				if(!CurrCof.TestDeletionPair(P,R,K,L))
					return pPrefComp;
				}
		}
	}
	return NULL;
#else
	INCIDENCE& Incid=m_List.front();
	if(Incid.second==0)
		return NULL;
	return *m_Map[Incid.first].first.begin();
#endif
}
#endif

#ifndef  _ALLOW_REDUNDANCY
void _CNewGreedyAnalyser::_CAdvancedComOnPath::SetSet(SET_OF_COMPONENTS& SetOfComponents) const
{
	SetOfComponents.clear();
	for(PIN_COMP_MAP::const_iterator mit=m_Map.begin(),_me=m_Map.end();mit!=_me;mit++)
	{
#ifdef _INCREMENT_TOP
		const PIN_COMP_SET& Set=mit->second;
#else
		const PIN_COMP_SET& Set=mit->second.first;
#endif
		SetOfComponents.insert(Set.begin(),Set.end());
	}
}
#endif

#ifdef _INCREMENT_TOP
void _CNewGreedyAnalyser::_CAdvancedComOnPath::PrepareNodesInPorgres(INT_LIST& NodesList)
{
	NodesList.clear();
	for(PIN_COMP_MAP::const_iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
		if(!it->second.empty())
			NodesList.push_back(it->first);
	NodesList.sort();
}
#endif
//void _CNewGreedyAnalyser::_CAdvancedComOnPath::GetFixedOrderComponents(COMPONENTS_LIST& List)
//{
//	List.clear();
//	_CComponent* pComponent=GetTopComponent();
//	while(pComponent!=NULL)
//	{
//		COMPONENT_PINS_LIST Pins;
//		_RemoveComponent(pComponent,Pins);
//		List.push_back(COMP_DATA(pComponent,Pins));
//		pComponent=GetTopComponent();
//	}
//	for(COMPONENTS_LIST::iterator it=List.begin(),_e=List.end();it!=_e;it++)
//		AddComponent(it->first);
//}

//*******************************_CAdvancedComOnPath**************************
//void _CNewGreedyAnalyser::TempRemoveComponent(_CComponent* pComponent,COMPONENT_PINS_LIST& RemovedPins)
//{
//	m_AdvancedComOnPath._RemoveComponent(pComponent,RemovedPins);
//}

//void _CNewGreedyAnalyser::PermRemoveComponent(_CComponent* pComponent)
//{
//	COMPONENT_PINS_LIST List;
//	m_AdvancedComOnPath._RemoveComponent(pComponent,List,true);
//}

//bool _CNewGreedyAnalyser::CanBeComplete(const _CNewGreedyStateCofactor& TestedState)
//{
//#ifdef  _ALLOW_REDUNDANCY
//	if(m_AdvancedComOnPath.ForcedDeletionEmpty())
//		return true;
//#endif
//	bool* pOldResult=NULL;
//	if(m_Cache.GetResultIfExists(TestedState,pOldResult))
//	{
//		return (m_CanBeComplate=*pOldResult);
//	}
//	_CComponent* pNextComp=m_AdvancedComOnPath.GetTopComponent(TestedState);
//	if(pNextComp==NULL)
//	{
//		return (m_CanBeComplate=*pOldResult=TestedState.IsEnoughTest());
//	}
//	size_t NoOfDesc=pNextComp->GreedyNoOfDescendants();
//	COMPONENT_PINS_LIST RemovedPins;
//	TempRemoveComponent(pNextComp,RemovedPins);
//	bool Result=false;
//	for(size_t i=0;i<NoOfDesc;i++)
//	{
//		const _CBasicSetOfDeletions* pDels;
//		pNextComp->GetDescendantGreedyDeletion(i,pDels);
//		ASSERTPDD(pDels!=NULL);
//		if((TestedState.GetNoOfDeletions()+pDels->size())>TestedState.HighestOrder())
//			continue;
//		_CNewGreedyStateCofactor NewState(TestedState);
//		NewState.AddDeletions(*pDels,RemovedPins);
//		if(NewState.IsEnough() && !m_AdvancedComOnPath.ThereAreForcedDeletions())
//			{
//				Result=true;
//				break;
//			}
//		if(NewState.IsInProgres() || NewState.IsEnough())
//			if(Result=CanBeComplete(NewState))
//				break;
//	}
//#ifndef _ALLOW_REDUNDANCY
//	if(!Result)
//#endif
//		m_AdvancedComOnPath.AddComponent(pNextComp);
//	return (m_CanBeComplate=*pOldResult=Result);
//}

//void _CNewGreedyAnalyser::TempRemoveComponent(const SET_OF_COMPONENTS& CompSet)
//{
//	if(CompSet.empty()) return;
//	m_RemComps=CompSet;
//	for(SET_OF_COMPONENTS::iterator it=m_RemComps.begin(),_e=m_RemComps.end();it!=_e;it++)
//		m_AdvancedComOnPath._RemoveComponent(*it,COMPONENT_PINS_LIST());
//}
//
//void _CNewGreedyAnalyser::RetryTempRemovedComponents()
//{
//	if(m_RemComps.empty()) return;
//	for(SET_OF_COMPONENTS::iterator it=m_RemComps.begin(),_e=m_RemComps.end();it!=_e;it++)
//		m_AdvancedComOnPath.AddComponent(*it);
//	m_RemComps.clear();
//}

//unsigned long long _CNewGreedyAnalyser::_CCacheContainer::GetHash(unsigned long long Core)
//{
//	Core=m_Cof.GetHash(Core);
//	for(SET_OF_COMPONENTS::iterator it=m_SetOfComp.begin(),_e=m_SetOfComp.end();it!=_e;it++)
//	{
//		Core*=HASH_CORE;
//		Core^=(unsigned long long)(*it);
//	}
//	return Core;
//}
//_CNewGreedyAnalyser::_CCacheContainer::_CCacheContainer(const _CNewGreedyStateCofactor& State,
//														const _CAdvancedComOnPath& ComPath):
//m_Cof(State),m_IsFeaseable(false)
//{
//	//ComPath.SetSet(m_SetOfComp);
//}
//
//bool _CNewGreedyAnalyser::_CCache::GetResultIfExists(const _CNewGreedyStateCofactor& Cofactor,bool*& Result)
//{
//	_CCacheContainer* pNewCotainer=new _CCacheContainer(Cofactor,m_AdvancedComOnPath);
//	unsigned long long Hash=pNewCotainer->GetHash();
//	pair<_CCache::iterator,_CCache::iterator> Range=equal_range(Hash);
//	for(_CCache::iterator it=Range.first;it!=Range.second;it++)
//		if(*pNewCotainer==*it->second)
//		{
//			delete pNewCotainer;
//			Result=&it->second->m_IsFeaseable;
//			return true;
//		}
//	Result=&insert(value_type(Hash,pNewCotainer))->second->m_IsFeaseable;
//	return false;
//}
//_CNewGreedyAnalyser::_CCache::~_CCache()
//{
//	for(iterator it=begin(),_e=end();it!=_e;it++)
//		delete it->second;
//}

//*************************_CNewGreedyAnalyser*****************************************
//*************************_CFastGreedyAnalyser********************************************

bool _CFastGreedyAnalyser::CanBeComplete(const _CNewGreedyStateCofactor& TestedState, SET_OF_COMPONENTS& IgnoredComponents)
{
	//if (m_OrgComponents.empty())
	//	return true;
	LIST_OF_COMPONENTS ObligatoryComps, ArbitraryComps;
	//_CTestIncidentsWRAPER IncdWrapper(m_pAnlyser->m_IncidentTable);
	//COMPONENT_PINS_LIST NewSep;
	//IncdWrapper.TestRemovalCompPins(IgnoredComponents, NewSep);
	for_each(m_OrgComponents.begin(), m_OrgComponents.end(), [&](_CComponent* pComp)
	{
		if (IsIn(IgnoredComponents, pComp)) return;
		if (pComp->DeletionsAreForced())
			ObligatoryComps.push_back(pComp);
		else
			//if (IncdWrapper.TestCompProduceSeparatedPins(*pComp))
			if (m_pAnlyser->m_IncidentTable.TestCompProduceSeparatedPins(*pComp))
				ObligatoryComps.push_back(pComp);
			else
				ArbitraryComps.push_back(pComp);
	}
	);
	if (!ObligatoryComps.empty())
	{
		for (auto pComp : ObligatoryComps)
			if (IsComponentContradicted(TestedState, pComp/*,IncdWrapper*/))
				return false;
		return true;
	}
	if (ArbitraryComps.empty())
		return true;
	for (auto pComp : ArbitraryComps)
		if (!IsComponentContradicted(TestedState, pComp/*,IncdWrapper*/))
			return true;
	return false;
}

bool _CFastGreedyAnalyser::IsComponentContradicted(const _CNewGreedyStateCofactor& TestedState, _CComponent* pComponent/*, _CTestIncidentsWRAPER TopIncd*/)
{
	bool Result=true;
	ASSERTPDD(m_pAnlyser!=NULL);
	//const COMPONENT_PINS_LIST* pPins;
	//m_pAnlyser->m_IncidentTable.TestRemovalCompPins(*pComponent,pPins);
	COMPONENT_PINS_LIST Pins;
	_CTestIncidentsWraper TestTable(m_pAnlyser->m_IncidentTable);
	//TopIncd.TestRemovalCompPins(*pComponent, Pins);
	TestTable.TestRemovalCompPins(*pComponent, Pins);
	//m_pAnlyser->m_IncidentTable.TestRemovalCompPins(*pComponent,Pins);
	size_t NoOfDesc=pComponent->NoOfEffevtiveDescendants();
	//if(!pComponent->DeletionsAreForced() && pPins->empty())
	if(!pComponent->DeletionsAreForced() && Pins.empty())
		Result=false;
	else
		for(size_t i=0;i<NoOfDesc;i++)
		{
			const _CMultiBasicSetOfDeletions* pDels=NULL;
			pComponent->GetDescendantDeletions(i,pDels);
			_CNewGreedyStateCofactor NewState(TestedState);
			//_CCommonStateCofactor::AD_DELETION Res=NewState.AddDeletions(*pDels,*pPins);
			AD_DELETION Res=NewState.AddDeletions(*pDels,Pins);
			if(Res==AD_LOOP || Res==AD_DISCONNECTION)
				continue;
			//if(!pPins->empty())
			//	Res=NewState.ReduceSeparateNodes(*pPins);
			//if(Res==_CCommonStateCofactor::AD_OK || Res==_CCommonStateCofactor::AD_ENOUGH || 
			//	Res==_CCommonStateCofactor::AD_COMPLETE)
			//{
				Result=false;
				break;
			//}
		}
		//m_pAnlyser->m_IncidentTable.RemoveTest();
	return Result;
}

//*************************_CFastGreedyAnalyser********************************************
//*************************_CFastCompOptimizer*****************************************
_CCircuitAnalyzer::_CFastCompOptimizer::_CFastCompOptimizer(_CCircuitAnalyzer* pAnalyzer, _CComponents& MainList, COMPONENT_PINS_SET& TouchedPinSet) :
/*m_pCurrentCompSet(NULL), */m_pAnalyzer(pAnalyzer), m_TouchedPinSet(TouchedPinSet), m_TheMostCommon(0)
{
	size_t TheMostCommonValue = 0;
	for(_CComponents::iterator it=MainList.begin(),_e=MainList.end();it!=_e;it++)
	{
		_CComponent* pComp=*it;
		COMPONENT_PINS_LIST Pins;
		pComp->RaportUniqConnections(Pins);
		Pins.sort();
		Pins.unique();
		for (COMPONENT_PINS_LIST::iterator pit = Pins.begin(), _pe = Pins.end(); pit != _pe; pit++)
		{
			size_t Pin = *pit;
			auto& Set = m_Map[Pin];
			Set.insert(pComp);
			size_t Size = Set.size();
			if (Size > TheMostCommonValue)
			{
				TheMostCommonValue = Size;
				m_TheMostCommon = Pin;
			}
		}
	}
	for (auto& it = m_Map.begin(), _e = m_Map.end(); it != _e; it++)
		m_SortedCompSet.push_back(&*it);
	SortCompSet();
}

void _CCircuitAnalyzer::_CFastCompOptimizer::SortCompSet()
{
	m_SortedCompSet.sort([&](INCD_MAP::value_type* pLeft, INCD_MAP::value_type* pRight)
	{
		size_t LSize = pLeft->second.size();
		size_t RSize = pRight->second.size();
		if (RSize == 0 && LSize > 0)
			return true;
		if (LSize == 0 && RSize > 0)
			return false;
		if (LSize != RSize)
			return LSize < RSize;
		bool LBoundary = m_pAnalyzer->IsBoundary(pLeft->first);
		bool RBoundary = m_pAnalyzer->IsBoundary(pRight->first);
		if (LBoundary == RBoundary)
			return false;
		return LBoundary;
	});
}

//void _CCircuitAnalyzer::_CFastCompOptimizer::PrepareMin()
//{
//	size_t MinIncidences=numeric_limits<size_t>::max(),
//		MinIncidencesBounary=numeric_limits<size_t>::max();
//	SET_OF_COMPONENTS* pBoundaryCurrentCompSet=NULL;
//	m_pCurrentCompSet=NULL;
//	for(INCD_MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
//	{
//		size_t Pin=it->first;
//		SET_OF_COMPONENTS& Set=it->second;
//		size_t Size=Set.size();
//		if(Size!=0 && Size<MinIncidences)
//		{
//			if(m_pAnalyzer->IsBoundary(Pin))
//			{
//				MinIncidencesBounary=Size;
//				pBoundaryCurrentCompSet=&Set;
//			}
//			else
//			{
//				MinIncidences=Size;
//				m_pCurrentCompSet=&Set;
//			}
//		}
//	}
//	if(m_pCurrentCompSet==NULL)
//		m_pCurrentCompSet=pBoundaryCurrentCompSet;
//}

//_CComponent* _CCircuitAnalyzer::_CFastCompOptimizer::GetNextComponent()//NULL - no components, extracts component from optimizer
//{
//	if(m_pCurrentCompSet==NULL || m_pCurrentCompSet->empty())
//		PrepareMin();
//	if(m_pCurrentCompSet==NULL || m_pCurrentCompSet->empty())
//		return NULL;
//	else
//	{
//		_CComponent* pComp=*m_pCurrentCompSet->begin();
//		if (!m_pAnalyzer->m_MainComponentsList.IsTheMostImportant(pComp))
//		{
//			for (auto pCompAux:*m_pCurrentCompSet)
//				if (m_pAnalyzer->m_MainComponentsList.IsTheMostImportant(pComp))
//				{
//					pComp = pCompAux;
//					break;
//				}
//		}
//		RemovComp(pComp);
//		return pComp;
//	}
//}

_CComponent* _CCircuitAnalyzer::_CFastCompOptimizer::GetNextComponent()//NULL - no components, extracts component from optimizer
{
	ASSERTPDD(!m_SortedCompSet.empty());
	auto sit = m_SortedCompSet.begin();
	SET_OF_COMPONENTS* pSet = &(*sit)->second;
	if (pSet->empty())
		return nullptr;
	auto it = pSet->begin();
	_CComponent* pComp = *it;
	while (!m_pAnalyzer->m_MainComponentsList.IsTheMostImportant(pComp))
	{
		_CComponent* pResComp = nullptr;
		for (auto pCompAux : *pSet)
			if (m_pAnalyzer->m_MainComponentsList.IsTheMostImportant(pCompAux))
			{
				pResComp = pCompAux;
				break;
			}
		if (pResComp != nullptr)
		{
			pComp = pResComp;
			break;
		}
		if (++sit == m_SortedCompSet.end())
			return nullptr;
		pSet = &(*sit)->second;
		ASSERTPDD(!pSet->empty());
		it = pSet->begin();
		pComp = *it;
	}
	RemovComp(pComp);
	return pComp;
}

void _CCircuitAnalyzer::_CFastCompOptimizer::RemovComp(_CComponent* pComp)
{
	pComp->RaportUniqConnections(m_PrevPins);
	m_PrevPins.sort();
	m_PrevPins.unique();
	for(COMPONENT_PINS_LIST::iterator pit=m_PrevPins.begin(),_pe=m_PrevPins.end();pit!=_pe;pit++)
		m_Map[*pit].erase(pComp);
	SortCompSet();
}

void _CCircuitAnalyzer::_CFastCompOptimizer::GetTouchedAndLast(LIST_OF_COMPONENTS& Components)
{
	SET_OF_COMPONENTS AuxSet;
	for (auto& VT : m_Map)
	{
		size_t Pin = VT.first;
		auto& Set = VT.second;
		if ((Pin!=m_TheMostCommon && IsIn(m_TouchedPinSet, Pin)) || Set.size() == 1)
		{
			AuxSet.insert(Set.begin(),Set.end());
		}
	}
	Components.assign(AuxSet.begin(), AuxSet.end());
}

void _CCircuitAnalyzer::_CFastCompOptimizer::GetSmallestIncds(LIST_OF_COMPONENTS& Components)
{
	//INCD_MAP SmallestMap;
	//GetSmallestIncds(SmallestMap);
	SET_OF_COMPONENTS BestSet,NonTouchedSet,BoundarySet/*,ZeroSet*/;
	//for(INCD_MAP::iterator it=SmallestMap.begin(),_e=SmallestMap.end();it!=_e;it++)
	for(INCD_MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
	{
		size_t Pin=it->first;
		SET_OF_COMPONENTS& Set=it->second;
		bool IsBoundary=m_pAnalyzer->IsBoundary(Pin);
		bool IsTouched=m_pAnalyzer->IsTouchedPin(Pin);
		//if(Pin==0)
		//	ZeroSet.insert(Set.begin(),Set.end());
		//else 
			if(IsBoundary)
				BoundarySet.insert(Set.begin(),Set.end());
			else
				if(IsTouched)
					BestSet.insert(Set.begin(),Set.end());
				else
					NonTouchedSet.insert(Set.begin(),Set.end());
	}
	if(!BestSet.empty())
		Components.assign(BestSet.begin(),BestSet.end());
	else if(!NonTouchedSet.empty())
		Components.assign(NonTouchedSet.begin(),NonTouchedSet.end());
	else if(!BoundarySet.empty())
		Components.assign(BoundarySet.begin(),BoundarySet.end());
	//else if(!ZeroSet.empty())
	//	Components.assign(ZeroSet.begin(),ZeroSet.end());
}

void _CCircuitAnalyzer::_CFastCompOptimizer::GetSmallestIncds(INCD_MAP& IncdMap)
{
	size_t MinSize=numeric_limits<size_t>::max();
	for(INCD_MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
	{
		size_t CurSize=it->second.size();
		//bool IsPrev=IsInList(m_PrevPins,it->first);
		if(CurSize==0) continue;
		if(/*!IsPrev && */CurSize>MinSize) continue;
		if(CurSize<MinSize) 
		{
			IncdMap.clear();
			MinSize=CurSize;
		}
		IncdMap.insert(*it);
	}
	for(COMPONENT_PINS_LIST::iterator pit=m_PrevPins.begin(),_pe=m_PrevPins.end();pit!=_pe;pit++)
	{
		size_t Pin=*pit;
		INCD_MAP::iterator oit=IncdMap.find(Pin);
		if(oit!=IncdMap.end()) continue;
		oit=m_Map.find(Pin);
		if(oit->first==0) continue;
		IncdMap.insert(*oit);
	}
}

//*************************_CFastCompOptimizer*****************************************
