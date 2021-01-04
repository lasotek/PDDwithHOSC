#include "StdAfx.h"
#include "NetDynamicAnalyser.h"
#include "PrjException.h"
#include "PDDEngine.h"


//char* _CNetCircuit::NCName="NetCircuit";

_CNetDynamicAnalyser::_CNetDynamicAnalyser(_CPDDEngine* pEngine, const string& NetName, bool BySubcricuit,string& CommonNodeName):
m_pEngine(pEngine),m_pMainCircuit(NULL),m_pNetCircuit(NULL),m_CommonNodeSet(!CommonNodeName.empty()),
m_InpGatesNo(0),m_CurrentStatesNo(0),m_pCommonDenCof(NULL),m_Symetric(true),m_NetName(NetName),m_iCommonDenCof(-1),m_BySubcircuit(BySubcricuit),
m_ForceCaluclationWithConductance(false)
{
	long Id=pEngine->CreateNewCircuit();
	m_pMainCircuit=pEngine->GetCircuit(Id);
	m_pMainCircuit->SetOptimizeComponentOrder(false);
	m_PassiveCompBuffer.m_pMainCircuit=m_pMainCircuit;
	if(!CommonNodeName.empty())
		m_pMainCircuit->DeclareGlobalRefNode(CommonNodeName);
	if(m_BySubcircuit)
		m_pNetCircuit=m_pMainCircuit->DefineNewNetDynSubcircuit(m_NetName,true);
	m_iCommonDenCof=m_pMainCircuit->CreateNewCofactor();
	//m_pCommonDenCof=CreateNewCofactor();
	m_pCommonDenCof=m_pMainCircuit->GetCofactor(m_iCommonDenCof);
}

_CNetDynamicAnalyser::~_CNetDynamicAnalyser()
{
	//delete m_pNetCircuit;
}

//_CNetDynamicAnalyser::_CRLCDSection::_CRLCDSection(const _CRLCDSection& Source):
//		m_SectionName(Source.m_SectionName), 
//		m_NodeIn(Source.m_NodeIn), 
//		m_NodeOut(Source.m_NodeOut), 
//		m_NodeCom(Source.m_NodeCom), 
//		m_NodeM(Source.m_NodeM), 
//		m_RValue(Source.m_RValue),
//		m_CValue(Source.m_CValue),
//		m_LValue(Source.m_LValue),
//		m_pCoupledSection(Source.m_pCoupledSection),
//		m_MValue(Source.m_MValue),
//		m_CouplingCValue (Source.m_CouplingCValue),
//		m_NodeCCoupled(Source.m_NodeCCoupled)
//{
//}
//
//_CNetDynamicAnalyser::_CRLCDSection::_CRLCDSection(const string& SectionName, 
//		int GNodeIn, int LNodeIn,
//		int GNodeOut, int LNodeOut,
//		int GNodeCom, int LNodeCom,
//		int GNodeM, int LNodeM,
//		long double RValue,
//		long double CValue,
//		long double LValue,
//		const _CRLCDSection* pCoupledSection,
//		long double MValue,
//		long double CouplingCValue):
//		m_SectionName(SectionName), 
//		m_NodeIn(GNodeIn,LNodeIn), 
//		m_NodeOut(GNodeOut,LNodeOut), 
//		m_NodeCom(GNodeCom,LNodeCom), 
//		m_NodeM(GNodeM,LNodeM), 
//		m_RValue(RValue),
//		m_CValue(CValue),
//		m_LValue(LValue),
//		m_pCoupledSection(pCoupledSection),
//		m_MValue(MValue),
//		m_CouplingCValue (CouplingCValue),
//		m_NodeCCoupled(-1,-1)
//{
//	if(m_pCoupledSection!=NULL)
//		m_NodeCCoupled=m_pCoupledSection->m_NodeOut;
//}


void _CNetDynamicAnalyser::AddRLCSection(
	long double RValue,
	long double CValue,
	long double LValue,
	const string& SectionName, 
	const string& NodeInS, 
	const string& NodeOutS, 
	const string& NodeComS) 
{
	SECTION_MAP::iterator it=m_SMap.find(SectionName);
	if(it!=m_SMap.end())
		RISEPDD(eWrongName,(string("Redefinition of section name")+SectionName).c_str());
	string MiddleNodeS=SectionName+'M';
	string RName=(m_ForceCaluclationWithConductance?"G_":"R_")+SectionName; 
	int /*GNodeIn=RegisterGlobalNodeName(NodeInS),*/
		GNodeOut=RegisterGlobalNodeName(NodeOutS),
		GNodeCom=RegisterGlobalNodeName(NodeComS),
		GMiddleNode=RegisterGlobalNodeName(MiddleNodeS);
	int /*NNodeIn=RegisterNetNodeName(NodeInS),*/
		NNodeOut=RegisterNetNodeName(NodeOutS),
		NNodeCom=RegisterNetNodeName(NodeComS),
		NMiddleNode=RegisterNetNodeName(MiddleNodeS);
	//m_FreshStates.push_back(_CStateVar("L_"+SectionName,_CStateVar::ST_CURRENT,LValue,GMiddleNode,GNodeOut));
	//m_FreshStates.push_back(_CStateVar("C_"+SectionName,_CStateVar::ST_VOLTAGE,CValue,GNodeOut,GNodeCom));
	_CStateVar* pLVal=m_FreshStates.RegisterState(*this,"L_"+SectionName,_CStateVar::ST_CURRENT,LValue,GMiddleNode,GNodeOut);
	m_FreshStates.RegisterState(*this,"C_"+SectionName,_CStateVar::ST_VOLTAGE,CValue,GNodeOut,GNodeCom);
	//if(pCoupledSection!=NULL)
	//	m_FreshStates.push_back(_CStateVar("C_"+SectionName+"-"+pCoupledSection->m_SectionName,_CStateVar::ST_VOLTAGE,CouplingCValue,GNodeOut,pCoupledSection->m_NodeOut.second));

	_CRLCDSectionMin RLCDSection(/*NNodeIn,GNodeIn,*/
								NNodeOut,GNodeOut,
								NNodeCom,GNodeCom,
								NMiddleNode,GMiddleNode,
								pLVal);
	m_SMap.insert(SECTION_MAP::value_type(SectionName,RLCDSection));
	/*m_pNetCircuit->*/AddResistor(RName, NodeInS, MiddleNodeS, m_ForceCaluclationWithConductance, m_ForceCaluclationWithConductance ? 1.0L / RValue : RValue);
	PairGlobalLocal(GMiddleNode,NMiddleNode);
	PairGlobalLocal(GNodeOut,NNodeOut);
	PairGlobalLocal(GNodeCom,NNodeCom);
}

