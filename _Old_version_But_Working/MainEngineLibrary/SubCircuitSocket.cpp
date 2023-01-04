#include "StdAfx.h"
#include "SubCircuitSocket.h"
#include "Circuit.h"
#include "MainCircuit.h"
#include "CSimpleVertexContainer.h"
#include "norm_float.h"


_CSubCircuitSocket::_CSubCircuitSocket(_CCircuit* pOwner,const string& InstanceName,_CSubCircuit *pModel,
	const _CIntNodes& ConnectedNodes, long hInternalCofactor):_CTreeForkComponent(pOwner),
									   m_InstanceName(InstanceName),m_pModel(pModel),
									   m_ConnectedNodes(ConnectedNodes),m_RealParameters(pOwner),m_MaxRank(0),
									   m_hInternalCofactor(hInternalCofactor)/*,
									   m_VerticesMaps(pOwner)*/
{
	//m_IAmNumericCopyOf=this;
	if(m_pModel!=NULL)
	{
		PrepareInterface2Model();
	}

}

_CSubCircuitSocket::~_CSubCircuitSocket(void)
{
	//while(!m_VerticesMapBuffor.empty())
	//{
	//	delete m_VerticesMapBuffor.back();
	//	m_VerticesMapBuffor.pop_back();
	//}
}

void _CSubCircuitSocket::PlugInComponent(_CIntegrityTable& IntegrityTable)
{
	COMPONENT_PINS Pins;
	for each (int Node in m_ConnectedNodes)
		if(Node>=0)
			Pins.push_back(Node);
	IntegrityTable.PlugComponents(Pins);
}

size_t _CSubCircuitSocket::GetHash(size_t Core) const
{
	auto lCore=Core;
	lCore=Hash_value(lCore,m_InstanceName);
	lCore*=HASH_FACTOR;
	lCore=Hash_value(lCore,m_pModel->m_Name.c_str());
	for each(int Node in m_ConnectedNodes)
	{
		lCore*=HASH_FACTOR;
		lCore^=Node;
	}
	return lCore;
}
//void _CSubCircuitSocket::PrepareLHStates(_CGraphState& HState, _CGraphState& LState)
//{
//}

//void _CSubCircuitSocket::Connect()
//{
//	m_pModel->CreateModelPDD();
//}

void _CSubCircuitSocket::CurrentPins(_CGraphTable& Tables,TWO_GRAPH_PINS& Pins) const
{
	int r,c;
	//for(_CIntNodes::const_iterator it=m_ConnectedNodes.begin();it!=m_ConnectedNodes.end();it++)
	//{
	//	r=Tables.RealRow(*it);
	//	c=Tables.RealColumn(*it);
	//	Pins.push_back(TWO_GRAPH_PIN(r,c));
	//}
	for each(int N in m_ConnectedNodes)
		if(N>=0)
		{
			r=Tables.RealRow(N);
			c=Tables.RealColumn(N);
			Pins.push_back(TWO_GRAPH_PIN(abs(r),abs(c)));
		}
}

void _CSubCircuitSocket::ProcessDescendant(unsigned Desc, _CGraphState* pState)
{
	int RDesc=m_AllParamConnections[Desc];
	if(RDesc<0)
		return;
	_CMultiDeletions* pMDels=m_RealParameters[RDesc];
	if (pMDels->size()>1)
		RISEPDD(eWrongCondition, "Funcion obsolate. Not for multilevel cofactors");
	if(!m_pModel->IsDeletionAdmissible(pMDels))
	{
		string Err="Abridged model cannot support cofactor ";
		Err+=(string)(*pMDels);
		RISEPDD(eIllegalOperation,Err.c_str());
	}
	//zdecydowanie poprawiæ
	_CDeletions* pDels = &(*pMDels)[0];
	pDels->ResetSgn(pState->GetGraphTable());
	for each (_CDeletion* pDel in *pDels)
		if(pDel->AddDeletion(pState)==TERM_SELFLOOP)
			break;
}

void _CSubCircuitSocket::CheckDisconnection(_CGraphState* pState) const
{
	for each(int N in m_ConnectedNodes)
	{
		if(N<0) continue;
		if(pState->IsSeparatedNode(N))
			break;
	}
}
unsigned _CSubCircuitSocket::MaxDescRank()  
{
	if(m_MaxRank==0)
		for(size_t i=0;i<NoOfDescendants();i++)
		{
			int j=m_AllParamConnections[i];
			if(i<0) continue;
			//_CDeletions* pDels = m_RealParameters[j];
			_CMultiDeletions* pDels = m_RealParameters[j];
			m_MaxRank = max(m_MaxRank, pDels->GetRank());
		}
	return m_MaxRank;
}

void _CSubCircuitSocket::Load(_binary_filer& Filer)
{
	Filer>>m_InstanceName;
	string ModelName;
	Filer>>ModelName;
	//m_pBaseCircuit-> dopisaæ odtworzenie modelu
	path_str ModelFile;
	Filer>>ModelFile;
	m_pModel=m_pBaseCircuit->PrepareSubcircuitFromName(ModelName,ModelFile);
	if(m_pModel==NULL)
	{
		string Err=ModelName+"cannot be found";
		RISEPDD(eIllegalModelName,Err.c_str());
	}
	unsigned Size;
	Filer>>Size;
	if(Size!=m_pModel->NoOfBoundaryNodes())
	{
		string Err="The number of nodes in socket does not fit to the model "+ModelName;
		RISEPDD(eFilerProblem,Err.c_str());
	}
	m_ConnectedNodes.resize(Size);
	for(unsigned i=0;i<Size;i++)
		Filer>>m_ConnectedNodes[i];
	Filer>>Size;
	if(Size!=m_pModel->NoOfBoundaryParams())//poprawiæ
	{
		string Err="The number of boundary parameters in socket is different than in model  "+ModelName;
		RISEPDD(eFilerProblem,Err.c_str());
	}
	m_AllParamConnections.resize(Size);
	for(unsigned i=0;i<Size;i++)
		Filer>>m_AllParamConnections[i];
	m_pModel->RequestModelAnalysis();
}

