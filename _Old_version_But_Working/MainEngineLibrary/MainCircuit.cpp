#include "StdAfx.h"
#include "MainCircuit.h"
#include "..\PDDErrorsAndConst.h"
#include <algorithm>
#include "PDDEngine.h"
#include "PrjException.h"
#include <fstream>
#include "SubCircuitSocket.h"
#include "Vertex.h"
#include <iomanip>
#include "CSimpleVertexContainer.h"
#include "ComponentPath.h"
#include <math.h>
#include "time_stamp.h"
#include <stdlib.h>
#include "SimplifierEngine.h"

long WrongCofIdError(long Id)
{
	char Buffer[20];
	sprintf_s(Buffer,sizeof(Buffer),"Cofactor %ld out of Range",Id);
	LastErrorInfo=string(Buffer);
	return WRONG_COFACTOR_ID;
}
long WrongContextName(const string& Name)
{
	LastErrorInfo=(string("Wrong Context Name ")+Name);
	return WRONG_CONTEXT_NAME;
}

//*************************_CMainCircuit
_CMainCircuit::_CMainCircuit(_CPDDEngine* MyEngine, bool OrderOptimalization,bool UseNewAnalyser):_CCircuit(false,OrderOptimalization,UseNewAnalyser), 
m_SubcircuitCounter(1),m_pEngine(MyEngine),m_FlatResultFactorizer(m_CompPathStorage,m_FlatVertexCache)
{
	m_ConstPathStore.SetBaseCircuit(this);
	m_PathStore.SetBaseCircuit(this);
	m_CompPathStorage.m_pMainCircuit=this;
	m_FlatVertexCache.m_pMainCircuit=this;
	m_ContextSExpFlatVertices.m_pMainCircuit=this;
}

_CMainCircuit::~_CMainCircuit(void)
{
	for(SUBCIRCUIT_RITERATOR it=m_SubcircuitMap.rbegin();it!=m_SubcircuitMap.rend();it++)
		delete (*it).second.second;
	//for(SUBCIRCUIT_ITERATOR it=m_SubcircuitMap.begin();it!=m_SubcircuitMap.end();it++)
	//	delete (*it).second.second;
	//for(COFACTORS_IT it=m_Cofactors.begin();it!=m_Cofactors.end();it++)
	//	if((*it)!=NULL)
	//		delete (*it);
}

void _CMainCircuit::SetOptimizeComponentOrder(bool Optimize)
{
	_CCircuit::SetOptimizeComponentOrder(Optimize);
	for(SUBCIRCUIT_ITERATOR it=m_SubcircuitMap.begin();it!=m_SubcircuitMap.end();it++)
		it->second.second->SetOptimizeComponentOrder(Optimize);
}

_CSubCircuit* _CMainCircuit::DefineNewNetDynSubcircuit(const string& name,bool ForceNewIfExist)
{
	SUBCIRCUIT_ITERATOR it=m_SubcircuitMap.find(name);
	if(it!=m_SubcircuitMap.end())
	{
		string Err=("The subcircuit ")+name+" has already been defined";
		RISEPDD(eNameDuplicated,Err.c_str());
	}
	_CSubCircuit* pSubCircuit=new _CSubCircuit(this,name,m_IsGlobal0Node?TranslateNode(0):EmptyString,true,m_UseNewAnalyser);
	if(!ForceNewIfExist)
		if(pSubCircuit->IsModelInFile())
		{
			delete pSubCircuit;
			string Err=string("The model: ")+name+" had been already defined\n"+
							  "\t1. Change the name, or\n"
							  "\t2. Use \"ForceNewIfExist\" option to overwrite the old version";
			RISEPDD(eMoreThanOneModelFile,Err.c_str());
		}
	m_SubcircuitMap.insert(SUBCIRCUIT_VALUE_TYPE(name,SUBCIRCUIT_DATA(m_SubcircuitCounter,pSubCircuit)));
	m_IndecesMap.insert(INDECES_VALUE_TYPE(m_SubcircuitCounter,name));
	m_SubcircuitCounter++;
	return pSubCircuit;
}

//long _CMainCircuit::DefineNewSubcircuit(const string& name, const _CStrNodes& BoundNodes, const SIZE_T_SET* pDeletionAdmissible, bool ForceNewIfExist)
long _CMainCircuit::DefineNewSubcircuit(const string& name, const _CStrNodes& BoundNodes, const SIZE_T_RANGE* pDeletionAdmissible, bool ForceNewIfExist)
{
	SUBCIRCUIT_ITERATOR it=m_SubcircuitMap.find(name);
	if(it!=m_SubcircuitMap.end())
	{
		LastErrorInfo=string("The subcircuit ")+name+" has already been defined";
		return SUBCIRCUIT_ALLREADY_EXISTS;
	}
	try
	{
		_CSubCircuit* pSubCircuit=new _CSubCircuit(this,m_OptimizeComponentOrder,m_UseNewAnalyser,name,BoundNodes,m_IsGlobal0Node?TranslateNode(0):EmptyString,true,pDeletionAdmissible);
		if(!ForceNewIfExist)
			try
			{
				if(pSubCircuit->IsModelInFile())
				{
					//delete pSubCircuit;
					//LastErrorInfo=string("The model: ")+name+" had been already defined\n"+
					//					 "\t1. Change the name, or\n"
					//					 "\t2. Use \"ForceNewIfExist\" option to overwrite the old version";
					//return SUBCIRCUIT_ALLREADY_EXISTS;
					pSubCircuit->LoadModel();
				}
			}
			catch(eMoreThanOneModelFile& e)
			{
				delete pSubCircuit;
				LastErrorInfo=e.what();
				return MORE_THAN_ONE_MODEL_EXISTS;
			}
		m_SubcircuitMap.insert(SUBCIRCUIT_VALUE_TYPE(name,SUBCIRCUIT_DATA(m_SubcircuitCounter,pSubCircuit)));
		m_IndecesMap.insert(INDECES_VALUE_TYPE(m_SubcircuitCounter,name));
		return m_SubcircuitCounter++;
	}
	catch(bad_alloc e)
	{
		LastErrorInfo=string(e.what())+" in _CMainCircuit::DefineNewSubcircuite";
		return ALLOCATION_ERROR;
	}
	return 0;
}
//long _CMainCircuit::TryToGetSubcircuitFromLibrary(const string& name, const _CStrNodes& BoundNodes,const SIZE_T_SET* pDeletionAdmissible)
long _CMainCircuit::TryToGetSubcircuitFromLibrary(const string& name, const _CStrNodes& BoundNodes,const SIZE_T_RANGE* pDeletionAdmissible)
{
	SUBCIRCUIT_ITERATOR it=m_SubcircuitMap.find(name);
	if(it!=m_SubcircuitMap.end())
	{
		LastErrorInfo=string("The subcircuit ")+name+" has already been defined";
		return SUBCIRCUIT_ALLREADY_EXISTS;
	}
	try
	{
		_CSubCircuit* pSubCircuit=new _CSubCircuit(this,m_OptimizeComponentOrder,m_UseNewAnalyser,name,BoundNodes,m_IsGlobal0Node?TranslateNode(0):EmptyString,false,pDeletionAdmissible);
		try
		{
			if(!pSubCircuit->IsModelInFile())
			{
				LastErrorInfo=string("The file: ")+_PLAIN_STR(pSubCircuit->GetSubcircuitModelPath())+" does not exists!";
				delete pSubCircuit;
				return SUBCIRCUIT_DOES_NOT_EXIST;
			}
			pSubCircuit->LoadModel();
		}
		catch(eMoreThanOneModelFile& e)
		{
			delete pSubCircuit;
			LastErrorInfo=e.what();
			return MORE_THAN_ONE_MODEL_EXISTS;
		}
		catch(eFilerProblem)
		{
			delete pSubCircuit;
			LastErrorInfo=string("File: ")+_PLAIN_STR(pSubCircuit->GetSubcircuitModelPath())+" is invalid.";
			return WRONG_FILE_FORMAT;
		}
		m_SubcircuitMap.insert(SUBCIRCUIT_VALUE_TYPE(name,SUBCIRCUIT_DATA(m_SubcircuitCounter,pSubCircuit)));
		m_IndecesMap.insert(INDECES_VALUE_TYPE(m_SubcircuitCounter,name));
		return m_SubcircuitCounter++;
	}
	catch(bad_alloc& e)
	{
		LastErrorInfo=string(e.what())+" in _CMainCircuit::DefineNewSubcircuite";
		return ALLOCATION_ERROR;
	}
	return 0;
}

long _CMainCircuit::DeclareGlobalRefNode(const string& sRefNode)
{
	
	if(!m_IdealComponentsList.empty() || !m_LocalComponentsList.empty() || m_SubcircuitCounter>1 || TranslateOrAddNode(sRefNode)!=0)
	{
		LastErrorInfo="Global reference node can be only declared before any stucture in the circuit (component, subcircuit, cofactor)";
		return GLOBAL_REFNODE_TOO_LATE;
	}
	m_IsGlobal0Node=true;
	return 0;
}

_CSubCircuit* _CMainCircuit::GetSubcircuit(const string& name)
{
	SUBCIRCUIT_ITERATOR it=m_SubcircuitMap.find(name);
	if(it==m_SubcircuitMap.end())
		return NULL;
	return it->second.second;
}

_CSubCircuit* _CMainCircuit::GetSubcircuit(long Id)
{
	INDECES_ITERATOR it=m_IndecesMap.find(Id);
	if(it==m_IndecesMap.end())
		return NULL;
	SUBCIRCUIT_MAP::iterator it2=m_SubcircuitMap.find(it->second);
	if(it2==m_SubcircuitMap.end())
		return NULL;
	else
		return it2->second.second;
}

long _CMainCircuit::FindSubId(const string& Name)
{
	if(Name=="main")
		return 0;
	SUBCIRCUIT_ITERATOR it=m_SubcircuitMap.find(Name);
	if(it==m_SubcircuitMap.end())
		return NULL;
	return it->second.first;
}

long _CMainCircuit::GetCircuitId(const _CCircuit* pCircuit)
{
	if (pCircuit == NULL)
		RISEPDD(eWrongCondition, "Invalid Cicircuit ref");
	if(pCircuit==this)
		return 0;
	return FindSubId(pCircuit->GetName());
}

_CCircuit* _CMainCircuit::GetCircuit(long Id)
{
	if(Id==0)
		return this;
	return GetSubcircuit(Id);
}
const char* CI="Cofactor Id is invalid";
_CCofactor* _CMainCircuit::GetCofactor(unsigned long Id)
{
	if(Id==0 || Id>m_Cofactors.size())
		RISEPDD(out_of_range,CI);
	_CCofactor* pRes=m_Cofactors[Id-1];
	if(pRes==NULL)
		RISEPDD(out_of_range,CI);
	return pRes;
}

void _CMainCircuit::DoTheCalculations()
{
//	CheckModelRecurrention();
	time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Preparing of main results");
	if(!m_UseNewAnalyser)
		PrepareCofactors();
	CreateModelPDD();
	if(!m_NewCreatedVertices.empty())
	{
		RISEPDD(eIllegalOperation,"Expectef 0 and 1 terminal only");	
	}
	ClearGarbage();
}

//void _CMainCircuit::CheckModelRecurrention()
//{
//	_CRecurrentChecker Checker;
//	CheckRecurrentPath(Checker);
//}

const path_str& _CMainCircuit::GetModelsPath() 
{
	return m_pEngine->GetModelsPath();
}

_CSubCircuit* _CMainCircuit::PrepareSubcircuitFromName(const string& Name, const path_str& ModelFile)
{
	SUBCIRCUIT_ITERATOR it=m_SubcircuitMap.find(Name);
	if(it!=m_SubcircuitMap.end())
		return it->second.second;
	_CSubCircuit* pNewSub=NULL;
	try
	{
		pNewSub=new _CSubCircuit(this,m_OptimizeComponentOrder,m_UseNewAnalyser,Name,GetModelsPath()+_T("\\")+ModelFile,
			m_IsGlobal0Node?TranslateNode(0):EmptyString,false);
		if(pNewSub==NULL)
		return NULL;
		if(!pNewSub->IsModelInFile())
		{
			string Err=string("The file: ")+_PLAIN_STR(pNewSub->GetSubcircuitModelPath())+" does not exists!";
			delete pNewSub;
			RISEPDD(eIllegalModelName, Err.c_str());
		}
		pNewSub->LoadModel();
	}
	catch(eMoreThanOneModelFile)
	{
		delete pNewSub;
		throw;
	}
	catch(eFilerProblem)
	{
		delete pNewSub;
		string Err=string("File: ")+_PLAIN_STR(pNewSub->GetSubcircuitModelPath())+" is invalid.";
		RISEPDD(eFilerProblem,Err.c_str());
	}
	m_SubcircuitMap.insert(SUBCIRCUIT_VALUE_TYPE(Name,SUBCIRCUIT_DATA(m_SubcircuitCounter,pNewSub)));
	m_IndecesMap.insert(INDECES_VALUE_TYPE(m_SubcircuitCounter,Name));
	m_SubcircuitCounter++;
	return pNewSub;
}

