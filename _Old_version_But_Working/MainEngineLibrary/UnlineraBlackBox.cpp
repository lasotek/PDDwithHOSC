#include "stdafx.h"
#include "UnlineraBlackBox.h"
#include "PrjException.h"
#include "PDDEngine.h"
#include "time_stamp.h"
#include "PrjException.h"


_CUnlinearBlackBoxWithModel::_CUnlinearBlackBoxWithModel(_CCircuit* pMainCircuit, const string& InstanceName, _CUnlineraBlackBox& Source, CONNECTION_MAP ConnectionMap) :
_CUnlineraBlackBox(Source), m_pModel(&Source)
{
	m_pCircuit = pMainCircuit;
	m_Name = InstanceName;
	for (auto& p : m_II)
	{
		ExchangeNode(p[1], ConnectionMap);
		ExchangeNode(p[2], ConnectionMap);
	}
	for (auto& p : m_IO)
	{
		ExchangeNode(p[1], ConnectionMap);
		ExchangeNode(p[2], ConnectionMap);
	}
	for (auto& p : m_VI)
	{
		ExchangeNode(p[1], ConnectionMap);
		ExchangeNode(p[2], ConnectionMap);
	}
	for (auto& p : m_VO)
	{
		ExchangeNode(p[1], ConnectionMap);
		ExchangeNode(p[2], ConnectionMap);
	}
}


void _CUnlinearBlackBoxWithModel::ExchangeNode(string& Node, CONNECTION_MAP ConnectionMap)
{
	auto fit = ConnectionMap.find(Node);
	if (fit == ConnectionMap.end())
		RISEPDD(eIllegalModelName, "Uknown internal node");
	Node = fit->second;
}


_CUnlineraBlackBoxEngine::_CUnlineraBlackBoxEngine(_CPDDEngine* pEngine) :m_pEngine(pEngine), m_MainCircuitId(pEngine->CreateNewCircuit()), m_pCommonDen(nullptr)
{
	m_pMainCircuit = m_pEngine->GetCircuit(m_MainCircuitId);
	ASSERTPDD(m_pMainCircuit != nullptr);
}


_CUnlineraBlackBoxEngine::~_CUnlineraBlackBoxEngine()
{
}

const _CUnlineraBlackBox& _CUnlineraBlackBoxEngine::AddBlackBox(const string& Name, STR_GATES VoltageInputs, STR_GATES CurrentInputs, STR_GATES VoltageOutputs, STR_GATES CurrentOutputs)
{
	m_BlackBoxes.push_back(_CUnlineraBlackBox(m_pMainCircuit, Name, VoltageInputs, CurrentInputs, VoltageOutputs, CurrentOutputs));
	_CUnlineraBlackBox& BlackBox = m_BlackBoxes.back();
	BlackBox.PreparePseudoComponents();
	return BlackBox;
}
long _CUnlineraBlackBoxEngine::DefineBlackBoxModel(const string& Name, STR_GATES VoltageInputs, STR_GATES CurrentInputs, STR_GATES VoltageOutputs, STR_GATES CurrentOutputs)
{
	set<string> NodesSet;
	for (auto g : VoltageInputs)
	{
		NodesSet.insert(g[1]);
		NodesSet.insert(g[2]);
	}
	for (auto g : CurrentInputs)
	{
		NodesSet.insert(g[1]);
		NodesSet.insert(g[2]);
	}
	for (auto g : VoltageOutputs)
	{
		NodesSet.insert(g[1]);
		NodesSet.insert(g[2]);
	}
	for (auto g : CurrentOutputs)
	{
		NodesSet.insert(g[1]);
		NodesSet.insert(g[2]);
	}
	_CStrNodes Nodes;
	Nodes.assign(NodesSet.begin(), NodesSet.end());
	long SubCirId = m_pMainCircuit->DefineNewSubcircuit(Name, Nodes, nullptr, true);
	_CSubCircuit* pSubCircuit = m_pMainCircuit->GetSubcircuit(SubCirId);
	m_BlackBoxesModels.insert(BLACK_BOX_MODELS::value_type(SubCirId, _CUnlineraBlackBox(pSubCircuit, Name, VoltageInputs, CurrentInputs, VoltageOutputs, CurrentOutputs))).first->second.PreparePseudoComponents();
	return SubCirId;
}