//void _CNetDynamicAnalyser::AddRLCSection(
//	long double RValue,
//	long double CValue,
//	long double LValue,
//	const string& SectionName, 
//	const string& NodeInS, 
//	const string& NodeOutS, 
//	const string& NodeComS, 
//	const string& CopledSectionName,
//	long double MValue,
//	long double CouplingCValue)
//{
//	string MiddleNodeS=SectionName+'M';
//	string RName="R_"+SectionName; 
//	int GNodeIn=RegisterGlobalNodeName(NodeInS),
//		GNodeOut=RegisterGlobalNodeName(NodeOutS),
//		GNodeCom=RegisterGlobalNodeName(NodeComS),
//		GMiddleNode=RegisterGlobalNodeName(MiddleNodeS);
//	int NNodeIn=RegisterNetNodeName(NodeInS),
//		NNodeOut=RegisterNetNodeName(NodeOutS),
//		NNodeCom=RegisterNetNodeName(NodeComS),
//		NMiddleNode=RegisterNetNodeName(MiddleNodeS);
//	_CRLCDSection* pCoupledSection=NULL;
//	if(!CopledSectionName.empty())
//	{
//		SECTION_MAP::iterator it=m_SMap.find(CopledSectionName);
//		if(it==m_SMap.end())
//		{
//			string str="Copled section "+CopledSectionName+" does not exist!";
//			RISEPDD(eWrongName,str.c_str());
//		}
//		else
//			pCoupledSection=&it->second;
//	}
//	_CRLCDSection RLCDSection(SectionName,
//		                        NNodeIn,GNodeIn,
//								NNodeOut,GNodeOut,
//								NNodeCom,GNodeCom,
//								NMiddleNode,GMiddleNode,
//								RValue,
//								CValue,
//								LValue,
//								pCoupledSection,
//								MValue,
//								CouplingCValue);
//	SECTION_MAP::iterator it=m_SMap.find(SectionName);
//	if(it!=m_SMap.end())
//		RISEPDD(eWrongName,(string("Redefinition of section name")+SectionName).c_str());
//	m_SMap.insert(SECTION_MAP::value_type(SectionName,RLCDSection));
//	m_FreshStates.push_back(_CStateVar("L_"+SectionName,_CStateVar::ST_CURRENT,LValue,GMiddleNode,GNodeOut));
//	m_FreshStates.push_back(_CStateVar("C_"+SectionName,_CStateVar::ST_VOLTAGE,CValue,GNodeOut,GNodeCom));
//	if(pCoupledSection!=NULL)
//		m_FreshStates.push_back(_CStateVar("C_"+SectionName+"-"+pCoupledSection->m_SectionName,_CStateVar::ST_VOLTAGE,CouplingCValue,GNodeOut,pCoupledSection->m_NodeOut.second));
//
//	m_pNetCircuit->AddResistor(RName,NodeInS,MiddleNodeS,false,RValue);
//	PairGlobalLocal(GMiddleNode,NMiddleNode);
//	PairGlobalLocal(GNodeOut,NNodeOut);
//	PairGlobalLocal(GNodeCom,NNodeCom);
//}

void _CNetDynamicAnalyser::CoupleSections(const string& S1, const string& S2, long double MValue, 
										  bool Capacitor, long double CCValue)
{
	SECTION_MAP::iterator it1=m_SMap.find(S1);
	if(it1==m_SMap.end())
	{
		string S="Unexisted sekction :"+S1;
		RISEPDD(eWrongName,S.c_str());
	}
	SECTION_MAP::iterator it2=m_SMap.find(S2);
	if(it2==m_SMap.end())
	{
		string S="Unexisted sekction :"+S2;
		RISEPDD(eWrongName,S.c_str());
	}
	_CStateVar* pL1=it1->second.GetLState();
	_CStateVar* pL2=it2->second.GetLState();
	m_CoupledInductorsMap.insert(COUPLED_INDUCTORS::value_type(A_STATE_COORDINATES(pL1,pL2),_CValueName("L_("+S1+"-"+S2+")",MValue)));
	//pL1->AddLCoupling(pL2,MValue);
	//pL2->AddLCoupling(pL1,MValue);
	if(!Capacitor) return;
	string CapName="C_("+S1+"-"+S2+")";
	int GNodeOut1=it1->second.GNodeOut();
	int GNodeOut2=it2->second.GNodeOut();
	m_FreshStates.RegisterState(*this,CapName,_CStateVar::ST_VOLTAGE,CCValue,GNodeOut1,GNodeOut2);
}

