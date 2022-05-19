#include "StdAfx.h"
#include "CSimpleVertexContainer.h"
#include "Vertex.h"
#include "Circuit.h"
#include "SubCircuitSocket.h"

_CSimpleVertexContainer EmptyVertex;
_CSimpleVertexContainer ZeroSVertex(0,0);
_CSimpleVertexContainer OneSVertex(1,1);
_CSimpleVertexContainer MOneSVertex(1,-1);

_CSExpandedVertices StaticEmptyVertices;
_CSExpandedVertices StaticZeroVertices(_CSExpandedVertices::ZERO);
_CSExpandedVertices StaticOneVertices(_CSExpandedVertices::ONE);
_CSExpandedVertices StaticMinusOneVertices(_CSExpandedVertices::MINUS_ONE);

long long GetTypedHash(long long& Core, const _CSimpleVertexContainer& Vertex)
{
	return Vertex.GetHash(Core);
}

//_CSimpleVertexContainer::_CSimpleVertexContainer(size_t EntryId, size_t PosId, const _CSubCircuitSocket* pSocket, _CSimpleVerticesMaps* pToSubcircuitLeves):
_CSimpleVertexContainer::_CSimpleVertexContainer(size_t EntryId, size_t PosId, const _CSubCircuitSocket* pSocket, 
												 _CSubCircuitVerticesMap* pToSubcircuitLeves,
												 long long MVId):
m_Multiplier(1), m_VertexType(VertType::InputConnector),m_pSocket(pSocket),
m_pConnectionVector(pToSubcircuitLeves), m_MVId(MVId)
{
	TGlobal_Vertex_Id Id;
	Id.PARTS.Circuit_Id=EntryId;
	Id.PARTS.Local_Id=PosId;
	m_lVertex=Id.Global_Id;
}
void _CSimpleVertexContainer::DecomposeInputIdx(size_t& EntryId, size_t& PosId) const
{
	ASSERTPDD(IsInputConnector() || IsLocalTerminal());
	TGlobal_Vertex_Id Id;
	Id.Global_Id=m_lVertex;
	EntryId=Id.PARTS.Circuit_Id;
	PosId=Id.PARTS.Local_Id;
}
long long _CSimpleVertexContainer::ConsiderInHash(long long& Seed) const
{
	Seed*=HASH_FACTOR;
	Seed^=(long long)m_VertexType;
	Seed*=HASH_FACTOR;
	Seed^=(long long)m_Multiplier;
	Seed*=HASH_FACTOR;
	Seed^=(long long)m_lVertex;
	Seed*=HASH_FACTOR;
	Seed^=(long long)m_pSocket;
	Seed*=HASH_FACTOR;
	Seed^=(long long)m_pConnectionVector;
	return Seed;
}

bool _CSimpleVertexContainer::IsLameLeg()
{
	return false;
}

long long _CSimpleVertexContainer::GetHash(long long& Core) const
{
	Core*=HASH_FACTOR;
	Core^=(long long)m_VertexType;
	Core*=HASH_FACTOR;
	Core^=m_lVertex;
	Core*=HASH_FACTOR;
	Core^=m_Multiplier;
	Core*=HASH_FACTOR;
	Core^=(long long)m_pSocket;
	Core*=HASH_FACTOR;
	Core^=(long long)m_pConnectionVector;
	Core*=HASH_FACTOR;
	Core^=m_MVId;
	return Core;
}
_CSimpleVertexContainer& _CSimpleVertexContainer::operator=(const _CSimpleVertexContainer& Right)
{
	m_VertexType=Right.m_VertexType;
	m_lVertex=Right.m_lVertex;
	m_Multiplier=Right.m_Multiplier;
	m_pSocket=Right.m_pSocket;
	m_pConnectionVector=Right.m_pConnectionVector;
	m_MVId=Right.m_MVId;
	return *this;
}

string _CSimpleVertexContainer::iVertex2string() const
{
	TGlobal_Vertex_Id ID;
	ID.Global_Id=m_lVertex;
	char Buffer[100];
	sprintf_s(Buffer,100,"%u.%u",ID.PARTS.Circuit_Id,ID.PARTS.Local_Id);
	return Buffer;
}
//void _CSimpleVertexContainer::WriteToStream(iostream& stream, bool StopEmbading, _CSimpleVerticesMapsSet& Set) const
//{
//	if(IsLocalTerminal())
//	{
//		TGlobal_Vertex_Id Id;
//		Id.Global_Id=m_lVertex;
//		//stream<<"LT[e("<<Id.PARTS.Circuit_Id<<").s^"<<Id.PARTS.Local_Id<<"]";
//		stream<<"LT[e("<<m_lVertex<<")]";
//		return /*stream*/;
//	}
//	if(IsInputConnector())
//	{
//		size_t LT=Set.Add(m_pConnectionVector);
//		long cId=m_pSocket->GetModelCircuit()->GetMyId();
//		const string& Name=m_pSocket->GetModelCircuit()->GetName();
//		TGlobal_Vertex_Id Id;
//		Id.Global_Id=m_lVertex;
//		//stream<<"IC[m("<<cId<<").e("<<Id.PARTS.Circuit_Id<<").s^"<<Id.PARTS.Local_Id<<"]";
//		stream<<"IC["<<Name<<".t("<<LT<<").e("<<Id.PARTS.Circuit_Id<<").s^"<<Id.PARTS.Local_Id<<"]";
//		return;
//	}
//	if(Is0())
//		stream<<"0";
//	else if(Is1())
//		stream<<"1";
//	else if(IsM1())
//		stream<<"(-1)";
//	else
//	{
//		if(m_Multiplier!=1)
//			if(m_Multiplier<0)
//				stream<<"(-"<<(-m_Multiplier)<<")*";
//			else
//				stream<<""<<m_Multiplier<<"*";
//		TGlobal_Vertex_Id ID;
//		ID.Global_Id=m_lVertex;
//		stream<<""<<ID.PARTS.Circuit_Id<<'.'<<ID.PARTS.Local_Id;
//	}
//	if(!StopEmbading &&IsInputConnector())
//	{
//		stream<<" - ";
//		m_pConnectionVector->WriteToStream(stream,Set);
//		//stream<<(*m_pConnectionVector);
//	}
//	//return stream;
//}