void _CSubCircuitSocket::Store(_binary_filer& Filer)
{
	Filer<<m_InstanceName;
	Filer<<(m_pModel->GetName());
	Filer<<m_pModel->PrepareShortFileName();
	size_t Size=m_ConnectedNodes.size();
	Filer<<Size;
	for each(int Node in m_ConnectedNodes)
		Filer<<Node;
	Size=m_AllParamConnections.size();
	Filer<<Size;
	for each(int DelIndx in m_AllParamConnections)
		Filer<<DelIndx;

}

bool _CSubCircuitSocket::IsEqualIfSameType(const _CComponent& RightComp)
{
	const _CSubCircuitSocket& RC=*dynamic_cast<const _CSubCircuitSocket*>(&RightComp);
	if(m_InstanceName!=RC.m_InstanceName)
		return false;
	if(m_pModel->m_Name!=RC.m_pModel->m_Name)
		return false;
	if(m_ConnectedNodes.size()!=RC.m_ConnectedNodes.size())
		return false;
	auto Size=m_ConnectedNodes.size();
	for(auto i=0;i<Size;i++)
		if(m_ConnectedNodes[i]!=RC.m_ConnectedNodes[i])
			return false;
	return true;
}


bool _CSubCircuitSocket::PrepareInterface2Model()
{
	path_str FilePath;
	if(!GetModelInterfaceSpace(FilePath))
		return false;
	_binary_filer Filer(_binary_filer::OPEN_MODE::o_read,FilePath,true);
	_CModelHeader Header(false);
	Filer>>Header;
	if(Header.WrongHeader())
		return false;
	if(m_pModel->m_UseNewAnalyser)
		NewModelTranslation();
	else
	{
		Filer.set(Header.InterfaceOff);
		_CBounderyParametersCache Parameters(m_pBaseCircuit);
		Filer>>Parameters;
		if(m_ConnectedNodes.ContainDongles())
			SimplyModelTranslation(Parameters);
		else
			AdvancedModelTranslation(Parameters);
	}
	return true;
}


void _CSubCircuitSocket::SimplyModelTranslation(_CBounderyParametersCache& BPC)
{
	PARAM_CONNECTIONS::size_type mId=(PARAM_CONNECTIONS::size_type)BPC.MaxId();
	m_AllParamConnections.resize(mId);
	for(PARAM_CONNECTIONS::size_type i=0;i<mId;i++)
	{
		_CMultiDeletions* pMultDels=BPC[i];
		if (pMultDels == NULL)
			continue;
		BPC.RemoveByIndex(i,false);
		for (auto& DelsPair : *pMultDels)
		{
			for (auto& pDel : DelsPair.second)
			{
				pDel->m_p = m_ConnectedNodes.get_sgn(pDel->m_p);
				pDel->m_r = m_ConnectedNodes.get_sgn(pDel->m_r);
				pDel->m_k = m_ConnectedNodes.get_sgn(pDel->m_k);
				pDel->m_l = m_ConnectedNodes.get_sgn(pDel->m_l);
			}

		}
		pMultDels->InvalidateHash();
		pMultDels = m_RealParameters.PushInCache(pMultDels, true);
		m_AllParamConnections[i] = (int)m_RealParameters.GetIndexOf(pMultDels, false);
	}
}

//void _CSubCircuitSocket::SimplyModelTranslation(_CBounderyParametersCache& BPC)
//{
//	PARAM_CONNECTIONS::size_type mId = (PARAM_CONNECTIONS::size_type)BPC.MaxId();
//	m_AllParamConnections.resize(mId);
//	for (PARAM_CONNECTIONS::size_type i = 0; i<mId; i++)
//	{
//		_CDeletions* pDels = BPC[i];
//		if (pDels == NULL)
//			continue;
//		BPC.RemoveByIndex(i, false);
//		for each(_CDeletion* pDel in *pDels)
//		{
//			pDel->m_p = m_ConnectedNodes.get_sgn(pDel->m_p);
//			pDel->m_r = m_ConnectedNodes.get_sgn(pDel->m_r);
//			pDel->m_k = m_ConnectedNodes.get_sgn(pDel->m_k);
//			pDel->m_l = m_ConnectedNodes.get_sgn(pDel->m_l);
//		}
//		pDels->InvalidateHash();
//		pDels = m_RealParameters.PushInCache(pDels, true);
//		m_AllParamConnections[i] = (PARAM_CONNECTIONS::size_type)m_RealParameters.GetIndexOf(pDels, false);
//	}
//}

void _CSubCircuitSocket::NewModelTranslation(/*_CBounderyParametersCache& BPC*/)
{
	SIGNED_PINS_COLOCATIONS PinsCollocation;
	COMPONENT_PINS_LIST Dongles;
	m_ConnectedNodes.SeparateDongles(PinsCollocation,Dongles);
	_CBounadryStatesVector* pBoundarVector=m_pModel->m_pNewDescndantsState;
	ASSERTPDD(pBoundarVector!=NULL);
	_CBounadryStatesVector NewVector(*pBoundarVector,PinsCollocation,Dongles);
	//PARAM_CONNECTIONS::size_type mId=(PARAM_CONNECTIONS::size_type)BPC.MaxId();
	m_AllParamConnections.resize(m_pModel->NoOfBoundaryParams(),-2);
	NewVector.ResetEmum();
	_CBoundaryState* pState=NULL;
	_CBounadryStatesVector::TERMINALS_LIST List;
	while(NewVector.GetNext(pState,List))
	{
		//_CDeletions* pDels=new _CDeletions(m_pBaseCircuit);
		_CMultiDeletions* pMultDels = new _CMultiDeletions(m_pBaseCircuit);
		pState->ReverseCofactor(*pMultDels);
		pMultDels = m_RealParameters.PushInCache(pMultDels, true);
		auto Index = (int)m_RealParameters.GetIndexOf(pMultDels, false);
		for(_CBounadryStatesVector::TERMINALS_LIST::iterator it=List.begin(),_e=List.end();it!=_e;it++)
			m_AllParamConnections[*it]=Index;
	}
	//?
}

void _CSubCircuitSocket::GetDescendantDeletions(unsigned Descendant, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	PrepareSetOfDeletions();
	ASSERTPDD(m_AllParamConnections.size()>Descendant);
	MULTI_BASIC_SET_OF_DELETIONS::iterator it=m_BasicSetOfDeletions.find(Descendant);
	ASSERTPDD(it!=m_BasicSetOfDeletions.end());
	pDeletions=&it->second;
}

