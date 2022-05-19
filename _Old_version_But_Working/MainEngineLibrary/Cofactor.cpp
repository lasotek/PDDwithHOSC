#include "StdAfx.h"
#include "Cofactor.h"
#include "Circuit.h"
#include "Vertex.h"
using namespace std;
using namespace stdext;

//**************************_CDeletion********************************************************
_CDeletion::_CDeletion(_CCircuit* pOwner,int p, int r, int k, int l):_CNullor(pOwner,p,r,k,l)
{
	if(abs(m_p)<abs(m_r))
	{
		swap(m_p,m_r);
		swap(m_k,m_l);
		if(m_p<0)
		{
			m_p=-m_p;
			m_r=-m_r;
		}
		if(m_k<0)
		{
			m_k=-m_k;
			m_l=-m_l;
		}
	}
}
_CDeletion::_CDeletion(const _CDeletion& Source,bool Transpose):
_CNullor(Source.m_pBaseCircuit,
		 Transpose?Source.m_k:Source.m_p,
		 Transpose?Source.m_l:Source.m_r,
		 Transpose?Source.m_p:Source.m_k,
		 Transpose?Source.m_r:Source.m_l)
{
}


char _CDeletion::Comp(const _CDeletion& Oper) const
{
	if(abs(m_p)<abs(Oper.m_p)) return -1;
	if(abs(m_p)>abs(Oper.m_p)) return +1;
	if(abs(m_k)<abs(Oper.m_k)) return -1;
	if(abs(m_k)>abs(Oper.m_k)) return +1;
	if(abs(m_r)<abs(Oper.m_r)) return -1;
	if(abs(m_r)>abs(Oper.m_r)) return +1;
	if(abs(m_l)<abs(Oper.m_l)) return -1;
	if(abs(m_l)>abs(Oper.m_l)) return +1;
	return 0;
}
bool _CDeletion::ChangeSign()
{
	if(m_p>0 && m_r>0)
	{
		swap(m_p,m_r);
		return true;
	}
	if(m_k>0 && m_l>0)
	{
		swap(m_k,m_l);
		return true;
	}
	return false;
}

int _CDeletion::IsCoincidentedWithShortCircuit(int HotNode, int ColdNode) const
{
	int Sgn=1,lSgn=1;
	if(HotNode==m_p && ColdNode==m_r)
		lSgn=1;
	else if(HotNode==m_r && ColdNode==m_p)
		lSgn=-1;
	else
		lSgn=0;
	Sgn*=lSgn;
	if(Sgn==0) return 0;
	if(HotNode==m_k && ColdNode==m_l)
		lSgn=1;
	else if(HotNode==m_l && ColdNode==m_k)
		lSgn=-1;
	else
		lSgn=0;
	Sgn*=lSgn;	
	return Sgn;
}

bool _CDeletion::AddMe(_CGraphTable* pTable)
{
	int p=m_p, r=m_r, k=m_k, l=m_l;
	if(pTable->ReadRealVlues(p,r,k,l)==TERM_SELFLOOP)
		return false;
	pTable->AddDeletion(p,r,k,l);
	return true;
}

void _CDeletion::WriteDeletionToTextStream(iostream& stream)
{
	stream<<"(";
	stream<<m_pBaseCircuit->ConvertNode(m_p);
	if(m_r>=0)
		stream<<"+";
	stream<<m_pBaseCircuit->ConvertNode(m_r)<<")("<<m_pBaseCircuit->ConvertNode(m_k);
	if(m_l>=0)
		stream<<"+";
	stream<<m_pBaseCircuit->ConvertNode(m_l)<<")";
}
_CDeletion::operator string() const
{
	char sgnr=m_r<0?'-':'+';
	char sgnc=m_l<0?'-':'+';
	char Buf[20];
	sprintf_s(Buf,20,"(%s%c%s)(%s%c%s)",m_pBaseCircuit->ConvertNode(m_p).c_str(),sgnr,
		m_pBaseCircuit->ConvertNode(m_r).c_str(),m_pBaseCircuit->ConvertNode(m_k).c_str(),sgnc,
		m_pBaseCircuit->ConvertNode(m_l).c_str());
	return Buf;
}

IMPLEMENT_DYNAMIC_CREATION(_CDeletion);
//**************************_CDeletion*********************************************************