void _CMainCircuit::GetOuterVertex(_CSExpandedVertices*& pVertices, _CModelVertex* pMVertex, _CSExpandedDescendends& TerminalVertices)
{
	RISEPDD(eIllegalOperation,"_CMainCircuit::GetOuterVertex is a dummy one and should never be called");
}
size_t _CMainCircuit::GetOuterVertexIndex(_CModelVertex* pMVertex)
{
	RISEPDD(eIllegalOperation,"_CMainCircuit::GetOuterVertexIndex is a dummy one and should never be called");
}
short _CMainCircuit::GetOuterVertexSign(_CModelVertex* pMVertex)
{
	RISEPDD(eIllegalOperation,"_CMainCircuit::GetOuterVertexSign is a dummy one and should never be called");
}

void _CMainCircuit::DetermineSimplePDD(bool sCExpanded, bool Approximated, const NumericType& Accuracy)
{
	time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Flat sExpanded form determination ");
	for(unsigned i=0;i<m_Cofactors.size();i++)
	{
		_CCofactor& Cof=*m_Cofactors[i];
		//const string& SDsc=(const string&)Cof;
		_CModelVertex* pMainVertex=Cof.GetMainVertex();
		_CSExpandedVertices& CofSVertices=GetMainSVertices(i,true);
		_CSExpandedDescendends Empty/*(pMainVertex)*/;
		_CSExpandedVertices* pCofSVertices=NULL;
		short AuxSgn=1;//Zapamiêtaæ gdzieœ
		_CDescSigns EmptySgns;
		pMainVertex->GetSimpleVertex(pCofSVertices,i,EmptyString,Empty,AuxSgn,EmptySgns,Approximated,Accuracy);
		CofSVertices=*pCofSVertices;
	}
	ClearDevelopingVerticesCache();
	m_SExpandedUniqueSet.clear();
}

//void _CMainCircuit::TestFlatPDDRemoval(long TrId)
//{
//	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
//	srand( (unsigned)time( NULL ));
//	size_t CofId=((size_t)rand()%m_Cofactors.size());
//	_CSExpFlatVerticesFactorized& CofFlatVertices=m_ContextSExpFlatVertices.GetSExpFlatResVertices(CofId,&Traitor.StrApproxContext());
//	size_t sPower=((size_t)rand()%CofFlatVertices.size());
//	const _CFactorizedVertexContainer* pCont=CofFlatVertices.sPowerCont[sPower];
//	while(pCont==NULL)
//	{
//		sPower=((size_t)rand()%CofFlatVertices.size());
//		pCont=CofFlatVertices.sPowerCont[sPower];
//	}
//	const _CFlatVertex* pVertex=pCont->m_List.front();
//	CofFlatVertices.sPowerCont[sPower]=NULL;
//	//m_FlatVertexCache.ReleaseAndKillVertex(pVertex);
//	m_FlatVertexCache.ReleaseVertex(pVertex);
//	m_FlatVertexCache.ClearReleased();
//}

//#define _OLD_VERSION

void _CMainCircuit::DetermineFlatSimplifiedPDD(long TrId, bool TryToFactorize)
{
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	if(m_ContextSExpFlatVertices.IsContextDetermined(&Traitor.StrApproxContext()))
		return;
	_CModelVerticesPath VerticesPath(this);
#ifdef _OLD_VERSION
	bool WasNew=false;
	typedef list<size_t> NEW_LIST;
	NEW_LIST NewList;
	for(size_t i=0;i<m_Cofactors.size();i++)
	{
		_CSparsePolynomial* pNumResult=NULL;
		GetNumericalCofactorAndReminder(i+1,TrId,pNumResult);
		_CSPowerLimiter Limiter(pNumResult,Traitor.GetApproxCriterion());
		_CCofactor& Cof=*m_Cofactors[i];
		_CModelVertex* pMainVertex=Cof.GetMainVertex();
		_CSExpFlatVerticesFactorized& CofFlatVertices=m_ContextSExpFlatVertices.GetSExpFlatResVertices(i,&Traitor.StrApproxContext());
		if(!CofFlatVertices.empty())
			continue;
		CofFlatVertices.SetOwner(this);
		WasNew=true;
		m_FlatVertexCache.SwitchCofactor(i+1);
		string Header("Flat simplified determination of cofactor No. ");
		Header+=ulong2str(i);
		time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,Header);
#ifdef _SIMP1
		_CSExpFlatVertices* pFlarVertices=NULL;
		_CMultLevelSExpFlatVertices EmptyTerminalVertices;
		_CFlatVerticesResContainerWrapper Wraper;
		pMainVertex->GetSimpleVertexApprox(pFlarVertices,Wraper,*pNumResult,Limiter,OnePattern,EmptyTerminalVertices,/*AuxSgn,EmptySgns,*/Traitor,VerticesPath);
		CofFlatVertices=_CSExpFlatVerticesFactorized(*pFlarVertices,this);
#endif
#ifdef _SIMP2
		for(_CSPowerLimiter::SET::iterator it=Limiter.m_Set.begin(),_e=Limiter.m_Set.end();it!=_e;it++)
		{
			unsigned short Power=*it;
			const _CFlatVertex* pResultVertex=NULL;
			short ResultSgn=1;
			NumericType Accuracy=pNumResult->get(Power)*Traitor.GetApproxCriterion()->GetAccurracy();
			pMainVertex->GetSimpleVertexApprox2(pResultVertex,ResultSgn,Power,Accuracy,Traitor,VerticesPath);
			CofFlatVertices.putFlatCont(Power,&_CFactorizedVertexContainer(ResultSgn,pResultVertex,this));
		}
#endif
		m_FlatVertexCache.TouchVertices(CofFlatVertices);
		NewList.push_back(i);
	}//Nowe
	if(WasNew && TryToFactorize)
	{
		string Header("Factorization of cofactor No. ");
		//m_FlatVertexCache.PrepareDividersPaths();
		for(NEW_LIST::iterator it=NewList.begin();it!=NewList.end();it++)
		{
			size_t NewId=*it;
			_CSExpFlatVerticesFactorized& Vertices2Factorize=
				m_ContextSExpFlatVertices.GetSExpFlatResVertices(NewId,&Traitor.StrApproxContext());
			for(size_t s=0;s<=Vertices2Factorize.size();s++)
			{
				time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,Header+ulong2str(NewId)+" s power: "+ulong2str(s));
				_CFactorizedVertexContainer* pCont=Vertices2Factorize.sPowerCont[s];
				if(pCont==NULL) continue;
				m_FlatVertexCache.FactorizeMe2(*pCont);
			}
		}
	}
	//m_FlatVertexCache.Clear();
	//if(WasNew)
	//{
	//	m_FlatVertexCache.SwitchCofactor(0);
	//	m_FlatVertexCache.LeaveZeroDesc();
	//}
	m_FlatVertexCache.ClearReleased();
	m_FlatVericesResCache.Clear();
#else
	_CNewSimplifierData Data(this, m_ContextSExpFlatVertices, Traitor);
	for(size_t i=0;i<m_Cofactors.size();i++)
	{
		size_t CofId=i+1;
		_CSparsePolynomial NumResult;
		GetNumericalCofactorAndReminder(CofId,TrId,NumResult);
		_CSPowerLimiter Limiter(&NumResult,Traitor.GetApproxCriterion(),MaxPower());
		_CCofactor& Cof=*m_Cofactors[i];
		_CModelVertex* pMainVertex=Cof.GetMainVertex();
		SHORT_SET& PowerSet=Limiter.m_Set;
		for(SHORT_SET::iterator it=PowerSet.begin(),_e=PowerSet.end();it!=_e;it++)
			Data.RegisterEntry(CofId,*it,pMainVertex,NumResult.get(*it),Traitor.GetApproxCriterion()->GetAccurracy(),Cof.GetGlobalMultiplier());
	}
	time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"FlatSimpolified determination");
 	Data.FindSimplifications(m_CirCompRedefEntry);
	if(TryToFactorize)
	{
		time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Factorisation of all");
#ifdef _FACT1
		string Header("Factorization of cofactor No. ");
		//m_FlatVertexCache.PrepareDividersPaths();
		for(size_t i=0;i<m_Cofactors.size();i++)
		{
			size_t NewId=i/*+1*/;
			_CSExpFlatVerticesFactorized& Vertices2Factorize=
				m_ContextSExpFlatVertices.GetSExpFlatResVertices(NewId,&Traitor.StrApproxContext());
			for(size_t s=0;s<=Vertices2Factorize.size();s++)
			{
				time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,Header+ulong2str(NewId)+" s power: "+ulong2str(s));
				_CFactorizedVertexContainer* pCont=Vertices2Factorize.sPowerCont[s];
				if(pCont==NULL) continue;
				m_FlatVertexCache.FactorizeMe2(*pCont);
			}
		}
		m_FlatVertexCache.ClearFeasDiv();
		m_FlatVertexCache.ClearReleased();
		m_FlatVertexCache.ClearOperCache();
#endif
#ifdef _FACT2
		m_FlatVertexCache.FactorizeAll(m_Cofactors.size(),m_ContextSExpFlatVertices,&Traitor.StrApproxContext());
#endif
#ifdef _FACT3
		//_CFlatResultFactorizer Factorizer(m_CompPathStorage);
		m_FlatResultFactorizer.Init();
		string Header("Factorization of cofactor No. ");
		//m_FlatVertexCache.PrepareDividersPaths();
		for(size_t i=0;i<m_Cofactors.size();i++)
		{
			size_t NewId=i/*+1*/;
			_CSExpFlatVerticesFactorized& Vertices2Factorize=
				m_ContextSExpFlatVertices.GetSExpFlatResVertices(NewId,&Traitor.StrApproxContext());
			for(size_t s=0;s<=Vertices2Factorize.size();s++)
			{
				time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,Header+ulong2str(NewId)+" s power: "+ulong2str(s));
				_CFactorizedVertexContainer* pCont=Vertices2Factorize.sPowerCont[s];
				if(pCont==NULL) continue;
				//SIGNED_TERM ResTerm=Factorizer.GetFactorizedValue(pCont->GetNFVertx());
				m_FlatResultFactorizer.NotifyFactorizedValue(i,s,pCont->GetNFVertx(),pCont->Sgn);
				//m_FlatVertexCache.FactorizeMe2(*pCont);
			}
		}
		m_FlatResultFactorizer.PrepareTrasferFuctions();
		m_FlatResultFactorizer.ClearGarbage();
#endif
	}
	//m_FlatVericesResCache.Clear();
