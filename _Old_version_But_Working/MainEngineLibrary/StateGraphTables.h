#pragma once
#include <vector>
#include <set>
//
//#include "Circuit.h"
#include "InUseCheckpoint.h"
#include "CacheTmpl.h"
#include "binary_filer.h"
#include "NodesManager.h"

using namespace std;
using namespace stdext;

typedef enum tagResState{IN_USE,TERM_ONE,TERM_SELFLOOP,TERM_DISCONNECTED,TERM_SIZE_REJECTED} RES_STATE;

class _CCircuit;
class _CListOfNodes : public vector<unsigned>,public _streamable
{
public:
	~_CListOfNodes()
	{
	}
	_CListOfNodes()/*:INIT_STREAMABLE(_CListOfNodes)*/ {}
	_CListOfNodes(int Size, const int&   InitN):vector<unsigned>(Size,InitN)/*,
		INIT_STREAMABLE(_CListOfNodes)*/{}
	virtual size_t GetHashValue(size_t Core=HASH_CORE);
protected:
	DECLARE_DYNAMIC_CREATION(_CListOfNodes);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);

};	

class _CSparseListOfNodes : public INT_INT_MAP, public _streamable
{
public:
protected:
	DECLARE_DYNAMIC_CREATION(_CSparseListOfNodes);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
};

class _CComponentPredicator
{
public:
	~_CComponentPredicator()
	{
	}
	_CComponentPredicator():
	  m_NoOfNewAncestors(0),
		  m_StepsToZero(0){}
	 _CComponentPredicator(const _CComponentPredicator& Source):
	  m_NoOfNewAncestors(Source.m_NoOfNewAncestors),
		 m_StepsToZero(Source.m_StepsToZero),
		 m_RealNodes (Source.m_RealNodes) {}
	_CComponentPredicator& operator=(const _CComponentPredicator& Source);
	void SetNoOfNewAncestors(int NewValue) {m_NoOfNewAncestors=0;}
protected:
	int m_NoOfNewAncestors;
	int m_StepsToZero;
	_CListOfNodes m_RealNodes;
};

class _CGenericStateVector : public _CListOfNodes
{
public:
	~_CGenericStateVector() 
	{
	}
	int ValueAt(int Index);
	virtual size_t GetHashValue(size_t Core=HASH_CORE) override;
};

class _CSparseGenericStateVector : public _CSparseListOfNodes
{
public:
	int ValueAt(int Index);
	virtual size_t GetHashValue(size_t Core=HASH_CORE);//Tutaj
};

class _CDeletions;
class _CMultiDeletions;
class _CGraphTable : public _CInUseCheckpoint, public _CHashable, public _streamable
{
public:
	_CGraphTable(unsigned NoOfNodes=0);
	_CGraphTable(const _CGraphTable& Source)/*:INIT_STREAMABLE(_CGraphTable)*/ {Copy(Source);}
	_CGraphTable(_CGraphTable& Source,unsigned NoOfBoundaries);
	~_CGraphTable(void);
	_CGraphTable& operator=(const _CGraphTable& Source)
	{
		Copy(Source);
		return *this;
	}
	bool operator==(_CGraphTable& Operand);
	RES_STATE ReadRealVlues(int& p, int& r, int& k, int& l);
	RES_STATE DoShortCircuit(int p, int r, int k, int l);
	int RealRow(int a) {return m_R.ValueAt(a);}
	int RealColumn(int a) {return  m_C.ValueAt(a);}
	short GetSgn() const {return m_Sgn;}
	void Table2Deletions(_CMultiDeletions& Deletions, int Highest, short& Sign);
	//void Table2Deletions(_CDeletions& Deletions, const _CIntNodes& Nodes, short& Sign);
	bool Table2Deletions2(_CMultiDeletions& Deletions, const _CIntNodes& Nodes, short& Sign);
	void ReleaseDongles(const _CIntNodes& Dongles, const _CIntNodes& Nodes); 
	unsigned GetMaxNoNode() const;
	unsigned GetMaxDeletions() const;
	void AddDeletion(int& p, int& r, int& k, int& l);
	//void DestroyIfUnregistered()
	//{
	//	if(!m_IsRegistered)
	//		delete this;
	//}
	short ExtractMultiplier();
	void MultiplySgn(short Multiplier) {m_Sgn*=Multiplier;}
	void ReduceToNoOfNodes(unsigned NewNoOfNodes);
protected:
	DECLARE_DYNAMIC_CREATION(_CGraphTable);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	virtual size_t DetermineHashKey(size_t Core=HASH_CORE) override;
	void Copy(const _CGraphTable& Source);
	_CGenericStateVector m_R,m_C,m_S;
	short m_Sgn;
	friend class _CGraphState;
};