//**************************_CDeletions********************************************************
_CDeletions::~_CDeletions()
{
	Clear();
}
void _CDeletions::Clear()
{
	for(iterator it=begin();it!=end();it++)
		delete (*it);
	clear();
}

_CDeletions::iterator _CDeletions::push_in_order(_CDeletion* pDel)
{
	//pDel->SetOwnerCircuit(m_pOwner);
	iterator it=begin();
	while(it!=end() && (**it)<(*pDel))
		it++;
	return insert(it,pDel);
}

int _CDeletions::Comp(const _CDeletions& Oper) const
{
	const_iterator it1,it2;
	for(it1=begin(),it2=Oper.begin();it1!=end() && it2!=Oper.end();it1++,it2++)
		if((**it1)<(**it2))
			return -1;
		else if((**it1)>(**it2))
			return 1;
	if(it1!=end())
		return 1;
	else if(it2!=Oper.end())
		return -1;
	else if(m_Multiplier<Oper.m_Multiplier)
		return -1;
	else if(m_Multiplier>Oper.m_Multiplier)
		return 1;
	else return 0;
}
long _CDeletions::DetermineHashKey(long Core)
{
	Core*=HASH_FACTOR;
	Core^=size();
	for(iterator it=begin();it!=end();it++)
	{
		Core*=HASH_FACTOR;
		Core=(*it)->GetHash(Core);
	}
	Core*=HASH_FACTOR;
	Core^=m_Multiplier;
	return Core;
}

bool _CDeletions::ChangeSign()
{
	ASSERTPDD(size()>0);
	iterator it=begin();
	while(it!=end() && !(*it)->ChangeSign())
		++it;
	if(it==end())
		return false;
	else
		return true;
}

void _CDeletions::Store(_binary_filer& Filer)
{
	_CHashable::Store(Filer);
	Filer<<size();
	for(iterator it=begin();it!=end();it++)
		Filer<<**it;
	Filer<<m_Multiplier;
	_streamable::Store(Filer);
}

void _CDeletions::Load(_binary_filer& Filer)
{
	Clear();
	_CHashable::Load(Filer);
	size_type size;
	Filer>>size;
	for(size_type i=0;i<size;i++)
	{
		_CDeletion* pDel=dynamic_cast<_CDeletion*>(Filer.RetriveDynamicClass());
		pDel->SetOwnerCircuit(m_pOwner);
		push_back(pDel);
	}
	Filer>>m_Multiplier;
	_streamable::Load(Filer);
}

void _CDeletions::SetOwner(_CCircuit* pOwner)
{
	if (m_pOwner != NULL && m_pOwner != pOwner)
		RISEPDD(eIllegalOperation, "_CDeletions::SetOwner: redefinition of the owner");
	m_pOwner=pOwner;
	for (auto& pDel : (*this))
		pDel->SetOwnerCircuit(pOwner);
}

bool _CDeletions::ContainNode(int Node)
{
	bool ContainRow = false,
		ContainCol = false;
	ABS(Node);
	for each (_CDeletion* pDel in *this)
	{
		if (!ContainRow && (abs(pDel->m_p) == Node || abs(pDel->m_r) == Node))
			ContainRow = true;
		if (!ContainCol && (abs(pDel->m_k) == Node || abs(pDel->m_l) == Node))
			ContainCol = true;
		if (ContainCol && ContainRow)
			return true;
	}
	return false;
}
//bool _CDeletions::ContainNode(int Node)
//{
//	bool ContainRow = false,
//		ContainCol = false;
//	ABS(Node);
//	for each (_CDeletion* pDel in *this)
//	{
//		if (!ContainRow && (abs(pDel->m_p) == Node || abs(pDel->m_r) == Node))
//			ContainRow = true;
//		if (!ContainCol && (abs(pDel->m_k) == Node || abs(pDel->m_l) == Node))
//			ContainCol = true;
//		if (ContainCol && ContainRow)
//			return true;
//	}
//	return false;
//}
bool _CDeletions::ContainNodes(_CIntNodes Nodes)
{
	for each(int Node in Nodes)
		if(!ContainNode(Node))
			return false;
	return true;
}

bool _CDeletions::AddDeletions(_CGraphTable* pTable)
{
	for each (_CDeletion* pDel in *this)
		if(!pDel->AddMe(pTable))
			return false;
	return true;
}
void _CDeletions::ResetSgn(_CGraphTable* pTable)
{
	pTable->MultiplySgn(m_Multiplier);
}