//void _CSubCircuitSocket::GetDescendantDeletions(unsigned Descendant, const _CBasicSetOfDeletions*& pDeletions)
//{
//	PrepareSetOfDeletions();
//	ASSERTPDD(m_AllParamConnections.size()>Descendant);
//	//Descendant=m_AllParamConnections[Descendant];
//	BASIC_SET_OF_DELETIONS::iterator it = m_BasicSetOfDeletions.find(Descendant);
//	ASSERTPDD(it != m_BasicSetOfDeletions.end());
//	pDeletions = &it->second;
//	//_CDeletions*& pDels=m_RealParameters.GetAt(Descendant);
//	//m_BasicSetOfDeletions.insert(BASIC_SET_OF_DELETIONS::value_type(Descendant,
//	//	_CBasicSetOfDeletions(*pDels)));
//}

void _CSubCircuitSocket::PrepareSetOfDeletions()
{
	//typedef list<_CBasicSetOfDeletions> _CMultiBasicSetOfDeletions;
	//typedef map<size_t, _CMultiBasicSetOfDeletions> MULTI_BASIC_SET_OF_DELETIONS;
	if (!m_BasicSetOfDeletions.empty())
		return;
	size_t Level = 0;
	m_GreedyDeletions.resize(m_RealParameters.size());
	for (size_t i = 0; i<m_RealParameters.size(); i++)
	{
		_CMultiDeletions*& pMultDels = m_RealParameters.GetAt(i);
		_CMultiBasicSetOfDeletions& iSet = m_BasicSetOfDeletions.insert(MULTI_BASIC_SET_OF_DELETIONS::value_type(i, 
			_CMultiBasicSetOfDeletions())).first->second;
		for (auto& DelPairs : *pMultDels)
		{
			iSet.push_back(_CBasicSetOfDeletions(DelPairs.second));
		}
		m_GreedyDeletions[i] = &iSet;
	}
	//class Comp
	//{
	//public:
	//	bool operator()(const _CMultiBasicSetOfDeletions* pL, const _CMultiBasicSetOfDeletions* pR)
	//	{
	//		return pL->front().size()>pR->front.size();
	//	}
	//};
	partial_sort(m_GreedyDeletions.begin(), m_GreedyDeletions.begin(), m_GreedyDeletions.end(), 
		[](const _CMultiBasicSetOfDeletions* pL, const _CMultiBasicSetOfDeletions* pR)
	{
		return pL->front().size()>pR->front().size();
	});

	//partial_sort(m_GreedyDeletions.begin(), m_GreedyDeletions.begin(), m_GreedyDeletions.end(), Comp());
}

//void _CSubCircuitSocket::PrepareSetOfDeletions()
//{
//	if (!m_BasicSetOfDeletions.empty())
//		return;
//	size_t Level = 0;
//	m_GreedyDeletions.resize(m_RealParameters.size());
//	for (size_t i = 0; i<m_RealParameters.size(); i++)
//	{
//		_CDeletions*& pDels = m_RealParameters.GetAt(i);
//		_CBasicSetOfDeletions& RBD = m_BasicSetOfDeletions.insert(BASIC_SET_OF_DELETIONS::value_type(i,
//			_CBasicSetOfDeletions(*pDels))).first->second;
//		m_GreedyDeletions[i] = &RBD;
//	}
//	class Comp
//	{
//	public:
//		bool operator()(const _CBasicSetOfDeletions* pL, const _CBasicSetOfDeletions* pR)
//		{
//			return pL->size()>pR->size();
//		}
//	};
//	partial_sort(m_GreedyDeletions.begin(), m_GreedyDeletions.begin(), m_GreedyDeletions.end(), Comp());
//}

//void _CSubCircuitSocket::PrepareSetOfDeletions()
//{
//	if(!m_BasicSetOfDeletions.empty())
//		return;
//	size_t Level=0;
//	m_GreedyDeletions.resize(m_AllParamConnections.size());
//	for(PARAM_CONNECTIONS::iterator it=m_AllParamConnections.begin(),_e=m_AllParamConnections.end();it!=_e;it++)
//	{
//		int Descendant=*it;
//		if(Descendant<0)
//			continue;
//		BASIC_SET_OF_DELETIONS::iterator fit=m_BasicSetOfDeletions.find(Descendant);
//		if(fit!=m_BasicSetOfDeletions.end()) continue;
//		_CDeletions*& pDels=m_RealParameters.GetAt(Descendant);
//		_CBasicSetOfDeletions& RBD=m_BasicSetOfDeletions.insert(BASIC_SET_OF_DELETIONS::value_type(Descendant,
//			_CBasicSetOfDeletions(*pDels))).first->second;
//		m_GreedyDeletions[Level++]=&RBD;
//	}
//	class Comp
//	{
//	public:
//		bool operator()(const _CBasicSetOfDeletions* pL,const _CBasicSetOfDeletions* pR)
//		{
//			return pL->size()>pR->size();
//		}
//	};
//	m_GreedyDeletions.resize(Level);
//	partial_sort(m_GreedyDeletions.begin(),m_GreedyDeletions.begin(),m_GreedyDeletions.end(),Comp());
//}

//void _CSubCircuitSocket::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CBasicSetOfDeletions*& pDeletions)
//{
//	PrepareSetOfDeletions();
//	ASSERTPDD(ImportanceLevel<m_GreedyDeletions.size());
//	pDeletions=m_GreedyDeletions[ImportanceLevel];
//}

void _CSubCircuitSocket::GetDescendantGreedyDeletion(unsigned ImportanceLevel, const _CMultiBasicSetOfDeletions*& pDeletions)
{
	PrepareSetOfDeletions();
	ASSERTPDD(ImportanceLevel<m_GreedyDeletions.size());
	pDeletions = m_GreedyDeletions[ImportanceLevel];
}

void _CSubCircuitSocket::RaportConnections(COMPONENT_PINS& Pins) const
{
	size_t Size=m_ConnectedNodes.size();
	Pins.resize(Size,0);
	size_t EffectiveSize=0;
	for(size_t i=0;i<Size;i++)
	{
		int Node=m_ConnectedNodes[i];
		if(Node>=0)
			Pins[EffectiveSize++]=Node;
	}
	Pins.resize(EffectiveSize);
}