void _CSimpleVertexContainer::WriteToStream(iostream& stream, bool StopEmbading, _CSimpleVerticesMapsSet& Set) const
{
	if(IsLocalTerminal())
	{
		TGlobal_Vertex_Id Id;
		Id.Global_Id=m_lVertex;
		//stream<<"LT[e("<<Id.PARTS.Circuit_Id<<").s^"<<Id.PARTS.Local_Id<<"]";
		stream<<"LT[e("<<Id.PARTS.Circuit_Id<<")]";//.s^"<<Id.PARTS.Local_Id<<"]";
		//stream<<"LT[e("<<m_lVertex<<")]";
		return /*stream*/;
	}
	if(IsInputConnector())
	{
		size_t LT=Set.Add(m_pConnectionVector);
		long cId=m_pSocket->GetModelCircuit()->GetMyId();
		const string& Name=m_pSocket->GetModelCircuit()->GetName();
		TGlobal_Vertex_Id Id;
		Id.Global_Id=m_lVertex;
		//stream<<"IC[m("<<cId<<").e("<<Id.PARTS.Circuit_Id<<").s^"<<Id.PARTS.Local_Id<<"]";
		//stream<<"IC["<<Name<<".t("<<LT<<").e("<<Id.PARTS.Circuit_Id<<").s^"<<Id.PARTS.Local_Id<<"]";
		stream<<"IC["<<Name<<".t("<<LT<<").e("<<Id.PARTS.Circuit_Id<<")]";
		return;
	}
	if(Is0())
		stream<<"0";
	else if(Is1())
		stream<<"1";
	else if(IsM1())
		stream<<"(-1)";
	else
	{
		if(m_Multiplier!=1)
			if(m_Multiplier<0)
				stream<<"(-"<<(-m_Multiplier)<<")*";
			else
				stream<<""<<m_Multiplier<<"*";
		TGlobal_Vertex_Id ID;
		ID.Global_Id=m_lVertex;
		stream<<""<<ID.PARTS.Circuit_Id<<'.'<<ID.PARTS.Local_Id;
	}
	//if(!StopEmbading &&IsInputConnector())
	//{
	//	stream<<" - ";
	//	m_pConnectionVector->WriteToStream(stream,Set);
	//	//stream<<(*m_pConnectionVector);
	//}
	//return stream;
}

//iostream& operator<<(iostream& stream,const _CSimpleVertexContainer& Container)
//{
//	//Container.WriteToStream(stream,false);
//	return Container.operator <<(stream);
//}

bool _CSimpleVertexContainer::operator==(const _CSimpleVertexContainer& Right) const
{
	return (m_VertexType==Right.m_VertexType && m_lVertex==Right.m_lVertex &&
		m_Multiplier==Right.m_Multiplier && m_pSocket==Right.m_pSocket && 
		m_pConnectionVector==Right.m_pConnectionVector && m_MVId==Right.m_MVId);
}


template<>
long long GetInputOperatorHash(long long& Core, const _CSimpleVertexContainer* Arg) 
{
	//return (long long)Core*HASH_FACTOR^(Arg->GetLVertex());
	return Arg->GetHash(Core);
}

template<>
long long GetInputOperatorHash(long long& Core,_C2Args Arg) 
{
	Core=Arg.first->GetHash(Core);
	Core=Arg.second->GetHash(Core);
	return Core;
}


long long _CSExpandedVertices::GetHash(long long& Core) const
{
	for(const_iterator it=begin();it!=end();it++)
	{
		Core*=HASH_FACTOR;
		Core=it->GetHash(Core);
	}
	return Core;
}
//void _CSExpandedVertices::Copy(const _CSExpandedVertices& Source, _CSubCircuitSocket* pSocket,_CSimpleVerticesMaps* pToSubcircuitInput)
void _CSExpandedVertices::Copy(const _CSExpandedVertices& Source, _CSubCircuitSocket* pSocket,_CSubCircuitVerticesMap* pToSubcircuitInput,long long MVId)
{
	clear();
	for each(const _CSimpleVertexContainer Cont in Source)
		push_back(_CSimpleVertexContainer(Cont,pSocket,pToSubcircuitInput,MVId));
}

void _CSExpandedVertices::SetInputConnectorData(size_t LocalVerticesId,const _CSExpandedVertices& LocalSource,
												//_CSubCircuitSocket* pSocket,_CSimpleVerticesMaps* pToSubcircuitInput)
												_CSubCircuitSocket* pSocket,_CSubCircuitVerticesMap* pToSubcircuitInput,
												long long MVId)
{
	clear();
	size_t _size=LocalSource.size();
	resize(_size);
	for(size_t i=0;i<_size;i++)
	{
		const _CSimpleVertexContainer& SCont=LocalSource[i];
		_CSimpleVertexContainer& TCont=at(i);
		if(SCont.Is01M1())
			TCont=SCont;
		else
			//TCont=_CSimpleVertexContainer(SCont,pSocket,pToSubcircuitInput);
			TCont=_CSimpleVertexContainer(LocalVerticesId,0/*i*/,pSocket,pToSubcircuitInput,MVId);
	}
	//for(iterator it=begin();it!=end();it++)
	//{
	//	_CSimpleVertexContainer& Cont=*it;
	//	if(!Cont.Is01M1())
	//		Cont.m_VertexType=_CSimpleVertexContainer::InputConnector;
	//	Cont.m_pConnectionVector=pToSubcircuitInput;
	//	Cont.m_pSocket=pSocket;
	//}
}

_CSimpleVertexContainer& _CSExpandedVertices::GetVertex(int Index)
{
	if(Index<0 || Index>=(int)size())
		return ZeroSVertex;
	else
		return at((unsigned)Index);
}

void _CSExpandedVertices::SetVertex(const _CSimpleVertexContainer& Vertex,int Index)
{
	if(Index<0)
		return;
	if(Index>=(int)size())
		resize(Index+1,ZeroSVertex);
	at((unsigned)Index)=Vertex;
}

long long _CSExpandedDescendends::GetHash(long long& Core) const
{
	for(const_iterator it=begin();it!=end();it++)
	{
		Core*=HASH_FACTOR;
		//Core=it->GetHash(Core);
		Core=(long long)(*it);
	}
	return Core;
}

long long _CSExpandedCofactorValues::GetHash(long long& Core) const
{
	for(const_iterator it=begin();it!=end();it++)
	{
		Core*=HASH_FACTOR;
		Core=it->GetHash(Core);
	}
	return Core;
}