void _CDeletions::WriteToTextStream(iostream& stream)
{
	if(m_Multiplier!=1)
		if(m_Multiplier==-1)
			stream<<"-";
		else
			stream<<""<<m_Multiplier<<"*";
	stream<<"D";
	bool First=true;
	for each(_CDeletion* pDel in *this)
	{
		if(First)
			First=false;
		else
			stream<<",";
		pDel->WriteDeletionToTextStream(stream);
	}
}

void _CMultiDeletions::WriteToTextStream(iostream& stream)
{
	bool First = true;
	for (auto& DelsPair : *this)
	{
		if (First)
			First = false;
		else
			stream << " || ";
		DelsPair.second.WriteToTextStream(stream);
	}
}



_CDeletions::operator string() const
{
	string res;
	size_t _size=size(),i=0;
	const_iterator it=begin(),_end=end();
	for(;it!=_end;i++,it++)
	{
		res+=(string)(**it);
		if(i<(_size-1))
			res+=',';
	}
	return res;
}

void /*int*//*Sgn changes*/ _CDeletions::CopyAndForceSuperposition(int ForcedSgn,_CDeletions& Source,unsigned char Operation,
		int HotInput, int ColdInput, int HotOutput, int ColdOutput)
{
	//int Sgn=1;
	clear();
	for(iterator it=Source.begin(),_e=Source.end();it!=_e;it++)
	{
		_CDeletion* pDel=*it;
		if(Operation & SUPERPOSE_SUBSTITUTE_INPUT)
		{
			int InSgn=pDel->IsCoincidentedWithShortCircuit(HotInput,ColdInput);
			if(InSgn!=0)
			{
				ForcedSgn*=InSgn;
				continue;
			}
		}
		if(Operation & SUPERPOSE_SUBSTITUTE_OUTPUT)
		{
			int OutSgn=pDel->IsCoincidentedWithShortCircuit(HotOutput,ColdOutput);
			if(OutSgn!=0)
			{
				ForcedSgn*=OutSgn;
				continue;
			}
		}
		_CDeletion* pNewDeletin=new _CDeletion(*pDel);
		if(pNewDeletin==NULL)
			throw bad_alloc();
		push_in_order(pNewDeletin);
	}
	if(Operation & SUPERPOSE_ADD_GATES)
	{
		_CDeletion* pNewDeletin=new _CDeletion(m_pOwner,HotInput,ColdInput,HotOutput,ColdOutput);
		if(pNewDeletin==NULL)
			throw bad_alloc();
		push_in_order(pNewDeletin);
	}
	m_Multiplier=ForcedSgn;
	//return Sgn;
}

void _CDeletions::CopyTranspose(bool OppositeSgn,_CDeletions& Source)
{
	Clear();
	m_Multiplier=OppositeSgn?-Source.m_Multiplier:Source.m_Multiplier;
	for each(_CDeletion* pDeletion in Source)
	{
		_CDeletion* pNewDeletion=new _CDeletion(*pDeletion,true);
		push_back(pNewDeletion);
	}
}
bool _CDeletions::IsSymetric() const
{
	for each(const _CDeletion* pDeletion in (*this))
		if(!pDeletion->IsSymetric())
			return false;
	return true;
}

_CDeletions::_CDeletions(const _CDeletions& Source,bool Transpose):m_pOwner(Source.m_pOwner),
m_Multiplier(Source.m_Multiplier)
{
	for each(_CDeletion* pDeletion in Source)
	{
		_CDeletion* pNewDeletion=new _CDeletion(*pDeletion,Transpose);
		push_back(pNewDeletion);
	}
}

IMPLEMENT_DYNAMIC_CREATION(_CDeletions);
//**************************_CDeletions********************************************************

//**************************_CMuliDeletions********************************************************
_CMultiDeletions::operator string() const
{
	bool First = true;
	string Res;
	for (auto& DelsPair : *this)
	{
		if (First)
			First = false;
		else
			Res += " || ";
		Res += (string)DelsPair.second;
	}
	return Res;
}
long _CMultiDeletions::DetermineHashKey(long Core)
{
	for (auto DelsPair : *this)
	{
		Core *= HASH_FACTOR;
		Core ^= DelsPair.first;
		Core = DelsPair.second.DetermineHashKey(Core);
	}
	return Core;
}

