#include "StdAfx.h"
#include "PDDEngineManager.h"
#include "PrjException.h"
#include "..\PDDErrorsAndConst.h"
#include <algorithm>
#include "MainCircuit.h"
#include "SubCircuit.h"
#include "NodesManager.h"


_CErrorQue::~_CErrorQue()
{

}

void _CErrorQue::insert_message(const string& message)
{
	if(m_MainList.size()>=m_MaxDepth)
		m_MainList.pop_front();
	m_MainList.push_back(message);
}

void _CErrorQue::ResetDepth(unsigned MaxDepth)
{
	m_MaxDepth=MaxDepth;
	while(m_MainList.size()>m_MaxDepth)
		m_MainList.pop_front();
}

#define len1 65
string _CErrorQue::ReportAll()
{
	string Res;
	unsigned NoOfMessages=m_MainList.size()-1;
	char buffer[len1];
	sprintf_s(buffer,len1,"%d",NoOfMessages);
	int max_len=strlen(buffer), lc=NoOfMessages;
	T_REV_ITERATOR rit=m_MainList.rbegin();
	for(;rit!=m_MainList.rend();lc--,rit++)
	{
		sprintf_s(buffer,len1,"%0*d - ",max_len,lc);
		string aux(buffer);
		aux+=(*rit);
		aux+="\n";
		Res+=aux;
	}
	return Res;
}
string _CErrorQue::PopMessage()
{
	if(m_MainList.size()==0)
		return EmptyString;
	string Res=m_MainList.front();
	m_MainList.pop_front();
	return Res;
}

_CErrorQue LastErrorInfo;
//string LastErrorInfo;
const string invalid_circuit_msg("Wrong circuit handler in: ");
const string invalid_engine_msg("Wrong engine handler in: ");
const string range_error_msg("Range error in: ");