void _CNetDynamicAnalyser::CoupleInductors(const string& L1, const string& L2, long double MValue)
{
	_CStateList::iterator it1=m_States.find(L1);
	if(it1==m_States.end())
	{
		string S="Unexisted inductor :"+L1;
		RISEPDD(eWrongName,S.c_str());
	}
	_CStateVar* pL1=it1->second;
	if(pL1->StateType()!=_CStateVar::ST_CURRENT)
	{
		string S=L1+" is not an inductor";
		RISEPDD(eWrongName,S.c_str());
	}
	_CStateList::iterator it2=m_States.find(L2);
	if(it2==m_States.end())
	{
		string S="Unexisted inductor :"+L2;
		RISEPDD(eWrongName,S.c_str());
	}
	_CStateVar* pL2=it2->second;
	if(pL2->StateType()!=_CStateVar::ST_CURRENT)
	{
		string S=L2+" is not an inductor";
		RISEPDD(eWrongName,S.c_str());
	}
	//pL1->AddLCoupling(pL2,MValue);
	//pL2->AddLCoupling(pL1,MValue);
	m_CoupledInductorsMap.insert(COUPLED_INDUCTORS::value_type(A_STATE_COORDINATES(pL1,pL2),_CValueName("L_("+L1+"-"+L2+")",MValue)));
}

void _CNetDynamicAnalyser::DefineInputVS(const string& ValueName, long double Voltage,const string& HotNode,const string& ColdNode)
{
	int GHotNode=RegisterGlobalNodeName(HotNode),
		GColdNode=RegisterGlobalNodeName(ColdNode);
	int NHotNode=RegisterNetNodeName(HotNode),
		NColdNode=RegisterNetNodeName(ColdNode);
	_CInput Inp(ValueName,Voltage,GHotNode,GColdNode);
	INP_SET::iterator it=m_InpSet.find(Inp);
	if(it!=m_InpSet.end())
		RISEPDD(eWrongNodeName,"Duplicated input");
	it=m_FeshInputs.find(Inp);
	if(it!=m_FeshInputs.end())
		RISEPDD(eWrongNodeName,"Duplicated input");
	m_FeshInputs.insert(Inp);
	m_InpGatesNo++;
	PairGlobalLocal(GHotNode,NHotNode);
	PairGlobalLocal(GColdNode,GColdNode);
}

void _CNetDynamicAnalyser::DefineOutputV(const string& ValueName, const string& HotNode,const string& ColdNode)
{
	int GHotNode=RegisterGlobalNodeName(HotNode),
		GColdNode=RegisterGlobalNodeName(ColdNode);
	int NHotNode=RegisterNetNodeName(HotNode),
		NColdNode=RegisterNetNodeName(ColdNode);
	_COutput Outp(ValueName,GHotNode,GColdNode);
	OUT_SET::iterator it=m_OutSet.find(Outp);
	if(it!=m_OutSet.end())
		RISEPDD(eWrongNodeName,"Duplicated outputs");
	it=m_FreshOuts.find(Outp);
	if(it!=m_FreshOuts.end())
		RISEPDD(eWrongNodeName,"Duplicated outputs");
	m_FreshOuts.insert(Outp);
	PairGlobalLocal(GHotNode,NHotNode);
	PairGlobalLocal(GColdNode,GColdNode);
}
int _CNetDynamicAnalyser::RegisterGlobalNodeName(const string& StrNode)
{
	if(StrNode.empty())
		if(m_CommonNodeSet)
			return 0;
		else
			RISEPDD(eWrongNodeName,"Common Reference node not defined");
	return m_pMainCircuit->TranslateOrAddNode(StrNode);
}

int _CNetDynamicAnalyser::RegisterNetNodeName(const string& StrNode)
{
	if(!m_BySubcircuit) return 0;
	if(StrNode.empty())
		if(m_CommonNodeSet)
			return 0;
		else
			RISEPDD(eWrongNodeName,"Common Reference node not defined");
	return m_pNetCircuit->TranslateOrAddNode(StrNode);
}

void _CNetDynamicAnalyser::PairGlobalLocal(int GlobalNode, int LocalNode)
{
	if(!m_BySubcircuit) return;
	m_iConnections.Connect(LocalNode,GlobalNode);
	//m_pNetCircuit->AddBoudaryNode(LocalNode);
}

//int _CNetDynamicAnalyser::NodeName2Node(const string& StrNode) 
//{
//	if(StrNode.empty())
//		if(m_CommonNodeSet)
//			return 0;
//		else
//			RISEPDD(eWrongNodeName,"Common Reference node not defined");
//	m_Connections.push_back(StrNode+" = "+StrNode);
//	m_pMainCircuit->TranslateOrAddNode(StrNode);
//	return m_pNetCircuit->TranslateOrAddNode(StrNode);
//}

int _CNetDynamicAnalyser::DenominatorRank()
{
	return m_pMainCircuit->NoOfNodes()-m_pCommonDenCof->GetRank();
}