bool _CSubCircuitSocket::DeletionsAreForced()  
{
	return m_RealParameters.DeletionsAreForced();
}

bool _CSubCircuitSocket::Translate2RealDescendant(size_t& Index) const 
{
	ASSERTPDD(Index<this->m_AllParamConnections.size());
	int RIndex=m_AllParamConnections[Index];
	if(RIndex<0)
		return false;
	Index=(size_t)RIndex;
	return true;
}

void _CSubCircuitSocket::AdvancedModelTranslation(_CBounderyParametersCache& BPC)
{
	_CIntNodes Dongles;
	m_ConnectedNodes.GetDongles(Dongles);
	PARAM_CONNECTIONS::size_type mId=(PARAM_CONNECTIONS::size_type)BPC.MaxId();
	m_AllParamConnections.resize(mId);
	for(PARAM_CONNECTIONS::size_type i=0;i<mId;i++)
	{
		//_CDeletions* pDels = BPC[i];
		_CMultiDeletions* pMulDels = BPC[i];
		if (pMulDels == NULL)
			continue;
		if (pMulDels->size() > 1)
			RISEPDD(eWrongCondition, "Obsolate function not form multilevel cofactors");
		_CDeletions* pDels = &(*pMulDels)[0];
		if(!pDels->ContainNodes(Dongles))
		{
			m_AllParamConnections[i]=-2;
			continue;
		}
		_CGraphTable TestTable((unsigned)m_ConnectedNodes.size());
		if(!pDels->AddDeletions(&TestTable))
		{
			m_AllParamConnections[i]=-2;
			continue;
		}
		pDels->ResetSgn(&TestTable);
		pDels=new _CDeletions(m_pBaseCircuit);
		TestTable.ReleaseDongles(Dongles,m_ConnectedNodes);
		short Sign=0;
		if(!TestTable.Table2Deletions2(*pMulDels,m_ConnectedNodes,Sign))
		{
			delete pDels;
			m_AllParamConnections[i]=-2;
			continue;
		}//dopisaæ
		pMulDels=m_RealParameters.PushInCache(pMulDels,true);
		m_AllParamConnections[i]=(int)m_RealParameters.GetIndexOf(pMulDels,false); 
	}
}

bool _CSubCircuitSocket::GetModelInterfaceSpace(path_str& FilePath)
{
	if(m_pModel==NULL)
		return false;
	return m_pModel->GetBoundaryParamsEntry(FilePath);
}


//void _CSubCircuitSocket::GetSimplyVertex(_CSExpandedVertices& Vertices,_CSExpandedDescendends& Descendents)
////_CSimpleVertexContainer _CSubCircuitSocket::GetSimplyVertex(_CSimpleVertexVector& Descendents)
//{
//	_CSExpandedDescendends ModelDescendents/*(Descendents.GetModelVertex())*/;
//	for each(int V in m_AllParamConnections)
//	{
//		if(V<0)
//			ModelDescendents.push_back(StaticZeroVertices);
//		else
//			ModelDescendents.push_back(Descendents[V]);
//	}
//	_CSimpleVertexMap* pVerticesMap=new _CSimpleVertexMap;
//	pVerticesMap->TranslateLocalTerminals(ModelDescendents);
//	_CRealGraphDevelopement Container(0,ModelDescendents,m_pModel->GetCofactor(1)->GetMainVertex());
//	_CSExpandedVertices ResVertices=m_pModel->FindResult(&Container);
//	if(!ResVertices.IsEmpty())
//	{
//		Vertices=ResVertices;
//		delete pVerticesMap;
//		return;
//	}
//	m_VerticesMapBuffor.push_back(pVerticesMap);
//	//_CSExpandedVertices& TreeVertices=m_pModel->GetNewSVertices();
//	m_pModel->DevelopeRealTree(Vertices, ModelDescendents);
//	Vertices.SetInputConnectorData(this,pVerticesMap);
//
//	Container.SetResult(Vertices);
//	_CRealGraphDevelopement* pContainer=m_pModel->FindExisting(&Container);
//	if(pContainer==NULL)
//	{
//		_CRealGraphDevelopement* pContainer=new _CRealGraphDevelopement(Container);
//		m_pModel->CacheTheResult(pContainer);
//	}
//	else
//		pContainer->SetResult(Vertices);
//	size_t LocalCofId=m_pModel->PushAndModifyLocalSVertices(Vertices);//poprawiamy
//}

unsigned short _CSubCircuitSocket::sPower() const
{
	return (unsigned short)m_pModel->MaxPower();
}

void _CSubCircuitSocket::WritePos(iostream& stream)
{
	stream<<"(";
	for(int i=0;i<(int)m_ConnectedNodes.size();i++)
	{
		if(i>0)
			stream<<", ";
		int NI=m_ConnectedNodes[i];
		stream<<m_pModel->ConvertNode(i)<<"="<<(NI<0?"[nc]":m_pBaseCircuit->ConvertNode(NI));
	}
	//stream<<"(I+="<<m_pBaseCircuit->ConvertNode(m_p)<<", I-="<<m_pBaseCircuit->ConvertNode(m_r)<<
	//	", O+="<<m_pBaseCircuit->ConvertNode(m_k)<<", O-="<<m_pBaseCircuit->ConvertNode(m_l)<<")";
	stream<<")";
}

//void _CSubCircuitSocket::RedefineParameterValue(const string& ParameterName, long double NewValue)
//{
//	const _CComponent* pComponent=m_pModel->Name2Component(ParameterName);
//	if(pComponent==NULL)
//	{
//		string Msg=ParameterName+" does not exist or is illegal";
//		RISEPDD(eIllegalOperation,Msg.c_str());
//	}
//	RedefineParameterValue(pComponent,NewValue);
//}
//
//void _CSubCircuitSocket::RedefineParameterValue (const _CComponent* pComponent, long double NewValue)
//{
//	if(!pComponent->HasParameterValue())
//	{
//		string Msg=pComponent->GetParameterName()+" has no value to change";
//		RISEPDD(eIllegalOperation,Msg.c_str());
//	}
//	m_RedefParameters.insert(REDEF_VALUE_TYPE(pComponent,NewValue));
//}
//
//const NumericType& _CSubCircuitSocket::GetParameterCurrentValue(const _CComponent* pComponent) const
//{
//	REDEF_CONST_ITERATOR it=m_RedefParameters.find(pComponent);
//	if(it!=m_RedefParameters.end())
//		return it->second;
//	if(pComponent->HasParameterValue())
//		return pComponent->GetParameterValue();
//	else
//	{
//		throw eIllegalOperation("The component has no value");
//	}
//}
//
//const NumericType& _CSubCircuitSocket::GetParameterCurrentValue(const string& ParameterName) const
//{
//	_CComponent* pComponent=m_pModel->Name2Component(ParameterName);
//	if(pComponent==NULL)
//	{
//		string Msg=ParameterName+" does not exist or is illegal";
//		RISEPDD(eIllegalOperation,Msg.c_str());
//	}
//	else
//		return GetParameterCurrentValue(pComponent);;
//}

