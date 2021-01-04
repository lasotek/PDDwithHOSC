#include "StdAfx.h"
#include "..\PDDErrorsAndConst.h"
#include "Circuit.h"
#include "TwoPorts.h"
#include "ControledSources.h"
#include "IdealComponents.h"
#include "SubCircuit.h"
#include "SubCircuitSocket.h"
#include <fstream>
#include <algorithm>
#include "MainCircuit.h"
#include "MetaComponente.h"

////*******************************_CLocalSimpleVertexStore
//
//_CSimpleVertexContainer* _CLocalSimpleVertexStore::GetRealVertex(const _CSimpleVertexContainer& InputTerminal,const string& OperationStrId)
//{
//	ASSERTPDD(InputTerminal.IsInputConnector());
//	return GetRealVertex(InputTerminal.GetTerminalIndex(),OperationStrId);
//}
//
//_CSimpleVertexContainer* _CLocalSimpleVertexStore::GetRealVertex(unsigned long long SVId,const string& OperationStrId)
//{
//	_id_pair Pair(SVId,OperationStrId);
//	iterator it=m_Map.find(Pair);
//	return it==m_Map.end() || it->first!=Pair?NULL:&(it->second);
//}
//
//void _CLocalSimpleVertexStore::SetRealVertex(const _CSimpleVertexContainer& InputTerminal,const string& OperationStrId,const _CSimpleVertexContainer& RealVertex)
//{
//	ASSERTPDD(InputTerminal.IsInputConnector());
//	m_Map.insert(value_type(_id_pair(InputTerminal.GetTerminalIndex(),OperationStrId),RealVertex));
//}
//void _CLocalSimpleVertexStore::SetRealVertex(unsigned long long SVId, const string& OperationStrId,const _CSimpleVertexContainer& RealVertex)
//{
//	m_Map.insert(value_type(_id_pair(SVId,OperationStrId),RealVertex));
//}
//
//bool _CLocalSimpleVertexStore::_cmp::operator() (const _id_pair& Left, const _id_pair& Right) const 
//{
//	if(Left.m_OperDescr>Right.m_OperDescr)
//		return true;
//	else if(Left.m_OperDescr==Right.m_OperDescr && Left.m_VCId>Right.m_VCId)
//		return true;
//	else
//		return false;
//}
//

//*******************************_CContextSExpandedCofactorValues
_CSExpandedCofactorValues* _CContextSExpandedCofactorValues::GetSExpandedCofValues2(const string& Context,TO_DO_IF_NOT_EXISTS ToDo,const string** ppNewConetxt)
{
	iterator it=m_vMap.find(Context);
	if(it==m_vMap.end())
	{
		if(ToDo==FORCE_IF_NOT_EXIST)
		{
			pair<iterator,bool> p=m_vMap.insert(value_type(Context,_CSExpandedCofactorValues()));
			if(!p.second)
			{
				RISEPDD(eUnexpectedCondition,"The problems with m_vMap");
			}
			else
				return &(p.first->second);
		}
		//if(ToDo==INHERIT_FROM_PARENTS)
		//	for(;;m_Counter++)
		//	{
		//		const string* pParent=m_pBaseCircuit->GetParentContext(Context,m_Counter);
		//		if(pParent==NULL)
		//			return NULL;
		//		if(ppNewConetxt!=NULL)
		//			*ppNewConetxt=pParent;
		//		_CSExpandedCofactorValues* Res=GetSExpandedCofValues2(*pParent,ToDo,ppNewConetxt);
		//		if(Res!=NULL)
		//			return Res;
		//	}
		if(ToDo==MUST_EXIST)
			return NULL;
	}
	return &(it->second);
}

_CSExpandedCofactorValues& _CContextSExpandedCofactorValues::GetSExpandedCofValues(const string& Context,TO_DO_IF_NOT_EXISTS ToDo,const string** ppNewConetxt)
{
	_CSExpandedCofactorValues* pRes=GetSExpandedCofValues2(Context,ToDo,ppNewConetxt);
	if(pRes==NULL)
	{
		RISEPDD(eVertexEntryUndifined,(Context+" is not defined").c_str());
	}
	else
		return *pRes;
}
_CSExpandedVertices& _CContextSExpandedCofactorValues::GetSExpandedVertices(const string& Context,TO_DO_IF_NOT_EXISTS ToDo,size_t CofId,const string** ppNewConetxt)
{
	_CSExpandedVertices* pRes=GetSExpandedVertices2(Context,ToDo,CofId,ppNewConetxt);
	if(pRes==NULL)
	{
		RISEPDD(eVertexEntryUndifined,(Context+" is not defined").c_str());
	}
	else
		return *pRes;
}

_CSExpandedVertices* _CContextSExpandedCofactorValues::GetSExpandedVertices2(const string& Context,TO_DO_IF_NOT_EXISTS ToDo,size_t CofId,const string** ppNewConetxt)
{
	_CSExpandedCofactorValues* pSECV=GetSExpandedCofValues2(Context,ToDo,ppNewConetxt);
	if(pSECV==NULL)
		return NULL;
	size_t _size=pSECV->size();
	if(CofId>=_size)
		if(ToDo==FORCE_IF_NOT_EXIST)
			pSECV->resize(CofId+1);
		//else if(ToDo==INHERIT_FROM_PARENTS)
		//	for(;;m_Counter++)
		//	{
		//		const string* pParent=m_pBaseCircuit->GetParentContext(Context,m_Counter);
		//		if(pParent==NULL)
		//			return NULL;
		//		if(ppNewConetxt!=NULL)
		//			*ppNewConetxt=pParent;
		//		_CSExpandedVertices* Res=GetSExpandedVertices2(*pParent,ToDo,CofId,ppNewConetxt);
		//		if(Res!=NULL)
		//			return Res;
		//	}
		else
			return NULL;
	return &((*pSECV)[(size_t)CofId]);
}

_CSimpleVertexContainer* _CContextSExpandedCofactorValues::GetSExpandedCoef2(const string& Context,TO_DO_IF_NOT_EXISTS ToDo,unsigned short sPower,size_t CofId,const string** ppNewConetxt)
{
	return NULL;
}
_CSimpleVertexContainer& _CContextSExpandedCofactorValues::GetSExpandedCoef(const string& Context,TO_DO_IF_NOT_EXISTS ToDo,unsigned short sPower,size_t CofId,const string** ppNewConetxt, long* pSPowerShift)
{
	_CSExpandedVertices& SEV=GetSExpandedVertices(Context,ToDo,CofId,ppNewConetxt);
	size_t _size=SEV.size();
	if(sPower>=_size)
		if(ToDo==FORCE_IF_NOT_EXIST)
			SEV.resize(sPower+1);
		else if(ToDo==MUST_EXIST)
			throw eVertexEntryUndifined();
		//else if(ToDo=INHERIT_FROM_PARENTS)
		//{	
		//	*ppNewConetxt=m_pBaseCircuit->GetParentContext(Context,m_Counter);
		//	m_Counter++;
		//}
		else
			RISEPDD(eUnexpectedCondition,"Wrong ToDo");
	if(ppNewConetxt!=NULL)
		*ppNewConetxt=m_NextContextMap.get(&Context,CofId,sPower);
	if(pSPowerShift!=NULL)
		*pSPowerShift=m_NextSPower.get(&Context,CofId,sPower);
	_CSimpleVertexContainer& Res=SEV[sPower];
	if(ToDo==MUST_EXIST && Res.IsEmpty())
		throw eVertexEntryUndifined();
	return Res;
}
_CSimpleVertexContainer* _CContextSExpandedCofactorValues::FindSimpleVertexContainer(const string& Context,unsigned short sPower, size_t CofId, const string** ppNewContext)
{
	m_Counter=0;

	return NULL;
}

bool _CContextSExpandedCofactorValues::Exists(const string& Context, unsigned short sPower, size_t CofId)
{
	iterator it=m_vMap.find(Context);
	if(it==m_vMap.end())
		return false;
	_CSExpandedCofactorValues& SECV=it->second;
	if(SECV.size()<=CofId)
		return false;
	_CSExpandedVertices& SEV=SECV[CofId];
	return SEV.size()>sPower && !SEV[sPower].IsEmpty();
}

void _CContextSExpandedCofactorValues::SetNoOfCofactors(const string& Context, size_t NewNo)
{
	_CSExpandedCofactorValues& SECV=GetSExpandedCofValues(Context);
	SECV.resize(NewNo);
}

void _CContextSExpandedCofactorValues::SetNoOfSCoef(const string& Context,size_t CofId,unsigned short sPowers)
{
	_CSExpandedVertices& SEV=GetSExpandedVertices(Context,FORCE_IF_NOT_EXIST,CofId);
	SEV.resize(sPowers);
}