void _CNetDynamicAnalyser::CreateNewACofItems(_CStateVar& StateInp, _CStateVar& StateOut)
{
	short Sgn=1;
	int Operation=0;
	bool Symetric=m_Symetric;
	int HotInput=StateInp.HotNode();
	int ColdInput=StateInp.ColdNode();
	int HotOutput=StateOut.HotNode();
	int ColdOutput=StateOut.ColdNode();
	if(&StateInp==&StateOut)
	{
		Sgn=-Sgn;
		Symetric=true;
		if(StateInp.StateType()==_CStateVar::ST_CURRENT)
			Operation=_CDeletions::SUPERPOSE_SUBSTITUTE_INPUT | _CDeletions::SUPERPOSE_SUBSTITUTE_OUTPUT;
		else
			Operation=_CDeletions::SUPERPOSE_ADD_GATES;
	}
	else 
	{
		Operation|=_CDeletions::SUPERPOSE_ADD_GATES;
		if(StateInp.StateType()==_CStateVar::ST_CURRENT)
			Operation|=_CDeletions::SUPERPOSE_SUBSTITUTE_INPUT;
		else
			Sgn=-Sgn;
		if(StateOut.StateType()==_CStateVar::ST_CURRENT)
			Operation|=_CDeletions::SUPERPOSE_SUBSTITUTE_OUTPUT;
	}
	long iCofactor=m_pMainCircuit->CreateNewCofactor();
	_CCofactor* pCofactor=m_pMainCircuit->GetCofactor(iCofactor);
	pCofactor->CopyAndForceSuperposition(*m_pCommonDenCof,Operation,HotInput,ColdInput,HotOutput,ColdOutput,Sgn);
	m_ACofactors.insert(A_COFACTOR_TABLE::value_type(A_STATE_COORDINATES(&StateInp,&StateOut),pCofactor));
	m_AICofactors.insert(AI_COFACTOR_TABLE::value_type(A_STATE_COORDINATES(&StateInp,&StateOut),iCofactor));
	string TrName="T("+StateInp.Description()+"=>"+StateOut.Description()+")";
	long TrId=m_pMainCircuit->DefineTransferFunctionSimp(TrName,iCofactor,m_iCommonDenCof);
	m_TransDefs.insert(TRANS_DEF_TABLE::value_type(A_STATE_COORDINATES(&StateInp,&StateOut),(size_t)TrId));
	if(Symetric) return;
	long iCofactorT=m_pMainCircuit->CreateNewCofactor();
	_CCofactor* pCofactorT=m_pMainCircuit->GetCofactor(iCofactorT);
	pCofactorT->CopyTranposed(*pCofactor,StateOut.StateType()==_CStateVar::ST_VOLTAGE);
	m_ACofactors.insert(A_COFACTOR_TABLE::value_type(A_STATE_COORDINATES(&StateOut,&StateInp),pCofactorT));
	m_AICofactors.insert(AI_COFACTOR_TABLE::value_type(A_STATE_COORDINATES(&StateOut,&StateInp),iCofactorT));
	string TrNameT="T("+StateOut.Description()+"=>"+StateInp.Description()+")";
	long TrIdT=m_pMainCircuit->DefineTransferFunctionSimp(TrNameT,iCofactorT,m_iCommonDenCof);
	m_TransDefs.insert(TRANS_DEF_TABLE::value_type(A_STATE_COORDINATES(&StateOut,&StateInp),(size_t)TrIdT));
}

void _CNetDynamicAnalyser::CreateNewBCofItems(const _CInput& Input, const _CStateVar& StateOut)
{
	int Operation=_CDeletions::SUPERPOSE_ADD_GATES | _CDeletions::SUPERPOSE_SUBSTITUTE_INPUT;
	if(StateOut.StateType()==_CStateVar::ST_CURRENT)
		Operation|=_CDeletions::SUPERPOSE_SUBSTITUTE_OUTPUT;
	long iCofactor=m_pMainCircuit->CreateNewCofactor();
	_CCofactor* pCofactor=m_pMainCircuit->GetCofactor(iCofactor);
	pCofactor->CopyAndForceSuperposition(*m_pCommonDenCof,Operation,Input.HotNode(),Input.ColdNode(),
		StateOut.HotNode(),StateOut.ColdNode());
	m_BCofactors.insert(B_COFACTOR_TABLE::value_type(B_STATE_COORDINATES(&Input,&StateOut),pCofactor));
	m_BICofactors.insert(BI_COFACTOR_TABLE::value_type(B_STATE_COORDINATES(&Input,&StateOut),iCofactor));
	string TrName="T("+Input.ValueName()+"=>"+StateOut.Description()+")";
	long TrId=m_pMainCircuit->DefineTransferFunctionSimp(TrName,iCofactor,m_iCommonDenCof);
	m_TransBaseDefs.insert(TRANS_BASE_DEF_TABLE::value_type(B_STATE_COORDINATES(&Input,&StateOut),(size_t)TrId));
}


void _CNetDynamicAnalyser::UpdateMatricesByNewInputGates()
{
	for(INP_SET::iterator iit=m_FeshInputs.begin(),_ie=m_FeshInputs.end();iit!=_ie;iit++)
	{
		const _CInput& Input=*m_InpSet.insert(*iit).first;
		int HotNode=Input.HotNode();
		int ColdNode=Input.ColdNode();
		m_pCommonDenCof->AddDeletions(HotNode,ColdNode,HotNode,ColdNode,false,false,0);
		for(A_COFACTOR_TABLE::iterator ait=m_ACofactors.begin(),_ae=m_ACofactors.end();ait!=_ae;ait++)
			ait->second->AddDeletions(HotNode,ColdNode,HotNode,ColdNode,false,false,0);
		for(B_COFACTOR_TABLE::iterator bit=m_BCofactors.begin(),_be=m_BCofactors.end();bit!=_be;bit++)
			bit->second->AddDeletions(HotNode,ColdNode,HotNode,ColdNode,false,false,0);
		for(_CStateList::iterator sit=m_States.begin(),_se=m_States.end();sit!=_se;sit++)
			CreateNewBCofItems(Input,*sit->second);
	}
	m_FeshInputs.clear();
}

