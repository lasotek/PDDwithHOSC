#include "StdAfx.h"
#include "StateGraphTables.h"
#include "PrjException.h"
#include "Circuit.h"
#include "Components.h"
#include "IdealComponents.h"

//template<class Type>
//inline Type sgn(const Type& Value)
//{
//	return Value<0?-1:1;
//}

template<class Type>
inline int SgnAbs(Type& Value)
{
	if(Value<0)
	{
		Value=-Value;
		return -1;
	}
	else
		return 1;
}
//template<class Type>
//inline void ABS(Type& a)
//{
//	if(a<0) a=-a;
//}
template<class Type>
inline void MaxMin(Type& a, Type& b)
{
	if(a<b) swap(a,b);
}

template<class Type>
inline void MaxMinAbs(Type&a, Type&b)
{
	ABS(a); ABS(b);
	MaxMin(a,b);
}
template<class Type>
inline void MaxMinSgn(Type& a, Type& b, short& Sgn)
{
	if(abs(a)<abs(b))
	{
		swap(a,b);
		Sgn=-Sgn;
	}
}

template<class Type>
inline void OrderSgn(Type& a, Type& b, short& Sgn)
{
	if(a<0)
	{
		a=-a;
		b=-b;
		Sgn=-Sgn;
	}
}

template<class Type>
inline void DoubleSgn(Type& a, Type& b, short& Sgn)
{
	if(a==-b)
	{
		b=0;
		Sgn*=2;
	}
}
//************************_CListOfNodes********************************
void _CListOfNodes::Store(_binary_filer& Filer)
{
	Filer<<size();
	for each(int N in *this)
		Filer<<N;
	_streamable::Store(Filer);
}

void _CListOfNodes::Load(_binary_filer& Filer)
{
	unsigned Size;
	Filer>>Size;
	resize(Size);
	for(unsigned i=0;i<Size;i++)
		Filer>>at(i);
	_streamable::Load(Filer);
}

IMPLEMENT_DYNAMIC_CREATION(_CListOfNodes);
//************************_CListOfNodes********************************
void _CSparseListOfNodes::Store(_binary_filer& Filer)
{
	char cc=';';
	for(INT_INT_MAP::iterator it=begin(),_e=end();it!=_e;it++)
	{
		Filer<<it->first;
		Filer<<it->second;
		Filer<<cc;
	}
	int Term=numeric_limits<int>::min();
	Filer<<Term<<Term<<cc;
	_streamable::Store(Filer);
}

void _CSparseListOfNodes::Load(_binary_filer& Filer)
{
	int FValue=0;
	int SValue=0;
	char cc=0;
	int Term=numeric_limits<int>::min();
	clear();
	while(true)
	{
		Filer>>FValue;
		Filer>>SValue;
		Filer>>cc;
		if(cc!=';')
		{
			string str="File: "+Filer.Name()+" is corrupted";
			RISEPDD(eFilerProblem,str.c_str());
		}
		if(FValue==Term && SValue==Term)
			break;
		insert(INT_INT_MAP::value_type(FValue,SValue));
	}
	_streamable::Load(Filer);
}

IMPLEMENT_DYNAMIC_CREATION(_CSparseListOfNodes);
//**************_CComponentPredicator**********************************

_CComponentPredicator& _CComponentPredicator::operator=(const _CComponentPredicator& Source)
{
	m_NoOfNewAncestors=Source.m_NoOfNewAncestors;
	m_StepsToZero=Source.m_StepsToZero;
	m_RealNodes.assign(Source.m_RealNodes.begin(),Source.m_RealNodes.end());
	return *this;
}

//**************_CListOfNodes**********************************
long _CListOfNodes::GetHashValue(long Core)
{
	for(_CListOfNodes::iterator it=begin();it!=end();it++)
	{
		Core*=HASH_FACTOR;
		Core^=(*it);
	}
	return Core;
}

//**************_CGenericStateVector****************************
int _CGenericStateVector::ValueAt(int Index)//!!!!!!!!
{
	int sgn=SgnAbs(Index);
	ASSERTPDD(Index<(int)size());
	int Value=at(Index);
	if(Value!=0 && Value!=Index)
	{
		int NewValue=ValueAt(Value);
		if(NewValue!=Value)
			at(Index)=NewValue;
		return NewValue*sgn;
	}
	return Value*sgn;
}


long _CGenericStateVector::GetHashValue(long Core)
{
	unsigned Size=size();
	for(unsigned i=0;i<Size;i++)
	{
		Core*=HASH_FACTOR;
		Core^=ValueAt(i);
	}
	return Core;
}
//**************_CSparseGenericStateVector****************************
int _CSparseGenericStateVector::ValueAt(int Index)//!!!!!!!!
{
	int sgn=SgnAbs(Index);
	INT_INT_MAP::iterator fit=find(Index);
	if(fit==end())
		return Index*sgn;
	int TempRes=fit->second;
	if(TempRes==0)
		return 0;
	int NewTempRes=ValueAt(TempRes);
	if(NewTempRes!=TempRes)
		fit->second=NewTempRes;
	return NewTempRes*sgn;
}


long _CSparseGenericStateVector::GetHashValue(long Core)
{
	for(INT_INT_MAP::iterator it=begin(),_e=end();it!=_e;it++)
	{
		Core*=HASH_FACTOR;
		Core^=it->first;
		Core*=HASH_FACTOR;
		Core^=ValueAt(it->second);
	}
	return Core;
}

//**************_CGraphTable****************************
_CGraphTable::_CGraphTable(int NoOfNodes)/*:INIT_STREAMABLE(_CGraphTable)*/
{
	m_Sgn=1;
	for(int i=0;i<NoOfNodes;i++)
	{
		m_R.push_back(i);
		m_C.push_back(i);
		m_S.push_back(i);
	}
}

