#include "StdAfx.h"
#include "SubCircuit.h"
#include "PrjException.h"
#include "search_filer.h"
#include "time_stamp.h"
#include <limits>

_CSubCircuit::_CSubCircuit(_CCircuit* Owner,bool OrderOptimalization,bool UseNewAnalyser ,const string& Name,
						   //const _CStrNodes& InjectedBoundaryNodes, const string& GlobalRefNode, bool MustBeNew,const SIZE_T_SET* pDeletionAdmissible):
						   const _CStrNodes& InjectedBoundaryNodes, const string& GlobalRefNode, bool MustBeNew,const SIZE_T_RANGE* pDeletionAdmissible):
_CCircuit(true,OrderOptimalization,UseNewAnalyser,GlobalRefNode),m_Owner(Owner),m_BoundaryNodes(InjectedBoundaryNodes), 
m_IsReady(false),m_Name(Name),m_InterfaceOffset(0),m_NumericOffset(0),m_MustBeNew(MustBeNew),m_HollowOutput(-1),m_IsHollowDetermined(false),
m_DeletionAdmissibleDef(pDeletionAdmissible!=NULL),m_IsSpecialNetDynamic(false),
m_MaxPossibleDeletions(UINT_MAX),m_pNewDescndantsState(NULL)
//,m_DeletionAdmissibleDef?m_DeletionAdmissible(*pDeletionAdmissible):m_DeletionAdmissible()
{
	m_BoundaryParameters.SetOwner(this);
	m_MapsBuffer.SetCircuit(this);
	m_NumResultsCache.SetOwner(this);
	if(m_DeletionAdmissibleDef)
		m_DeletionAdmissible=*pDeletionAdmissible;
//	m_BoundaryCofactors.SetOwner(this);
	for each(string Node in InjectedBoundaryNodes)
	{
		m_BoundaryNodesMap.insert(BOUNDARY_NODES_TYPE(Node,TranslateOrAddNode(Node)));
	}
}

_CSubCircuit::_CSubCircuit(_CCircuit* Owner,bool OrderOptimalization,bool UseNewAnalyser ,const string& Name, const path_str FileName,const string GlobalRefNode, bool MustBeNew):
_CCircuit(true,OrderOptimalization,UseNewAnalyser,GlobalRefNode),m_Owner(Owner), m_IsReady(false),m_Name(Name),m_InterfaceOffset(0),m_NumericOffset(0),m_MustBeNew(MustBeNew),m_ModelPath(FileName),
m_HollowOutput(-1),m_IsHollowDetermined(false),m_DeletionAdmissibleDef(false),m_IsSpecialNetDynamic(false),
m_MaxPossibleDeletions(UINT_MAX),m_pNewDescndantsState(NULL)
{
//	m_BoundaryCofactors.SetOwner(this);
	m_BoundaryParameters.SetOwner(this);
	m_MapsBuffer.SetCircuit(this);
	m_NumResultsCache.SetOwner(this);
}

_CSubCircuit::_CSubCircuit(_CCircuit* Owner, const string& Name, const string& GlobalRefNode, bool MustBeNew,bool UseNewAnalyser):
_CCircuit(true,false,UseNewAnalyser,GlobalRefNode),m_Owner(Owner), 
m_IsReady(false),m_Name(Name),m_InterfaceOffset(0),m_NumericOffset(0),m_MustBeNew(MustBeNew),
m_HollowOutput(-1),m_IsHollowDetermined(false),
m_DeletionAdmissibleDef(true),m_IsSpecialNetDynamic(true)
,m_MaxPossibleDeletions(UINT_MAX),m_pNewDescndantsState(NULL)
{
	m_BoundaryParameters.SetOwner(this);
	m_MapsBuffer.SetCircuit(this);
	m_NumResultsCache.SetOwner(this);
}