void _CMultiDeletions::Store(_binary_filer& Filer)
{
	Filer << size();
	for (auto DelsPair : *this)
	{
		Filer << DelsPair.first;
		Filer << DelsPair.second;
	}
}

void _CMultiDeletions::Load(_binary_filer& Filer)
{
	size_t Size=0;
	Filer >> Size;
	for (size_t i = 0; i < Size; i++)
	{
		size_t Index = 0;
		Filer >> Index;
		_CDeletions& Dels=insert(value_type(Index,_CDeletions(m_pOwner))).first->second;
		Filer >> Dels;
	}
}

size_t _CMultiDeletions::GetRank() const
{
	size_t Size = 0;
	for (auto& PairDesc : (*this))
		Size = max(Size, PairDesc.second.GetRank());
	return Size;
}

_CDeletions::iterator _CMultiDeletions::push_in_order(_CDeletion* pDel, size_t Level)
{
	pDel->SetOwnerCircuit(m_pOwner);
	_CDeletions& Dels = (*this)[Level];
	if (Dels.GetOwner() == nullptr)
		Dels.SetOwner(m_pOwner);
	return Dels.push_in_order(pDel);
}

void _CMultiDeletions::SetOwner(_CCircuit* pOwner)
{ 
	m_pOwner = pOwner; 
	for (auto& DelsPair : (*this))
		DelsPair.second.SetOwner(pOwner);
}

bool _CMultiDeletions::IsSymetric() const
{
	for (auto& DelsPair : (*this))
		if (!DelsPair.second.IsSymetric())
			return false;
	return true;
}
IMPLEMENT_DYNAMIC_CREATION(_CMultiDeletions);

//**************************_CMuliDeletions********************************************************

IMPLEMENT_DYNAMIC_CREATION(_CCofactor);

_CCofactor::_CCofactor(_CCircuit* pOwner) : m_pOwnerCircuit(pOwner),m_Deletions(pOwner),m_pMainModelVertex(NULL),m_GlobalMultiplier(1), m_NextFreeContextPos(-1)
{
}

_CCofactor::~_CCofactor(void)
{
	if(m_pMainModelVertex!=NULL)
		m_pMainModelVertex->UnTouchMe();
}

void _CCofactor::SetCircuitOwner(_CCircuit* pOwner)
{
	if(m_pOwnerCircuit!=NULL)
		RISEPDD(eIllegalOperation,"_CCofactor::SetCircuitOwner: circuit owner redefinition prohibited");
	m_pOwnerCircuit=pOwner;
	m_Deletions.SetOwner(pOwner);
}

void _CCofactor::AddDeletions(string sp, string sr, string sk, string sl, size_t Level)
{
	if(sp.size()==0) RISEPDD(eIllegalOperation,"\"p\" parameter cannont be empty");
	if(sr.size()==0) RISEPDD(eIllegalOperation,"\"r\" parameter cannont be empty");
	if(sk.size()==0) RISEPDD(eIllegalOperation,"\"k\" parameter cannont be empty");
	if(sl.size()==0) RISEPDD(eIllegalOperation,"\"l\" parameter cannont be empty");
	bool RowsNegative=false,ColumnsNegative=false;
	if(sr[0]=='-')
	{
		RowsNegative=true;
		sr=&sr.c_str()[1];
	}
	if(sl[0]=='-')
	{
		ColumnsNegative=true;
		sl=&sl.c_str()[1];
	}
	int p=m_pOwnerCircuit->TranslateOrAddNode(sp),
		r=m_pOwnerCircuit->TranslateOrAddNode(sr),
		k=m_pOwnerCircuit->TranslateOrAddNode(sk),
		l=m_pOwnerCircuit->TranslateOrAddNode(sl);
	AddDeletions(p, r, k, l, RowsNegative, ColumnsNegative, Level);
}
bool _CCofactor::operator==(const _CCofactor& _right)
{
	return false;
}