void _CNetDynamicAnalyser::UpdateMatricesByNewStates()
{
	for(_CStateList::iterator it=m_FreshStates.begin(),_e=m_FreshStates.end();it!=_e;/*it++*/)
	{
		//m_States.push_back(*it);
		//_CStateVar& State=m_States.back();
		_CStateList::iterator nit=it;
		m_States.insert(it,++nit);
		_CStateVar& State=*it->second;
		it=m_FreshStates.erase(it);
		int HotNode=State.HotNode();
		int ColdNode=State.ColdNode();
		if(State.StateType()==_CStateVar::ST_CURRENT)
		{
			m_pCommonDenCof->AddDeletions(HotNode,ColdNode,HotNode,ColdNode,false,false,0);
			for(A_COFACTOR_TABLE::iterator ait=m_ACofactors.begin(),_ae=m_ACofactors.end();ait!=_ae;ait++)
				ait->second->AddDeletions(HotNode,ColdNode,HotNode,ColdNode,false,false,0);
			for(B_COFACTOR_TABLE::iterator bit=m_BCofactors.begin(),_be=m_BCofactors.end();bit!=_be;bit++)
				bit->second->AddDeletions(HotNode,ColdNode,HotNode,ColdNode,false,false,0);
		}
		for(INP_SET::iterator iit=m_InpSet.begin(),_ie=m_InpSet.end();iit!=_ie;iit++)
			CreateNewBCofItems(*iit,State);
		//CreateNewACofItems(State,State);
		//for(STATES::iterator oldit=m_States.begin(),_oe=m_States.end();oldit!=_oe;oldit++)
		//	CreateNewACofItems(*oldit,State);
		for(_CStateList::iterator oldit=m_States.begin(),_oe=m_States.end();oldit!=_oe;oldit++)
			CreateNewACofItems(*oldit->second,State);
	}
	m_FreshStates.clear();
}

void _CNetDynamicAnalyser::UpdateMatricesByNewOutputGates()
{
}

_CNetDynamicAnalyser::COF_OF_A_RES _CNetDynamicAnalyser::Cof_of_A(_CStateVar* Row,_CStateVar* Col)
{
	A_COFACTOR_TABLE::iterator it=m_ACofactors.find(A_STATE_COORDINATES(Row,Col));
	if(it!=m_ACofactors.end())
		return COF_OF_A_RES(it->second,1);
	if(!m_Symetric)
		RISEPDD(eWrongCondition,"Wrong coordinates in asymetric _CNetDynamicAnalyser::Cof_of_A");
	it=m_ACofactors.find(A_STATE_COORDINATES(Col,Row));
	if(it==m_ACofactors.end())
		RISEPDD(eWrongCondition,"Wrong coordinates in _CNetDynamicAnalyser::Cof_of_A");
	return COF_OF_A_RES(it->second,(Row->StateType()!=Col->StateType())?-1:1);//????
}

long _CNetDynamicAnalyser::AddResistor(const string& name, const string& NodeA, const string& NodeB,bool IsConductance,long double Value)
{
	m_PassiveCompBuffer.StoreContener(new _CResistorComponentCont(name,NodeA,NodeB,IsConductance,Value));
	return 0;
}

long _CNetDynamicAnalyser::AddCapacitor(const string& name, const string& NodeA, const string& NodeB,long double Value)
{
	int GHotNode=RegisterGlobalNodeName(NodeA),
		GColdNode=RegisterGlobalNodeName(NodeB);
	int NHotNode=RegisterNetNodeName(NodeA),
		NColdNode=RegisterNetNodeName(NodeB);
	//m_FreshStates.push_back(_CStateVar(name,_CStateVar::ST_VOLTAGE,Value,GHotNode,GColdNode));
	m_FreshStates.RegisterState(*this,name,_CStateVar::ST_VOLTAGE,Value,GHotNode,GColdNode);
	PairGlobalLocal(GHotNode,NHotNode);
	PairGlobalLocal(GColdNode,GColdNode);
	return 0;
}

long _CNetDynamicAnalyser::AddInductor(const string& name, const string& NodeA, const string& NodeB,long double Value)
{
	int GHotNode=RegisterGlobalNodeName(NodeA),
		GColdNode=RegisterGlobalNodeName(NodeB);
	int NHotNode=RegisterNetNodeName(NodeA),
		NColdNode=RegisterNetNodeName(NodeB);
	//m_FreshStates.push_back(_CStateVar(name,_CStateVar::ST_CURRENT,Value,GHotNode,GColdNode));
	m_FreshStates.RegisterState(*this,name,_CStateVar::ST_CURRENT,Value,GHotNode,GColdNode);
	PairGlobalLocal(GHotNode,NHotNode);
	PairGlobalLocal(GColdNode,GColdNode);
	return 0;
}

long _CNetDynamicAnalyser::AddTransconductance(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
	long double Value)
{

	m_Symetric=OutP==InP && OutM==InM;
	m_PassiveCompBuffer.StoreContener(new _CTransonductanceComponentCont(name,OutP,OutM,InP,InM,Value));
	return 0;
}

long _CNetDynamicAnalyser::AddTransimpedance(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
	long double Value)
{
	m_Symetric=OutP==InM && OutM==InP;
	m_PassiveCompBuffer.StoreContener(new _CTransimpedanceComponentCont(name,OutP,OutM,InP,InM,Value));
	return 0;
}

long _CNetDynamicAnalyser::AddVoltageGain(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
	long double Value)
{
	m_Symetric=OutP==InM && OutM==InP;
	m_PassiveCompBuffer.StoreContener(new _CVoltageGainComponentCont(name,OutP,OutM,InP,InM,Value));
	return 0;
}

long _CNetDynamicAnalyser::AddCurrentGain(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
	long double Value)
{
	m_Symetric=OutP==InP && OutM==InM;
	m_PassiveCompBuffer.StoreContener(new _CCurrentGainComponentCont(name,OutP,OutM,InP,InM,Value));
	return 0;
}

long _CNetDynamicAnalyser::AddNullor(const string& OutP, const string& OutM, const string& InP, const string& InM)
{
	m_Symetric=(OutP==InP && OutM==InM) || (OutP==InM && OutM==InP);
	m_PassiveCompBuffer.StoreContener(new _CNullorComponentCont(OutP,OutM,InP,InM));
	return 0;
}