void _CUnlineraBlackBoxEngine::AddBlackBoxModelInsatance(const string& InsatnceName, long ModelId, vector<string> Connections)
{
	_CNodesConnections NodesConnection((const _CNodesConnections& )_CStrConnection2NodesConection(_CStrNodes(Connections)));
	auto fit = m_BlackBoxesModels.find(ModelId);
	if (fit == m_BlackBoxesModels.end())
		RISEPDD(eIllegalModelName, "Model not defined!");
	_CUnlineraBlackBox& Box= fit->second;
	_CSubCircuit* pSubCir = dynamic_cast<_CSubCircuit*> (Box.getCirc());
	m_pMainCircuit->IncludeSubcircuit(InsatnceName, pSubCir, NodesConnection);
	//
	CONNECTION_MAP AuxMap;
	for (auto p : NodesConnection)
		AuxMap[p.first] = p.second;
	m_BlackBoxesWithModel.push_back(_CUnlinearBlackBoxWithModel(m_pMainCircuit, InsatnceName, Box, AuxMap));
}



void _CUnlineraBlackBox::PreparePseudoComponents()
{
	for (auto& GateVI : m_VI)
	{
		string s_k = GateVI[1], s_l = GateVI[2];
		if (s_k == s_l)
			continue;
		for (auto& GateVO : m_VO)
		{
			string s_p = GateVO[1], s_r = GateVO[2] + "A";
			m_pCircuit->AddVoltageGain("k_" + m_Name + "_(" + GateVI[0] + "->" + GateVO[0] + ")", s_p, s_r, s_k, s_l, 1.0, 0);
		}
		for (auto& GateIO : m_IO)
		{
			string s_r = GateIO[1], s_p = GateIO[2];
			if (s_p == s_r)
				continue;
			m_pCircuit->AddTransconductance("g_" + m_Name + "_(" + GateVI[0] + "->" + GateIO[0] + ")", s_p, s_r, s_k, s_l, 1.0, 0);
		}
	}
	for (auto& GateII : m_II)
	{
		string s_k = GateII[1], s_l = GateII[2];
		if (s_k == s_l)
			continue;
		for (auto& GateVO : m_VO)
		{
			string s_p = GateVO[1], s_r = GateVO[2] + "A";
			m_pCircuit->AddTransimpedance("r_" + m_Name + "_(" + GateII[0] + "->" + GateVO[0] + ")", s_p, s_r, s_k, s_l, 1.0, 0);
		}
		for (auto& GateIO : m_IO)
		{
			string s_r = GateIO[1], s_p = GateIO[2];
			if (s_p == s_r)
				continue;
			m_pCircuit->AddCurrentGain("k_" + m_Name + "_(" + GateII[0] + "->" + GateIO[0] + ")", s_p, s_r, s_k, s_l, 1.0, 0);
		}

	}
}

//void _CUnlineraBlackBoxEngine::PreparePseudoComponents()
//{
//	for (auto& Box : m_BlackBoxes)
//		Box.PreparePseudoComponents();
//}