_CGraphTable::_CGraphTable(_CGraphTable& Source,unsigned NoOfBoundaries)
{
	m_Sgn=Source.m_Sgn;
	m_R.resize(NoOfBoundaries);
	m_C.resize(NoOfBoundaries);
	m_S.resize(NoOfBoundaries);
	for(unsigned i=0;i<NoOfBoundaries;i++)
	{
		m_R[i]=Source.m_R.ValueAt(i);
		m_C[i]=Source.m_C.ValueAt(i);
		m_S[i]=Source.m_S.ValueAt(i);
	}
}

_CGraphTable::~_CGraphTable(void)
{
}

unsigned _CGraphTable::GetMaxNoNode() const
{
	unsigned Res=m_C.size();
	ASSERTPDD(Res>0);
	return Res;
}
unsigned _CGraphTable::GetMaxDeletions() const
{
	unsigned Res=m_C.size();
	ASSERTPDD(Res>1);
	return Res-1;
} 

void _CGraphTable::Copy(const _CGraphTable& Source)
{
	m_R.assign(Source.m_R.begin(),Source.m_R.end());
	m_C.assign(Source.m_C.begin(),Source.m_C.end());
	m_S.assign(Source.m_S.begin(),Source.m_S.end());
	m_Sgn=Source.m_Sgn;
}

bool _CGraphTable::operator==(_CGraphTable& Operand)
{
	if(m_Sgn!=Operand.m_Sgn)
		return false;
	_CGenericStateVector::size_type Size=m_C.size();
	if(Size!=Operand.m_C.size())
		return false;
	for(int i=0;i<(int)Size;i++)
	{
		if(m_C.ValueAt(i)!=Operand.m_C.ValueAt(i))
			return false;
		if(m_R.ValueAt(i)!=Operand.m_R.ValueAt(i))
			return false;
		if(m_S.ValueAt(i)!=Operand.m_S.ValueAt(i))
			return false;
	}
	return true;
}

long _CGraphTable::DetermineHashKey(long Core)
{
	Core*=HASH_FACTOR;
	Core^=m_Sgn;
	Core*=HASH_FACTOR;
	Core=m_C.GetHashValue(Core);
	Core*=HASH_FACTOR;
	Core=m_R.GetHashValue(Core);
	Core*=HASH_FACTOR;
	Core=m_S.GetHashValue(Core);
	return Core;
}

RES_STATE _CGraphTable::ReadRealVlues(int& p, int& r, int& k, int& l)
{
	p=m_R.ValueAt(p);
	r=m_R.ValueAt(r);
	k=m_C.ValueAt(k);
	l=m_C.ValueAt(l);
	if(p==r || k==l)
		return TERM_SELFLOOP;
	return IN_USE;
}

void swap_sgn(int& a, int& b, short& sgn)
{
	if(abs(a)<abs(b))
	{
		swap(a,b);
		sgn=-sgn;
	}
	if(a<0)
	{
		a=-a;
		b=-b;
		sgn=-sgn;
	}
	if(a==-b)
	{
		b=0;
		sgn*=2;
	}
}

RES_STATE _CGraphTable::DoShortCircuit(int p, int r, int k, int l)
{
	if(p==r || k==l)
		return TERM_SELFLOOP;
	swap_sgn(p,r,m_Sgn);
	swap_sgn(k,l,m_Sgn);
	m_R[p]=r;
	m_C[k]=l;
	int s1=m_S.ValueAt(p), s2=m_S.ValueAt(k);
	if(s1==s2)
		m_S[s1]=0;
	else
	{
		if(s1<s2)
			swap(s1,s2);
		m_S[s1]=s2;
		m_Sgn=-m_Sgn;
	}
	return IN_USE;
}
void _CGraphTable::AddDeletion(int& p, int& r, int& k, int& l)
{
	ASSERTPDD(p!=r && k!=l);
	MaxMinSgn(p,r,m_Sgn);
	MaxMinSgn(k,l,m_Sgn);
	OrderSgn(p,r,m_Sgn);
	OrderSgn(k,l,m_Sgn);
	if(p==-r)
	{
		m_Sgn*=2;
		r=0;
	}
	if(k==-l)
	{
		m_Sgn*=2;
		l=0;
	}
	m_R[p]=r;
	m_C[k]=l;
	int a=m_S.ValueAt(p),b=m_S.ValueAt(k);
	if(a==b)
		m_S[a]=0;
	else
	{
		if(a<b)
			swap(a,b);
		m_Sgn=-m_Sgn;
		m_S[a]=b;
	}
}

short _CGraphTable::ExtractMultiplier()
{
	short Res=m_Sgn;
	m_Sgn=1;
	//short Res=abs(m_Sgn);
	//m_Sgn=sgn(m_Sgn);
	return Res;
}

void _CGraphTable::Table2Deletions(/*_CCircuit* pOwner,*/ _CMultiDeletions& Deletions, int Highest, short& Sign)
{
	if (Deletions.size()>1)
		RISEPDD(eWrongCondition, "Function obsolate for higher order deletions");
	short sgn=1;
	int P=0,K=0;
	for(;P<Highest;P++)
	{
		int R=m_R.ValueAt(P);
		if(R!=P)
		{
			bool NotFound=true;
			for(;K<Highest;K++)
			{
				int L=m_C.ValueAt(K);
				if(K!=L)
				{
					_CDeletion* pNewDel=new _CDeletion(Deletions.GetOwner(),P,R,K,L);
					Deletions.push_in_order(pNewDel);
					if(P!=K) 
						sgn=-sgn;
					NotFound=false;
					K++;
					break;
				}
			}
			if(NotFound)
				RISEPDD(eWrongCondition,"Impossible: Asymetric GraphTable");
		}
	}
	Sign=m_Sgn*sgn;
	//Deletions.SetMultiplier(Sign);
}