#endif
}
void _CMainCircuit::PrepareAndWriteFlatSimplifiedPDDDiagrams(long TrId, bool Factorize,const string& FileName, bool Append)
{
	DetermineFlatSimplifiedPDD(TrId,Factorize);
	time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Flat vertices report");
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	const _CApproxCriterion& AppCrit=*Traitor.GetApproxCriterion();
	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
		Append?ios::out | ios::app : ios::out);
	if(Append) file<<'\n';
	file<<AppCrit;
	m_FlatVertexCache.ToStream(file);
	file<<endl;
}
void _CMainCircuit::WriteFactorizedPDDList(long TrId,const string& FileName, bool Append)
{
	DetermineFlatSimplifiedPDD(TrId,true);
	time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Factorization report");
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	const _CApproxCriterion& AppCrit=*Traitor.GetApproxCriterion();
	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
		Append?ios::out | ios::app : ios::out);
	if(Append) file<<'\n';
	file<<"====Factorized====="<<endl;
	file<<AppCrit;
	m_FlatResultFactorizer.SoE2Stream(file);
	file<<endl;
	file<<"====Factorized====="<<endl;
}
bool _CMainCircuit::WriteFlatSimplifiedPDDEntry(iostream& Stream, long TrId, size_t CofId)
{
	CofId--;
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	if(!m_ContextSExpFlatVertices.IsContextDetermined(&Traitor.StrApproxContext()))
		return false;
	//_CCofactor& Cof=*m_Cofactors[CofId];
	//_CModelVertex* pMainVertex=Cof.GetMainVertex();
	_CSExpFlatVerticesFactorized& CofFlatVertices=m_ContextSExpFlatVertices.GetSExpFlatResVertices(CofId,&Traitor.StrApproxContext());
	//Cof.WriteCofactorDescription(file);
	//file<<" = "<<endl;
	CofFlatVertices.ToStream(Stream,m_FlatVertexCache);
	//Stream<<endl;
	return true;
}
void _CMainCircuit::WriteFlatSimplifiedPDD(long TrId, bool TryToFactorize,bool Write1Level,const string& FileName, bool Append)
{
	DetermineFlatSimplifiedPDD(TrId,TryToFactorize);
	time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Flat vertices report");
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	const _CApproxCriterion& AppCrit=*Traitor.GetApproxCriterion();
	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
		Append?ios::out | ios::app : ios::out);
	if(Append) file<<'\n';
	file<<AppCrit;
	for(unsigned i=0;i<m_Cofactors.size();i++)
	{
		_CCofactor& Cof=*m_Cofactors[i];
		_CModelVertex* pMainVertex=Cof.GetMainVertex();
		_CSExpFlatVerticesFactorized& CofFlatVertices=m_ContextSExpFlatVertices.GetSExpFlatResVertices(i,&Traitor.StrApproxContext());
		Cof.WriteCofactorDescription(file);
		file<<" = "<<endl;
		CofFlatVertices.ToStream(file,m_FlatVertexCache);
		file<<endl;
		//file<<"Potentially to reduce:"<<endl;
		//m_FlatVertexCache.PotSimpToStream(file,i);
		//file<<endl;
	}
	m_FlatVertexCache.ToStream(file);
	file<<endl;
	if(TryToFactorize)
	{
		m_FlatResultFactorizer.SoE2Stream(file);
		for(unsigned i=0;i<m_Cofactors.size();i++)
		{
			_CCofactor& Cof=*m_Cofactors[i];
			_CModelVertex* pMainVertex=Cof.GetMainVertex();
			_CSExpFlatVerticesFactorized& CofFlatVertices=m_ContextSExpFlatVertices.GetSExpFlatResVertices(i,&Traitor.StrApproxContext());
			Cof.WriteCofactorDescription(file);
			file<<" = ";
			bool first=true;
			for(size_t s=CofFlatVertices.size()+1;s!=0;s--)
			{
				//_CFlatResultFactorizer::SIGNED_ID Entry=m_FlatResultFactorizer.GetValueId(i,s-1);
				SIGNED_TERM Entry=m_FlatResultFactorizer.GetTerm(i,s-1);
				if(Entry.first==0 && Entry.second==NULL)
					continue;
				bool Signed=false;
				short Sgn=Entry.first;
				if(Sgn<0)
				{
					file<<" - ";
					Signed=true;
					Sgn=abs(Sgn);
				}
				if(Sgn!=1)
				{
					file<<'('<<Sgn<<")*";
					Signed=true;
				}
				if(first)
					first=false;
				else if(!Signed)
					file<<" + ";
				if(s>1)
					file<<"s^"<<s-1<<"*";
				//if(Entry.first!=1)
				//	file<<'('<<Entry.first<<")*";
				//Entry.second->SubEntry2Stream(
				m_FlatResultFactorizer.SubEntry2Stream(Entry.second,file);
				//file<<'X'<<Entry.second;
			}
			file<<endl;
		}
		if(Write1Level)
			m_FlatResultFactorizer.Regular2Stream(file);
		m_FlatResultFactorizer.WriteTransferFunctions(file);
	}
}
size_t _CMainCircuit::GetPowerOfCofFlatVertices(long TrId,long CofId)
{
	CofId--;
	ASSERTPDD(CofId>=0 && CofId<(long)m_Cofactors.size());
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	_CSExpFlatVerticesFactorized& CofFlatVertices=m_ContextSExpFlatVertices.GetSExpFlatResVertices(CofId,&Traitor.StrApproxContext());
	return CofFlatVertices.size();
}
void _CMainCircuit::FactorizedCofFlat1LevelVertices2Stream(iostream& Stream,long TrId,long CofId)
{
	bool first=true;
	for(size_t s=GetPowerOfCofFlatVertices(TrId,CofId)+1;s!=0;s--)
	{
		//_CFlatResultFactorizer::SIGNED_ID Entry=m_FlatResultFactorizer.GetValueId(i,s-1);
		SIGNED_TERM Entry=m_FlatResultFactorizer.GetTerm(CofId-1,s-1);
		if(Entry.first==0 && Entry.second==NULL)
			continue;
		bool Signed=false;
		short Sgn=Entry.first;
		if(Sgn<0)
		{
			Stream<<" - ";
			Signed=true;
			Sgn=abs(Sgn);
		}
		if(Sgn!=1)
		{
			Stream<<'('<<Sgn<<")*";
			Signed=true;
		}
		if(first)
			first=false;
		else if(!Signed)
			Stream<<" + ";
		if(s>1)
			Stream<<"s^"<<s-1<<"*";
		//if(Entry.first!=1)
		//	file<<'('<<Entry.first<<")*";
		//Entry.second->SubEntry2Stream(
		m_FlatResultFactorizer.SubEntry2Stream1Level(Entry.second,Stream);
		//file<<'X'<<Entry.second;
	}
	if(first)
		Stream<<"0";
	Stream<<endl;
}

void _CMainCircuit::FactorizedCofFlatVertices2Stream(iostream& Stream,long TrId,long CofId)
{
	bool first=true;
	for(size_t s=GetPowerOfCofFlatVertices(TrId,CofId)+1;s!=0;s--)
	{
		//_CFlatResultFactorizer::SIGNED_ID Entry=m_FlatResultFactorizer.GetValueId(i,s-1);
		SIGNED_TERM Entry=m_FlatResultFactorizer.GetTerm(CofId-1,s-1);
		if(Entry.first==0 && Entry.second==NULL)
			continue;
		bool Signed=false;
		short Sgn=Entry.first;
		if(Sgn<0)
		{
			Stream<<" - ";
			Signed=true;
			Sgn=abs(Sgn);
		}
		if(Sgn!=1)
		{
			Stream<<'('<<Sgn<<")*";
			Signed=true;
		}
		if(first)
			first=false;
		else if(!Signed)
			Stream<<" + ";
		if(s>1)
			Stream<<"s^"<<s-1<<"*";
		//if(Entry.first!=1)
		//	file<<'('<<Entry.first<<")*";
		//Entry.second->SubEntry2Stream(
		m_FlatResultFactorizer.SubEntry2Stream(Entry.second,Stream);
		//file<<'X'<<Entry.second;
	}
	if(first)
		Stream<<"0";
	Stream<<endl;
}

//void _CMainCircuit::WriteFlatSimplifiedSoP(long TrId, bool TryToFactorize,const string& FileName, bool Append)
//{
//	DetermineFlatSimplifiedPDD(TrId,TryToFactorize);
//	time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"SoP report");
//	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
//	const _CApproxCriterion& AppCrit=*Traitor.GetApproxCriterion();
//	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
//		Append?ios::out | ios::app : ios::out);
//	if(Append) file<<'\n';
//	file<<"Sum of products for ";
//	file<<AppCrit;
//	for(unsigned i=0;i<m_Cofactors.size();i++)
//	{
//		_CCofactor& Cof=*m_Cofactors[i];
//		_CModelVertex* pMainVertex=Cof.GetMainVertex();
//		_CSExpFlatVerticesFactorized& CofFlatVertices=m_ContextSExpFlatVertices.GetSExpFlatResVertices(i,&Traitor.StrApproxContext());
//		Cof.WriteCofactorDescription(file);
//		file<<" = "<<endl;
//		CofFlatVertices.SoPToStream(file);
//		file<<endl;
//	}
//	//m_FlatVertexCache.ToStream(file);
//}

//void _CMainCircuit::WriteFlatSimplifiedFeasibleDividers(long TrId, const string& FileName, bool Append)
//{
//	DetermineFlatSimplifiedPDD(TrId,true);
//	time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Feasible dividers report");
//	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
//		Append?ios::out | ios::app : ios::out);
//	if(Append) file<<'\n';
//	//m_FlatVertexCache.ToStreamFeasibleDeviders(file);
//	m_FlatVertexCache.ToStreamFeasibleDevidersPaths(file);
//}



#define _USE_PATH_TRAITOR

#ifdef _USE_PATH_TRAITOR
//long _CMainCircuit::GetDirectNmericalPolynomial(unsigned long CofId, const string& Context, _CSparsePolynomial*& pOutPolynomial)
//{
//	long TrId=GetFreePathTraitor(Context);
//	long Res=GetNumericalCofactorAndReminder(CofId,TrId,pOutPolynomial);
//	ReleasePathTraitor(TrId);
//	return Res;
//}
long _CMainCircuit::GetDirectNmericalPolynomial(unsigned long CofId, const string& Context, _CSparsePolynomial& OutPolynomial)
{
	long TrId=GetFreePathTraitor(Context);
	long Res=GetNumericalCofactorAndReminder(CofId,TrId,OutPolynomial);
	ReleasePathTraitor(TrId);
	return Res;
}
#else
long _CMainCircuit::GetDirectNmericalPolynomial(unsigned long CofId, const string& Context, _CSparsePolynomial*& pOutPolynomial)
{
	if(CofId==0 || CofId>m_Cofactors.size())
		return WrongCofIdError(CofId);
	CofId--;
	_CCofactor& Cof=*m_Cofactors[CofId];
	const string* pUContext=&m_ContextStrings.GetUniqueString(Context);
	_CModelVertex* pMainVertex=Cof.GetMainVertex(pUContext);
	if(pMainVertex==NULL)
		WrongContextName(*pUContext);
	pOutPolynomial=&m_NumericPolynomials[NUM_POLY_ITEM_ID(pUContext,pMainVertex)];
	if(pOutPolynomial->empty())
	{
		time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Direct numerical value determination");
		const _CSubModelNumericPattern* pMainOutput=NULL;
		_CDirectNumContainer* pCacheEntry=NULL;
		pMainVertex->PrepareDirectNumericValue(pMainOutput,pCacheEntry,pUContext);//????
		if(pMainOutput==NULL)
			return -1;
		ASSERTPDD(pMainOutput->size()==1);
		//*pOutPolynomial=(*pMainOutput)[0];
		*pOutPolynomial=(*pMainOutput).get(0);
		if(Cof.GetGlobalMultiplier()!=1)
			(*pOutPolynomial)*=Cof.GetGlobalMultiplier();
		//m_DirNumCache.ReportHitTest();
		//m_DirNumCache.Clear();
		m_DirNumCache.HibernateAll();
	}
	return 0;
}
#endif

long _CMainCircuit::GetNumericalCofactorAndReminder(unsigned long CofId, long hPathTraitor, _CSparsePolynomial& Result)
{
	if(CofId==0 || CofId>m_Cofactors.size())
		return WrongCofIdError(CofId);
	CofId--;
	_CCofactor& Cof=*m_Cofactors[CofId];
	_CPathTraitor& Traitor=GetExistedPathTraitor(hPathTraitor);
	//_CPathTraitor::FOLLOWER Follower=Traitor.GetStartFollower();
	const string& BaseContext=Traitor.GetBaseContext();
	const string& UContext=Traitor.StrContext2();
	_CModelVertex* pMainVertex=Cof.GetMainVertex(&BaseContext);
	if(pMainVertex==NULL)
		WrongContextName(UContext);
	_CSparsePolynomial* pResult=&m_NumericPolynomials[NUM_POLY_ITEM_ID(&UContext,pMainVertex)];
	if(pResult->empty())
	{
		time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Direct numerical cof/rem value determination");
		const _CSubModelNumericPattern* pMainOutput=NULL;
		_CDirectNumContainer* pCacheEntry=NULL;
		//pMainVertex->PrepareDirectNumericalDivRem(pMainOutput, pCacheEntry, Traitor);
		pMainVertex->PrepareDirectNumericalDivRem(pMainOutput, pCacheEntry, Traitor,m_CirCompRedefEntry);
		if (pMainOutput == NULL)
			return -1;
		ASSERTPDD(pMainOutput->size()<=1);
		*pResult=(*pMainOutput).get(0);
	}
	short Mult=Cof.GetGlobalMultiplier();
	if(Mult!=1)
	{
		_CSparsePolynomial MultP;
		MultP.put(0,Mult);
		Result=(*pResult)*MultP;
	}
	else
		Result=(*pResult);
	return 0;
}

//long _CMainCircuit::GetNumericalCofactorAndReminder(unsigned long CofId, long hPathTraitor, _CSparsePolynomial*& pResult)
//{
//	if(CofId==0 || CofId>m_Cofactors.size())
//		return WrongCofIdError(CofId);
//	CofId--;
//	_CCofactor& Cof=*m_Cofactors[CofId];
//	_CPathTraitor& Traitor=GetExistedPathTraitor(hPathTraitor);
//	//_CPathTraitor::FOLLOWER Follower=Traitor.GetStartFollower();
//	const string& BaseContext=Traitor.GetBaseContext();
//	const string& UContext=Traitor.StrContext2();
//	_CModelVertex* pMainVertex=Cof.GetMainVertex(&BaseContext);
//	if(pMainVertex==NULL)
//		WrongContextName(UContext);
//	pResult=&m_NumericPolynomials[NUM_POLY_ITEM_ID(&UContext,pMainVertex)];
//	if(pResult->empty())
//	{
//		time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Direct numerical cof/rem value determination");
//		const _CSubModelNumericPattern* pMainOutput=NULL;
//		_CDirectNumContainer* pCacheEntry=NULL;
//		pMainVertex->PrepareDirectNumericalDivRem(pMainOutput,pCacheEntry,Traitor);
//		if(pMainOutput==NULL)
//			return -1;
//		ASSERTPDD(pMainOutput->size()<=1);
//		//*pOutPolynomial=(*pMainOutput)[0];
//
//		_CSparsePolynomial MultP;
//		MultP.put(0,Cof.GetGlobalMultiplier());
//		if(pMainOutput->size()==1)
//			*pResult=((*pMainOutput).get(0)*MultP);
//		//m_DirNumCache.Clear();//???????????????????????
//	}
//	return 0;
//}

