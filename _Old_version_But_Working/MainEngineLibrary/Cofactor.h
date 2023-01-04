#pragma once
#include "StateGraphTables.h"
#include "IdealComponents.h"
#include "CacheTmpl.h"
#include <list>
#include <vector>
#include "Vertex.h"
#include "StrOperIdentifier.h"

class _CCircuit;
class _CDeletion : public _CNullor
{
public:
	~_CDeletion()
	{
	}
	_CDeletion(_CCircuit* pOwner=NULL):_CNullor(pOwner) {}
	_CDeletion(_CCircuit* pOwner,int p, int r, int k, int l);
	_CDeletion(const _CDeletion& Source,bool Transpose=false);
	bool operator==(const _CDeletion& Oper) const 
	{ 
		return m_p==Oper.m_p && m_r==Oper.m_r && m_k==Oper.m_k && m_l==Oper.m_l; 
	}
	//bool operator==(const _CDeletion& Oper) const { return Comp(Oper) == 0; }
	bool operator<(const _CDeletion& Oper) const { return Comp(Oper)<0; }
	bool operator>(const _CDeletion& Oper) const {return Comp(Oper)>0;}
	bool operator<=(const _CDeletion& Oper) const {return Comp(Oper)<=0;}
	bool operator>=(const _CDeletion& Oper) const {return Comp(Oper)>=0;}
	bool ChangeSign();
	bool AddMe(_CGraphTable* pTable);
	RES_STATE AddDeletion(_CGraphState* pState) 
	{
		return pState->AddDeletion(m_p,m_r,m_k,m_l);
	}
	void WriteDeletionToTextStream(iostream& stream);
	operator string() const;
	int IsCoincidentedWithShortCircuit(int HotNode, int ColdNode) const;//0 - not, +-1 sgn
	bool IsSymetric() const {return m_p==m_k && m_r==m_l;}
	int P() const {return m_p;}
	int R() const {return m_r;}
	int K() const {return m_k;}
	int L() const {return m_l;}
protected:
	void WriteType(iostream& stream) {}//To make
	DECLARE_DYNAMIC_CREATION(_CDeletion);
	char Comp(const _CDeletion& Oper) const;
	friend class _CDeletions;
	friend class _CSubCircuitSocket;
};

//typedef list<_CDeletion*> LIST_OF_DELETION;
//typedef list<LIST_OF_DELETION> MULTI_LIST_OF_FELETIONS;

class _CDeletions : public list<_CDeletion*>, public _streamable, public _CHashable
//class _CDeletions : public MULTI_LIST_OF_FELETIONS, public _streamable, public _CHashable
{
public:
	_CDeletions(_CCircuit* pOwner=NULL):m_pOwner(pOwner),m_Multiplier(1) {}
	_CDeletions(const _CDeletions& Source,bool Transpose=false);
	~_CDeletions();
	iterator push_in_order(_CDeletion* Del);
	bool operator==(const _CDeletions& Oper) const 
	{ 
		return m_Multiplier == Oper.m_Multiplier && 
			*(const list<_CDeletion*>*)this == *(const list<_CDeletion*>*)&Oper;
	}
	//bool operator==(const _CDeletions& Oper) const { return Comp(Oper) == 0; }
	bool operator>(const _CDeletions& Oper) const { return Comp(Oper)>0; }
	bool operator<(const _CDeletions& Oper) const {return Comp(Oper)<0;}
	bool operator>=(const _CDeletions& Oper) const {return Comp(Oper)>=0;}
	bool operator<=(const _CDeletions& Oper) const {return Comp(Oper)<=0;}
//	operator long() {return GetHash();}
	virtual size_t DetermineHashKey(size_t Core=HASH_CORE) override;
	bool ChangeSign();
	void Clear();
	void SetOwner(_CCircuit* pOwner);
	_CCircuit* GetOwner() {return m_pOwner;}
	void SetMultiplier(short Multiplier) {m_Multiplier=Multiplier;}
	bool ContainNode(int Node);
	bool ContainNodes(_CIntNodes Nodes);
	bool AddDeletions(_CGraphTable* pTable);
	void ResetSgn(_CGraphTable* pTable);
	void WriteToTextStream(iostream& stream);
	operator string() const;
	enum {SUPERPOSE_SUBSTITUTE_INPUT=0x1,SUPERPOSE_SUBSTITUTE_OUTPUT=0x2,SUPERPOSE_ADD_GATES=0x4};
	void /*int*//*Sgn changes*/ CopyAndForceSuperposition(int ForcedSgn,_CDeletions& Source,unsigned char Operation,
		int HotInput, int ColdInput, int HotOutput, int ColdOutput);
	void CopyTranspose(bool OppositeSgn,_CDeletions& Source);
	bool IsSymetric() const;
	unsigned GetRank() const {return (unsigned)size();}
	short Sgn() const {return m_Multiplier;}
protected:
	void DestroyIfUnregistered()
	{
		if(!m_IsRegistered)
			delete this;
	}
	DECLARE_DYNAMIC_CREATION(_CDeletions);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);

	int Comp(const _CDeletions& Oper) const;
	friend class _CSubCircuit;
	_CCircuit* m_pOwner;
	short m_Multiplier;
};