bool _CGraphTable::Table2Deletions2(_CMultiDeletions& Deletions, const _CIntNodes& Nodes, short& Sign)
{
	if (Deletions.size()>1)
		RISEPDD(eWrongCondition, "Function obsolate for higher order deletions");
	typedef pair<int, int> DEL;
	typedef vector<DEL> POT_DEL;
	POT_DEL PotRows,PotCols;
	int Highest=Nodes.size();
	short sgn=1;
	for(int P=0;P<Highest;P++)
	{
		while(P<Highest && Nodes[P]==-1) P++;
		if(P>=Highest) break;
		int R=m_R.ValueAt(P);
		int C=m_C.ValueAt(P);
		if(P!=R)
			PotRows.push_back(DEL(P,R));
		if(P!=C)
			PotCols.push_back(DEL(P,C));
	}
	if(PotCols.size()!=PotRows.size())
		return false;
	for(size_t i=0;i<PotCols.size();i++)
	{
		DEL& ROW=PotRows[i];
		DEL& COL=PotCols[i];
		_CDeletion* pNewDel=
			new _CDeletion(Deletions.GetOwner(),Nodes[ROW.first],Nodes[ROW.second],
			Nodes[COL.first],Nodes[COL.second]);
		Deletions.push_in_order(pNewDel);
		if(ROW.first!=COL.first)
			sgn=-sgn;
	}
	Sign=m_Sgn*sgn;
	Deletions[0].SetMultiplier(Sign);
	return true;
}

//void _CGraphTable::Table2Deletions(_CDeletions& Deletions, const _CIntNodes& Nodes, short& Sign)
//{
//	int Highest=Nodes.size();
//	short sgn=1;
//	int P=0,K=0;
//	for(;P<Highest;P++)
//	{
//		while(P<Highest && Nodes[P]==-1)
//			P++;
//		//if(Nodes[P]==-1)
//		//	P++;
//		if(P>=Highest)
//			break;
//		int R=m_R.ValueAt(P);
//		if(R!=P)
//		{
//			bool NotFound=true;
//			for(;K<Highest;K++)
//			{
//				while(K<Highest && Nodes[K]==-1)
//					K++;
//				//if(Nodes[K]==-1)
//				//	K++;
//				if(K>=Highest)
//					break;
//				int L=m_C.ValueAt(K);
//				if(K!=L)
//				{
//					_CDeletion* pNewDel=
//						new _CDeletion(Deletions.GetOwner(),Nodes[P],Nodes[R],Nodes[K],Nodes[L]);
//					Deletions.push_in_order(pNewDel);
//					if(P!=K) 
//						sgn=-sgn;
//					NotFound=false;
//					K++;
//					break;
//				}
//			}
//			if(NotFound)
//				RISEPDD(eWrongCondition,"Impossible: Asymetric GraphTable");
//		}
//	}
//	Sign=m_Sgn*sgn;
//	Deletions.SetMultiplier(Sign);
//}
void _CGraphTable::ReleaseDongles(const _CIntNodes& Dongles, const _CIntNodes& Nodes)
{
	for each(int Node in Dongles)
	{
		int P1=m_R.ValueAt(Node),K1=m_C.ValueAt(Node);
		if(P1==Node)
			for(unsigned i=Node+1;i<m_R.size();i++)
				if(Nodes[i]>=0 && abs(P1=m_R.ValueAt(i))==Node)
				{
					m_R[Node]=i*sgn(P1);
					m_R[i]=i;
					break;
				}
		if(K1==Node)
			for(unsigned i=Node+1;i<m_C.size();i++)
				if(Nodes[i]>=0 && abs(K1=m_C.ValueAt(i))==Node)
				{
					m_C[Node]=i*sgn(P1);
					m_C[i]=i;
					break;
				}
	}
}

IMPLEMENT_DYNAMIC_CREATION(_CGraphTable);
void _CGraphTable::Store(_binary_filer& Filer)
{
	_CInUseCheckpoint::Store(Filer);
	_CHashable::Store(Filer);
	Filer<<m_Sgn;
	Filer<<m_R;
	Filer<<m_C;
	Filer<<m_S;
	_streamable::Store(Filer);
}

void _CGraphTable::Load(_binary_filer& Filer)
{
	_CInUseCheckpoint::Load(Filer);
	_CHashable::Load(Filer);
	Filer>>m_Sgn;
	Filer>>m_R;
	Filer>>m_C;
	Filer>>m_S;
	_streamable::Load(Filer);
}

void _CGraphTable::ReduceToNoOfNodes(unsigned NewNoOfNodes)
{
	if(NewNoOfNodes<m_C.size())
	{
		m_C.resize(NewNoOfNodes);
		m_R.resize(NewNoOfNodes);
		m_S.resize(NewNoOfNodes);
	}
}

//******************_CGraphTableCache*******************

IMPLEMENT_DYNAMIC_CREATION(_CGraphTableCache);
//void _CGraphTableCache::Store(_binary_filer &Filer)
//{
//	Filer<<m_Index2ObjectMap.size();
//	for(INDEX2OBJECT_MAP_ITERATOR it=m_Index2ObjectMap.begin();it!=m_Index2ObjectMap.end();it++)
//		Filer<<*(it->second);
//}
//
//void _CGraphTableCache::Load(_binary_filer& Filer)
//{
//	Clear();
//	INDEX2OBJECT_MAP::size_type size;
//	Filer>>size;
//	for(INDEX2OBJECT_MAP::size_type i=0;i<size;i++)
//	{
//		_CGraphTable* pTable=dynamic_cast<_CGraphTable*>(Filer.RetriveDynamicClass());
//		PushInCache(pTable);
//	}
//}