long _CMainCircuit::GetDirectNumericalCofactorToComponent(unsigned long CofId, 
														  unsigned short sPower, 
														  const string& ParameterPath, 
														  _CSparsePolynomial* pResult,
														  const string** pResContext,
														  const string& BaseContext)
{
	if(CofId==0 || CofId>m_Cofactors.size())
		return WrongCofIdError(CofId);
	CofId--;
	//_CConstComponentPath& Path1=m_ConstPathStore.GetPath(ParameterPath);
	_CComponentPath& Path=m_PathStore.GetPath(ParameterPath);
	//_CComponentPath Path(this,ParameterPath);
	_CComponent* pOperComp=NULL;
	_CCircuit* pOperCircuit=NULL;
	//if(Path.PrepareLocalOperation(pOperCircuit,pOperComp))
	//{
	//	pOperCircuit->PreparDivider(pOperComp);
	//}
	_CComponentPath::PATHRPointer ID;
	if(Path.PreparLastLocalData(ID,pOperCircuit,pOperComp))
	{
		pOperCircuit->PreparDivider(pOperComp);
		while(Path.PreparPrevLocalData(ID,pOperCircuit,pOperComp))
			pOperCircuit->PreparDivider(pOperComp);
	}
	//=Path.PreparNextLocalData(NULL,pOperCircuit,pOperComp);//usun¹æ
	//string OC=(BaseContext.empty()?EmptyString:BaseContext+"-")+CofString+Path.GetPathDescr();
	////_CContextStringBuffer::PARENTS_VECTOR Vector(1,&BaseContext);
	//const string& OutputContext=m_ContextStrings.GetUniqueString(OC/*,Vector*/);
	//DOUBLE_RES& Res=m_NumResults.force_at(OutputContext,CofId,sPower);
	//if(!Res.IsEmpty())
	//{
	//	Result=Res;
	//	return 0;
	//}
	//if(m_CSSimpleMainVertices.Exists(OutputContext,sPower,CofId))
	//	return GetSPowerCoefficient(CofId,sPower,Result,OutputContext);
	//const _CCofactorReminderSuplement& Sup=m_DataSuplements.GetCofRemSuplement(Path,true,BaseContext);
	//_CNumericalCofactorOperator Operator(this,sPower,Sup,OutputContext,BaseContext,&m_SPowerCoeffCache);
	//_CSExpandedVertices& Vertices=GetMainSVertices(CofId,false);
	//_CSimpleVertexContainer& Cont=Vertices[sPower];
	//Result=Cont.DoUnaryOpertationForWholeTree(Operator,false);
	//Result*=Operator.Pop1Multiplier()*Cont.GetLocalMultiplier();
	//Res=Result;
	return 0;
}

//
long _CMainCircuit::GetDirectNumericalReminderToComponent(unsigned long CofId, 
														  unsigned short sPower, 
														  const string& ParameterPath, 
														  _CSparsePolynomial* pResult,
														  const string** pResContext,
														  const string& BaseContext)
{
	if(CofId==0 || CofId>m_Cofactors.size())
		return WrongCofIdError(CofId);
	CofId--;
	//_CConstComponentPath& Path=m_ConstPathStore.GetConstantPath(ParameterPath);
	//string OC=(BaseContext.empty()?EmptyString:BaseContext+"-")+RemString+Path.GetPathDescr();
	////_CContextStringBuffer::PARENTS_VECTOR Vector(1,&BaseContext);
	//const string& OutputContext=m_ContextStrings.GetUniqueString(OC/*,Vector*/);
	//DOUBLE_RES& Res=m_NumResults.force_at(OutputContext,CofId,sPower);
	//if(!Res.IsEmpty())
	//{
	//	Result=Res;
	//	return 0;
	//}
	//if(m_CSSimpleMainVertices.Exists(OutputContext,sPower,CofId))
	//	return GetSPowerCoefficient(CofId,sPower,Result,OutputContext);

	//const _CCofactorReminderSuplement& Sup=m_DataSuplements.GetCofRemSuplement(Path,false,BaseContext);
	//_CNumericalReminderOperator Operator(this,sPower,Sup,OutputContext,BaseContext,&m_SPowerCoeffCache);
	//_CSExpandedVertices& Vertices=GetMainSVertices(CofId,false);
	//_CSimpleVertexContainer& Cont=Vertices[sPower];
	//Result=Cont.DoUnaryOpertationForWholeTree(Operator,false);
	//Result*=Operator.Pop1Multiplier();
	//Res=Result;
	return 0;
}

long _CMainCircuit::GetDirectNmericalValue(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& Value)
{
	//_CSparsePolynomial* pOutPolynomila=NULL;
	//long State=GetDirectNmericalPolynomial(CofId,Context,pOutPolynomila);
	//if(State!=0)
	//	return State;
	//ASSERTPDD(pOutPolynomila!=NULL);
	//Value=pOutPolynomila->get(sPower);
	//return 0;
	_CSparsePolynomial OutPolynomila;
	long State=GetDirectNmericalPolynomial(CofId,Context,OutPolynomila);
	if(State!=0)
		return State;
	Value=OutPolynomila.get(sPower);
	return 0;
}

long _CMainCircuit::GetDirectNmericalPolynomialSize(unsigned long CofId, const string& Context)
{
	//_CSparsePolynomial* pOutPolynomila=NULL;
	//long State=GetDirectNmericalPolynomial(CofId,Context,pOutPolynomila);
	//if(State!=0)
	//	return State;
	//ASSERTPDD(pOutPolynomila!=NULL);
	//return (long)pOutPolynomila->get_order();
	_CSparsePolynomial OutPolynomila;
	long State=GetDirectNmericalPolynomial(CofId,Context,OutPolynomila);
	if(State!=0)
		return State;
	return (long)OutPolynomila.get_order();
}

//long _CMainCircuit::WriteDirectNmericalPolynomial(unsigned long CofId, 
//												  const string& Context,
//												  const string& FileName, 
//												  bool Append, 
//												  _CSparsePolynomial** ppPolResult,
//												  bool TestDistribution,
//												  long double UpperAngFrequency)
//{
//	_CSparsePolynomial* pOutPolynomila=NULL;
//	long State=GetDirectNmericalPolynomial(CofId,Context,pOutPolynomila);
//	if(State!=0)
//		return State;
//	if(ppPolResult!=NULL)
//		*ppPolResult=pOutPolynomila;
//	ASSERTPDD(pOutPolynomila!=NULL);
//	if(CofId==0 || CofId>m_Cofactors.size())
//		return WrongCofIdError(CofId);
//	CofId--;
//	_CCofactor& Cof=*m_Cofactors[CofId];
//	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
//		Append?ios::out | ios::app : ios::out);
//	if(Append) file<<'\n';
//	size_t _order=pOutPolynomila->get_order();
//	string LP,PP;
//	if(!Context.empty()) 
//	{
//		LP="[";PP="]";
//	}
//	file<<Context<<LP;
//	Cof.WriteCofactorDescription(file);
//	file<<PP<<"=";
//	pOutPolynomila->WriteToTextStream(file);
//	file<<endl;
//	if(TestDistribution)
//	{
//		_CSparsePolynomial::MagnitudeDistribution MagDist;
//		//pOutPolynomila->GetMagnitudeDistribution(MagDist,UpperAngFrequency);
//		pOutPolynomila->GetMagnitudeDistribution(MagDist,1.0);
//		file<<MagDist;
//		pOutPolynomila->GetMagnitudeDistribution(MagDist,UpperAngFrequency);
//		file<<MagDist;
//		_CSparsePolynomial::EnergyDistribution EngDist;
//		pOutPolynomila->GetEnergyDistribution(EngDist,1.0);
//		file<<EngDist;
//		pOutPolynomila->GetEnergyDistribution(EngDist,UpperAngFrequency);
//		file<<EngDist;
//	}
//	//if(pOutPolynomila->empty())
//	//	file<<"0";
//	//else
//	//	for(size_t i=_order;;i--)
//	//	{
//	//		const NumericType& Value=pOutPolynomila->get(i);
//	//		if(Value==EmptyFloat) 
//	//			if(i==0)
//	//				break;
//	//			else 
//	//				continue;
//	//		file<<((Value < EmptyFloat)?" - ":" + ");
//	//		if(i>0)
//	//			file<<"s^"<<i<<"*";
//	//		file<<abs(Value);
//	//		if(i==0) break;
//	//	}
//	//file<<'\n';
//	return 0;
//}

long _CMainCircuit::WriteDirectNmericalPolynomial(unsigned long CofId,
	const string& Context,
	iostream& file,
	_CSparsePolynomial* pPolResult,
	bool FindZeroes,
	bool TestDistribution,
	long double UpperAngFrequency)
{
	_CSparsePolynomial OutPolynomila;
	long State = GetDirectNmericalPolynomial(CofId, Context, OutPolynomila);
	if (State != 0)
		return State;
	if (pPolResult != NULL)
		*pPolResult = OutPolynomila;
	if (CofId == 0 || CofId>m_Cofactors.size())
		return WrongCofIdError(CofId);
	CofId--;
	_CCofactor& Cof = *m_Cofactors[CofId];
	size_t _order = OutPolynomila.get_order();
	string LP, PP;
	if (!Context.empty())
	{
		LP = "["; PP = "]";
	}
	file << Context << LP;
	Cof.WriteCofactorDescription(file);
	file << PP << "=";
	OutPolynomila.WriteToTextStream(file);
	file << endl;
	if (FindZeroes)
	{
		NormVectComplex Zeroes;
		try
		{
			SpPoly2Roots(OutPolynomila, Zeroes);
		}
		catch (...)
		{

		}
		file << "List of roots:" << endl;
		Zeroes.Write2Stream(file);

	}
	if (TestDistribution)
	{
		_CSparsePolynomial::MagnitudeDistribution MagDist;
		//pOutPolynomila->GetMagnitudeDistribution(MagDist,UpperAngFrequency);
		OutPolynomila.GetMagnitudeDistribution(MagDist, 1.0);
		file << MagDist;
		OutPolynomila.GetMagnitudeDistribution(MagDist, UpperAngFrequency);
		file << MagDist;
		_CSparsePolynomial::EnergyDistribution EngDist;
		OutPolynomila.GetEnergyDistribution(EngDist, 1.0);
		file << EngDist;
		OutPolynomila.GetEnergyDistribution(EngDist, UpperAngFrequency);
		file << EngDist;
	}
	return 0;
}

long _CMainCircuit::WriteDirectNmericalPolynomial(unsigned long CofId, 
												  const string& Context,
												  const string& FileName, 
												  bool Append, 
												  _CSparsePolynomial* pPolResult,
												  bool FindZeroes,
												  bool TestDistribution,
												  long double UpperAngFrequency)
{
	_CSparsePolynomial OutPolynomila;
	long State=GetDirectNmericalPolynomial(CofId,Context,OutPolynomila);
	if(State!=0)
		return State;
	if(pPolResult!=NULL)
		*pPolResult=OutPolynomila;
	if(CofId==0 || CofId>m_Cofactors.size())
		return WrongCofIdError(CofId);
	CofId--;
	_CCofactor& Cof=*m_Cofactors[CofId];
	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
		Append?ios::out | ios::app : ios::out);
	if(Append) file<<'\n';
	size_t _order=OutPolynomila.get_order();
	string LP,PP;
	if(!Context.empty()) 
	{
		LP="[";PP="]";
	}
	file<<Context<<LP;
	Cof.WriteCofactorDescription(file);
	file<<PP<<"=";
	OutPolynomila.WriteToTextStream(file);
	file<<endl;
	if(FindZeroes)
	{
		NormVectComplex Zeroes;
		try
		{
			SpPoly2Roots(OutPolynomila,Zeroes);
		}
		catch(...)
		{

		}
		file<<"List of roots:"<<endl;
		Zeroes.Write2Stream(file);

	}
	if(TestDistribution)
	{
		_CSparsePolynomial::MagnitudeDistribution MagDist;
		//pOutPolynomila->GetMagnitudeDistribution(MagDist,UpperAngFrequency);
		OutPolynomila.GetMagnitudeDistribution(MagDist,1.0);
		file<<MagDist;
		OutPolynomila.GetMagnitudeDistribution(MagDist,UpperAngFrequency);
		file<<MagDist;
		_CSparsePolynomial::EnergyDistribution EngDist;
		OutPolynomila.GetEnergyDistribution(EngDist,1.0);
		file<<EngDist;
		OutPolynomila.GetEnergyDistribution(EngDist,UpperAngFrequency);
		file<<EngDist;
	}
	return 0;
}

long _CMainCircuit::WriteIntermediateNumericPolynomials(const string& FileName,bool Append)
{
	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
		Append?ios::out | ios::app : ios::out);
	if(Append) file<<'\n';
	file<<"Intermediate Results:\n";
	for(long i=1;i<m_SubcircuitCounter;i++)
	{
		_CSubCircuit* pSubCircuit=GetSubcircuit(i);
		if(pSubCircuit!=NULL)
		{
			file<<"SubId: "<<i<<"\n";
			pSubCircuit->WriteIntermediateNumericPolynomials(file);
		}
	}
	return 0;
}