class _CMultiDeletions : public map <size_t, _CDeletions >, public _streamable, public _CHashable
{
public:
	_CMultiDeletions(_CCircuit* pOwner = nullptr):m_pOwner(pOwner) 
	{
		insert(value_type(0, _CDeletions(pOwner)));
	}
	void WriteToTextStream(iostream& stream);
	explicit operator string() const;
	virtual size_t DetermineHashKey(size_t Core = HASH_CORE) override;
	void SetOwner(_CCircuit* pOwner);
	unsigned GetRank() const;
	_CCircuit* GetOwner() { return m_pOwner; }
	_CDeletions::iterator push_in_order(_CDeletion* pDel, size_t Level = 0);
	bool IsSymetric() const;
protected:
	void DestroyIfUnregistered()
	{
		if (!m_IsRegistered)
			delete this;
	}
	DECLARE_DYNAMIC_CREATION(_CMultiDeletions);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	_CCircuit* m_pOwner;
	friend class _CSubCircuit;
};

class _CCofactor : public _streamable, public _CStrOperIdentifier
{
public:
	_CCofactor(_CCircuit* pOwner=NULL);
	~_CCofactor(void);
	void CopyAndForceSuperposition(_CCofactor& Source,unsigned char Operation,
		int HotInput, int ColdInput, int HotOutput, int ColdOutput,short Sgn=1);
	void CopyTranposed(_CCofactor& Source,bool OppositeSgn);
	void AddDeletions(string p, string r, string k, string l, size_t Level = 0);
	void AddDeletions(int p, int r, int k, int l, bool RowsNegative, bool ColumnsNegative,size_t Level);
	void PrepareCofactor(const _CModelVertex& TopVertex);
	bool operator==(const _CCofactor& _right);
	void SetCircuitOwner(_CCircuit* pOwner);
	void Skip0Suppressed(/*bool CutLeaves*/);
	//_CSExpandedVertices& SetMainSimpleVertices(_CSExpandedVertices& Vertices) 
	//{
	//	m_MainVertices=Vertices;
	//	for each(_CSimpleVertexContainer Vert in m_MainVertices)
	//		Vert.MultiplyMultiplier(m_GlobalMultiplier);
	//	return m_MainVertices;
	//}
	_CModelVertex* GetMainVertex(const string* Context) const;
	_CModelVertex* GetMainVertex() const 
	{
		return GetMainVertex(&EmptyString);
	}
	//_CSExpandedVertices& GetMainSVertices() {return m_MainVertices;}
	void WriteToTextStream(iostream& stream,_CSimpleVerticesMapsSet& Set);
	//void WriteToTextStream(iostream& stream);
	void WriteToTextStreamModel(iostream& stream);
	//void SkipDuplicated(_CDuplicatedVertexCatcher& Catcher)
	//{
	//	m_pMainModelVertex->SkipDuplicated(Catcher);
	//}
	void Skip0SupressedAndDuplicated(_CDuplicatedVertexCatcher& Catcher);
	//{
	//	m_pMainModelVertex->Skip0SupressedAndDuplicated(Catcher);
	//}
	void MultSgn(short Mult) {m_GlobalMultiplier*=Mult;} 
	size_t GetMaxSPower();
	void WriteCofactorDescription(iostream& stream) {m_Deletions.WriteToTextStream(stream);}
	inline short GetGlobalMultiplier() const {return m_GlobalMultiplier;}
	unsigned long GetRank() {return (unsigned long)m_Deletions.size();}
	bool IsZero() const 
	{
		return (m_pMainModelVertex!=NULL && m_pMainModelVertex->Is0());
	}
	void ResetDeletions()
	{
		m_Deletions.clear();
		m_GlobalMultiplier=1;
	}
	const _CMultiDeletions& GetDeletions() const { return m_Deletions; }
	void GetDeletions(_CMultiBasicSetOfDeletions& MultSetOfDels);
	//const _CDeletions& GetDeletions() const { return m_Deletions; }
protected:
	typedef unordered_map<const string*,_CModelVertex*> CONTEXT_MAIN_MODEL_VERTEX;
	typedef CONTEXT_MAIN_MODEL_VERTEX::iterator CONTEXT_iterator;
	typedef CONTEXT_MAIN_MODEL_VERTEX::value_type CONTEXT_value_type;
	virtual void CreateStrIdentyfier(string& StrId) {StrId="D"+(string)m_Deletions;}
	DECLARE_DYNAMIC_CREATION(_CCofactor);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	void LoadContext(_binary_filer& Filer);
	void StoreContext(_binary_filer& Filer,CONTEXT_iterator cit);
	void StoreContext(_binary_filer& Filer,const string* Context);
	typedef _CDeletions::iterator DEL_ITERATOR;
	_CMultiDeletions m_Deletions;
	//_CDeletions m_Deletions;
	_declspec(property (get=GetMainVertex,put=SetMainVertex)) _CModelVertex* pMainModelVertex;
	_declspec(property (get=GetMainVertex,put=SetMainVertex)) _CModelVertex* pContextMainModelVertex[];
	_CModelVertex* m_pMainModelVertex;
	CONTEXT_MAIN_MODEL_VERTEX m_ContextMainModelVertices;
	short m_GlobalMultiplier;
	//string m_DefGlobalMultiplierStr;
	void SetMainVertex(const string* Context,_CModelVertex* pModelVertex);
	void SetMainVertex(_CModelVertex* pModelVertex)
	{
		SetMainVertex(&EmptyString,pModelVertex);
	}
	_CCircuit* m_pOwnerCircuit;
	//_CVerticesMapCache m_VerMapCache;
	long long m_NextFreeContextPos;
	friend class _CCircuit;
};