void _CSubCircuit::DefineBoundaryBounds(const _CStrNodes& InjectedBoundaryNodes)
{
	m_BoundaryNodes=InjectedBoundaryNodes;
	for each(string Node in InjectedBoundaryNodes)
	{
		m_BoundaryNodesMap.insert(BOUNDARY_NODES_TYPE(Node,TranslateOrAddNode(Node)));
	}
}
void _CSubCircuit::DefineBoundaryBoundsAndConnect(const _CIntNodes& InjectedConnectionNodes)
{
	m_BoundaryNodes.clear();
	m_BoundaryNodesMap.clear();
	for each(int Node in InjectedConnectionNodes)
	{
		AddBoudaryNode(Node);
	}
}
void _CSubCircuit::ForceBoundariesAndConnect(const INT_INT_MAP& Connections)
{
	//INT_INT_MULT_MAP Nodes2Exchange;
	int BoundariesCounter=0;
	m_BoundaryNodes.clear();
	m_BoundaryNodesMap.clear();
	for(INT_INT_MAP::const_iterator it=Connections.begin(),_e=Connections.end();
		it!=_e;it++,BoundariesCounter++)
	{
		int ForcedLPin=it->first;
		int GPin=it->second;
		//if(ForcedLPin>BoundariesCounter)
		//{
		//	Nodes2Exchange[BoundariesCounter]=ForcedLPin;
		//	Nodes2Exchange[ForcedLPin]=BoundariesCounter;
		//}
		AddBoudaryNode(GPin);
	}
	//if(!Nodes2Exchange.empty())
	//{
	//	_CExchangMapWraper Wraper(Nodes2Exchange);
	//	m_LocalNodes.ExchangeNumbers(Wraper);
	//	m_IdealComponentsList.ExchangeNumbers(Wraper);
	//	m_LocalComponentsList.ExchangeNumbers(Wraper);
	//}
}
void _CSubCircuit::ReorderNodesIfNeccessary()
{
	INT_INT_MAP Nodes2Exchange;
	int BoundariesCounter=0;
	for(_CStrNodes::iterator sit=m_BoundaryNodes.begin(),_se=m_BoundaryNodes.end();
		sit!=_se;sit++,BoundariesCounter++)
	{
		BOUNDARY_NODES_MAP::iterator mit=m_BoundaryNodesMap.find(*sit);
		ASSERTPDD(mit!=m_BoundaryNodesMap.end());
		if(mit->second!=BoundariesCounter)
		{
			Nodes2Exchange[BoundariesCounter]=mit->second;
			Nodes2Exchange[mit->second]=BoundariesCounter;
			mit->second=BoundariesCounter;
		}
	}
	if(!Nodes2Exchange.empty())
	{
		_CExchangMapWraper Wraper(Nodes2Exchange);
		m_LocalNodes.ExchangeNumbers(Wraper);
		m_IdealComponentsList.ExchangeNumbers(Wraper);
		m_LocalComponentsList.ExchangeNumbers(Wraper);
	}
}

bool _CSubCircuit::AddBoudaryNode(string& Node)
{
	if(m_BoundaryNodesMap.find(Node)!=m_BoundaryNodesMap.end())
		return false;
	int iNode=TranslateOrAddNode(Node);
	m_BoundaryNodes.push_back(Node);
	m_BoundaryNodesMap.insert(BOUNDARY_NODES_TYPE(Node,iNode));
	return true;
}

bool _CSubCircuit::AddBoudaryNode(int Node)
{
	if(Node==0 && m_IsGlobal0Node)
		return true;
	string StrNode=TranslateNode(Node);
	if(StrNode.empty() || m_BoundaryNodesMap.find(StrNode)!=m_BoundaryNodesMap.end())
		return false;
	m_BoundaryNodes.push_back(StrNode);
	m_BoundaryNodesMap.insert(BOUNDARY_NODES_TYPE(StrNode,Node));
	return true;
}

_CSubCircuit::~_CSubCircuit(void)
{
	delete m_pNewDescndantsState;
}

_CSubCircuit* _CSubCircuit::GetSubcircuit(const string& name)
{
	return m_Owner->GetSubcircuit(name);
}

_CSubCircuit* _CSubCircuit::GetSubcircuit(long Id)
{
	return m_Owner->GetSubcircuit(Id);
}

void _CSubCircuit::PlugComponents(_CIntegrityTable& IntegrityTable)
{
	_CCircuit::PlugComponents(IntegrityTable);
	if(m_IsGlobal0Node)
		IntegrityTable.MarkBoundaryNode(0);
	for(_CStrNodes::iterator it=m_BoundaryNodes.begin();it!=m_BoundaryNodes.end();it++)
		IntegrityTable.MarkBoundaryNode(m_LocalNodes.ConvertOrAddNode(*it));
}

void _CSubCircuit::CleanExtraMaps()
{
	for(MVERTEX_2_DELS::iterator it=m_Vertex2Dels.begin(),_e=m_Vertex2Dels.end();it!=_e;)
	{
		if(it->first->IsUseless())
			it=m_Vertex2Dels.erase(it);
		else
			it++;
	}
	//for(MVERTEX_2_SIGN::iterator it=m_Vertex2Sign.begin(),_e=m_Vertex2Sign.end();it!=_e;)
	//{
	//	if(it->first->IsUseless())
	//		it=m_Vertex2Sign.erase(it);
	//	else
	//		it++;
	//}
	for(DELS_2_MVERTEX::iterator it=m_Dels2VertexMap.begin(),_e=m_Dels2VertexMap.end();it!=_e;)
	{
		if(it->second->IsUseless())
			it=m_Dels2VertexMap.erase(it);
		else
			it++;
	}
}