long _CNetDynamicAnalyser::AddPathologicalPair(const string& OutP, const string& OutM, const string& InP, const string& InM,bool CurrentMirror,bool VoltageMirror)
{
	m_Symetric=!CurrentMirror && !VoltageMirror && OutP==InP && OutM==InM;
	m_PassiveCompBuffer.StoreContener(new _CPathologicalPairomponentCont(OutP,OutM,InP,InM,CurrentMirror,VoltageMirror));
	return 0;
}

long _CNetDynamicAnalyser::AddOpAmp(const string& Out, const string& II, const string& NI)
{
	m_Symetric=false;
	m_PassiveCompBuffer.StoreContener(new _COpAmpComponentCont(Out,II,NI));
	return 0;
}

long _CNetDynamicAnalyser::AddCurrentConveyor(const string& X, const string& Y, const string& Z,short Type)
{
	m_Symetric=false;
	m_PassiveCompBuffer.StoreContener(new _CCurrentConveyorComponentCont(X,Y,Z,Type));
	return 0;
}

long _CNetDynamicAnalyser::IncludeSubcircuit(const string& InstanceName, const string& ModelName, const _CStrNodes& Connections)
{
	m_PassiveCompBuffer.StoreContener(new _CSubcircuitComponentCont(m_Symetric,InstanceName,ModelName,Connections));
	//long Res=m_pNetCircuit->IncludeSubcircuit(InstanceName,ModelName,Connections);
	//_CSubCircuit* pSubCircuit=m_pNetCircuit->GetSubcircuit(ModelName);
	//m_Symetric=pSubCircuit->IsModelSymetric();
	//return Res;
	return 0;
}

long _CNetDynamicAnalyser::IncludeSubcircuit(const string& InstanceName, long ModelID, const _CStrNodes& Connections)
{
	m_PassiveCompBuffer.StoreContener(new _CSubcircuitComponentCont(m_Symetric,InstanceName,ModelID,Connections));
	//long Res=m_pNetCircuit->IncludeSubcircuit(InstanceName,ModelID,Connections);
	//_CSubCircuit* pSubCircuit=m_pNetCircuit->GetSubcircuit(ModelID);
	//m_Symetric=pSubCircuit->IsModelSymetric();
	//return Res;
	return 0;
}


void _CNetDynamicAnalyser::_CLocalIntNodes::Connect(int LocalPin, int GlobalPin)
{
	m_MaxLocalPin=max(m_MaxLocalPin,LocalPin);
	INT_INT_MAP::iterator it=m_Connections.find(LocalPin);
	if(it==m_Connections.end())
		m_Connections.insert(INT_INT_MAP::value_type(LocalPin,GlobalPin));
	else if(it->second!=GlobalPin)
	{
		string str="Redefinition of pin "+long2str(LocalPin);
		RISEPDD(eWrongCondition,str.c_str());
	}
}

_CIntNodes* _CNetDynamicAnalyser::_CLocalIntNodes::ToIntNodes(_CIntNodes* pIntNodes)
{
	ASSERTPDD(pIntNodes!=NULL);
	pIntNodes->clear();
	pIntNodes->resize(m_MaxLocalPin+1,-1);
	for(INT_INT_MAP::iterator it=m_Connections.begin(),_e=m_Connections.end();it!=_e;it++)
		pIntNodes->at(it->first)=it->second;
	return pIntNodes;
}
_CIntNodes* _CNetDynamicAnalyser::_CLocalIntNodes::ToIntNodesOrdered(_CIntNodes* pIntNodes)
{
	ASSERTPDD(pIntNodes!=NULL);
	pIntNodes->clear();
	pIntNodes->resize(m_Connections.size(),-1);
	int Counter=0;
	for(INT_INT_MAP::iterator it=m_Connections.begin(),_e=m_Connections.end();it!=_e;it++,Counter++)
		pIntNodes->at(Counter)=it->second;
	return pIntNodes;
}


void _CNetDynamicAnalyser::DetermineNetModel()
{
	if(m_BySubcircuit)
	{
		m_pNetCircuit->ForceBoundariesAndConnect(m_iConnections);//Numeracja
		m_PassiveCompBuffer.AddComponents(m_pNetCircuit);
	}
	else
		m_PassiveCompBuffer.AddComponents(m_pMainCircuit);
	UpdateMatricesByNewInputGates();
	UpdateMatricesByNewStates();
	if(m_BySubcircuit)
	{
		size_t CoffRank=m_pCommonDenCof->GetRank(),
			MinRank=CoffRank>1?CoffRank-1:0,
			MaxRank=CoffRank+1;
		SIZE_T_RANGE Range(MinRank,MaxRank);
		m_pNetCircuit->SetRankLimits(Range);
		_CIntNodes BNodes;
		m_iConnections.ToIntNodesOrdered(&BNodes);
		m_pMainCircuit->IncludeSubcircuit("",m_pNetCircuit,BNodes);
	}
	m_pMainCircuit->DoTheCalculations();
}

string _CNetDynamicAnalyser::Translate(const _CInput& Input) const
{
	string Res="V_("+Input.ValueName()+")_("+
		m_pMainCircuit->TranslateNode(Input.HotNode())+"-"+
		m_pMainCircuit->TranslateNode(Input.ColdNode())+")";
	return Res;
}

string _CNetDynamicAnalyser::Translate(const _CStateVar& State,bool Input) const
{
	string Res;
	const string& Name=State.ValueName();
	if(Input)
	{
		Res+=State.StateType()==_CStateVar::ST_CURRENT?"V_("+Name+")_(":"I_("+Name+")_(";
		Res+=State.StateType()==_CStateVar::ST_CURRENT?
			m_pMainCircuit->TranslateNode(State.HotNode()):
			m_pMainCircuit->TranslateNode(State.ColdNode());
		Res+="-";
		Res+=State.StateType()==_CStateVar::ST_CURRENT?
			m_pMainCircuit->TranslateNode(State.ColdNode()):
			m_pMainCircuit->TranslateNode(State.HotNode());
		Res+=")";
	}
	else
		Res+=(State.StateType()==_CStateVar::ST_CURRENT?"I_("+Name+")_(":"V_("+Name+")_(")+
			m_pMainCircuit->TranslateNode(State.HotNode())+"-"+
			m_pMainCircuit->TranslateNode(State.ColdNode())+")";
	return Res;
}