int _CSubCircuitSocket::HollowOutput() 
{
	//return m_pModel->GetHollowOutput();
	int ModelHollow=m_pModel->GetHollowOutput();
	if(ModelHollow<0)
		return -1;
	size_t _size=m_AllParamConnections.size();
	ASSERTPDD((size_t)ModelHollow<_size);
	//int HollowPin = -1;
	int InternalPin=m_AllParamConnections[ModelHollow];
	for (size_t i = 0; i < _size; i++)
		//if (m_AllParamConnections[i] == ModelHollow)
		//	if (HollowPin >= 0)
		//		return -1;
		//	else
		//		HollowPin = i;
		if(i!=ModelHollow && m_AllParamConnections[i]==InternalPin)
			return -1;
	//return m_AllParamConnections[ModelHollow];
	//return ModelHollow;
	return InternalPin;
	//return HollowPin;
}

_CSubCircuit* _CSubCircuitSocket::GetModelCircuit(/*const string& Context*/) const
{
	//if(&Context==&EmptyString)
		return m_pModel;
	//CONTEXT_MODEL::const_iterator it=m_Models.find(&Context);
	//return (it==m_Models.end())?NULL:it->second;
}



void _CSubCircuitSocket::ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials,
	const _CDescModelNumericPattern& DescendantsNumResuls,
	const _CDescSignsVector& DescendantsSigns,
	_CCompRedefEntry& CompRedefEntry,
	const string* Context)
{
	CompRedefEntry.WeekMergeSubtree(m_pModel->GetComponentRedef());
	const _CCompRedefEntry* pUniqRedefEntry = m_pBaseCircuit->GetUniqCompRedef(&CompRedefEntry);
	const _CSubModelNumericPattern*& pResultPattern = m_LocalResPolynomials[Context][pUniqRedefEntry];
	if (pResultPattern == NULL)
	{
		_CSubCircuit* pModel = GetModelCircuit(/**Context*/);//popraw
		_CSubModelNumericPattern& GlobalResultPattern = pModel->GetExistingNumPoly(Context, pUniqRedefEntry);
		pResultPattern = &GlobalResultPattern;
		if (GlobalResultPattern.empty())
		{
			const _CSubModelNumericPattern* pLocalRes = NULL;
			_CDirectNumContainer* pCacheEntry = NULL;
			_CCofactor* pCof = pModel->m_Cofactors[0];//Context?
			_CModelVertex* pMVertex = pCof->GetMainVertex(Context);
			pMVertex->PrepareDirectNumericValue(pLocalRes, pCacheEntry, Context, CompRedefEntry[this]);//???
			GlobalResultPattern = *pLocalRes;
			pModel->NotifyReadyNew(Context, pUniqRedefEntry);
			m_DirNumCache.HibernateAll();
		}
	}
	for (size_t i = 0; i<pResultPattern->size(); i++)
	{
		int DescIndex = m_AllParamConnections[i];
		if (DescIndex >= 0)
		{
			short Sgn = DescendantsSigns[DescIndex];
			if (Sgn == 0)
				continue;
			else if (Sgn != 1)
			{
				_CSparsePolynomial Mult;
				Mult.put(0, Sgn);
				ResPolynomials += ((*DescendantsNumResuls[DescIndex])*pResultPattern->get(i)*Mult);

			}
			else
				ResPolynomials += ((*DescendantsNumResuls[DescIndex])*pResultPattern->get(i));
		}
	}
}

//void _CSubCircuitSocket::ApplyMe2Result(_CSubModelNumericPattern& ResPolynomials,
//	const _CDescModelNumericPattern& DescendantsNumResuls,
//	const _CDescSignsVector& DescendantsSigns,
//	_CSubCircuitSocket* pOwnerSocket,
//	const string* Context)
//{
//	//_CSubModelNumericPattern& ResultPattern=m_LocalResPolynomials[Context];
//	const _CSubModelNumericPattern*& pResultPattern = m_LocalResPolynomials[Context];
//	if (pResultPattern == NULL)
//	{
//		_CSubCircuit* pModel = GetModelCircuit(/**Context*/);//popraw
//		//const _CSubModelNumericPattern* pExistedPattern=pModel->GetExistingNumPoly(m_RedefParameters);
//		_CSubModelNumericPattern& GlobalResultPattern = pModel->GetExistingNumPoly(Context, m_RedefParameters, m_IAmNumericCopyOf);
//		pResultPattern = &GlobalResultPattern;
//		//if(pExistedPattern!=NULL)
//		//{
//		//	ResultPattern=*pExistedPattern;
//		//}
//		//else
//		if (GlobalResultPattern.empty())
//		{
//			const _CSubModelNumericPattern* pLocalRes = NULL;
//			_CDirectNumContainer* pCacheEntry = NULL;
//			_CCofactor* pCof = pModel->m_Cofactors[0];//Context?
//			_CModelVertex* pMVertex = pCof->GetMainVertex(Context);
//			//pMVertex->PrepareDirectNumericValue(pLocalRes,Context,this);
//			pMVertex->PrepareDirectNumericValue(pLocalRes, pCacheEntry, Context, this);//???
//			GlobalResultPattern = *pLocalRes;
//			//pModel->RegisterNumPoly(m_RedefParameters,&ResultPattern);
//			//pModel->RegisterNumPoly(m_RedefParameters,pResultPattern);
//			//m_LocalResPolynomials[Context]=pResultPattern;
//			//pResultPattern;
//			pModel->NotifyReadyNew(Context, m_RedefParameters);
//			//m_DirNumCache.ReportHitTest();
//			//m_DirNumCache.Clear();
//			m_DirNumCache.HibernateAll();
//		}
//	}
//	//for(size_t i=0;i<ResultPattern.size();i++)
//	for (size_t i = 0; i<pResultPattern->size(); i++)
//	{
//		int DescIndex = m_AllParamConnections[i];
//		if (DescIndex >= 0)
//		{
//			short Sgn = DescendantsSigns[DescIndex];
//			if (Sgn == 0)
//				continue;
//			else if (Sgn != 1)
//			{
//				_CSparsePolynomial Mult;
//				Mult.put(0, Sgn);
//				ResPolynomials += ((*DescendantsNumResuls[DescIndex])*pResultPattern->get(i)*Mult);
//
//			}
//			else
//				ResPolynomials += ((*DescendantsNumResuls[DescIndex])*pResultPattern->get(i));
//		}
//	}
//	//if(DescLocalSgn!=1)
//	//	ResPolynomials*=DescLocalSgn;
//	//size_t InSize=DescendantsNumResuls.size();
//	//ASSERTPDD(InSize==ResultPattern.size());
//	//size_t OutSize=DescendantsNumResuls[0]->size();
//	//ResPolynomials.clear();
//	//ResPolynomials.resize(OutSize);
//	//for(size_t OutI=0;OutI<OutSize;OutI++)
//	//{
//	//	_CSparsePolynomial& OutPoly=ResPolynomials[OutI];
//	//	const _CSubModelNumericPattern& DescSet=*DescendantsNumResuls[OutI];
//	//	for(size_t InI=0;InI<InSize;InI++)
//	//	{
//	//		OutPoly+=(DescSet[InI]*ResultPattern[InI]);
//	//	}
//	//}
//}