void _CCofactor::AddDeletions(int p, int r, int k, int l, bool RowsNegative, bool ColumnsNegative, size_t Level)
{
	_CDeletion* pDeletion=new _CDeletion(m_pOwnerCircuit,p,RowsNegative?-r:r,k,ColumnsNegative?-l:l);
	if(pDeletion==NULL)
		throw bad_alloc();
	auto& Dels = m_Deletions[Level];
	if (Dels.GetOwner()==nullptr)
		Dels.SetOwner(m_pOwnerCircuit);
	Dels.push_in_order(pDeletion);
}

void _CCofactor::CopyAndForceSuperposition(_CCofactor& Source,unsigned char Operation,
		int HotInput, int ColdInput, int HotOutput, int ColdOutput,short Sgn)
{
	m_GlobalMultiplier=Source.m_GlobalMultiplier/**Sgn*/;
	//if(m_GlobalMultiplier!=1)
	//	m_DefGlobalMultiplierStr="("+long2str(m_GlobalMultiplier)+")*";
	for (auto& DelPair : Source.m_Deletions)
		m_Deletions[DelPair.first].CopyAndForceSuperposition(Sgn, Source.m_Deletions[DelPair.first], Operation, HotInput, ColdInput, HotOutput, ColdOutput);
	///*m_GlobalMultiplier*=*/m_Deletions.CopyAndForceSuperposition(Sgn,Source.m_Deletions,Operation,HotInput,ColdInput,HotOutput,ColdOutput);
}
void _CCofactor::CopyTranposed(_CCofactor& Source,bool OppositeSgn)
{
	m_GlobalMultiplier=/*OppositeSgn?-Source.m_GlobalMultiplier:*/Source.m_GlobalMultiplier;
	//if(m_GlobalMultiplier!=1)
	//	m_DefGlobalMultiplierStr="("+long2str(m_GlobalMultiplier)+")*";
	for (auto& DelPair : Source.m_Deletions)
		m_Deletions[DelPair.first].CopyTranspose(OppositeSgn, DelPair.second);
	//m_Deletions.CopyTranspose(OppositeSgn,Source.m_Deletions);
}

void _CCofactor::PrepareCofactor(const _CModelVertex& TopVertex)
{
	_CModelVertex* pCoffMainVertex=new _CModelVertex(TopVertex.GetInputState(),m_pOwnerCircuit,true);
	if(pCoffMainVertex==NULL)
		throw bad_alloc();
	try
	{
		if (m_Deletions.size() > 1)
			RISEPDD(eWrongCondition, "Obsolate function does not allow multilevel cofactors");
		for(DEL_ITERATOR it=m_Deletions[0].begin();it!=m_Deletions[0].end();it++)
			(*it)->ProcessCMVertex(*pCoffMainVertex,m_pOwnerCircuit->NoOfNodes());
		pCoffMainVertex->GetInputState()->TestState();
		m_GlobalMultiplier=pCoffMainVertex->GetInputState()->ExtractMultiplier();
		pMainModelVertex=m_pOwnerCircuit->GetRegisteredMVertex(pCoffMainVertex);
	}
	catch(...)
	{
		pCoffMainVertex->DestroyIfUnregistered();
		throw;
	}
}

void _CCofactor::SetMainVertex(const string* Context,_CModelVertex* pModelVertex)
{
	if(Context==&EmptyString)
	{
		if(m_pMainModelVertex==pModelVertex)
			return;
		if(m_pMainModelVertex!=NULL)
			m_pMainModelVertex->UnTouchMe();
		m_pMainModelVertex=pModelVertex;
		if(m_pMainModelVertex!=NULL)
			m_pMainModelVertex->TouchMe();
	}
	else
	{
		_CModelVertex*& pModelV=m_ContextMainModelVertices[Context];
		if(pModelV==pModelVertex)
			return;
		if(pModelV!=NULL)
			pModelV->UnTouchMe();
		pModelV=pModelVertex;
		if(pModelV!=NULL)
			pModelV->TouchMe();
	}
}

_CModelVertex* _CCofactor::GetMainVertex(const string* Context) const 
{
	if(Context==&EmptyString)
		return m_pMainModelVertex;
	else
	{
		CONTEXT_MAIN_MODEL_VERTEX::const_iterator it=m_ContextMainModelVertices.find(Context);
		if(it==m_ContextMainModelVertices.end())
			return NULL;
		else
			return it->second;
	}
}