void _CSubCircuit::PrepareSubModelPDD(bool IgnoreSgnPropagation)
{
	if(m_IsReady)
		return;
	if(m_MustBeNew)
	{
		const string& RFileName=GetGlobalRaportFileName();
		time_stamp stamp(RFileName.empty()?"Raport.txt":RFileName,"Preparing of model: "+wstr2str(GetSubcircuitModelPath()));
		PrepareCofactors();
		PreparePromissing();
		CreateModelPDD();
		MVERTEX_2_SIGN MVert2Sgn;
		FinalizeLeaves(MVert2Sgn);
		if(m_UseNewAnalyser)
			m_pNewDescndantsState=new _CBounadryStatesVector(m_pNewAnalyxer->GetOriginalDescState());
		ClearGarbage(&MVert2Sgn,IgnoreSgnPropagation);
		StoreModel();
	}
	else
		LoadModel();
	m_IsReady=true;
}
void _CSubCircuit::FinalizeLeaves(MVERTEX_2_SIGN& MVertex2Sgn)
{
	if(m_UseNewAnalyser)
		NewFinalizeLeaves(MVertex2Sgn);
	else
		for(MODEL_V_IT it=m_NewCreatedVertices.begin();it!=m_NewCreatedVertices.end();it++)
			TermLeave2Param(*it,MVertex2Sgn);
}

long long _CSubCircuit::GetBoundaryParamIndex(_CModelVertex* pLeafVertex) 
{
	ASSERTPDD(pLeafVertex->IsLeaf());
	_CMultiDeletions* pDel=MVertex2Dels(pLeafVertex);
	return m_BoundaryParameters.GetIndexOf(pDel,false);
}

long long _CSubCircuit::GetBoundaryParamIndexIfUnique(_CModelVertex* pLeafVertex)
{
	ASSERTPDD(pLeafVertex->IsLeaf());
	if(m_TerminalsCounter[pLeafVertex]!=1)
		return -1;
	_CMultiDeletions* pDel=MVertex2Dels(pLeafVertex);
	return m_BoundaryParameters.GetIndexOf(pDel,false);
}
bool _CSubCircuit::IsDeletionAdmissible(const _CMultiDeletions* pDeletions)//Wa¿ne
{
	if(!m_DeletionAdmissibleDef)
		return true;
	size_t NoOfDels=pDeletions->size();
	size_t NoOfDelsRequired=m_BoundaryNodes.size();
	return (IsInRange(m_DeletionAdmissible,NoOfDelsRequired-NoOfDels));
}

void _CSubCircuit::NewFinalizeLeaves(MVERTEX_2_SIGN& MVertex2Sgn)
{
	//const _CCircuitAnalyzer::BOUNDARIES_MAP& BoundariesMap=m_pNewAnalyxer->GetBoundariesMap();
	size_t NoOfDongles=m_pNewAnalyxer->GetNoOfSubciruitDongles();
	for(size_t i=0;i<NoOfDongles;i++)
	{
		_CModelVertex* pNewModelVertex=new _CModelVertex();
		try
		{
			pNewModelVertex->m_OwnerCircuit=this;
			_CMultiDeletions* pBopundartParam=new _CMultiDeletions(this);
			m_pNewAnalyxer->PrepareDongle(i,*pBopundartParam,*pNewModelVertex,m_TerminalsCounter[pNewModelVertex]);
			try
			{
				long long Id=m_ModelVerticesCache.ForceIntoCache(pNewModelVertex);
				pBopundartParam=PushIntoCache(pBopundartParam);
				m_Dels2VertexMap.insert(DELS_2_MVERTEX::value_type(pBopundartParam,pNewModelVertex));
				m_Vertex2Dels.insert(MVERTEX_2_DELS::value_type(pNewModelVertex,pBopundartParam));
				//MVertex2Sgn.insert(MVERTEX_2_SIGN::value_type(pNewModelVertex,Sgn));
			}
			catch(...)
			{
				delete pBopundartParam;
				throw;
			}
		}
		catch(...)
		{
			delete pNewModelVertex;
			throw;
		}
	}

}

void _CSubCircuit::TermLeave2Param(_CModelVertex* pVertex,MVERTEX_2_SIGN& MVertex2Sgn)
{
	_CGraphTable* pTable=pVertex->GetGraphTable();
	//_CDeletions* pBoundaryParam = NULL, *pNewBoundaryParam = new _CDeletions(this);
	_CMultiDeletions* pBoundaryParam = NULL, *pNewBoundaryParam = new _CMultiDeletions(this);
	short Sign = 0;
	pTable->Table2Deletions(*pNewBoundaryParam,NoOfBoundaryNodes(),Sign);//dopisaæ
	pBoundaryParam=PushIntoCache(pNewBoundaryParam);
	m_Dels2VertexMap.insert(DELS_2_MVERTEX::value_type(pBoundaryParam,pVertex));
	m_Vertex2Dels.insert(MVERTEX_2_DELS::value_type(pVertex,pBoundaryParam));
	MVertex2Sgn.insert(MVERTEX_2_SIGN::value_type(pVertex,Sign));
}