void _CSubCircuitSocket::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	for(_CIntNodes::iterator it=m_ConnectedNodes.begin(),_e=m_ConnectedNodes.end();
		it!=_e;it++)
		Numbers2Exchange.ExchangeIfNeede(*it);
}

void _CSubCircuitSocket::GetSimplyVertex(_CModelVertex* pBaseVertex,_CSExpandedVertices& Vertices,_CSExpandedDescendends& Descendents,
										 short& TopSgn, const _CDescSigns& BottomSgn, bool Approximated, const NumericType& Accuracy)
//_CSimpleVertexContainer _CSubCircuitSocket::GetSimplyVertex(_CSimpleVertexVector& Descendents)
{
	_CSExpandedDescendends ModelDescendents/*(Descendents.GetModelVertex())*/;
	for each(int V in m_AllParamConnections)
	{
		if(V<0)
			ModelDescendents.push_back(&StaticZeroVertices);
		else
			ModelDescendents.push_back(Descendents[V]);
	}
	//_CSimpleVertexMap &VerticesMap=m_VerticesMapBuffor.GetNewEmptyMap(m_pBaseCircuit);
	_CSubCircuitVerticesMap* pNewMap=new _CSubCircuitVerticesMap(m_pModel/*,m_VerMapCache*/);
	try
	{
		pNewMap->TranslateLocalTerminals(ModelDescendents,EmptyString);
		pNewMap=m_pModel->RegisterMap(pNewMap);
		//m_VerticesMaps.TranslateLocalTerminals(ModelDescendents,EmptyString);
		_CRealGraphDevelopement Container(0,m_InstanceName,ModelDescendents,m_pModel->GetCofactor(1)->GetMainVertex());
		//_CSExpandedVertices& ResVertices=m_pModel->FindOrRegisterResult(&Container);
		_CSExpandedVertices* pResVertices=NULL;
		_CRealGraphDevelopement* pResContainer=m_pModel->FindExisting(&Container);
		if(pResContainer==NULL)
		{
			//_CSExpandedVertices ResVertices;
			m_pModel->DevelopeRealTree(m_InstanceName,pResVertices, ModelDescendents,TopSgn, BottomSgn,Approximated,Accuracy);
			//pResVertices=m_pModel->GetUniqueSEExpandedVertices(ResVertices);
		}
		else
		{
			pResVertices=pResContainer->GetVertexResult();
			TopSgn=pResContainer->GetTopSgnResult();
		}
		//if(ResVertices.IsEmpty())
		//	m_pModel->DevelopeRealTree(m_InstanceName,ResVertices, ModelDescendents);
		auto LocalCofId=m_pModel->PushLocalSVertices(*pResVertices);//poprawiamy
		Container.SetResult(pResVertices,TopSgn);
		m_pModel->RegisterResult(&Container);	
		//Vertices.SetInputConnectorData(LocalCofId, ResVertices, this, &m_VerticesMaps);
		long long Index=pBaseVertex->GetMyId();
		Vertices.SetInputConnectorData(LocalCofId, *pResVertices, this, pNewMap,Index);
	}
	catch(exception)
	{
		if(pNewMap!=NULL && !pNewMap->IsRegisterd())
			delete pNewMap;
		throw;
	}
}

void _CSubCircuitSocket::GetSimplyFlatVertex2(const _CModelVertex* pCallerVertex,
	_CModelVerticesPath& VerticesPath,
	_CPathTraitor& PathTraitor,
	unsigned long SPower,
	NumericType& CurrentAllowedInaccuracy,
	const _CFlatVertex*& pResultVertex,
	short& TopSgn,
	const _CDescModelNumericPattern& DescNumValues,
	_CCompRedefEntry& RedefEntry)
{
	_CSubModelNumericPattern* pScaler=NULL;
	VerticesPath.Step2NextVertex(pCallerVertex);
	bool IsNew=VerticesPath.GetBaseCircuit()->ForcePattern(VerticesPath,pScaler);
	VerticesPath.StepOutOneVertex();
	if(IsNew)
	{
		ASSERTPDD(pScaler!=NULL && pScaler->empty());
		const _CSubModelNumericPattern* pOldScaller=VerticesPath.GetTopSubModelNumericPatter();
		pScaler->NewScaler(*pOldScaller,DescNumValues);
	}
	VerticesPath.Step2NextVertex(pCallerVertex,pScaler);
	m_pModel->GetSimpleVertexApprox2(pResultVertex,TopSgn,SPower,CurrentAllowedInaccuracy,PathTraitor,VerticesPath,RedefEntry[this]);
	VerticesPath.StepOutOneVertex();
}