class _CGraphTableCache : public _CStreamableCacheTmpl<_CGraphTable>//, public _streamable
{
protected:
//	void Store(_binary_filer& Filer);
//	void Load(_binary_filer& Filer);
	DECLARE_DYNAMIC_CREATION(_CGraphTableCache);
public:
	~_CGraphTableCache()
	{
	}
};

//typedef vector<unsigned> COMPONENT_PINS;
//typedef pair<unsigned,unsigned> TWO_GRAPH_PIN;
//typedef vector<TWO_GRAPH_PIN> TWO_GRAPH_PINS; 
//
//
//typedef set<int> INT_SET;


class _CSparseCommIntegrity : public _CInUseCheckpoint, public _CHashable, public _streamable
{
public:
	_CSparseCommIntegrity() {}
	_CSparseCommIntegrity(const _CSparseCommIntegrity& Source):m_IncidentCurNodes(Source.m_IncidentCurNodes),
		m_Zeros(Source.m_Zeros) {}
	_CSparseCommIntegrity(const _CSparseCommIntegrity& Source,const COMPONENT_PINS& Pins):
		m_IncidentCurNodes(Source.m_IncidentCurNodes),m_Zeros(Source.m_Zeros) 
	{
		Remove(Pins);
	}
	int AddIncidence(int NoNumber);
	int RemoveIncidence(int NodeNumber);
	int ShortcircuitNodes(int TargetNode, int SuppresedNode);
	void Remove(const COMPONENT_PINS& Pins);
	void Copy(const _CSparseCommIntegrity& Source);
	void CopyAndRemove(const _CSparseCommIntegrity& Source,const COMPONENT_PINS& Pins);
protected:
	friend class _CSparseDeletionAnalyser;
 	bool IsConnected( _CSparseGenericStateVector& StateVector);
	INT_INT_MAP m_IncidentCurNodes;
	INT_SET m_Zeros;
	DECLARE_DYNAMIC_CREATION(_CSparseCommIntegrity);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	virtual size_t DetermineHashKey(size_t Core=HASH_CORE) override;
};

class _CSparseDeletionAnalyser : public _CInUseCheckpoint, public _CHashable, public _streamable
{
public:
	_CSparseDeletionAnalyser(const _CSparseCommIntegrity* pIntegrity=NULL):
	  m_pIntegrity(pIntegrity),m_Sgn(1) {}
	_CSparseDeletionAnalyser(const _CSparseDeletionAnalyser& Source,
		const _CSparseCommIntegrity* pIntegrity=NULL):
		m_pIntegrity(pIntegrity),
			m_R(Source.m_R),
			m_C(Source.m_C),
			m_S(Source.m_S),
			m_Sgn(Source.m_Sgn) {}
		bool EachNodeIsExternal() const {return m_pIntegrity==NULL;}
protected:
	friend class _CSparseCommIntegrity;
	DECLARE_DYNAMIC_CREATION(_CSparseDeletionAnalyser);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	virtual size_t DetermineHashKey(size_t Core=HASH_CORE) override;
	const _CSparseCommIntegrity* m_pIntegrity;
	_CSparseGenericStateVector m_R,m_C,m_S;
	short m_Sgn;
};

