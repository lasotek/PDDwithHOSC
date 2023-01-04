#pragma once
#include "Components.h"
#include "InUseCheckpoint.h"
#include "CacheTmpl.h"
#include "Cofactor.h"
#include "Components.h"
#include "binary_filer.h"
#include <unordered_map>
#include <memory>

inline unsigned iAbs(int i) {return (unsigned)(i<0?-i:i);}
inline short iSgn(int i) {return i<0?-1:1;}

//#define _INCREMENT_TOP

extern COMPONENT_PINS_LIST EmptyPinList;
class _CCircuitAnalyzer;

typedef set<_CComponent*> SET_OF_COMPONENTS;
typedef SET_OF_COMPONENTS IGNORE_COMP_SET;

typedef SIGNED_PINS_COLOCATIONS NODES_CONNECTION;
typedef COMPONENT_PINS_LIST DONGLE_NODES;


class _CGlobalIncidenceTable
{
public:
	_CGlobalIncidenceTable()/*:m_IsInTest(false)*/ {}
	void SetSize(size_t size) {m_MainVector.resize(size,0);}
	void AddPins(const COMPONENT_PINS_SET& Pins,bool TouchOnly);
	void AddPins(const COMPONENT_PINS& Pins,bool TouchOnly);
	void AddCompPins(const _CComponent& Component,bool TouchOnly);
	void AddCompPins(const _CComponents& Components,bool TouchOnly,const COMPONENT_PINS_LIST** ppNewSeparateNodes=NULL);

	void RemoveCompPins(const _CComponent& Component, const COMPONENT_PINS_LIST*& pNewSeparateNodes);
	void CorrectCompPins(COMPONENT_PINS_LIST& SeparateNodes);
	//void TestRemovalCompPins(const _CComponent& Component, const COMPONENT_PINS_LIST*& pNewSeparateNodes);
	//void TestRemovalCompPins(const _CComponent& Component, COMPONENT_PINS_LIST& NewSeparateNodes);
	//void TestRemovalCompPins(const _CComponent& Component1, const _CComponent& Component2, COMPONENT_PINS_LIST& NewSeparateNodes);
	//void ResetTest();
	void RaportAllSeparateNodes(const COMPONENT_PINS_LIST*& pNewSeparateNodes);
	//bool IsInTestMode()const {return m_IsInTest;}
	//bool RemoveTest(size_t Counter=1);
	void Resize(size_t size) {m_MainVector.resize(size,0);}
	//const COMPONENT_PINS_LIST* CurrentTestNewSeparatePinsConst() const
	//{
	//	return /*m_IsInTest?&m_TestNewSeparatePins.back():*/&m_MainNewSeparatePins;
	//}
	size_t GetCurrentIncidences(size_t Node) const;
	void Clear()
	{
		m_MainVector.clear();
		m_MainNewSeparatePins.clear();
		//m_IsInTest=false;
		//m_TestVectors.clear();
		//m_TestNewSeparatePins.clear();
		//m_ComponentsInTest.clear();
	}
	unsigned int GetIncidentComponents(size_t Pin) 
	{
		COMPONENT_PINS* pPins=CurrentCompPins();
		ASSERTPDD(Pin<pPins->size());
		return (*pPins)[Pin];
	}
	bool TestCompProduceSeparatedPins(const _CComponent& Component);
protected:
	friend class _CTestIncidentsWraper;
	friend class _CTestIncidentsWRAPER;
	COMPONENT_PINS_LIST* CurrentNewSeparatePins()
	{
		return /*m_IsInTest?&m_TestNewSeparatePins.back():*/&m_MainNewSeparatePins;
	}
	const COMPONENT_PINS* CurrentCompPins() const
	{
		//return /*m_IsInTest?&m_TestVectors.back():*/&m_MainVector;
		return /*m_IsInTest?&m_TestVector:*/&m_MainVector;
	}
	COMPONENT_PINS* CurrentCompPins()
	{
		//return /*m_IsInTest?&m_TestVectors.back():*/&m_MainVector;
		return /*m_IsInTest?&m_TestVector:*/&m_MainVector;
	}
	void RegisterPin(unsigned Pin);
	void UnregisterPin(unsigned Pin);
	COMPONENT_PINS m_MainVector;
	COMPONENT_PINS_LIST m_MainNewSeparatePins;
	//bool m_IsInTest;
	COMPONENT_PINS m_TestVector;
	//typedef list<COMPONENT_PINS> TEST_VECTORS;
	//TEST_VECTORS m_TestVectors;
	//typedef list<COMPONENT_PINS_LIST> TEST_NEW_SEPARATE_PINS;
	//TEST_NEW_SEPARATE_PINS m_TestNewSeparatePins;
	//typedef list<const _CComponent*> COMPONENTS_IN_TEST;
	//COMPONENTS_IN_TEST m_ComponentsInTest;
	//list<unsigned short> m_NoOfElelements;
	//typedef list<_CTestIncidentsWraper*> WRAPERS;
	//WRAPERS m_Wrapers;
	//void RegisterWraper(_CTestIncidentsWraper* pWraper)
	//{
	//	m_Wrapers.push_back(pWraper);
	//}
	//void UnRegisterWraper(_CTestIncidentsWraper* pWraper)
	//{
	//	ASSERTPDD(pWraper==m_Wrapers.back());
	//	m_Wrapers.pop_back();
	//}
	//void PreparPins(COMPONENT_PINS& Pins);
};

class _CTestIncidentsWraper
{
public:
	_CTestIncidentsWraper(_CGlobalIncidenceTable& GlobalTable);
	_CTestIncidentsWraper(const _CTestIncidentsWraper& TopTmble) = delete;
	~_CTestIncidentsWraper();
	void TestRemovalCompPins(const _CComponent& Component, COMPONENT_PINS_LIST& NewSeparateNodes);
	void TestRemovalCompPins(const _CComponent& Component1, const _CComponent& Component2, COMPONENT_PINS_LIST& NewSeparateNodes);
	//void TestRemovalCompPinsFast(const _CComponent& Component, COMPONENT_PINS_LIST& NewSeparateNodes) const;
	//void TestRemovalCompPinsFast(const _CComponent& Component1, const _CComponent& Component2, COMPONENT_PINS_LIST& NewSeparateNodes) const;
protected:
	friend class _CGlobalIncidenceTable;
	_CGlobalIncidenceTable& m_GlobalTable;
	//COMPONENT_PINS m_LocalPins;
	COMPONENT_PINS m_1CompPins;
	COMPONENT_PINS m_2CompPins;
	COMPONENT_PINS& m_TestVector;

};

//class _CTestIncidentsWRAPER
//{
//public:
//	_CTestIncidentsWRAPER(_CGlobalIncidenceTable& GlobalTable);
//	_CTestIncidentsWRAPER(const _CTestIncidentsWRAPER& TopTable);
//	void TestRemovalCompPins(const _CComponent& Component, COMPONENT_PINS_LIST& NewSeparateNodes);
//	void TestRemovalCompPins(const SET_OF_COMPONENTS& Components, COMPONENT_PINS_LIST& NewSeparateNodes);
//	void TestRemovalCompPins(const _CComponent& Component1, const _CComponent& Component2, COMPONENT_PINS_LIST& NewSeparateNodes);
//	bool TestCompProduceSeparatedPins(const _CComponent& Component);
//protected:
//	friend class _CGlobalIncidenceTable;
//	COMPONENT_PINS m_TestVector;
//};

template<typename VALUE_TYPE>
bool IsOdd(VALUE_TYPE Value)
{
	return (Value & (VALUE_TYPE)1)==(VALUE_TYPE)1;
}

template<typename VALUE_TYPE>
bool IsEven(VALUE_TYPE Value)
{
	return (Value & (VALUE_TYPE)1)==0;
}

class _CSubNodeContractionsSimpl : public _streamable
{
public:
	_CSubNodeContractionsSimpl():m_CurrZero(0)/*,m_DelayedZero(-1)*/,m_Pos2Rem(-1),m_HasNegativeDels(false) {}
	void Clear()
	{
		m_CurrZero = 0;
		m_Pos2Rem = -1;
		m_HasNegativeDels = false;
		m_List.clear();
		m_ModifyList.clear();
		m_AuxTestList.clear();
	}
	_CSubNodeContractionsSimpl(const _CSubNodeContractionsSimpl& Source):
	m_CurrZero(Source.m_CurrZero),m_HasNegativeDels(Source.m_HasNegativeDels),
		m_List(Source.m_List)/*,m_DelayedZero(Source.m_DelayedZero)*/ {}
	typedef pair<int,int> CONTRACTION;
	typedef list<CONTRACTION> CONTRACTION_LIST;
	typedef CONTRACTION_LIST::iterator _iterator;
	typedef CONTRACTION_LIST::const_iterator _const_iterator;
	int GetContractedNodeFast(int Node) const;
	bool ContainNode(int Node);
	bool ContainNodes(const DONGLE_NODES& Nodes);
	const CONTRACTION_LIST& GetNodesList() const {return m_List;}
	typedef list<int> POS_LIST;
	short TranslateMe(const NODES_CONNECTION& NodesConnections,POS_LIST& PosList/*,short& Sgn*/);
	unsigned long long GetHash(unsigned long long Core=HASH_CORE);
	bool IsEqual(const _CSubNodeContractionsSimpl& Other) const;  
	int GetContractedNodeAndRebuild(int& Node);
	typedef pair<short/*sgn change*/,int/* Shift */> _NODE_RES;
	bool empty() const {return m_List.empty();}
	inline int NegNode(int Node) {return iAbs(Node)==m_CurrZero?m_CurrZero:-Node;}
	size_t NoOfDel() const {return m_List.size();}
	typedef map<int,INT_LIST> CONTR_LIST; 
	void SetTouched(INT_LIST& ExpectedList, CONTR_LIST& ContrList) const;
	bool GlueItIfCanBeGluedTogether(const _CSubNodeContractionsSimpl& Another,short Sgn, short& OutSgn);
	int GetCurrZero() const { return m_CurrZero; }
	virtual bool IsBoundaryNode(unsigned long Node) { return false; }
	_NODE_RES Contract(int ANode, int BNode);
	bool FastLoopTest(int& ANode, int& BNode) const;
	bool operator==(const _CSubNodeContractionsSimpl& Other) const { return IsEqual(Other); }
	virtual short CompOrderWith(const _CSubNodeContractionsSimpl& Right) const;
	bool CurrZeroDontCare() { return !m_HasNegativeDels; }
	int CurrentZero() const { return m_CurrZero; }
protected:
	inline int _neg_pin(int pin) { return pin == m_CurrZero ? pin : -pin; }
	//typedef map<size_t /*Pos*/, CONTRACTION_LIST::iterator> DIFFERS_RES_MAP;
	typedef pair<size_t /*Pos*/, CONTRACTION_LIST::iterator> DIFFERS_RES;
	//SL 2nd comparer
	typedef enum{ EQUAL, TWO_DEFFERENTS, TOO_MUCH_DIFFERENT } COMP_RESULT;
	COMP_RESULT Compare2Compress(_CSubNodeContractionsSimpl& Other, DIFFERS_RES& MyRes, DIFFERS_RES& OtherRes);
	//SL 2nd Comparer
	class ContrCompartator {
	public:
		//typedef pair<CONTRACTION_LIST::iterator/*Original*/, CONTRACTION_LIST/*Prefered to chaneg*/> EXCHANGE_PAIR;
		typedef pair<CONTRACTION_LIST::iterator/*Original*/, int/*Prefered target to change*/> EXCHANGE_PAIR;
		typedef list<EXCHANGE_PAIR> EXCHANGE_PAIRS_LIST;
		ContrCompartator(_CSubNodeContractionsSimpl& me);
		COMP_RESULT CompareWith(_CSubNodeContractionsSimpl& other);
		COMP_RESULT result() const;
		//const EXCHANGE_PAIRS_LIST* pExchanges() const { return (m_Result != TWO_DEFFERENTS)? nullptr : &m_ExchangePairList; }
		const DIFFERS_RES* pMyyPairToCompres() const { return m_pMineDif; }
		const DIFFERS_RES* pThierPairToCompres() const { return m_pTheirDif; }
		void clear() {
			m_MineList.clear();
			m_Other_List.clear();
			m_Result = TOO_MUCH_DIFFERENT;
			m_ExchangePairList.clear();
			m_pMineDif = m_pTheirDif = nullptr;
		}
		bool needsModification() { return !m_ExchangePairList.empty(); }
		void UpdateModyfication();
	protected:
		_CSubNodeContractionsSimpl& m_me;
		list<DIFFERS_RES> m_MineList, m_Other_List;
		void InitList(_CSubNodeContractionsSimpl& other);
		COMP_RESULT m_Result;
		//unique_ptr<DIFFERS_RES> m_pMineDif, m_pTheirDif;