bool /*NewPath*/ _CSubCircuitSocket::PerformSimplyFlatVertexTravers(
		_CModelVertex* pCallerVertex,
		unsigned long SPower,
		_CPreFlatVertexContainer*& pResultVertex,
		short& TopSgn,
		_CNewSimplifierData& Data,
		_CCompRedefEntry& RedefEntry
		) 
{
	bool Result=false;
	_CModelVerticesPath& VerticesPath=Data.ModelVerticesPath;
	_CSubModelNumericPattern* pScaler=NULL;
	if(!Data.PathTraitor.GetApproxCriterion()->IsExact())
	{
		VerticesPath.Step2NextVertex(pCallerVertex,pScaler);
		bool IsNew=Data.SubModelNumericParrernStorage.ForcePattern(VerticesPath,pScaler);
		VerticesPath.StepOutOneVertex();
		if(IsNew)
		{
			ASSERTPDD(pScaler!=NULL && pScaler->empty());
			_CDescModelNumericPattern DescNumValues;
			_CDescSigns2 Sgns;
			_CDirNumContainers Locker;
			pCallerVertex->DetermineNumericDescendants(DescNumValues,Sgns,Locker,Data,RedefEntry);
			const _CSubModelNumericPattern* pOldScaller=VerticesPath.GetTopSubModelNumericPatter();
			pScaler->NewScaler(pOldScaller,DescNumValues,Sgns,Data.MaxPower());
		}
	}
	VerticesPath.Step2NextVertex(pCallerVertex,pScaler);
	Result=m_pModel->PerformNewSimplyfyApprox(pResultVertex,TopSgn,SPower,Data,RedefEntry[this]);
	VerticesPath.StepOutOneVertex();
	return Result;
}

//void _CSubCircuitSocket::GetSimplyFlatVertex(_CModelVerticesTraitor& VerticesTraitor,
//void _CSubCircuitSocket::_GetSimplyFlatVertex(_CModelVerticesPath& VerticesPath,
//											 _CPathTraitor& PathTraitor,
//											 const _CSparsePolynomial& TopNumValue,
//											 const _CSPowerLimiter& TopLimiter,
//											 const _CFlatVerticesSPowerLimiter& DescLimiters,
//											 _CSExpFlatVertices& ResVertices,
//											 const _CMultLevelSExpFlatVertices& Descendents,
//											 const _CSubModelNumericPattern& Scaler,
//											 const _CDescModelNumericPattern& DescNumValues,
//											 //short& TopSgn, 
//											 const _CDescSigns2& BottomSgn)
//{
//	_CSExpFlatVerticesFilter Filter;
//	_CMultLevelSExpFlatVertices RealDescendents;
//	for(size_t i=0;i<m_AllParamConnections.size();i++)
//	{
//		int V=m_AllParamConnections[i];
//		if(V<0 || !DescLimiters.IsEntryAllowed(i))
//			RealDescendents.force_at(i)=NULL;
//		else
//			RealDescendents.force_at(i)=Filter.PutFiltered(Descendents[i],BottomSgn[i]);
//	}
//	ASSERTPDD(DescNumValues.size()==Descendents.size());
//	_CSubModelNumericPattern NewScaler(Scaler,DescNumValues);
//	_CFlatVerticesResContainerWrapper Wrapper;
//	_CSExpFlatVertices* pResVertices=NULL;
//	m_pModel->DevelopeRealTree(pResVertices,Wrapper,TopNumValue,TopLimiter,NewScaler,RealDescendents,/*TopSgn,
//		BottomSgn,*/PathTraitor,VerticesPath);
//		//BottomSgn,PathTraitor,VerticesTraitor);
//	ResVertices=*pResVertices;
///*
//void _CModelVertex::GetSimpleVertexApprox( _CSExpFlatVertices*& pOutResVertices, 
//		                                   _CFlatVerticesResContainer*& pCacheContainer,
//										   _CSPowerLimiter& Limiter,
//										   const _CSubModelNumericPattern& Scaler,
//										   _CMultLevelSExpFlatVertices& TerminalVertices, 
//										   short& TopSgn, 
//										   const _CDescSigns& BottomSgns,
//										   _CPathTraitor& MainTraitor,
//										   _CModelVerticesTraitor& VerticesTraitor)
//*/
//
//	//_CSubCircuitVerticesMap* pNewMap=new _CSubCircuitVerticesMap(m_pModel/*,m_VerMapCache*/);
//	//try
//	//{
//	//	pNewMap->TranslateLocalTerminals(ModelDescendents,EmptyString);
//	//	pNewMap=m_pModel->RegisterMap(pNewMap);
//	//	_CRealGraphDevelopement Container(0,m_InstanceName,ModelDescendents,m_pModel->GetCofactor(1)->GetMainVertex());
//	//	_CSExpandedVertices* pResVertices=NULL;
//	//	_CRealGraphDevelopement* pResContainer=m_pModel->FindExisting(&Container);
//	//	if(pResContainer==NULL)
//	//	{
//	//		m_pModel->DevelopeRealTree(m_InstanceName,pResVertices, ModelDescendents,TopSgn, BottomSgn,Approximated,Accuracy);
//	//	}
//	//	else
//	//	{
//	//		pResVertices=pResContainer->GetVertexResult();
//	//		TopSgn=pResContainer->GetTopSgnResult();
//	//	}
//	//	size_t LocalCofId=m_pModel->PushLocalSVertices(*pResVertices);//poprawiamy
//	//	Container.SetResult(pResVertices,TopSgn);
//	//	m_pModel->RegisterResult(&Container);	
//	//	long long Index=pBaseVertex->GetMyId();
//	//	Vertices.SetInputConnectorData(LocalCofId, *pResVertices, this, pNewMap,Index);
//	//}
//	//catch(exception e)
//	//{
//	//	if(pNewMap!=NULL && !pNewMap->IsRegisterd())
//	//		delete pNewMap;
//	//	throw;
//	//}
//}