//***************** _CSparseCommIntegrity*********************
IMPLEMENT_DYNAMIC_CREATION(_CSparseCommIntegrity);
void _CSparseCommIntegrity::Store(_binary_filer& Filer)
{
	_CInUseCheckpoint::Store(Filer);
	_streamable::Store(Filer);
	_CHashable::Store(Filer);
	int Term=numeric_limits<int>::min();
	char cc=';';
	for(INT_INT_MAP::iterator it=m_IncidentCurNodes.begin(),_e=m_IncidentCurNodes.end();
		it!=_e;it++)
	{
		Filer<<it->first;
		Filer<<it->second;
		Filer<<cc;
	}
	Filer<<Term<<Term<<cc;
}

void _CSparseCommIntegrity::Load(_binary_filer& Filer)
{
	_CInUseCheckpoint::Load(Filer);
	_streamable::Load(Filer);
	_CHashable::Load(Filer);
	int Term=numeric_limits<int>::min();
	int FValue=0;
	int SValue=0;
	char cc=0;
	m_IncidentCurNodes.clear();
	while(true)
	{
		Filer>>FValue;
		Filer>>SValue;
		Filer>>cc;
		if(cc!=';')
		{
			string str="File: "+Filer.Name()+" is corrupted";
			RISEPDD(eFilerProblem,str.c_str());
		}
		if(FValue==Term && SValue==Term)
			break;
		m_IncidentCurNodes.insert(INT_INT_MAP::value_type(FValue,SValue));
		if(SValue==0)
			m_Zeros.insert(FValue);
	}
}

long _CSparseCommIntegrity::DetermineHashKey(long Core)
{
	for(INT_INT_MAP::iterator it=m_IncidentCurNodes.begin(),_e=m_IncidentCurNodes.end();
		it!=_e;it++)
	{
		Core*=HASH_FACTOR;
		Core^=it->first;
		Core*=HASH_FACTOR;
		Core^=it->second;
	}
	return Core;
}
int _CSparseCommIntegrity::AddIncidence(int NoNumber)
{
	int& Value=m_IncidentCurNodes[NoNumber];
	if(Value==0)
	{
		INT_SET::iterator it=m_Zeros.find(NoNumber);
		if(it!=m_Zeros.end())
			m_Zeros.erase(NoNumber);
	}
	//Value++;
	return ++Value;
}

int _CSparseCommIntegrity::RemoveIncidence(int NodeNumber)
{
	INT_INT_MAP::iterator it=m_IncidentCurNodes.find(NodeNumber);
	if(it==m_IncidentCurNodes.end())
		RISEPDD(out_of_range,"Can't remove componnent connected to not defined node");
	int& Value=it->second;
	if(Value<=0)
	{
		string str="There is no comonent connected to node :"+long2str(NodeNumber);
		RISEPDD(eWrongCondition,str.c_str());
	}
	Value--;
	if(Value==0)
		m_Zeros.insert(NodeNumber);
	return Value;
}

int _CSparseCommIntegrity::ShortcircuitNodes(int TargetNode, int SuppresedNode)
{
	INT_INT_MAP::iterator TNit=m_IncidentCurNodes.find(TargetNode);
	ASSERTPDD(TNit!=m_IncidentCurNodes.end());
	INT_INT_MAP::iterator SNit=m_IncidentCurNodes.find(SuppresedNode);
	ASSERTPDD(SNit!=m_IncidentCurNodes.end());
	int& TNValue=TNit->second;
	int& SNValue=SNit->second;
	if(TNValue==0 && TNValue!=0)
		m_Zeros.erase(TargetNode);
	if(SNValue==0)
		m_Zeros.erase(SuppresedNode);
	TNValue+=SNValue;
	m_IncidentCurNodes.erase(SNit);
	return TNValue;
}

void _CSparseCommIntegrity::Copy(const _CSparseCommIntegrity& Source)
{
	m_IncidentCurNodes=Source.m_IncidentCurNodes;
	m_Zeros=Source.m_Zeros;
}

void _CSparseCommIntegrity::CopyAndRemove(const _CSparseCommIntegrity& Source,const COMPONENT_PINS& Pins)
{
	m_IncidentCurNodes=Source.m_IncidentCurNodes;
	m_Zeros=Source.m_Zeros;
	Remove(Pins);
}
void _CSparseCommIntegrity::Remove(const COMPONENT_PINS& Pins)
{
	for(COMPONENT_PINS::const_iterator it=Pins.begin(),_e=Pins.end();it!=_e;it++)
		RemoveIncidence(*it);
}
bool _CSparseCommIntegrity::IsConnected(_CSparseGenericStateVector& StateVector)
{
	for(INT_SET::iterator it=m_Zeros.begin(),_e=m_Zeros.end();it!=_e;it++)
	{
		const int& ZeroPos=*it;
		int Value=abs(StateVector.ValueAt(ZeroPos));
		if(Value!=ZeroPos)
			continue;
		bool LocalConnected=false;
		for(_CSparseGenericStateVector::iterator vit=StateVector.begin(),_ve=StateVector.end();vit!=_ve;vit++)
			if(abs(vit->second)==ZeroPos && m_Zeros.find(vit->first)==m_Zeros.end())
			{
				LocalConnected=true;
				break;
			}
		if(!LocalConnected)
			return false;
	}
	return true;
}

//***************** _CSparseDeletionAnalyser*********************
IMPLEMENT_DYNAMIC_CREATION(_CSparseDeletionAnalyser);
void _CSparseDeletionAnalyser::Store(_binary_filer& Filer)
{
	_CInUseCheckpoint::Store(Filer);
	_CHashable::Store(Filer);
	Filer<<m_Sgn;
	Filer<<m_R;
	Filer<<m_C;
	Filer<<m_S;
	_streamable::Store(Filer);
}
void _CSparseDeletionAnalyser::Load(_binary_filer& Filer)
{
	_CInUseCheckpoint::Load(Filer);
	_CHashable::Load(Filer);
	Filer>>m_Sgn;
	Filer>>m_R;
	Filer>>m_C;
	Filer>>m_S;
	_streamable::Load(Filer);
}