//long _CMainCircuit::PerformFullCofRemTest(const string& ElementPath, 
//										  const string& OutTexFileName, 
//										  bool CheckResIntegrity,
//										  bool Append, 
//										  bool TestDistribution, 
//										  long double UpperAngFrequency)
//{
//	time_stamp Timer(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,string("Test of Divider and Reminder for ")+ElementPath);
//	long TrId=GetFreePathTraitor(EmptyString);
//	for(size_t i=0;i<m_Cofactors.size();i++)
//	{
//		const _CSubModelNumericPattern *pCompPattern=NULL;
//		ClearPathTraitor(TrId);
//		const _CComponent* pComponent=AddParamDivider(TrId,ElementPath,&pCompPattern);
//		_CSparsePolynomial* pDivider=NULL;
//		WriteDirectNmericalPolynomial(i+1,TrId,OutTexFileName,true,&pDivider,TestDistribution,UpperAngFrequency);
//		ClearPathTraitor(TrId);
//		AddParamReminder(TrId,ElementPath);
//		_CSparsePolynomial* pReminder=NULL;
//		WriteDirectNmericalPolynomial(i+1,TrId,OutTexFileName,true,&pReminder,TestDistribution,UpperAngFrequency);
//		if(CheckResIntegrity)
//		{
//			_CSparsePolynomial TestOutRes;
//			TestOutRes=(*pDivider)*(pCompPattern->get(0))+(*pReminder);
//			//_CSparsePolynomial::NumComplex Test=TestOutRes.freq_response(1.0e9);
//			//_CSparsePolynomial::MagnitudeDistribution MagDist;
//			//TestOutRes.GetMagnitudeDistribution(MagDist,1.0e9);
//			//_CSparsePolynomial::EnergyDistribution EngDist;
//			//TestOutRes.GetEnergyDistribution(EngDist,1.0e9);
//			//NumericType ABS=abs(Test);
//			//NumericType ARG=arg(Test);
//			fstream file(m_ModelFileName.empty()?OutTexFileName.c_str():m_ModelFileName.c_str()
//				,Append?ios::out | ios::app : ios::out);
//			file<<"\n Result test:\n";
//			file<<'[';
//			pDivider->WriteToTextStream(file);
//			file<<"]·[";
//			pCompPattern->get(0).WriteToTextStream(file);
//			file<<"]\n+[";
//			pReminder->WriteToTextStream(file);
//			file<<"]\n= ";
//			TestOutRes.WriteToTextStream(file);
//			file<<endl;
//			//file<<MagDist;
//		}
//	}
//	ReleasePathTraitor(TrId);
//	return 0;
//}

long _CMainCircuit::PerformFullCofRemTest(const string& ElementPath, 
										  const string& OutTexFileName, 
										  bool CheckResIntegrity,
										  bool Append, 
										  bool TestDistribution, 
										  long double UpperAngFrequency)
{
	time_stamp Timer(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,string("Test of Divider and Reminder for ")+ElementPath);
	long TrId=GetFreePathTraitor(EmptyString);
	for(size_t i=0;i<m_Cofactors.size();i++)
	{
		const _CSubModelNumericPattern *pCompPattern=NULL;
		ClearPathTraitor(TrId);
		const _CComponent* pComponent=AddParamDivider(TrId,ElementPath,&pCompPattern);
		_CSparsePolynomial Divider;
		WriteDirectNmericalPolynomial(i+1,TrId,OutTexFileName,true,&Divider,TestDistribution,UpperAngFrequency);
		ClearPathTraitor(TrId);
		AddParamReminder(TrId,ElementPath);
		_CSparsePolynomial Reminder;
		WriteDirectNmericalPolynomial(i+1,TrId,OutTexFileName,true,&Reminder,TestDistribution,UpperAngFrequency);
		if(CheckResIntegrity)
		{
			_CSparsePolynomial TestOutRes;
			TestOutRes=Divider*(pCompPattern->get(0))+Reminder;
			fstream file(m_ModelFileName.empty()?OutTexFileName.c_str():m_ModelFileName.c_str()
				,Append?ios::out | ios::app : ios::out);
			file<<"\n Result test:\n";
			file<<'[';
			Divider.WriteToTextStream(file);
			file<<"]·[";
			pCompPattern->get(0).WriteToTextStream(file);
			file<<"]\n+[";
			Reminder.WriteToTextStream(file);
			file<<"]\n= ";
			TestOutRes.WriteToTextStream(file);
			file<<endl;
			//file<<MagDist;
		}
	}
	ReleasePathTraitor(TrId);
	return 0;
}

long _CMainCircuit::WriteModelsToTextFile(const string& FileName)
{
	time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Multilevel result raporting");
	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),ios::out | (m_ModelFileName.empty()?0:ios::app));
	for(long i=1;i<m_SubcircuitCounter;i++)
	{
		_CSubCircuit* pSubCircuit=GetSubcircuit(i);
		if(pSubCircuit!=NULL)
		{
			file<<"SubId: "<<i<<"\n";
			pSubCircuit->WriteCircuitModelToStream(file);
			file<<"\n";
		}
	}
	file<<"SubId: 0\n";
	WriteCircuitModelToStream(file);
	return 0;
}

//long _CMainCircuit::WriteCircuitToTextFile(const string& FileName, bool DevelopeOrdered, bool ComputeNumerical)
//{
//	fstream file(m_CircuitFileName.empty()?FileName.c_str():m_CircuitFileName.c_str(),ios::out | (m_CircuitFileName.empty()?0:ios::app));
//	{
//		time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"sExpanded form reporting ");
//		for(long i=1;i<m_SubcircuitCounter;i++)
//		{
//			_CSubCircuit* pSubCircuit=GetSubcircuit(i);
//			if(pSubCircuit!=NULL)
//			{
//				file<<"SubId: "<<i<<"\n";
//				pSubCircuit->WriteDescriptionToStream(file);
//				file<<"\n";
//			}
//		}
//		file<<"SubId: 0\n";
//		WriteDescriptionToStream(file);
//		file<<"\n";
//		_CSimpleVerticesMapsSet Set;
//
//		for each (_CCofactor* pCof in m_Cofactors)
//		{
//			pCof->WriteToTextStream(file,Set);
//			//pCof->WriteToTextStream(file);
//		}
//		file<<"\n";
//		WriteSimpleVertices(0,file, Set);
//		//WriteSimpleVertices(0,file);
//		Set.WriteToStream(file); //Zmodyfikuj
//		file<<"\n";
//		for(long i=1;i<m_SubcircuitCounter;i++)
//		{
//			_CSubCircuit* pSubCircuit=GetSubcircuit(i);
//			if(pSubCircuit!=NULL)
//			{
//				_CSimpleVerticesMapsSet Set;
//				pSubCircuit->WriteInputConnectionEntry(file, Set);
//				pSubCircuit->WriteSimpleVertices(i,file, Set);
//				//pSubCircuit->WriteInputConnectionEntry(file);
//				//pSubCircuit->WriteSimpleVertices(i,file);
//				Set.WriteToStream(file); //Zmodyfikuj
//				file<<"\n";
//			}
//		}
//	}
//	if(DevelopeOrdered)
//	{
//		time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Exact cofactor enumeration ");
//		WriteOrderedSimpleVertices(file);
//		file<<"\n";
//	}
//	if(ComputeNumerical)
//	{
//		time_stamp stamp(m_RaportFileName.empty()?"Raport.txt":m_RaportFileName,"Numerical calculatiom ");
//		WriteSNumericalResults(file);
//	}
//	return 0;
//}

bool _CMainCircuit::IsSExpanded()
{
	return m_pEngine->IsSExpanded();
}


void _CMainCircuit::WriteOrderedSimpleVertices(iostream& stream)
{
	stream<<endl;
	_CVertex2TextOperator Vertex2Text(this, 0,stream,EmptyString);
	size_t _csize=m_Cofactors.size();
	for(size_t i=0;i<_csize;i++)
	{
		_CCofactor& Cof=*m_Cofactors[i];
//		_CSExpandedVertices& VCS=m_SimpleMainVertices[i]; 
		_CSExpandedVertices& VCS=m_CSSimpleMainVertices.GetSExpandedVertices(EmptyString,
			_CContextSExpandedCofactorValues::MUST_EXIST,i); 
//		_CSExpandedVertices VCS=pCof->GetMainSVertices();
		size_t _ssize=VCS.size();
		for(size_t j=0;j<_ssize;j++)
		{
			size_t a=_ssize-j-1;
			//_CSimpleVertexContainer& StartVertex=VCS[a];
			_CSimpleVertexContainer& StartVertex=VCS[j];
			if(StartVertex.Is0())
				continue;
			Vertex2Text.ReSetSPower(j);
			const _CRepeatContainer* Res=StartVertex.DoUnaryOpertationForWholeTree(Vertex2Text,false);
			//string sMult;
			//Vertex2Text.MultGStr(sMult,StartVertex);
			stream<<"The coefficient by ";
			Cof.WriteCofactorDescription(stream);
			//stream<<" - s^"<<a<<" is "<<sMult<<Res<<endl<<endl;
			stream<<" - s^"<<j<<" is "/*<<sMult*/<<Res->GetNodeName()<<endl<<endl;
		}
		//for(_CSExpandedVertices::iterator it=VCS.begin();it!=VCS.end();it++)
		//{
		//	string Res=(*it).DoUnaryOpertationForWholeTree(Vertex2Text,false);
		//	//Dopisaæ wyjœcie i znak
		//}
	}
}
//long double _CMainCircuit::GetSPowerCoefficient(_CSExpandedVertices& Vertices, unsigned short sPower) 
//long _CMainCircuit::GetSPowerCoefficient(unsigned long CofId, unsigned short sPower, NumericType& Result,const string& Context) 
//{
//	if(CofId==0 || CofId>m_Cofactors.size())
//		return WrongCofIdError(CofId);
//	CofId--;
//	const string* pUContext=m_ContextStrings.CheckUniqName(Context);
//	if(pUContext==NULL)
//		return WrongContextName(Context);
//	DOUBLE_RES& Res=m_NumResults.force_at(*pUContext,CofId,sPower);
//	if(!Res.IsEmpty())
//	{
//		Result=Res;
//		return 0;
//	}
//	_CSExpandedVertices& Vertices=GetMainSVertices(CofId,false);
//	//m_SPowerCoeffCache.clear();
//	_CSPowerCoeffOperator SPowerOperator(this,sPower,*pUContext,&m_SPowerCoeffCache);
//	_CSimpleVertexContainer& Cont=Vertices[sPower];
//	Result=Cont.DoUnaryOpertationForWholeTree(SPowerOperator,false);
//	//Result*=SPowerOperator.Pop1Multiplier()*Cont.GetLocalMultiplier();
//	//Result*=SPowerOperator.Get1Multiplier()*Cont.GetLocalMultiplier();
//	Res=Result;
//	return 0;
//}

const char* CNE="You try to determime the cofactor of value that does not exist!";
//long _CMainCircuit::GetNumericalCofactorToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, NumericType& Result, const string& BaseContext)
//{
//	if(CofId==0 || CofId>m_Cofactors.size())
//		return WrongCofIdError(CofId);
//	CofId--;
//	_CConstComponentPath& Path=m_ConstPathStore.GetPath(ParameterPath);
//	string OC=(BaseContext.empty()?EmptyString:BaseContext+"-")+CofString+Path.GetPathDescr();
//	//_CContextStringBuffer::PARENTS_VECTOR Vector(1,&BaseContext);
//	const string& OutputContext=m_ContextStrings.GetUniqueString(OC/*,Vector*/);
//	DOUBLE_RES& Res=m_NumResults.force_at(OutputContext,CofId,sPower);
//	if(!Res.IsEmpty())
//	{
//		Result=Res;
//		return 0;
//	}
//	if(m_CSSimpleMainVertices.Exists(OutputContext,sPower,CofId))
//		return GetSPowerCoefficient(CofId,sPower,Result,OutputContext);
//	const _CCofactorReminderSuplement& Sup=m_DataSuplements.GetCofRemSuplement(Path,true,BaseContext);
//	_CNumericalCofactorOperator Operator(this,sPower,Sup,OutputContext,BaseContext,&m_SPowerCoeffCache);
//	_CSExpandedVertices& Vertices=GetMainSVertices(CofId,false);
//	_CSimpleVertexContainer& Cont=Vertices[sPower];
//	Result=Cont.DoUnaryOpertationForWholeTree(Operator,false);
//	Result*=Operator.Pop1Multiplier()*Cont.GetLocalMultiplier();
//	Res=Result;
//	return 0;
//}
//

long _CMainCircuit::CreateCofactorToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, const string& BaseContext, const string*& pOutputContext)
{
	try
	{
		if(CofId==0 || CofId>m_Cofactors.size())
			return WrongCofIdError(CofId);
		CofId--;
		_CSimpleVertexContainer& BaseVertex=
			m_CSSimpleMainVertices.GetSExpandedCoef(BaseContext,_CContextSExpandedCofactorValues::MUST_EXIST,
			sPower,CofId);
		_CConstComponentPath& Path=m_ConstPathStore.GetPath(ParameterPath);
		string OC=(BaseContext.empty()?EmptyString:BaseContext+"-")+CofString+Path.GetPathDescr();
		//_CContextStringBuffer::PARENTS_VECTOR Vector(1,&BaseContext);
		pOutputContext=&m_ContextStrings.GetUniqueString(OC/*,Vector*/);
		_CSimpleVertexContainer& ResVertex=
			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext,
			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
		if(!ResVertex.IsEmpty())
			return 0;
		const _CCofactorReminderSuplement& Sup=m_DataSuplements.GetCofRemSuplement(Path,true,BaseContext);
		_CVertexCofactorOperator Operator(this,sPower,Sup,*pOutputContext,BaseContext,&m_VCRCache);
		ResVertex=BaseVertex.DoUnaryOpertationForWholeTree(Operator,false);
		short Mult=Operator.Pop1Multiplier()*BaseVertex.GetLocalMultiplier();
		//Zobaczymy
		return 0;
	}
	catch(eVertexEntryUndifined)
	{
		return WrongContextName(BaseContext);
	}
}