_CSExpandedVertices& _CContextSExpandedCofactorValues::GetNewSVertices(const string& Context)
{
	_CSExpandedCofactorValues& SECV=GetSExpandedCofValues(Context);
	return GetSExpandedVertices(Context,FORCE_IF_NOT_EXIST,SECV.size());
}

size_t _CContextSExpandedCofactorValues::PushUniqueSExpandedVertices(const string& Context,const _CSExpandedVertices& Vertices)
{
	_CSExpandedCofactorValues& SECV=GetSExpandedCofValues(Context);
	size_t i=0,_size=SECV.size();
	for(;i<_size;i++)
		if(SECV[i]==Vertices)
			return i;
	SECV.push_back(Vertices);
	return SECV.size()-1;
}

size_t _CContextSExpandedCofactorValues::PushUniqueVertex(size_t EntryId, size_t PosId, 
														  _CSimpleVertexContainer& NewVertex, 
														  const string& Context, const string& PostContext)
{
	m_NextContextMap.put(&Context,EntryId,PosId,&PostContext);
	_CSExpandedVertices& SEV=GetSExpandedVertices(Context,FORCE_IF_NOT_EXIST,EntryId);
	size_t _size=SEV.size();
	if(PosId>=_size)
		SEV.resize(PosId+1);
	else
	{
		_CSimpleVertexContainer& Cont=SEV[PosId];
		if(Cont.IsEmpty())
		{
			Cont=NewVertex;
			return PosId;
		}
		if(Cont!=NewVertex)
		{
			for(size_t i=0;i<_size;i++)
				if(SEV[i]==NewVertex)
					return i;
			PosId=_size;
			SEV.resize(_size+1);
		}
	}
	SEV[PosId]=NewVertex;
	return PosId;
}

bool /*Was Changed*/ _CContextSExpandedCofactorValues::RegisterAndPreparInputVertex
	(size_t EntryId, size_t PosId, const _CSubCircuitSocket* pSocket, _CSubCircuitVerticesMap* pMap,
	const MVIDS& MVId,
	_CSimpleVertexContainer& NewVertex, const string& Context, const string& PostContext)
{
	if(NewVertex.Is01M1())
	{
		//GetSExpandedCoef(Context,true,PosId,EntryId)=NewVertex;
		return true;
	}
	size_t NewPosId=PushUniqueVertex(EntryId,PosId,NewVertex,Context,PostContext);
	NewVertex=_CSimpleVertexContainer(EntryId,NewPosId,pSocket,pMap,MVId.back());
	return PosId!=NewPosId;
}

void _CContextSExpandedCofactorValues::WriteToStream(iostream& stream,_CSimpleVerticesMapsSet& Set)
//void _CContextSExpandedCofactorValues::WriteToStream(iostream& stream)
{
	iterator it=m_vMap.find(EmptyString), _end=m_vMap.end();
	if(it!=_end)
	{
		stream<<"\t Main Context: \n";
		_CSExpandedCofactorValues& Values=it->second;
		size_type _size=Values.size();
		for(size_type i=0;i<_size;i++)
		{
			stream<<"\t\t Model Entry No. "<<i<<":";
			Values[i].WriteToStream(stream,Set);
			//Values[i].WriteToStream(stream);
		}
	}
	for(it=m_vMap.begin();it!=_end;it++)
	{
		const string& Context=it->first;
		if(Context.empty())
			continue;
		stream<<"\t Conext "<<Context<<": \n";
		_CSExpandedCofactorValues& Values=it->second;
		size_type _size=Values.size();
		for(size_type i=0;i<_size;i++)
		{
			stream<<"\t\t Model Entry No. "<<i<<":";
			Values[i].WriteToStream(stream,Set);
			//Values[i].WriteToStream(stream);
		}
	}
}

void _CContextSExpandedCofactorValues::WriteToStream(size_t CofId, iostream& stream, _CSimpleVerticesMapsSet& Set)
//void _CContextSExpandedCofactorValues::WriteToStream(size_t CofId, iostream& stream)
{
	iterator it=m_vMap.find(EmptyString);
	if(it!=m_vMap.end())
	{
		stream<<"\t Main Context: \n";
		_CSExpandedCofactorValues& Values=it->second;
		stream<<"\t\t";
		Values[CofId].WriteToStream(stream,Set);
		//Values[CofId].WriteToStream(stream);
	}
	it=m_vMap.begin();
	iterator _end=m_vMap.end();
	for(;it!=_end;it++)
	{
		const string& Context=it->first;
		if(Context.empty())
			continue;
		stream<<"\t Conext "<<Context<<": \n";
		_CSExpandedCofactorValues& Values=it->second;
		stream<<"\t\t";
		Values[CofId].WriteToStream(stream,Set);
		//Values[CofId].WriteToStream(stream);
	}
}

//_CNumericalSCofactors& _CContextSExpandedCofactorValues::GetNumericalSCofValues(const string& Context)
//{
//	niterator it=m_NumMap.find(Context);
//	if(it==m_NumMap.end())
//	{
//		pair<niterator,bool> p=m_NumMap.insert(nvalue_type(Context,_CNumericalSCofactors()));
//		if(!p.second)
//		{
//			RISEPDD(eUnexpectedCondition,"The problems with m_NumMap");
//		}
//		else
//			return p.first->second;
//	}
//	return it->second;
//}
//
//_CNumericalSCoeff& _CContextSExpandedCofactorValues::GetNumericalSCoeffs(const string& Context,size_t CofId,bool Synchronize)
//{
//	static _CNumericalSCoeff Empty;
//	return Empty;
//}
//
//_CNumericalSEntry& _CContextSExpandedCofactorValues::GetNumericalSEntry(const string& Context,unsigned short sPower,size_t CofId,bool Synchronize)
//{
//	static _CNumericalSEntry Empty;
//	return Empty;
//}

//**************************_CCircuit
_CCircuit::_CCircuit(bool IsLocal,bool OrderOptimalization,bool UseNewAnalyser, const string& GlobalRefNode):
m_OptimizeComponentOrder(OrderOptimalization),m_LocalNodes(GlobalRefNode),m_IsLocal(IsLocal),
m_ComponentListFinished(false),m_IsGlobal0Node(!GlobalRefNode.empty()),m_DestroyingInProgess(false),
m_pNextComponentOptimizer(NULL), m_LargestDescs(2), m_pNewAnalyxer(NULL), m_UseNewAnalyser(UseNewAnalyser), m_CirCompRedefEntry(nullptr,nullptr)
{
	m_ModelVerticesCache.SetOwnerCircuit(this);
	m_Cofactors.SetOwner(this);
	m_VerticesInAnalysis.SetOwner(this);
	m_NewCreatedVertices.SetOwner(this);
	m_IdealComponentsList.SetOwner(this);
	m_LocalComponentsList.SetOwner(this);
	m_GraphStateCache.SetOwner(this);
	m_CSSimpleMainVertices.SetCircuit(this);
	m_CirCompRedefEntry.m_pOwner = this;
}

_CCircuit::~_CCircuit(void)
{
	m_DestroyingInProgess=true;
	//if(m_pNextComponentOptimizer!=NULL)
		delete m_pNextComponentOptimizer;
	//for(COFACTORS_IT it=m_Cofactors.begin();it!=m_Cofactors.end();it++)
	//	if((*it)!=NULL)
	//		delete (*it);
		delete m_pNewAnalyxer;
}

_CGraphTable* _CCircuit::PushIntoCache(_CGraphTable* pObject)
{
	_CGraphTable* pNewObject=NULL;
	try
	{
		pNewObject=m_GraphTableCache.PushInCache(pObject,false);
		pNewObject->m_IsRegistered=true;
		return pNewObject;
	}
	catch(exception)
	{
		if(pObject!=NULL)
			if(!pObject->IsRegistered())
				delete pObject;
		//if(pNewObject!=pObject && pObject!=NULL)
		//	delete pObject;
		throw;
	}
}
_CIntegrityTable* _CCircuit::PushIntoCache(_CIntegrityTable* pObject)
{
	_CIntegrityTable* pNewObject=NULL;
	try
	{
		//pNewObject=m_IntegrityTableCache.PushInCache(pObject,false);
		pNewObject=m_AdvIntegrityTableCache.GetTarget().PushInCache(pObject,false);
		pNewObject->m_IsRegistered=true;
		return pNewObject;
	}
	catch(exception)
	{
		if(pObject!=NULL)
			if(!pObject->IsRegistered())
				delete pObject;
		//if(pNewObject!=pObject && pObject!=NULL)
		//	delete pObject;
		throw;
	}
}
_CGraphState* _CCircuit::PushIntoCache(_CGraphState* pObject)
{
	_CGraphState* pNewObject=NULL;
	try
	{
		pNewObject=m_GraphStateCache.PushInCache(pObject,false);
		pNewObject->m_IsRegistered=true;
		return pNewObject;
	}
	catch(exception)
	{
		if(pObject!=NULL)
			pObject->DestroyIfUnregistered();
//		if(pNewObject!=pObject && pObject!=NULL)
//			delete pObject;
		throw;
	}
}