		DIFFERS_RES *m_pMineDif, *m_pTheirDif;
		EXCHANGE_PAIRS_LIST m_ExchangePairList;
	};
	bool CompressIt(COMP_RESULT Operation, _CSubNodeContractionsSimpl& Other, short& MySgn, short& OtherSgn, 
		DIFFERS_RES& MyRes, DIFFERS_RES& OtherRes);
	bool CompressIt2(COMP_RESULT Operation, _CSubNodeContractionsSimpl& Other, short& MySgn, short& OtherSgn,
		ContrCompartator& CompResults, bool testOnly = false);
	bool Split1IfNeeded(_CSubNodeContractionsSimpl& Second, short& Sgn);
	_NODE_RES _IgnoreNode(int Node);
	CONTRACTION_LIST m_List,m_AuxTestList;
	_iterator _find2insert(int NodeNo,int& Pos);//throws if already exists
	_iterator _find_existed(int NodeNo);//return end if not exists
	_const_iterator _find_existed(int NodeNo) const;//return end if not exists
	_iterator _find_existed(int NodeNo,int& Pos);//return end if not exists
	_iterator _find_first_into(int NodeNo,int& Pos);//return end if not exists
	_iterator _search_and_prepare_first_negative(int& Pos);//return end if not exists
	int _translate(int NodeNo) const;
	int _translate_and_update(int& NodeNo);
	int _insert(int NodeA, int NodeB);
	int _remove(int NodeA);
	void _Flatten();
	short _reverse(_iterator& It);//sgn change
	int _it2pos(_iterator& It);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	DECLARE_DYNAMIC_CREATION(_CSubNodeContractionsSimpl);
	int m_CurrZero/*,m_DelayedZero*/;
	typedef enum{CAN,NEED2SPLIT,DISCONNECT} CAN_REM;
	CAN_REM CanRemove(int Node);
	void UpdateNegatives();
	void DelayRemove()
	{
		//m_DelayedZero=Node;
		m_RemIt=m_List.end();
		m_Pos2Rem=-1;
		m_ModifyList.clear();
	}
	typedef CONTRACTION_LIST::iterator iterator;
	CONTRACTION_LIST::iterator m_RemIt;
	typedef list<CONTRACTION_LIST::iterator> TO_MODIFY;
	TO_MODIFY m_ModifyList;
	int m_Pos2Rem = -1;
	bool m_HasNegativeDels;
	friend class _CStateCofactor;
	friend class _CBoundaryState;
	friend class _CNewGreedyAnalyser;
	friend class _CNewGreedyStateCofactor;
	friend class _CDeletionsCompresor;
	friend class _CComplexDeletionsSimplifier;
	friend class _CContractionPair;
	friend class _CContractionPairSimpl;
};

class _CSubNodeContractions : public _CSubNodeContractionsSimpl
{
public:
	_CSubNodeContractions(_CCircuitAnalyzer* pAnalyzer):m_pAnalyser(pAnalyzer) {}
	_CSubNodeContractions(const _CSubNodeContractions& Source) :_CSubNodeContractionsSimpl(Source),
		m_pAnalyser(Source.m_pAnalyser) {}
	_CSubNodeContractions(const _CSubNodeContractionsSimpl& Source, _CCircuitAnalyzer* pAnalyzer) :_CSubNodeContractionsSimpl(Source),
		m_pAnalyser(pAnalyzer) {}
	bool GreedyTestContract(int& ANode, int& BNode);
	void AddTestContract(int& ANode, int& BNode);
	bool IsSeparateNodeDisconneced(const COMPONENT_PINS_LIST& SeparateNodes,const _CGlobalIncidenceTable& GlobalTable) const;
	bool IsPinConnected(int Node) const;
	bool IsPinConnected(const COMPONENT_PINS_SET& DisconnnectedPins) const;
	bool IsPinConnected(const COMPONENT_PINS_LIST& DisconnnectedPins) const;
	bool IsEnough() const;
	virtual bool IsBoundaryNode(unsigned long Node) override;
	const _CCircuitAnalyzer* GetAnalyser() const { return m_pAnalyser; }
protected:
	_CCircuitAnalyzer* m_pAnalyser;
	//CONTRACTION_LIST m_AuxTestList;
	class _CConnectedNodes : public unordered_multimap<int,int> 
	{
	public:
		_CConnectedNodes(const CONTRACTION_LIST& Source,int CurrZero);
	};
	friend class _CDeletionsCompresor;
};

typedef enum{ AD_OK, AD_LOOP, AD_DISCONNECTION, AD_COMPLETE, AD_ENOUGH, AD_NEED_SPLIT_COL, AD_NEED_SPLIT_ROW, AD_NEED_SPLIY_BOTH } AD_DELETION;

class _CContractionPair;

class _CContractionPairSimpl : public _streamable
{
public:
	_CContractionPairSimpl() :m_Sgn(1), m_State(AD_OK) {}
	_CContractionPairSimpl(const _CSubNodeContractionsSimpl& C, const _CSubNodeContractionsSimpl& R, short Sgn) :
		m_C(C), m_R(R), m_Sgn(Sgn),m_State(AD_OK) {}
	_CContractionPairSimpl(const _CContractionPairSimpl& Source) :m_Sgn(Source.m_Sgn),
		m_R(Source.m_R),
		m_C(Source.m_C),m_State(Source.m_State) {}
	_CContractionPairSimpl(const _CContractionPair& Source);
	//bool Try2Compress(_CContractionPairSimpl& Other);
	bool Try2Compress2(_CContractionPairSimpl& Other);
	bool IsSelfLoop() { return m_State == AD_LOOP; }
	bool Try2Split(_CContractionPairSimpl& NewReminder);
	bool SplitRows(_CContractionPairSimpl& NewReminder);
	bool SplitCols(_CContractionPairSimpl& NewReminder);
	bool IsEqualIgnorigSign(const _CContractionPairSimpl& Right, short& Sgn) const
	{
		Sgn = m_Sgn;
		return m_R == Right.m_R && m_C == Right.m_C;
	}
	unsigned long long GetHash(unsigned long long Core = HASH_CORE);
	AD_DELETION RemoveNode(int Node);
	AD_DELETION RemoveNodes(const COMPONENT_PINS_LIST& NewSeparateNodes);
	AD_DELETION Translate(const NODES_CONNECTION& NodesConnections);
	bool ContainNegatives() { return m_R.m_HasNegativeDels || m_C.m_HasNegativeDels; }
	void _Store(_binary_filer& Filer);
	void _Load(_binary_filer& Filer);
	void ReverseCofactor(_CCofactor& OutCofactor, short GlobalSgn, size_t Index) const;
	void ReverseCofactor(_CBasicSetOfDeletions& OutCofactor, short GlobalSgn) const;
	void ReverseCofactor(_CDeletions& OutCofactor, short GlobalSgn) const;
	bool operator==(const _CContractionPairSimpl& Right) const
	{
		return m_State==Right.m_State && m_Sgn == Right.m_Sgn && m_R == Right.m_R && m_C == Right.m_C;
	}
	bool IsOkOrComplete() { return m_State == AD_OK || m_State == AD_COMPLETE; }
protected:
	typedef _CSubNodeContractions::_NODE_RES _NODE_RES;
	typedef _CSubNodeContractions::CAN_REM CAN_REM;
	_CSubNodeContractionsSimpl m_R;
	_CSubNodeContractionsSimpl m_C;
	short m_Sgn;
	//bool m_SelfLoop;
	AD_DELETION m_State;
	friend class _CContractionPair;
	friend class _CContractionListImpl;
	friend class _CContractionList;
};

class _CContractionPair
{
public:
	_CContractionPair(const _CContractionPair& Source)
		:m_Sgn(Source.m_Sgn), m_R(Source.m_R), m_C(Source.m_C), m_pAnalyzer(Source.m_pAnalyzer), m_HighestOrder(Source.m_HighestOrder),
		m_State(Source.m_State) {}
	_CContractionPair(_CCircuitAnalyzer* pAnalyzer);
	_CContractionPair(const _CSubNodeContractions& C, const _CSubNodeContractions& R, short Sgn);
	_CContractionPair(const _CSubNodeContractionsSimpl& C, const _CSubNodeContractionsSimpl& R, short Sgn, _CCircuitAnalyzer* pAnalyzer);
	_CContractionPair(const _CContractionPairSimpl& Source, _CCircuitAnalyzer* pAnalyzer);
	bool Try2Compress(_CContractionPair& Other);
	bool Try2Compress2(_CContractionPair& Other);
	bool IsSelfLoop() { return m_State == AD_LOOP; }
	bool IsOkOrComplete() { return m_State == AD_OK || m_State == AD_COMPLETE; }
	bool IsComplete() { return m_State == AD_COMPLETE; }
	AD_DELETION GetState() { return m_State; }
	bool Try2Split(_CContractionPair& NewReminder);
	bool SplitRows(_CContractionPair& NewReminder);
	bool SplitCols(_CContractionPair& NewReminder);
	bool IsEqualIgnorigSign(const _CContractionPair& Right, short& Sgn)
	{
		Sgn = m_Sgn;
		return m_R == Right.m_R && m_C == Right.m_C;
	}
	unsigned long long GetHash(unsigned long long Core = HASH_CORE);
	AD_DELETION AddDeletionPair(int P, int R, int K, int L);
	bool IsCompleteTest() const { return m_HighestOrder == m_R.NoOfDel(); }
	bool IsEnoughTest() const;
	bool IsEnough() const { return m_C.IsEnough() && m_R.IsEnough(); }
	bool ContainNegatives() { return m_R.m_HasNegativeDels || m_C.m_HasNegativeDels; }
	AD_DELETION RemoveNode(int Node);
	typedef list<_CContractionPair> SPLITED_LIST;
	AD_DELETION RemoveNodes(const COMPONENT_PINS_LIST& NewSeparateNodes, SPLITED_LIST& SplitedList);
	void ReverseCofactor(_CCofactor& OutCofactor, short GlobalSgn, size_t Index) const;
	void ReverseCofactor(_CBasicSetOfDeletions& OutCofactor, short GlobalSgn) const;
	void ReverseCofactor(_CDeletions& OutCofactor, short GlobalSgn) const;
	size_t GetHighestOrder() const { return m_HighestOrder; }
	typedef _CSubNodeContractions::CONTR_LIST CONTR_LIST;
	void SetTouched(INT_LIST& ExpectedList, CONTR_LIST& ContrList) const
	{
		m_R.SetTouched(ExpectedList, ContrList);
		m_C.SetTouched(ExpectedList, ContrList);
	}
	bool operator==(const _CContractionPair& Right) const
	{
		return m_State == Right.m_State && m_HighestOrder == Right.m_HighestOrder && m_Sgn == Right.m_Sgn && m_R == Right.m_R && m_C == Right.m_C;
	}
protected:
	typedef _CSubNodeContractions::_NODE_RES _NODE_RES;
	typedef _CSubNodeContractions::CAN_REM CAN_REM;
	_CSubNodeContractions m_R;
	_CSubNodeContractions m_C;
	short m_Sgn;
	//bool m_SelfLoop;
	AD_DELETION m_State;
	_CCircuitAnalyzer* m_pAnalyzer;
	size_t m_HighestOrder;
	friend class _CContractionList;
	friend class _CContractionPairSimpl;
};