void _CSubCircuit::LoadModel() 
{
	_CModelHeader Header(false);
	_binary_filer Filer(_binary_filer::OPEN_MODE::o_read,GetSubcircuitModelPath(),true);
	Filer>>Header;
	if(Header.WrongHeader())
		RISEPDD(eFilerProblem,"Wrong Header");
	m_InterfaceOffset=Header.InterfaceOff;
	m_NumericOffset=Header.NumericOff;
	_Load(Filer);
}

void _CSubCircuit::StoreModel()
{
	_CModelHeader Header(true);
	_binary_filer Filer(_binary_filer::OPEN_MODE::o_write,GetSubcircuitModelPath(),true);
 	Filer.set(sizeof(Header));
	_Store(Filer);
	Header.InterfaceOff=m_InterfaceOffset;//Tu dopisaæ
	Header.NumericOff=m_NumericOffset;
	Filer.set(0);
	Filer<<Header;
}

void _CSubCircuit::ForceGlobalRefNode(const string& RefName)
{
	if(TranslateOrAddNode(RefName)==0)
		m_IsGlobal0Node=true;
}

int _CSubCircuit::ConvertBoundaryNode(const string& sNode)
{
	BOUNDARY_NODES_IT it=m_BoundaryNodesMap.find(sNode);
	if(it==m_BoundaryNodesMap.end())
		return -1;
	else
		return it->second;
}

void _CSubCircuit::Store(_binary_filer& Filer)
{
	_CCircuit::Store(Filer);
	Filer<<m_BoundaryNodes.size();
	for(_CStrNodes::iterator it=m_BoundaryNodes.begin();it!=m_BoundaryNodes.end();it++)
	{
		BOUNDARY_NODES_IT mit=m_BoundaryNodesMap.find(*it);
		Filer<<mit->first;
		Filer<<mit->second;
	}
	m_InterfaceOffset=Filer.pos();
	Filer<<m_BoundaryParameters;
	Filer<<m_Vertex2Dels.size();
	for(MVERTEX_2_DELS::iterator m2d_it=m_Vertex2Dels.begin();m2d_it!=m_Vertex2Dels.end();m2d_it++)
	{
		//dopisaæ
		_CModelVertex* pVertex=m2d_it->first;
		//MVERTEX_2_SIGN::iterator m2s_it=m_Vertex2Sign.find(pVertex);
		//ASSERTPDD(m2s_it!=m_Vertex2Sign.end());
		long long llVertex=GetModelVertexIndex(m2d_it->first);
		long long llBoundParam=GetBoundaryParamIndex(m2d_it->second);
		Filer<<llVertex<<llBoundParam/*<<m2s_it->second*/;
	}
	Filer<<m_DeletionAdmissibleDef;
	Filer<<m_DeletionAdmissible.first;
	Filer<<m_DeletionAdmissible.second;
	//if(m_DeletionAdmissibleDef)
	//{
	//	Filer<<m_DeletionAdmissible.size();
	//	for(SIZE_T_SET::const_iterator it=m_DeletionAdmissible.begin();
	//		it!=m_DeletionAdmissible.end();it++)
	//	{
	//		Filer<<(*it);
	//	}
	//}
	m_NumericOffset=Filer.pos();
	m_NumResultsCache.Store(Filer);
	//_CNumericHeader NH;
	//Filer<<NH;
	//Filer>>m_BoundaryCofactors;
	//for(unsigned i=0;i<m_BoundaryCofactors.size();i++)
	//{
	//	_CCofactor* pCof=m_BoundaryCofactors[i];
	//	COF_2_MVERTEX::iterator it=m_Cof2VertexMap.find(pCof);
	//	long long MVid=GetModelVertexIndex(it->second);
	//	Filer<<MVid;
	//}
	Filer<<m_UseNewAnalyser;
	if(m_UseNewAnalyser)
		Filer.StoreDynamicClass(m_pNewDescndantsState);
}