_CModelVertex* _CCircuit::PushIntoCache(_CModelVertex* pObject)
{
#ifdef _DEBUG
	_CGraphState* pInputState=pObject->m_pInputState;
#endif
	if(pObject->Is0())
	{
		pObject->DestroyIfUnregistered();
		return GetGlobal0Vertex();
	}
	if(pObject->Is1())
	{
		pObject->DestroyIfUnregistered();
		return GetGlobal1Vertex();
	}
	if(pObject->IsM1())
	{
		pObject->DestroyIfUnregistered();
		return GetGlobalM1Vertex();
	}
	_CModelVertex* pNewObject=NULL;
	try
	{
		pNewObject=m_ModelVerticesCache.PushInCache(pObject,false);
		pNewObject->m_IsRegistered=true;
		if(pNewObject==pObject)
			m_NewCreatedVertices.push_back(pObject);
		else if(!pObject->IsRegistered())
			delete pObject;
	}
	catch(exception)
	{
		if(pObject!=NULL)
			pObject->DestroyIfUnregistered();
		//if(pNewObject!=pObject && pObject!=NULL)
		//	delete pObject;
		throw;
	}
	return pNewObject;
}

_CVertex* _CCircuit::PushIntoCache(_CVertex* pObject)
{
	_CVertex* pNewObject=NULL;
	try
	{
		pNewObject=m_SimpleLocalVerticesCache.PushInCache(pObject,false);
		pNewObject->m_IsRegistered=true;
		if(!pObject->IsRegistered())
			delete pObject;
	}
	catch(exception)
	{
		if(pObject!=NULL && !pObject->IsRegistered())
			delete pObject;
		//if(pNewObject!=pObject && pObject!=NULL)
		//	delete pObject;
		throw;
	}
	return pNewObject;
}

const char* check_info="Adding new componnts after calculation prohibites";
#define IDEAL_CHECK_COMPONENTYS if(m_ComponentListFinished)\
	RISEPDD(eIllegalOperation,check_info);

#define CHECK_COMPONENTYS IDEAL_CHECK_COMPONENTYS\
	if(!TestNameUniqness(name))\
	{\
		LastErrorInfo=string("You've just tried to redefine component ")+name;\
		return DUPLICATED_COMPONENT_NAME;\
	}