//typedef list<_CContractionPair> CONTRATION_LIST;


typedef list<_CBasicPairOfDeletion> MULTI_BASIC_PAIR_OF_DELETIONS;
class _CContractionList;

class _CContractionListImpl : public _streamable
{
public:
	_CContractionListImpl() :m_GlobalSgn(1), m_State(AD_OK), m_List(1, _CContractionPairSimpl()) {}
	_CContractionListImpl(const _CContractionListImpl& Source) 
		:m_GlobalSgn(Source.m_GlobalSgn), m_List(Source.m_List), m_State(Source.m_State) {}
	_CContractionListImpl(const _CContractionList& Source);
	size_t NoOfDels() const
	{
		return m_List.size() < 1 ? 0 : m_List.front().m_C.NoOfDel();
	}
	void insert_in_order(_CContractionPairSimpl& Source);
	void Compress();
	void Split();
	bool IsEqualIgnoringSgn(const _CContractionListImpl& Other) const { return m_State==Other.m_State && m_List == Other.m_List; }
	bool IsEqualWithSgn(const _CContractionListImpl& Other) const { return m_State == Other.m_State && m_GlobalSgn == Other.m_GlobalSgn && m_List == Other.m_List; }
	short& ComSgn() { return m_GlobalSgn; }
	void Translate(int p, int r, int k, int l, MULTI_BASIC_PAIR_OF_DELETIONS& Result) const;
	unsigned long long GetHash(unsigned long long Core = HASH_CORE);
	bool IsSelfLoop() { return m_State == AD_LOOP; }
	bool IsOk() { return m_State == AD_OK; }
	AD_DELETION GetState() { return m_State; }
	AD_DELETION Translate(const NODES_CONNECTION& NodesConnections);
	AD_DELETION RemoveNodes(const COMPONENT_PINS_LIST& NewSeparateNodes);
	AD_DELETION RemoveNode(int Node);
	bool ContainNegatives();
	void _Store(_binary_filer& Filer);
	void _Load(_binary_filer& Filer);
	void ReverseCofactor(_CCofactor& OutCofactor) const;
	void ReverseCofactor(_CMultiBasicSetOfDeletions& OutCofactor) const;
	void ReverseCofactor(_CMultiDeletions& OutCofactor) const;
	bool operator==(const _CContractionListImpl& Right) const { return m_State == Right.m_State && m_List == Right.m_List; }
protected:
	void RetriveCommonSgn();
	typedef list<_CContractionPairSimpl> LIST;
	LIST m_List;
	short m_GlobalSgn;
	AD_DELETION m_State;
	friend class _CContractionList;
};

class _CContractionList
{
public:
	_CContractionList(_CCircuitAnalyzer* pAnalyzer) :m_GlobalSgn(1), m_pAnalyzer(pAnalyzer), m_State(AD_OK), m_List(1, _CContractionPair(pAnalyzer)){}
	_CContractionList(const _CContractionList& Source)
		:m_GlobalSgn(Source.m_GlobalSgn), m_List(Source.m_List), m_pAnalyzer(Source.m_pAnalyzer),m_State(Source.m_State) {}
	_CContractionList(const _CContractionListImpl& Source, _CCircuitAnalyzer* pAnalyzer);
	size_t NoOfDels() const
	{
		return m_List.size() < 1 ? 0 : m_List.front().m_C.NoOfDel();
	}
	void insert_in_order(_CContractionPair& Source);
	void sort();
	void Compress();
	void Split();
	bool IsEqualIgnoringSgn(const _CContractionList& Other) const { return m_List == Other.m_List; }
	bool IsEqualWithSgn(const _CContractionList& Other) const { return m_GlobalSgn == Other.m_GlobalSgn && m_List == Other.m_List; }
	short& ComSgn() { return m_GlobalSgn; }
	void Translate(int p, int r, int k, int l, MULTI_BASIC_PAIR_OF_DELETIONS& Result) const;
	unsigned long long GetHash(unsigned long long Core = HASH_CORE);
	AD_DELETION RemoveNodes(const COMPONENT_PINS_LIST& NewSeparateNodes);
	AD_DELETION AddDeletions(const _CMultiBasicSetOfDeletions& SetOfDeletions, const COMPONENT_PINS_LIST& NewSeparateNodes);
	bool ContainNegatives();
	AD_DELETION RemoveNode(int Node);
	AD_DELETION GetState() const { return m_State; }
	bool IsCompleteTest();
	bool IsEnoughTest() const;
	void ReverseCofactor(_CCofactor& OutCofactor) const;
	void ReverseCofactor(_CMultiBasicSetOfDeletions& OutCofactor) const;
	void ReverseCofactor(_CMultiDeletions& OutCofactor) const;
	size_t GetHighestOrder() const { return m_List.empty() ? 0 : m_List.back().GetHighestOrder(); }
	typedef _CSubNodeContractions::CONTR_LIST CONTR_LIST;
	void SetTouched(INT_LIST& ExpectedList, CONTR_LIST& ContrList) const;
	bool operator==(const _CContractionList& Right) const { return m_pAnalyzer==Right.m_pAnalyzer && m_State == Right.m_State && m_List == Right.m_List; }
protected:
	void RetriveCommonSgn();
	void DenormalizeSgn();
	typedef list<_CContractionPair> LIST;
	void SplitRows(LIST& ContractionPairs);
	void SplitCols(LIST& ContractionPairs);
	void SplitBoth(LIST& ContractionPairs)
	{
		SplitRows(ContractionPairs);
		SplitCols(ContractionPairs);
	}
	void SplitRowsOnce(LIST& ContractionPairs);
	void SplitColsOnce(LIST& ContractionPairs);
	void SplitBothOnce(LIST& ContractionPairs)
	{
		SplitRowsOnce(ContractionPairs);
		SplitColsOnce(ContractionPairs);
	}
	void RemoveNodeFrom(LIST& ContractionPairs, int Node);
	LIST m_List;
	short m_GlobalSgn;
	AD_DELETION m_State;
	friend class _CContractionListImpl;
	friend class _CNewGreedyStateCofactor;
	_CCircuitAnalyzer* m_pAnalyzer;
	bool RemoveNodesFromPair(const COMPONENT_PINS_LIST& NewSeparateNodes, _CContractionPair& MainPair, LIST& AdditionalList);// true - result in list
};

class _CDeletionsCompresor : public _CContractionPairSimpl
{
public:
	_CDeletionsCompresor(int CurrentZero = 0) {}
	bool Try2CompressWith(const _CDeletionsCompresor& OtherContractions);
protected:
};

class _CComplexDeletionsSimplifier : public _CContractionPairSimpl
{
public:
	_CComplexDeletionsSimplifier() :m_SelfLoop(false) {}
	_CComplexDeletionsSimplifier(const _CBasicSetOfDeletions& SetOfDeletions);
	_CComplexDeletionsSimplifier(const _CComplexDeletionsSimplifier& Source) 
		:_CContractionPairSimpl(Source), m_SelfLoop(Source.m_SelfLoop) {}
	bool Translate(_CBasicSetOfDeletions& SetOfDeletions);
	void MultiplySgn(short Mult) { m_Sgn *= Mult; }
	bool AddDeletions(const _CBasicSetOfDeletions& SetOfDeletions);
	bool AddPair(const _CBasicPairOfDeletion& Pair);
	bool IsSelfLoop() { return m_SelfLoop; }
	//bool Try2Compress(_CComplexDeletionsSimplifier& Other);
protected:
	bool m_SelfLoop;
};

class _ComplexDeletionCompressor
{
public:
	_ComplexDeletionCompressor(const _CMultiBasicSetOfDeletions& MulSetOfDel);
	bool Simplify();
	void TranslateBack(_CMultiBasicSetOfDeletions& MulSetOfDel);
protected:
	typedef list<_CComplexDeletionsSimplifier> LIST;
	LIST m_List;
};


class _CCommonStateCofactor : public _CInUseCheckpoint, public _CHashable
{
public:
	//typedef enum{ AD_OK, AD_LOOP, AD_DISCONNECTION, AD_COMPLETE, AD_ENOUGH } AD_DELETION;
	//typedef _CContractionList::AD_DELETION AD_DELETION;
	_CCommonStateCofactor(_CCircuitAnalyzer* pAnalyzer);
	//_CCommonStateCofactor(const _CCommonStateCofactor& Source): m_State(Source.m_State),
	//m_R(Source.m_R), m_C(Source.m_C), m_pAnalyzer(Source.m_pAnalyzer), m_HighestOrder(Source.m_HighestOrder)
	/*, m_NoOfDeletions(Source.m_NoOfDeletions)*/ /*{}*/
	_CCommonStateCofactor(const _CCommonStateCofactor& Source) : /*m_State(Source.m_State),*/
		m_Contractions(Source.m_Contractions), m_pAnalyzer(Source.m_pAnalyzer)/*, m_HighestOrder(Source.m_HighestOrder)*/ {}
	//inline AD_DELETION GetState() const { return m_State; }
	inline AD_DELETION GetState() const { return m_Contractions.GetState(); }
	inline bool IsLoop() const { return GetState() == AD_LOOP; }
	inline bool IsDisconnected() const { return GetState() == AD_DISCONNECTION; }
	inline bool IsComplete() const { return GetState() == AD_COMPLETE; }
	inline bool IsEnough() const { return GetState() == AD_ENOUGH; }
	inline bool IsInProgres() const { return GetState() == AD_OK; }
	inline size_t GetNoOfDeletions() const 
	{
		//size_t rNoDel=m_Contractions.front().m_R.NoOfDel();
		//size_t cNoDel=m_Contractions.front().m_C.NoOfDel();
		return m_Contractions.NoOfDels();
		//return m_NoOfDeletions;
	}
	virtual size_t DetermineHashKey(size_t Core=HASH_CORE) override;
	virtual size_t GetHash(size_t Core=HASH_CORE);
	//virtual bool IsEqualIgnoreSgn(_CCommonStateCofactor& Other);
	//const _CSubNodeContractions& R() const {return m_R;}
	//const _CSubNodeContractions& C() const {return m_C;}
	//size_t HighestOrder() const {return m_HighestOrder;}
	size_t HighestOrder() const { return m_Contractions.GetHighestOrder(); }
	bool operator==(const _CCommonStateCofactor& Right) const
	{
		return m_Contractions.IsEqualIgnoringSgn(Right.m_Contractions);
		//return  m_HighestOrder == Right.m_HighestOrder &&
		//	m_State == Right.m_State && m_Contractions.IsEqualIgnoringSgn(Right.m_Contractions);
	}
	//bool operator==(_CCommonStateCofactor& Right)
	//{
	//	return  m_HighestOrder == Right.m_HighestOrder &&
	//		m_State == Right.m_State && m_R.IsEqual(Right.m_R) && m_C.IsEqual(Right.m_C);
	//}
	bool IsEnoughTest() const;
	void Translate(int p, int r, int k, int l, MULTI_BASIC_PAIR_OF_DELETIONS& Results) const;
	//bool Translate(int& p, int& r, int& k, int& l) const;
protected:
	typedef _CSubNodeContractions::_NODE_RES _NODE_RES;
	typedef _CSubNodeContractions::CONTRACTION CONTRACTION;
	typedef _CSubNodeContractions::CONTRACTION_LIST CONTRACTION_LIST;
	typedef _CSubNodeContractions::_const_iterator _const_iterator;
	typedef _CSubNodeContractions::_iterator _iterator;
	inline bool IsCompleteTest();
	_CContractionList m_Contractions;
	//_CSubNodeContractions m_R;
	//_CSubNodeContractions m_C;
	//AD_DELETION m_State;
	_CCircuitAnalyzer* m_pAnalyzer;
	//size_t m_HighestOrder;
	//size_t m_NoOfDeletions;
};