class _CIntegrityTable : public _CInUseCheckpoint, public _CHashable, public _streamable
{
public:
	_CIntegrityTable(int NoOfNodes=0):m_R(NoOfNodes,0),m_C(NoOfNodes,0) {}
	_CIntegrityTable(const _CIntegrityTable& Source){Copy(Source);} 
	_CIntegrityTable(const _CIntegrityTable& Source, unsigned NoOfBounderies);
	~_CIntegrityTable(void);
	_CIntegrityTable& operator=(const _CIntegrityTable& Source)
	{
		Copy(Source);
		return *this;
	}
	bool operator==(_CIntegrityTable& Operand);
	bool IsCompressedToNode();
	RES_STATE TestState();
//	RES_STATE DoOpenCircuit(int p, int r, int k, int l);
//	RES_STATE DoShortCircuit(int p, int r, int k, int l, bool WithoutRemoval);
	void PlugComponents(const COMPONENT_PINS& Pins);
	void UnPlugComponent(const TWO_GRAPH_PINS& Pins);
	bool IsDisconnected(const TWO_GRAPH_PINS& Pins);
	bool IsDisconnected(int r, int c);
	void MarkBoundaryNode(unsigned Pin);
	RES_STATE AddDeletion(int p, int r, int k, int l);
	RES_STATE CheckDisconnetions(const TWO_GRAPH_PINS& Pins);
	//void DestroyIfUnregistered()
	//{
	//	DestroyMembers();
	//	if(!m_IsRegistered)
	//		delete this;
	//}
	void ReduceToNoOfNodes(unsigned NewNoOfNodes);
	int Row(unsigned a) {return m_R[a];}
	int Column(unsigned a) {return  m_C[a];}
protected:
	DECLARE_DYNAMIC_CREATION(_CIntegrityTable);
	void Store(_binary_filer& Filer); 
	void Load(_binary_filer& Filer);
	virtual size_t DetermineHashKey(size_t Core=HASH_CORE) override;
	void Copy(const _CIntegrityTable& Source);
	_CListOfNodes m_R,m_C;
	friend class _CGraphState;
};

class _CIntegrityTableCache : public _CStreamableCacheTmpl<_CIntegrityTable>
{
public:
	~_CIntegrityTableCache()
	{
	}
protected:
	DECLARE_DYNAMIC_CREATION(_CIntegrityTableCache);
};

