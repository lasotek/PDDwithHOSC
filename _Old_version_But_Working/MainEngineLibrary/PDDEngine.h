#pragma once
#include "MainCircuit.h"
#include "NetDynamicAnalyser.h"
#include "UnlineraBlackBox.h"
#include <vector>
#include <string>
using namespace std;

//#ifdef _UNICODE
//#define	 path_str wstring
//#else
//#define	path_str string
//#endif

class  _CPDDEngine
{
public:
	_CPDDEngine(bool sExpanded, bool ComponentOptimization,bool UseNewAnalyser);
	~_CPDDEngine(void);
	long CreateNewCircuit();
	_CMainCircuit* GetCircuit(long long Id);
	long CreateNewNetAnalyser(const string& NetName,bool BySubcircuit,string& CommonNodeName=EmptyString);
	_CNetDynamicAnalyser* GetNetAnalyser(long Id);
	long RemoveCircuit(long Id);
	void SetPath2Models(const path_str& ModelPath) {m_ModelPath=ModelPath;}
	const path_str& GetModelsPath();
	bool IsSExpanded() {return m_sExpanded; }
	_CUnlineraBlackBoxEngine& BlackBoxEngine();
protected:
	path_str m_ModelPath;
	typedef vector<_CMainCircuit*> TCircuitsVector;
	typedef TCircuitsVector::iterator TCircuitIt;
	typedef TCircuitsVector::reverse_iterator TReCircuitIt;
	TCircuitsVector m_Circuits;
	bool m_sExpanded;
	bool m_ComponentOptimization;
	bool m_UseNewAnalyser;
	typedef vector<_CNetDynamicAnalyser*> ANALYSER_VECTOR;
	ANALYSER_VECTOR m_NetAnalysers;
	unique_ptr<_CUnlineraBlackBoxEngine> m_pBlackBoxEngine;
};