void _CSubCircuit::Load(_binary_filer& Filer)
{
	_CCircuit::Load(Filer);
	m_BoundaryNodes.clear();
	m_BoundaryNodesMap.clear();
	m_Dels2VertexMap.clear();
	m_Vertex2Dels.clear();
	//m_Vertex2Sign.clear();
	//m_LocalBoundarySimpleVertices.clear();
	_CStrNodes::size_type Size;
	Filer>>Size;
	for(_CStrNodes::size_type i=0;i<Size;i++)
	{
		string strNode;
		int iNode;
		Filer>>strNode;
		Filer>>iNode;
		m_BoundaryNodes.push_back(strNode);
		m_BoundaryNodesMap.insert(BOUNDARY_NODES_TYPE(strNode,iNode));
	}
	if(m_InterfaceOffset!=Filer.pos())
		RISEPDD(eFilerProblem,"Improper interface offset");
	Filer>>m_BoundaryParameters;
	MVERTEX_2_DELS::size_type Size1;
	Filer>>Size1;
	for(MVERTEX_2_DELS::size_type i=0;i<Size1;i++)
	{
		//dpisaæ
		long long llVertex,llBoundParam;
		//short Sgn;
		Filer>>llVertex>>llBoundParam/*>>Sgn*/;
		_CModelVertex* pVertex=GetModelVertexFromIndex(llVertex);
		//_CDeletions* pDel = GetBoundaryParamFromIndex(llBoundParam);
		_CMultiDeletions* pDel = GetBoundaryParamFromIndex(llBoundParam);
		m_Vertex2Dels.insert(MVERTEX_2_DELS::value_type(pVertex, pDel));
		//m_Vertex2Sign.insert(MVERTEX_2_SIGN::value_type(pVertex,Sgn));
		m_Dels2VertexMap.insert(DELS_2_MVERTEX::value_type(pDel,pVertex));
		//m_LocalBoundarySimpleVertices.insert(MVERTEX_SVERTEX_PAIR(pVertex,_CSimpleVertexContainer(llBoundParam)));
		//m_LocalTerminalSimpleVertex.push_back(_CSimpleVertexContainer(llBoundParam));
	}
	//m_DeletionAdmissible.clear();
	Filer>>m_DeletionAdmissibleDef;
	Filer>>m_DeletionAdmissible.first;
	Filer>>m_DeletionAdmissible.second;
	//if(m_DeletionAdmissibleDef)
	//{
	//	size_t Size;
	//	Filer>>Size;
	//	for(size_t i=0;i<Size;i++)
	//	{
	//		size_t Value;
	//		Filer>>Value;
	//		m_DeletionAdmissible.insert(Value);
	//	}
	//}
	Filer.set(m_NumericOffset);
	m_NumResultsCache.Load(Filer,m_CirCompRedefEntry);
	//_CNumericHeader NH;
	//Filer>>NH;
	//for(size_t i=0;i<NH.NoOfResults;i++)
	//{
	//}
	//Filer<<m_BoundaryCofactors;
	//for(unsigned i=0;i<m_BoundaryCofactors.size();i++)
	//{
	//	long long MVid;
	//	Filer>>MVid;
	//	_CCofactor* pCof=m_BoundaryCofactors[i];
	//	_CModelVertex* pVertex=GetModelVertexFromIndex(MVid);
	//	m_Cof2VertexMap.insert(COF_2_MVERTEX::value_type(pCof,pVertex));
	//	m_Vertex2Cof.insert(MVERTEX_2_COF::value_type(pVertex,pCof));
	//}
	Filer>>m_UseNewAnalyser;
	if(m_UseNewAnalyser)
	{
		if(m_pNewDescndantsState!=NULL)
			delete m_pNewDescndantsState;
		m_pNewDescndantsState=dynamic_cast<_CBounadryStatesVector*>(Filer.RetriveDynamicClass());
	}
	m_IsReady=true;
}

_CSubCircuit* _CSubCircuit::PrepareSubcircuitFromName(const string& Name, const path_str& ModelFile)
{
	return m_Owner->PrepareSubcircuitFromName(Name,ModelFile);
}

path_str _CSubCircuit::PrepareShortFileName()
{
	path_str Res=_T("");
	Res+=_PATH_STR(m_Name);
	if(m_IsGlobal0Node)
		Res+=_T("_ggnd");
	for each(string OutN in m_BoundaryNodes)
	{
		Res+=_TCHAR('_');
		Res+=_PATH_STR(OutN);
	}
	if(m_DeletionAdmissibleDef)
	{
		Res+=_T("_Lim_[");
		Res+=_PATH_STR(ulong2str(m_DeletionAdmissible.first));
		Res+=_T(",");
		Res+=_PATH_STR(ulong2str(m_DeletionAdmissible.second));
		Res+=_T("]");
		//for(SIZE_T_SET::const_iterator it=m_DeletionAdmissible.begin();
		//	it!=m_DeletionAdmissible.end();it++)
		//{
		//	Res+=_TCHAR('_');
		//	Res+=_PATH_STR(ulong2str(*it));
		//}
	}
	Res+=_T(".pddm");
	return Res;
}