long _CMainCircuit::AppendToFileCoefVertices(const string& FileName, unsigned long CofId, unsigned short sPower, const string& ParameterPath)
{
	fstream file(m_CircuitFileName.empty()?FileName.c_str():m_CircuitFileName.c_str(),
		ios::out | ios::app);
	file<<"\n"<<ParameterPath<<" - Cofactor No: "<<CofId<<" Coeff. s^"<<sPower<<":\n";
	return WriteCoefVertices(file,CofId,sPower,ParameterPath);
}
//void _CMainCircuit::RegisterInFile(const char* FileName,string ParameterName,long double Value)
//{
//	fstream file(FileName,ios::out | ios::app);
//	file<<ParameterName<<" = "<<Value<<endl;
//}

long _CMainCircuit::WriteCoefVertices(iostream& stream, unsigned long CofId, unsigned short sPower, const string& Context)
{
	try
	{
		if(CofId==0 || CofId>=m_Cofactors.size())
			return WrongCofIdError(CofId);
		CofId--;
		const string& ContextString=*m_ContextStrings.CheckUniqName(Context);
		//const string& ContextString=m_ContextStrings.GetUniqueString(Context);
		_CSimpleVertexContainer& BaseVertex=
			m_CSSimpleMainVertices.GetSExpandedCoef(ContextString,
			_CContextSExpandedCofactorValues::MUST_EXIST,sPower,CofId);
		_CVertex2TextOperator Operator(this,sPower,stream,ContextString);
		if(BaseVertex.Is01M1())
		{
			stream<<Operator.DoThe01Operation(BaseVertex)<<"\n";
		}
		else
		{
			BaseVertex.DoUnaryOpertationForWholeTree(Operator,false);
			//short Mult=Operator.Pop1Multiplier();
			short Mult=Operator.Get1Multiplier();
			if(Mult!=1)
				stream<<"Multiply it by "<<Mult;
			stream<<endl;
		}
		return 0;
	}
	catch(eVertexEntryUndifined)
	{
		return WrongContextName(Context);
	}
}


const char* RNE="You try to determime the cofactor of value that does not exist!";

//long _CMainCircuit::GetNumericalReminderToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, NumericType& Result,const string& BaseContext)
//{
//	if(CofId==0 || CofId>m_Cofactors.size())
//		return WrongCofIdError(CofId);
//	CofId--;
//	_CConstComponentPath& Path=m_ConstPathStore.GetPath(ParameterPath);
//	string OC=(BaseContext.empty()?EmptyString:BaseContext+"-")+RemString+Path.GetPathDescr();
//	//_CContextStringBuffer::PARENTS_VECTOR Vector(1,&BaseContext);
//	const string& OutputContext=m_ContextStrings.GetUniqueString(OC/*,Vector*/);
//	DOUBLE_RES& Res=m_NumResults.force_at(OutputContext,CofId,sPower);
//	if(!Res.IsEmpty())
//	{
//		Result=Res;
//		return 0;
//	}
//	if(m_CSSimpleMainVertices.Exists(OutputContext,sPower,CofId))
//		return GetSPowerCoefficient(CofId,sPower,Result,OutputContext);
//
//	const _CCofactorReminderSuplement& Sup=m_DataSuplements.GetCofRemSuplement(Path,false,BaseContext);
//	_CNumericalReminderOperator Operator(this,sPower,Sup,OutputContext,BaseContext,&m_SPowerCoeffCache);
//	_CSExpandedVertices& Vertices=GetMainSVertices(CofId,false);
//	_CSimpleVertexContainer& Cont=Vertices[sPower];
//	Result=Cont.DoUnaryOpertationForWholeTree(Operator,false);
//	Result*=Operator.Pop1Multiplier();
//	Res=Result;
//	return 0;
//}

long _CMainCircuit::CreateReminderToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, const string& BaseContext, const string*& pOutputContext)
{
	try
	{
		if(CofId==0 || CofId>m_Cofactors.size())
			return WrongCofIdError(CofId);
		CofId--;
		_CSimpleVertexContainer& BaseVertex=
			m_CSSimpleMainVertices.GetSExpandedCoef(BaseContext,
			_CContextSExpandedCofactorValues::MUST_EXIST,sPower,CofId);
		_CConstComponentPath& Path=m_ConstPathStore.GetPath(ParameterPath);
		string OC=(BaseContext.empty()?EmptyString:BaseContext+"-")+RemString+Path.GetPathDescr();
		//_CContextStringBuffer::PARENTS_VECTOR Vector(1,&BaseContext);
		pOutputContext=&m_ContextStrings.GetUniqueString(OC/*,Vector*/);
		_CSimpleVertexContainer& ResVertex=
			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext,
			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
		if(!ResVertex.IsEmpty())
			return true;
		const _CCofactorReminderSuplement& Sup=m_DataSuplements.GetCofRemSuplement(Path,false,BaseContext);
		_CVertexReminderOperator Operator(this,sPower,Sup,*pOutputContext,BaseContext,&m_VCRCache);
		ResVertex=BaseVertex.DoUnaryOpertationForWholeTree(Operator,false);
		short Mult=Operator.Pop1Multiplier();
		//Zobaczymy
		return 0;
	}
	catch(eVertexEntryUndifined)
	{
		return WrongContextName(BaseContext);
	}
}
//void _CMainCircuit::ReComputeSNmumericalResults()
//{
//	ResetSPowerCoeffCache();
//	//m_CofRemResultCache.clear();
//	//m_NumericalSCofactors.Reset(m_SimpleMainVertices.size());
//}

//void _CMainCircuit::WriteSNumericalResults(iostream& stream)
//{
//	unsigned _csize=m_Cofactors.size(),
//		//_sesize=m_SimpleMainVertices.size();
//		_sesize=m_CSSimpleMainVertices.GetSExpandedCofValues(EmptyString,
//		_CContextSExpandedCofactorValues::MUST_EXIST).size();
//
//	if(_csize!=_sesize)
//		RISEPDD(eIllegalOperation,"You cannot use _CMainCircuit::WriteSNumericalResults befor _CMainCircuit::DetermineSimplePDD");
//	for(unsigned i=0;i<m_Cofactors.size();i++)
//	{
//		m_Cofactors[i]->WriteCofactorDescription(stream);
//		stream<<" = ";
//		_CSExpandedVertices& Vertices=m_CSSimpleMainVertices.GetSExpandedCofValues(EmptyString)[i];
//		unsigned _svsize=Vertices.size();
//		NumericType Res=0.0;
//		bool IsSmth=false;
//		for(unsigned j=_svsize-1;j>0;j--)
//		{
//			GetSPowerCoefficient(i+1,j,Res);
//			if(Res==0.0) continue;
//			if(IsSmth || sgn(Res)==-1)
//				stream<<(sgn(Res)==-1?" - ":" + ");
//			IsSmth=true;
//			stream<<"s^"<<j<<"*";
//			stream<</*<setw(10)<<*/setprecision(20)<<abs(Res);
//			//stream<<" + ";
//		}
//		GetSPowerCoefficient(i+1,0,Res);
//		if(IsSmth || sgn(Res)==-1)
//			stream<<(sgn(Res)==-1?" - ":" + ");
//		stream<</*setw(10)<<*/setprecision(20)<<abs(Res);
//		stream<<"\n";
//	}
//}

void _CMainCircuit::Path2CPath(_CConstComponentPath& CPath,_CComponentPath& Path)
{
	CPath.m_HasComponent=Path.m_HasComponent;
	CPath.m_pBaseCircuit=Path.m_pBaseCircuit;
	for(_CComponentPath::PATH_iterator it=Path.m_Path.begin();it!=Path.m_Path.end();it++)
		CPath.m_Path.push_back(*it);
}

void _CMainCircuit::ClearNemericalDependentsInCache(_CComponentPath& Path)
{
	//m_ResNumericalSCofactors.ClearIt();
	m_NumResults.ClearAll();
	//m_WeightRes.ClearAll();
	_CConstComponentPath CPath(this);
	Path2CPath(CPath,Path);
	//m_SPowerCoeffCache.ClearLower(CPath);
	//m_WeightCache.ClearLower(CPath);
}

//bool _CMainCircuit::ModifyParameter(const string& ParamPath, long double NewValue)
//{
//	_CComponentPath CPath(this,ParamPath);
//	_CSubCircuitSocket* pSocket=CPath.GetTopMostSocket();
//	if(pSocket==NULL)
//		CPath.GetComponent()->ModifyValue(NewValue);
//	else
//		pSocket->RedefineParameterValue(CPath.GetComponent(),NewValue);
//	ClearNemericalDependentsInCache(CPath);//poprawiæ
//	return true;
//}

//long _CMainCircuit::FindTheLeastSignificant(unsigned long CofId, unsigned short sPower, const string& Context, long double* pNumericalResult, VECTOR_STRING* pResultPath,string* pStrResult)//returns shortest path Id or Error if <0
//{
//	try
//	{
//		_CLSPath Path;
//		long double Value;
//		long Res=DeterminPathWeights(CofId,sPower,Context,Value,Path);
//		if(Res<0)
//			return Res;
//		if(pNumericalResult!=NULL)
//		{
//			long double Res=0.0;
//			GetSPowerCoefficient(CofId,sPower,Res,Context);
//			*pNumericalResult=100*Value/Res;
//		}
//		if(pResultPath!=NULL)
//			Path.ToVectorString(*pResultPath);
//		if(pStrResult!=NULL)
//			Path.ToOneString(*pStrResult);
//		//_CConstComponentPath& Path=m_ConstPathStore.GetConstantPath(Context);
//		const string* pUContext=m_ContextStrings.CheckContextName(Context);
//		//const string* pUContext=&m_ContextStrings.GetUniqueString(Context);
//		if(pUContext==NULL)
//			return WrongContextName(Context);
//
//		return 0;
//	}
//	catch(eVertexEntryUndifined e)
//	{
//		LastErrorInfo=e.what();
//		return WRONG_CONTEXT_NAME;
//	}
//	catch(eWrongName e)
//	{
//		LastErrorInfo=e.what();
//		return WRONG_COMPONENT_INSTANCE_PATH;
//	}
//}

//long _CMainCircuit::GetTheLeastSignificantPath(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& NumericalResult,_CLSPath& Path)
//{
//	try
//	{
//		if(CofId==0 || CofId>m_Cofactors.size())
//			return WrongCofIdError(CofId);
//		CofId--;
//		const string* pUContext=m_ContextStrings.CheckUniqName(Context);
//		if(pUContext==NULL)
//			return WrongContextName(Context);
//		_CSimpleVertexContainer& BaseVertex=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pUContext,
//			_CContextSExpandedCofactorValues::MUST_EXIST,sPower,CofId);
//		if(BaseVertex.Is01M1())
//			return NO_MORE_TERMS;
//		_CPathWeightOperator Operator(this,sPower,*pUContext,&m_WeightCache);
//		_CWeightResult Res=BaseVertex.DoUnaryOpertationForWholeTree(Operator,false);
//		NumericalResult=Res.GetValue()*Operator.Pop1Multiplier();
//		_CLeastSgnfcntPathOperator Operator2(this,sPower,*pUContext,m_WeightCache,Path);
//		BaseVertex.DoUnaryOpertationForWholeTree(Operator2,false);
//		return 0;
//	}
//	catch(eVertexEntryUndifined)
//	{
//		return WrongContextName(Context);
//	}
//}

long DivisionBy0()
{
	LastErrorInfo="The least significant path removel is allowed only for non-zero values";
	return OPERATION_PROHIBITED;
}

//long _CMainCircuit::GetTheLeastSignificantPathPtc(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& NumericalResult,_CLSPath& Path)
//{
//	long Res=GetTheLeastSignificantPath(CofId,sPower,Context,NumericalResult,Path);
//	if(Res!=0) return Res;
//	NumericType Den=0.0;
//	Res=GetSPowerCoefficient(CofId,sPower,Den,Context);
//	if(Den==0.0)
//		return DivisionBy0();
//	if(Res!=0) return Res;
//	NumericalResult*=100;
//	NumericalResult/=Den;
//	return 0;
//}

//long _CMainCircuit::RemoveTheLeastSigmificantPath(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& AbsolutDecrease, const string*& pOutputContext)
//{
//	_CLSPath Path;
//	long Res=GetTheLeastSignificantPath(CofId,sPower,Context,AbsolutDecrease,Path);
//	if(Res!=0)
//		return Res;
//	return RemovePath(CofId,sPower,Context,Path,pOutputContext);
//}

//long _CMainCircuit::RemoveTheLeastSigmificantPathPtc(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& RelativePtcDecrease, const string*& pOutputContext)
//{
//	_CLSPath Path;
//	long Res=GetTheLeastSignificantPathPtc(CofId,sPower,Context,RelativePtcDecrease,Path);
//	if(Res!=0)
//		return Res;
//	return RemovePath(CofId,sPower,Context,Path,pOutputContext);
//}