void _CNetDynamicAnalyser::ReportMatricesComponents(
	bool NumericValue,
	bool FlatApprox,
	bool Factorise,
	bool FlatRes,
	bool AllDerivatives,
	bool Exact,
	const NumericType& Accurracy)
{
	if(AllDerivatives)
		m_PassiveCompBuffer.Prepare4Derivatieves();
	long AppPathTr=-1;
	if(FlatApprox)
	{
		AppPathTr=m_pMainCircuit->GetFreePathTraitor(EmptyString);
		m_pMainCircuit->SetApproximationCriterion(AppPathTr,Exact,Accurracy);
		m_pMainCircuit->PrepareAndWriteFlatSimplifiedPDDDiagrams(AppPathTr,Factorise,m_OutFileName,true);
		m_pMainCircuit->DetermineFlatSimplifiedPDD(AppPathTr,Factorise);
		if(Factorise)
		{
			m_pMainCircuit->WriteFactorizedPDDList(AppPathTr,m_OutFileName,true);
		}
	}
	fstream file(m_OutFileName.c_str(),	ios::out | ios::app);
	file<<endl;
	file<<"List of States"<<endl;
	//for(STATES::iterator it=m_States.begin(),_e=m_States.end();it!=_e;it++)
	//	file<<(*it).Description()<<endl;
	for(_CStateList::iterator it=m_States.begin(),_e=m_States.end();it!=_e;it++)
		file<<it->second->Description()<<endl;

	ReportCofactor(file,m_iCommonDenCof,"The common denominator",
		NumericValue,FlatApprox,Factorise,FlatRes,AllDerivatives,AppPathTr);
	for(BI_COFACTOR_TABLE::iterator bit=m_BICofactors.begin(),_be=m_BICofactors.end();bit!=_be;bit++)
	{
		const _CInput* pInp=bit->first.first;
		const _CStateVar* pOState=bit->first.second;
		string Descr="Numerator of "+Translate(*pOState,false)+"/"+Translate(*pInp)+" is:";
		ReportCofactor(file,bit->second,Descr,NumericValue,FlatApprox,Factorise,FlatRes,AllDerivatives,AppPathTr);
		TRANS_BASE_DEF_TABLE::iterator TrIt=m_TransBaseDefs.find(bit->first);
		if(TrIt!=m_TransBaseDefs.end())
			m_pMainCircuit->WriteSimplifiedTransferFunction(TrIt->second,file);
	}	
	for(AI_COFACTOR_TABLE::iterator ait=m_AICofactors.begin(),_ae=m_AICofactors.end();ait!=_ae;ait++)
	{
		const _CStateVar* pIState=ait->first.first;
		const _CStateVar* pOState=ait->first.second;
		_CCofactor* pCof=m_pMainCircuit->GetCofactor(ait->second);
		if(pCof->IsZero()) continue;
		//string sI=Translate(*pIState,true);
		//string sO=Translate(*pOSate,false);
		string Descr="Numerator of "+Translate(*pOState,false)+"/"+Translate(*pIState,true)+" is:";
		ReportCofactor(file,ait->second,Descr,NumericValue,FlatApprox,Factorise,FlatRes,AllDerivatives,AppPathTr);
		TRANS_DEF_TABLE::iterator TrIt=m_TransDefs.find(ait->first);
		bool TrDefined=TrIt!=m_TransDefs.end();
		if(TrDefined)
			m_pMainCircuit->WriteSimplifiedTransferFunction(TrIt->second,file);
		if(m_Symetric && pIState!=pOState)
		{
			Descr="Numerator of ";
			if(pOState->StateType()!=pIState->StateType())
				Descr+=" - ";
			Descr+=Translate(*pIState,false)+"/"+Translate(*pOState,true)+" is:";
			ReportCofactor(file,ait->second,Descr,NumericValue,FlatApprox,Factorise,FlatRes,AllDerivatives,AppPathTr);
			if(TrDefined)
				m_pMainCircuit->WriteSimplifiedTransferFunction(TrIt->second,file);
		}
		//string Descr="Numerator of "+Translate(*pOState,false)+"/"+Translate(*pIState,true);
		//if(m_Symetric && pIState!=pOState)
		//{
		//	Descr+=" and ";
		//	if(pOState->StateType()!=pIState->StateType())
		//		Descr+=" - ";
		//	Descr+=Translate(*pIState,false)+"/"+Translate(*pOState,true);
		//}
		//Descr+=" is:";
		//ReportCofactor(file,ait->second,Descr,NumericValue,FlatApprox,Factorise,FlatRes,AppPathTr);
	}
	//m_pMainCircuit->WriteSimplifiedTransferFunctions(file);
}

