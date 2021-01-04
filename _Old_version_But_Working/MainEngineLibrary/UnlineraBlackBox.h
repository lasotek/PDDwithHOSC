#pragma once
#include "norm_float.h"
#include "MainCircuit.h"
#include "SubCircuit.h"
#include "Circuit.h"
#include "TwoPorts.h"
#include "ControledSources.h"
#include <array>

typedef array<string, 3> STR_GATE;/* GateName, N_HIGH, N_LOW*/
typedef vector<STR_GATE> STR_GATES;

class _CUnlineraBlackBoxBase
{
public:
	//typedef const tuple<string, int, int> GATE;
	//typedef const vector<GATE> GATES;
	_CUnlineraBlackBoxBase(string Name, STR_GATES VoltageInputs, STR_GATES CurrentInputs, STR_GATES VoltageOutputs, STR_GATES CurrentOutputs) :
		m_Name(Name), m_VI(VoltageInputs), m_II(CurrentInputs), m_VO(VoltageOutputs), m_IO(CurrentOutputs) {}
	const STR_GATES& VI() { return m_VI; }
	const STR_GATES& II() { return m_II; }
	const STR_GATES& VO() { return m_VO; }
	const STR_GATES& IO() { return m_IO; }
	const string& Name() { return m_Name; }
protected:
	STR_GATES m_VI;
	STR_GATES m_II;
	STR_GATES m_VO;
	STR_GATES m_IO;
	string m_Name;
};

class _CUnlineraBlackBox : public _CUnlineraBlackBoxBase
{
public:
	_CUnlineraBlackBox(_CCircuit* pMainCircuit, string Name, STR_GATES VoltageInputs, STR_GATES CurrentInputs, STR_GATES VoltageOutputs, STR_GATES CurrentOutputs) :
		m_pCircuit(pMainCircuit), _CUnlineraBlackBoxBase(Name, VoltageInputs, CurrentInputs, VoltageOutputs, CurrentOutputs) {}
	virtual void PreparePseudoComponents();
	_CCircuit* getCirc() {return m_pCircuit;}
protected:
	_CCircuit* m_pCircuit;
};

typedef map<string, string> CONNECTION_MAP;

class _CUnlinearBlackBoxWithModel : public _CUnlineraBlackBox
{
public:
	_CUnlinearBlackBoxWithModel(_CCircuit* pMainCircuit, const string& InstanceName, _CUnlineraBlackBox& Source, CONNECTION_MAP ConnectionMap);
	virtual void PreparePseudoComponents() override {}
protected:
	void ExchangeNode(string& Node, CONNECTION_MAP ConnectionMap);
	_CUnlineraBlackBox* m_pModel;
};


class _CUnlineraBlackBoxEngine
{
public:
	_CUnlineraBlackBoxEngine(_CPDDEngine* pEngine);
	~_CUnlineraBlackBoxEngine();
	const _CUnlineraBlackBox& AddBlackBox(const string& Name, STR_GATES VoltageInputs, STR_GATES CurrentInputs, STR_GATES VoltageOutputs, STR_GATES CurrentOutputs);
	long  DefineBlackBoxModel(const string& Name, STR_GATES VoltageInputs, STR_GATES CurrentInputs, STR_GATES VoltageOutputs, STR_GATES CurrentOutputs);
	void AddBlackBoxModelInsatance(const string& InsatnceNameName, long ModelId, vector<string> Connections);
	//void PreparePseudoComponents();
	void PrepareCofacotrs(long Id = -1);
	void SetGlobalRefNode(const string& Node) 
	{
		m_pMainCircuit->DeclareGlobalRefNode(Node);
	}
	void SetGlobalModelFileName(const string& Name, bool ClearOld=false)
	{
		m_pMainCircuit->SetGlobalModelFileName(Name, ClearOld);
	}
	void SetGlobalRaportFileName(const string& Name, bool ClearOld=false)
	{
		m_pMainCircuit->SetGlobalRaportFileName(Name, ClearOld);
	}
	_CMainCircuit* GetAssociatedCircuit() const { return m_pMainCircuit; }
	void AddAutonomicVoltage(const STR_GATE& Gate) { m_VoltageSources.push_back(Gate); }
	void AddAutonomicCurrent(const STR_GATE& Gate) { m_CurentSources.push_back(Gate); }
	long long GetHCircuit() const { return m_MainCircuitId; }
	void DoTheAnalysis() { m_pMainCircuit->DoTheCalculations(); }
	void WriteModelsToTextFile(const string& FileName);
	void DetAndWriteNumericalResults(const string& FileName);
	void WriteSomeSimplifications(bool TryToFactorize, bool Write1Level, const string& FileName, bool Append);
	void SetCompPriority(size_t Priority)
	{
		m_pMainCircuit->SetCompPriority(Priority);
	}
	inline void ClearCompPriority()
	{
		SetCompPriority(numeric_limits<size_t>::max());
	}
	void SetCompPrioritySub(long SubId, size_t Priority);
	inline void ClearCompPrioritySub(long SubId) 
	{
		SetCompPrioritySub(SubId, numeric_limits<size_t>::max());
	}
protected:
	void PrepareComDen();
	void PrepareTheRestCofactors(long Id = -1);
	void PrepareGatesList();
	inline int TranslateOrAddNode(const string& Node) { return m_pMainCircuit->TranslateOrAddNode(Node); }
	_CPDDEngine* m_pEngine;
	long long m_MainCircuitId;
	_CMainCircuit* m_pMainCircuit;
	typedef list<_CUnlineraBlackBox> BLACK_BOXES;
	BLACK_BOXES m_BlackBoxes;
	typedef list<_CUnlinearBlackBoxWithModel> BLACK_BOXES_WITH_MODEL;
	BLACK_BOXES_WITH_MODEL m_BlackBoxesWithModel;
	typedef map<long, _CUnlineraBlackBox> BLACK_BOX_MODELS;
	BLACK_BOX_MODELS m_BlackBoxesModels;
	typedef list<STR_GATE> AUTONOMIC_GATES;
	AUTONOMIC_GATES m_VoltageSources, m_CurentSources;
	_CCofactor* m_pCommonDen;
	typedef map<unsigned long, _CCofactor*> NUM_INDEX_TO_COF;
	typedef map<string, unsigned long> NUM_ID_TO_INDEX;
	typedef map<unsigned long, string> NUM_INDEX_TO_ID;
	//unsigned long m_GlobalIdx;
	NUM_INDEX_TO_COF m_Idx2Cof;
	NUM_ID_TO_INDEX m_Id2Idx;
	NUM_INDEX_TO_ID m_Idx2Id;
	typedef tuple<string, int, int> INT_GATE;
	typedef list<INT_GATE> INT_GATES_LIST;
	INT_GATES_LIST m_VoltageInputs, m_CurrentInputs, m_VoltageOutputs, m_CurrentOutputs;
};