void _CCofactor::StoreContext(_binary_filer& Filer, CONTEXT_iterator cit)
{
	Filer.end();
	Filer<<*(cit->first);
	Filer<<m_pOwnerCircuit->GetModelVertexIndex(cit->second);
	long long NextFreeContextPos=Filer.pos();
	long long M1=-1;
	Filer<<M1;
	long long TempRetard=Filer.pos();
	Filer.set(m_NextFreeContextPos);
	Filer<<NextFreeContextPos;
	m_NextFreeContextPos=NextFreeContextPos;
	Filer.set(TempRetard);
}

void _CCofactor::StoreContext(_binary_filer& Filer,const string* Context)
{
	CONTEXT_iterator it=m_ContextMainModelVertices.find(Context);
	if(it!=m_ContextMainModelVertices.end())
		StoreContext(Filer,it);
}

void _CCofactor::Store(_binary_filer& Filer)
{
	Filer<<m_GlobalMultiplier;
	Filer<<m_Deletions;
	long long Index=m_pOwnerCircuit->GetModelVertexIndex(m_pMainModelVertex);
	Filer<<Index;
	Filer<<m_ContextMainModelVertices.size();
	for(CONTEXT_iterator it=m_ContextMainModelVertices.begin(),_e=m_ContextMainModelVertices.end();
		it!=_e;it++)
	{
		Filer<<*(it->first);
		Filer<<m_pOwnerCircuit->GetModelVertexIndex(it->second);
	}
	m_NextFreeContextPos=Filer.pos();
	long long p=-1;
	Filer<<p;
	_streamable::Store(Filer);
}

void _CCofactor::LoadContext(_binary_filer& Filer)
{
	string s;
	Filer>>s;
	const string* cps=m_pOwnerCircuit->GetUniqueContextStringName(&s);
	long long Index;
	Filer>>Index;
	_CModelVertex* pVertex=m_pOwnerCircuit->GetModelVertexFromIndex(Index);
	m_ContextMainModelVertices.insert(CONTEXT_value_type(cps,pVertex));
}
void _CCofactor::Load(_binary_filer& Filer)
{
	Filer>>m_GlobalMultiplier;
	Filer>>m_Deletions;
	long long Index;
	Filer>>Index;
	m_pMainModelVertex=m_pOwnerCircuit->GetModelVertexFromIndex(Index);
	size_t _s=0;
	Filer>>_s;
	m_ContextMainModelVertices.clear();
	for(size_t i=0;i<_s;i++)
		LoadContext(Filer);
	m_NextFreeContextPos=Filer.pos();
	long long NextPos=-1;
	Filer>>NextPos;
	long long TmpEnd=Filer.pos();
	while(NextPos>0)
	{
		Filer.set(NextPos);
		LoadContext(Filer);
		m_NextFreeContextPos=Filer.pos();
		Filer>>NextPos;
	}
	Filer.set(TmpEnd);
	_streamable::Load(Filer);
}

void _CCofactor::Skip0Suppressed(/*bool CutLeaves*/)
{
	if(m_pMainModelVertex==NULL)
		return;
	m_pMainModelVertex->Skip0Suppressed(/*CutLeaves*/);
	int HollowOutput;
	if(m_pMainModelVertex->IsDegenerated(HollowOutput))
	{
		m_GlobalMultiplier*=m_pMainModelVertex->GetMultiplier(HollowOutput);
		pMainModelVertex=m_pMainModelVertex->GetDescendant(HollowOutput);
	}
}
void _CCofactor::WriteToTextStreamModel(iostream& stream)
{
	m_Deletions.WriteToTextStream(stream);
	stream<<" Vertex: ";
	long long iVertex=m_pOwnerCircuit->GetModelVertexIndex(m_pMainModelVertex);
	if(m_GlobalMultiplier!=1 && iVertex!=0)
		stream<<"("<<m_GlobalMultiplier<<") * ";
	stream<<iVertex<<endl;
}

void _CCofactor::WriteToTextStream(iostream& stream,_CSimpleVerticesMapsSet& Set)
//void _CCofactor::WriteToTextStream(iostream& stream)
{
	m_Deletions.WriteToTextStream(stream);
	stream<<" Vertex: \n";
	//m_pOwnerCircuit->GetSVerticesForCofactor(this).WriteToStream(stream,Set);
	m_pOwnerCircuit->EnumCofoctorForContexts(this,stream,Set);
	//m_pOwnerCircuit->EnumCofoctorForContexts(this,stream);
	stream<<"\n"; 
}

