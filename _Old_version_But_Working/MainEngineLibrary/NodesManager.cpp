#include "StdAfx.h"
#include "NodesManager.h"
#include "PrjException.h"
#include <string.h>
#include <algorithm>

int _CIntNodes::GetNextDongle()
{
	//for(unsigned i=m_CurrPos;i<size();i++)
	//{
	//	m_CurrPos=i;
	//	if(at(i)==-1)
	//	{
	//		m_CurrPos++;
	//		return i;
	//	}
	//}
	for(;m_CurrPos<size();m_CurrPos++)
	{
		if(at(m_CurrPos)==-1)
			return m_CurrPos++;
	}
	m_CurrPos=0;
	return -1;
}

int _CIntNodes::get_sgn(int Index)
{
	int Sgn=Index<0?-1:1;
	int Value=at(abs(Index));
	return Sgn*Value;
}
bool _CIntNodes::ContainDongles()
{
	m_CurrPos=0;
	int FD=GetNextDongle();
	m_CurrPos=0;
	return FD==-1;
}

void _CIntNodes::GetDongles(_CIntNodes& DonglesIntNos)
{
	DonglesIntNos.clear();
	int Dongle=GetNextDongle();
	while(Dongle>-1)
	{
		DonglesIntNos.push_back(Dongle);
		Dongle=GetNextDongle();
	}
}

void _CIntNodes::SeparateDongles(SIGNED_PINS_COLOCATIONS& PinsCollocation,COMPONENT_PINS_LIST& Dongles)
{
	PinsCollocation.clear();
	Dongles.clear();
	for(size_t i=0;i<size();i++)
	{
		int Value=at(i);
		if(Value<0)
			Dongles.push_back(i);
		else
			PinsCollocation.insert(SIGNED_PINS_COLOCATIONS::value_type(i,Value));
	}
}


int& _CIntNodes::force_at(size_t Index)
{
	if(Index>=size())
		resize(Index+1,-1);
	return at(Index);
}

_CStrConnection2NodesConection::_CStrConnection2NodesConection(const _CStrConnections& StrConnections)
{
	for(_CStrConnections::const_iterator it=StrConnections.begin();it!=StrConnections.end();it++)
		SwapAndRegister(*it);
}

void _CStrConnection2NodesConection::RemoveWhiteSpace(string& Text)
{
	string::iterator it=Text.begin();
	while(it!=Text.end() && *it==' ')
		it=Text.erase(it);
	while(it!=Text.end() && *it!=' ') it++;
	while(it!=Text.end() && *it==' ')
		it=Text.erase(it);
	if(it!=Text.end())
		throw eWrongFormatConnection();
}

void _CStrConnection2NodesConection::SwapAndRegister(const string& TextToParse)
{
	string::size_type EQPos=TextToParse.find('=');
	if(EQPos==string::npos)
		throw eWrongFormatConnection();
	//komunikat o b³êdzie
	string ExternalName=TextToParse.substr(0,EQPos++);
	string InternalName=TextToParse.substr(EQPos,TextToParse.length()-EQPos);
	RemoveWhiteSpace(ExternalName);
	RemoveWhiteSpace(InternalName);
	m_Connections.push_back(value_type(ExternalName,InternalName));
}

_CNodesManager::_CNodesManager(const string GlobalRefNode):m_CurrentNoNode(-1)
{
	if(!GlobalRefNode.empty())
		ConvertOrAddNode(GlobalRefNode);
}

vector<int> _CNodesManager::ConvertOrAddNodes(const vector<string>& NodesName)
{
	vector<int> res;
	res.reserve(NodesName.size());
	for (auto n_str : NodesName)
		res.push_back(ConvertOrAddNode(n_str));
	return res;
}

int _CNodesManager::ConvertOrAddNode(const string& NodeName)
{
	int sgn = 1;
	string localNodeName;
	if (NodeName.at(0) == '-')
	{
		sgn = -1;
		localNodeName = NodeName.substr(1);

	}
	else
		localNodeName = NodeName;
	int Node = ConvertNode(localNodeName);
	if (Node < 0)
	{
		m_Nodes.insert(TValueType(NodeName, ++m_CurrentNoNode));
		return m_CurrentNoNode;
	}
	else
		return Node*sgn;
}	

int _CNodesManager::ConvertNode(const string& NodeName)
{
	if(NodeName.empty())
		return -1;
	TNodesIterator it=m_Nodes.find(NodeName);
	if(it!=m_Nodes.end())
		return it->second;
	else
		return -1;
}	

string _CNodesManager::ConvertNode(int NodeNo)
{
	short Sgn=1;
	if(NodeNo<0)
	{
		Sgn=-1;
		NodeNo=-NodeNo;
	}
	TNodesIterator it=find_if(m_Nodes.begin(),m_Nodes.end(),CIndexFinder(NodeNo));
	if(it==m_Nodes.end())
		return EmptyString;
	else
		return Sgn==-1?string("-")+it->first:it->first;
}

_CIntNodes _CNodesManager::ConvertNodes(const _CStrNodes& Nodes)
{
	_CIntNodes OutNodes;
	for(_CStrNodes::const_iterator it=Nodes.begin();it!=Nodes.end();it++)
		OutNodes.push_back(ConvertNode(*it));
	return OutNodes;
}

_CIntNodes _CNodesManager::ConvertOrAddNodes(const _CStrNodes& Nodes)
{
	_CIntNodes OutNodes;
	for(_CStrNodes::const_iterator it=Nodes.begin();it!=Nodes.end();it++)
		OutNodes.push_back(ConvertOrAddNode(*it));
	return OutNodes;
}

void _CNodesManager::ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange)
{
	for(TNodesSet::iterator it=m_Nodes.begin(),_e=m_Nodes.end();it!=_e;it++)
		Numbers2Exchange.ExchangeIfNeede(it->second);
}

_CStrNodes _CNodesManager::ConvertNodes(const _CIntNodes& Nodes)
{
	_CStrNodes OutNodes;
	for(_CIntNodes::const_iterator it=Nodes.begin();it!=Nodes.end();it++)
		OutNodes.push_back(ConvertNode(*it));
	return OutNodes;
}

void _CNodesManager::Store(_binary_filer& Filer)
{
	Filer<<m_CurrentNoNode;
	Filer<<m_Nodes.size();
	for(TNodesIterator it=m_Nodes.begin();it!=m_Nodes.end();it++)
	{
		Filer<<it->first;
		Filer<<it->second;
	}
	_streamable::Store(Filer);
}

void _CNodesManager::Load(_binary_filer& Filer)
{
	m_Nodes.clear();
	Filer>>m_CurrentNoNode;
	TNodesSet::size_type size;
	Filer>>size;
	for(TNodesSet::size_type i=0;i<size;i++)
	{
		TNodesSet::key_type Name;
		TNodesSet::mapped_type Index;
		Filer>>Name;
		Filer>>Index;
		m_Nodes.insert(TValueType(Name,Index));
	}
	_streamable::Load(Filer);
}
IMPLEMENT_DYNAMIC_CREATION(_CNodesManager);