long _CCircuit::AddResistor(const string& name, const string& NodeA, const string& NodeB,bool IsConductance,long double Value)
{
	//CHECK_COMPONENTYS;
	try
	{
		_CTwoPort* pTwoPort=IsConductance?
			dynamic_cast<_CTwoPort*>(new _CConductance(this,name,TranslateOrAddNode(NodeA),TranslateOrAddNode(NodeB),Value)):
			dynamic_cast<_CTwoPort*>(new _CResistance(this,name,TranslateOrAddNode(NodeA),TranslateOrAddNode(NodeB),Value));
		if (!m_LocalComponentsList.AddAnotherComponent(pTwoPort,m_CompPriority))
		{
			delete pTwoPort;
			LastErrorInfo = name + " is not the coupled simply component component of the same type";
			return DUPLICATED_COMPONENT_NAME;
		}
		//m_LocalComponentsList.push_back(pTwoPort);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddResistor";
		return ALLOCATION_ERROR;
	}
	return 0;
}
long _CCircuit::AddCapacitor(const string& name, const string& NodeA, const string& NodeB,long double Value)
{
	//CHECK_COMPONENTYS;
	try
	{
		_CCapacitance* pCap=new _CCapacitance(this,name,TranslateOrAddNode(NodeA),TranslateOrAddNode(NodeB),Value);
		if (!m_LocalComponentsList.AddAnotherComponent(pCap,m_CompPriority))
		{
			delete pCap;
			LastErrorInfo = name + " is not the coupled simply component component of the same type";
			return DUPLICATED_COMPONENT_NAME;
		}
		//m_LocalComponentsList.push_back(pCap);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddCapacitor";
		return ALLOCATION_ERROR;
	}
	return 0;
}
long _CCircuit::AddInductor(const string& name, const string& NodeA, const string& NodeB,long double Value)
{
	//CHECK_COMPONENTYS;
	try
	{
		_CInductance* pInd=new _CInductance(this,name,TranslateOrAddNode(NodeA),TranslateOrAddNode(NodeB),Value);
		if (!m_LocalComponentsList.AddAnotherComponent(pInd,m_CompPriority))
		{
			delete pInd;
			LastErrorInfo = name + " is not the coupled simply component component of the same type";
			return DUPLICATED_COMPONENT_NAME;
		}
		//m_LocalComponentsList.push_back(pInd);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddInductor";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddTransconductance(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value, int sPower)
{
	//CHECK_COMPONENTYS;
	try
	{
		_CTransconductance* pgm=new _CTransconductance(this,name,
			TranslateOrAddNode(OutP), TranslateOrAddNode(OutM),
			TranslateOrAddNode(InP), TranslateOrAddNode(InM),
			Value,sPower);
		if (!m_LocalComponentsList.AddAnotherComponent(pgm,m_CompPriority))
		{
			delete pgm;
			LastErrorInfo = name + " is not the coupled simply component component of the same type";
			return DUPLICATED_COMPONENT_NAME;
		}
		//m_LocalComponentsList.push_back(pgm);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddTransconductance";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddTransimpedance(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value, int sPower)
{
	//CHECK_COMPONENTYS;
	try
	{
		_CTransimpedance* prm=new _CTransimpedance(this,name,
			TranslateOrAddNode(OutP), TranslateOrAddNode(OutM),
			TranslateOrAddNode(InP), TranslateOrAddNode(InM),
			Value,sPower);
		if (!m_LocalComponentsList.AddAnotherComponent(prm,m_CompPriority))
		{
			delete prm;
			LastErrorInfo = name + " is not the coupled simply component component of the same type";
			return DUPLICATED_COMPONENT_NAME;
		}
		//m_LocalComponentsList.push_back(prm);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddTransimpedance";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddVoltageGain(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value, int sPower)
{
	//CHECK_COMPONENTYS;
	try
	{
		_CVoltageGain* pk=new _CVoltageGain(this,name,
			TranslateOrAddNode(OutP), TranslateOrAddNode(OutM),
			TranslateOrAddNode(InP), TranslateOrAddNode(InM),
			Value,sPower);
		if (!m_LocalComponentsList.AddAnotherComponent(pk, m_CompPriority))
		{
			delete pk;
			LastErrorInfo = name + " is not the coupled simply component component of the same type";
			return DUPLICATED_COMPONENT_NAME;
		}
		//m_LocalComponentsList.push_back(pk);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddVoltageGain";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddCurrentGain(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value, int sPower)
{
	//CHECK_COMPONENTYS;
	try
	{
		_CCurrentGain* pb=new _CCurrentGain(this,name,
			TranslateOrAddNode(OutP), TranslateOrAddNode(OutM),
			TranslateOrAddNode(InP), TranslateOrAddNode(InM),
			Value,sPower);
		if (!m_LocalComponentsList.AddAnotherComponent(pb, m_CompPriority))
		{
			delete pb;
			LastErrorInfo = name + " is not the coupled simply component of the same type";
			return DUPLICATED_COMPONENT_NAME;
		}
		//m_LocalComponentsList.push_back(pb);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddCurrentGain";
		return ALLOCATION_ERROR;
	}
	return 0;
}
long _CCircuit::AddFDCFOA_RM(const string& name, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
	const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long double Value)
{
	try
	{
		_CFDCFOA_RM* pb = new _CFDCFOA_RM(this, name,
			TranslateOrAddNode(Vyp), TranslateOrAddNode(Vyn),
			TranslateOrAddNode(Ixp), TranslateOrAddNode(Ixn),
			TranslateOrAddNode(Izp), TranslateOrAddNode(Izn),
			TranslateOrAddNode(Vwp), TranslateOrAddNode(Vwn),
			Value);
		if (!m_LocalComponentsList.AddAnotherComponent(pb,m_CompPriority))
		{
			delete pb;
			LastErrorInfo = name + " is not the coupled simply component of the same type";
			return DUPLICATED_COMPONENT_NAME;
		}
		//m_LocalComponentsList.push_back(pb);
	}
	catch (bad_alloc& e)
	{
		LastErrorInfo = string(e.what()) + " in _CCircuit::AddCurrentGain";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddNullor(const string& OutP, const string& OutM, const string& InP, const string& InM)
{
	IDEAL_CHECK_COMPONENTYS;
	try
	{
		_CNullor* pn=new _CNullor(this,
			TranslateOrAddNode(OutP), TranslateOrAddNode(OutM),
			TranslateOrAddNode(InP), TranslateOrAddNode(InM));
		m_IdealComponentsList.push_back(pn);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddNullor";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddDVCCII(const string& Ix, const string& Iy1, const string& Iy2, const string& Iz1, const string& Iz2, const string& GND)
{
	IDEAL_CHECK_COMPONENTYS;
	try
	{
		int gnd = 0;
		if (m_IsGlobal0Node)
		{
			if (!GND.empty())
				RISEPDD(eWrongNodeName, "Global reference node was predefined");
		}
		else
			gnd = TranslateOrAddNode(GND);
		_CDVCCII* pn = new _CDVCCII(this, TranslateOrAddNode(Ix),
			TranslateOrAddNode(Iy1), TranslateOrAddNode(Iy2),
			TranslateOrAddNode(Iz1), TranslateOrAddNode(Iz2), gnd);
		m_IdealComponentsList.push_back(pn);
	}
	catch (bad_alloc& e)
	{
		LastErrorInfo = string(e.what()) + " in _CCircuit::AddNullor";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddFDCFOA_RM_INF(const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
	const string& Izp, const string& Izn, const string& Vwp, const string& Vwn)
{
	IDEAL_CHECK_COMPONENTYS;
	try
	{
		_CFDCFOA_RM_INF* pn = new _CFDCFOA_RM_INF(this,
			TranslateOrAddNode(Vyp), TranslateOrAddNode(Vyn),
			TranslateOrAddNode(Ixp), TranslateOrAddNode(Ixn), 
			TranslateOrAddNode(Izp), TranslateOrAddNode(Izn),
			TranslateOrAddNode(Vwp), TranslateOrAddNode(Vwn));
		m_IdealComponentsList.push_back(pn);
	}
	catch (bad_alloc& e)
	{
		LastErrorInfo = string(e.what()) + " in _CCircuit::AddFDCFOA_RM_INF";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddFDCCII(const string& Vx1, const string& Vx2, const string& Vy1, const string& Vy2, const string& Vy3, const string& Vy4,
	const string& Iz1, const string& Iz2, const string& GND)
{
	IDEAL_CHECK_COMPONENTYS;
	try
	{
		int gnd = 0;
		if (m_IsGlobal0Node)
		{
			if (!GND.empty())
				RISEPDD(eWrongNodeName, "Global reference node was predefined");
		}
		else
			gnd = TranslateOrAddNode(GND);
		_CFDCCII* pn = new _CFDCCII(this, 
			TranslateOrAddNode(Vx1), TranslateOrAddNode(Vx2),
			TranslateOrAddNode(Vy1), TranslateOrAddNode(Vy2), TranslateOrAddNode(Vy3), TranslateOrAddNode(Vy4),
			TranslateOrAddNode(Iz1), TranslateOrAddNode(Iz2), gnd);
		m_IdealComponentsList.push_back(pn);
	}
	catch (bad_alloc& e)
	{
		LastErrorInfo = string(e.what()) + " in _CCircuit::AddNullor";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddPathologicalPair(const string& OutP, const string& OutM, const string& InP, const string& InM,
									bool CurrentMirror,bool VoltageMirror)
{
	IDEAL_CHECK_COMPONENTYS;
	try
	{
		_CNullor* pn=new _CNullor(this,
			TranslateOrAddNode(OutP), TranslateOrAddNode(OutM)*(CurrentMirror?-1:1),
			TranslateOrAddNode(InP), TranslateOrAddNode(InM)*(VoltageMirror?-1:1));
		m_IdealComponentsList.push_back(pn);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddNullor";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CCircuit::AddVoltageFollower(const string& OutP, const string& OutM, const string& InP, const string& InM)
{
	IDEAL_CHECK_COMPONENTYS;
	try
	{
		_CValtageFollower* pn = new _CValtageFollower(this,
			TranslateOrAddNode(OutP), TranslateOrAddNode(OutM),
			TranslateOrAddNode(InP), TranslateOrAddNode(InM));
		m_IdealComponentsList.push_back(pn);
	}
	catch (bad_alloc& e)
	{
		LastErrorInfo = string(e.what()) + " in _CCircuit::AddVoltageFollower";
		return ALLOCATION_ERROR;
	}
	return 0;
}
long _CCircuit::AddCurrentFollower(const string& OutP, const string& OutM, const string& InP, const string& InM)
{
	IDEAL_CHECK_COMPONENTYS;
	try
	{
		_CCurrentFollower* pn = new _CCurrentFollower(this,
			TranslateOrAddNode(OutP), TranslateOrAddNode(OutM),
			TranslateOrAddNode(InP), TranslateOrAddNode(InM));
		m_IdealComponentsList.push_back(pn);
	}
	catch (bad_alloc& e)
	{
		LastErrorInfo = string(e.what()) + " in _CCircuit::AddCurrentFollower";
		return ALLOCATION_ERROR;
	}
	return 0;
}

const char* ErrorLackOfGlobal0="Op-amps and current conveyors contain internal reference node.\n"
							   "You should define global reference node for proper calulations";
long _CCircuit::AddOpAmp(const string& Out, const string& II, const string& NI)
{
	IDEAL_CHECK_COMPONENTYS;
	try
	{
		if(!m_IsGlobal0Node)
		{
			LastErrorInfo=ErrorLackOfGlobal0;
			return LACK_OF_REFNODE;
		}
		_COpAmp* pOpAmp=new _COpAmp(this,
			TranslateOrAddNode(II), TranslateOrAddNode(NI),
			TranslateOrAddNode(Out));
		m_IdealComponentsList.push_back(pOpAmp);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddOpAmp";
		return ALLOCATION_ERROR;
	}
	return 0;
}
//long _CCircuit::AddMOCCII(const string& X, const string& Y, const vector<string>& Zs, short Type)
//{
//	IDEAL_CHECK_COMPONENTYS;
//	if (!m_IsGlobal0Node)
//	{
//		LastErrorInfo = ErrorLackOfGlobal0;
//		return LACK_OF_REFNODE;
//	}
//	if (Type<0 || Type>CONV_ICCIIm)
//	{
//		LastErrorInfo = "Only 4 types of conveyors: CCIIp(0), CCIIm(1), ICCIIp(2), ICCIIm(3) are legal.";
//		return INVALID_CONVEYOR_TYPE;
//	}
//	try
//	{
//		//_CCurrentConveyor* pCC = new _CCurrentConveyor(this,
//		//	TranslateOrAddNode(X), TranslateOrAddNode(Y),
//		//	TranslateOrAddNode(Z), Type);
//		//m_IdealComponentsList.push_back(pCC);
//	}
//	catch (bad_alloc& e)
//	{
//		LastErrorInfo = string(e.what()) + " in _CCircuit::AddCurrentConveyor";
//		return ALLOCATION_ERROR;
//	}
//	return 0;
//}

long _CCircuit::AddBOCII(const string& X, const string& Y, const string& Zp, const string& Zm, short Type)
{
	IDEAL_CHECK_COMPONENTYS;
	if (!m_IsGlobal0Node)
	{
		LastErrorInfo = ErrorLackOfGlobal0;
		return LACK_OF_REFNODE;
	}
	if (Type<CONV_MOCCI || Type>CONV_IMOCCIII)
	{
		LastErrorInfo = "Only 4 types of conveyors: CONV_BOCCI, CONV_IBOCCI, CONV_BOCCII, CONV_IBOCCII,"
						" CONV_BOCCIIII, CONV_IBOCCIIII are legal.";
		return INVALID_CONVEYOR_TYPE;
	}
	try
	{
		_CBOCC* pCC = new _CBOCC(this, TranslateOrAddNode(X), TranslateOrAddNode(Y),
			TranslateOrAddNode(Zp), TranslateOrAddNode(Zm), Type);
		m_IdealComponentsList.push_back(pCC);
	}
	catch (bad_alloc& e)
	{
		LastErrorInfo = string(e.what()) + " in _CCircuit::AddCurrentConveyor";
		return ALLOCATION_ERROR;
	}
	return 0;

}

long _CCircuit::AddCurrentConveyor(const string& X, const string& Y, const string& Z, short Type)
{
	IDEAL_CHECK_COMPONENTYS;
	if(!m_IsGlobal0Node)
	{
		LastErrorInfo=ErrorLackOfGlobal0;
		return LACK_OF_REFNODE;
	}
	if(Type<0 || Type>CONV_ICCIIm)
	{
		LastErrorInfo="Only 4 types of conveyors: CCIIp(0), CCIIm(1), ICCIIp(2), ICCIIm(3) are legal.";
		return INVALID_CONVEYOR_TYPE;
	}
	try
	{
		_CCurrentConveyor* pCC=new _CCurrentConveyor(this,
			TranslateOrAddNode(X), TranslateOrAddNode(Y),
			TranslateOrAddNode(Z),Type);
		m_IdealComponentsList.push_back(pCC);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CCircuit::AddCurrentConveyor";
		return ALLOCATION_ERROR;
	}
	return 0;
}
long _CCircuit::IncludeSubcircuit(const string& name, const string& ModelName, const _CStrNodes& Connections, long InternalCofactor)
{
	//CHECK_COMPONENTYS;
	_CSubCircuit* pSubCircuit=GetSubcircuit(ModelName);
	if(pSubCircuit==NULL)
	{
		LastErrorInfo="Wrong Model Name";
		return INVALID_SUBCIRCUIT_NAME;
	}
	if (InternalCofactor<0 || (unsigned long)InternalCofactor > m_Cofactors.size()) {
		LastErrorInfo = "Wrong Cofactor Selected";
		return INVALID_COFACTOR_ID;
	}
	try
	{
		return IncludeSubcircuit(name,pSubCircuit,_CStrConnection2NodesConection(Connections), InternalCofactor);
	}
	catch(eWrongFormatConnection& e)
	{
		LastErrorInfo=e.what();
		return WRONG_CONNECTION_FORMAT;
	}
}

long _CCircuit::IncludeSubcircuit(const string& name, long ModelID, const _CStrNodes& Connections, long InternalCofactor)
{
	CHECK_COMPONENTYS;
	_CSubCircuit* pSubCircuit=GetSubcircuit(ModelID);
	if(pSubCircuit==NULL)
	{
		LastErrorInfo="Wrong Model ID";
		return INVALID_SUBCIRCUIT_NAME;
	}
	if (InternalCofactor<0 || (InternalCofactor>1 && (unsigned long)InternalCofactor > pSubCircuit->m_Cofactors.size())) {
		LastErrorInfo = "Wrong Cofactor Selected";
		return INVALID_COFACTOR_ID;
	}
	try
	{
		return IncludeSubcircuit(name,GetSubcircuit(ModelID),_CStrConnection2NodesConection(Connections), InternalCofactor);
	}
	catch(eWrongFormatConnection& e)
	{
		LastErrorInfo=e.what();
		return WRONG_CONNECTION_FORMAT;
	}
}

long _CCircuit::ForceModelCalculation(long ModelID,bool IgnoreSgnPropagation)
{
	_CSubCircuit* pSubCircuit=GetSubcircuit(ModelID);
	if(pSubCircuit==NULL)
	{
		LastErrorInfo="Wrong Model ID";
		return INVALID_CIRCUIT_HANDLER;
	}
	path_str Path;
	try
	{
		pSubCircuit->RequestModelAnalysis(IgnoreSgnPropagation);
	}
	catch(exception)
	{
		LastErrorInfo="Unexpected operation";
		return UNEXPECTED_ERROR;
	}
	return 0;
}

//void _CCircuit::CNodesConnections2IntNodes(const _CNodesConnections& Connections, _CIntNodes IntNodes)
//{
//}

long _CCircuit::IncludeSubcircuit(const string& name, _CSubCircuit* pModel, const _CNodesConnections& Connections, long InternalCofactor)
{
	//CHECK_COMPONENTYS;
 	_CIntNodes ExternalNodes;
	if(m_IsGlobal0Node)
	{
		ExternalNodes.resize(pModel->m_BoundaryNodes.size()+1,-1);
		ExternalNodes[0]=0;
	}
	else
	{
		ExternalNodes.resize(pModel->m_BoundaryNodes.size(),-1);
	}
	for(_CNodesConnections::const_iterator it=Connections.begin();it!=Connections.end();it++)
	{
		const string& sModelNode=it->first;
		const string& sCircuitNode=it->second;
		int ModelNode=pModel->ConvertBoundaryNode(sModelNode);
		if(ModelNode<0)
		{
			LastErrorInfo="Wrong model boundary name: "+sModelNode;
			return ILLEGAL_BOUNDRY_NODES;
		}
		int CircuitNode=TranslateOrAddNode(sCircuitNode);
		ExternalNodes[ModelNode]=CircuitNode;
	}
	_CSubCircuitSocket* pNewInstance=new _CSubCircuitSocket(this,name,pModel,ExternalNodes, InternalCofactor);
	m_LargestDescs=max(m_LargestDescs,pNewInstance->NoOfDescendants());
	if (!m_LocalComponentsList.AddAnotherComponent(pNewInstance,m_CompPriority))
	{
		LastErrorInfo = name + " is not the coupled simply component component of the same type";
		return DUPLICATED_COMPONENT_NAME;
	}
	//m_LocalComponentsList.push_back(pNewInstance);
	return 0;
}
long _CCircuit::IncludeSubcircuit(const string& InstanceName, _CSubCircuit* pModel, const _CIntNodes& ConnectionsTable, long InternalCofactor)
{
	_CSubCircuitSocket* pNewInstance=new _CSubCircuitSocket(this,InstanceName,pModel,ConnectionsTable, InternalCofactor);
	m_LargestDescs=max(m_LargestDescs,pNewInstance->NoOfDescendants());
	m_LocalComponentsList.push_back(pNewInstance);
	return 0;
}

_CMainCircuit* _CCircuit::GetMainTopCircuit()
{
	return dynamic_cast<_CMainCircuit*> (GetCircuit(0));
}

bool _CCircuit::ModifySubParameter(const string& ParamPath, long double NewValue)
{
	_CComponentPath CPath(this, ParamPath);
	m_CirCompRedefEntry.AddRedefValue(CPath, NewValue);
	return true;
}
const _CCompRedefEntry& _CCircuit::GetComponentRedefEntry(_CComponent* pComp)
{
	return m_CirCompRedefEntry[pComp];
}


_CComponent* _CCircuit::PrepareAndGetNextComponent(bool& IsLast)
{
	IsLast=false;
	if(m_OptimizeComponentOrder)
	{
		if(m_pNextComponentOptimizer==NULL)
		{
			m_pNextComponentOptimizer=new _CNextComponentOptimizer(m_LocalComponentsList,m_NewCreatedVertices,FirstInternalNode(),HighestNoOfNode());
			if(m_pNextComponentOptimizer==NULL)
				throw bad_alloc();
		}
		_CComponent* pNextComponent=m_pNextComponentOptimizer->GetNextComponent(IsLast);
		if(pNextComponent==NULL)
		{
			delete m_pNextComponentOptimizer;
			m_pNextComponentOptimizer=NULL;
		}
		return pNextComponent;
	}
	else
	{
		if(m_LocalComponentsList.empty())
			return NULL;
		IsLast=m_LocalComponentsList.size()==1;
		_CComponent* pResComp=m_LocalComponentsList.front();
		m_LocalComponentsList.pop_front();
		//if(m_CurrentComponentIt==m_LocalComponentsList.end())
		//{
		//	m_CurrentComponentIt=m_LocalComponentsList.begin();
		//	return NULL;
		//}
		//if(!(*m_CurrentComponentIt)->IsReadyToPlug())
		//	++m_CurrentComponentIt;
		//if(m_CurrentComponentIt==m_LocalComponentsList.end())
		//	return NULL;
		//_CComponent* pResComp=*(m_CurrentComponentIt++);
		//IsLast=m_CurrentComponentIt==m_LocalComponentsList.end();
		return pResComp;
	}
	return NULL;
}
unsigned long _CCircuit::CreateNewCofactor()
{
	_CCofactor* pNewCofactor=new _CCofactor(this);
	if(pNewCofactor==NULL)
		throw bad_alloc();
	try
	{
		m_Cofactors.push_back(pNewCofactor);
		return m_Cofactors.size();
	}
	catch(bad_alloc)
	{
		delete pNewCofactor;
		throw;
	}
	catch(out_of_range)
	{
		return 0;
	}
}
long _CCircuit::getCofactor(unsigned long hCofactor, _CCofactor*& pCofacotor) {
	if (hCofactor<=0 || m_Cofactors.size() > hCofactor)
		return -1;
	pCofacotor = m_Cofactors[hCofactor-1];
	return 0;
}

void _CCircuit::PlugComponents(_CIntegrityTable& IntegrityTable)
{
	for(COMP_IT it=m_LocalComponentsList.begin();it!=m_LocalComponentsList.end();it++)
		(*it)->PlugInComponent(IntegrityTable);
}

void _CCircuit::PrepareCofactors()
{
	m_ComponentListFinished=true;
	m_UniqeNamesSet.clear();
//	int NoNodes=NoOfNodes();
//	ASSERTPDD(NoNodes>1);
	_CGraphState* pState = new _CGraphState(this);
	_CModelVertex* pBasicVertex = new _CModelVertex(pState, this);
	if (!m_UseNewAnalyser)
	{
		PlugComponents(*pState->GetIntegrityTable());
		for (COMP_IT it = m_IdealComponentsList.begin(); it != m_IdealComponentsList.end(); it++)
			(*it)->ProcessCMVertex(*pBasicVertex, NoOfNodes());
		//		(*it)->PrepareLState(*pState,0);
	}
	//if (m_Cofactors.empty()) {
	//	ASSERTPDD(m_IsLocal);
	//	_CCofactor* pCofactor = new _CCofactor(this);
	//	m_Cofactors.push_back(pCofactor);
	//}
	{
		if(m_Cofactors.size()==0)
			CreateNewCofactor();
		for(COFACTORS_IT it=m_Cofactors.begin();it!=m_Cofactors.end();it++)
			(*it)->PrepareCofactor(*pBasicVertex);
	}
	pBasicVertex->DestroyIfUnregistered();
}

template<typename Item>
void List2InvertedPHashdMap(const list<Item>& List,hash_map<const Item*,size_t>& Map,size_t FirstIndex=0)
{
	for(list<Item>::const_iterator it=List.begin(),_e=List.end();it!=_e;it++,FirstIndex++)
		Map.insert(hash_map<const Item*,size_t>::value_type(&*it,FirstIndex));
}


void _CCircuit::TranslateTempVertices2ModelVertices()
{
#ifdef  _ALLOW_REDUNDANCY
	m_pNewAnalyxer->SimplifyToBeCanonic();
#endif
	const _CCircuitAnalyzer::VERTICES_LIST& TempVerticesList=m_pNewAnalyxer->GetTempVerticesList();
	//typedef hash_multimap<const _CTempVertex*,_CModelVertex**> COL_MAP;
	_CCircuitAnalyzer::COL_MAP& ColMap=m_pNewAnalyxer->GetColMap();
	const _CCircuitAnalyzer::TEMP_VERTEX_VECTOR& CofVerticesVector=m_pNewAnalyxer->GetCofVerticesVector(); 
	ASSERTPDD(m_Cofactors.size()==CofVerticesVector.size());
	for(size_t i=0;i<m_Cofactors.size();i++)
	{
		const _CTempVertex::DESC_DATA& TempVertexData=CofVerticesVector[i];
		_CCofactor* pCof=m_Cofactors[i];
		pCof->m_GlobalMultiplier=TempVertexData.second;
		if(TempVertexData.first==NULL)
		{
			m_pNewAnalyxer->RegisterTempVertex2ModelVertex(&(TempVertexData.first),&(pCof->m_pMainModelVertex));
		}
		else if(TempVertexData.first->Is0())
			pCof->m_pMainModelVertex=GetGlobal0Vertex();
		else if(TempVertexData.first->Is1())
			pCof->m_pMainModelVertex=GetGlobal1Vertex();
		else
			ColMap.insert(_CCircuitAnalyzer::COL_MAP::value_type(TempVertexData.first,&pCof->m_pMainModelVertex));

	}
	for(_CCircuitAnalyzer::VERTICES_LIST::const_iterator it=TempVerticesList.begin(),
		_e=TempVerticesList.end();it!=_e;it++)
	{
		const _CTempVertex& TVertex=*it;
		try
		{
			_CModelVertex* pNewVertex=new _CModelVertex;
			try
			{
				pNewVertex->m_OwnerCircuit=this;
				pNewVertex->ForceHash((long)TVertex.GetHash());
				long long Id=m_ModelVerticesCache.ForceIntoCache(pNewVertex);
				pair<_CCircuitAnalyzer::COL_MAP::iterator,_CCircuitAnalyzer::COL_MAP::iterator> Range=ColMap.equal_range(&TVertex);
				for(_CCircuitAnalyzer::COL_MAP::iterator mit=Range.first;mit!=Range.second;mit++)
				{
					*mit->second=pNewVertex;
					pNewVertex->TouchMe();
				}
				_CComponent* pComponent=TVertex.GetComponent();
				pNewVertex->m_pComponent=pComponent;
				size_t NoOfVertices=pComponent->NoOfDescendants();
				pNewVertex->ResizeDescendants(NoOfVertices);
				for(size_t i=0;i<NoOfVertices;i++)
				{
					_CDescendantContainer& Container=pNewVertex->m_Descendants[i];
					size_t RI=i;
					if(!pComponent->Translate2RealDescendant(RI))
					{
						Container.m_Multiplier=0;
						Container.m_pDescendant=GetGlobal0Vertex();
						continue;
					}
					const _CTempVertex::DESC_DATA& Data1=TVertex.GetDescData(RI);
					Container.m_Multiplier=Data1.second;
					if(Data1.first==NULL)
					{
						m_pNewAnalyxer->RegisterTempVertex2ModelVertex(&(Data1.first),&(Container.m_pDescendant));
					}
					else if(Data1.first->Is0())
						Container.m_pDescendant=GetGlobal0Vertex();
					else if(Data1.first->Is1())
						Container.m_pDescendant=GetGlobal1Vertex();
					else
						ColMap.insert(_CCircuitAnalyzer::COL_MAP::value_type(Data1.first,&Container.m_pDescendant));
				}
			}
			catch(...)
			{
				delete pNewVertex;
				throw;
			}
		}
		catch(std::bad_alloc)
		{
			throw;
		}
	}
}

void _CCircuit::CreateModelPDD()
{
	if(m_UseNewAnalyser)
	{
		PrepareNewCircuitAnalyser();
		m_pNewAnalyxer->PerformAnalysis(m_OptimizeComponentOrder);
		m_pNewAnalyxer->ClearGarbage();
		TranslateTempVertices2ModelVertices();
		//KillNewCricuitAnalyser();
	}
	else
	{
		_CComponent* pComponent=NULL;
		//m_CurrentComponentIt=m_LocalComponentsList.begin();
		bool IsLastOne=false;
		_CComponents RealLocalComponentsList;
		while((pComponent=PrepareAndGetNextComponent(IsLastOne))!=NULL)
		{
			m_AdvIntegrityTableCache.SwapAndPrepare();
			RealLocalComponentsList.push_back(pComponent);
			PostProcessComponent(pComponent);
			//m_VerticesInAnalysis.FreeStatestIfPosibble();
			m_VerticesInAnalysis.assign(m_NewCreatedVertices.begin(),m_NewCreatedVertices.end());
			m_NewCreatedVertices.clear();
			ClearTerminals();
			for(MODEL_V_IT it=m_VerticesInAnalysis.begin();it!=m_VerticesInAnalysis.end();it++)
			{
				pComponent->ProcessCMVertex(**it,IsLastOne?NoOfBoundaryNodes():NoOfNodes());
				(*it)->RealiseIntTable();
	//			(*it)->AddComponent(*pComponent,0);
			}
			//if(m_OptimizeComponentOrder)
			//RealLocalComponentsList.push_back(pComponent);
			//PostProcessComponent(pComponent);
		}
		//if(m_OptimizeComponentOrder)
		//{
		m_LocalComponentsList=RealLocalComponentsList;
		RealLocalComponentsList.clear();
		//}
	}
}


const char* oor="Wrong Cofactor ID";
_CCofactor* _CCircuit::GetCofactor(unsigned long CId)
{
	if(CId==0 || CId>m_Cofactors.size())
		RISEPDD(out_of_range,oor);
	_CCofactor* pCof=m_Cofactors[CId-1];
	if(pCof==NULL)
		RISEPDD(out_of_range,oor);
	return pCof;
}

_CModelVertex* _CCircuit::GetRegisteredMVertex(_CGraphState* pInputState)
{
	RES_STATE State=pInputState->GetState();
	if(State==TERM_SELFLOOP || State==TERM_DISCONNECTED || State==TERM_SIZE_REJECTED)
	{
		pInputState->DestroyIfUnregistered();
		return GetGlobal0Vertex();
	}
	try
	{
		pInputState->RegisterMembers();
		_CGraphState* pNewInputState=PushIntoCache(pInputState);
		if(!pInputState->IsRegistered())
			delete pInputState;
		return CountTerminals(PushIntoCache(new _CModelVertex(pNewInputState,this)));
	}
	catch(exception)
	{
		pInputState->DestroyIfUnregistered();
		throw;
	}
}

_CModelVertex* _CCircuit::GetRegisteredMVertex(_CModelVertex* pSourceVertex)
{
	if(pSourceVertex->Is0())
	{
		pSourceVertex->DestroyIfUnregistered();
		return GetGlobal0Vertex();
	}
	pSourceVertex->RegisterMembers();
	return PushIntoCache(pSourceVertex);
}

vector<int> _CCircuit::TranslateOrAddNodes(const vector<string>& Nodes)
{
	return m_LocalNodes.ConvertOrAddNodes(Nodes);
}

int _CCircuit::TranslateOrAddNode(const string& Node)
{
	return m_LocalNodes.ConvertOrAddNode(Node);
}

int _CCircuit::TranslateNode(const string& Node)
{
	return m_LocalNodes.ConvertNode(Node);
}

string _CCircuit::TranslateNode(int Node) 
{
	return m_LocalNodes.ConvertNode(Node);
}


void _CCircuit::ClearGarbage(MVERTEX_2_SIGN* pMVertex2Sgn,bool IgnoreSgnPropagation)
{
	m_VerticesInAnalysis.FreeStatestIfPosibble();
	m_VerticesInAnalysis.clear();
	m_NewCreatedVertices.FreeStatestIfPosibble();
	m_NewCreatedVertices.clear();
	Skip0SupressedAndDuplicated(pMVertex2Sgn, IgnoreSgnPropagation /*|| m_UseNewAnalyser*/);
	CleanExtraMaps();
	m_ModelVerticesCache.ClearGarbage();

	//m_GraphStateCache.ClearGarbage();
	//m_AdvIntegrityTableCache.Clear();
	//m_GraphTableCache.ClearGarbage();

	m_GraphStateCache.Clear();
	m_AdvIntegrityTableCache.Clear();
	m_GraphTableCache.Clear();
	if(m_UseNewAnalyser)
		KillNewCricuitAnalyser();
}
long long _CCircuit::GetStateTableIndex(_CGraphState* pTable) 
{
	if(pTable==NULL)
		return -1;
	return m_GraphStateCache.GetIndexOf(pTable,false);
}
_CGraphState* _CCircuit::GetStateTableFromIndex(long long Index) 
{
	if(Index<0)
		return NULL;
	return m_GraphStateCache[Index];
}

long long _CCircuit::GetModelVertexIndex(_CModelVertex* pVertex)
{
	if(pVertex==GetGlobal0Vertex())
		return 0;
	if(pVertex==GetGlobal1Vertex())
		return 1;
	if(pVertex==GetGlobalM1Vertex())
		return -1;
	return m_ModelVerticesCache.GetIndexOf(pVertex,false);
}

_CModelVertex* _CCircuit::GetModelVertexFromIndex(long long Index) 
{
	switch(Index)
	{
	case 0: return GetGlobal0Vertex();
	case 1: return GetGlobal1Vertex();
	default:
		try
		{
			return m_ModelVerticesCache[Index];
		}
		catch(out_of_range)
		{
			return NULL;
		}
	}
}
void _CCircuit::Store(_binary_filer& Filer)
{
	Filer<<m_LocalNodes;
	Filer<<m_IdealComponentsList;
	Filer<<m_LocalComponentsList;
	//if(!Filer.IsShortMode())
	//{
	//	Filer<<m_GraphTableCache;
	//	Filer<<m_IntegrityTableCache;
	//	Filer<<m_GraphStateCache;
	//}
	Filer<<m_ModelVerticesCache;
	Filer<<m_Cofactors;
//	Filer<<m_NewCreatedVertices;
	_streamable::Store(Filer);
}

void _CCircuit::Load(_binary_filer& Filer)
{
	m_ComponentListFinished=true;
	Filer>>m_LocalNodes;
	Filer>>m_IdealComponentsList;
	Filer>>m_LocalComponentsList;
	//if(!Filer.IsShortMode())
	//{
	//	Filer>>m_GraphTableCache;
	//	Filer>>m_IntegrityTableCache;
	//	Filer>>m_GraphStateCache;
	//}
	Filer>>m_ModelVerticesCache;
	Filer>>m_Cofactors;
	_streamable::Load(Filer);
//	Filer>>m_NewCreatedVertices;
}
_CVertex* _CCircuit::GlobalVertexFromId(long long Id) 
{
	if(Id==0 || Id==1 || Id==-1)
		return NULL;
	TGlobal_Vertex_Id GVId={Id};
	_CCircuit* pCircuit=GetCircuit(GVId.PARTS.Circuit_Id);
	return pCircuit==NULL?NULL:pCircuit->m_SimpleLocalVerticesCache.GetAt(GVId.PARTS.Local_Id);
}

long long _CCircuit::GlobalIdFromLocalVertex(_CVertex* pVertex) 
{
	TGlobal_Vertex_Id GVId;
	GVId.PARTS.Circuit_Id=GetCircuitId(this);
	GVId.PARTS.Local_Id=(long)m_SimpleLocalVerticesCache.GetIndexOf(pVertex,false);
	return GVId.Global_Id;
}

_CVertex* _CCircuit::GetSimpleVertex(_CComponent* pParameter, const _CSimpleVertexContainer& hSucc,  
		const _CSimpleVertexContainer& lSucc, bool Negated)
{
	_CVertex* pNewVertex=new _CVertex(this,pParameter,hSucc,lSucc,Negated);
	pNewVertex=PushIntoCache(pNewVertex);
	return pNewVertex;
}

_CSimpleVertexContainer _CCircuit::GetSimpleVertexId(_CComponent* pParameter, 
													 const _CSimpleVertexContainer& hSucc, 
													 const _CSimpleVertexContainer& lSucc, 
													 short Mult1, short Mult0, short MultTop,
													 bool Negated)
{
	//Negated ????
	if(hSucc.Is0())
		return lSucc;
	_CSimpleVertexContainer nhSucc=hSucc,nlSucc=lSucc;
	short Mult=1;
	long long Test=lSucc.GetLVertex();
	if((nlSucc.Is0() || nhSucc.GetLocalMultiplier()==nlSucc.GetLocalMultiplier())&& nhSucc.GetLocalMultiplier()!=1)
	{
		Mult=nhSucc.GetLocalMultiplier();
		nhSucc.ReduceMultiplier();
		nlSucc.ReduceMultiplier();
	}
	_CVertex* pNewVertex=GetSimpleVertex(pParameter,nhSucc,nlSucc,Negated);
//	Mult=pNewVertex->GlobalMultiplier();
	return _CSimpleVertexContainer(GlobalIdFromLocalVertex(pNewVertex),Mult);
}

void _CCircuit::TouchVertex(long long iVertex)
{
	if(iVertex<1)
		return;
	_CVertex* pVertex=GlobalVertexFromId(iVertex);
	if(pVertex!=NULL)
		pVertex->TouchMe();
}

void _CCircuit::UnTouchVertex(long long iVertex)
{
	if(iVertex<1)
		return;
	_CVertex* pVertex=GlobalVertexFromId(iVertex);
	if(pVertex!=NULL)
		pVertex->UnTouchMe();
}

//void _CCircuit::GetSimpleTree(_CSExpandedVertices& TreeVertices, unsigned CofId, _CSExpandedDescendends& ExternalVertices)
//{
//	_CCofactor* pCof=m_Cofactors[CofId];
//	_CModelVertex* pMVertex=pCof->GetMainVertex();//poprawiæ
//	if(pMVertex==NULL)
//		return;
//	//pTreeVertices=&pCof->GetMainSVertices();
//	pMVertex->GetSimpleVertex(TreeVertices,CofId,ExternalVertices);
//}
void _CCircuit::WriteDescriptionToStream(iostream& stream)
{
	WriteHeader(stream);
	stream<<"Components:\n";
	for each(_CComponent* pComp in m_IdealComponentsList)
	{
		pComp->WirteToTextStream(stream);
	}
	for each(_CComponent* pComp in m_LocalComponentsList)
	{
		pComp->WirteToTextStream(stream);
	}
}

void _CCircuit::WriteCircuitModelToStream(iostream& stream, bool WithCofactros)
{
	WriteDescriptionToStream(stream);
	if (WithCofactros)
		for each (_CCofactor* pCof in m_Cofactors)
		{
			pCof->WriteToTextStreamModel(stream);
		}
	long long Max=m_ModelVerticesCache.MaxId();
	size_t GlobalNoOfAdds = 0;
	size_t GlobalNoOfMults = 0;
	size_t GlobalNoOfMultsNonOne = 0;
	for(long long l=2;l<Max;l++)
	{
		_CModelVertex* pMVertex=GetModelVertexFromIndex(l);
		if(pMVertex==NULL || pMVertex->IsLeaf())
			continue;
		size_t LocalNoOfAdds = 0;
		size_t LocalNoOfMults = 0;
		size_t LocalNoOfMultsNonOne = 0;
		pMVertex->GetNumberOfOperations(LocalNoOfMults, LocalNoOfAdds, LocalNoOfMultsNonOne);
		GlobalNoOfMults += LocalNoOfMults;
		GlobalNoOfAdds += LocalNoOfAdds;
		GlobalNoOfMultsNonOne += LocalNoOfMultsNonOne;
		stream<<""<<l<<": ";
		pMVertex->WriteToTextStream(stream);

	}
	WriteOutParamsToTextStream(stream);
	stream << "No of multiplication is " << GlobalNoOfMults << "("<<GlobalNoOfMultsNonOne<<") and no of additions is " << GlobalNoOfAdds << endl;
}

void _CCircuit::WriteSimpleVertices(long CircuitId,iostream& stream, _CSimpleVerticesMapsSet& Set)
//void _CCircuit::WriteSimpleVertices(long CircuitId,iostream& stream)
{
	long long Max=m_SimpleLocalVerticesCache.MaxId();
	for(long long i=2;i<Max;i++)
	{
		_CVertex* pVertex=NULL;
		try
		{
			pVertex=m_SimpleLocalVerticesCache[i];
		}
		catch(out_of_range)
		{
			continue;
		}
		stream<<""<<CircuitId<<"."<<i<<": ";
		pVertex->WriteToTextStream(stream,Set);
		//pVertex->WriteToTextStream(stream);
	}
}

unsigned _CCircuit::GetSVerticesIndexForCofactor(_CCofactor* pCofactor)
{
	unsigned i=0,_size=m_Cofactors.size();
	for(;i<_size;i++)
		if(m_Cofactors[i]==pCofactor)
			break;
	if(i>=_size)
		RISEPDD(eIllegalOperation,"_CCircuit::GetSVerticesForCofactor(_CCofactor* pCofactor) - pCofactor is illegal reference");
	return i;
}

_CSExpandedVertices& _CCircuit::GetMainSVertices(unsigned IdNo,bool ForceIfNotExist) 
{
	return m_CSSimpleMainVertices.GetSExpandedVertices(EmptyString,
		ForceIfNotExist?_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST:_CContextSExpandedCofactorValues::MUST_EXIST,
		IdNo);
	//unsigned _size=m_SimpleMainVertices.size();
	//if(IdNo>=_size)
	//	if(ForceIfNotExist)
	//		m_SimpleMainVertices.resize(IdNo+1);
	//	else
	//		RISEPDD(out_of_range,"_CCircuit::GetMainSVertices: to large IdNo");
	//return m_SimpleMainVertices[IdNo];
}

_CSExpandedVertices& _CCircuit::GetNewSVertices()
{
	return m_CSSimpleMainVertices.GetNewSVertices(EmptyString);
	//unsigned _size=m_SimpleMainVertices.size();
	//m_SimpleMainVertices.resize(_size+1);
	//return m_SimpleMainVertices[_size];
}
size_t _CCircuit::PushLocalSVertices(const _CSExpandedVertices& Vertices,const string& Context)
{
	return m_CSSimpleMainVertices.PushUniqueSExpandedVertices(Context,Vertices);
}//poprawiamy 

void _CCircuit::Skip0SuppresesVertices(/*bool CutLeaves*/)
{
	for each(_CCofactor* pCof in m_Cofactors)
		pCof->Skip0Suppressed(/*CutLeaves*/);
}

//void _CCircuit::SkipDuplicates()
//{
//	_CDuplicatedVertexCatcher Catcher;
//	for each (_CCofactor* pCof in m_Cofactors)
//	{
//		pCof->SkipDuplicated(Catcher);
//	}
//}

void _CCircuit::Skip0SupressedAndDuplicated(MVERTEX_2_SIGN* pMVertex2Sgn,bool IgnoreSgnPropagation)
{
	if(m_UseNewAnalyser)
		return;
	_CDuplicatedVertexCatcher Catcher(*this,pMVertex2Sgn,IgnoreSgnPropagation);
	for each (_CCofactor* pCof in m_Cofactors)
		pCof->Skip0SupressedAndDuplicated(Catcher);
}


_CComponent* _CCircuit::Name2Component(const string& Name) const
{
	class _CFind
	{
	public: 
		_CFind(const string& Name):m_name(Name) {}
		bool operator()(_CComponent* pComp)
		{
			return pComp->GetParameterName()==m_name;
		}
	protected:
		const string& m_name;
	};
	_CComponents::const_iterator it=find_if(m_LocalComponentsList.begin(),m_LocalComponentsList.end(),
		_CFind(Name));
	return it==m_LocalComponentsList.end()?NULL:*it;
}

const string& _CCircuit::Component2Name(const _CComponent* pComponent) const
{
	_CComponents::const_iterator it=find(m_LocalComponentsList.begin(),m_LocalComponentsList.end(),
		pComponent);
	return it==m_LocalComponentsList.end()?EmptyString:(*it)->GetParameterName();
}

_CComponent* _CCircuit::FindComponent(const string& Name)
{
	class Finder
	{
	public:
		Finder(const string& Name):m_str(Name) {}
		bool operator()(_CComponent* pComponent) {return pComponent->GetParameterName()==m_str;}
	protected:
		const string& m_str;
	};
	_CComponents::iterator it=find_if(m_LocalComponentsList.begin(),m_LocalComponentsList.end(),Finder(Name));
	return it==m_LocalComponentsList.end()?NULL:*it;
}

void _CCircuit::EnumCofoctorForContexts(_CCofactor* pCofactor, iostream& stream,_CSimpleVerticesMapsSet& Set)
//void _CCircuit::EnumCofoctorForContexts(_CCofactor* pCofactor, iostream& stream)
{
	size_t CofId=GetSVerticesIndexForCofactor(pCofactor);
	m_CSSimpleMainVertices.WriteToStream(CofId,stream,Set);
	//m_CSSimpleMainVertices.WriteToStream(CofId,stream);
}

void _CCircuit::ClearPostSimpleVerticesDetermination()
{
	for each(_CComponent* pComp in m_LocalComponentsList)
		pComp->CleanGarbage();
}

bool _CCircuit::TestNameUniqness(const string& Name)
{
	UQSet::iterator it=m_UniqeNamesSet.find(Name);
	if(it!=m_UniqeNamesSet.end())
		return false;
	//{
	//	string Msg=string("You've just tried to redefine component ")+Name;
	//	throw eNameDuplication(Msg.c_str());
	//}
	m_UniqeNamesSet.insert(Name);
	return true;
}

long long _CCircuit::ModelVertex2Index(_CModelVertex* pMVertex)
{
	try
	{
		return m_ModelVerticesCache.GetIndexOf(pMVertex,false);
	}
	catch(out_of_range)
	{
		return -1;
	}
}

const _CModelVertex* _CCircuit::Index2ModelVertex(long long Index)
{
	try
	{
		return m_ModelVerticesCache.GetAt(Index);
	}
	catch(out_of_range)
	{
		return NULL;
	}
}

void _CCircuit::CacheTheResult(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket)
{
	ASSERTPDD(pMasterSocket!=NULL);
	pMasterSocket->CacheTheResult(pCacheContainer);
}

void _CCircuit::RegisterResult(_CDirectNumContainer* pCacheContainer)
{
	return m_DirNumCache.RegisterResult(pCacheContainer);
}

//void _CCircuit::RegisterResult(_CDirectNumContainer* pCacheContainer, _CSubCircuitSocket* pMasterSocket)
//{
//	ASSERTPDD(pMasterSocket!=NULL);
//	return pMasterSocket->RegisterResult(pCacheContainer);
//}

_CDirectNumContainer* _CCircuit::FindExisting(_CDirectNumContainer* pCacheContainer)
{
	return dynamic_cast<_CDirectNumContainer*> (m_DirNumCache.FindExisting(pCacheContainer));
}

//_CDirectNumContainer* _CCircuit::FindExisting(_CDirectNumContainer* pCacheContainer, _CSubCircuitSocket* pMasterSocket)
//{
//	ASSERTPDD(pMasterSocket != NULL);
//	return pMasterSocket->GetOriginal()->FindExisting(pCacheContainer);
//}

void _CCircuit::PreparDivider(_CComponent* pToComponent)
{
}

void _CCircuit::PreparReminder(_CComponent* pToComponent)
{
}


void _CCircuit::FindDuplicateLeaf(_CModelVertex* pOriginalVertex, _CModelVertex*& pNonDuplicateVertex,
		short& MovedSign, MVERTEX_2_SIGN* MVertex2Sgn)
{
	RISEPDD(eWrongCondition,"FindDuplicateLeaf - only for sub-circuit allowed");
}