class _CGraphState : public _CInUseCheckpoint,public _CHashable, public _streamable
{
public:
	_CGraphState():m_pCircuitOwner(NULL), 
		m_pGraphTable(NULL), 
		m_pIntegrityTable(NULL),m_State(IN_USE),m_NoOfDeletions(0) {}
	_CGraphState(_CCircuit* Owner,
		_CGraphTable* pGTable,
		_CIntegrityTable* pITable):m_pCircuitOwner(Owner), 
		m_pGraphTable(pGTable), 
		m_pIntegrityTable(pITable),m_State(IN_USE),m_NoOfDeletions(0)/*, INIT_STREAMABLE(_CGraphState)*/
	{
		if(m_pGraphTable!=NULL && m_pIntegrityTable!=NULL)
			TouchDepended();
	}
	_CGraphState(_CCircuit* pOnlyOwner);//only basic state
	_CGraphState(const _CGraphState& Source, bool ForceNewTables=false):
			m_pGraphTable(NULL),m_pIntegrityTable(NULL)/*, INIT_STREAMABLE(_CGraphState)*/
		{Copy(Source, ForceNewTables);}//dodaæ full copy
	_CGraphState(const _CGraphState& Source,unsigned NoOfBoundaryNodes);
	~_CGraphState(void);
	//RES_STATE DoShortCircuit(int p, int r, int k, int l, bool PureShortCircuit=false);
	//void TestShortCircuit(int p, int r, int k, int l,_CComponentPredicator* pPredicator);
	//RES_STATE DoOpenCircuit(int p, int r, int k, int l);
	//void TestOpenCircuit(int p, int r, int k, int l,_CComponentPredicator* pPredicator);
	_CGraphState& operator=(const _CGraphState& Operand) 
	{
		Copy(Operand,false);
		return *this;
	}
	bool operator==(_CGraphState& Operand);
	void ForceZero();
	RES_STATE GetState() const {return m_State;}
	void RegisterMembers();
	RES_STATE TestState();
	_declspec(property(get=GetIntegrityTable, put=SetIntegrityTable)) _CIntegrityTable* pIntegrityTable;
	_declspec(property(get=GetGraphTable, put=SetGraphTable)) _CGraphTable* pGraphTable;
	void SetIntegrityTable(_CIntegrityTable* pIntegrityTable);
	void SetGraphTable(_CGraphTable* pGraphTable);
	_CIntegrityTable* GetIntegrityTable() const {return m_pIntegrityTable;}
	_CGraphTable* GetGraphTable() const {return m_pGraphTable;}
	RES_STATE AddDeletion(int p, int r, int k, int l);
	RES_STATE CheckDisconnetions(const TWO_GRAPH_PINS& Pins) 
	{
		return m_pIntegrityTable->CheckDisconnetions(Pins);
	}
	unsigned GetMaxNoDeletions() const {return m_pGraphTable->GetMaxDeletions()-m_NoOfDeletions;}
	bool DestroyIfUnregistered()
	{
//		DestroyMembers();
		if(!m_IsRegistered)
		{
			delete this;
			return true;
		}
		return false;
	}
	bool IsSeparatedNode(int Node);
	RES_STATE CheckCompressionToNode();
	short ExtractMultiplier();
	void SetOwner(_CCircuit* pOwner);
	short GetSgn() {return m_pGraphTable->GetSgn();}
	void ReduceToNoOfNodes(unsigned NewNoOfNodes)
	{
		if(m_State!=IN_USE) return;
		m_pGraphTable->ReduceToNoOfNodes(NewNoOfNodes);
		m_pIntegrityTable->ReduceToNoOfNodes(NewNoOfNodes);
	}
	void FindTheSmallestIncidence(set<size_t>& RSet,set<size_t>& CSet, TWO_GRAPH_PINS& Pins, size_t MinPos, size_t MaxPos,size_t& TheSmallest, size_t& Repetition);
	unsigned NoOfDeletions() const {return m_NoOfDeletions; }
	void ForceSizeRejected() {m_State=TERM_SIZE_REJECTED;}
	void FreeTables()
	{
		pIntegrityTable=NULL;
		pGraphTable=NULL;
	}
protected:
	DECLARE_DYNAMIC_CREATION(_CGraphState);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	//void DestroyMembers()
	//{
	//	if(m_pGraphTable!=NULL)
	//		m_pGraphTable->DestroyIfUnregistered();
	//	if(m_pIntegrityTable!=NULL)
	//		m_pIntegrityTable->DestroyIfUnregistered();
	//}
	void ReleaseTables()
	{
		pIntegrityTable=NULL;
		pGraphTable=NULL;
	}
	RES_STATE TermSelfLoop()
	{
		ReleaseTables();
		return (m_State=TERM_SELFLOOP);
	}
	RES_STATE TermDisconnected()
	{
		ReleaseTables();
		return (m_State=TERM_SELFLOOP);
	}
	RES_STATE TermOne()
	{
		ReleaseTables();
		return (m_State=TERM_ONE);
	}
	virtual size_t DetermineHashKey(size_t Core=HASH_CORE) override;
	void Copy(const _CGraphState& Source, bool ForceNewTables);
	RES_STATE m_State;
	virtual void TouchDepended();
	virtual void UnTouchDepended();
	_CCircuit* m_pCircuitOwner;
	_CGraphTable* m_pGraphTable;
	_CIntegrityTable* m_pIntegrityTable;
	unsigned m_NoOfDeletions;
	friend class _CModelVertex;
};

class _CGraphStateCache : public _CStreamableCacheTmpl<_CGraphState>
{
public:
	~_CGraphStateCache()
	{
	}
	_CGraphStateCache():m_pOwner(NULL) {}
	void SetOwner(_CCircuit* pOwner);
	void DoSomeExtras(_streamable* pMe);
protected:
	DECLARE_DYNAMIC_CREATION(_CGraphStateCache);
	_CCircuit* m_pOwner;
};

//class _CCofactor
//{
//protected:
//	_CGraphState* m_MainGraphState;
//};

//class _CCofactorsList : public list<_CGraphState*>
//{
//};

