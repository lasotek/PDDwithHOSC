#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <tuple>
#include "binary_filer.h"

using namespace std;
using namespace stdext;

/* Reseved names:	"GND", "0" for reference node
**					"LGND". "0L" for local reference node*/


class _CStrDels 
{
public:
	_CStrDels(short Sgn = 1) : m_GlobalSgn(Sgn) {}
	void ReSetGlobalSgn(short Sgn = 1) { m_GlobalSgn = Sgn; }
protected:
	typedef tuple<string /*p*/, string /*r*/, short /*RowNeg*/, string /*k*/, string /*l*/, short /*ColNeg*/> INP_DEL;
	typedef list<INP_DEL> LIST;
	LIST m_List;
	short m_GlobalSgn;
	friend class  _CNodesManager;
};

class _CStrNodes : public vector<string>
{
public:
	_CStrNodes(const vector<string>& Source) :vector<string>(Source) {}
	~_CStrNodes()
	{
	}
	_CStrNodes() {}
	_CStrNodes(int NoOfStr, const char* Strs[])
	{
		for(int i=0;i<NoOfStr;i++)
			push_back(Strs[i]);
	}
};

typedef _CStrNodes		_CStrConnections;

class _CIntNodes : public vector<int>
{
public:
	~_CIntNodes() 
	{
	}
	_CIntNodes():m_CurrPos(0) {}
	int GetNextDongle();
	bool ContainDongles();
	void GetDongles(_CIntNodes& DonglesIntNos);
	int get_sgn(int Index);
	int& force_at(size_t Index);
	void SeparateDongles(SIGNED_PINS_COLOCATIONS& PinsCollocation,COMPONENT_PINS_LIST& Dongles);
protected:
	unsigned m_CurrPos;
};

class _CNodesConnections : public vector<pair<string,string>>
{
public:
	~_CNodesConnections()
	{
	}
	string& GetExternalBoundaryNodeName(unsigned Index) {return at(Index).first;}
	string& GetModelBoundaryNodeName(unsigned Index) {return at(Index).second;}
};

class _CStrConnection2NodesConection
{
public:
	~_CStrConnection2NodesConection()
	{
	}
	_CStrConnection2NodesConection(const _CStrConnections& StrConnections);
	operator const _CNodesConnections&() {return m_Connections;}
protected:
	void SwapAndRegister(const string& TextToParse);
	void RemoveWhiteSpace(string& Text);
	_CNodesConnections m_Connections;
	typedef _CNodesConnections::value_type value_type;
};

class  _CNodesManager : public _streamable
{
public:
	_CNodesManager() {}
	_CNodesManager(const string GlobalRefNode);
	~_CNodesManager(void) 
	{
	}
	vector<int> ConvertOrAddNodes(const vector<string>& NodesName);
	int ConvertOrAddNode(const string& NodeName);
	int ConvertNode(const string& NodeName);
	//const string& ConvertNode(int NodeNo);
	string ConvertNode(int NodeNo);
	inline int GetCount() const {return m_CurrentNoNode+1; }
	inline int GetHighestNo() const {return m_CurrentNoNode; }
	_CIntNodes ConvertNodes(const _CStrNodes& Nodes);
	_CStrNodes ConvertNodes(const _CIntNodes& Nodes);
	_CIntNodes ConvertOrAddNodes(const _CStrNodes& Nodes);
	void ExchangeNumbers(const _CExchangMapWraper& Numbers2Exchange);
//	bool IsGlobalZero() {return m_IsGlobalZero; }
protected:
	DECLARE_DYNAMIC_CREATION(_CNodesManager);
	void Store(_binary_filer& Filer);
	void Load(_binary_filer& Filer);
	typedef unordered_map<string,int> TNodesSet;
	typedef TNodesSet::iterator TNodesIterator;
	typedef TNodesSet::value_type TValueType;
		class CIndexFinder
		{
		public:
			CIndexFinder(unsigned Index):m_Index(Index) {}
			bool operator()(const TValueType& Obj) {return Obj.second==m_Index;}
		private:
			unsigned m_Index;
		};
	TNodesSet m_Nodes;
	int m_CurrentNoNode{};
//	bool m_IsLocal;
//	bool m_IsGlobalZero;
};