void _CSExpandedVertices::WriteToStream(iostream& stream, _CSimpleVerticesMapsSet& Set)
//void _CSExpandedVertices::WriteToStream(iostream& stream)
//iostream& _CSExpandedVertices::operator<<(iostream& stream) const
{
	if(empty())
	{
		stream<<"[ ]\n";
		return ;
	}
	stream<<"[";
	size_type i,_max;
	i=_max=size()-1;
	bool First=true;
	//const _CSimpleVertexContainer* pLastN0Vertex=NULL;
	for(;i>0;i--)
	{
		const _CSimpleVertexContainer& Vertex=at(i);
		if(Vertex.IsEmpty() || Vertex.Is0())
			continue;
		if(!First)
		{
			stream<<" + ";
		}
		else
			First=false;
		//pLastN0Vertex=&Vertex;
		stream<<"s^"<<i<<"*(";
		//stream<<Vertex<<")";
		Vertex.WriteToStream(stream,true,Set);
		//Vertex.WriteToStream(stream,true);
		stream<<")";
	}
	const _CSimpleVertexContainer& Vertex=at(0);
	if(!Vertex.IsEmpty() && !Vertex.Is0())
	{
		//pLastN0Vertex=&Vertex;
		stream<<" + (";
		Vertex.WriteToStream(stream,true,Set);
		//Vertex.WriteToStream(stream,true);
		stream<<")";
		//stream<<Vertex<<")";
	}
	stream<<"]\n";
	//if(pLastN0Vertex!=NULL)
	//{
	//	const _CSimpleVerticesMaps* pVertexMap=pLastN0Vertex->GetConnectionMap();
	//	if(pVertexMap!=NULL)
	//	{
	//		stream<<"\t External descendants:\n";
	//		stream<<(*pVertexMap);
	//	}
	//}
	//return stream;
}

_CSExpandedVertices* _CSExpandedUniqeSet::GetUnique(_CSExpandedVertices& Source)
{
	long long hash=HASH_CORE;
	hash=Source.GetHash(hash);
	RANGE range=m_Set.equal_range(hash);
	for(iterator it=range.first;it!=range.second;it++)
		if(it->second==Source)
			return &(it->second);
	return &(m_Set.insert(value_type(hash,Source))->second);
}


//iostream& operator<<(iostream& stream, const _CSExpandedVertices& Vertices)
//{	
//	return Vertices.operator<<(stream);
//}

//iostream& _CSimpleVertexMap::operator<<(iostream& stream) const
//{
//	for(ALL_MAPS::const_iterator ait=m_Maps.begin();ait!=m_Maps.end();ait++)
//	{
//		const string* Context=ait->first;
//		stream<<"\t\t";
//		if(Context->empty())
//			stream<<"Main Context: ";
//		else
//			stream<<"Context "<<*Context<<": ";
//		stream<<"{";
//		const BASIC_MAP& bMap=ait->second;
//		for(size_type i=0;i<m_NoOfDescandence;i++)
//		{
//			if(i>0)
//				stream<<", ";
//			bool IsDefined=false;
//			stream<<"e("<<i<<")=";
//			for(int j=m_MaxSPower;j>=0;j--)
//			{
//				TGlobal_Vertex_Id Id;
//				Id.PARTS.Circuit_Id=i;
//				Id.PARTS.Local_Id=j;
//				BASIC_MAP::const_iterator it=bMap.find(Id.Global_Id);
//				if(it==bMap.end())
//					continue;
//				if(IsDefined)
//					stream<<"+";
//				else
//					IsDefined=true;
//				if(j>0)
//					stream<<"s^"<<j<<"*";
//				stream<<"(";
//				(it->second).WriteToStream(stream,true);
//				//stream<<(it->second)<<")";
//				stream<<")";
//			}
//			if(!IsDefined)
//				stream<<"(0)";
//		}
//		stream<<"}\n";
//	}
//	return stream;
//}
//
//iostream& operator<<(iostream& stream, const _CSimpleVerticesMaps& VerticesMap)
//{
//	return VerticesMap.operator <<(stream);
//}