////===================================_CGreedyStateCofactor
//
//class _CGreedyStateCofactor : public _CCommonStateCofactor
//{
//public:
//	_CGreedyStateCofactor(_CCircuitAnalyzer* pAnalyzer):_CCommonStateCofactor(pAnalyzer) {}
//	_CGreedyStateCofactor(const _CCommonStateCofactor& Source): _CCommonStateCofactor(Source) {}
//	AD_DELETION TestGreedyAddDeletionPair(const _CBasicSetOfDeletions& SetOfDeletions);
//	bool operator==(_CGreedyStateCofactor& Other) 
//	{
//		return IsEqualIgnoreSgn(Other);
//	}
//protected:
//	friend class _CGreedyResContainer;
//};
//
//class _CGreedyResContainer : public _CHashable
//{
//public:
//	class _comp_path_pred : public less<const _CComponent*>
//	{
//	public:
//		bool operator()(const _CComponent* pLeft, const _CComponent* pRight)
//		{
//			bool LFD=pLeft->DeletionsAreForced();
//			bool RFD=pRight->DeletionsAreForced();
//			if(LFD==RFD)
//				return less<const _CComponent*>::operator ()(pLeft,pRight); 
//			else
//				return LFD;
//		}
//	};
//	typedef set<_CComponent*,_comp_path_pred> COMP_ON_A_PATH;
//	_CGreedyResContainer(const _CGreedyResContainer& Source):
//			m_CompOnAPath(Source.m_CompOnAPath),
//			m_StateCof(Source.m_StateCof),
//			m_CanBeComplate(Source.m_CanBeComplate),
//			m_IgnoreCache(Source.m_IgnoreCache)
//			{}
//	_CGreedyResContainer(const _CCommonStateCofactor& SourceCof, _CComponents& InitComponentList,SET_OF_COMPONENTS& IgnoreComponents,bool IgnoreCache);
//
//	_CGreedyResContainer(const _CCommonStateCofactor& SourceCof,COMP_ON_A_PATH& PrevPath,SET_OF_COMPONENTS& IgnoreComponents,bool IgnoreCache);
//	virtual long DetermineHashKey(long Core=HASH_CORE) override;
//	_CGreedyStateCofactor& GetState() {return m_StateCof;}
//	bool operator==(_CGreedyResContainer& Other);
//	bool ResolveAmIFeasible();
//	bool IgnoreCache() {return m_IgnoreCache;}
//protected:
//	COMP_ON_A_PATH m_CompOnAPath;
//	typedef set<_CComponent*> PIN_COMP_SET;
//	typedef map<size_t,PIN_COMP_SET> PIN_COMP_MAP;
//	_CGreedyStateCofactor m_StateCof;
//	bool m_CanBeComplate;
//	bool m_IgnoreCache;
//	friend class _CGreadyResCache;
//};
//===================================_CGreedyStateCofactor
//
//
//class _CGreadyResCache
//{
//public:
//	bool GetExistingResult(_CGreedyResContainer& ResContainer,bool*& pResult);
//	bool IsFeasible(_CGreedyResContainer& ResContainer);
//	void ClearCache() {m_Map.clear();}
//protected:
//	typedef hash_multimap<long,_CGreedyResContainer> MAP;
//	MAP m_Map;
//};
//
//
//===================================_CGreedyStateCofactor


//class _CGreadyResCache
//{
//public:
//	bool GetExistingResult(_CGreedyResContainer& ResContainer,bool*& pResult);
//	bool IsFeasible(_CGreedyResContainer& ResContainer);
//	void ClearCache() {m_Map.clear();}
//protected:
//	typedef hash_multimap<long,_CGreedyResContainer> MAP;
//	MAP m_Map;
//};
//


class _CStateCofactor : public _CCommonStateCofactor, public _streamable//, public _CInUseCheckpoint,public _CHashable
{
public:
	_CStateCofactor(_CCircuitAnalyzer* pAnalyzer=NULL):
		_CCommonStateCofactor(pAnalyzer),
		  //m_Sgn(1),
		  m_ZeroWasRemoved(false),
		  m_DelayedZero(-1)
		{}
	_CStateCofactor(const _CStateCofactor& Source): 
			_CCommonStateCofactor(Source),
				//m_Sgn(Source.m_Sgn), 
			m_ZeroWasRemoved(Source.m_ZeroWasRemoved),
			m_DelayedZero(Source.m_DelayedZero)
			{}
	inline short& Sgn() 
	{
		return m_Contractions.ComSgn();
		//return m_Sgn;
	}
	inline void CorrectSign(short MultSgm) 
	{
		m_Contractions.ComSgn() *= MultSgm;
		//m_Sgn*=MultSgm;
	}
	//AD_DELETION AddDeletionPair(int P, int R, int K, int L, 
	//	short Mult,const COMPONENT_PINS_LIST& NewSeparateNodes=EmptyPinList);
	AD_DELETION AddDeletions(const _CMultiBasicSetOfDeletions& Deletions,
		const COMPONENT_PINS_LIST& NewSeparateNodes=EmptyPinList);
	void ReverseCofactor(_CCofactor& OutCofactor) const;
	void ReverseCofactor(_CMultiBasicSetOfDeletions& OutCofactor) const;
	void ReverseCofactor(_CMultiDeletions& OutCofactor) const;
	AD_DELETION ReduceSeparateNodes(const COMPONENT_PINS_LIST& SeparateNodes);
	//AD_DELETION ReduceSeparateNode(int Node);
	//bool TestIsDisconnected(const COMPONENT_PINS_LIST& NewSeparateNodes);
	//bool IsEqualWithSgn(_CStateCofactor& Other, short& Sgn);
	//inline short ReCreateS() { return m_S.Recreate(m_R.GetNodesMap(),m_C.GetNodesMap());}
	bool operator==(_CStateCofactor& Other) const
	{
		return m_Contractions.IsEqualIgnoringSgn(Other.m_Contractions);
	}
	//bool operator==(_CStateCofactor& Other)
	//{
	//	return IsEqualIgnoreSgn(Other);
	//}
	//bool Translate2Nodes(const PINS_COLOCATIONS& PinsColocations, 
	//	const COMPONENT_PINS_SET& DisconnnectedPins);
	//bool ReDisconnectNodes(const COMPONENT_PINS_LIST& ReDisconnectedPins);
	virtual unsigned long long GetHash(unsigned long long Core=HASH_CORE);
	//virtual bool IsEqualIgnoreSgn(_CCommonStateCofactor& Other) override;
protected:
	//unsigned short FlattenAndReMismatch();
	//_CSignAidBuffer m_S;
	//unsigned short m_Mismatch;
	//short m_Sgn;
	bool m_ZeroWasRemoved;
	friend class _CBoundaryState;
	typedef _CSubNodeContractions::CAN_REM CAN_REM;
	int m_DelayedZero;
	//virtual void Store(_binary_filer& Filer) override;
	//virtual void Load(_binary_filer& Filer) override;
	//DECLARE_DYNAMIC_CREATION(_CStateCofactor);
	friend class _CNewGreedyStateCofactor;
};

class _CMultiStateCofactor
{
public:
	_CMultiStateCofactor(_CCircuitAnalyzer* pAnalyzer):m_pAnalyzer(pAnalyzer) {}
	void CompressCofactors();
protected:
	typedef list<_CStateCofactor> LIST;
	LIST m_List;
	_CCircuitAnalyzer* m_pAnalyzer;
};

class _CBoundaryState : public _streamable
{
public:
	_CBoundaryState()/*:m_State(AD_OK)*/ {}
	//_CBoundaryState(const _CBoundaryState& Source) :m_R(Source.m_R), m_C(Source.m_C),/*m_S(Source.m_S),*/
	//	m_Sgn(Source.m_Sgn), m_State(AD_OK) {}
	_CBoundaryState(const _CBoundaryState& Source) :m_Contractions(Source.m_Contractions)/*, m_State(AD_OK)*/ {}
	_CBoundaryState(const _CStateCofactor& Source);
	//_CBoundaryState(const _CStateCofactor& Source) :m_R(Source.m_R), m_C(Source.m_C),/*m_S(Source.m_S),*/
	//	m_Sgn(Source.m_Sgn), m_State(AD_OK) {}
	_CBoundaryState(const _CBoundaryState& Source, const NODES_CONNECTION& NodesConnection, const DONGLE_NODES& Dongles);
	typedef enum{AD_OK,AD_LOOP,AD_DISCONNECTION} AD_STATE;
	unsigned long long GetHash(unsigned long long Core=HASH_CORE);
	void ReverseCofactor(_CCofactor& OutCofactor) const;
	void ReverseCofactor(_CMultiBasicSetOfDeletions& OutCofactor) const;
	void ReverseCofactor(_CMultiDeletions& OutCofactor) const;
	bool IsOk() { return m_Contractions.IsOk(); }
	bool operator==(_CBoundaryState& Right) const
	{
		return m_Contractions.IsEqualWithSgn(Right.m_Contractions)/* && m_State == Right.m_State*/;
	}
	//bool operator==(_CBoundaryState& Right)
	//{
	//	return m_R.IsEqual(Right.m_R) && m_C.IsEqual(Right.m_C) && m_Sgn == Right.m_Sgn && m_State == Right.m_State;
	//}
protected:
	typedef _CSubNodeContractions::CAN_REM CAN_REM;
	typedef _CSubNodeContractions::_const_iterator _const_iterator;
	typedef _CSubNodeContractions::_iterator _iterator;
	void ReduceSeparateNodes(const DONGLE_NODES& Nodes);
	//_CSubNodeContractionsSimpl m_R;
	//_CSubNodeContractionsSimpl m_C;
	////_CSignAidBuffer m_S;
	//short m_Sgn;
	_CContractionListImpl m_Contractions;
	//AD_STATE m_State;
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	DECLARE_DYNAMIC_CREATION(_CBoundaryState);
};

class _CBoundarySocketStateVector
{
public:
protected:
};

typedef vector<_CStateCofactor> ORIGINAL_DESC_STATES;