//const _CSubModelNumericPattern* _CSubCircuitSocket::GetMyNumPattern(_CSubCircuitSocket* pOwnerSocket, _CPathTraitor& PathTraitor)
//{
//	_CPathTraitor::COMP_RESULT CompRes=PathTraitor.Compare(this);
//	short& Comp=CompRes.first;
//	short& Oper=CompRes.second;
//	if(Comp>0 && Oper==_CPathTraitor::DIVIDER)
//		return &ZeroPattern;
//	if(Comp==0)
//		PathTraitor.SubFollower();
//	else
//		PathTraitor.SideFollower();
//	const _CSubModelNumericPattern*& pResultPattern=m_LocalResPolynomials[&PathTraitor.StrContext2()];
//	if(pResultPattern==NULL)
//	{	
//		_CSubCircuit* pModel=GetModelCircuit();
//		_CSubModelNumericPattern& GlobalResultPattern=pModel->GetExistingNumPoly(&PathTraitor.StrContext2(),m_RedefParameters,m_IAmNumericCopyOf);
//		pResultPattern=&GlobalResultPattern;
//		if(GlobalResultPattern.empty())
//		{
//			const _CSubModelNumericPattern* pLocalRes=NULL;
//			_CDirectNumContainer* pCacheEntry=NULL;
//			_CCofactor* pCof=pModel->m_Cofactors[0];//Context?
//			_CModelVertex* pMVertex=pCof->GetMainVertex(&PathTraitor.GetBaseContext());
//			pMVertex->PrepareDirectNumericalDivRem(pLocalRes,pCacheEntry,PathTraitor,this);
//			GlobalResultPattern=*pLocalRes;
//			short TopSgn=pCof->GetGlobalMultiplier();
//			if(TopSgn!=1)
//			{
//				fstream file("TopSgn.txt",ios::out | ios::app);
//				file<<"TopSgn = "<<TopSgn<<endl;
//				_CSparsePolynomial MultP;
//				MultP.put(0,TopSgn);
//				GlobalResultPattern*=MultP;
//			}
//			pModel->NotifyReadyNew(&PathTraitor.StrContext2(),m_RedefParameters);
//			m_DirNumCache.HibernateAll();
//		}
//	}
//	PathTraitor.PopLastLevel();
//	return pResultPattern;
//}


const _CSubModelNumericPattern* _CSubCircuitSocket::GetMyNumPattern(_CCompRedefEntry& RedefEntry, _CPathTraitor& PathTraitor)
{
	_CPathTraitor::COMP_RESULT CompRes = PathTraitor.Compare(this);
	short& Comp = CompRes.first;
	short& Oper = CompRes.second;
	if (Comp>0 && Oper == _CPathTraitor::DIVIDER)
		return &ZeroPattern;
	if (Comp == 0)
		PathTraitor.SubFollower();
	else
		PathTraitor.SideFollower();
	_CCompRedefEntry& NextEntry = RedefEntry[this];
	NextEntry.WeekMergeSubtree(m_pModel->GetComponentRedef());//le
	const _CCompRedefEntry* pUniqRedefEntry = m_pBaseCircuit->GetUniqCompRedef(&NextEntry);


	const _CSubModelNumericPattern*& pResultPattern = m_LocalResPolynomials[&PathTraitor.StrContext2()][pUniqRedefEntry];
	if (pResultPattern == NULL)
	{
		_CSubCircuit* pModel = GetModelCircuit();
		_CSubModelNumericPattern& GlobalResultPattern = pModel->GetExistingNumPoly(&PathTraitor.StrContext2(), pUniqRedefEntry);//uniq
		pResultPattern = &GlobalResultPattern;
		if (GlobalResultPattern.empty())
		{
			const _CSubModelNumericPattern* pLocalRes = NULL;
			_CDirectNumContainer* pCacheEntry = NULL;
			_CCofactor* pCof = pModel->m_Cofactors[0];//Context?
			_CModelVertex* pMVertex = pCof->GetMainVertex(&PathTraitor.GetBaseContext());
			pMVertex->PrepareDirectNumericalDivRem(pLocalRes, pCacheEntry, PathTraitor, RedefEntry[this]);//?
			GlobalResultPattern = *pLocalRes;
			short TopSgn = pCof->GetGlobalMultiplier();
			if (TopSgn != 1)
			{
				fstream file("TopSgn.txt", ios::out | ios::app);
				file << "TopSgn = " << TopSgn << endl;
				_CSparsePolynomial MultP;
				MultP.put(0, TopSgn);
				GlobalResultPattern *= MultP;
			}
			pModel->NotifyReadyNew(&PathTraitor.StrContext2(), pUniqRedefEntry);
			m_DirNumCache.HibernateAll();
		}
	}

	//fstream file("Redefs.txt", ios::out | ios::app);
	//file << "Subcircuit - " << m_InstanceName << endl;
	//RedefEntry[this].WriteToText(file);
	////pUniqRedefEntry->WriteToText(file);
	//file << endl;

	PathTraitor.PopLastLevel();
	return pResultPattern;
}

//_CSubModelNumericPattern* _CSubCircuitSocket::GetNumPatIfRedefined(const _CComponent* pComponent,  const NumericType*& pValue)
//{
//	_CRedefParamValues::iterator rit=m_RedefParameters.find(pComponent);
//	if(rit==m_RedefParameters.end())
//		return NULL;
//	pValue=&(rit->second);
//	return &m_Comp2NumPat[pComponent];
//}

IMPLEMENT_DYNAMIC_CREATION(_CSubCircuitSocket);
void _string_list::parse(const string& input_line,const string& white_spaces)
{
	char* in_l=_strdup(input_line.c_str());
	if(in_l==NULL)
		throw bad_alloc();
	char* w_s=_strdup(white_spaces.c_str());
	if(w_s==NULL)
	{
		free(in_l);
		throw bad_alloc();
	}
	char* token=NULL;
	char* next_token=NULL;
	try
	{
		token=strtok_s(in_l,w_s,&next_token);
		while(token!=NULL)
		{
			push_back(token);
			token=strtok_s(NULL,w_s,&next_token);
		}
	}
	catch(exception)
	{
		free(w_s);
		free(in_l);
		throw;
	}
	free(w_s);
	free(in_l);
}

//long long _CSubCircuitSocket::REDEF_PARAMETERS_VALUE::GetHush(long long Core) const
//{
//	for(const_iterator it=begin(),_e=end();it!=_e;it++)
//	{
//		Core*=HASH_FACTOR;
//		Core^=(long long)it->first;
//		Core*=HASH_CORE;
//		Core^=GetTypedHash(Core,it->second);
//	}
//	return Core;
//}