path_str _CSubCircuit::PrepareFileName()
{
	return GetModelsPath()+_T("\\")+PrepareShortFileName();
	//path_str Res=GetModelsPath()+_T("\\");
	//Res+=_PATH_STR(m_Name);
	//if(m_IsGlobal0Node)
	//	Res+=_T("_ggnd");
	//for each(string OutN in m_BoundaryNodes)
	//{
	//	Res+=_TCHAR('_');
	//	Res+=_PATH_STR(OutN);
	//}
	//Res+=_T(".pddm");
	//return Res;
}
const path_str& _CSubCircuit::GetSubcircuitModelPath()
{
	if(m_ModelPath.empty())
		m_ModelPath=PrepareFileName();
	return m_ModelPath;
}

bool _CSubCircuit::IsModelInFile()
{
	_search_filer sf(GetSubcircuitModelPath());
	_search_filer::size_type Size=sf.size();
	if(Size>1)
	{
		string msg=string("The file model \"")+_PLAIN_STR(m_ModelPath)+"\" exists more than once";
		RISEPDD(eMoreThanOneModelFile,msg.c_str());
	}
	return Size==1;
}

unsigned _CSubCircuit::NoOfBoundaryNodes()
{
	return m_BoundaryNodes.size()+(m_IsGlobal0Node?1:0);
}

_CMultiDeletions* _CSubCircuit::PushIntoCache(_CMultiDeletions* pObject)
{
	_CMultiDeletions* pNewObject = NULL;
	try
	{
		pNewObject=m_BoundaryParameters.PushInCache(pObject,true);
		pNewObject->m_IsRegistered=true;
		return pNewObject;
	}
	catch(exception)
	{
		if(pObject!=NULL)
			pObject->DestroyIfUnregistered();
		throw;
	}
}
bool _CSubCircuit::GetBoundaryParamsEntry(path_str& Path) 
{
	if(!m_IsReady)
	{
		PrepareSubModelPDD();
		if(!m_IsReady)
			return false;
	}
	Path=m_ModelPath;
	return true;
}

void _CSubCircuit::GetOuterVertex(_CSExpandedVertices*& pVertices, _CModelVertex* pMVertex, _CSExpandedDescendends& TerminalVertices)
{
	size_t DelIndex=GetOuterVertexIndex(pMVertex);
	if(DelIndex>=TerminalVertices.size())
		RISEPDD(out_of_range,"TerminalVerticeses are too short in _CSubCircuit::GetOuterVertex");
	pVertices=TerminalVertices.at(DelIndex);
}

static char* ErrorInfo="Function _CSubCircuit::GetOuterVertex only for leaves";
size_t _CSubCircuit::GetOuterVertexIndex(_CModelVertex* pMVertex)
{
	if(!pMVertex->IsLeaf())
		RISEPDD(eIllegalOperation,ErrorInfo);
	MVERTEX_2_DELS::iterator Del_it=m_Vertex2Dels.find(pMVertex);
	if(Del_it==m_Vertex2Dels.end())
		RISEPDD(eIllegalOperation,ErrorInfo);
	return (size_t)m_BoundaryParameters.GetIndexOf(Del_it->second,false);
}

//short _CSubCircuit::GetOuterVertexSign(_CModelVertex* pMVertex)
//{
//	if(!pMVertex->IsLeaf())
//		RISEPDD(eIllegalOperation,ErrorInfo);
//	MVERTEX_2_SIGN::iterator Sgn_it=m_Vertex2Sign.find(pMVertex);
//	if(Sgn_it==m_Vertex2Sign.end())
//		RISEPDD(eIllegalOperation,ErrorInfo);
//	return  Sgn_it->second;
//}

void _CSubCircuit::WriteHeader(iostream& stream)
{
	stream<<"The subcircuit: "<<m_Name<<"(";
	if(m_IsGlobal0Node)
	{
		stream<<ConvertNode(0);
		if(m_BoundaryNodes.size()>0)
			stream<<", ";
	}
	for(unsigned i=0;i<m_BoundaryNodes.size();i++)
	{
		if(i>0)
			stream<<", ";
		stream<<m_BoundaryNodes[i];
	}
	stream<<")\n";
}

//_CDeletions* _CSubCircuit::MVertex2Dels(_CModelVertex* pVertex)
_CMultiDeletions* _CSubCircuit::MVertex2Dels(_CModelVertex* pVertex)
{
	MVERTEX_2_DELS::iterator it=m_Vertex2Dels.find(pVertex);
	if(it!=m_Vertex2Dels.end())
		return it->second;
	else
		RISEPDD(eUnexpectedCondition, "Tu ciê mam");
//		return NULL;
}