void _CNetDynamicAnalyser::ReportCofactor(
						iostream& Stream,
						unsigned long CofId,
						const string& CoffDescription, 
						bool NumericValue,bool FlatApprox,bool Factorise, bool FlatRes, bool AllDerivatieves,
						long AppId)
{
	_CCofactor* pCof=m_pMainCircuit->GetCofactor(CofId);
	if(pCof->IsZero()) return;
	Stream<<"=================================================="<<endl;
	Stream<<CoffDescription<<endl;
	pCof->WriteToTextStreamModel(Stream);
	if(NumericValue)
	{
		_CSparsePolynomial ResPolynomial;
		m_pMainCircuit->GetDirectNmericalPolynomial(CofId,EmptyString,ResPolynomial);
		Stream<<"Numeric value: ";
		ResPolynomial.WriteToTextStream(Stream);
		Stream<<endl;
		if(AllDerivatieves)
		{ 
			//m_PassiveCompBuffer.ReportDerivatives(Stream,CofId);
			m_PassiveCompBuffer.ReportSensitivieties(Stream,CofId,m_iCommonDenCof);
		}
	}
	if(FlatApprox)
	{
		Stream<<"Flat representation entry = ";
		m_pMainCircuit->WriteFlatSimplifiedPDDEntry(Stream,AppId,CofId);
		//Stream<<endl;
		if(Factorise)
		{
			Stream<<"Factorized entry = ";
			m_pMainCircuit->FactorizedCofFlatVertices2Stream(Stream,AppId,CofId);
			if(FlatRes)
			{
				Stream<<"Flat Factorized value = ";
				m_pMainCircuit->FactorizedCofFlat1LevelVertices2Stream(Stream,AppId,CofId);
			}
		}
	}
	Stream<<"=================================================="<<endl;
}

_CNetDynamicAnalyser::_CStateList::~_CStateList()
{
	for(iterator it=begin(),_e=end();it!=_e;it++)
		delete it->second;
}

_CNetDynamicAnalyser::_CStateVar* _CNetDynamicAnalyser::_CStateList::RegisterState(
													  _CNetDynamicAnalyser& ParentAnalyzer,
	                                                  const string& ValueName,
													  _CStateVar::STATE_TYPE Type, 
													  long double  Value,
													  int HotNode,
													  int ColdNode)
{
	_CStateVar* pNewState=new _CStateVar(ParentAnalyzer,ValueName,Type,Value,HotNode,ColdNode);
	try
	{
		insert(value_type(ValueName,pNewState));
		return pNewState;
	}
	catch(...)
	{
		delete pNewState;
		throw;
	}
}

//bool _CNetDynamicAnalyser::_CStateVar::AddLCoupling(_CStateVar* pState,long double Value)
//{
//	if(pState->StateType()!=_CStateVar::ST_CURRENT)
//		return false;
//	if(m_pCoupled==NULL)
//		m_pCoupled=new COUPLED;
//	m_pCoupled->insert(COUPLED::value_type(pState,Value));
//	return true;
//}

void _CPassiveComponentBuff::clear()
{
	for(LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;)
	{
		delete *it;
		it=m_List.erase(it);
	}
}
void _CPassiveComponentBuff::AddComponents(_CCircuit* pCircuit)
{
	for(LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		(*it)->AddComponent(pCircuit);
}
void _CPassiveComponentBuff::Prepare4Derivatieves()
{
	ASSERTPDD(m_pMainCircuit!=NULL);
	for(LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		(*it)->CreateCompTraitor(m_pMainCircuit);
}

void _CPassiveComponentBuff::ReportDerivatives(iostream& file,long CofId)
{
	ASSERTPDD(m_pMainCircuit!=NULL);
	_CSparsePolynomial ResPoly;
	for(LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		if((*it)->GetDerivative(CofId,ResPoly))
		{
			file<<"Derivative by "<<(*it)->m_name<<": ";
			ResPoly.WriteToTextStream(file);
			file<<endl;
		}
		
}
void _CPassiveComponentBuff::ReportSensitivieties(iostream& file,long CofId, long ComDenCofId)
{
	ASSERTPDD(m_pMainCircuit!=NULL);
	_CSparsePolynomial ResPoly;
	_CSparsePolynomial SensPoly;
	m_pMainCircuit->GetDirectNmericalPolynomial(CofId,EmptyString,ResPoly);
	if(IsZero(ResPoly.get(0))) return;
	for(LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		if((*it)->GetDerivative(CofId,SensPoly))
		{
			file<<"Sensitivity of cofactor by "<<(*it)->m_name<<": ";
			NumericType Res=SensPoly.get(0)/ResPoly.get(0)*(*it)->GetValue();
			file<<Res;
			if(CofId!=ComDenCofId)
			{	
				_CSparsePolynomial DenResPoly,DenSensPoly;
				m_pMainCircuit->GetDirectNmericalPolynomial(ComDenCofId,EmptyString,DenResPoly);
				(*it)->GetDerivative(ComDenCofId,DenSensPoly);
				NumericType DenRes=DenSensPoly.get(0)/DenResPoly.get(0)*(*it)->GetValue();
				file<<"\tSensitivity of function by "<<(*it)->m_name<<": ";
				file<<Res-DenRes;
			}
			file<<endl;
		}
}

_CPassiveComponentCont::~_CPassiveComponentCont()
{
	//if(m_pMainCircuit!=NULL && m_hTraitor>=0)
	//	m_pMainCircuit->ClearPathTraitor(m_hTraitor);
}

void _CPassiveComponentCont::CreateCompTraitor(_CMainCircuit* pMainCircuit)
{
	if(!m_ConsiderInDerivative) return;
	if(m_pMainCircuit!=NULL)
		m_pMainCircuit->ReleasePathTraitor(m_hTraitor);
	m_pMainCircuit=pMainCircuit;
	if(m_pMainCircuit!=NULL)
	{
		m_hTraitor=m_pMainCircuit->GetFreePathTraitor();
		m_pMainCircuit->AddParamDivider(m_hTraitor,m_name);
	}
}

bool _CPassiveComponentCont::GetDerivative(long CofId,_CSparsePolynomial& Result)
{
	if(!m_ConsiderInDerivative) return false;
	Result.ClearAll();
	m_pMainCircuit->GetNumericalCofactorAndReminder(CofId,m_hTraitor,Result);
	return true;
}