class _CBounadryStatesVector : public _streamable
{
public:
	_CBounadryStatesVector() 
	{
		m_EnumIt=m_UniqMap.end();
	}
	_CBounadryStatesVector(const _CBounadryStatesVector& Source):m_UniqMap(Source.m_UniqMap),
		m_State2Index(Source.m_State2Index),m_Index2State(Source.m_Index2State)
	{
		m_EnumIt=m_UniqMap.end();
	}
	_CBounadryStatesVector(const ORIGINAL_DESC_STATES& Source);
	_CBounadryStatesVector(const _CBounadryStatesVector& Source,
		const NODES_CONNECTION& NodesConnection,const DONGLE_NODES& Dongles);
	//bool ReverseDeletions(size_t Index, _CDeletions& Dels);
	bool ReverseDeletions(size_t Index, _CMultiDeletions& Dels);
	void ResetEmum() { m_EnumIt = m_UniqMap.end(); }
	typedef list<size_t> TERMINALS_LIST;
	bool GetNext(_CBoundaryState*& pNextState, TERMINALS_LIST& TerminalsLits);
protected:
	DECLARE_DYNAMIC_CREATION(_CBounadryStatesVector);
	virtual void Store(_binary_filer& Filer) override;
	virtual void Load(_binary_filer& Filer) override;
	typedef unordered_multimap<unsigned long long,_CBoundaryState> UNIQ_MAP; 
	UNIQ_MAP m_UniqMap;
	typedef unordered_multimap<const _CBoundaryState*,size_t> STATE2INDEX;
	STATE2INDEX m_State2Index;
	typedef map<size_t,_CBoundaryState*> INDEX2STATE;
	INDEX2STATE m_Index2State;
	_CBoundaryState& GetUnique(_CBoundaryState& Source);
	UNIQ_MAP::iterator m_EnumIt;
	//typedef map<size_t,_CBoundaryState> MAP;
	//MAP m_Map;
};

class _CSocketBoundaryStatVector
{
public:
	_CSocketBoundaryStatVector() {}
protected:
};

class _CTempVertex
{
public:
	_CTempVertex(_CCircuitAnalyzer* pCircuitAnalyzer=NULL,
		_CComponent* pComponent=NULL):
	m_pCircuitAnalyzer(pCircuitAnalyzer),
	m_pComponent(pComponent),
	m_Descendants(pComponent==NULL?0:pComponent->NoOfEffevtiveDescendants(),DESC_DATA(NULL,1)) {}
	_CTempVertex(const _CTempVertex& Source):
		m_pCircuitAnalyzer(Source.m_pCircuitAnalyzer),
		m_pComponent(Source.m_pComponent),
		m_Descendants(Source.m_Descendants) {}
	_CComponent* GetComponent() const {return m_pComponent;}
	typedef pair<_CTempVertex*,short> DESC_DATA;
	const DESC_DATA& GetDescData(size_t DescId) const;
	size_t GetNoOfDescendants() const {return m_Descendants.size();}
	bool Is0();
	bool Is1();
	unsigned long long GetHash(unsigned long long Core=HASH_CORE) const;
	bool operator==(_CTempVertex& Right);
	short RetriveComMult();
	bool IsHollow();
	const DESC_DATA* GetHollowPinData();
	int GetHollowPin() const { return m_pComponent->HollowOutput(); }
	size_t GetDescDataSize() const { return m_Descendants.size(); }
	const DESC_DATA* GetPinData(size_t Index) const;
protected:
	_CComponent* m_pComponent;
	_CCircuitAnalyzer* m_pCircuitAnalyzer;
	typedef vector<DESC_DATA> DESCENDANT_VECTOR;
	DESCENDANT_VECTOR m_Descendants;
	DESC_DATA& GetDescData(size_t DescId);
	friend class _CCircuitAnalyzer;
};

class _CStateCofactorCache/*: public _CCacheTmpl<_CStateCofactor>*/
{
public:
	_CStateCofactorCache():m_PrevOrder(0),m_CurrOrder(0) {}
	//_CStateCofactorCache(_CStateCofactorCache& Source);//Clears original Map
	_CStateCofactor* RegisterUnique(_CStateCofactor* pStateCofactor, short& SgnChange);
	void KillOutOfDate();
	void Clear();
	void DetachCache();
protected:
	typedef _CCacheTmpl<_CStateCofactor> LOCAL_CACHE;
	typedef unordered_map<size_t,  LOCAL_CACHE> MAP;
	MAP m_Map;
	size_t m_PrevOrder;
	size_t m_CurrOrder;
};

class _CBasicCofactorVector : public vector<_CMultiBasicSetOfDeletions>
{
public:
	_CBasicCofactorVector() {}
	_CBasicCofactorVector(const _CMultiBasicSetOfDeletions& Source) :vector<_CMultiBasicSetOfDeletions>(1,Source) {}
	_CBasicCofactorVector(const _CCofactorsVector& Source);
};
//class _CBasicCofactorVector : public vector<_CBasicSetOfDeletions>
//{
//public:
//	_CBasicCofactorVector() {}
//	_CBasicCofactorVector(const _CBasicCofactorVector& Source) :vector<_CBasicSetOfDeletions>(Source) {}
//	_CBasicCofactorVector(const _CCofactorsVector& Source);
//};

typedef set<_CStateCofactor*> STATE_COFACTOR_SET;

class _CMultLevelInterface
{
public:
	_CMultLevelInterface(const size_t Level, _CMultLevelInterface* pUpperInterface):
		m_Level(Level), 
		m_pUpperInterface(pUpperInterface)
		{}
	_CMultLevelInterface(const _CMultLevelInterface& Source):
		m_Level(Source.m_Level), 
		m_pUpperInterface(Source.m_pUpperInterface)
		{}
	_CMultLevelInterface* GetLevelInterface(size_t Level);
	_CMultLevelInterface* GetPrevLevelInterface() 
	{
		ASSERTPDD(m_Level>=1);
		return GetLevelInterface(m_Level-1);
	}
	virtual STATE_COFACTOR_SET* GetMyCofState()=0;
	typedef SET_OF_COMPONENTS IGNORE_COMP_SET;
	virtual void PrepareIgnoreCompSet(IGNORE_COMP_SET& IgnoreCompSet) {} 
	virtual void PrevTestRemovalCompPins(const COMPONENT_PINS_LIST*& pPinsLis) {}
	virtual void PrevTestRemoveTest() {}
protected:
	const size_t m_Level;
	_CMultLevelInterface* m_pUpperInterface;
};
//*************************_CNewGreedyStateCofactor*****************************************
class _CNewGreedyAnalyser;

class _CNewGreedyStateCofactor : public _CCommonStateCofactor
{
public:
#ifdef _INCREMENT_TOP
	_CNewGreedyStateCofactor(const _CNewGreedyStateCofactor& Source): 
		_CCommonStateCofactor(Source),
			m_ZeroWasRemoved(Source.m_ZeroWasRemoved),
			m_DelayedZero(Source.m_DelayedZero),
			m_OriginalNodes(Source.m_OriginalNodes),
			m_ErasedNodes(Source.m_ErasedNodes) {}
	_CNewGreedyStateCofactor(const _CStateCofactor& Source, const _CNewGreedyAnalyser& Analyser,const INT_LIST& FixedNodes):
		_CCommonStateCofactor(Source),
			m_ZeroWasRemoved(Source.m_ZeroWasRemoved),
			m_DelayedZero(Source.m_DelayedZero),
			m_OriginalNodes(FixedNodes) {}
	_CNewGreedyStateCofactor(const _CStateCofactor& Source):
		_CCommonStateCofactor(Source),
		m_ZeroWasRemoved(Source.m_ZeroWasRemoved),
		m_DelayedZero(Source.m_DelayedZero)
	{}
#else
	_CNewGreedyStateCofactor(const _CNewGreedyStateCofactor& Source): 
		_CCommonStateCofactor(Source),
			m_ZeroWasRemoved(Source.m_ZeroWasRemoved),
			m_DelayedZero(Source.m_DelayedZero)
		{}
	_CNewGreedyStateCofactor(const _CStateCofactor& Source/*, const _CNewGreedyAnalyser& Analyser*/):
		_CCommonStateCofactor(Source),
			m_ZeroWasRemoved(Source.m_ZeroWasRemoved),
			m_DelayedZero(Source.m_DelayedZero)
		{}
#endif
	AD_DELETION TestGreedyAddDeletionPair(const _CBasicSetOfDeletions& SetOfDeletions);
	//bool TestDeletionPair(int& P, int& R, int& K, int& L) const;
	//AD_DELETION AddDeletionPair(int P, int R, int K, int L, 
		//const COMPONENT_PINS_LIST& NewSeparateNodes=EmptyPinList);
	//AD_DELETION AddDeletions(const _CBasicSetOfDeletions& Deletions,
	//	const COMPONENT_PINS_LIST& NewSeparateNodes = EmptyPinList);
	AD_DELETION AddDeletions(const _CMultiBasicSetOfDeletions& Deletions,
		const COMPONENT_PINS_LIST& NewSeparateNodes = EmptyPinList);
	AD_DELETION ReduceSeparateNodes(const COMPONENT_PINS_LIST& SeparateNodes);
	//AD_DELETION ReduceSeparateNode(int Node);
	virtual unsigned long long GetHash(unsigned long long Core=HASH_CORE);
	bool operator==(_CNewGreedyStateCofactor& Right) const
	{
		return m_ZeroWasRemoved==Right.m_ZeroWasRemoved &&
			m_DelayedZero==Right.m_DelayedZero &&
			(_CCommonStateCofactor)(*this)==(_CCommonStateCofactor)Right;
	}
	typedef _CSubNodeContractionsSimpl::CONTR_LIST CONTR_LIST; 
protected:
#ifdef _INCREMENT_TOP
	void SetTouched(INT_LIST& MNR, CONTR_LIST& MRCTRL) const;
#endif
	bool m_ZeroWasRemoved;
	//friend class _CBoundaryState;
	typedef _CSubNodeContractions::CAN_REM CAN_REM;
	int m_DelayedZero;
	friend class _CNewGreedyResContainer;
	friend class _CNewGreedyAnalyser;
	friend class _CAdvancedComOnPath;
	typedef INT_LIST NODES_SET;
#ifdef _INCREMENT_TOP
	const NODES_SET& m_OriginalNodes;
	NODES_SET m_ErasedNodes;
#endif
	//NODES_SET m_TNR,m_TNC;//Touched Nodes
	//NODES_SET m_MNR,m_MNC;//Missed Nodes
	//NODES_SET m_JAR,m_JAC;//Just Added
};
//*************************_CNewGreedyStateCofactor*****************************************