bool _CSubCircuit::IsSuppresedLeaf(_CModelVertex* pLeaf)
{
	ASSERTPDD(pLeaf->IsLeaf());
	MVERTEX_2_DELS::iterator it=m_Vertex2Dels.find(pLeaf);
	return it==m_Vertex2Dels.end();
}

size_t _CSubCircuit::FirstInternalNode() 
{
	return ConvertBoundaryNode(m_BoundaryNodes.back())+1;
}

void _CSubCircuit::DevelopeRealTree(const string& SocketName,_CSExpandedVertices*& pTreeVertices, _CSExpandedDescendends& ExternalVertices,
									short& TopSgn,const _CDescSigns& BottomSgns, bool Approximated, const NumericType& Accuracy)
{
	_CCofactor* pCof=m_Cofactors[0];
	_CModelVertex* pMVertex=pCof->GetMainVertex();//poprawiæ
	if(pMVertex==NULL)
		return;
	pMVertex->GetSimpleVertex(pTreeVertices,0,SocketName,ExternalVertices,TopSgn,BottomSgns,Approximated,Accuracy);
}

//void _CSubCircuit::DevelopeRealTree( _CSExpFlatVertices*& pOutResVertices, 
//	                                   _CFlatVerticesResContainer*& pCacheContainer,
//									   const _CSparsePolynomial& TopNumValue,
//									   const _CSPowerLimiter& Limiter,
//									   const _CSubModelNumericPattern& Scaler,
//									   _CMultLevelSExpFlatVertices& TerminalVertices, 
//									   //short& TopSgn, 
//									   //const _CDescSigns& BottomSgns,
//									   _CPathTraitor& MainTraitor,
//									   _CModelVerticesPath& VerticesPath)
//									   //_CModelVerticesTraitor& VerticesTraitor)
//{
//	_CCofactor* pCof=m_Cofactors[0];
//	_CModelVertex* pMVertex=pCof->GetMainVertex();//poprawiæ
//	if(pMVertex==NULL)
//		return;
//	pMVertex->GetSimpleVertexApprox(pOutResVertices,pCacheContainer,TopNumValue,Limiter,Scaler,
//		TerminalVertices,/*TopSgn,BottomSgns,*/MainTraitor,VerticesPath);
//}

int _CSubCircuit::GetHollowOutput()
{
	//return -1;
	if(!m_IsHollowDetermined)
	{
		m_IsHollowDetermined=true;
		_CCofactor* pCof=m_Cofactors[0];
		_CModelVertex* pMVertex=pCof->GetMainVertex();//poprawiæ
		if(pMVertex==NULL)
			m_HollowOutput=-1;
		else
		{
			_CModelVertex* pHollowLeaf=pMVertex->GetNextHollowVertex();
			if(pHollowLeaf==NULL || pHollowLeaf->IsTerminal())
				m_HollowOutput=-1;
			else
				m_HollowOutput=(int)GetBoundaryParamIndexIfUnique(pHollowLeaf);
		}
	}
	return m_HollowOutput;
}

//void _CSubCircuit::DetermineNumericPattern(_CSubCircuitSocket* pSocket,_CModelNumericPattern& NumericPattern)
void _CSubCircuit::DetermineNumericPattern(_CSubCircuitSocket* pSocket,_CSubModelNumericPattern& NumericPattern)
{
}

void _CSubCircuit::WriteOutParamsToTextStream(iostream& stream)
{
	for(size_t i=0;i<m_BoundaryParameters.size();i++)
	{
		//_CDeletions* pDels = m_BoundaryParameters[i];
		_CMultiDeletions* pDels = m_BoundaryParameters[i];
		if (pDels == NULL)
			continue;
		stream<<"Ext.Par. No:"<<i<<" represented by: ";
		pDels->WriteToTextStream(stream);
		stream<<endl;
	}
	//_CSubCircuit& SubCr=*dynamic_cast<_CSubCircuit*>(m_OwnerCircuit);
	//_CDeletions* pDels=SubCr.MVertex2Dels(this);
	//long long OutParam=SubCr.GetBoundaryParamIndex(this);
	//short Sgn=SubCr.GetOuterVertexSign(this);
	//stream<<"External Parameter No:"<<OutParam<<" represented by: ";
	//if(pDels!=NULL)
	//	pDels->WriteToTextStream(stream);//????
}