//_CSimpleVerticesMaps::_CBoundaryTerminals& _CSimpleVerticesMaps::GetBTMapInherited(const string& Context)
//{
//	static _CBoundaryTerminals Res;
//	_CContext2Map::iterator cit=m_MainMap.find(&Context);
//	if(cit!=m_MainMap.end())
//		return cit->second;
//	return Res;
//}
//
//_CSimpleVerticesMaps::T_INDEX 
//_CSimpleVerticesMaps::_CBoundaryTerminals::Boundary2LocalTerminal(_CSimpleVertexContainer& Vertex,const string* pContext)
//{
//	//BOUNDARY_2_LOCAL::iterator it=m_Boundary2Local.find(Vertex);
//	BOUNDARY_2_LOCAL::iterator it=Find(Vertex);
//	if(it!=m_Boundary2Local.end())
//		return it->second;
//	//pair<BOUNDARY_2_LOCAL::iterator,bool> Res=
//	it=	m_Boundary2Local.insert(BOUNDARY_2_LOCAL::value_type(Vertex,m_Counter));
//	//if(Res.second)
//	//	it=Res.first;
//	//else
//	//	RISEPDD(eUnexpectedCondition,"Change conversion _CSimpleVertexContainer to size_t");
//	m_Local2Boundary.insert(LOCAL_2_BOUNDARY::value_type(m_Counter,&(it->first)));
//	m_HardDefined.insert(HARD_DEFINITION::value_type(m_Counter,false));
//	m_PostTerminalContext.insert(POST_TERMINAL_CONTEXT::value_type(m_Counter,pContext));
//	return m_Counter++;
//}
//
//void _CSimpleVerticesMaps::TranslateLocalTerminals(_CSExpandedDescendends& Descendants, const string& Context)
//{
//	_CBoundaryTerminals& BT=m_MainMap[&Context];
//	for(_CSExpandedDescendends::iterator eit=Descendants.begin();eit!=Descendants.end();eit++)
//	{
//		_CSExpandedVertices& EVCS=*eit;
//		for(_CSExpandedVertices::iterator it=EVCS.begin();it!=EVCS.end();it++)
//		{
//			_CSimpleVertexContainer& Cont=*it;
//			if(Cont.Is0())
//				continue;
//			Cont=_CSimpleVertexContainer(BT.Boundary2LocalTerminal(Cont,&Context));
//		}
//	}
//}
//
//const  _CSimpleVertexContainer* _CSimpleVerticesMaps::_CBoundaryTerminals::GetIfExists(T_INDEX Index) const
//{
//	LOCAL_2_BOUNDARY::const_iterator it=m_Local2Boundary.find(Index);
//	return it==m_Local2Boundary.end()?NULL:it->second;
//}
//
////const _CSimpleVertexContainer* _CSimpleVerticesMaps::SearchSVertex(T_INDEX Index,const string*& pContext) const
////{
////	const _CSimpleVertexContainer* pResContainer=NULL;
////	_CContext2Map::const_iterator it=m_MainMap.find(pContext);
////	if(it!=m_MainMap.end())
////	{
////		pResContainer=it->second.GetIfExists(Index);
////		if(pResContainer==NULL)
////			pResContainer=SearchNextSVertex(Index,pContext);
////	}
////	else
////		pResContainer=SearchNextSVertex(Index,pContext);
////	return pResContainer;
////}
////
////const _CSimpleVertexContainer* _CSimpleVerticesMaps::SearchNextSVertex(T_INDEX Index,const string*& pContext) const
////{
////	const _CSimpleVertexContainer* pResContainer=NULL;
////	size_t Counter=0;
////	const string* pOldContext=pContext;
////	while(pContext!=NULL)
////	{
////		pContext=m_pBaseCircuit->GetParentContext(*pOldContext,Counter);
////		Counter++;
////		if(pContext!=NULL)
////		{
////			pResContainer=SearchSVertex(Index,pContext);
////			if(pResContainer!=NULL)
////				return pResContainer;
////		}
////	}
////	return pResContainer;
////}
//
//const _CSimpleVertexContainer& _CSimpleVerticesMaps::get(long long Index,const string*& pContext) const
//{
//	_CContext2Map::const_iterator cit=m_MainMap.find(pContext);
//	if(cit==m_MainMap.end())
//		RISEPDD(eUnexpectedCondition,(*pContext+" doesn't exist").c_str());
//	const _CBoundaryTerminals& BT=cit->second;
//	return BT.get(Index,pContext);
//	//const _CSimpleVertexContainer* pVertex=SearchSVertex(Index,pContext);
//	//if(pVertex!=NULL)
//	//	return *pVertex;
//	//else
//	//	RISEPDD(eUnexpectedCondition,":( Some Terminal Vertex must exists!");
//}
//
//_CSimpleVerticesMaps::_CBoundaryTerminals::BOUNDARY_2_LOCAL::iterator 
//_CSimpleVerticesMaps::_CBoundaryTerminals::Find(const _CSimpleVertexContainer& VC) 
//{
//	typedef BOUNDARY_2_LOCAL::iterator iterator;
//	pair<iterator,iterator> Res=
//		m_Boundary2Local.equal_range(VC);
//	for(iterator it=Res.first;it!=Res.second;it++)
//		if(it->first==VC)
//			return it;
//	return m_Boundary2Local.end();
//}
//
//_CSimpleVerticesMaps::T_INDEX 
//_CSimpleVerticesMaps::_CBoundaryTerminals::put(T_INDEX Index, _CSimpleVertexContainer& Value, const string* pPostTerminalContext)
//{
//	LOCAL_2_BOUNDARY::iterator it=m_Local2Boundary.find(Index);
//	POST_TERMINAL_CONTEXT::iterator itc=m_PostTerminalContext.find(Index);
//	if(it!=m_Local2Boundary.end())
//	{
//		ASSERTPDD(itc!=m_PostTerminalContext.end());
//		if(*(it->second)!=Value || (itc->second!=pPostTerminalContext))
//		{
//			BOUNDARY_2_LOCAL::iterator bit=Find(Value);
//			if(bit!=m_Boundary2Local.end())
//				return bit->second;
//			else
//				Index=++m_Counter;
//		}
//		else
//			return Index;
//	}
//	m_Counter=max((unsigned long long)m_Counter,(unsigned long long)Index);
//	//pair<BOUNDARY_2_LOCAL::iterator,bool> Res=m_Boundary2Local.insert(BOUNDARY_2_LOCAL::value_type(Value,Index));
//	BOUNDARY_2_LOCAL::iterator bit
//		=m_Boundary2Local.insert(BOUNDARY_2_LOCAL::value_type(Value,Index));
//	//if(!Res.second)
//	//	RISEPDD(eUnexpectedCondition,"Improve _CSimpleVertexContainer size_t conversion");
//	//m_Local2Boundary.insert(LOCAL_2_BOUNDARY::value_type(Index,&(Res.first->first)));
//	m_Local2Boundary.insert(LOCAL_2_BOUNDARY::value_type(Index,&(bit->first)));
//	m_HardDefined.insert(HARD_DEFINITION::value_type(Index,true));
//	m_PostTerminalContext.insert(POST_TERMINAL_CONTEXT::value_type(Index,pPostTerminalContext));
//	return Index;
//}
//
//const _CSimpleVertexContainer& _CSimpleVerticesMaps::_CBoundaryTerminals::get(T_INDEX Index, const string*& pPostTerminalContext) const
//{
//	LOCAL_2_BOUNDARY::const_iterator it1=m_Local2Boundary.find(Index);
//	POST_TERMINAL_CONTEXT::const_iterator it2=m_PostTerminalContext.find(Index);
//	if(it1==m_Local2Boundary.end() || it2==m_PostTerminalContext.end())
//		RISEPDD(eUnexpectedCondition,"No boundary Vertex, or no context");
//	pPostTerminalContext=it2->second;
//	return *(it1->second);
//}
//
//_CSimpleVerticesMaps::T_INDEX 
//_CSimpleVerticesMaps::put(T_INDEX Index,const string& Context, _CSimpleVertexContainer& Value, 
//						  const string& PostTerminalContext, const string& BaseContext)
//{
//	//m_MainMap[&Context][Index].VertIndex()=Value;
//	return m_MainMap[&Context].put(Index,Value,&PostTerminalContext);
//}
//void _CSimpleVerticesMaps::_CBoundaryTerminals::WriteToStream(iostream& stream,_CSimpleVerticesMapsSet& Set) const
////iostream& _CSimpleVerticesMaps::_CBoundaryTerminals::operator<<(iostream& stream) const
//{
//	stream<<"[";
//	bool First=true;
//	for(size_t i=0;i<m_Counter;i++)
//	{
//		if(!First)
//			stream<<", ";
//		LOCAL_2_BOUNDARY::const_iterator it=m_Local2Boundary.find(i);
//		if(it==m_Local2Boundary.end())
//			continue;
//		TGlobal_Vertex_Id Id;
//		Id.Global_Id=it->second->m_lVertex;
//		//stream<<"e("<<i<<")=("<<Id.PARTS.Circuit_Id<<"."<<Id.PARTS.Local_Id<<")";
//		stream<<"e("<<i<<")=(";
//		it->second->WriteToStream(stream,true, Set);
//		stream<<")";
//		First=false;
//	}
//	stream<<"]";
//	//return stream;
//}
//
////iostream& operator<<(iostream& stream,const _CSimpleVerticesMaps::_CBoundaryTerminals& Terminals) 
////{
////	return Terminals.operator <<(stream);
////}
//
//void _CSimpleVerticesMaps::WriteToStream(iostream& stream,_CSimpleVerticesMapsSet& Set) const
////iostream& _CSimpleVerticesMaps::operator<<(iostream& stream) const
//{
//	for(_CContext2Map::const_iterator cit=m_MainMap.begin();cit!=m_MainMap.end();cit++)
//	{
//		const string* pContext=cit->first;
//		stream<<"\t\t\t";
//		if(pContext->empty())
//			stream<<"Main Context: ";
//		else
//			stream<<"Context "<<*pContext<<": ";
//		const _CBoundaryTerminals& BoundaryTerminals=cit->second;
//		BoundaryTerminals.WriteToStream(stream,Set);
//		stream<<endl;
//	}
//	//return stream;
//}
//
//_CSimpleVerticesMaps& _CSimpleVerticesMaps::operator=(const _CSimpleVerticesMaps& Source)
//{
//	m_pBaseCircuit=Source.m_pBaseCircuit;
//	m_MainMap=Source.m_MainMap;
//	return *this;
//}
//_CSimpleVerticesMaps::_CBoundaryTerminals& _CSimpleVerticesMaps::_CBoundaryTerminals::operator=(const _CBoundaryTerminals& Source)
//{
//	m_Counter=Source.m_Counter;
//	m_Boundary2Local=Source.m_Boundary2Local;
//	m_Local2Boundary=Source.m_Local2Boundary;
//	m_HardDefined=Source.m_HardDefined;
//	return *this;
//}
//bool _CSimpleVerticesMaps::_CBoundaryTerminals::operator==(const _CBoundaryTerminals& Right) const
//{
//	return (m_Counter==Right.m_Counter) &&
//		(m_HardDefined==Right.m_HardDefined) &&
//		(m_Local2Boundary==Right.m_Local2Boundary) &&
//		(m_Boundary2Local==Right.m_Boundary2Local);
//}
//
//bool _CSimpleVerticesMaps::operator==(const _CSimpleVerticesMaps& Right) const
//{
//	return (m_pBaseCircuit==Right.m_pBaseCircuit) &&
//		(m_MainMap==Right.m_MainMap);
//}