long _CSparseDeletionAnalyser::DetermineHashKey(long Core)
{
	Core=m_R.GetHashValue(Core);
	Core=m_C.GetHashValue(Core);
	Core=m_S.GetHashValue(Core);
	Core*=HASH_CORE;
	Core^=m_Sgn;
	return 0;
}
//***************** _CIntegrityTable*********************

_CIntegrityTable::_CIntegrityTable(const _CIntegrityTable& Source, unsigned NoOfBounderies)
{
	m_R.assign(Source.m_R.begin(),Source.m_R.begin()+NoOfBounderies);
	m_C.assign(Source.m_C.begin(),Source.m_C.begin()+NoOfBounderies);
}

_CIntegrityTable::~_CIntegrityTable(void)
{
}
void _CIntegrityTable::Copy(const _CIntegrityTable& Source)
{
	m_R.assign(Source.m_R.begin(),Source.m_R.end());
	m_C.assign(Source.m_C.begin(),Source.m_C.end());
}

bool _CIntegrityTable::operator==(_CIntegrityTable& Operand)
{
	_CListOfNodes::size_type Size=m_C.size();
	if(Size!=Operand.m_C.size())
		return false;
	for(int i=0;i<(int)Size;i++)
		if(m_C[i]!=Operand.m_C[i] ||
			m_R[i]!=Operand.m_R[i])
			return false;
	return true;
}
long _CIntegrityTable::DetermineHashKey(long Core)
{
	Core=m_C.GetHashValue(Core);
	Core*=HASH_FACTOR;
	Core=m_R.GetHashValue(Core);
	Core*=HASH_FACTOR;
	return Core;
}

RES_STATE _CIntegrityTable::CheckDisconnetions(const TWO_GRAPH_PINS& Pins)
{
	for(TWO_GRAPH_PINS::const_reverse_iterator it=Pins.rbegin();it!=Pins.rend();it++)
		if(m_R[it->first]==0 || m_C[it->second]==0)
			if(it->first!=1 || it->second!=1)
				return TERM_DISCONNECTED;
			else
				return TestState()?TERM_ONE:TERM_DISCONNECTED;
	return IN_USE;
}

bool _CIntegrityTable::IsCompressedToNode()
{
	unsigned size=m_C.size();
	if(m_R[0]==0 && m_C[0]==0)
	{
		for(unsigned i=1;i<size;i++)
			if(m_R[i]>-1 || m_C[i]>-1)
				return false;
		return true;
	}
	//else
	//	for(int i=1;i<size;i++)
	//		if(m_R[i]==0 || m_C[i]==0)
	//			return false;
	return false;
}
RES_STATE _CIntegrityTable::TestState()
{
	unsigned size=m_R.size();
	RES_STATE Res=IN_USE;
	if(m_R[0]==0 && m_C[0]==0)
	{
		Res=TERM_ONE;
		for(unsigned i=2;i<size;i++)
			if(m_R[i]!=-1 || m_C[i]!=-1)
			{
				Res=TERM_DISCONNECTED;
				break;
			}
	}
	else
		for(unsigned i=1;i<size;i++)
			if(m_R[i]==0 || m_C[i]==0)
				return TERM_DISCONNECTED;
	return Res;
}

//RES_STATE _CIntegrityTable::DoOpenCircuit(int p, int r, int k, int l)
//{
//	ABS(p);ABS(r);ABS(k);ABS(l);
//	int a=--m_R[p], b=--m_R[r], c=--m_C[k], d=--m_C[l];
//	if(a==0 || b==0 || c==0 || d==0 /*&& p==r && k==l*/)
//		return IsCompressedToNode()?TERM_ONE:TERM_DISCONNECTED;
//	return IN_USE;
//}
//
//RES_STATE _CIntegrityTable::DoShortCircuit(int p, int r, int k, int l, bool WithoutRemoval)
//{
//	MaxMinAbs(p,r);MaxMinAbs(k,l);
//	int a=m_R[p], b=m_R[r], c=m_C[k], d=m_C[l];
//	if(!WithoutRemoval)
//	{
//		a--;b--;c--;d--;
//	}
//	b+=a;
//	d+=c;
//	m_R[p]=-1; m_R[r]=b;
//	m_C[k]=-1; m_C[l]=d;
//	if(!WithoutRemoval && (b==0 || d==0))
//		return IsCompressedToNode()?TERM_ONE:TERM_DISCONNECTED;
//	return IN_USE;
//}

void _CIntegrityTable::PlugComponents(const COMPONENT_PINS& Pins)
{
	for(COMPONENT_PINS::const_iterator it=Pins.begin();it!=Pins.end();it++)
	{
		unsigned pin=*it;
		ASSERTPDD(pin<m_C.size());
		m_C[pin]++;m_R[pin]++;
	}
}
void _CIntegrityTable::UnPlugComponent(const TWO_GRAPH_PINS& Pins)
{
	for(TWO_GRAPH_PINS::const_iterator it=Pins.begin();it!=Pins.end();it++)
	{
		TWO_GRAPH_PIN pins=*it;
		ASSERTPDD(pins.first<m_R.size() && pins.second<m_C.size());
		m_C[pins.second]--;m_R[pins.first]--;
	}
}
bool _CIntegrityTable::IsDisconnected(const TWO_GRAPH_PINS& Pins)
{
	for(TWO_GRAPH_PINS::const_iterator it=Pins.begin();it!=Pins.end();it++)
	{
		TWO_GRAPH_PIN pins=*it;
		ASSERTPDD(pins.first<m_R.size() && pins.second<m_C.size());
		if(m_R[pins.first]==0 || m_C[pins.second]==0)
			if(pins.first!=0 || pins.second!=0)
				return true;
			else
				return !IsCompressedToNode();
	}
	return false;
}
bool _CIntegrityTable::IsDisconnected(int r, int c)
{
	ABS(r); ABS(c);
	return (m_R[r]==0 && r!=0) || (m_C[c]==0 && c!=0);
}