//class _CBounderyParametersCache : public _CStreamableCacheTmpl<_CDeletions>
class _CBounderyParametersCache : public _CStreamableCacheTmpl<_CMultiDeletions>
	{
public:
	~_CBounderyParametersCache() 
	{
	}
	_CBounderyParametersCache(_CCircuit* pOwner=NULL):m_pOwner(pOwner) {}
	void SetOwner(_CCircuit* pOwner);
	bool DeletionsAreForced() const;
protected:
	void DoSomeExtras(_streamable *pMe);
	DECLARE_DYNAMIC_CREATION(_CBounderyParametersCache);
	_CCircuit* m_pOwner;
};

//class _CBoundaryParameters : public vector<_CDeletions*>, public _streamable
//{
//public:
//	_CBoundaryParameters(_CCircuit* pOwner=NULL):m_pOwner(pOwner) {}
//	void SetOwner(_CCircuit* pOwner);
//protected:
//	void Load(_binary_filer& Filer);
//	void Store(_binary_filer& Filer);
//	void DoSomeExtras(_streamable *pMe);
//	DECLARE_DYNAMIC_CREATION(_CBoundaryParameters);
//	_CCircuit* m_pOwner;
//};

class _CCofactorsVector : public vector<_CCofactor*>, public _streamable, public _co_streamable
{
public:
	_CCofactorsVector():m_OwnerCircuit(NULL) {}
	~_CCofactorsVector();
	void SetOwner(_CCircuit* NewOwner);
protected:
	DECLARE_DYNAMIC_CREATION(_CCofactorsVector);
	void DoSomeExtras(_streamable* pstrObj);
	_CCircuit* m_OwnerCircuit;
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
};