//**
//_CSubCircuitVerticesMap::_CBoundaryTerminals& _CSubCircuitVerticesMap::GetBTMapInherited(const string& Context)
//{
//	static _CBoundaryTerminals Res;
//	_CContext2Map::iterator cit=m_MainMap.find(&Context);
//	if(cit!=m_MainMap.end())
//		return cit->second;
//	return Res;
//}

//_CSubCircuitVerticesMap::T_INDEX 
//_CSubCircuitVerticesMap::_CBoundaryTerminals::Boundary2LocalTerminal(size_t ProposedEntry, size_t ProposedSPower,
//																	 _CSimpleVertexContainer& Vertex,const string* pContext)
//{
//	//BOUNDARY_2_LOCAL::iterator it=m_Boundary2Local.find(Vertex);
//	BOUNDARY_2_LOCAL::iterator it=Find(Vertex);
//	if(it!=m_Boundary2Local.end())
//		return it->second;
//	//pair<BOUNDARY_2_LOCAL::iterator,bool> Res=
//	T_INDEX PropId=_l2ll(ProposedEntry,ProposedSPower);
//	ASSERTPDD(m_Local2Boundary.find(PropId)==m_Local2Boundary.end());
//	it=	m_Boundary2Local.insert(BOUNDARY_2_LOCAL::value_type(Vertex,PropId));
//	//if(Res.second)
//	//	it=Res.first;
//	//else
//	//	RISEPDD(eUnexpectedCondition,"Change conversion _CSimpleVertexContainer to size_t");
//	m_Local2Boundary.insert(LOCAL_2_BOUNDARY::value_type(PropId,&(it->first)));
//	m_HardDefined.insert(HARD_DEFINITION::value_type(PropId,false));
//	m_PostTerminalContext.insert(POST_TERMINAL_CONTEXT::value_type(PropId,pContext));
//	return PropId;
//}

bool _CSubCircuitVerticesMap::_Cache::check_if_exists(size_t& Entry, size_t& sPower, _CSimpleVertexContainer& VC)
{
	long long Hash=HASH_CORE;
	Hash=SCHash(Hash,VC);
	pair<MAP::iterator,MAP::iterator> FR=m_Map.equal_range(Hash);
	for(MAP::iterator it=FR.first;it!=FR.second;it++)
		if(it->second.m_VC==VC && sPower==it->second.m_sPower)
		{
			Entry=it->second.m_Entry;
			//sPower=it->second.m_sPower;
			return true;
		}
	m_Map.insert(MAP::value_type(Hash,__Container(Entry,sPower,VC)));
	return false;
}

long long _CSubCircuitVerticesMap::GetHash(const string* pContext)
{
	if(!m_ValidHash)
	{
		m_MyHash=HASH_CORE;
		_CContextMap::iterator it=m_Map.find(&EmptyString);
		if(it==m_Map.end())
			RISEPDD(eUnexpectedCondition,"Empty Context is obligatory");
		_CSCoffTab& CoffTab=it->second;
		size_t _en_size=CoffTab.size();
		for(size_t en=0;en<_en_size;en++)
		{
			_CSCoeffsTab& CoeffTab=CoffTab[en];
			size_t _sp_size=CoeffTab.size();
			for(size_t sp=0;sp<_sp_size;sp++)
			{
				__Int_Type& IT=CoeffTab[sp];
				_CSimpleVertexContainer& Cont=IT.first;
				m_MyHash=Cont.ConsiderInHash(m_MyHash);
			}
		}
		m_ValidHash=true;
	}
	return m_MyHash;
}