//*************************_CNewGreedyAnalyser**********************************************
//class _CNewGreedyAnalyser 
//{
//public:
//	_CNewGreedyAnalyser(const _CNewGreedyAnalyser& Source):
//			m_CanBeComplate(Source.m_CanBeComplate),
//				m_AdvancedComOnPath(Source.m_AdvancedComOnPath), m_Cache(m_AdvancedComOnPath),m_pAnalyzer(Source.m_pAnalyzer)
//			{}
//
//	_CNewGreedyAnalyser(_CComponents& InitComponentList):m_AdvancedComOnPath(InitComponentList,m_pAnalyzer),m_Cache(m_AdvancedComOnPath),
//		m_pAnalyzer(NULL){}
//	bool CanBeComplete(const _CNewGreedyStateCofactor& TestedState);
//	void TempRemoveComponent(_CComponent* pComponent,COMPONENT_PINS_LIST& RemovedPins=COMPONENT_PINS_LIST());
//	void PermRemoveComponent(_CComponent* pComponent);
//	void TempRemoveComponent(const SET_OF_COMPONENTS& CompSet);
//	void RetryTempRemovedComponents();
//	typedef pair<_CComponent*,COMPONENT_PINS_LIST> COMP_DATA;
//	typedef list<COMP_DATA> COMPONENTS_LIST;
//#ifdef _INCREMENT_TOP
//	void PrepareNodesInPorgres(INT_LIST& NodesList) {m_AdvancedComOnPath.PrepareNodesInPorgres(NodesList); }
//#endif
//	//void GetFixedOrderComponents(COMPONENTS_LIST& List) {m_AdvancedComOnPath.GetFixedOrderComponents(List);}
//#ifndef _ALLOW_REDUNDANCY
//	void Store() {m_AdvancedComOnPath.Store();}
//	void Restore() {m_AdvancedComOnPath.Restore();m_RemComps.clear();}
//#endif
//
//protected:
//	friend class _CCircuitAnalyzer;
//	_CCircuitAnalyzer* m_pAnalyzer;
//	typedef set<_CComponent* /*,_comp_path_pred*/> COMP_ON_A_PATH;
//
//#ifdef _ALLOW_REDUNDANCY
//
//	class _CAdvancedComOnPath
//	{
//	public:
//		bool ForcedDeletionEmpty() {return m_ForcedDeletionsComps.empty();}
//		_CAdvancedComOnPath(_CComponents& InitComponentList,_CCircuitAnalyzer*& pAnalyser);
//		_CAdvancedComOnPath(const _CAdvancedComOnPath& Source): m_pAnalyzer(Source.m_pAnalyzer),m_ForcedDeletionsComps(Source.m_ForcedDeletionsComps) {}
//		_CAdvancedComOnPath(const COMP_ON_A_PATH& Source,_CCircuitAnalyzer*& pAnalyser);
//		void AddComponent(_CComponent* pComponent,bool Reorder=true);
//		//void _RemoveComponent(_CComponent* pComponent,COMPONENT_PINS_LIST& RemovedPins,bool PermanentRemove=false,bool Reorder=true);
//		void _RemoveComponent(_CComponent* pComponent,COMPONENT_PINS_LIST& RemovedPins,bool PermanentRemove=false,bool Reorder=true);
//		_CComponent* GetTopComponent(const _CNewGreedyStateCofactor& CurrCof);
//		bool ThereAreForcedDeletions() {return !m_ForcedDeletionsComps.empty();}
//	protected:
//		friend class _CNewGreedyStateCofactor;
//		typedef _CNewGreedyStateCofactor::NODES_SET NODES_SET;
//		_CCircuitAnalyzer*& m_pAnalyzer;
//		COMP_ON_A_PATH m_ForcedDeletionsComps;
//	};
//
//#else
//
//	class _CAdvancedComOnPath
//	{
//	public:
//		bool ForcedDeletionEmpty() {return m_ForcedDeletionsComps.empty();}
//		_CAdvancedComOnPath(_CComponents& InitComponentList,_CCircuitAnalyzer*& pAnalyser);
//		_CAdvancedComOnPath(const _CAdvancedComOnPath& Source):m_Map(Source.m_Map),/*m_List(Source.m_List),*/
//		m_pAnalyzer(Source.m_pAnalyzer),m_ForcedDeletionsComps(Source.m_ForcedDeletionsComps) {}
//		_CAdvancedComOnPath(const COMP_ON_A_PATH& Source,_CCircuitAnalyzer*& pAnalyser);
//		void AddComponent(_CComponent* pComponent,bool Reorder=true);
//		//void _RemoveComponent(_CComponent* pComponent,COMPONENT_PINS_LIST& RemovedPins,bool PermanentRemove=false,bool Reorder=true);
//		void _RemoveComponent(_CComponent* pComponent,COMPONENT_PINS_LIST& RemovedPins,bool PermanentRemove=false,bool Reorder=true);
//		_CComponent* GetTopComponent(const _CNewGreedyStateCofactor& CurrCof);
//		bool ThereAreForcedDeletions() {return !m_ForcedDeletionsComps.empty();}
//		void SetSet(SET_OF_COMPONENTS& SetOfComponents) const;
//		//void GetFixedOrderComponents(COMPONENTS_LIST& List);
//		void Store()
//		{
//			m_CopyMap=m_Map;
//			m_CopyForcedDeletionsComps=m_ForcedDeletionsComps;
//		}
//		void Restore()
//		{
//			m_Map=m_CopyMap;
//			m_ForcedDeletionsComps=m_CopyForcedDeletionsComps;
//			PrepareSortedList();//Odtworzyæ Forced
//		}
//#ifdef _INCREMENT_TOP
//		void PrepareNodesInPorgres(INT_LIST& NodesList);
//		typedef _CSubNodeContractionsSimpl::CONTR_LIST CONTR_LIST; 
//#endif
//	protected:
//		friend class _CNewGreedyStateCofactor;
//		typedef _CNewGreedyStateCofactor::NODES_SET NODES_SET;
//		_CCircuitAnalyzer*& m_pAnalyzer;
//#ifndef _INCREMENT_TOP
//		void PrepareSortedList();
//#endif
//		COMP_ON_A_PATH m_ForcedDeletionsComps;
//		typedef COMP_ON_A_PATH PIN_COMP_SET;
//#ifdef _INCREMENT_TOP
//		typedef map<size_t,PIN_COMP_SET> PIN_COMP_MAP;
//#else
//		typedef pair<size_t/*PinNo*/,size_t/*Incidences*/> INCIDENCE;
//		typedef list<INCIDENCE> INCIDENCE_LIST;
//		INCIDENCE_LIST m_List;
//		typedef pair<PIN_COMP_SET,INCIDENCE_LIST::iterator> PIN_COMP_PAIR;
//		typedef map<size_t,PIN_COMP_PAIR> PIN_COMP_MAP;
//		class _sort
//		{
//		public:
//			bool operator()(const INCIDENCE& left, const INCIDENCE& right)
//			{
//				if(left.second==right.second)
//					return left.first<right.first;
//				return left.second==0?false:(right.second==0?true:left.second<right.second);
//			}
//		};
//
//#endif
//		PIN_COMP_MAP m_Map;
//	private:
//		PIN_COMP_MAP m_CopyMap;
//		COMP_ON_A_PATH m_CopyForcedDeletionsComps;
//	};
//
//#endif
//
//	_CAdvancedComOnPath m_AdvancedComOnPath;
//	//typedef set<_CComponent*> PIN_COMP_SET;
//	//typedef map<size_t,PIN_COMP_SET> PIN_COMP_MAP;
//	SET_OF_COMPONENTS m_RemComps;
//	//_CNewGreedyStateCofactor m_StateCof;
//	bool m_CanBeComplate;
//	class _CCacheContainer
//	{
//	public:
//		_CCacheContainer(const _CNewGreedyStateCofactor& State,const _CAdvancedComOnPath& ComPath);
//		unsigned long long GetHash(unsigned long long Core=HASH_CORE);
//		bool operator==(_CCacheContainer& Right)
//		{
//			return Right.m_Cof==m_Cof && Right.m_SetOfComp==m_SetOfComp;
//		}
//		bool m_IsFeaseable;
//	protected:
//		_CNewGreedyStateCofactor m_Cof;
//		SET_OF_COMPONENTS m_SetOfComp;
//		friend class _CAdvancedComOnPath;
//		friend class _CCache;
//	};
//	class _CCache : public hash_multimap<unsigned long long,_CCacheContainer*>
//	{
//	public:
//		_CCache(_CAdvancedComOnPath& AdvancedComOnPath):m_AdvancedComOnPath(AdvancedComOnPath) {}
//		~_CCache();
//		bool GetResultIfExists(const _CNewGreedyStateCofactor& Cofactor,bool*& Result); 
//	protected:
//		_CAdvancedComOnPath& m_AdvancedComOnPath;
//	};
//	_CCache m_Cache;
//	friend class _CNewGreedyStateCofactor;
//};
//*************************_CNewGreedyAnalyser*********************************************
//*************************_CFasrGreedyAnalyser********************************************
class _CFastGreedyAnalyser
{
public:
	_CFastGreedyAnalyser(const _CComponents& OrgComponents):m_OrgComponents(OrgComponents),
		m_pAnlyser(NULL) {}
	bool CanBeComplete(const _CNewGreedyStateCofactor& TestedState, SET_OF_COMPONENTS& IgnoredComponents);
protected:
	bool IsComponentContradicted(const _CNewGreedyStateCofactor& TestedState,_CComponent* pComponent/*,_CTestIncidentsWRAPER TopIncd*/);
	const _CComponents& m_OrgComponents;
	_CCircuitAnalyzer* m_pAnlyser;
	friend class _CCircuitAnalyzer;
};
//*************************_CFasrGreedyAnalyser********************************************
//*************************_CCircuitAnalyzer***********************************************


class _CCircuitAnalyzer /*: public _CMultLevelInterface*/
{
public:
	_CCircuitAnalyzer( 
		_CCircuit* pTopCircuit,
		_CComponents& IdealComponentsList,
		_CComponents& MainComponentsList,
		bool OrderOptimisation,
		COMPONENT_PINS_SET* pBoundaryPins=NULL,
		_CCofactorsVector& TopCofactors=_CCofactorsVector());//if subcircuit
	~_CCircuitAnalyzer();
	//void SetBaseCircuit(_CCircuit* pBaseCircuit) {m_pBaseCircuit=pBaseCircuit;}
	bool ChangeTopDescendant(_CTempVertex* pOldVertex, _CTempVertex* pNewVerex);
	unsigned NoOfNodes() const;
	unsigned HighestNoOfNodes(bool Row=false) const;
	void AddAnotherComponent(_CComponent& NextComponent, bool Try2OptimizeOrder);
	void FinishAnalysis();
	size_t NoOfModelDescendants() const;
	//void GetOriginalDesndantDeltions(size_t Index,_CBasicSetOfDeletions& Deletions) const;
	//void GetOriginalDesndantDeltions(size_t Index,_CCofactor& Cofactor) const;
	//void GetTranslatedAllDesndantDeltions(const PINS_COLOCATIONS& PinsColocations,
	//	BASIC_SET_OF_DELETIONS& SetOfDeletions) const;
	void PerformAnalysis(bool Try2OptimizeOrder);
	STATE_COFACTOR_SET* GetBaseStates() {return m_pCurrentStateCofactors;}
	//virtual STATE_COFACTOR_SET* GetMyCofState() override {return m_pCurrentStateCofactors;}
	bool IsSubmodel() {return m_pBoundaryPins!=NULL;}
	bool IsBoundary(unsigned Node);
	//bool IsGreedyFeasible(_CGreedyResContainer& ResContainer)
	//{
	//	return m_GreedyResCache.IsFeasible(ResContainer);
	//}
	bool IsPinIgnored(unsigned Node)
	{
		return IsIn(m_IgnoredPins,Node);
	}
	typedef list<_CTempVertex> VERTICES_LIST;