#define USES_CIRCUIT _CMainCircuit* pCircuit=NULL;long state=0;\
	if((state=hCircuit2Circuit(hCircuit,pCircuit))<0)\
		return state;\
		try{

#define FINISH_CIRCUIT }\
	catch(exception& e)\
{\
	LastErrorInfo=e.what();\
	return UNEXPECTED_ERROR;\
}

#define USES_SUBCIRCUIT_NAME _CSubCircuit* pSubCircuit=NULL;long state=0;\
	if((state=hCircuit2SubCircuit(hCircuit,SubcircuitName,pSubCircuit))<0)\
		return state;\
		try{

#define FINISH_SUBCIRCUIT }\
	catch(exception& e)\
{\
	LastErrorInfo=e.what();\
	return UNEXPECTED_ERROR;\
}

#define USES_SUBCIRCUIT_ID _CSubCircuit* pSubCircuit=NULL;long state=0;\
	if((state=hCircuit2SubCircuit(hCircuit,SubcircuitId,pSubCircuit))<0)\
		return state;\
		try{


CPDDEngineManager::~CPDDEngineManager(void)
{
	for_each(m_Engines.begin(),m_Engines.end(),CAllRemover());
}

long CPDDEngineManager::GetPDDEngine(ULONG hAppInstance,bool sExpanded, bool ComponentOrderOptimization,bool UseNewAnalyser)
{
	try
	{
		_CPDDEngine* pEngine=new _CPDDEngine(sExpanded,ComponentOrderOptimization,UseNewAnalyser);
		_CPDDEngineInstance* pInstance=new _CPDDEngineInstance (hAppInstance,pEngine);
		int i=1;
		ITERATOR itest=find(m_Engines.begin(),m_Engines.end(),(_CPDDEngineInstance*)NULL);
		if(itest!=m_Engines.end())
		{
			(*itest)=pInstance;
			return itest-m_Engines.begin()+1;
		}
		m_Engines.push_back(pInstance);
		return m_Engines.size();
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in GetPDDEngine";
		return ALLOCATION_ERROR;
	}
	catch(exception& e)
	{
		LastErrorInfo=e.what();
		return UNEXPECTED_ERROR;
	}
}

int CPDDEngineManager::ReleasePDDEngine(long hInstanceEngine)
{
	try
	{
		if(hInstanceEngine==0 && hInstanceEngine>(long)m_Engines.size())
		{
			LastErrorInfo=invalid_circuit_msg+"CPDDEngineManager::ReleasePDDEngine";
			return INVALID_CIRCUIT_HANDLER;
		}
		_CPDDEngineInstance*& pInstance=m_Engines.at(hInstanceEngine-1);
		if(pInstance==NULL)
			return EMPTY_VALUE;
		delete pInstance->second;
		delete pInstance;
		pInstance=NULL;
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in ReleasePDDEngine";
		return ALLOCATION_ERROR;
	}
	catch(range_error& e)
	{
		LastErrorInfo=string(e.what())+" in ReleasePDDEngine";
		return RANGE_ERROR;
	}
	catch(exception& e)
	{
		LastErrorInfo=e.what();
		return UNEXPECTED_ERROR;
	}
	return 0;
}
int CPDDEngineManager::ReleaseMyPDDEngines(ULONG hAppInstance)
{	
	try{
		return for_each(m_Engines.begin(),m_Engines.end(),CInstanceRemover(hAppInstance));
	}
	catch(exception& e)
	{
		LastErrorInfo=e.what();
		return -1;
	}
}

long long CPDDEngineManager::GetNewCircuit(long hEngineInstance)
{
	try
	{
		if(hEngineInstance<0 || hEngineInstance>(long)m_Engines.size())
		{
			LastErrorInfo=invalid_engine_msg+"CPDDEngineManager::GetNewCircuit";
			return MakeCircuitId(INVALID_ENGINE_HANDLER,0);
		}
		_CPDDEngineInstance* pInst=m_Engines[hEngineInstance-1];
		if(pInst==NULL)
		{
			LastErrorInfo=invalid_engine_msg+"CPDDEngineManager::GetNewCircuit";
			return MakeCircuitId(INVALID_ENGINE_HANDLER,0);
		}
		_CPDDEngine* pEng=pInst->second;
		return MakeCircuitId(hEngineInstance,pEng->CreateNewCircuit());
	}
	catch(out_of_range& e)
	{
		LastErrorInfo=string(e.what())+" in GetNewCircuit";
		return MakeCircuitId(RANGE_ERROR,0);
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in GetNewCircuit";
		return MakeCircuitId(ALLOCATION_ERROR,0);
	}
}

long CPDDEngineManager::EngineError(long long hCircuit)
{
	long Error=GetEngineId(hCircuit);
	return Error<0?Error:0;
}

long CPDDEngineManager::CircuitError(long long hCircuit)
{
	long Error=GetCircuitInEngine(hCircuit);
	return Error<0?Error:0;
}

//long CPDDEngineManager::CreateNewSubcircuit(long long hCircuit, string SubName, const _CStrNodes& BoundaryNodes,const SIZE_T_SET* pDeletionAdmissible,bool ForceNewIfExist)
long CPDDEngineManager::CreateNewSubcircuit(long long hCircuit, string SubName, const _CStrNodes& BoundaryNodes,const SIZE_T_RANGE* pDeletionAdmissible,bool ForceNewIfExist)
{
	USES_CIRCUIT;
	return pCircuit->DefineNewSubcircuit(SubName,BoundaryNodes, pDeletionAdmissible,ForceNewIfExist);
	FINISH_SUBCIRCUIT;
}

//long CPDDEngineManager::CreateSubcircuitFromLibrary(long long hCircuit, string SubName, const _CStrNodes& BoundaryNodes,const SIZE_T_SET* pDeletionAdmissible)
long CPDDEngineManager::CreateSubcircuitFromLibrary(long long hCircuit, string SubName, const _CStrNodes& BoundaryNodes,const SIZE_T_RANGE* pDeletionAdmissible)
{
	USES_CIRCUIT;
	return pCircuit->TryToGetSubcircuitFromLibrary(SubName,BoundaryNodes,pDeletionAdmissible);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::hCircuit2Circuit(long long hCircuit, _CMainCircuit*& pCircuit)
{
	long hEngine=GetEngineId(hCircuit);
	long CircuitID=GetCircuitInEngine(hCircuit);
	try
	{
		if(hEngine<0 || hEngine>(long)m_Engines.size())
		{
			LastErrorInfo=invalid_circuit_msg+"CPDDEngineManager::hCircuit2Circuit";
			return INVALID_CIRCUIT_HANDLER;
		}
		_CPDDEngineInstance* pInstance=m_Engines[hEngine-1];
		if(pInstance==NULL)
		{
			LastErrorInfo=invalid_circuit_msg+"CPDDEngineManager::hCircuit2Circuit";
			return INVALID_CIRCUIT_HANDLER;
		}
		_CPDDEngine* pEngine=pInstance->second;
		if(pEngine==NULL)
		{
			LastErrorInfo=invalid_circuit_msg+"CPDDEngineManager::hCircuit2Circuit";
			return INVALID_CIRCUIT_HANDLER;
		}
		pCircuit=pEngine->GetCircuit(CircuitID);
		if(pCircuit==NULL)
		{
			LastErrorInfo=invalid_circuit_msg+"CPDDEngineManager::hCircuit2Circuit";
			return INVALID_CIRCUIT_HANDLER;
		}
	}
	catch(exception& e)
	{
		LastErrorInfo=string(e.what())+" in CPDDEngineManager::hCircuit2Circuit";
		return UNEXPECTED_ERROR;
	}
	return 0;
}
long CPDDEngineManager::hCircuit2SubCircuit(long long hCircuit, const string& SubName,_CSubCircuit*& pSubCircuit)
{
	USES_CIRCUIT;
	if((pSubCircuit=pCircuit->GetSubcircuit(SubName))==NULL)
	{
		LastErrorInfo=string("The circuit ")+SubName+" has not been defined yet.";
		return INVALID_SUBCIRCUIT_NAME;
	}
	else
		return 0;
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::hCircuit2SubCircuit(long long hCircuit, long SubID,_CSubCircuit*& pSubCircuit)
{
	USES_CIRCUIT;
	if((pSubCircuit=pCircuit->GetSubcircuit(SubID))==NULL)
	{
		LastErrorInfo="Wrong subcircuit ID in CPDDEngineManager::hCircuit2SubCircuit";
		return INVALID_SUBCIRCUIT_ID;
	}
	else
		return 0;
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::SetOrederOptimization(long long hCircuit, bool Optimize)
{
	USES_CIRCUIT;
	pCircuit->SetOptimizeComponentOrder(Optimize);
	return 0;
	FINISH_SUBCIRCUIT;
}

long long CPDDEngineManager::CreateNewCofactor(long long hCircuit)
{
	USES_CIRCUIT;
	long LocalhCofactor=pCircuit->CreateNewCofactor();
	m_Cofactors.insert(COF_MAP_VALUE_TYPE(++m_CofactorCounter,GLOBAL_COFACTORS(hCircuit,LocalhCofactor)));
	return m_CofactorCounter;
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::CreateNewCofactor(long long hCircuit, long SubcircuitId) {
	USES_SUBCIRCUIT_ID;
	long localCofactor = pSubCircuit->CreateNewCofactor();
	return localCofactor;
	FINISH_SUBCIRCUIT;
}
long CPDDEngineManager::CreateNewCofactor(long long hCircuit, const string& SubcircuitName) {
	USES_SUBCIRCUIT_NAME;
	long localCofactor = pSubCircuit->CreateNewCofactor();
	return localCofactor;
	FINISH_SUBCIRCUIT;
}

long long CPDDEngineManager::CreateNewCofactor(long long hCircuit, const DELETIONS& newDels) {
	USES_CIRCUIT;
	long LocalhCofactor = pCircuit->CreateNewCofactor();
	if (LocalhCofactor < 0) return -1;
	_CCofactor* pCofactor = nullptr;
	if (pCircuit->getCofactor(LocalhCofactor, pCofactor) < 0) return -1;
	for (const auto& Del : newDels) {
		pCofactor->AddDeletions(Del.first.first, Del.first.second, Del.second.first, Del.second.second);
	}
	m_Cofactors.insert(COF_MAP_VALUE_TYPE(++m_CofactorCounter, GLOBAL_COFACTORS(hCircuit, LocalhCofactor)));
	return m_CofactorCounter;
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::CreateNewCofactor(long long hCircuit, long SubcircuitId, const DELETIONS& newDels) {
	USES_SUBCIRCUIT_ID;
	long localCofactor = pSubCircuit->CreateNewCofactor();
	if (localCofactor < 0) return -1;
	_CCofactor* pCofactor = nullptr;
	if (pSubCircuit->getCofactor(localCofactor, pCofactor) < 0) return -1;
	for (const auto& Del : newDels) {
		pCofactor->AddDeletions(Del.first.first, Del.first.second, Del.second.first, Del.second.second);
	}
	return localCofactor;
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::CreateNewCofactor(long long hCircuit, const string& SubcircuitName, const DELETIONS& newDels) {
	USES_SUBCIRCUIT_NAME;
	long localCofactor = pSubCircuit->CreateNewCofactor();
	if (localCofactor < 0) return -1;
	_CCofactor* pCofactor = nullptr;
	if (pSubCircuit->getCofactor(localCofactor, pCofactor) < 0) return -1;
	for (const auto& Del : newDels) {
		pCofactor->AddDeletions(Del.first.first, Del.first.second, Del.second.first, Del.second.second);
	}
	return localCofactor;
	FINISH_SUBCIRCUIT;
}

long long CPDDEngineManager::DefineTransferFunctionSimp(const string& Name,long long hNumerator, long long hDenominator)
{
	long long hNCircuit,hDCircuit;
	unsigned long hNCofactor, hDCofactor;
	long state=hCofactor2CofId(hNumerator, hNCofactor,hNCircuit);
	if(state<0)
		return state;
	state=hCofactor2CofId(hDenominator,hDCofactor,hDCircuit);
	if(state<0)
		return state;
	if(hNCircuit!=hDCircuit)
		return NOT_THE_SAME_CIRCUIT;
	_CMainCircuit* pMainCircuit=NULL;
	state=hCircuit2Circuit(hNCircuit,pMainCircuit);
	if(state<0)
		return state;
	return pMainCircuit->DefineTransferFunctionSimp(Name,hNCofactor,hDCofactor);
}
long CPDDEngineManager::RaportTransferFunctionValuesLog(long long hCircuit, long long hTransfer, const string& Context, int LowestDec, int HighestDec, size_t NoOfPointsPerDec, bool Transfer2MagPhase)
{
	USES_CIRCUIT;
	return pCircuit->RaportTransferFunctionValuesLog(hTransfer,Context,LowestDec,HighestDec,NoOfPointsPerDec,Transfer2MagPhase);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::RaportTransferFunctionValuesLin(long long hCircuit, long long hTransfer, const string& Context, long double LowestAngFreq, long double HighestAngFreq, size_t NoOfGlobalPoints, bool Transfer2MagPhase)
{
	USES_CIRCUIT;
	return pCircuit->RaportTransferFunctionValuesLin(hTransfer,Context,LowestAngFreq,HighestAngFreq,NoOfGlobalPoints,Transfer2MagPhase);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddExtraTransferFunctionSimp(long long PairId, long long hExtraCofactor)
{
	return 0;
}

long CPDDEngineManager::AddPairOfDeletions(long long hCofactor, const string& sp,const string& sr,const string& sk,const string& sl)
{
	_CCofactor* pCofactor=NULL;
	long state=hCoffactor2CCofactor(hCofactor,pCofactor);
	if(state<0)
		return state;
	pCofactor->AddDeletions(sp,sr,sk,sl);
	return 0; 
}

long CPDDEngineManager::AddPairOfDeletions(long long hCircuit, long SubcircuitId, long hCofactor, const string& sp, const string& sr, const string& sk, const string& sl) {
	if (hCircuit < 0)
		return -1;
	USES_SUBCIRCUIT_ID;
	_CCofactor* pCofactor = nullptr;
	if (pSubCircuit->getCofactor((unsigned long)hCofactor, pCofactor) < 0)
		return -1;
	pCofactor->AddDeletions(sp, sr, sk, sl);
	return 0;
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddPairOfDeletions(long long hCircuit, const string& SubcircuitName, long hCofactor, const string& sp, const string& sr, const string& sk, const string& sl) {
	if (hCircuit < 0)
		return -1;
	USES_SUBCIRCUIT_NAME;
	_CCofactor* pCofactor = nullptr;
	if (pSubCircuit->getCofactor((unsigned long)hCofactor, pCofactor) < 0)
		return -1;
	pCofactor->AddDeletions(sp, sr, sk, sl);
	return 0;
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::DoTheCalculation(long long hCircuit)
{
	USES_CIRCUIT;
	try
	{
		pCircuit->DoTheCalculations();
		return 0;
	}
	//catch(eRecurrentLoop e)
	//{
	//	LastErrorInfo=e.what();
	//	return MODEL_RECURRENTION_LOOP;
	//}
	catch(eIllegalOperation& e)
	{
		LastErrorInfo=e.what();
		return OPERATION_PROHIBITED;
	}
	FINISH_CIRCUIT;
}

//long CPDDEngineManager::TestFlatPDDRemoval(long long hCircuit,long TrId)
//{
//	USES_CIRCUIT;
//	try
//	{
//		pCircuit->TestFlatPDDRemoval(TrId);
//		return 0;
//	}
//	catch(eIllegalOperation e)
//	{
//		LastErrorInfo=e.what();
//		return OPERATION_PROHIBITED;
//	}
//	FINISH_CIRCUIT;
//}

long CPDDEngineManager::DetermineFlatSimplifiedPDD(long long hCircuit, long TrId, bool TryToFactorize)
{
	USES_CIRCUIT;
	try
	{
		pCircuit->DetermineFlatSimplifiedPDD(TrId,TryToFactorize);
		return 0;
	}
	catch(eIllegalOperation& e)
	{
		LastErrorInfo=e.what();
		return OPERATION_PROHIBITED;
	}
	FINISH_CIRCUIT;
}

long CPDDEngineManager::WriteFlatSimplifiedPDD(long long hCircuit, long TrId, bool TryToFactorize,bool Write1Level,const string& FileName, bool Append)
{
	USES_CIRCUIT;
	try
	{
		pCircuit->WriteFlatSimplifiedPDD(TrId,TryToFactorize,Write1Level,FileName,Append);
		return 0;
	}
	catch(eIllegalOperation& e)
	{
		LastErrorInfo=e.what();
		return OPERATION_PROHIBITED;
	}
	FINISH_CIRCUIT;
}

//long CPDDEngineManager::WriteFlatSimplifiedSoP(long long hCircuit, long TrId, bool TryToFactorize,const string& FileName, bool Append)
//{
//	USES_CIRCUIT;
//	try
//	{
//		pCircuit->WriteFlatSimplifiedSoP(TrId,TryToFactorize,FileName,Append);
//		return 0;
//	}
//	catch(eIllegalOperation e)
//	{
//		LastErrorInfo=e.what();
//		return OPERATION_PROHIBITED;
//	}
//	FINISH_CIRCUIT;
//}
//
//long CPDDEngineManager::WriteFlatSimplifiedFeasibleDividers(long long hCircuit,long TrId,const string& FileName, bool Append)
//{
//	USES_CIRCUIT;
//	try
//	{
//		pCircuit->WriteFlatSimplifiedFeasibleDividers(TrId,FileName,Append);
//		return 0;
//	}
//	catch(eIllegalOperation e)
//	{
//		LastErrorInfo=e.what();
//		return OPERATION_PROHIBITED;
//	}
//	FINISH_CIRCUIT;
//}

long CPDDEngineManager::DetermineSimplePDD(long long hCircuit, bool sCExpanded,bool Approximated,long double aAccuracy)
{
	USES_CIRCUIT;
	try
	{
		if(aAccuracy<0.0 || aAccuracy>1.0)
			return WRONG_ACCURACY_VALUE;
		NumericType Accuracy=aAccuracy;
		pCircuit->DetermineSimplePDD(sCExpanded,Approximated,Accuracy);
		return 0;
	}
	catch(eIllegalOperation& e)
	{
		LastErrorInfo=e.what();
		return OPERATION_PROHIBITED;
	}
	FINISH_CIRCUIT;
}

long CPDDEngineManager::hCoffactor2CCofactor(long long hCofactor,_CCofactor*& pCofactor)
{
	COF_MAP_ITERATOR it=m_Cofactors.find(hCofactor);
	if(it==m_Cofactors.end())
		RISEPDD(out_of_range,"Illegal hCofactor handler");
	long long hCircuit=it->second.first;
	long hLocalCofactor=it->second.second;
	USES_CIRCUIT;
	try
	{
		pCofactor=pCircuit->GetCofactor(hLocalCofactor);
	}
	catch(out_of_range& e)
	{
		LastErrorInfo=e.what();
		return ILLEGAL_COFACTOR_ID;
	}
	return 0;
	FINISH_CIRCUIT;
}

long CPDDEngineManager::hCofactor2CofId(long long hCofactor,unsigned long& CofId,long long& hCircuit)
{
	COF_MAP_ITERATOR it=m_Cofactors.find(hCofactor);
	if(it==m_Cofactors.end())
		return INVALID_COFACTOR_ID;
	hCircuit=it->second.first;
	CofId=it->second.second;
	return 0;
}

long CPDDEngineManager::DeclareGlobalRefNode(long long hCircuit, const string& sRefNode)
{
	USES_CIRCUIT
	return pCircuit->DeclareGlobalRefNode(sRefNode);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddResistor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		bool AsConductance)
{
	USES_CIRCUIT
	return pCircuit->AddResistor(name,NodeA,NodeB,AsConductance,Value);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddResistor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		const string& SubcircuitName, bool AsConductance)
{
	USES_SUBCIRCUIT_NAME
	return pSubCircuit->AddResistor(name,NodeA,NodeB,AsConductance,Value);
	FINISH_SUBCIRCUIT;
}
long CPDDEngineManager::AddResistor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		long SubcircuitId, bool AsConductance)
{
	USES_SUBCIRCUIT_ID
	return pSubCircuit->AddResistor(name,NodeA,NodeB,AsConductance,Value);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddCapacitor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value)
{
	USES_CIRCUIT
	return pCircuit->AddCapacitor(name,NodeA,NodeB,Value);
	FINISH_SUBCIRCUIT;
}
long CPDDEngineManager::AddCapacitor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
	const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME
	return pSubCircuit->AddCapacitor(name,NodeA,NodeB,Value);
	FINISH_SUBCIRCUIT;
}
long CPDDEngineManager::AddCapacitor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
	long SubcircuitId)
{
	USES_SUBCIRCUIT_ID
	return pSubCircuit->AddCapacitor(name,NodeA,NodeB,Value);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddInductor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value)
{
	USES_CIRCUIT
	return pCircuit->AddInductor(name,NodeA,NodeB,Value);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddInductor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
	const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME
	return pSubCircuit->AddInductor(name,NodeA,NodeB,Value);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddInductor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
	long SubcircuitId)
{
	USES_SUBCIRCUIT_ID
	return pSubCircuit->AddInductor(name,NodeA,NodeB,Value);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddTransconductance(long long hCircuit,const string& name,const string& OutP, const string& OutM,
	const string& InP, const string& InM, int sPower, long double Value)
{
	USES_CIRCUIT;
	return pCircuit->AddTransconductance(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddTransconductance(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
	const string& InP, const string& InM, int sPower, long double Value, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddTransconductance(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddTransconductance(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
	const string& InP, const string& InM, int sPower, long double Value,long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddTransconductance(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddTransimpedance(long long hCircuit,const string& name,const string& OutP, const string& OutM,
	const string& InP, const string& InM, int sPower, long double Value)
{
	USES_CIRCUIT;
	return pCircuit->AddTransimpedance(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddTransimpedance(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
	const string& InP, const string& InM, int sPower, long double Value, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddTransimpedance(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddTransimpedance(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
	const string& InP, const string& InM, int sPower, long double Value,long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddTransimpedance(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddVoltageGain(long long hCircuit,const string& name,const string& OutP, const string& OutM,
	const string& InP, const string& InM, int sPower, long double Value)
{
	USES_CIRCUIT;
	return pCircuit->AddVoltageGain(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddVoltageGain(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
	const string& InP, const string& InM, int sPower, long double Value, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddVoltageGain(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddVoltageGain(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
	const string& InP, const string& InM, int sPower, long double Value,long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddVoltageGain(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddCurrentGain(long long hCircuit,const string& name,const string& OutP, const string& OutM,
	const string& InP, const string& InM, int sPower, long double Value)
{
	USES_CIRCUIT;
	return pCircuit->AddCurrentGain(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddCurrentGain(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
	const string& InP, const string& InM, int sPower, long double Value, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddCurrentGain(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddCurrentGain(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
	const string& InP, const string& InM, int sPower, long double Value,long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddCurrentGain(name,OutP,OutM,InP,InM,Value,sPower);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddNullor(long long hCircuit,const string& OutP, const string& OutM,const string& InP, const string& InM)
{
	USES_CIRCUIT;
	return pCircuit->AddNullor(OutP,OutM,InP,InM);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddNullor(long long hCircuit,const string& OutP, const string& OutM, const string& InP, const string& InM,
	const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddNullor(OutP,OutM,InP,InM);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddNullor(long long hCircuit,const string& OutP, const string& OutM, const string& InP, const string& InM,
	long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddNullor(OutP,OutM,InP,InM);
	FINISH_SUBCIRCUIT;
}
long CPDDEngineManager::AddPathologicalPair(long long hCircuit,const string& OutP, const string& OutM,const string& InP, const string& InM,bool CurrentMirror,bool VoltageMirror)
{
	USES_CIRCUIT;
	return pCircuit->AddPathologicalPair(OutP,OutM,InP,InM,CurrentMirror,VoltageMirror);
	FINISH_SUBCIRCUIT;
}
long CPDDEngineManager::AddPathologicalPair(long long hCircuit,const string& OutP, const string& OutM, const string& InP, const string& InM,bool CurrentMirror,bool VoltageMirror,
	const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddPathologicalPair(OutP,OutM,InP,InM,CurrentMirror,VoltageMirror);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddPathologicalPair(long long hCircuit,const string& OutP, const string& OutM, const string& InP, const string& InM,bool CurrentMirror,bool VoltageMirror,
	long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddPathologicalPair(OutP,OutM,InP,InM,CurrentMirror,VoltageMirror);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddVoltageFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM)
{
	USES_CIRCUIT;
	return pCircuit->AddVoltageFollower(OutP, OutM, InP, InM);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddVoltageFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM,
	const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddVoltageFollower(OutP, OutM, InP, InM);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddVoltageFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM,
	long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddVoltageFollower(OutP, OutM, InP, InM);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddCurrentFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM)
{
	USES_CIRCUIT;
	return pCircuit->AddCurrentFollower(OutP, OutM, InP, InM);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddCurrentFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM,
	const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddCurrentFollower(OutP, OutM, InP, InM);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddCurrentFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM,
	long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddCurrentFollower(OutP, OutM, InP, InM);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddDVCCII(long long hCircuit, const string& Ix, const string& Iy1, const string& Iy2, const string& Iz1, const string& Iz2)
{
	USES_CIRCUIT;
	if (!pCircuit->IsGlobal0Defined())
	{
		LastErrorInfo = "DVCCII needs global ref-node defined";
		return LACK_OF_REFNODE;
	}
	else
		return pCircuit->AddDVCCII(Ix, Iy1, Iy2, Iz1, Iz2);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddDVCCII(long long hCircuit, const string& Ix, const string& Iy1, const string& Iy2, const string& Iz1, const string& Iz2, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	if (!pSubCircuit->IsGlobal0Defined())
	{
		LastErrorInfo = "DVCCII needs global ref-node defined";
		return LACK_OF_REFNODE;
	}
	else
		return pSubCircuit->AddDVCCII(Ix, Iy1, Iy2, Iz1, Iz2);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddDVCCII(long long hCircuit, const string& Ix, const string& Iy1, const string& Iy2, const string& Iz1, const string& Iz2, long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	if (!pSubCircuit->IsGlobal0Defined())
	{
		LastErrorInfo = "DVCCII needs global ref-node defined";
		return LACK_OF_REFNODE;
	}
	else
		return pSubCircuit->AddDVCCII(Ix, Iy1, Iy2, Iz1, Iz2);
	FINISH_SUBCIRCUIT;
}


long CPDDEngineManager::AddFDCCII(long long hCircuit, const string& Vx1, const string& Vx2,
	const string& Vy1, const string& Vy2, const string& Vy3, const string& Vy4,
	const string& Iz1, const string& Iz2)
{
	USES_CIRCUIT;
	if (!pCircuit->IsGlobal0Defined())
	{
		LastErrorInfo = "FDVCCII needs global ref-node defined";
		return LACK_OF_REFNODE;
	}
	else
		return pCircuit->AddFDCCII(Vx1, Vx2, Vy1, Vy2, Vy3, Vy4, Iz1, Iz2);
	FINISH_SUBCIRCUIT;
}


long CPDDEngineManager::AddFDCCII(long long hCircuit, const string& Vx1, const string& Vx2,
	const string& Vy1, const string& Vy2, const string& Vy3, const string& Vy4,
	const string& Iz1, const string& Iz2, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	if (!pSubCircuit->IsGlobal0Defined())
	{
		LastErrorInfo = "FDVCCII needs global ref-node defined";
		return LACK_OF_REFNODE;
	}
	else
		return pSubCircuit->AddFDCCII(Vx1, Vx2, Vy1, Vy2, Vy3, Vy4, Iz1, Iz2);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddFDCCII(long long hCircuit, const string& Vx1, const string& Vx2,
	const string& Vy1, const string& Vy2, const string& Vy3, const string& Vy4,
	const string& Iz1, const string& Iz2, long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	if (!pSubCircuit->IsGlobal0Defined())
	{
		LastErrorInfo = "FDVCCII needs global ref-node defined";
		return LACK_OF_REFNODE;
	}
	else
		return pSubCircuit->AddFDCCII(Vx1, Vx2, Vy1, Vy2, Vy3, Vy4, Iz1, Iz2);
	FINISH_SUBCIRCUIT;
}


long CPDDEngineManager::AddOpAmp(long long hCircuit,const string& Out, const string& II, const string& NI)
{
	USES_CIRCUIT;
	return pCircuit->AddOpAmp(Out,II,NI);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddOpAmp(long long hCircuit,const string& Out, const string& II, const string& NI,const string& SubcircuitName)\
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddOpAmp(Out,II,NI);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddOpAmp(long long hCircuit,const string& Out, const string& II, const string& NI,long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddOpAmp(Out,II,NI);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddCurrentConveyor(long long hCircuit,const string& X, const string& Y, const string& Z, short Type)
{
	USES_CIRCUIT;
	return pCircuit->AddCurrentConveyor(X,Y,Z,Type);
	FINISH_SUBCIRCUIT
}

long CPDDEngineManager::AddCurrentConveyor(long long hCircuit,const string& X, const string& Y, const string& Z, short Type,const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddCurrentConveyor(X,Y,Z, Type);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddCurrentConveyor(long long hCircuit,const string& X, const string& Y, const string& Z, short Type,long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddCurrentConveyor(X,Y,Z, Type);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddBOCII(long long hCircuit, const string& X, const string& Y, const string& Zp, const string& Zm, short Type)
{
	USES_CIRCUIT;
	return pCircuit->AddBOCII(X, Y, Zp, Zm, Type);
	FINISH_SUBCIRCUIT
}

long CPDDEngineManager::AddBOCII(long long hCircuit, const string& X, const string& Y, const string& Zp, const string& Zm, short Type, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddBOCII(X, Y, Zp, Zm, Type);
	FINISH_SUBCIRCUIT;
}
long CPDDEngineManager::AddBOCII(long long hCircuit, const string& X, const string& Y, const string& Zp, const string& Zm, short Type, long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddBOCII(X, Y, Zp, Zm, Type);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddFDCFOA_RM_INF(long long hCircuit, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
	const string& Izp, const string& Izn, const string& Vwp, const string& Vwn)
{
	USES_CIRCUIT;
	return pCircuit->AddFDCFOA_RM_INF(Vyp,Vyn,Ixp,Ixn,Izp,Izn,Vwp,Vwn);
	FINISH_SUBCIRCUIT
}

long CPDDEngineManager::AddFDCFOA_RM_INF(long long hCircuit, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
	const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddFDCFOA_RM_INF(Vyp, Vyn, Ixp, Ixn, Izp, Izn, Vwp, Vwn);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddFDCFOA_RM_INF(long long hCircuit, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
	const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->AddFDCFOA_RM_INF(Vyp, Vyn, Ixp, Ixn, Izp, Izn, Vwp, Vwn);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddFDCFOA_RM(long long hCircuit, const string& name, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
	const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long double Value)
{
	USES_CIRCUIT;
	return pCircuit->AddFDCFOA_RM(name,Vyp, Vyn, Ixp, Ixn, Izp, Izn, Vwp, Vwn,Value);
	FINISH_SUBCIRCUIT
}

long CPDDEngineManager::AddFDCFOA_RM(long long hCircuit, const string& name, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
	const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long double Value, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->AddFDCFOA_RM(name, Vyp, Vyn, Ixp, Ixn, Izp, Izn, Vwp, Vwn, Value);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::AddFDCFOA_RM(long long hCircuit, const string& name, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
	const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long double Value, long SubcircuitId)
{
	USES_CIRCUIT;
	return pCircuit->AddFDCFOA_RM(name, Vyp, Vyn, Ixp, Ixn, Izp, Izn, Vwp, Vwn, Value);
	FINISH_SUBCIRCUIT
}

long CPDDEngineManager::IncludeSubcircuit(long long hCircuit, const string& InstanceName, const string& ModelName, const _CStrNodes& Connections, long InternalCofactor)
{
	USES_CIRCUIT;
	return pCircuit->IncludeSubcircuit(InstanceName,ModelName,Connections);
	FINISH_SUBCIRCUIT
}

long CPDDEngineManager::IncludeSubcircuit(long long hCircuit, const string& InstanceName, const string& ModelName, const string& SubcircuitName, const _CStrNodes& Connections, long InternalCofactor)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->IncludeSubcircuit(InstanceName,ModelName,Connections);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::IncludeSubcircuit(long long hCircuit, const string& InstanceName, const string& ModelName, long SubcircuitId, const _CStrNodes& Connections, long InternalCofactor)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->IncludeSubcircuit(InstanceName,ModelName,Connections);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::IncludeSubcircuit(long long hCircuit,const string& InstanceName, long ModelID, const _CStrNodes& Connections, long InternalCofactor)
{
	USES_CIRCUIT;
	return pCircuit->IncludeSubcircuit(InstanceName,ModelID,Connections, InternalCofactor);
	FINISH_SUBCIRCUIT
}

long CPDDEngineManager::IncludeSubcircuit(long long hCircuit, const string& InstanceName, long ModelID, const string& SubcircuitName, const _CStrNodes& Connections, long InternalCofactor)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->IncludeSubcircuit(InstanceName,ModelID,Connections, InternalCofactor);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::IncludeSubcircuit(long long hCircuit, const string& InstanceName, long ModelID, long SubcircuitId, const _CStrNodes& Connections, long InternalCofactor)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->IncludeSubcircuit(InstanceName,ModelID,Connections, InternalCofactor);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::ForceModelCalculation(long long hCircuit,long ModelID,bool IgnoreSgnPropagation)
{
	USES_CIRCUIT;
	return pCircuit->ForceModelCalculation(ModelID, IgnoreSgnPropagation);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::WriteModelsToTextFile(long long hCircuit, const string& FileName)
{
	USES_CIRCUIT;
	return pCircuit->WriteModelsToTextFile(FileName);
	FINISH_CIRCUIT;
}

long CPDDEngineManager::SetCompPriority(long long hCircuit, size_t Priority)
{
	USES_CIRCUIT;
	return pCircuit->SetCompPriority(Priority);
	FINISH_SUBCIRCUIT
}

long CPDDEngineManager::SetCompPriority(long long hCircuit, const string& SubcircuitName, size_t Priority)
{
	USES_SUBCIRCUIT_NAME;
	return pSubCircuit->SetCompPriority(Priority);
	FINISH_SUBCIRCUIT;
}

long CPDDEngineManager::SetCompPriority(long long hCircuit, long SubcircuitId, size_t Priority)
{
	USES_SUBCIRCUIT_ID;
	return pSubCircuit->SetCompPriority(Priority);
	FINISH_SUBCIRCUIT;
}

//long CPDDEngineManager::WriteCircuitToTextFile(long long hCircuit, const string& FileName, bool DevelopeOrdered, bool ComputeNumerical)
//{
//	USES_CIRCUIT;
//	return pCircuit->WriteCircuitToTextFile(FileName,DevelopeOrdered,ComputeNumerical);
//	FINISH_CIRCUIT;
//}

//long CPDDEngineManager::GetSPowerCoefficient(NumericType& Result, long long hCircuit, unsigned long CofId, unsigned short sPower,const string& Context)
//{
//	USES_CIRCUIT;
//	return pCircuit->GetSPowerCoefficient(CofId,sPower,Result,Context);
//	FINISH_CIRCUIT;
//}

long CPDDEngineManager::ModifyComponentParameter(long long hCircuit,const string& InstancePath, long double NewValue)
{
	USES_CIRCUIT;
	pCircuit->ModifySubParameter(InstancePath, NewValue);
	//pCircuit->ModifyParameter(InstancePath,NewValue);
	return 0;
	FINISH_CIRCUIT;
}

long CPDDEngineManager::ModifyComponentParameter(long long hCircuit, const string& InstancePath, long double NewValue, const string& SubcircuitName)
{
	USES_SUBCIRCUIT_NAME;
	pSubCircuit->ModifySubParameter(InstancePath, NewValue);
	return 0;
	FINISH_SUBCIRCUIT;
}
long CPDDEngineManager::ModifyComponentParameter(long long hCircuit, const string& InstancePath, long double NewValue, long SubcircuitId)
{
	USES_SUBCIRCUIT_ID;
	pSubCircuit->ModifySubParameter(InstancePath,NewValue);
	return 0;
	FINISH_SUBCIRCUIT;
}

//long CPDDEngineManager::GetNumericalCofactorToComponent(NumericType& Result, long long hCircuit, unsigned long CofId, unsigned short sPower, const string& ParameterPath)
//{
//	USES_CIRCUIT;
//	return pCircuit->GetNumericalCofactorToComponent(CofId,sPower,ParameterPath,Result);
//	FINISH_CIRCUIT;
//}

long CPDDEngineManager::GetDirectNumericalCofactorToComponent(_CSparsePolynomial* pResult,  
															  long long hCofactor, 
															  unsigned short sPower, 
															  const string& ParameterPath,
															  const string** pResContext,
															  const string& BaseContext)
{
	long long hCircuit=0;
	unsigned long CofId=0;
	long Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
	if(Status<0)
		return Status;
	USES_CIRCUIT;
	return pCircuit->GetDirectNumericalCofactorToComponent(CofId,sPower,ParameterPath,pResult,
		pResContext,BaseContext);
	FINISH_CIRCUIT;
}

long CPDDEngineManager::CreateCofactorToComponent(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& ParameterPath, const string& BaseContext, const string*& pOutputContext)
{
	USES_CIRCUIT;
	return pCircuit->CreateCofactorToComponent(CofId,sPower,ParameterPath, BaseContext, pOutputContext);
	FINISH_CIRCUIT;
}


long CPDDEngineManager::CreateReminderToComponent(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& ParameterPath, const string& BaseContext, const string*& pOutputContext)
{
	USES_CIRCUIT;
	return pCircuit->CreateReminderToComponent(CofId,sPower,ParameterPath, BaseContext, pOutputContext);
	FINISH_CIRCUIT;
}
long CPDDEngineManager::AppendToFileCoefVertices(long long hCircuit,const string& FileName, unsigned long CofId, unsigned short sPower, const string& ParameterPath)
{
	USES_CIRCUIT;
	return pCircuit->AppendToFileCoefVertices(FileName,CofId,sPower,ParameterPath);
	FINISH_CIRCUIT;
}

long CPDDEngineManager::GetFreePathTraitor(long long hCircuit,const string& BaseConetx)
{
	USES_CIRCUIT;
	return pCircuit->GetFreePathTraitor(BaseConetx);
	FINISH_CIRCUIT;
}

long CPDDEngineManager::AddParamReminder(long long hCircuit, long TrId, const string& ElementPath)
{
	USES_CIRCUIT;
	try
	{
		pCircuit->AddParamReminder(TrId,ElementPath);
		return 0;
	}
	catch(eMultipleDivRemOperation& e)
	{
		LastErrorInfo=e.what();
		return MULTIPL_DIV_REM_OPERATION;
	}
	catch(out_of_range& e)
	{
		LastErrorInfo=e.what();
		return INVALID_PATH_TRACE_ID;
	}
	FINISH_CIRCUIT;
}

long CPDDEngineManager::AddParamDivider(long long hCircuit, long TrId, const string& ElementPath)
{
	USES_CIRCUIT;
	try
	{
		pCircuit->AddParamDivider(TrId,ElementPath);
		return 0;
	}
	catch(eMultipleDivRemOperation& e)
	{
		LastErrorInfo=e.what();
		return MULTIPL_DIV_REM_OPERATION;
	}
	catch(out_of_range& e)
	{
		LastErrorInfo=e.what();
		return INVALID_PATH_TRACE_ID;
	}
	FINISH_CIRCUIT;
}

long CPDDEngineManager::SetApproximationCriterion(long long hCircuit,
												  long TrId,
												  bool Exact,
												  const NumericType& Accurracy,
												  const NumericType& HighFrequency,
												  const NumericType& LowFrequency,
												  short MaxPower,
												  short MinPower)
{
	USES_CIRCUIT;
	try
	{
		pCircuit->SetApproximationCriterion(TrId,
			Exact,
			Accurracy,
			HighFrequency,
			LowFrequency,
			MaxPower,
			MinPower);
		return 0;
	}
	catch(out_of_range& e)
	{
		LastErrorInfo=e.what();
		return INVALID_PATH_TRACE_ID;
	}
	FINISH_CIRCUIT;
}

long CPDDEngineManager::ReleasePathTraitor(long long hCircuit, long TrId)
{
	USES_CIRCUIT;
	try
	{
		return pCircuit->ReleasePathTraitor(TrId);
	}
	catch(out_of_range& e)
	{
		LastErrorInfo=e.what();
		return INVALID_PATH_TRACE_ID;
	}
	FINISH_CIRCUIT;
}


long CPDDEngineManager::ClearPathTraitor(long long hCircuit, long TrId)
{
	USES_CIRCUIT;
	try
	{
		return pCircuit->ClearPathTraitor(TrId);
	}
	catch(out_of_range& e)
	{
		LastErrorInfo=e.what();
		return INVALID_PATH_TRACE_ID;
	}
	FINISH_CIRCUIT;
}

long CPDDEngineManager::TestPathTraitor(long long hCircuit, long TrId)
{
	USES_CIRCUIT;
	return pCircuit->TestPathTraitor(TrId);
	FINISH_CIRCUIT;
}
//long CPDDEngineManager::FindTheLeastSignificant(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, long double* pNumericalResult, VECTOR_STRING* pResultPath,string* pStrResult)//returns shortest path Id or Error if <0
//{
//	USES_CIRCUIT;
//	return pCircuit->FindTheLeastSignificant(CofId,sPower,Context,pNumericalResult,pResultPath,pStrResult);
//	FINISH_CIRCUIT;
//}

//long CPDDEngineManager::GetTheLeastSignificantPath(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, NumericType& NumericalResult,_CLSPath& Path)
//{
//	USES_CIRCUIT;
//	return pCircuit->GetTheLeastSignificantPath(CofId,sPower,Context,NumericalResult,Path);
//	FINISH_CIRCUIT;
//}

//long CPDDEngineManager::GetTheLeastSignificantPathPtc(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, NumericType& NumericalResult,_CLSPath& Path)
//{
//	USES_CIRCUIT;
//	return pCircuit->GetTheLeastSignificantPathPtc(CofId,sPower,Context,NumericalResult,Path);
//	FINISH_CIRCUIT;
//}

long CPDDEngineManager::RemovePath(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, _CLSPath& Path, const string*& pOutputContext)
{
	USES_CIRCUIT;
	return pCircuit->RemovePath(CofId,sPower,Context,Path,pOutputContext);
	FINISH_CIRCUIT;
}

//long CPDDEngineManager::RemoveTheLeastSigmificantPath(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, NumericType& AbsolutDecrease, const string*& pOutputContext)
//{
//	USES_CIRCUIT;
//	return pCircuit->RemoveTheLeastSigmificantPath(CofId,sPower,Context,AbsolutDecrease,pOutputContext);
//	FINISH_CIRCUIT;
//}

//long CPDDEngineManager::RemoveTheLeastSigmificantPathPtc(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, NumericType& RelativePtcDecrease, const string*& pOutputContext)
//{
//	USES_CIRCUIT;
//	return pCircuit->RemoveTheLeastSigmificantPathPtc(CofId,sPower,Context,RelativePtcDecrease,pOutputContext);
//	FINISH_CIRCUIT;
//}

//long CPDDEngineManager::RemoveTheLeastSigmificantPaths(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, const NumericType& MaxDecreasePtc, NumericType& ResultDecreasePtc,const string*& pOutputContext,const char* FileToRegister, bool RegOnlyFinalResult)
//{
//	USES_CIRCUIT;
//	return pCircuit->RemoveTheLeastSigmificantPaths(CofId,sPower,Context,MaxDecreasePtc,ResultDecreasePtc,pOutputContext,FileToRegister,RegOnlyFinalResult);
//	FINISH_CIRCUIT;
//}

//long CPDDEngineManager::GetNumericalReminderToComponent(NumericType& Result, long long hCircuit, unsigned long CofId, unsigned short sPower, const string& ParameterPath)
//{
//	USES_CIRCUIT;
//	return pCircuit->GetNumericalReminderToComponent(CofId,sPower,ParameterPath,Result);
//	FINISH_CIRCUIT;
//}

long CPDDEngineManager::GetDirectNumericalReminderToComponent(_CSparsePolynomial* pResult,  
															  long long hCofactor, 
															  unsigned short sPower, 
															  const string& ParameterPath,
															  const string** pResContext,
															  const string& BaseContext)
{
	long long hCircuit=0;
	unsigned long CofId=0;
	long Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
	if(Status<0)
		return Status;
	USES_CIRCUIT;
	return pCircuit->GetDirectNumericalReminderToComponent(CofId,sPower,ParameterPath,pResult);
	FINISH_CIRCUIT;
}

long CPDDEngineManager::WriteDirectNmericalPolynomial(long long hCofactor, 
													  long hPathTraitor,
													  const string& FileName, 
													  bool Append, 
													  bool TestDistribution,
													  long double UpperAngFrequency)
{
	long long hCircuit=0;
	unsigned long CofId=0;
	long Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
	if(Status<0)
		return Status;
	USES_CIRCUIT;
	return pCircuit->WriteDirectNmericalPolynomial(CofId,hPathTraitor,FileName,Append,
		NULL,TestDistribution,UpperAngFrequency);
	FINISH_CIRCUIT;
}

//long CPDDEngineManager::GetNumericalCofactorAndReminder(_CSparsePolynomial*& pResult, long long hCofactor, long hPathTraitor)
//{
//	long long hCircuit=0;
//	unsigned long CofId=0;
//	long Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
//	if(Status<0)
//		return Status;
//	USES_CIRCUIT;
//	try
//	{
//		return pCircuit->GetNumericalCofactorAndReminder(CofId,hPathTraitor,pResult);
//	}
//	catch(out_of_range e)
//	{
//		LastErrorInfo=e.what();
//		return INVALID_PATH_TRACE_ID;
//	}
//	FINISH_CIRCUIT;
//}
long CPDDEngineManager::GetNumericalCofactorAndReminder(_CSparsePolynomial& Result, long long hCofactor, long hPathTraitor)
{
	long long hCircuit=0;
	unsigned long CofId=0;
	long Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
	if(Status<0)
		return Status;
	USES_CIRCUIT;
	try
	{
		return pCircuit->GetNumericalCofactorAndReminder(CofId,hPathTraitor,Result);
	}
	catch(out_of_range& e)
	{
		LastErrorInfo=e.what();
		return INVALID_PATH_TRACE_ID;
	}
	FINISH_CIRCUIT;
}

//long CPDDEngineManager::GetDirectNmericalPolynomial(long long hCircuit,unsigned long CofId, const string& Context, _CSPolynomial*& pOutPolynomial)
//long CPDDEngineManager::GetDirectNmericalPolynomial(long long hCofactor,const string& Context, _CSparsePolynomial*& pOutPolynomial)
//{
//	long long hCircuit=0;
//	unsigned long CofId=0;
//	long  Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
//	if(Status<0)
//		return Status;
//	USES_CIRCUIT;
//	return pCircuit->GetDirectNmericalPolynomial(CofId,Context,pOutPolynomial);
//	FINISH_CIRCUIT;
//}

long CPDDEngineManager::GetDirectNmericalPolynomial(long long hCofactor,const string& Context, _CSparsePolynomial& OutPolynomial)
{
	long long hCircuit=0;
	unsigned long CofId=0;
	long  Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
	if(Status<0)
		return Status;
	USES_CIRCUIT;
	return pCircuit->GetDirectNmericalPolynomial(CofId,Context,OutPolynomial);
	FINISH_CIRCUIT;
}

long CPDDEngineManager::GetDirectNmericalValue(long long hCofactor,unsigned short sPower, const string& Context, NumericType& Value)
{
	long long hCircuit=0;
	unsigned long CofId=0;
	long Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
	if(Status<0)
		return Status;
	USES_CIRCUIT;
	return pCircuit->GetDirectNmericalValue(CofId,sPower,Context,Value);
	FINISH_CIRCUIT;
	//_CSPolynomial* pOutPolynomial=NULL;
	//GetDirectNmericalPolynomial(hCircuit,CofId,Context,pOutPolynomial);
	//if(pOutPolynomial==NULL || pOutPolynomial->size()<=sPower)
	//	return -1;
	//Value=pOutPolynomial->at(sPower);
	//return 0;
}

long CPDDEngineManager::GetDirectNmericalPolynomialSize(long long hCofactor,const string& Context)
{
	long long hCircuit=0;
	unsigned long CofId=0;
	long Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
	if(Status<0)
		return Status;
	USES_CIRCUIT;
	return pCircuit->GetDirectNmericalPolynomialSize(CofId,Context);
	FINISH_CIRCUIT;
	//_CSPolynomial* pOutPolynomial=NULL;
	//GetDirectNmericalPolynomial(hCircuit,CofId,Context,pOutPolynomial);
	//if(pOutPolynomial==NULL)
	//	return -1;
	//return pOutPolynomial->size();
}
long CPDDEngineManager::WriteDirectNmericalPolynomial(long long hCofactor, 
													  const string& Context,
													  const string& FileName, 
													  bool Append, 
													  bool TestDistribution,
													  bool FindZeroes,
													  long double UpperAngFrequency)
{
	long long hCircuit=0;
	unsigned long CofId=0;
	long Status=hCofactor2CofId(hCofactor,CofId,hCircuit);
	if(Status<0)
		return Status;
	USES_CIRCUIT;
	return pCircuit->WriteDirectNmericalPolynomial(CofId,Context,FileName,Append,NULL,FindZeroes,TestDistribution,UpperAngFrequency);
	FINISH_CIRCUIT;
}


long CPDDEngineManager::WriteIntermediateNumericPolynomials(long long hCircuit, 
															const string& FileName,
															bool Append)
{
	USES_CIRCUIT;
	return pCircuit->WriteIntermediateNumericPolynomials(FileName,Append);
	FINISH_CIRCUIT;
}

long CPDDEngineManager::PerformFullCofRemTest(long long hCircuit, 
											  const string& ElementPath, 
											  const string& OutTexFileName, 
											  bool CheckResIntegrit, 
											  bool Append, 
											  bool TestDistribution,
											  long double UpperAngFrequency)
{
	USES_CIRCUIT;
	return pCircuit->PerformFullCofRemTest(ElementPath,OutTexFileName,CheckResIntegrit,Append,TestDistribution,UpperAngFrequency);
	FINISH_CIRCUIT;
}

long CPDDEngineManager::SetGlobalModelFileName(long long hCircuit, const string& Name, bool ClearOld)
{
	USES_CIRCUIT;
	pCircuit->SetGlobalModelFileName(Name,ClearOld);
	return 0;
	FINISH_CIRCUIT;
}

long CPDDEngineManager::SetGlobalCircuitFileName(long long hCircuit, const string& Name,bool ClearOld)
{
	USES_CIRCUIT;
	pCircuit->SetGlobalCircuitFileName(Name,ClearOld);
	return 0;
	FINISH_CIRCUIT;
}

long CPDDEngineManager::SetGlobalRaportFileName(long long hCircuit, const string& Name, bool ClearOld)
{
	USES_CIRCUIT;
	pCircuit->SetGlobalRaportFileName(Name,ClearOld);
	return 0;
	FINISH_CIRCUIT;
}

void CPDDEngineManager::CRemover::RemoveAt(_CPDDEngineInstance*& pEngine)
{
	if(pEngine==NULL) return;
	delete pEngine->second;
	delete pEngine;
}

void CPDDEngineManager::CInstanceRemover::operator() (_CPDDEngineInstance*& pEngine)
{
	if(pEngine!=NULL && pEngine->first==m_hAppInstance)
	{
		RemoveAt(pEngine);
		pEngine=NULL;
		m_Counter++;
	}
}

_CNetDynamicAnalyser* CPDDEngineManager::GetTestAnalyser(string NetName, bool BySubcircuit,string ReferenceNode)
{
	long hEngine=GetPDDEngine(0,true,false,true);
	_CPDDEngine& Engine=*m_Engines[hEngine-1]->second;
	return Engine.GetNetAnalyser(Engine.CreateNewNetAnalyser(NetName,BySubcircuit,ReferenceNode));
}

_CUnlineraBlackBoxEngine* CPDDEngineManager::GetNewUnlinearBlackBox()
{
	long hEngine = GetPDDEngine(0, true, false, true);
	_CPDDEngine& Engine = *m_Engines[hEngine - 1]->second;
	return &Engine.BlackBoxEngine();
}