void _CSubCircuitVerticesMap::TranslateLocalTerminals(_CSExpandedDescendends& Descendants, const string& Context)
{
	_Cache Cache;
	size_t _esize=Descendants.size();
	for(size_t en=0;en<_esize;en++)
	{
		size_t enn=en/*_esize-en-1*/;
		_CSExpandedVertices* pEVCS=Descendants[enn];
		size_t _ssize=pEVCS->size();
		_CSExpandedVertices OutEVCs;
		OutEVCs.resize(_ssize);
		bool IsChanged=false;
		for(size_t s=0;s<_ssize;s++)
		{
			size_t sn=s/*_ssize-s-1*/,
				rsn=sn,
				renn=enn;
			_CSimpleVertexContainer& Cont=pEVCS->at(sn);
			//if(!Cont.Is01M1())
			if(!Cont.Is0())
			{
				if(!Cache.check_if_exists(renn,rsn,Cont))
				{
					__Int_Type& A=force_at(Context,renn,(unsigned short)rsn);
					if(!A.IsEmpty())
					{
						int a=5;
					}
					A=__Int_Type(Cont,&Context);
				}
				OutEVCs[sn]=_CSimpleVertexContainer(_l2ll(renn,0/*rsn*/));
				IsChanged=true;
			}
			else
				OutEVCs[sn]=Cont;
 		}
		if(IsChanged)
			Descendants[enn]=m_pBaseCircuit->GetUniqueSEExpandedVertices(OutEVCs);
	}
	if(m_Map.empty())
		EnsureMap(Context);
}

//const  _CSimpleVertexContainer* _CSubCircuitVerticesMap::_CBoundaryTerminals::GetIfExists(T_INDEX Index) const
//{
//	LOCAL_2_BOUNDARY::const_iterator it=m_Local2Boundary.find(Index);
//	return it==m_Local2Boundary.end()?NULL:it->second;
//}

//const _CSimpleVertexContainer* _CSimpleVerticesMaps::SearchSVertex(T_INDEX Index,const string*& pContext) const
//{
//	const _CSimpleVertexContainer* pResContainer=NULL;
//	_CContext2Map::const_iterator it=m_MainMap.find(pContext);
//	if(it!=m_MainMap.end())
//	{
//		pResContainer=it->second.GetIfExists(Index);
//		if(pResContainer==NULL)
//			pResContainer=SearchNextSVertex(Index,pContext);
//	}
//	else
//		pResContainer=SearchNextSVertex(Index,pContext);
//	return pResContainer;
//}
//
//const _CSimpleVertexContainer* _CSimpleVerticesMaps::SearchNextSVertex(T_INDEX Index,const string*& pContext) const
//{
//	const _CSimpleVertexContainer* pResContainer=NULL;
//	size_t Counter=0;
//	const string* pOldContext=pContext;
//	while(pContext!=NULL)
//	{
//		pContext=m_pBaseCircuit->GetParentContext(*pOldContext,Counter);
//		Counter++;
//		if(pContext!=NULL)
//		{
//			pResContainer=SearchSVertex(Index,pContext);
//			if(pResContainer!=NULL)
//				return pResContainer;
//		}
//	}
//	return pResContainer;
//}

const _CSimpleVertexContainer& _CSubCircuitVerticesMap::get(size_t EntryNo, size_t SPower,const string*& pContext) const
{
	const _CSimpleVertexContainer* pRes=smart_get(EntryNo,SPower,pContext);
	if(pRes==NULL)
		RISEPDD(eUnexpectedCondition,":( Some Terminal Vertex must exists!");
	return *pRes;

}

const _CSimpleVertexContainer* _CSubCircuitVerticesMap::smart_get(size_t EntryNo, size_t SPower,const string*& pContext) const
{
	const __Int_Type* pIT=smart_at(*pContext,EntryNo,(unsigned short)SPower);
	if(pIT==NULL || pIT->first.IsEmpty())
		return NULL;
	pContext=pIT->second;
	return &pIT->first;
}

const _CSimpleVertexContainer* _CSubCircuitVerticesMap::smart_get(T_INDEX Index,const string*& pContext) const
{
	size_t Entry=_Hi(Index),
		sPower=_Lo(Index);
	return smart_get(Entry,sPower,pContext);
}

const _CSimpleVertexContainer& _CSubCircuitVerticesMap::get(T_INDEX Index,const string*& pContext) const
{
	const _CSimpleVertexContainer* pRes=smart_get(Index,pContext);
	if(pRes==NULL)
		RISEPDD(eUnexpectedCondition,":( Some Terminal Vertex must exists!");
	return *pRes;
}

_CSubCircuitVerticesMap::T_INDEX 
_CSubCircuitVerticesMap::put(size_t Entry, size_t sPower,const string& Context, _CSimpleVertexContainer& Value, 
		const string& PostTerminalContext, const string& BaseContext)
{
	T_INDEX Index=_l2ll(Entry,sPower);
	__Int_Type* pNewPlace=&force_at(Context,Entry,(unsigned short)sPower);
	if(!pNewPlace->first.IsEmpty())
	{
		if(pNewPlace->first==Value && pNewPlace->second==&PostTerminalContext)
			return Index;
		_CSCoffTab& SCT=m_Map[&Context];
		size_t _esize=SCT.size();
		for(size_t e=0;e<_esize;e++)
		{
			_CSCoeffsTab& SCOEF=SCT[e];
			size_t _ssize=SCOEF.size();
			for(size_t s=0;s<_ssize;s++)
			{
				__Int_Type& OldPlace=SCOEF[s];
				if(OldPlace.first==Value && OldPlace.second==&PostTerminalContext)
					return _l2ll(e,s);
			}
		}
		size_t NewEntry=m_Map[&BaseContext].size();
		pNewPlace=&force_at(Context,NewEntry,(unsigned short)sPower);
		Index=_l2ll(NewEntry,(unsigned short)sPower);
	}
	pNewPlace->first=Value;
	pNewPlace->second=&PostTerminalContext;
	//m_MainMap[&Context][Index].VertIndex()=Value;
	//return m_MainMap[&Context].put(Index,Value,&PostTerminalContext);
	return Index;
}