void _CUnlineraBlackBoxEngine::PrepareGatesList()
{
	for (auto& VGate : m_VoltageSources)
		m_VoltageInputs.push_back(INT_GATE(VGate[0], TranslateOrAddNode(VGate[1]), TranslateOrAddNode(VGate[2])));
	for (auto& IGate : m_CurentSources)
		m_CurrentInputs.push_back(INT_GATE(IGate[0], TranslateOrAddNode(IGate[1]), TranslateOrAddNode(IGate[2])));
	for (auto& Box : m_BlackBoxes)
	{
		const string& Name = Box.Name();
		for (auto& VIGate : Box.VO())
			m_VoltageInputs.push_back(INT_GATE(Name + ".U_" + VIGate[0], TranslateOrAddNode(VIGate[2] + "A"), TranslateOrAddNode(VIGate[2])));
		for (auto& VOGate : Box.VI())
			m_VoltageOutputs.push_back(INT_GATE(Name + ".U_" + VOGate[0], TranslateOrAddNode(VOGate[1]), TranslateOrAddNode(VOGate[2])));
		for (auto& IIGate : Box.IO())
			m_CurrentInputs.push_back(INT_GATE(Name + ".I_" + IIGate[0], TranslateOrAddNode(IIGate[1]), TranslateOrAddNode(IIGate[2])));
		for (auto& IOGate : Box.II())
			m_CurrentOutputs.push_back(INT_GATE(Name + ".I_" + IOGate[0], TranslateOrAddNode(IOGate[1]), TranslateOrAddNode(IOGate[2])));
	}
	for (auto& Box : m_BlackBoxesWithModel)
	{
		const string& Name = Box.Name();
		for (auto& VIGate : Box.VO())
			m_VoltageInputs.push_back(INT_GATE(Name + ".U_" + VIGate[0], TranslateOrAddNode(VIGate[2] + "A"), TranslateOrAddNode(VIGate[2])));
		for (auto& VOGate : Box.VI())
			m_VoltageOutputs.push_back(INT_GATE(Name + ".U_" + VOGate[0], TranslateOrAddNode(VOGate[1]), TranslateOrAddNode(VOGate[2])));
		for (auto& IIGate : Box.IO())
			m_CurrentInputs.push_back(INT_GATE(Name + ".I_" + IIGate[0], TranslateOrAddNode(IIGate[1]), TranslateOrAddNode(IIGate[2])));
		for (auto& IOGate : Box.II())
			m_CurrentOutputs.push_back(INT_GATE(Name + ".I_" + IOGate[0], TranslateOrAddNode(IOGate[1]), TranslateOrAddNode(IOGate[2])));
	}
}

void _CUnlineraBlackBoxEngine::PrepareCofacotrs(long Id)
{
	PrepareGatesList();
	PrepareComDen();
	PrepareTheRestCofactors(Id);
}


void _CUnlineraBlackBoxEngine::PrepareComDen()
{
	unsigned long m_GlobalIdx = m_pMainCircuit->CreateNewCofactor();
	m_pCommonDen = m_pMainCircuit->GetCofactor(m_GlobalIdx);
	for (auto& Gate : m_VoltageInputs)
	{
		int a = std::get<1>(Gate), b = std::get<2>(Gate);
		if (a == b)
			RISEPDD(eWrongCondition, "Singular circuit");
		m_pCommonDen->AddDeletions(a, b, a, b, false, false, 0);
	}
	for (auto& Gate : m_CurrentOutputs)
	{
		int a = std::get<1>(Gate), b = std::get<2>(Gate);
		if (a == b)
			RISEPDD(eWrongCondition, "Singular circuit");
		m_pCommonDen->AddDeletions(a, b, a, b, false, false, 0);
	}
	string tName = "Com. Den.";
	m_Idx2Cof.insert(NUM_INDEX_TO_COF::value_type(m_GlobalIdx, m_pCommonDen));
	m_Id2Idx.insert(NUM_ID_TO_INDEX::value_type(tName, m_GlobalIdx));
	m_Idx2Id.insert(NUM_INDEX_TO_ID::value_type(m_GlobalIdx, tName));
}