void _CSubCircuit::FindDuplicateLeaf(_CModelVertex* pOriginalVertex, _CModelVertex*& pNonDuplicateVertex,
		short& MovedSign,MVERTEX_2_SIGN* pMVertex2Sgn)
{
	if(pOriginalVertex==NULL || !pOriginalVertex->IsLeaf())
		RISEPDD(eWrongCondition,"In FindDuplicateLeaf pOriginalVertex should be a leaf");
	MVERTEX_2_DELS::iterator m2v_it=m_Vertex2Dels.find(pOriginalVertex);
	if(m2v_it==m_Vertex2Dels.end())
		RISEPDD(eWrongCondition,"In FindDuplicateLeaf pOriginalVertex is invalid");
	typedef pair<DELS_2_MVERTEX::iterator,DELS_2_MVERTEX::iterator> RANGE;
	RANGE Range=m_Dels2VertexMap.equal_range(m2v_it->second);
	ASSERTPDD(Range.first!=m_Dels2VertexMap.end());
	pNonDuplicateVertex=Range.first->second;
	//MVERTEX_2_SIGN::iterator mv2sgn_it=m_Vertex2Sign.find(pOriginalVertex);
	MVERTEX_2_SIGN::iterator mv2sgn_it=pMVertex2Sgn->find(pOriginalVertex);
	//ASSERTPDD(mv2sgn_it!=m_Vertex2Sign.end());
	ASSERTPDD(mv2sgn_it!=pMVertex2Sgn->end());
	MovedSign=mv2sgn_it->second;
}

void _CSubCircuit::GetSimpleVertexApprox2(const _CFlatVertex*& pResultVertex,
										  short& ResultSgn,
										  //const _CSparsePolynomial& TopNumValue,
										  size_t Power,
										  NumericType& AbsolutInaccuracyAllowed,  
										  //const _CSubModelNumericPattern& Scaler,
										  _CPathTraitor& MainTraitor,
										  _CModelVerticesPath& VerticesPath,
										  _CCompRedefEntry& RedefEntry)
{
	_CCofactor* pCof=m_Cofactors[0];
	_CModelVertex* pMVertex=pCof->GetMainVertex();//poprawiæ
	if(pMVertex==NULL)
		RISEPDD(eWrongCondition,"Wrong subcircuit data structire");
	ResultSgn*=pCof->GetGlobalMultiplier();
	pMVertex->GetSimpleVertexApprox2(pResultVertex,ResultSgn,/*TopNumValue,*/Power,
		AbsolutInaccuracyAllowed,MainTraitor,VerticesPath,RedefEntry);
}

bool/*NewAdded*/ _CSubCircuit::PerformNewSimplyfyApprox(_CPreFlatVertexContainer*& pResultVertex,
								  short& ResultSgn,
								  size_t Power,
								  _CNewSimplifierData& Data, _CCompRedefEntry& RedefEntry)
{
	_CCofactor* pCof=m_Cofactors[0];
	_CModelVertex* pMVertex=pCof->GetMainVertex();//poprawiæ
	if(pMVertex==NULL)
		RISEPDD(eWrongCondition,"Wrong subcircuit data structire");
	ResultSgn*=pCof->GetGlobalMultiplier();
	return pMVertex->PerformNewSimplyfyApprox(pResultVertex, ResultSgn, Power, Data, RedefEntry);
}

bool _CSubCircuit::IsModelSymetric() const
{
	for(MVERTEX_2_DELS::const_iterator it=m_Vertex2Dels.begin(),_e=m_Vertex2Dels.end();it!=_e;it++)
		if(!it->second->IsSymetric())
			return false;
	return true;
}

void _CSubCircuit::PostProcessComponent(_CComponent* pJustProcessedComponent) 
{
	if(m_DeletionAdmissibleDef)
		m_MaxPossibleDeletions-=pJustProcessedComponent->MaxDescRank();
}

bool _CSubCircuit::IsPromising(_CGraphState* pState)
{
	if(!m_DeletionAdmissibleDef)
		return true;
	unsigned CurrDels=pState->NoOfDeletions();
	//if(CurrDels>max(m_DeletionAdmissible.first,m_DeletionAdmissible.second))
	//	return false;
	if(CurrDels+min(m_DeletionAdmissible.first,m_DeletionAdmissible.second)>HighestNoOfNode())
	{
		pState->ForceSizeRejected();
		return false;
	}
	if(CurrDels+m_MaxPossibleDeletions+max(m_DeletionAdmissible.first,m_DeletionAdmissible.second)<HighestNoOfNode())
	{
		pState->ForceSizeRejected();
		return false;
	}
	return true;
}

void _CSubCircuit::PreparePromissing()
{
	if(!m_DeletionAdmissibleDef) return;
	m_MaxPossibleDeletions=0;
	for(_CComponents::iterator it=m_LocalComponentsList.begin(),_e=m_LocalComponentsList.end();it!=_e;it++)
	{
		_CComponent* pComp=*it;
		m_MaxPossibleDeletions+=pComp->MaxDescRank();
	}
}


void _CSubCircuit::SetRankLimits(const SIZE_T_RANGE& Range)
{
	m_DeletionAdmissibleDef=true;
	m_DeletionAdmissible=Range;
	m_MaxPossibleDeletions=0;
}