long _CMainCircuit::RemovePath(unsigned long CofId, unsigned short sPower, const string& Context, _CLSPath& Path, const string*& pOutputContext)
{
	try
	{
		if(CofId==0 || CofId>m_Cofactors.size())
			return WrongCofIdError(CofId);
		CofId--;
		const string* pUContext=m_ContextStrings.CheckUniqName(Context);
		if(pUContext==NULL)
			return WrongContextName(Context);
		_CSimpleVertexContainer& BaseVertex=
			m_CSSimpleMainVertices.GetSExpandedCoef(*pUContext,
			_CContextSExpandedCofactorValues::MUST_EXIST,sPower,CofId);
		string OC=Context;
		if(OC.empty())
			OC+=("Cf:"+ulong2str(CofId)+";sP:"+ulong2str(sPower)+"; ");
		else
			OC+=" -\n";
		string PStr;
		Path.ToOneString(PStr);
		OC+=PStr;
		pOutputContext=&m_ContextStrings.GetUniqueString(OC);

		_CPathRemoveOperator Operator(this,sPower,Path,*pOutputContext,*pUContext,&m_VCRCache);
		_CSimpleVertexContainer& ResVertex=
			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext,
			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
		ResVertex=BaseVertex.DoUnaryOpertationForWholeTree(Operator,false);
		return 0;
	}
	catch(eVertexEntryUndifined)
	{
		return WrongContextName(Context);
	}
}
//long _CMainCircuit::RemoveTheLeastSigmificantPaths(unsigned long CofId, 
//												   unsigned short sPower, 
//												   const string& Context, 
//												   const NumericType& MaxDecreasePtc, 
//												   NumericType& ResultDecreasePtc,
//												   const string*& pOutputContext,
//												   const char* FileToRegister, 
//												   bool RegOnlyFinalResult)
//{
//	try
//	{
//		if(CofId==0 || CofId>m_Cofactors.size())
//			return WrongCofIdError(CofId);
//		CofId--;
//		//_CConstComponentPath& Path=m_ConstPathStore.GetConstantPath(Context);
//		const string* pUContext=m_ContextStrings.CheckUniqName(Context);
//		if(pUContext==NULL)
//			return WrongContextName(Context);
//		NumericType TRes=0.0;
//		long Res=GetSPowerCoefficient(CofId,sPower,TRes,Context);
//		if(Res!=0)
//			return Res;
//		if(TRes==0.0)
//			return DivisionBy0();
//		_CSimpleVertexContainer* pBaseVertex=
//			&m_CSSimpleMainVertices.GetSExpandedCoef(*pUContext,
//			_CContextSExpandedCofactorValues::MUST_EXIST,sPower,CofId);
//		_CPathWeightOperator PWOperator(this,sPower,*pUContext,&m_WeightCache);
//		_CWeightResult WRes=pBaseVertex->DoUnaryOpertationForWholeTree(PWOperator,false);
//		NumericType lDecrese=WRes.GetValue()*PWOperator.Pop1Multiplier()*100/TRes;
//		if(abs(lDecrese)>abs(MaxDecreasePtc))
//		{
//			ResultDecreasePtc=0.0;
//			return 0;
//		}
//		ResultDecreasePtc=lDecrese;
//		_CLSPath Path;
//		_CLeastSgnfcntPathOperator LSPOperator(this,sPower,*pUContext,m_WeightCache,Path);
//		pBaseVertex->DoUnaryOpertationForWholeTree(LSPOperator,false);
//		string OC=Context;
//		if(OC.empty())
//			OC+=("Cf:"+ulong2str(CofId)+";sP:"+ulong2str(sPower)+"; ");
//		else
//			OC+=" -\n";
//		string PStr;
//		Path.ToOneString(PStr);
//		OC+=PStr;
//		pOutputContext=&m_ContextStrings.GetUniqueString(OC);
//		_CPathRemoveOperator PROperator(this,sPower,Path,*pOutputContext,*pUContext,&m_VCRCache);
//		_CSimpleVertexContainer* pResVertex=
//			&m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		*pResVertex=pBaseVertex->DoUnaryOpertationForWholeTree(PROperator,false);
//		if(FileToRegister!=NULL && !RegOnlyFinalResult)
//			AppendToFileCoefVertices(string(FileToRegister),CofId,sPower,*pOutputContext);
//		while(!pResVertex->Is01M1() && abs(ResultDecreasePtc)<=abs(MaxDecreasePtc))
//		{
//			PWOperator.ChangeBaseOperationContext(*pOutputContext);
//			WRes=pResVertex->DoUnaryOpertationForWholeTree(PWOperator,false);
//			lDecrese=WRes.GetValue()*PWOperator.Pop1Multiplier()*100/TRes;
//			NumericType CurrDescreasa=ResultDecreasePtc+lDecrese;
//			if(abs(CurrDescreasa)>abs(MaxDecreasePtc))
//				break;
//			ResultDecreasePtc=CurrDescreasa;
//			LSPOperator.ChangeBaseOperationContext(*pOutputContext);
//			Path.clear();
//			pBaseVertex->DoUnaryOpertationForWholeTree(LSPOperator,false);
//			PROperator.ChangeBaseOperationContext(*pOutputContext);
//			Path.ToOneString(PStr);
//			OC+=" -\n";
//			OC+=PStr;
//			pOutputContext=&m_ContextStrings.GetUniqueString(OC);
//			PROperator.ChangeResultOperationContext(*pOutputContext);
//			PROperator.RestartPath();
//			_CSimpleVertexContainer* pOldResVertex=pResVertex;
//			pResVertex=&m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext,
//				_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//			*pResVertex=pOldResVertex->DoUnaryOpertationForWholeTree(PROperator,false);
//			if(FileToRegister!=NULL && !RegOnlyFinalResult)
//				AppendToFileCoefVertices(string(FileToRegister),CofId,sPower,*pOutputContext);
//		}
//		if(FileToRegister!=NULL && RegOnlyFinalResult)
//			AppendToFileCoefVertices(string(FileToRegister),CofId,sPower,*pOutputContext);
//		return 0;
//	}
//	catch(eVertexEntryUndifined)
//	{
//		return WrongContextName(Context);
//	}
//}

void _CMainCircuit::ClearDevelopingVerticesCache()
{
	for each(SUBCIRCUIT_VALUE_TYPE Value in m_SubcircuitMap)
		Value.second.second->ClearDevelopingVerticesCache();
	m_DevelopingVerticesCache.Clear();
}

void _CMainCircuit::ClearNumResultCaches()
{
	for each(SUBCIRCUIT_VALUE_TYPE Value in m_SubcircuitMap)
		Value.second.second->ClearNumResultCache();
}

long _CMainCircuit::GetFreePathTraitor(const string& BaseContext)
{
	long FFree=-1;
	PATH_TRAITORS::iterator _e=m_PathTraitors.end();
	while(m_PathTraitors.find(++FFree)!=_e);
	return m_PathTraitors.insert(PATH_TRAITORS::value_type(FFree,_CPathTraitor(this,BaseContext))).first->first;
}

//long _CMainCircuit::WriteDirectNmericalPolynomial(unsigned long CofId, 
//												  long hPathTraitor,
//												  const string& FileName, 
//												  bool Append, 
//												  _CSparsePolynomial** ppPolResult,
//												  bool TestDistribution, 
//												  long double UpperAngFrequency)
//{
//	_CPathTraitor& Traitor=GetExistedPathTraitor(hPathTraitor);
//	const string& Context=Traitor.StrContext2();
//	_CSparsePolynomial* pOutPolynomila=NULL;
//	long State=GetNumericalCofactorAndReminder(CofId,hPathTraitor,pOutPolynomila);
//	if(State!=0)
//		return State;
//	if(ppPolResult!=NULL)
//		*ppPolResult=pOutPolynomila;
//	ASSERTPDD(pOutPolynomila!=NULL);
//	if(CofId==0 || CofId>m_Cofactors.size())
//		return WrongCofIdError(CofId);
//	CofId--;
//	_CCofactor& Cof=*m_Cofactors[CofId];
//	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
//		Append?ios::out | ios::app : ios::out);
//	if(Append) file<<'\n';
//	size_t _order=pOutPolynomila->get_order();
//	string LP,PP;
//	if(!Context.empty()) 
//	{
//		LP="[";PP="]";
//	}
//	file<<Context<<LP;
//	Cof.WriteCofactorDescription(file);
//	file<<PP<<"=";
//	pOutPolynomila->WriteToTextStream(file);
//	file<<endl;
//	if(TestDistribution)
//	{
//		_CSparsePolynomial::MagnitudeDistribution MagDist;
//		pOutPolynomila->GetMagnitudeDistribution(MagDist,UpperAngFrequency);
//		file<<MagDist;
//		_CSparsePolynomial::EnergyDistribution EngDist;
//		pOutPolynomila->GetEnergyDistribution(EngDist,UpperAngFrequency);
//		file<<EngDist;
//	}
//	//if(pOutPolynomila->empty())
//	//	file<<"0";
//	//else
//	//	for(size_t i=_order;;i--)
//	//	{
//	//		const NumericType& Value=pOutPolynomila->get(i);
//	//		if(Value==EmptyFloat) 
//	//			if(i==0)
//	//				break;
//	//			else 
//	//				continue;
//	//		file<<((Value < EmptyFloat)?" - ":" + ");
//	//		if(i>0)
//	//			file<<"s^"<<i<<"*";
//	//		file<<abs(Value);
//	//		if(i==0) break;
//	//	}
//	//file<<'\n';
//	return 0;
//}

long _CMainCircuit::WriteDirectNmericalPolynomial(unsigned long CofId, 
												  long hPathTraitor,
												  const string& FileName, 
												  bool Append, 
												  _CSparsePolynomial* pPolResult,
												  bool TestDistribution, 
												  long double UpperAngFrequency)
{
	_CPathTraitor& Traitor=GetExistedPathTraitor(hPathTraitor);
	const string& Context=Traitor.StrContext2();
	_CSparsePolynomial OutPolynomila;
	long State=GetNumericalCofactorAndReminder(CofId,hPathTraitor,OutPolynomila);
	if(State!=0)
		return State;
	if(pPolResult!=NULL)
		*pPolResult=OutPolynomila;
	if(CofId==0 || CofId>m_Cofactors.size())
		return WrongCofIdError(CofId);
	CofId--;
	_CCofactor& Cof=*m_Cofactors[CofId];
	fstream file(m_ModelFileName.empty()?FileName.c_str():m_ModelFileName.c_str(),
		Append?ios::out | ios::app : ios::out);
	if(Append) file<<'\n';
	size_t _order=OutPolynomila.get_order();
	string LP,PP;
	if(!Context.empty()) 
	{
		LP="[";PP="]";
	}
	file<<Context<<LP;
	Cof.WriteCofactorDescription(file);
	file<<PP<<"=";
	OutPolynomila.WriteToTextStream(file);
	file<<endl;
	if(TestDistribution)
	{
		_CSparsePolynomial::MagnitudeDistribution MagDist;
		OutPolynomila.GetMagnitudeDistribution(MagDist,UpperAngFrequency);
		file<<MagDist;
		_CSparsePolynomial::EnergyDistribution EngDist;
		OutPolynomila.GetEnergyDistribution(EngDist,UpperAngFrequency);
		file<<EngDist;
	}
	return 0;
}

_CPathTraitor& _CMainCircuit::GetExistedPathTraitor(long TrId)
{
	PATH_TRAITORS::iterator it=m_PathTraitors.find(TrId);
	if(it==m_PathTraitors.end())
		RISEPDD(out_of_range,"Id not exists");
	return it->second;
}

const _CComponent* _CMainCircuit::AddParamReminder(long TrId, const string& ElementPath, const _CSubModelNumericPattern** ppCompPatt)
{
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	return Traitor.AddReminder(ElementPath,m_CirCompRedefEntry,ppCompPatt);
}

const _CComponent* _CMainCircuit::AddParamDivider(long TrId, const string& ElementPath,const _CSubModelNumericPattern** ppCompPatt)
{
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	return Traitor.AddDivider(ElementPath,m_CirCompRedefEntry,ppCompPatt);
}

void _CMainCircuit::SetApproximationCriterion(long TrId,
											  bool Exact,
											  const NumericType& Accurracy,
											  const NumericType& HighFrequency, 
											  const NumericType& LowFrequency,
											  short MaxPower,
											  short MinPower)
{
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	Traitor.SetApproximationCriterion(Exact,Accurracy,HighFrequency,LowFrequency,MaxPower,MinPower);
}

long _CMainCircuit::ReleasePathTraitor(long TrId)
{
	PATH_TRAITORS::iterator it=m_PathTraitors.find(TrId);
	if(it==m_PathTraitors.end())
		RISEPDD(out_of_range,"Id not exists");
	m_PathTraitors.erase(it);
	return 0;
}

long _CMainCircuit::ClearPathTraitor(long TrId)
{
	PATH_TRAITORS::iterator it=m_PathTraitors.find(TrId);
	if(it==m_PathTraitors.end())
		RISEPDD(out_of_range,"Id not exists");
	it->second.Clear();
	return 0;
}