void _CIntegrityTable::MarkBoundaryNode(unsigned Pin)
{
	ASSERTPDD(Pin<m_C.size());
	m_C[Pin]++;
	m_R[Pin]++;
}

RES_STATE _CIntegrityTable::AddDeletion(int p, int r, int k, int l)
{
	ABS(r); ABS(l);
	m_R[r]+=m_R[p];
	m_C[l]+=m_C[k];
	m_R[p]=-1;
	m_C[k]=-1;
	if(m_R[r]==0 && m_C[l]==0)
		if(r!=0 || l!=0)
			return TERM_DISCONNECTED;
		else
			return IsCompressedToNode()?TERM_ONE:TERM_DISCONNECTED;
	return IN_USE;
}

IMPLEMENT_DYNAMIC_CREATION(_CIntegrityTable);

void _CIntegrityTable::Store(_binary_filer& Filer)
{
	_CInUseCheckpoint::Store(Filer);
	_CHashable::Store(Filer);
	Filer<<m_C;
	Filer<<m_R;
	_streamable::Store(Filer);
}

void _CIntegrityTable::Load(_binary_filer& Filer)
{
	_CInUseCheckpoint::Load(Filer);
	_CHashable::Load(Filer);
	Filer>>m_C;
	Filer>>m_R;
	_streamable::Load(Filer);
}

void _CIntegrityTable::ReduceToNoOfNodes(unsigned NewNoOfNodes)
{
	if(NewNoOfNodes<m_C.size())
	{
		m_C.resize(NewNoOfNodes);
		m_R.resize(NewNoOfNodes);
	}
}
//******************_CIntegrityTableCache****************
IMPLEMENT_DYNAMIC_CREATION(_CIntegrityTableCache);

//****************** _CGraphState************************
_CGraphState::_CGraphState(_CCircuit* pOnlyOwner):m_pCircuitOwner(pOnlyOwner),m_NoOfDeletions(0),m_State(IN_USE),
m_pGraphTable(NULL),m_pIntegrityTable(NULL)/*,INIT_STREAMABLE(_CGraphState)*/
{
	ASSERTPDD(pOnlyOwner!=NULL);
	unsigned NoOfNodes=pOnlyOwner->NoOfNodes();
	pGraphTable=new _CGraphTable(NoOfNodes);
	pIntegrityTable=new _CIntegrityTable(NoOfNodes);
}
_CGraphState::_CGraphState(const _CGraphState& Source,unsigned NoOfBoundaryNodes):
m_pCircuitOwner(Source.m_pCircuitOwner),m_NoOfDeletions(Source.m_NoOfDeletions),
m_State(Source.m_State),m_pGraphTable(NULL),m_pIntegrityTable(NULL)
{
	pGraphTable=new _CGraphTable(*Source.m_pGraphTable,NoOfBoundaryNodes);
	pIntegrityTable=new _CIntegrityTable(*Source.m_pIntegrityTable,NoOfBoundaryNodes);
}

void _CGraphState::TouchDepended()
{
	if(m_pGraphTable!=NULL)
		m_pGraphTable->TouchMe();
	if(m_pIntegrityTable!=NULL)
		m_pIntegrityTable->TouchMe();
}
void _CGraphState::UnTouchDepended()
{
	if(m_pGraphTable!=NULL)
		m_pGraphTable->UnTouchMe();
	if(m_pIntegrityTable!=NULL)
		m_pIntegrityTable->UnTouchMe();
}


RES_STATE _CGraphState::AddDeletion(int p, int r, int k, int l)
{
	if(m_State==TERM_SELFLOOP || m_State==TERM_ONE || m_NoOfDeletions>=m_pGraphTable->GetMaxDeletions())
		return TermSelfLoop();
	if(m_pGraphTable->ReadRealVlues(p,r,k,l)==TERM_SELFLOOP)
		return TermSelfLoop();
	m_pGraphTable->AddDeletion(p,r,k,l);
	m_State=m_pIntegrityTable->AddDeletion(p,r,k,l);
	m_NoOfDeletions++;
	if(m_State==IN_USE && m_NoOfDeletions==m_pGraphTable->GetMaxDeletions())
		if(m_pIntegrityTable->IsCompressedToNode())
			m_State=TERM_ONE;
	return m_State;
}
bool _CGraphState::IsSeparatedNode(int Node)
{
	int a=m_pGraphTable->RealRow(Node),
		b=m_pGraphTable->RealColumn(Node);
	if(m_pIntegrityTable->IsDisconnected(a,b))
	{
		m_State=TERM_DISCONNECTED;
		return true;
	}
	else
		return false;
}
RES_STATE _CGraphState::CheckCompressionToNode()
{
	if(m_pIntegrityTable!=NULL && m_pIntegrityTable->IsCompressedToNode())
		m_State=TERM_ONE;
	return m_State;
}
short _CGraphState::ExtractMultiplier()
{
	return (m_State==IN_USE || m_State==TERM_ONE)?m_pGraphTable->ExtractMultiplier():0;
}