_CSubCircuitVerticesMap::T_INDEX 
_CSubCircuitVerticesMap::put(T_INDEX Index,const string& Context, _CSimpleVertexContainer& Value,
							 const string& PostTerminalContext, const string& BaseContext)
{
	size_t Entry=_Hi(Index),
		sPower=_Lo(Index);
	__Int_Type* pNewPlace=&force_at(Context,Entry,(unsigned short)sPower);
	if(!pNewPlace->first.IsEmpty())
	{
		if(pNewPlace->first==Value && pNewPlace->second==&PostTerminalContext)
			return Index;
		_CSCoffTab& SCT=m_Map[&Context];
		size_t _esize=SCT.size();
		for(size_t e=0;e<_esize;e++)
		{
			_CSCoeffsTab& SCOEF=SCT[e];
			size_t _ssize=SCOEF.size();
			for(size_t s=0;s<_ssize;s++)
			{
				__Int_Type& OldPlace=SCOEF[s];
				if(OldPlace.first==Value && OldPlace.second==&PostTerminalContext)
					return _l2ll(e,s);
			}
		}
		size_t NewEntry=m_Map[&BaseContext].size();
		pNewPlace=&force_at(Context,NewEntry,(unsigned short)sPower);
		Index=_l2ll(NewEntry,sPower);
	}
	pNewPlace->first=Value;
	pNewPlace->second=&PostTerminalContext;
	//m_MainMap[&Context][Index].VertIndex()=Value;
	//return m_MainMap[&Context].put(Index,Value,&PostTerminalContext);
	return Index;
}
//void _CSubCircuitVerticesMap::_CBoundaryTerminals::WriteToStream(iostream& stream,_CSimpleVerticesMapsSet& Set) const
////iostream& _CSimpleVerticesMaps::_CBoundaryTerminals::operator<<(iostream& stream) const
//{
//	stream<<"[";
//	bool First=true;
//	for(size_t i=0;i<m_Counter;i++)
//	{
//		if(!First)
//			stream<<", ";
//		LOCAL_2_BOUNDARY::const_iterator it=m_Local2Boundary.find(i);
//		if(it==m_Local2Boundary.end())
//			continue;
//		TGlobal_Vertex_Id Id;
//		Id.Global_Id=it->second->m_lVertex;
//		//stream<<"e("<<i<<")=("<<Id.PARTS.Circuit_Id<<"."<<Id.PARTS.Local_Id<<")";
//		stream<<"e("<<i<<")=(";
//		it->second->WriteToStream(stream,true, Set);
//		stream<<")";
//		First=false;
//	}
//	stream<<"]";
//	//return stream;
//}

//iostream& operator<<(iostream& stream,const _CSimpleVerticesMaps::_CBoundaryTerminals& Terminals) 
//{
//	return Terminals.operator <<(stream);
//}

void _CSubCircuitVerticesMap::WriteToStream(iostream& stream,_CSimpleVerticesMapsSet& Set) const
{
	for(_CContextMap::const_iterator it=m_Map.begin();it!=m_Map.end();it++)
	{
		const string* pContext=it->first;
		stream<<"\t\t\t";
		if(pContext->empty())
			stream<<"The main context: ";
		else
			stream<<"Context "<<*pContext<<": ";
		const _CSCoffTab& SCT=it->second;
		stream<<"[";
		size_t _esize=SCT.size();
		bool FirstEntry=true;
		for(size_t e=0;e<_esize;e++)
		{
			size_t en=_esize-1-e;
			const _CSCoeffsTab& SCET=SCT[en];
			size_t _ssize=SCET.size();
			if(_ssize==0)
				continue;
			if(!FirstEntry)
				stream<<", ";
			else
				FirstEntry=false;
			stream<<"e("<<en<<")=(";
			bool FirstPower=true;
			for(size_t s=0;s<_ssize;s++)
			{
				size_t sn=_ssize-1-s;
				const __Int_Type& IT=SCET[sn];
				if(IT.first.IsEmpty())
					continue;
				if(!FirstPower)
					stream<<" + ";
				else
					FirstPower=false;
				IT.first.WriteToStream(stream,true,Set);
				if(sn!=0)
					stream<<"*s^"<<sn;
				if(IT.second!=pContext)
					stream<<">>"<<*(IT.second)<<"<<";
			}
			stream<<")";
		}
		stream<<"]"<<endl;
	}
	//for(_CContext2Map::const_iterator cit=m_MainMap.begin();cit!=m_MainMap.end();cit++)
	//{
	//	const string* pContext=cit->first;
	//	stream<<"\t\t\t";
	//	if(pContext->empty())
	//		stream<<"Main Context: ";
	//	else
	//		stream<<"Context "<<*pContext<<": ";
	//	const _CBoundaryTerminals& BoundaryTerminals=cit->second;
	//	BoundaryTerminals.WriteToStream(stream,Set);
	//	stream<<endl;
	//}
	//return stream;
}

_CSubCircuitVerticesMap& _CSubCircuitVerticesMap::operator=(const _CSubCircuitVerticesMap& Source)
{
	m_pBaseCircuit=Source.m_pBaseCircuit;
	m_Map=Source.m_Map;
	return *this;
}
//_CSubCircuitVerticesMap::_CBoundaryTerminals& _CSubCircuitVerticesMap::_CBoundaryTerminals::operator=(const _CBoundaryTerminals& Source)
//{
//	m_Counter=Source.m_Counter;
//	m_Boundary2Local=Source.m_Boundary2Local;
//	m_Local2Boundary=Source.m_Local2Boundary;
//	m_HardDefined=Source.m_HardDefined;
//	return *this;
//}
//bool _CSubCircuitVerticesMap::_CBoundaryTerminals::operator==(const _CBoundaryTerminals& Right) const
//{
//	return (m_Counter==Right.m_Counter) &&
//		(m_HardDefined==Right.m_HardDefined) &&
//		(m_Local2Boundary==Right.m_Local2Boundary) &&
//		(m_Boundary2Local==Right.m_Boundary2Local);
//}

