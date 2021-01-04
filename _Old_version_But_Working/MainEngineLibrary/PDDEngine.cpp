#include "StdAfx.h"
#include "PDDEngine.h"
#include "..\PDDErrorsAndConst.h"
#include <algorithm>
#include "current_dir.h"

_CPDDEngine::_CPDDEngine(bool sExpanded, bool ComponentOptimization,bool UseNewAnalyser):
m_sExpanded(sExpanded),
m_ComponentOptimization(ComponentOptimization),
m_UseNewAnalyser(UseNewAnalyser)
{
}

_CPDDEngine::~_CPDDEngine(void)
{
	for(TReCircuitIt it=m_Circuits.rbegin();it!=m_Circuits.rend();it++)
		if((*it)!=NULL)
			delete (*it);
	for(ANALYSER_VECTOR::reverse_iterator it=m_NetAnalysers.rbegin();it!=m_NetAnalysers.rend();it++)
		if((*it)!=NULL)
			delete (*it);
}

_CUnlineraBlackBoxEngine& _CPDDEngine::BlackBoxEngine()
{
	if (!m_pBlackBoxEngine)
		m_pBlackBoxEngine = make_unique<_CUnlineraBlackBoxEngine>(this);
	return *m_pBlackBoxEngine;
}

long _CPDDEngine::CreateNewCircuit()
{
	try
	{
		_CMainCircuit* pMainCircuit=new _CMainCircuit(this,m_ComponentOptimization,m_UseNewAnalyser);
		TCircuitIt it=find(m_Circuits.begin(),m_Circuits.end(),(_CMainCircuit*)NULL);
		if(it==m_Circuits.end())
		{
			m_Circuits.push_back(pMainCircuit);
			return m_Circuits.size();
		}
		(*it)=pMainCircuit;
		return it-m_Circuits.begin()+1;
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CPDDEngine::CreateNewCircuit";
		return ALLOCATION_ERROR;
	}
	return 0;
}

_CMainCircuit* _CPDDEngine::GetCircuit(long long Id)
{
	if(Id<0 || Id>(long long)m_Circuits.size())
		return NULL;
	return m_Circuits[(size_t)Id-1];
}

long _CPDDEngine::CreateNewNetAnalyser(const string& NetName,bool BySubcircuit,string& CommonNodeName)
{
	try
	{
		_CNetDynamicAnalyser* pNetDynamic=new _CNetDynamicAnalyser(this,NetName,BySubcircuit,CommonNodeName);
		ANALYSER_VECTOR::iterator it=find(m_NetAnalysers.begin(),m_NetAnalysers.end(),(_CNetDynamicAnalyser*)NULL);
		if(it==m_NetAnalysers.end())
		{
			m_NetAnalysers.push_back(pNetDynamic);
			return m_NetAnalysers.size();
		}
		(*it)=pNetDynamic;
		return it-m_NetAnalysers.begin()+1;
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CPDDEngine::CreateNewNetAnalyser";
		return ALLOCATION_ERROR;
	}
	return 0;
}

_CNetDynamicAnalyser* _CPDDEngine::GetNetAnalyser(long Id)
{
	if(Id<0 || Id>(long)m_NetAnalysers.size())
		return NULL;
	return m_NetAnalysers[Id-1];
}

long _CPDDEngine::RemoveCircuit(long Id)
{
	if(Id>=(long)m_Circuits.size())
	{
		LastErrorInfo=invalid_circuit_msg+"_CPDDEngine::RemoveCircuit"; 
		return INVALID_CIRCUIT_HANDLER;
	}
	_CMainCircuit*& pCircuit=m_Circuits[Id-1];
	if(pCircuit==NULL)
	{
		LastErrorInfo=invalid_circuit_msg+"_CPDDEngine::RemoveCircuit"; 
		return INVALID_CIRCUIT_HANDLER;
	}
	delete pCircuit;
	pCircuit=NULL;
	return 0;
}
const path_str& _CPDDEngine::GetModelsPath() 
{
	if(m_ModelPath.empty())
		m_ModelPath=current_dir();
	return m_ModelPath;
}