_CGraphState::~_CGraphState(void)
{
	if(m_pGraphTable!=NULL && m_pIntegrityTable!=NULL)
	{
		UnTouchDepended();
		if(!m_pGraphTable->IsRegistered())
			delete m_pGraphTable;
		if(!m_pIntegrityTable->IsRegistered())
			delete m_pIntegrityTable;
	}
}

//RES_STATE _CGraphState::DoShortCircuit(int p, int r, int k, int l, bool PureShortCircuit)
//{
//	if(m_State==TERM_ONE || m_State==TERM_SELFLOOP)
//	{
//		m_State=TERM_SELFLOOP;
//		return m_State;
//	}
//	m_State=m_pGraphTable->ReadRealVlues(p,r,k,l);
//	if(PureShortCircuit)
//	{
//		if(m_State==TERM_SELFLOOP)
//		{
//			//delete m_pGraphTable;
//			m_pGraphTable=NULL;
//			//delete m_pIntegrityTable;
//			m_pIntegrityTable=NULL;
//			return m_State;
//		}
//		m_State=m_pGraphTable->DoShortCircuit(p,r,k,l);
//		if(m_State==TERM_SELFLOOP)
//		{
//			//delete m_pIntegrityTable;
//			//delete m_pGraphTable;
//			m_pGraphTable=NULL;
//			m_pIntegrityTable=NULL;
//			return m_State;
//		}
//		m_State=m_pIntegrityTable->DoShortCircuit(p,r,k,l, PureShortCircuit);
//		if(m_State==TERM_DISCONNECTED)
//		{
//			//delete m_pIntegrityTable;
//			m_pGraphTable=NULL;
//			m_pIntegrityTable=NULL;
//			return m_State;
//		}
//	}
//	else
//	{
//		if(m_State==TERM_SELFLOOP)
//		{
//			pGraphTable=NULL;
//			pIntegrityTable=NULL;
//			return m_State;
//		}
//		_CGraphTable* pNewGraphTable=new _CGraphTable(*m_pGraphTable);
//		m_State=pNewGraphTable->DoShortCircuit(p,r,k,l);
//		if(m_State==TERM_SELFLOOP)
//		{
//			delete pNewGraphTable;
//			pGraphTable=NULL;
//			pIntegrityTable=NULL;
//			return m_State;
//		}
//		try
//		{
//			_CIntegrityTable* pNewIntegrityTable=new _CIntegrityTable(*m_pIntegrityTable);
//			m_State=pNewIntegrityTable->DoShortCircuit(p,r,k,l, PureShortCircuit);
//			if(m_State==TERM_DISCONNECTED)
//			{
//				delete pNewIntegrityTable;
//				delete pNewGraphTable;
//				pGraphTable=NULL;
//				pIntegrityTable=NULL;
//				return m_State;
//			}
//			try
//			{
//				pIntegrityTable=m_pCircuitOwner->PushIntoCache(pNewIntegrityTable);
//				pGraphTable=m_pCircuitOwner->PushIntoCache(pNewGraphTable);
//			}
//			catch(bad_alloc)
//			{
//				delete pNewIntegrityTable;
//				throw;
//			}
//
//		}
//		catch (bad_alloc)
//		{
//			delete pNewGraphTable;
//			throw;
//		}
//	}
//	return IN_USE;
//}

//void _CGraphState::TestShortCircuit(int p, int r, int k, int l,_CComponentPredicator* pPredicator)
//{
//	if(m_State==TERM_SELFLOOP || m_State==TERM_ONE)
//	{
//		pPredicator->SetNoOfNewAncestors(0);
//		return;
//	}
//	int ShortState=m_pGraphTable->ReadRealVlues(p,r,k,l);
//}

//RES_STATE _CGraphState::DoOpenCircuit(int p, int r, int k, int l)
//{
//	if(m_State==TERM_SELFLOOP)
//		return m_State;
//	m_pGraphTable->ReadRealVlues(p,r,k,l);
//	ABS(p);ABS(r);ABS(k);ABS(l);
//	_CIntegrityTable* pNewIntTable=new _CIntegrityTable(*m_pIntegrityTable);
//	m_State=pNewIntTable->DoOpenCircuit(p,r,k,l);
//	if(m_State==TERM_SELFLOOP)
//	{
//		delete pNewIntTable;
//		pIntegrityTable=NULL;
//		pGraphTable=NULL;
//	}
//	else
//	try
//	{
//		pIntegrityTable=m_pCircuitOwner->PushIntoCache(pNewIntTable);
//	}
//	catch(exception)
//	{
//		pIntegrityTable=NULL;
//		throw;
//	}
//	return m_State;
//}
//
//void _CGraphState::TestOpenCircuit(int p, int r, int k, int l,_CComponentPredicator* pPredicator)
//{
//}


void _CGraphState::Copy(const _CGraphState& Source, bool ForceNewTables)
{
	m_State=Source.m_State;
	m_pCircuitOwner=Source.m_pCircuitOwner;
	m_NoOfDeletions=Source.m_NoOfDeletions;
	if(ForceNewTables && Source.pIntegrityTable!=NULL && Source.pGraphTable!=NULL)
	{
		pIntegrityTable=new _CIntegrityTable(*Source.pIntegrityTable);
		try
		{
			pGraphTable=new _CGraphTable(*Source.pGraphTable);
		}
		catch(bad_alloc)
		{
			delete pIntegrityTable;
			pIntegrityTable=NULL;
			throw;
		}
	}
	else
	{
		pIntegrityTable=Source.pIntegrityTable;
		pGraphTable=Source.pGraphTable;
	}
}

void _CGraphState::SetIntegrityTable(_CIntegrityTable* pIntegrityTable)
{
	if(m_pIntegrityTable==pIntegrityTable)
		return;
	if(m_pIntegrityTable!=NULL)
	{
		m_pIntegrityTable->UnTouchMe();
		if(!m_pIntegrityTable->IsRegistered())
			delete m_pIntegrityTable;
	}
	m_pIntegrityTable=pIntegrityTable;
	if(m_pIntegrityTable!=NULL)
		m_pIntegrityTable->TouchMe();
}