bool _CSubCircuitVerticesMap::operator==(const _CSubCircuitVerticesMap& Right) const
{
	return (m_pBaseCircuit==Right.m_pBaseCircuit) &&
		(m_Map==Right.m_Map);
}
bool _CSubCircuitVerticesMap::IsContextEql(_CSubCircuitVerticesMap& Right, const string* pContext) 
{
	if(GetHash(pContext)!=Right.GetHash(pContext))
		return false;
	if(m_pBaseCircuit!=Right.m_pBaseCircuit)
		return false;
	const _CSCoffTab* pTab=m_Map.smart_at(pContext);
	const _CSCoffTab* pRTab=Right.m_Map.smart_at(pContext);
	if(pTab==NULL || pRTab==NULL)
		return false;
	return (*pTab)==(*pRTab);
}

bool operator==(_CSubCircuitVerticesMap& Left, _CSubCircuitVerticesMap& Right)
{
	return Left.operator==(Right);
}
//**




//size_t _CSimpleVerticesMapsSet::Add(const _CSimpleVerticesMaps* pVertex)
size_t _CSimpleVerticesMapsSet::Add(const _CSubCircuitVerticesMap* pVertex)
{
	CONT_2_INDEX::iterator it=m_Cont2Index.find(pVertex);
	if(it!=m_Cont2Index.end())
		return it->second;
	m_Index2Vector.push_back(pVertex);
	size_t Index=m_Index2Vector.size(); Index--;
	m_Cont2Index.insert(CONT_2_INDEX::value_type(pVertex,Index));
	return Index;
}

void _CSimpleVerticesMapsSet::WriteToStream(iostream& stream)
{
	if(size()==0) return;
	stream<<endl;
	stream<<"\tLocal terminals connections:\n";
	for(size_t i=0;i<size();i++)
	{
		stream<<"\t\t Terminals set No. "<<i<<":\n";
		m_Index2Vector[i]->WriteToStream(stream,*this);
		stream<<endl;
	}
	stream<<endl;
}

_CSubCircuitVerticesMapBuffer::~_CSubCircuitVerticesMapBuffer()
{
	for(iterator it=begin();it!=end();it++)
	{
		delete it->second.first;
		//erase(it);
	}
}

_CSubCircuitVerticesMap* _CSubCircuitVerticesMapBuffer::RegisterMap(_CSubCircuitVerticesMap* pMap, bool DeleteIfAlreadyExist)
{
	iterator it=__Find(pMap);
	if(it!=end())
	{
		if(DeleteIfAlreadyExist)
			delete pMap;
		return it->second.first;
	}
	long long Key=pMap->GetHash(&EmptyString);
	m_Vector.force_at(m_Counter)=pMap;
	insert(value_type(Key,ENTRY(pMap,m_Counter++)));
	pMap->m_IsRegistered=true;
	return pMap;
}

_CSubCircuitVerticesMapBuffer::iterator 
_CSubCircuitVerticesMapBuffer::__Find(_CSubCircuitVerticesMap* pMap)
{
	long long Key=pMap->GetHash(&EmptyString);
	pair<iterator, iterator> r=equal_range(Key);
	for(iterator it=r.first;it!=r.second;it++)
		if(*(it->second.first)==*pMap)
			return it;
	return end();
}

size_t _CSubCircuitVerticesMapBuffer::Map2Index(_CSubCircuitVerticesMap* pMap)
{
	iterator it=__Find(pMap);
	if(it!=end())
		return it->second.second;
	return 0;
}

void _CSubCircuitVerticesMapBuffer::WriteToStream(iostream& stream)
{
//	if(size()==0) return;
//	stream<<endl;
//	stream<<"\tLocal terminals connections:\n";
//	for(size_t i=0;i<size();i++)
//	{
//		stream<<"\t\t Terminals set No. "<<i<<":\n";
//		m_Index2Vector[i]->WriteToStream(stream,*this);
//		stream<<endl;
//	}
//	stream<<endl;
}


//************************class _CSimilarVertMaps

//void _CSimilarVertMaps::Add(_CSubCircuitVerticesMap* pMap, unsigned AuxId)
//{
//	//push_back(PAIR(pMap,AuxId));
//	insert(PAIR(pMap,AuxId));
//}
//
//bool _CSimilarVertMaps::FindProperMap(const string* pContext,long long  lTerminalVertexToCheck,
//		const _CSimpleVertexContainer* pExpectedVertex,
//		unsigned& OldAuxId)
//{
//	unsigned GlobalAuxId=0;
//	for(iterator it=begin();it!=end();it++)
//	{
//		PAIR pr=(*it);
//		_CSubCircuitVerticesMap* pMap=pr.first;
//		const string* pCurrContext;
//		const _CSimpleVertexContainer* pNextVertex=pMap->smart_get(lTerminalVertexToCheck,pCurrContext);
//		if(pNextVertex==NULL)
//			continue;
//		if(*pNextVertex==*pExpectedVertex)
//		{
//			OldAuxId=pr.second;
//			return true;
//		}
//		else
//			GlobalAuxId=max(GlobalAuxId,pr.second);
//	}
//	OldAuxId=GlobalAuxId;
//	return false;
//}

//************************class _CSimilarVertMaps

//_CSimilarVertMapsStack::~_CSimilarVertMapsStack()
//{
//	for each(_CSimilarVertMaps* pMap in *this)
//		delete pMap;
//}
//
//void _CSimilarVertMapsStack::push_back_new()
//{
//	push_back(new _CSimilarVertMaps());
//}
//
//bool _CSimilarVertMapsStack::pop_back_and_destroy()
//{
//	if(empty())
//		return false;
//	delete back();
//	pop_back();
//	return true;
//}
//_CSimilarVertMaps* _CSimilarVertMapsStack::smart_back()
//{
//	return empty()?NULL:list<_CSimilarVertMaps*>::back();
//}
//
//bool _CSimilarVertMapsStack::pop_back_and_store(_CSimilarVertMaps& VMToStore)
//{
//	if(empty())
//		return false;
//	_CSimilarVertMaps* pVM=back();
//	VMToStore=*pVM;
//	delete pVM;
//	pop_back();
//	return true;
//}
//
//void _CSimilarVertMapsStack::push_back_and_copy(const _CSimilarVertMaps* pSource)
//{
//	push_back(pSource==NULL?new _CSimilarVertMaps():new _CSimilarVertMaps(*pSource));
//}