long _CMainCircuit::TestPathTraitor(long TrId)
{
	_CPathTraitor& Traitor=GetExistedPathTraitor(TrId);
	const string& Context=Traitor.StrContext2();
	const string& Context2=Traitor.StrContext2();
	Traitor.SideFollower();
	const string& Context2a=Traitor.StrContext2();
	Traitor.PopLastLevel();
	if(Traitor.NextFollower())
	{
		const string& Context3=Traitor.StrContext2();
		Traitor.SideFollower();
		const string& Context3a=Traitor.StrContext2();
		Traitor.PopLastLevel();
		if(Traitor.SubFollower())
		{
			const string& Context4=Traitor.StrContext2();
			Traitor.SideFollower();
			const string& Context4a=Traitor.StrContext2();
			Traitor.PopLastLevel();
			const _CComponent* pComponent=Traitor.CurrentComponent();
			short Oper=Traitor.CurrentOper();
			_CPathTraitor::OPER_DATA OperData=Traitor.CurrentData();
			if(Traitor.NextFollower())
			{
				_CPathTraitor::COMP_RESULT CompRes=Traitor.Compare(pComponent);
				Traitor.SideFollower();
				const string& Context5a=Traitor.StrContext2();
				Traitor.PopLastLevel();
				if(Traitor.NextFollower())
				{
					_CPathTraitor::COMP_RESULT CompRes=Traitor.Compare(pComponent);
					Traitor.PopLastLevel();
				}
				Traitor.PopLastLevel();
			}
			if(Traitor.NextFollower())
			{

				const string& Context5=Traitor.StrContext2();
				if(Traitor.NextFollower())
				{
					const string& Context6=Traitor.StrContext2();
					Traitor.PopLastLevel();
				}
				Traitor.PopLastLevel();
			}
			Traitor.PopLastLevel();
		}
		Traitor.PopLastLevel();
	}
	return 0;
}

void _CMainCircuit::CacheTheResult(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket)
{
	ASSERTPDD(pMasterSocket==NULL);
	m_DirNumCache.CacheTheResult(pCacheContainer);
}

//void _CMainCircuit::RegisterResult(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket)
//{
//	ASSERTPDD(pMasterSocket==NULL);
//	return m_DirNumCache.RegisterResult(pCacheContainer);
//}

//_CDirectNumContainer* _CMainCircuit::FindExisting(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket)
//{
//	ASSERTPDD(pMasterSocket==NULL);
//	return dynamic_cast<_CDirectNumContainer*> (m_DirNumCache.FindExisting(pCacheContainer));
//}

void _CMainCircuit::SetGlobalModelFileName(const string& Name,bool ClearOld)
{
	m_ModelFileName=Name;
	if(ClearOld)
		fstream tmp(m_ModelFileName.c_str(),ios::out);
}

void _CMainCircuit::SetGlobalCircuitFileName(const string& Name,bool ClearOld) 
{
	m_CircuitFileName=Name;
	if(ClearOld)
		fstream tmp(m_CircuitFileName.c_str(),ios::out);
}

void _CMainCircuit::SetGlobalRaportFileName(const string& Name,bool ClearOld) 
{
	m_RaportFileName=Name;
	if(ClearOld)
		fstream tmp(m_RaportFileName.c_str(),ios::out);
}

_CFlatVertex* _CMainCircuit::PushIntoCache(_CFlatVertex* pObject)
{
	_CFlatVertex* pNewObject=NULL;
	try
	{
		pNewObject=m_FlatVertexCache.PushInCache(pObject,false);
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

long _CMainCircuit::DefineTransferFunctionSimp(const string& Name,unsigned  long NumeratorCof, unsigned  long  DenominatorCof)
{
	if(NumeratorCof==0 || NumeratorCof>m_Cofactors.size())
		return WrongCofIdError(NumeratorCof);
	if(DenominatorCof==0 || DenominatorCof>m_Cofactors.size())
		return WrongCofIdError(DenominatorCof);
	NumeratorCof--;DenominatorCof--;
	const _CCofactor* pNumCof=m_Cofactors[NumeratorCof];
	const _CCofactor* pDenCof=m_Cofactors[DenominatorCof];
	return m_FlatResultFactorizer.DefineTransferFunctionSimp(Name,(size_t)NumeratorCof,pNumCof,(size_t)DenominatorCof,pDenCof);
}

long _CMainCircuit::RaportTransferFunctionValuesLog(long long hTransfer, const string& Context, int LowestDec, int HighestDec, size_t NoOfPointsPerDec, bool Transfer2MagPhase)
{
	size_t NumId=0, DenId = 0;
	if (!m_FlatResultFactorizer.TranslateTrans2DenAndNum((size_t)hTransfer, NumId, DenId))
		return -1;
	NumericVector FreqVector;
	PrepareLogAngFreqSpace(FreqVector, LowestDec, HighestDec, NoOfPointsPerDec);
	return RaportTransferFunctionValues(m_FlatResultFactorizer.FuncName((size_t)hTransfer),Context,FreqVector, NumId, DenId, Transfer2MagPhase);
}
long _CMainCircuit::RaportTransferFunctionValuesLin(long long hTransfer, const string& Context, long double LowestAngFreq, long double HighestAngFreq, size_t NoOfGlobalPoints, bool Transfer2MagPhase)
{
	size_t NumId = 0, DenId = 0;
	if (!m_FlatResultFactorizer.TranslateTrans2DenAndNum((size_t)hTransfer, NumId, DenId))
		return -1;
	NumericVector FreqVector;
	PrepareLinAngFreqSpace(FreqVector, LowestAngFreq, HighestAngFreq, NoOfGlobalPoints);
	return RaportTransferFunctionValues(m_FlatResultFactorizer.FuncName((size_t)hTransfer), Context, FreqVector, NumId, DenId, Transfer2MagPhase);
}

long _CMainCircuit::RaportTransferFunctionValues(const string& Name, const string& Context, NumericVector& FreqVector, size_t NumId, size_t DenId, bool Transfer2MagPhase)
{
	_CSparsePolynomial NumPoly, DenPoly;
	long StateN = GetDirectNmericalPolynomial(NumId+1, Context, NumPoly);//Context
	if (StateN != 0)
		return StateN;
	long StateD = GetDirectNmericalPolynomial(DenId+1, Context, DenPoly);
	if (StateD != 0)
		return StateD;
	NormVectComplex NumComplex, DenComplex, TransComplex;
	DetermineFreqsValues(NumComplex, NumPoly, FreqVector);
	DetermineFreqsValues(DenComplex, DenPoly, FreqVector);
	ASSERTPDD(NumComplex.size()==DenComplex.size());
	auto nit = NumComplex.begin();
	auto _ne = NumComplex.end();
	auto dit = DenComplex.begin();
	auto _de = DenComplex.end();
	for (; nit != _ne && dit != _de; nit++, dit++)
		TransComplex.push_back((*nit)/(*dit));
	fstream file(m_ModelFileName.c_str(), ios::out | ios::app);
	file << Name<<"\t\t";
	if (Transfer2MagPhase)
		file << "Magnitude\t\tPhase"<<endl;
	else
		file << "Real\t\tImaginary"<<endl;
	auto fqit = FreqVector.begin();
	auto _fqe = FreqVector.end();
	auto tcit = TransComplex.begin();
	auto _tce = TransComplex.end();
	for (; fqit != _fqe && tcit != _tce; fqit++, tcit++)
	{
		file << *fqit << "\t\t";
		if (Transfer2MagPhase)
		{
			long double _real = tcit->real().ToLD();
			long double _imag = tcit->imag().ToLD();
			long double Mag = sqrt(_real*_real+_imag*_imag);
			long double Phase = atan2(_imag, _real)*180/acos(-1.0);
			file << Mag << "\t\t" << Phase << endl;
		}
		else
		{
			file << tcit->real() << "\t\t" << tcit->imag() << "*j" << endl;
		}
	}
	return 0;
}

//long _CMainCircuit::DeterminPathWeights(unsigned long CofId, unsigned short sPower, const string& Context, long double& NumericalResult,_CLSPath& Path)
//{
//	try
//	{
//		if(CofId>=m_Cofactors.size())
//			return WrongCofIdError(CofId);
//		//_CConstComponentPath& Path=m_ConstPathStore.GetConstantPath(Context);
//		const string* pUContext=m_ContextStrings.CheckContextName(Context);
//		if(pUContext==NULL)
//			return WrongContextName(Context);
//		_CPathWeightOperator Operator(this,*pUContext,&m_WeightCache);
//		_CSimpleVertexContainer& BaseVertex=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pUContext,
//			_CContextSExpandedCofactorValues::MUST_EXIST,sPower,CofId);
//		//_CWeightResult& Res=m_WeightRes.GetResult(*pUContext,CofId,sPower);
//		//if(Res.IsEmpty())
//		//	Res=BaseVertex.DoUnaryOpertationForWholeTree(Operator);
//		_CWeightResult Res=BaseVertex.DoUnaryOpertationForWholeTree(Operator);
//		NumericalResult=Res.GetValue();
//		long double RValue=0.0;
//		GetSPowerCoefficient(CofId,sPower,RValue,*pUContext);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		long double ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//
//		//long double pct=Res.GetValue()*100/GetSPowerCoefficient(CofId,sPower,*pUContext);
//		//_CLSPath ResPath/*(this)*/;
//		_CLeastSgnfcntPathOperator Operator2(this,*pUContext,m_WeightCache,Path);
//		BaseVertex.DoUnaryOpertationForWholeTree(Operator2);
//		string PStr;
//		Path.ToOneString(PStr);
//		string OC=(Context.empty()?EmptyString:Context+" -\n")+PStr;
//		//_CContextStringBuffer::PARENTS_VECTOR Vector(1,&*pUContext);
//		const string* pOutputContext=&m_ContextStrings.GetUniqueString(OC/*,Vector*/);
//		_CPathRemoveOperator Operator3(this,Path,*pOutputContext,*pUContext,&m_VCRCache);
//		_CSimpleVertexContainer& ResVertex=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext);
//		//Path.clear();
//
//		Operator.ChangeBaseOperationContext(*pOutputContext);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext2=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext);
//		Operator3.ChangeResultOperationContext(*pOutputContext2);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex2=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext2,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex2=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext2);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext2);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext2);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext3=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext2);
//		Operator3.ChangeResultOperationContext(*pOutputContext3);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex3=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext3,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex3=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext3);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext3);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext3);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext4=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext3);
//		Operator3.ChangeResultOperationContext(*pOutputContext4);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex4=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext4,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex4=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext4);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext4);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext4);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext5=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext4);
//		Operator3.ChangeResultOperationContext(*pOutputContext5);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex5=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext5,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex5=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext5);
//
//
//		Operator.ChangeBaseOperationContext(*pOutputContext5);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext5);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext6=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext5);
//		Operator3.ChangeResultOperationContext(*pOutputContext6);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex6=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext6,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex6=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext6);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext6);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext6);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext7=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext6);
//		Operator3.ChangeResultOperationContext(*pOutputContext7);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex7=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext7,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex7=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext7);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext7);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext7);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext8=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext7);
//		Operator3.ChangeResultOperationContext(*pOutputContext8);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex8=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext8,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex8=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext8);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext8);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext8);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext9=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext8);
//		Operator3.ChangeResultOperationContext(*pOutputContext9);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex9=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext9,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex9=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext9);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext9);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext9);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext10=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext9);
//		Operator3.ChangeResultOperationContext(*pOutputContext10);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex10=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext10,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex10=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext10);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext10);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext10);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext11=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext10);
//		Operator3.ChangeResultOperationContext(*pOutputContext11);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex11=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext11,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex11=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext11);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext11);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext11);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext12=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext11);
//		Operator3.ChangeResultOperationContext(*pOutputContext12);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex12=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext12,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex12=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext12);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext12);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext12);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext13=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext12);
//		Operator3.ChangeResultOperationContext(*pOutputContext13);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex13=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext13,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex13=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext13);
//
//		Operator.ChangeBaseOperationContext(*pOutputContext13);
//		Res=ResVertex.DoUnaryOpertationForWholeTree(Operator);
//		RegisterInFile("Circuit2.txt","Value",RValue);
//		RegisterInFile("Circuit2.txt","Increase",Res.GetValue());
//		ptc=Res.GetValue()*100/RValue;
//		RegisterInFile("Circuit2.txt","Ptc",ptc);
//		Path.clear();
//		Operator2.ChangeBaseOperationContext(*pOutputContext13);
//		ResVertex.DoUnaryOpertationForWholeTree(Operator2);
//		Path.ToOneString(PStr);
//		OC+=" -\n";
//		OC+=PStr;
//		const string* pOutputContext14=&m_ContextStrings.GetUniqueString(OC);
//		Operator3.ChangeBaseOperationContext(*pOutputContext13);
//		Operator3.ChangeResultOperationContext(*pOutputContext14);
//		Operator3.RestartPath();
//		_CSimpleVertexContainer& ResVertex14=
//			m_CSSimpleMainVertices.GetSExpandedCoef(*pOutputContext14,
//			_CContextSExpandedCofactorValues::FORCE_IF_NOT_EXIST,sPower,CofId);
//		ResVertex14=BaseVertex.DoUnaryOpertationForWholeTree(Operator3);
//		AppendToFileCoefVertices("Circuit2.txt",CofId,sPower,*pOutputContext14);
//		return 0;
//	}
//	catch(eVertexEntryUndifined e)
//	{
//		return WrongContextName(Context);
//	}
//}