void _CUnlineraBlackBoxEngine::PrepareTheRestCofactors(long Id)
{
	long Counter = 0;
	//for (auto& VIGate : m_VoltageInputs)
	//{
	//	auto& IName = std::get<0>(VIGate);
	//	auto a = std::get<1>(VIGate);
	//	auto d = std::get<2>(VIGate);
	//	if (a == d)
	//		continue;
	//	for (auto& VOGate : m_VoltageOutputs)
	//	{
	//		auto& OName = std::get<0>(VOGate);
	//		auto b = std::get<1>(VOGate);
	//		auto c = std::get<2>(VOGate);
	//		if (b == c)
	//			continue;
	//		if (Id > 0 && Id < ++Counter)
	//			break;
	//		//if (Id > 0 && Id != ++Counter)
	//		//	continue;
	//		unsigned long m_GlobalIdx = m_pMainCircuit->CreateNewCofactor();
	//		_CCofactor* pNewCof = m_pMainCircuit->GetCofactor(m_GlobalIdx);
	//		pNewCof->CopyAndForceSuperposition(*m_pCommonDen, _CDeletions::SUPERPOSE_SUBSTITUTE_INPUT | _CDeletions::SUPERPOSE_ADD_GATES, a, d, b, c);
	//		string tName = "NumKu_(" + IName + "->" + OName + ")";
	//		m_Idx2Cof.insert(NUM_INDEX_TO_COF::value_type(m_GlobalIdx, pNewCof));
	//		m_Id2Idx.insert(NUM_ID_TO_INDEX::value_type(tName, m_GlobalIdx));
	//		m_Idx2Id.insert(NUM_INDEX_TO_ID::value_type(m_GlobalIdx, tName));
	//	}
	//	for (auto& IOGate : m_CurrentOutputs)
	//	{
	//		auto& OName = std::get<0>(IOGate);
	//		auto b = std::get<1>(IOGate);
	//		auto c = std::get<2>(IOGate);
	//		if (b == c)
	//			continue;
	//		if (Id > 0 && Id < ++Counter)
	//			break;
	//		//if (Id > 0 && Id != ++Counter)
	//		//	continue;
	//		unsigned long m_GlobalIdx = m_pMainCircuit->CreateNewCofactor();
	//		_CCofactor* pNewCof = m_pMainCircuit->GetCofactor(m_GlobalIdx);
	//		pNewCof->CopyAndForceSuperposition(*m_pCommonDen, _CDeletions::SUPERPOSE_SUBSTITUTE_INPUT | _CDeletions::SUPERPOSE_SUBSTITUTE_OUTPUT | _CDeletions::SUPERPOSE_ADD_GATES, a, d, b, c);
	//		string tName = "NumN_(" + IName + "->" + OName + ")";
	//		m_Idx2Cof.insert(NUM_INDEX_TO_COF::value_type(m_GlobalIdx, pNewCof));
	//		m_Id2Idx.insert(NUM_ID_TO_INDEX::value_type(tName, m_GlobalIdx));
	//		m_Idx2Id.insert(NUM_INDEX_TO_ID::value_type(m_GlobalIdx, tName));
	//	}
	//}
	for (auto& OIGate : m_CurrentInputs)
	{
		auto& IName = std::get<0>(OIGate);
		auto a = std::get<1>(OIGate);
		auto d = std::get<2>(OIGate);
		if (a == d)
			continue; 
		if (Id > 0 && Id != ++Counter)
			continue;
		for (auto& VOGate : m_VoltageOutputs)
		{
			auto& OName = std::get<0>(VOGate);
			auto b = std::get<1>(VOGate);
			auto c = std::get<2>(VOGate);
			if (b == c)
				continue;
			//if (Id > 0 && Id < ++Counter)
			//	break;
			//if (Id > 0 && Id != ++Counter)
			//	continue;
			unsigned long m_GlobalIdx = m_pMainCircuit->CreateNewCofactor();
			_CCofactor* pNewCof = m_pMainCircuit->GetCofactor(m_GlobalIdx);
			pNewCof->CopyAndForceSuperposition(*m_pCommonDen, _CDeletions::SUPERPOSE_ADD_GATES, a, d, b, c);
			string tName = "NumM_(" + IName + "->" + OName + ")";
			m_Idx2Cof.insert(NUM_INDEX_TO_COF::value_type(m_GlobalIdx, pNewCof));
			m_Id2Idx.insert(NUM_ID_TO_INDEX::value_type(tName, m_GlobalIdx));
			m_Idx2Id.insert(NUM_INDEX_TO_ID::value_type(m_GlobalIdx, tName));
		}
		for (auto& IOGate : m_CurrentOutputs)
		{
			auto& OName = std::get<0>(IOGate);
			auto b = std::get<1>(IOGate);
			auto c = std::get<2>(IOGate);
			if (b == c)
				continue;
			//if (Id > 0 && Id < ++Counter)
			//	break;
			//if (Id > 0 && Id != ++Counter)
			//	continue;
			unsigned long m_GlobalIdx = m_pMainCircuit->CreateNewCofactor();
			_CCofactor* pNewCof = m_pMainCircuit->GetCofactor(m_GlobalIdx);
			pNewCof->CopyAndForceSuperposition(*m_pCommonDen, _CDeletions::SUPERPOSE_SUBSTITUTE_OUTPUT | _CDeletions::SUPERPOSE_ADD_GATES, a, d, b, c);
			string tName = "NumKi_(" + IName + "->" + OName + ")";
			m_Idx2Cof.insert(NUM_INDEX_TO_COF::value_type(m_GlobalIdx, pNewCof));
			m_Id2Idx.insert(NUM_ID_TO_INDEX::value_type(tName, m_GlobalIdx));
			m_Idx2Id.insert(NUM_INDEX_TO_ID::value_type(m_GlobalIdx, tName));
		}
	}
}