	const VERTICES_LIST& GetTempVerticesList() const {return m_TempVerticesList;}
	typedef vector<const _CTempVertex*> CONST_VERTICES_VECTOR;
	void GetTempVerticesVector(CONST_VERTICES_VECTOR& VertVector);
	typedef _CTempVertex::DESC_DATA VERTEX_ROOT;
	typedef vector<VERTEX_ROOT> TEMP_VERTEX_VECTOR;
	const TEMP_VERTEX_VECTOR& GetCofVerticesVector() {return m_TopTempVertexVector;}
	typedef unordered_multimap<const _CTempVertex*,_CModelVertex**> COL_MAP;
	COL_MAP& GetColMap() {return m_ColMap;}
	typedef const _CTempVertex* CONST_P_TEMP_VERTEX;
	void RegisterTempVertex2ModelVertex(_CTempVertex* const * ppTempVertex,_CModelVertex** ppModelVertex);
	typedef unordered_multimap<_CStateCofactor*, _CModelVertex**> BOUNDARIES_MAP;
	const BOUNDARIES_MAP& GetBoundariesMap();
	unsigned long GetNoOfSubciruitDongles() const;
	//void PrepareDongle(size_t DongleNo, _CDeletions& Deletions, _CModelVertex& ModelVertex, unsigned long& Counter);
	void PrepareDongle(unsigned long DongleNo, _CMultiDeletions& Deletions, _CModelVertex& ModelVertex, unsigned long& Counter);
	void ClearGarbage();
	const ORIGINAL_DESC_STATES& GetOriginalDescState();
	size_t DonglIndex(const _CTempVertex** ppDongleAdress) const
	{
		if(!m_CanonizationMode || m_pSubcircuitOutData==NULL) return 0;
		return m_pSubcircuitOutData->DongleIndex(ppDongleAdress);
	}
	void SimplifyToBeCanonic();
	//void OnCanonizationMode() {m_CanonizationMode=true;}
	//void OffCanonizationMode() {m_CanonizationMode=false;}
	_CCircuit* GetOwner() { return m_pBaseCircuit; }
protected:
	bool IsDescEmpty(const _CTempVertex::DESC_DATA* pDescData);
	bool IsVertexHollow(const _CTempVertex& Vertex);
	void ReportMinIncidences(_CComponent& Component,unsigned long& MinValue, unsigned long& MinNumber); 
	int DetermineNoOfIcidenceInNode(int Node);//-1 means boundary
	typedef list<VERTICES_LIST::iterator> VERTICES_IT_LIST;
	void PrepareVertItList(VERTICES_IT_LIST& List);
	bool m_CanonizationMode;
	//typedef vector<const _CBasicSetOfDeletions*> DESC_SETS_OFDELETIONS;
	typedef pair<_CStateCofactor*,short> DESC_COFACTORS;
	typedef vector<DESC_COFACTORS> DESC_COFACTORS_V;
	COL_MAP m_ColMap;
	friend class _CStateCofactor;
	//_CGreadyResCache m_GreedyResCache;
	_CComponent* GetNextOptimalComponent();
	_CComponent* GetNextOptimalComponentFast();
	void FIndNotConnected(const PINS_COLOCATIONS& PinsColocations, COMPONENT_PINS_LIST& DisconnectedPins) const;
	unique_ptr<COMPONENT_PINS_SET> m_pBoundaryPins;
	//COMPONENT_PINS_SET* m_pBoundaryPins;
	COMPONENT_PINS_SET m_IgnoredPins;
	bool m_IsSubcircuit;
	//void SwitchCache();
	void SwitchTempVert2Redef();
	inline void SwitchAll()
	{
		//SwitchCache();
		SwitchTempVert2Redef();
		SwitchStateCofactorSets();
	}
	inline void PrepareIncidentTable()
	{
		m_IncidentTable.AddCompPins(m_MainComponentsList,false,&m_pSeparateNodes2Test);
	}
	void PrepareCofactorsStates();
	_CCofactorsVector& m_TopCofactors;
	typedef vector<_CStateCofactor*> STATE_COF_VECTOR;
	STATE_COF_VECTOR m_TopStateCofVector;
	TEMP_VERTEX_VECTOR m_TopTempVertexVector;
	_CComponents& m_IdealComponentsList;
	_CComponents& m_MainComponentsList;
	_CComponents m_AnalyzedComponents;
#ifdef _REORDER
	_CNewGreedyAnalyser::COMPONENTS_LIST m_OrderedComponentsList;
#endif
	_CStateCofactorCache m_StateCache;
	//_CStateCofactorCache m_Cache1;
	//_CStateCofactorCache m_Cache2;
	//_CStateCofactorCache* m_pCurrentStateCache;
	//_CStateCofactorCache* m_pPreveStateCache;
	_CGlobalIncidenceTable m_IncidentTable;
	_CCircuit* m_pBaseCircuit;
	bool CanBeDifferentThenZero(const _CStateCofactor& InputState,IGNORE_COMP_SET& IgnoreSet,bool IgnoreCache);
	VERTICES_LIST m_TempVerticesList;
	void MakeTempVerticesCanonc();
	_CTempVertex& GetNewVertex(_CComponent* pComponent);
	bool GetNewOrExistedVertex(_CTempVertex*& PVertex,
		_CComponent* pComponent,
		const DESC_COFACTORS_V& Desc);//true if is new
	class _CUniqueCacheContainer
	{
	public:
		_CUniqueCacheContainer(const _CComponent* pComponent,
								const DESC_COFACTORS_V& DescDesc,
								_CTempVertex* pVertex=NULL):m_pComponent(pComponent),
								m_DescDesc(DescDesc),
								m_pVertex(pVertex) {}
		_CUniqueCacheContainer(const _CUniqueCacheContainer& Source):m_pComponent(Source.m_pComponent),
								m_DescDesc(Source.m_DescDesc),
								m_pVertex(Source.m_pVertex) {}
		bool operator==(const _CUniqueCacheContainer& Right) const
		{
			return m_pComponent==Right.m_pComponent && m_DescDesc==Right.m_DescDesc;
		}
		unsigned long long GetHash(unsigned long long Core=HASH_CORE);
		_CTempVertex* m_pVertex;
	protected:
		const _CComponent* m_pComponent;
		const DESC_COFACTORS_V m_DescDesc;
	};
	short CommMultiplierExtract(DESC_COFACTORS_V& Desc) const;
	typedef unordered_map<unsigned long long,_CUniqueCacheContainer> AUX_TEMP_VERTEX_MAP;
	AUX_TEMP_VERTEX_MAP m_AuxTempVertexMap;
	const COMPONENT_PINS_LIST* m_pSeparateNodes2Test;
	typedef unordered_multimap<_CStateCofactor*,VERTEX_ROOT*> TEMP_VERTS_2_REDEFINE;
	TEMP_VERTS_2_REDEFINE m_TempVerts2Redefine1;
	TEMP_VERTS_2_REDEFINE m_TempVerts2Redefine2;
	TEMP_VERTS_2_REDEFINE* m_pCurrentTempVerts2Redefine;
	TEMP_VERTS_2_REDEFINE* m_pPrevTempVerts2Redefine;
	_CTempVertex m_1_Vertex;
	_CTempVertex m_0_Vertex;
	STATE_COFACTOR_SET m_StateCofactorSet1;
	STATE_COFACTOR_SET m_StateCofactorSet2;
	STATE_COFACTOR_SET* m_pPrevStateCofactors; 
	STATE_COFACTOR_SET* m_pCurrentStateCofactors;
	//_CNewGreedyAnalyser m_GreadyResAnalyser;
	_CFastGreedyAnalyser m_FastGreedyAnalyser;
	void SwitchStateCofactorSets()
	{
		swap(m_pPrevStateCofactors,m_pCurrentStateCofactors);
		m_pCurrentStateCofactors->clear();
	}
	class _CSubciruitOutsData
	{
	public:
		_CSubciruitOutsData(_CCircuitAnalyzer& Analyzer):m_Analyzer(Analyzer) 
		{
			PrepareMe();
		}
		size_t GetDescamdamt(const _CTempVertex* pDongleVertex);
		//_CStateCofactor* GetDescCofactor(size_t Index);
		size_t GetNoOfModelDescendants() const {return m_OriginalDescStates.size();}
		const _CStateCofactor& GetOrginalStateCofactor(size_t Index) const 
		{
			return m_OriginalDescStates[Index];
		}
		size_t NoOfDescendants() {return m_OriginalDescStates.size();}
		void RegisterTempVertex2ModelVertex(_CTempVertex* const * ppTempVertex,_CModelVertex** ppModelVertex);
		const BOUNDARIES_MAP& GetBoundariesMap() { return m_BoundariesMap; }
		unsigned long GetNoOfSubciruitDongles() const { return (unsigned long)m_OriginalDescStates.size(); }
		//void PrepareDongle(size_t DongleNo, _CDeletions& Deletions, _CModelVertex& ModelVertex, unsigned long& Counter);
		void PrepareDongle(unsigned long DongleNo, _CMultiDeletions& Deletions, _CModelVertex& ModelVertex, unsigned long& Counter);
		const ORIGINAL_DESC_STATES& GetOriginalDescState() { return m_OriginalDescStates; }
		size_t DongleIndex(const _CTempVertex** ppDongleAddres) const;
	protected:
		void PrepareMe();
		_CCircuitAnalyzer& m_Analyzer;
		//typedef vector<_CStateCofactor> ORIGINAL_DESC_STATES;
		ORIGINAL_DESC_STATES m_OriginalDescStates;
		typedef unordered_map<const _CTempVertex**,size_t>  VERTEX_2_ORIGINAL_DESC_INDEX;
		VERTEX_2_ORIGINAL_DESC_INDEX m_Vertex2OriginalDescIndex;
		typedef map<unsigned long/*Index*/,unsigned long/*Counter*/> COUNTER;
		COUNTER m_Counter;
		//typedef hash_multimap<_CStateCofactor*, _CModelVertex**> BOUNDARIES_MAP;
		BOUNDARIES_MAP m_BoundariesMap;
	};
	unique_ptr<_CSubciruitOutsData> m_pSubcircuitOutData;
	//_CSubciruitOutsData* m_pSubcircuitOutData;
	class _COptimizationComponentData /*: public _CMultLevelInterface*/
	{
	public:
		_COptimizationComponentData(
			_CCircuitAnalyzer& Analyzer,
			_CComponent& MyComponent,
			//size_t TopLevel,
			//_CMultLevelInterface* pTopInterface,
			_CStateCofactorCache& TopStatesCache,
			STATE_COFACTOR_SET& LocalStateSet):
			m_Analyzer(Analyzer),
			m_MyComponent(MyComponent),
			m_HollowedVertices(0),
			m_CreatedVertices(0),
			m_DescendantsInNextLevel(0),
			m_GlobalPossibleDescendants(0),
			m_1Descendants(0),
			m_NoOfRemovedPins(0),
			m_pNextLevelOptimizationMap(NULL),
			m_pMyTopStates(NULL),
			m_MinIncidenceAfterReduction(0),
			m_NoOfPinsWithMinIncidence(0),
			m_StateCache(TopStatesCache),
			m_MyProductStates(LocalStateSet)//,
			//_CMultLevelInterface(TopLevel+1,pTopInterface)//Poprawiæ
		  {}
		_COptimizationComponentData(const _COptimizationComponentData& Source);
		~_COptimizationComponentData()
		{
			delete m_pNextLevelOptimizationMap;
		}
		//bool AnyWayIsFull() {return m_DescendantsInNextLevel==ossibleDescendants;}
		typedef map<_CComponent*,_COptimizationComponentData> OPTIMIZATION_MAP;
		void TestInfluence();
		virtual STATE_COFACTOR_SET* GetMyCofState() /*override*/ {return &m_MyProductStates;}
		//virtual void PrepareIgnoreCompSet(IGNORE_COMP_SET& IgnoreCompSet) override 
		//{
		//	m_pUpperInterface->PrepareIgnoreCompSet(IgnoreCompSet);
		//	IgnoreCompSet.insert(&m_MyComponent);
		//} 
		//void PrepareIgnoreCompSet(IGNORE_COMP_SET& IgnoreCompSet) 
		//{
		//	IgnoreCompSet.insert(&m_MyComponent);
		//} 
		//virtual void PrevTestRemovalCompPins(const COMPONENT_PINS_LIST*& pPinsLis) override 
		//{
		//	m_pUpperInterface->PrevTestRemovalCompPins(pPinsLis);
		//	m_Analyzer.m_IncidentTable.TestRemovalCompPins(m_MyComponent,pPinsLis);
		//}
		//void PrevTestRemovalCompPins(const COMPONENT_PINS_LIST*& pPinsLis) 
		//{
		//	m_Analyzer.m_IncidentTable.TestRemovalCompPins(m_MyComponent,pPinsLis);
		//}
		//virtual void PrevTestRemoveTest() override 
		//{
		//	m_Analyzer.m_IncidentTable.RemoveTest();
		//	m_pUpperInterface->PrevTestRemoveTest();
		//}
		//void PrevTestRemoveTest() 
		//{
		//	m_Analyzer.m_IncidentTable.RemoveTest();
		//}
		unsigned GetDescendantsInNextLevel() const {return m_DescendantsInNextLevel;}
		unsigned GetHollowedVertices() const {return m_HollowedVertices;}
		unsigned GetCreatedVertices() const {return m_CreatedVertices;}
		unsigned GetGlobalPossibleDescendants() const {return m_GlobalPossibleDescendants;}
		unsigned Get1Descendants() const {return m_1Descendants;}
		unsigned GetRelativeDescendantsInNextLevel() const { return 1000 * m_DescendantsInNextLevel / m_GlobalPossibleDescendants; }
		typedef enum{
			DescendantsInNextLevel, RelativeDescendantsInNextLevel, VerticesHollowed, VerticesCreated,
			GlobalPossibleDedcendants,Vertices1Suppressed,MinIncidence,MinIncidenceCounter,LastIncidence, NodesRemoved} VERT_TYPE;
		unsigned Criterion(VERT_TYPE Type)
		{
			switch(Type)
			{
			case DescendantsInNextLevel: return m_DescendantsInNextLevel;
			case RelativeDescendantsInNextLevel: return m_GlobalPossibleDescendants==0? 0 : 1000 * m_DescendantsInNextLevel / m_GlobalPossibleDescendants;
			case VerticesHollowed: return m_HollowedVertices;
			case VerticesCreated: return m_CreatedVertices;
			case GlobalPossibleDedcendants: return m_GlobalPossibleDescendants;
			case MinIncidence: return m_MinIncidenceAfterReduction;
			case MinIncidenceCounter: return m_NoOfPinsWithMinIncidence;
			case LastIncidence: return m_MinIncidenceAfterReduction - m_NoOfPinsWithMinIncidence;
			case NodesRemoved: return m_NoOfRemovedPins;
			default: return m_1Descendants;
			}
		}
		//NodesRemoved 
		int Comp(const _COptimizationComponentData& Right) const;
		//_COptimizationComponentData& GetHigherLevelData(_CComponent* pComopnent);
		bool ComponentsOverlap(const _CComponent& Left, const _CComponent& Right);
		//const _CStateCofactor* Get
		const STATE_COFACTOR_SET* GetMyTopStates() {return m_pMyTopStates;};
	protected:
		unsigned long m_HollowedVertices;
		unsigned long m_CreatedVertices;
		unsigned long m_GlobalPossibleDescendants;
		unsigned long m_1Descendants;
		unsigned long m_DescendantsInNextLevel;
		unsigned long m_MinIncidenceAfterReduction;
		unsigned long m_NoOfPinsWithMinIncidence;
		unsigned long m_NoOfRemovedPins;
		_CComponent& m_MyComponent;
		STATE_COFACTOR_SET* m_pMyTopStates;
		STATE_COFACTOR_SET& m_MyProductStates;
		_CCircuitAnalyzer& m_Analyzer;
		//typedef list<_CStateCofactor> H_LEVEL_STATES;
		//H_LEVEL_STATES m_MyStates;
		_CStateCofactorCache& m_StateCache;
		OPTIMIZATION_MAP* m_pNextLevelOptimizationMap;
	};
	class _C2ndOrderOptimizationComponentData /*: public _CMultLevelInterface*/
	{
	public:
		_C2ndOrderOptimizationComponentData(
			_CCircuitAnalyzer& Analyzer,
			_CComponent& MyComponent1,
			_CComponent& MyComponent2/*,
			_CStateCofactorCache& TopStatesCache,
			STATE_COFACTOR_SET& LocalStateSet*/):
			m_Analyzer(Analyzer),
			m_My1stComponent(MyComponent1),
			m_My2nComponent(MyComponent2),
			//m_HollowedVertices1_2(0),
			//m_HollowedVertices2_1(0),
			m_CreatedVertices1_2(0),
			m_CreatedVertices2_1(0),
			//m_DescendantsInNextLevel1_2(0),
			//m_DescendantsInNextLevel2_1(0),
			//m_GlobalPossibleDescendants(0),
			//m_1Descendants1_2(0),
			//m_1Descendants2_1(0),
			m_pMyTopStates(NULL)//,
			//m_StateCache(TopStatesCache),
			//m_MyProductStates(LocalStateSet),
			//_CMultLevelInterface(TopLevel+1,pTopInterface)//Poprawiæ
		  {}
		int Test2ndInfluence();
		int CompResults();
	protected:
		//size_t m_HollowedVertices1_2;
		//size_t m_HollowedVertices2_1;
		size_t m_CreatedVertices1_2;
		size_t m_CreatedVertices2_1;
		//size_t m_GlobalPossibleDescendants;
		//size_t m_1Descendants1_2;
		//size_t m_1Descendants2_1;
		//size_t m_DescendantsInNextLevel1_2;
		//size_t m_DescendantsInNextLevel2_1;
		_CComponent& m_My1stComponent;
		_CComponent& m_My2nComponent;
		STATE_COFACTOR_SET* m_pMyTopStates;
		//STATE_COFACTOR_SET m_MyProductStates;
		_CCircuitAnalyzer& m_Analyzer;
		//_CStateCofactorCache m_StateCache;
	};
	typedef map<_CComponent*, _COptimizationComponentData> OPTIMIZATION_MAP;
	typedef unique_ptr<OPTIMIZATION_MAP> P_OPTIMIZATION_MAP;
	typedef pair<int, int> DEL;
	typedef list<DEL> LIST_DELS;
	//typedef list<LIST_DELS> MULTI_LIST_DELS;
	//typedef pair<MULTI_LIST_DELS, MULTI_LIST_DELS> PAIR_LIST;//first - Rows, second - Columns
	typedef pair<LIST_DELS, LIST_DELS> PAIR_LIST;//first - Rows, second - Columns
	typedef map<_CStateCofactor*, PAIR_LIST> STATE_DEP_LIST_DELS;
	typedef map<_CComponent*,STATE_DEP_LIST_DELS> COMP_LIST_DELS;
	void PrepareCompListDels(OPTIMIZATION_MAP& OptMap, COMP_LIST_DELS& CompListDels);
	//bool ComponentsOverlaps(OPTIMIZATION_MAP::iterator it_Left,OPTIMIZATION_MAP::iterator it_Right);
	bool ComponentsOverlaps(STATE_DEP_LIST_DELS& Left,STATE_DEP_LIST_DELS& Right);
	P_OPTIMIZATION_MAP m_pOptimizationMap;
	P_OPTIMIZATION_MAP m_pOptimizationMapH;
	P_OPTIMIZATION_MAP m_pOptimizationMapC;
	//OPTIMIZATION_MAP* m_pOptimizationMap;
	//OPTIMIZATION_MAP* m_pOptimizationMapH;
	//OPTIMIZATION_MAP* m_pOptimizationMapC;
	void FilterTheSmallest(P_OPTIMIZATION_MAP& pOptimizationMap, _COptimizationComponentData::VERT_TYPE Type) const;
	void FilterTheGreatest(P_OPTIMIZATION_MAP& pOptimizationMap, _COptimizationComponentData::VERT_TYPE Type) const;
	void FilterClosed2Prev(P_OPTIMIZATION_MAP& pOptimizationMap) const;
	_CComponent* GetHigherOrderOptimization();
	//int TestOrder(_CComponent* pPred,_CComponent* pSucc);
	COMPONENT_PINS_SET m_TouchedPinSet;
	void RemoveTouches(const COMPONENT_PINS_LIST& List)
	{
		for each (auto Pin in List)
			m_TouchedPinSet.erase(Pin);
	}
	void TouchPin(unsigned Pin)
	{
		m_TouchedPinSet.insert(Pin);
	}
	void TouchPin(const _CComponent& Component);
	bool CoincidentWithProcessed(const _CComponent& Component);
	void TouchPin(const _CCofactorsVector& Vector)
	{
		for(_CCofactorsVector::const_iterator it=Vector.begin(),_e=Vector.end();it!=_e;it++)
			TouchPin(**it);
	}
	void TouchPin(const _CCofactor& Cofactor)
	{
		TouchPin(Cofactor.GetDeletions());
	}
	void TouchPin(const _CDeletions& Deletions)
	{
		for (auto& pDel : Deletions)
			TouchPin(*pDel);
	}
	void TouchPin(const _CMultiDeletions& MultiDeletions)
	{
		for (auto& DelsPair : MultiDeletions)
			TouchPin(DelsPair.second);
	}