void _CGraphState::SetGraphTable(_CGraphTable* pGraphTable)
{
	if(m_pGraphTable==pGraphTable)
		return;
	if(m_pGraphTable!=NULL)
	{
		m_pGraphTable->UnTouchMe();
		if(!m_pGraphTable->IsRegistered())
			delete m_pGraphTable;
	}
	m_pGraphTable=pGraphTable;
	if(m_pGraphTable!=NULL)
		m_pGraphTable->TouchMe();
}

bool _CGraphState::operator==(_CGraphState& Operand)
{
	if(m_State!=Operand.m_State ||
		!((*m_pGraphTable)==(*Operand.m_pGraphTable)) ||
		!((*m_pIntegrityTable)==(*Operand.m_pIntegrityTable)) ||
		m_pCircuitOwner!=Operand.m_pCircuitOwner)
		return false;
	return true;
}

long _CGraphState::DetermineHashKey(long Core)
{
	Core*=HASH_FACTOR;
	Core^=(long)m_State;
	Core*=HASH_FACTOR;
	if(m_pGraphTable!=NULL)
		Core=m_pGraphTable->DetermineHashKey(Core);
	Core*=HASH_FACTOR;
	if(m_pIntegrityTable!=NULL)
		Core^=m_pIntegrityTable->DetermineHashKey(Core);
	return Core;
}

void _CGraphState::ForceZero()
{
	m_State=TERM_SELFLOOP;
	pIntegrityTable=NULL;
	pGraphTable=NULL;
}

void _CGraphState::RegisterMembers()
{
	if(m_pGraphTable==NULL || m_pIntegrityTable==NULL)
		return;
	ASSERTPDD(m_pCircuitOwner!=NULL);
	pGraphTable=m_pCircuitOwner->PushIntoCache(m_pGraphTable);
	pIntegrityTable=m_pCircuitOwner->PushIntoCache(m_pIntegrityTable);
}
RES_STATE _CGraphState::TestState()
{	
	if(m_pIntegrityTable!=NULL)
		m_State=m_pIntegrityTable->TestState();
//	int size=m_pGraphTable->GetMaxNoNode()+1;
//	for(int i=1;i<size && !IsSeparatedNode(i);i++);
	return m_State;
}

void _CGraphState::Store(_binary_filer& Filer)
{
	_CInUseCheckpoint::Store(Filer);
	_CHashable::Store(Filer);
	//Filer<<m_NoOfDeletions;
	//Filer<<(char)m_State;
	//long long Index=m_pCircuitOwner->GetGraphTableIndex(m_pGraphTable);
	//Filer<<Index;
	//Index=m_pCircuitOwner->GetIntegrityTableIndex(m_pIntegrityTable);
	//Filer<<Index;
	_streamable::Store(Filer);
}

void _CGraphState::Load(_binary_filer& Filer)
{
	_CInUseCheckpoint::Load(Filer);
	_CHashable::Load(Filer);
	//Filer>>m_NoOfDeletions;
	//char aux;
	//Filer>>aux;
	//m_State=(RES_STATE)aux;
	//long long Index;
	//Filer>>Index;
	//m_pGraphTable=m_pCircuitOwner->GetGraphTableFromIndex(Index);
	//Filer>>Index;
	//m_pIntegrityTable=m_pCircuitOwner->GetIntegrityTableFromIndex(Index);
	_streamable::Load(Filer);
}


IMPLEMENT_DYNAMIC_CREATION(_CGraphState);

void _CGraphState::SetOwner(_CCircuit* pOwner)
{
	if(m_pCircuitOwner!=NULL)
		RISEPDD(eIllegalOperation,"_CGraphState::SetOwner: Operation for allready initialized value is prohibited");
	m_pCircuitOwner=pOwner;
}
void _CGraphState::FindTheSmallestIncidence(set<size_t>& RSet,set<size_t>& CSet, TWO_GRAPH_PINS& Pins, size_t MinPos, size_t MaxPos,size_t& TheSmallest, size_t& Repetition)
{
	TheSmallest=numeric_limits<size_t>::max();
	Repetition=0;
	for(TWO_GRAPH_PINS::iterator it=Pins.begin();it!=Pins.end();it++)
	{
		size_t rp=(*it).first;
		if(rp>=MinPos && rp<=MaxPos && (RSet.find(rp)==RSet.end()))
		{
			RSet.insert(rp);
			rp=m_pIntegrityTable->Row(rp);
			if(TheSmallest>rp)
			{
				TheSmallest=rp;
				Repetition=1;
			}
			else if(TheSmallest==rp)
				Repetition++;
		}
		rp=(*it).second;
		if(rp>=MinPos && rp<=MaxPos && (CSet.find(rp)==CSet.end()))
		{
			CSet.insert(rp);
			rp=m_pIntegrityTable->Column(rp);
			if(TheSmallest>rp)
			{
				TheSmallest=rp;
				Repetition=1;
			}
			else if(TheSmallest==rp)
				Repetition++;
		}
	}
}

IMPLEMENT_DYNAMIC_CREATION(_CGraphStateCache);

void _CGraphStateCache::SetOwner(_CCircuit* pOwner)
{
	if(m_pOwner!=NULL)
		RISEPDD(eIllegalOperation,"_CGraphStateCache::SetOwner: Operation for allready initialized value is prohibited");
	m_pOwner=pOwner;
}

void _CGraphStateCache::DoSomeExtras(_streamable* pMe)
{
	dynamic_cast<_CGraphState*>(pMe)->SetOwner(m_pOwner);
}