void _CUnlineraBlackBoxEngine::WriteModelsToTextFile(const string& FileName)
{
	const string& RaportFileName = m_pMainCircuit->GetRaportFileName();
	const string& ModelFileName = m_pMainCircuit->GetModelFileName();
	time_stamp stamp(RaportFileName.empty() ? "Raport.txt" : RaportFileName, "Multilevel result raporting");
	fstream file(ModelFileName.empty() ? FileName.c_str() : ModelFileName.c_str(), ios::out | (ModelFileName.empty() ? 0 : ios::app));
	for (auto& bbm : m_BlackBoxesModels)
	{
		file << "Subcircuit Id = " << bbm.first << endl;
		_CCircuit* pSubCir = bbm.second.getCirc();
		pSubCir->WriteDescriptionToStream(file);
		pSubCir->WriteCircuitModelToStream(file, false);
	}
	m_pMainCircuit->WriteDescriptionToStream(file);
	file << "Common denominator: ";
	m_pCommonDen->WriteToTextStreamModel(file);
	for (auto& Idx2Cof : m_Idx2Cof)
	{
		file << m_Idx2Id[Idx2Cof.first]<<": ";
		Idx2Cof.second->WriteToTextStreamModel(file);
	}
	file << "SubId: 0\n";
	m_pMainCircuit->WriteCircuitModelToStream(file,false);
}

void _CUnlineraBlackBoxEngine::DetAndWriteNumericalResults(const string& FileName)
{
	const string& RaportFileName = m_pMainCircuit->GetRaportFileName();
	const string& ModelFileName = m_pMainCircuit->GetModelFileName();
	time_stamp stamp(RaportFileName.empty() ? "Raport.txt" : RaportFileName, "All numeric dtermination");
	fstream file(ModelFileName.empty() ? FileName.c_str() : ModelFileName.c_str(), ios::out | (ModelFileName.empty() ? 0 : ios::app));
	for (auto& en : m_Idx2Id)
	{
		unsigned long Id = en.first;
		time_stamp stamp(RaportFileName.empty() ? "Raport.txt" : RaportFileName, "Dtermination of cofactor "+long2str(Id));
		const string& tname = en.second;
		_CCofactor* pCof = m_Idx2Cof[Id];
		file << " Cofactor " << tname;
		m_pMainCircuit->WriteDirectNmericalPolynomial(Id, EmptyString, file, nullptr, false, false, 1e6);
	}
}

void _CUnlineraBlackBoxEngine::WriteSomeSimplifications(bool TryToFactorize, bool Write1Level, const string& FileName, bool Append)
{
	long PT = m_pMainCircuit->GetFreePathTraitor();
	m_pMainCircuit->SetApproximationCriterion(PT);
	m_pMainCircuit->WriteFlatSimplifiedPDD(PT, TryToFactorize, Write1Level, FileName, Append);
	m_pMainCircuit->ReleasePathTraitor(PT);
}

void _CUnlineraBlackBoxEngine::SetCompPrioritySub(long SubId, size_t Priority)
{
	auto fit = m_BlackBoxesModels.find(SubId);
	if (fit == m_BlackBoxesModels.end())
		RISEPDD(eIllegalModelName, "Wrong subcircuit Id");
	fit->second.getCirc()->SetCompPriority(Priority);
}