	void TouchPin(const _CDeletion& Deletion)
	{
		TouchPin(abs(Deletion.P()));
		TouchPin(abs(Deletion.R()));
		TouchPin(abs(Deletion.K()));
		TouchPin(abs(Deletion.L()));
	}
	
	bool IsTouchedPin(unsigned Pin)
	{
		return IsIn(m_TouchedPinSet,Pin);
	}
	//typedef pair<_CComponent*,_CComponent*> COMP_COORDS;
	//typedef map<COMP_COORDS,short> COMP_MAP;
	//COMP_MAP m_ComMap;
	//bool StoreInComMap(_CComponent* pLeft,_CComponent* pRight,short CompRes);
	//short GetFromComMap(_CComponent* pLeft,_CComponent* pRight);
	friend class _CTempVertex;
	_CStateCofactor* m_HollowState;
	class _CFastCompOptimizer
	{
	public:
		_CFastCompOptimizer(_CCircuitAnalyzer* pAnalyzer, _CComponents& MainList, COMPONENT_PINS_SET& TouchedPinSet);
		_CComponent* GetNextComponent();//NULL - no components, extracts component from optimizer
		typedef map<unsigned,SET_OF_COMPONENTS> INCD_MAP;
		void GetSmallestIncds(LIST_OF_COMPONENTS& Components);
		void GetTouchedAndLast(LIST_OF_COMPONENTS& Components);
		void RemovComp(_CComponent* pComp);
	protected:
		void GetSmallestIncds(INCD_MAP& IncdMap);
		INCD_MAP m_Map;
		typedef list<INCD_MAP::value_type*> SORTED_COMP_SETS;
		SORTED_COMP_SETS m_SortedCompSet;
		void SortCompSet();
		//SET_OF_COMPONENTS* m_pCurrentCompSet;
		//void PrepareMin();
		_CCircuitAnalyzer* m_pAnalyzer;
		COMPONENT_PINS_LIST m_PrevPins;
		COMPONENT_PINS_SET& m_TouchedPinSet;
		size_t m_TheMostCommon;
	};
	unique_ptr<_CFastCompOptimizer> m_pFastOptimizer;
	//_CFastCompOptimizer* m_pFastOptimizer;
	friend class _CFastGreedyAnalyser;
	friend class _CContractionPair;
	friend class _CContractionPairSimpl;
};