size_t _CCofactor::GetMaxSPower()
{
	try
	{
		return m_pOwnerCircuit->GetSVerticesForCofactor(this).size()-1;
	}
	catch(out_of_range)
	{
		return 0;
	}
}

void _CCofactor::GetDeletions(_CMultiBasicSetOfDeletions& MultSetOfDels)
{
	MultSetOfDels.clear();
	for (auto& PairDel : m_Deletions)
		MultSetOfDels.push_back(PairDel.second);
}

void _CCofactor::Skip0SupressedAndDuplicated(_CDuplicatedVertexCatcher& Catcher)
{
	pMainModelVertex->Skip0SupressedAndDuplicated(Catcher);
	{
		_CModelVertex* pNewVertex=NULL;
		short Sgn=1;
		Catcher.GetUnique(pMainModelVertex,pNewVertex,Sgn);
		if(pNewVertex!=pMainModelVertex)
		{
			pMainModelVertex=pNewVertex;
			//m_GlobalMultiplier*=Sgn;
		}
		m_GlobalMultiplier*=Sgn;
	}
	int HollowOutput;
	if(pMainModelVertex->IsDegenerated(HollowOutput))
	{
		_CModelVertex* pNewVertex=m_pMainModelVertex->GetDescendant(HollowOutput);
		m_GlobalMultiplier*=m_pMainModelVertex->GetMultiplier(HollowOutput);
		pMainModelVertex=pNewVertex;
		//short NewMult=m_pDescendants[i].m_Multiplier*pDescVertex->GetMultiplier(1);
		//_CModelVertex* pNewDesc=pDescVertex->GetDescendant(1);
		////pDescVertex->SetDescendant(1,NULL,0);
		//SetDescendant(i,pNewDesc,NewMult);
		//pDescVertex=pNewDesc;
	}
	if(pMainModelVertex->IsEmptyMacro())
	{
		pMainModelVertex=GetGlobal0Vertex();
	}
}


//*************************_CBounderyParametersCache****************************
void _CBounderyParametersCache::DoSomeExtras(_streamable *pMe)
{
	dynamic_cast<_CMultiDeletions*>(pMe)->SetOwner(m_pOwner);
}

void _CBounderyParametersCache::SetOwner(_CCircuit* pOwner)
{
	if(m_pOwner!=NULL)
		RISEPDD(eIllegalOperation,"_CBounderyParametersCache::SetOwner - Redefinition of the owner circuit prohibited");
	m_pOwner=pOwner;
}

bool _CBounderyParametersCache::DeletionsAreForced() const
{
	for(HASH2OBJECT_MAP::const_iterator it=m_Hash2ObjectMap.begin(),_e=m_Hash2ObjectMap.end();it!=_e;it++)
		if(it->second.second->empty())
			return false;
	return true;
}

IMPLEMENT_DYNAMIC_CREATION(_CBounderyParametersCache);

//*************************_CBounderyParametersCache****************************

_CCofactorsVector::~_CCofactorsVector()
{
	for(iterator it=begin();it!=end();it++)
		if((*it)!=NULL)
			delete (*it);
}

void _CCofactorsVector::SetOwner(_CCircuit* NewOwner)
{
	if(m_OwnerCircuit!=NULL)
		RISEPDD(eIllegalOperation,"_CCofactorsVector::SetOwner - Redefinition of the owner circuit prohibited");
	m_OwnerCircuit=NewOwner;
}
void _CCofactorsVector::DoSomeExtras(_streamable* pstrObj)
{
	dynamic_cast<_CCofactor*>(pstrObj)->SetCircuitOwner(m_OwnerCircuit);
}

void _CCofactorsVector::Store(_binary_filer& Filer)
{
	Filer<<size();
	for(iterator it=begin();it!=end();it++)
		Filer<<**it;
	_streamable::Store(Filer);
}

void _CCofactorsVector::Load(_binary_filer& Filer)
{
	clear();
	size_type size;
	Filer>>size;
	resize(size);
	for(size_type i=0;i<size;i++)
		at(i)=dynamic_cast<_CCofactor*>(Filer.RetriveDynamicClass(this));
	_streamable::Load(Filer);
}

IMPLEMENT_DYNAMIC_CREATION(_CCofactorsVector);