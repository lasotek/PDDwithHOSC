#include "StdAfx.h"
//#include "MainCircuit.h"
//#include "..\PDDErrorsAndConst.h"
//#include <algorithm>
#include "PDDEngine.h"
//#include "PrjException.h"
//#include <fstream>
//#include "SubCircuitSocket.h"
//#include "Vertex.h"
//#include <iomanip>
#include <math.h>
#include "UnaryOperators.h"
#include "CSimpleVertexContainer.h"

//_CWeightResult EmptyWeightResult;

//*********************************_CConstComplementStore
//_CConstComponentPath& _CConstComplementStore::GetConstantPath(const string& strPath)
//{
//	iterator fit=m_Store.find(strPath);
//	if(fit!=m_Store.end())
//		return fit->second;
//	_CConstComponentPath NewPath(m_pBaseCircuit);
//	pair<iterator,bool> res=m_Store.insert(value_type(strPath,NewPath));
//	if(!res.second)
//		RISEPDD(eIllegalOperation,"_CConstComplementStore::GetConstantPath - the problem with map");
//	fit=res.first;
//	_CConstComponentPath& ResPath=fit->second;
//	try
//	{
//		if(!strPath.empty())
//			ResPath.ParsePath(m_pBaseCircuit,strPath);
//		return ResPath;
//	}
//	catch(exception e)
//	{
//		m_Store.erase(fit);
//		throw;
//	}
//}
//*********************************_CRepeatContainer
unsigned long long _CRepeatContainer::GetHash() const
{
	unsigned long long Res=HASH_CORE;
	Res*=HASH_FACTOR;
	Res^=(unsigned long long)m_pValuePath;
	Res*=HASH_FACTOR;
	Res^=(unsigned long long)m_Res1;
	Res*=HASH_FACTOR;
	Res^=(unsigned long long)m_Res0;
	Res*=HASH_FACTOR;
	Res^=(unsigned long long)m_Multiplier;
	Res*=HASH_FACTOR;
	Res^=(unsigned long long)m_1Multiplier;
	Res*=HASH_FACTOR;
	Res^=(unsigned long long)m_0Multiplier;
	return Res;
}
const _CRepeatContainer& _CRepeatContainer::operator=(const _CRepeatContainer& Source)
{
	m_Index=Source.m_Index;
	m_pValuePath=Source.m_pValuePath;
	m_Res0=Source.m_Res0;
	m_Res1=Source.m_Res1;
	return *this;
}
string _CRepeatContainer::Get0NodeName() const
{
	return Mult2Str(m_Res0->m_Multiplier*m_0Multiplier)+m_Res0->GetMainNodeName();
}

string _CRepeatContainer::Get1NodeName() const
{
	return Mult2Str(m_Res1->m_Multiplier*m_1Multiplier)+m_Res1->GetMainNodeName();
}

string _CRepeatContainer::GetMainNodeName() const
{
	string Res(*m_pValuePath);
	if(m_Index<4)
	{
		string Primes(m_Index,'\'');
		Res+=Primes;
	}
	else
	{
		Res+='(';
		Res+=ulong2str(m_Index);
		Res+=')';
	}
	return Res;
}

string _CRepeatContainer::GetNodeName() const
{
	return Mult2Str(m_Multiplier)+GetMainNodeName();
}


//*********************************_CRepeatContainer
//*********************************_CVertex2TextOperator::_CRepeatCache
_CRepeatContainer* _CVertex2TextOperator::_CRepeatCache::Register(const _CRepeatContainer& Source)
{
	unsigned long long Hash=Source.GetHash();
	return &(insert(value_type(Hash,Source))->second);
}

bool _CVertex2TextOperator::_CRepeatCache::FindExisted(_CRepeatContainer** ppSource)
{
	unsigned long long Hash=(*ppSource)->GetHash();
	pair<iterator,iterator> Range=equal_range(Hash);
	for(iterator it=Range.first;it!=Range.second;it++)
		if(it->second==**ppSource)
		{
			*ppSource=&(it->second);
			return true;
		}
	return false;
}

//*********************************_CVertex2TextOperator::_CRepeatCache
//*********************************_CVertex2TextOperator

//void _CVertex2TextOperator::Mult2Str(string& Str, short Multiplier)
//{
//	if(Multiplier==-1)
//		Str="(-1)·";
//	else if(Multiplier!=1)
//		Str="("+long2str(Multiplier)+")·";
//	else
//		Str.clear();
//}

 _CRepeatContainer* EmptyRepeatContainer=NULL;

_CVertex2TextOperator::~_CVertex2TextOperator()
{
	if(m_RepCounter>0)
		m_stream<<"There are "<<m_RepCounter<<" repetitions\n";
}

_CVertex2TextOperator::MyType _CVertex2TextOperator::DoTheOperation(const _CVertex& Vertex, 
	const _CSimpleVertexContainer& SVertex, const MyType& Res1, const MyType& Res0)
{
	if(SVertex.Is01M1())
		throw eIllegalOperation("DoThe01Operation: Vertex cannot be 0,1 nor -1");
	string NodeName=GetInstancePrefix();
	if(!NodeName.empty())
		NodeName+=".";
	NodeName+=SVertex.iVertex2string();
	unsigned long* pCounter=NULL;
	const string& cNodeName=m_ClearPaths.GetUniqueString(NodeName,&pCounter);
	_CRepeatContainer TestCont(&cNodeName,Res1,Res0,SVertex.GetLocalMultiplier(),Vertex.Get1SuccMultiplier(),Vertex.Get0SuccMultiplier());
	_CRepeatContainer* pResCont=&TestCont;
	if(m_RepeatCache.FindExisted(&pResCont))
		return pResCont;
	TestCont.SetIndex((*pCounter)++);
	const _CSubCircuitSocket* pSocket=GetCurrentSocket();
	string OutStr=TestCont.GetNodeName()+": ";
	if(pSocket!=NULL)
	{
		OutStr+=GetInstancePrefix();
		OutStr+='.';
	}
	OutStr+=Vertex.GetComponent()->GetParameterName();
	//string sMult1, sMult0;
	//Mult2Str(sMult1,Pop1Multiplier());
	//Mult2Str(sMult0,Pop0Multiplier());
	//Mult2Str(sMult1,Get1Multiplier());
	//Mult2Str(sMult0,Get0Multiplier());
	//short hMult=GetRealMultiplier(Vertex.Get1Succ());
	//m_stream<<"("<<sMult1<<Res1<<","<<sMult0<<Res0<<")"<<endl;
	OutStr+='[';
	//m_OutStr+=sMult1;
	OutStr+=TestCont.Get1NodeName();
	OutStr+=", ";
	//m_OutStr+=sMult0;
	OutStr+=TestCont.Get0NodeName();
	OutStr+=']';
	m_stream<<OutStr<<endl;
//	//short LocalMultiplier=SVertex.GetLocalMultiplier();
//	//string prefix;
//	//if(LocalMultiplier==-1)
//	//	prefix='-';
//	//else if(LocalMultiplier!=1)
//	//{
//	//	prefix=string("(")+ulong2str(LocalMultiplier)+")*";
//	//	//m_stream<<"("<<LocalMultiplier<<")*";
//	//}
//	//string Mult;
//	//Mult2Str(m_sMult,SVertex.GetLocalMultiplier());
//	return /*Mult+*/NodeName;
	return m_RepeatCache.Register(TestCont);
}

//void _CVertex2TextOperator::FinishDoTheOperation(string& Result)
//{
//#ifdef _DEBUG
//	if(m_TestCache.find(m_OutStr)==m_TestCache.end())
//	{
//		m_TestCache.insert(m_OutStr);
//		//m_stream<<OutStr<<endl;
//	}
//	else
//	{
//		m_OutStr+='r';
//		m_RepCounter++;
//		//m_RepCache.insert(OutStr);
//	}
//#endif
//	m_stream<<m_OutStr<<endl;
//	//MultiplierModyfication(Result);	
//}

void _CVertex2TextOperator::ModifyByMultiplier(MyType& Result, short Multiplier)
{
	if(Multiplier==1 || Multiplier==0)
		return;
	_CRepeatContainer NewRes(*Result);
	NewRes.SetMultiplier(Multiplier);
	_CRepeatContainer* pNewRes=&NewRes;
	if(m_RepeatCache.FindExisted(&pNewRes))
	{
		Result=pNewRes;
		return;
	}
	Result=m_RepeatCache.Register(NewRes);
	//string str;
	//Mult2Str(str,Multiplier);
	//Result=str+Result;
}

_CVertex2TextOperator::MyType _CVertex2TextOperator::DoThe01Operation(const _CSimpleVertexContainer& Vertex)
{
	if(Vertex.Is0())
		return m_RepeatCache.Get0Desc();
	if(Vertex.Is1())
		return m_RepeatCache.Get1Desc();
	if(Vertex.IsM1())
		return m_RepeatCache.GetM1Desc();
	RISEPDD(eIllegalOperation,"DoThe01Operation: Vertex should be 0,1 or -1");
}
//void _CVertex2TextOperator::WritePrefix()
//{
//	for each(const _CSubCircuitSocket* pSock in m_SubInstances)
//	{
//		m_stream<<(pSock->GetParameterName());
//		m_stream<<".";
//	}
//}
//string _CVertex2TextOperator::GetInstancePrefix()
//{
//	string Res;
//	for each(const _CSubCircuitSocket* pSock in m_SubInstances)
//	{
//		Res+=pSock->GetParameterName();
//		Res+='.';
//	}
//	return Res;
//}

//*********************************_CSPowerCoeffOperator
//NumericType _CSPowerCoeffOperator::DoTheOperation(const _CVertex& Vertex, 
//	const _CSimpleVertexContainer& SVertex, const NumericType& Res1, 
//	const NumericType& Res0)
//{
//    NumericType Res=0.0;
//	if(Res1!=0.0)
//	{
//		NumericType Value=GetRealCompValue(Vertex);
//		short M1=1;//Pop1Multiplier();
//		//short M1=Get1Multiplier();
//		Res+=(Res1*Value*M1);
//	}
//	if(Res0!=0.0)
//	{
//		short M0=1;//Pop0Multiplier();
//		//short M0=Get0Multiplier();
//		Res+=(Res0*M0);
//	}
//	return Res/**SVertex.GetLocalMultiplier()*/;
//}
//
//NumericType _CSPowerCoeffOperator::DoThe01Operation(const _CSimpleVertexContainer& Vertex)
//{
//	if(!Vertex.Is01M1())
//		RISEPDD(eIllegalOperation,"_CSPowerCoeffOperator::DoThe01Operation can be used only for 0,1, or -1 terminal vertex");
//	NumericType Res=(long double)Vertex.GetLocalVertex();
//	short Multiplier=Vertex.GetLocalMultiplier();
//	//switch(Vertex.GetLocalVertex())
//	//{
//	//case -1: Res=-1; break;
//	//case 1: Res=1; break;
//	//case 0: Res=0; break;
//	//}
//	return (Res*Multiplier);
//	//return (long double)(Vertex.GetLocalVertex()/**Vertex.GetLocalMultiplier()*/);
//}


/////////////////////////////////////

////*************************_CNumericalSCoeff
//bool _CNumericalSCoeff::GetSCoeff(unsigned Index, long double& Value)
//{
//	if(Index>=size())
//		return false;
//	_CNumericalSEntry& SE=at(Index);
//	if(!SE.first)
//		return false;
//	Value=SE.second;
//	return true;
//}
//void _CNumericalSCoeff::SetSCoeff(unsigned Index, const long double& Value)
//{
//	if(Index>=size())
//		resize(Index+1,_CNumericalSEntry(false,0.0));
//	_CNumericalSEntry Entry(true,Value);
//	at(Index)=Entry;
//}

//*********************************_CNumericalCofactorReminderOperator

//void _CNumericalCofactorReminderOperator::PushState(_CSimpleVerticesMaps* pTerminalVertices,_CSubCircuitSocket* pSocket)
//{
//	_CSPowerCoeffOperator::PushState(pTerminalVertices,pSocket);
//	m_CurrentPath.SubmergeSocket(pSocket);
//}
//
//void _CNumericalCofactorReminderOperator::PopState()
//{
//	_CSPowerCoeffOperator::PopState();
//	m_CurrentPath.EmergeSocket();
//}

//short _CNumericalCofactorReminderOperator::CompVertex(const _CVertex& Vertex,const _CSimpleVertexContainer& SVertex)
//{
//	if(SVertex.Is01M1())
//		return 1;
//	const _CComponent* pComp=Vertex.GetComponent();
//	return m_CurrentPath.SetComponent(pComp).Comp(m_Suplement.GetPath());
//}
////*********************************_CNumericalCofactorOperator
//NumericType _CNumericalCofactorOperator::Operate(const _CVertex& Vertex, 
//												 const _CSimpleVertexContainer& SVertex)
//{
//	//const _CComponent* pComp=Vertex.GetComponent();
//	switch(CompVertex(Vertex,SVertex))
//	{
//	case -1:
//		{
//			//long double Res0=Vertex.Get0Succ().DoUnaryOpertationForWholeTree(*this,true);
//			//string Vertex0Path,Vertex1Path;
//			NumericType Res0=TraceDescendant(true,Vertex);
//			NumericType Res1=TraceDescendant(false,Vertex);
//			NumericType Res=DoTheOperation(Vertex,SVertex,Res1,Res0);
//			return Res;
//		}
//	case 1:
//		return 0.0;
//	case 0: 
//		{
//			_CSPowerCoeffOperator VPower(*this);
//			NumericType Res=VPower.TraceDescendant(false,Vertex);
//			Res*=VPower.Pop1Multiplier();
//			return Res;
//		}
//	default: ;
//	}
//	RISEPDD(eIllegalOperation,"_CNumericalCofactorOperator::Determine1DescendantRes - ouch!");
//}
//NumericType _CNumericalCofactorOperator::Operate(const _CVertex& Vertex, 
//												 const _CSimpleVertexContainer& SVertex,
//												 _CAuxBasePlatorm* pAuxBasePlatform)
//{
//	bool MaybeExisted=m_PrevValueAlreadyDefined==Maybe;
//	const _CSimpleVertexContainer* p0Vertex=ANY_VERTEX;
//	unsigned Succ0AuxId=0;
//	unsigned Succ1AuxId=0;
//	const _CSimpleVertexContainer* p1Vertex=ANY_VERTEX;
//	//const _CComponent* pComp=Vertex.GetComponent();
//	switch(CompVertex(Vertex,SVertex))
//	{
//	case -1:
//		{
//			//long double Res0=Vertex.Get0Succ().DoUnaryOpertationForWholeTree(*this,true);
//			//string Vertex0Path,Vertex1Path;
//			MVIDS Vertex0MVId, Vertex1MVId;
//			_CExactOutContainer* pPrev1ExactOutContainer=NULL;
//			_CExactOutContainer* pPrev0ExactOutContainer=NULL;
//			NumericType Res0=TraceDescendant(true,Vertex,p0Vertex,Succ0AuxId,Vertex0MVId,pPrev0ExactOutContainer);
//			NumericType Res1=TraceDescendant(false,Vertex,p1Vertex,Succ1AuxId,Vertex1MVId,pPrev1ExactOutContainer);
//			NumericType Res=0.0;
//			if(MaybeExisted)
//			{
//
//				//if(SimilarVertList.FitTheResult(m_CurrAuxId,Res,m_CurrentMVIds,p1Vertex,Vertex1MVId,
//				//	Succ1AuxId,p0Vertex,Vertex0MVId,Succ0AuxId))
//				//{
//				//	//NotifyAlreadyDefined();
//				//	return Res;
//				//}
//			}
//			else
//				m_CurrAuxId=0;
//			Res=DoTheOperation(Vertex,SVertex,Res1,Res0);
//			m_pDescSimpleVertex1=p1Vertex;
//			m_Succ0AuxId=Succ0AuxId;
//			m_Succ1AuxId=Succ1AuxId;
//			m_pDescSimpleVertex0=p0Vertex;
//			m_Vert0MVId=Vertex0MVId;
//			m_Vert1MVId=Vertex1MVId;
//			m_pVert0ExactContainer=pPrev0ExactOutContainer;
//			m_pVert1ExactContainer=pPrev1ExactOutContainer;
//			//m_Vertex0Path=Vertex0Path;
//			//m_Vertex1Path=Vertex1Path;
//			return Res;
//		}
//	case 1:
//		m_pDescSimpleVertex1=ANY_VERTEX;
//		m_pDescSimpleVertex0=ANY_VERTEX;
//		return 0.0;
//	case 0: 
//		{
//			_CSPowerCoeffOperator VPower(*this);
//			NumericType Res=VPower.TraceDescendant(false,Vertex,m_pDescSimpleVertex1,m_Succ1AuxId,m_Vert1MVId,m_pVert1ExactContainer);
//			m_pPreviousRealSimpleVertex=VPower.m_pPreviousRealSimpleVertex;//a
//			m_Succ1AuxId=m_CurrAuxId=VPower.GetAuxId();
//			m_pDescSimpleVertex0=ANY_VERTEX;
//			Res*=VPower.Pop1Multiplier();
//			return Res;
//		}
//	default: ;
//	}
//	RISEPDD(eIllegalOperation,"_CNumericalCofactorOperator::Determine1DescendantRes - ouch!");
//}

//*********************************_CNumericalReminderOperator

//NumericType _CNumericalReminderOperator::Operate(const _CVertex& Vertex, 
//												 const _CSimpleVertexContainer& SVertex,
//												 _CAuxBasePlatorm* pAuxBasePlatform)
//{
//	bool MaybeExisted=m_PrevValueAlreadyDefined==Maybe;
//	const _CSimpleVertexContainer* p0Vertex=ANY_VERTEX;
//	const _CSimpleVertexContainer* p1Vertex=ANY_VERTEX;
//	unsigned Succ0AuxId=0;
//	unsigned Succ1AuxId=0;
//	//const _CComponent* pComp=Vertex.GetComponent();
//	switch(CompVertex(Vertex,SVertex))
//	{
//	case -1:
//		{
//			//string Vertex0Path,Vertex1Path;
//			MVIDS Vertex0MVId,Vertex1MVId;
//			_CExactOutContainer* pVertex0EcactOutContainer=NULL;
//			_CExactOutContainer* pVertex1EcactOutContainer=NULL;
//			NumericType Res0=TraceDescendant(true,Vertex,p0Vertex,Succ0AuxId,Vertex0MVId,pVertex0EcactOutContainer);
//			NumericType Res1=TraceDescendant(false,Vertex,p1Vertex,Succ1AuxId,Vertex1MVId,pVertex1EcactOutContainer);
//			NumericType Res=0.0;
//			if(MaybeExisted)
//			{
//				//if(SimilarVertList.FitTheResult(m_CurrAuxId,Res,m_CurrentMVIds,p1Vertex,Vertex1MVId,Succ1AuxId,
//				//	p0Vertex,Vertex0MVId,Succ0AuxId))
//				//{
//				//	//NotifyAlreadyDefined();
//				//	return Res;
//				//}
//			}
//			else
//				m_CurrAuxId=0;
//			Res=DoTheOperation(Vertex,SVertex,Res1,Res0);
//			m_pDescSimpleVertex1=p1Vertex;
//			m_Succ0AuxId=Succ0AuxId;
//			m_Succ1AuxId=Succ1AuxId;
//			m_pDescSimpleVertex0=p0Vertex;
//			m_Vert0MVId=Vertex0MVId;
//			m_Vert1MVId=Vertex1MVId;
//			m_pVert0ExactContainer=pVertex0EcactOutContainer;
//			m_pVert1ExactContainer=pVertex1EcactOutContainer;
//			//m_Vertex0Path=Vertex0Path;
//			//m_Vertex1Path=Vertex1Path;
//			return Res;
//		}
//	case 1: 
//		{
//			_CSPowerCoeffOperator VPower(*this);
//			NumericType Res=SVertex.DoUnaryOpertationForWholeTree(VPower,false);
//			VPower.CopyPreviousDescendants(*this);
//			m_pDescSimpleVertex0=m_pDescSimpleVertex1=ANY_VERTEX;
//			Res*=VPower.Pop1Multiplier();
//			return Res;
//
//		}
//	case 0: 
//		{
//			_CSPowerCoeffOperator VPower(*this);
//			//long double Res=Vertex.Get0Succ().DoUnaryOpertationForWholeTree(VPower,true);
//			NumericType Res=VPower.TraceDescendant(true,Vertex,m_pDescSimpleVertex0,m_Succ0AuxId,m_Vert0MVId,m_pVert0ExactContainer);//a
//			m_pPreviousRealSimpleVertex=VPower.m_pPreviousRealSimpleVertex;
//			m_Succ1AuxId=m_CurrAuxId=VPower.GetAuxId();
//			m_pDescSimpleVertex1=ANY_VERTEX;
//			Res*=VPower.Pop0Multiplier();
//			return Res;
//		}
//	default: ;
//	}
//	RISEPDD(eIllegalOperation,"_CNumericalCofactorOperator::Determine1DescendantRes - ouch!");
//}
//NumericType _CNumericalReminderOperator::Operate(const _CVertex& Vertex, 
//												 const _CSimpleVertexContainer& SVertex)
//{
//	switch(CompVertex(Vertex,SVertex))
//	{
//	case -1:
//		{
//			NumericType Res0=TraceDescendant(true,Vertex);
//			NumericType Res1=TraceDescendant(false,Vertex);
//			NumericType Res=0.0;
//			return DoTheOperation(Vertex,SVertex,Res1,Res0);
//		}
//	case 1: 
//		{
//			_CSPowerCoeffOperator VPower(*this);
//			return SVertex.DoUnaryOpertationForWholeTree(VPower,false)*VPower.Pop1Multiplier();
//
//		}
//	case 0: 
//		{
//			_CSPowerCoeffOperator VPower(*this);
//			//return Vertex.Get0Succ().DoUnaryOpertationForWholeTree(VPower,true);
//			return VPower.TraceDescendant(true,Vertex)*VPower.Pop0Multiplier();
//		}
//	default: ;
//	}
//	RISEPDD(eIllegalOperation,"_CNumericalCofactorOperator::Determine1DescendantRes - ouch!");
//}

//********************************_CCofRemResultCache
//_CCofRemResultCache::iterator _CCofRemResultCache::GetNumericalSItertator(_CConstComponentPath& CompPath,bool IsCofactor)
//{
// 	static _CNumericalSCofactors Empty;
//	_CCofactorReminderSuplement Sup(CompPath,IsCofactor);
//	iterator it=FindIt(Sup);
//	if(it==end())
//		it=insert(value_type(Sup,Empty));
//	return it;
//}
//
//_CCofRemResultCache::iterator _CCofRemResultCache::FindIt(_CCofactorReminderSuplement& Sup)
//{
//	pair<iterator,iterator> Range=equal_range(Sup);
//	for(iterator it=Range.first;it!=Range.second;it++)
//		if(it->first==Sup)
//			return it;
//	return end();
//}
//************************************_CSuplementStorage
_CSuplementStorage::~_CSuplementStorage()
{
	for each(value_type value in m_Map)
		delete value.second;
}

const _CCofactorReminderSuplement& _CSuplementStorage::GetCofRemSuplement(_CConstComponentPath& SourcePath, bool IsCofactor,const string& BaseContext)
{
	string StrId=(IsCofactor?CofString:RemString)+SourcePath.GetPathDescr();
	iterator it=m_Map.find(StrId);
	if(it==m_Map.end())
	{
		_CCofactorReminderSuplement* pSuplement=new _CCofactorReminderSuplement(SourcePath,IsCofactor,BaseContext);
		try
		{
			pair<iterator,bool> Res=m_Map.insert(value_type(StrId,pSuplement));
			if(Res.second)
				it=Res.first;
			else
				RISEPDD(eUnexpectedCondition,"Problem with m_Map");
		}
		catch(exception)
		{
			delete pSuplement;
			throw;
		}
	}
	return *(static_cast<_CCofactorReminderSuplement*>(it->second));
}

//************************************_CResVertexOperator
void _CResVertexOperator::PostOperateSubCircuitResult(unsigned long EntryId, unsigned long PosId, _CSimpleVertexContainer& NewRes)
{
	if(NewRes.Is01M1())
		return;
	GetCurrentCircuit()->RegisterAndPreparInputVertex(EntryId,PosId,/*m_SubInstances.back()*/
		m_CurrentPath.GetCurrentSocket(),
		m_CurrentTerminalVertices.back(),GetCurrentMVIds(),NewRes,GetOperationContext(),GetPostTerminalDescendantContext());
}

void _CResVertexOperator::PostOperateTerminals(long long VertId, _CSimpleVertexContainer& NewRes)
{
	if(!NewRes.Is0())
	{
		//_CSimpleVerticesMaps* pCurrentMap=m_CurrentTerminalVertices.back();
		_CSubCircuitVerticesMap* pCurrentMap=m_CurrentTerminalVertices.back();
		VertId=pCurrentMap->put(VertId,GetOperationContext(),NewRes,
			GetPostTerminalDescendantContext(),GetBaseContext());
		//NewRes=_CSimpleVertexContainer(VertId,m_SubInstances.back(),pCurrentMap);
		NewRes=_CSimpleVertexContainer(VertId);
	}
}
//_CSimpleVertexContainer _CResVertexOperator::Operate(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex,
//													 _CAuxBasePlatorm* pAuxBasePlatform)
//{
//	ASSERTPDD(m_Stage!=Normal);
//	if(m_Stage==Enumeration)
//	{
//		const _CSimpleVertexContainer* pDescSimpleVertex0=ANY_VERTEX;
//		unsigned Succ0AuxId=0;
//		//string Vertex1Path, Vertex0Path;
//		MVIDS Vertex1MVId, Vertex0MVId;
//		const _CSimpleVertexContainer* pDescSimpleVertex1=ANY_VERTEX;
//		unsigned Succ1AuxId=0;
//		_CExactOutContainer* pVert0ExactOutContainer=NULL;
//		_CExactOutContainer* pVert1ExactOutContainer=NULL;
//		TraceDescendant(true,Vertex,pDescSimpleVertex0,Succ0AuxId,Vertex0MVId,pVert0ExactOutContainer);
//		TraceDescendant(false,Vertex,pDescSimpleVertex1,Succ1AuxId,Vertex1MVId,pVert1ExactOutContainer);//a
//		m_pDescSimpleVertex0=pDescSimpleVertex0;
//		m_Succ0AuxId=Succ0AuxId;
//		m_pDescSimpleVertex1=pDescSimpleVertex1;
//		m_Succ1AuxId=Succ1AuxId;
//		m_Vert0MVId=Vertex0MVId;
//		m_Vert1MVId=Vertex1MVId;
//		m_pVert0ExactContainer=pVert0ExactOutContainer;
//		m_pVert1ExactContainer=pVert1ExactOutContainer;
//	}
//	return SVertex;
//}
_CSimpleVertexContainer _CResVertexOperator::Operate(const _CVertex& Vertex, const _CSimpleVertexContainer& SVertex)
{
	ASSERTPDD(m_Stage!=Normal);
	if(m_Stage==Enumeration)
	{
		TraceDescendant(true,Vertex);
		TraceDescendant(false,Vertex);
	}
	return SVertex;
}

_CSimpleVertexContainer _CResVertexOperator::DoTheOperation(const _CVertex& Vertex, 
	const _CSimpleVertexContainer& SVertex, const _CSimpleVertexContainer& Res1, 
	const _CSimpleVertexContainer& Res0)
{
	if(Res1==Vertex.Get1Succ() && Res0==Vertex.Get0Succ())
		return SVertex;
	if(Res1.Is0())
		return Res0;
	return GetCurrentCircuit()->GetSimpleVertexId(Vertex.GetComponent(),Res1,Res0);
}

//*********************************_CVertexCofactorReminderOperator
short _CVertexCofactorReminderOperator::CompVertex(const _CVertex& Vertex,const _CSimpleVertexContainer& SVertex)
{
	if(SVertex.Is01M1())
		return 1;
	const _CComponent* pComp=Vertex.GetComponent();
	return m_CurrentPath.SetComponent(pComp).Comp(m_Suplement.GetPath());
}
//*********************************_CVertexCofactorReminderOperator

//*********************************_CVertexCofactorOperator
//_CSimpleVertexContainer _CVertexCofactorOperator::Operate(const _CVertex& Vertex, 
//														  const _CSimpleVertexContainer& SVertex,
//														  _CAuxBasePlatorm* pAuxBasePlatform)
//{
//	if(m_Stage!=Normal)
//		return _CResVertexOperator::Operate(Vertex,SVertex,pAuxBasePlatform);
//	bool MaybeExisted=m_PrevValueAlreadyDefined==Maybe;
//	const _CSimpleVertexContainer* p0Vertex=ANY_VERTEX;
//	unsigned Succ0AuxId=0;
//	unsigned Succ1AuxId=0;
//	const _CSimpleVertexContainer* p1Vertex=ANY_VERTEX;
//	//const _CComponent* pComp=Vertex.GetComponent();
//	//switch(m_CurrentPath.SetComponent(pComp).Comp(m_Suplement.GetPath()))
//	switch(CompVertex(Vertex,SVertex))
//	{
//	case -1:
//		{
//			//string Vertex1Path,Vertex0Path;
//			MVIDS Vertex1MVId,Vertex0MVId;
//			_CExactOutContainer* pVert0Exact0Container=NULL;
//			_CExactOutContainer* pVert1Exact0Container=NULL;
//			_CSimpleVertexContainer  Res0=TraceDescendant(true,Vertex,p0Vertex,Succ0AuxId,Vertex0MVId,pVert0Exact0Container);
//			_CSimpleVertexContainer  Res1=TraceDescendant(false,Vertex,p1Vertex,Succ1AuxId,Vertex1MVId,pVert1Exact0Container);//a
//			_CSimpleVertexContainer Res;
//			if(MaybeExisted)
//			{
//				//if(SimilarVertList.FitTheResult(m_CurrAuxId,Res,m_CurrentMVIds,p1Vertex,Vertex1MVId,Succ1AuxId,
//				//	p0Vertex,Vertex0MVId,Succ1AuxId))
//				//{
//				//	//NotifyAlreadyDefined();
//				//	return Res;
//				//}
//			}
//			else
//				m_CurrAuxId=0;
//			Res=DoTheOperation(Vertex,SVertex,Res1,Res0);
//			m_pDescSimpleVertex1=p1Vertex;
//			m_Succ0AuxId=Succ0AuxId;
//			m_Succ1AuxId=Succ1AuxId;
//			m_pDescSimpleVertex0=p0Vertex;
//			m_Vert0MVId=Vertex0MVId;
//			m_Vert1MVId=Vertex1MVId;
//			m_pVert0ExactContainer=pVert0Exact0Container;
//			m_pVert1ExactContainer=pVert1Exact0Container;
//			return Res;
//
//		}
//	case 1:
//		//if(MaybeExisted)
//		//	NotifyAlreadyDefined();
//		m_pDescSimpleVertex1=ANY_VERTEX;
//		m_pDescSimpleVertex0=ANY_VERTEX;
//		return ZeroSVertex;
//	case 0: 
//		{
//			const _CSimpleVertexContainer& Succ1=FixSubTree(Vertex.Get1Succ(),false);
//			//if(MaybeExisted)
//			//	NotifyAlreadyDefined();
//			short ExtraMultiplier=Pop1Multiplier();
//			m_pDescSimpleVertex1=m_pPreviousRealSimpleVertex;
//			m_Succ1AuxId=m_CurrAuxId;
//			m_pDescSimpleVertex0=ANY_VERTEX;
//			if(ExtraMultiplier==1)
//				return Succ1;
//			else
//				return _CSimpleVertexContainer(Succ1,ExtraMultiplier);
//		}
//	default: ;
//	}
//	RISEPDD(eIllegalOperation,"_CVertexCofactorOperator::Determine1DescendantRes - ouch!");
//	//return _CSimpleVertexContainer();
//}
_CSimpleVertexContainer _CVertexCofactorOperator::Operate(const _CVertex& Vertex, 
														  const _CSimpleVertexContainer& SVertex)
{
	if(m_Stage!=Normal)
		return _CResVertexOperator::Operate(Vertex,SVertex);
	switch(CompVertex(Vertex,SVertex))
	{
	case -1:
		{
			_CSimpleVertexContainer  Res0=TraceDescendant(true,Vertex);
			_CSimpleVertexContainer  Res1=TraceDescendant(false,Vertex);
			return DoTheOperation(Vertex,SVertex,Res1,Res0);

		}
	case 1:
		return ZeroSVertex;
	case 0: 
		{
			const _CSimpleVertexContainer& Succ1=FixSubTree(Vertex.Get1Succ(),false);
			short ExtraMultiplier=Pop1Multiplier();
			if(ExtraMultiplier==1)
				return Succ1;
			else
				return _CSimpleVertexContainer(Succ1,ExtraMultiplier);
		}
	default: ;
	}
	RISEPDD(eIllegalOperation,"_CVertexCofactorOperator::Determine1DescendantRes - ouch!");
	//return _CSimpleVertexContainer();
}
_CSimpleVertexContainer _CVertexCofactorOperator::DoThe01Operation(const _CSimpleVertexContainer& Vertex)
{
	return ZeroSVertex;
}

//**********************************_CVertexReminderOperator
//_CSimpleVertexContainer _CVertexReminderOperator::Operate(const _CVertex& Vertex, 
//														  const _CSimpleVertexContainer& SVertex,
//														  _CAuxBasePlatorm* pAuxBasePlatform)
//{
//	if(m_Stage!=Normal)
//		return _CResVertexOperator::Operate(Vertex,SVertex,pAuxBasePlatform);
//	bool MaybeExisted=m_PrevValueAlreadyDefined==Maybe;
//	const _CSimpleVertexContainer* p0Vertex=ANY_VERTEX;
//	unsigned Succ0AuxId=0;
//	unsigned Succ1AuxId=0;
//	const _CSimpleVertexContainer* p1Vertex=ANY_VERTEX;
//	//const _CComponent* pComp=Vertex.GetComponent();
//	switch(CompVertex(Vertex,SVertex))
//	//switch(m_CurrentPath.SetComponent(pComp).Comp(m_Suplement.GetPath()))
//	{
//	case -1:
//		{
//			//string Vertex0Path,Vertex1Path;
//			MVIDS Vertex0MVId,Vertex1MVId;
//			_CExactOutContainer* pVert0ExactOutContainer=NULL;
//			_CExactOutContainer* pVert1ExactOutContainer=NULL;
//			_CSimpleVertexContainer  Res0=TraceDescendant(true,Vertex,p0Vertex,Succ0AuxId,Vertex0MVId,pVert0ExactOutContainer);
//			_CSimpleVertexContainer  Res1=TraceDescendant(false,Vertex,p1Vertex,Succ1AuxId,Vertex1MVId,pVert1ExactOutContainer);
//			_CSimpleVertexContainer Res;
//			if(MaybeExisted)
//			{
//				//if(SimilarVertList.FitTheResult(m_CurrAuxId,Res,m_CurrentMVIds,p1Vertex,Vertex1MVId,Succ1AuxId,
//				//	p0Vertex,Vertex0MVId,Succ1AuxId))
//				//{
//				//	//NotifyAlreadyDefined();
//				//	return Res;
//				//}
//			}
//			else
//				m_CurrAuxId=0;
//			Res=DoTheOperation(Vertex,SVertex,Res1,Res0);
//			m_pDescSimpleVertex1=p1Vertex;
//			m_Succ0AuxId=Succ0AuxId;
//			m_Succ1AuxId=Succ1AuxId;
//			m_pDescSimpleVertex0=p0Vertex;
//			m_Vert0MVId=Vertex0MVId;
//			m_Vert1MVId=Vertex1MVId;
//			m_pVert1ExactContainer=pVert1ExactOutContainer;
//			m_pVert0ExactContainer=pVert0ExactOutContainer;
//			return Res;
//		}
//	case 1: 
//		{
//			m_pDescSimpleVertex0=m_pDescSimpleVertex1=ANY_VERTEX;
//			return FixSubTree(SVertex,false);
//		}
//	case 0: 
//		{
//			const _CSimpleVertexContainer& Succ0=FixSubTree(Vertex.Get0Succ(),false);
//			short ExtraMultiplier=Pop0Multiplier();
//			m_pDescSimpleVertex0=m_pPreviousRealSimpleVertex;
//			m_Succ0AuxId=m_CurrAuxId;
//			m_pDescSimpleVertex1=ANY_VERTEX;
//			if(ExtraMultiplier==1 || ExtraMultiplier==0)
//				return Succ0;
//			else
//				return _CSimpleVertexContainer(Succ0,ExtraMultiplier);
//		}
//	default: ;
//	}
//	RISEPDD(eIllegalOperation,"_CVertexReminderOperator::Determine1DescendantRes - ouch!");
//}
_CSimpleVertexContainer _CVertexReminderOperator::Operate(const _CVertex& Vertex, 
														  const _CSimpleVertexContainer& SVertex)
{
	if(m_Stage!=Normal)
		return _CResVertexOperator::Operate(Vertex,SVertex);

	switch(CompVertex(Vertex,SVertex))
	//switch(m_CurrentPath.SetComponent(pComp).Comp(m_Suplement.GetPath()))
	{
	case -1:
		{
			_CSimpleVertexContainer  Res0=TraceDescendant(true,Vertex);
			_CSimpleVertexContainer  Res1=TraceDescendant(false,Vertex);
			_CSimpleVertexContainer Res;
			return DoTheOperation(Vertex,SVertex,Res1,Res0);
		}
	case 1: 
		{
			return FixSubTree(SVertex,false);
		}
	case 0: 
		{
			const _CSimpleVertexContainer& Succ0=FixSubTree(Vertex.Get0Succ(),false);
			short ExtraMultiplier=Pop0Multiplier();
			if(ExtraMultiplier==1 || ExtraMultiplier==0)
				return Succ0;
			else
				return _CSimpleVertexContainer(Succ0,ExtraMultiplier);
		}
	default: ;
	}
	RISEPDD(eIllegalOperation,"_CVertexReminderOperator::Determine1DescendantRes - ouch!");
}

_CSimpleVertexContainer _CVertexReminderOperator::DoThe01Operation(const _CSimpleVertexContainer& Vertex)
{
	return Vertex;
}
//***********************************_CWeightResult
//_CWeightResult::_CWeightResult(const _CVertex* pVertex, const _CSimpleVertexContainer* pContainer, const _CSubCircuitSocket* pSocket, NumericType ldDesc1, NumericType ldDesc0)/*:*/
////m_pVertex(pVertex),m_pSVertex(pContainer),m_pSocket(pSocket)
//{
//	
//	ASSERTPDD(/*pSVertex!=NULL && */pVertex!=NULL);
//	_CComponent* pComponent=pVertex->GetComponent();
//	NumericType Value=pSocket==NULL?pComponent->GetParameterValue():
//		pSocket->GetParameterCurrentValue(pComponent);
//	ldDesc1*=Value;
//	short m=1;
//	if((m=pVertex->Get1SuccMultiplier())!=1)
//		ldDesc1*=m;
//	if((m=pVertex->Get0SuccMultiplier())!=1 && m!=0)
//		ldDesc0*=m;
//	if(ldDesc0==0.0 || abs(ldDesc1)<abs(ldDesc0))
//	{
//		m_Value=ldDesc1;
//		m_Direction=Desc1;
//	}
//	else
//	{
//		m_Value=ldDesc0;
//		m_Direction=Desc0;
//	}
//	if((m=pContainer->GetLocalMultiplier())!=1)
//		m_Value*=m;
//}
//
//_CWeightResult _CPathWeightOperator::DoTheOperation(const _CVertex& Vertex, 
//		const _CSimpleVertexContainer& SVertex, const _CWeightResult& Res1, 
//		const _CWeightResult& Res0)
//{
//	return _CWeightResult(&Vertex,&SVertex,GetCurrentSocket(),Res1.GetValue()/**GetRealCompValue(Vertex)*/,Res0.GetValue());
//}
//
//_CWeightResult _CPathWeightOperator::DoThe01Operation(const _CSimpleVertexContainer& Vertex)
//{
//	if(Vertex.Is0())
//		return _CWeightResult(0.0);
//	if(Vertex.Is1())
//		return _CWeightResult(1.0);
//	if(Vertex.IsM1())
//		return _CWeightResult(-1.0);
//	RISEPDD(eIllegalOperation,"Only for 1,0,-1 terminals");
//}


//_CLeastSgnfcntPathOperator::RES_TYPE _CLeastSgnfcntPathOperator::DoThe01Operation(const _CSimpleVertexContainer& Vertex)
//{
//	return &m_PathResult;
//}

//_CLeastSgnfcntPathOperator::RES_TYPE _CLeastSgnfcntPathOperator::Operate(const _CVertex& Vertex, 
//																		 const _CSimpleVertexContainer& SVertex,
//																		 _CAuxBasePlatorm* pAuxBasePlatform)
//{
//	_CWeightResult WR;
//	unsigned AuxId;
//	if(m_Weights.GetExisted(
//		_CRES_CONTAINER(&SVertex,GetOperationContext(),
//		                 m_CurrentPath.ClearComponent(),
//						 GetTerminalVert(),
//						 GetCurrentMVIds()),
//		WR,AuxId,
//		NULL,
//		NULL/*m_SVMStack.smart_back()*/,
//		//NULL,
//		//NULL,
//		//NULL,
//		//NULL,
//		NULL,
//		NULL
//		)!=EXACT_EQUAL)
//		RISEPDD(eUnexpectedCondition,"Use weights' determination first!!!!");
//	//_CLeastSgnfcntPathOperator::RES_TYPE Dummy;
//	if(WR.GetDirection()==_CWeightResult::Desc1)
//	{
//		/*Dummy=*/Vertex.Get1Succ().DoUnaryOpertationForWholeTree(*this,false);
//		m_CurrentPath.SetComponent(Vertex.GetComponent());
//		m_PathResult.push_front(m_CurrentPath);
//	}
//	else if(WR.GetDirection()==_CWeightResult::Desc0)
//		/*Dummy=*/Vertex.Get0Succ().DoUnaryOpertationForWholeTree(*this,true);
//	else
//		RISEPDD(eUnexpectedCondition,"Wrong direction");
//	return /*&m_PathResult*/0.0;
//}
//_CLeastSgnfcntPathOperator::RES_TYPE _CLeastSgnfcntPathOperator::Operate(const _CVertex& Vertex, 
//																		 const _CSimpleVertexContainer& SVertex)
//{
//	_CWeightResult WR;
//	if(m_Weights.GetExisted(
//		_CRES_CONTAINER(&SVertex,GetOperationContext(),
//						GetCurrentSPower(),
//						GetCurrentMVIds()),WR))
//		RISEPDD(eUnexpectedCondition,"Use weights' determination first!!!!");
//	//_CLeastSgnfcntPathOperator::RES_TYPE Dummy;
//	if(WR.GetDirection()==_CWeightResult::Desc1)
//	{
//		/*Dummy=*/Vertex.Get1Succ().DoUnaryOpertationForWholeTree(*this,false);
//		m_CurrentPath.SetComponent(Vertex.GetComponent());
//		m_PathResult.push_front(m_CurrentPath);
//	}
//	else if(WR.GetDirection()==_CWeightResult::Desc0)
//		/*Dummy=*/Vertex.Get0Succ().DoUnaryOpertationForWholeTree(*this,true);
//	else
//		RISEPDD(eUnexpectedCondition,"Wrong direction");
//	return /*&m_PathResult*/0.0;
//}
//*********************************************_CLSPath
void _CLSPath::ToVectorString(VECTOR_STRING& VString) 
{
	VString.clear();
	for(iterator it=begin();it!=end();it++)
	{
		VString.push_back((*it).GetPathDescr());
	}
}

void _CLSPath::ToOneString(string& StrValue) 
{
	bool Init=true;
	StrValue.clear();
	for(iterator it=begin();it!=end();it++)
	{
		if(Init)
			Init=false;
		else
			StrValue+=" << ";
		StrValue+=(*it).GetPathDescr();
	}
}

//**************************************_CPathRemoveOperator
short _CPathRemoveOperator::CompVertex(const _CVertex& Vertex,const _CSimpleVertexContainer& SVertex)
{
	if(SVertex.Is01M1())
		return 1;
	const _CComponent* pComp=Vertex.GetComponent();
	return m_CurrentPath.SetComponent(pComp).Comp(*m_RemPathIterator);
}

//_CSimpleVertexContainer _CPathRemoveOperator::Operate(const _CVertex& Vertex, 
//													  const _CSimpleVertexContainer& SVertex,
//													  _CAuxBasePlatorm* pAuxBasePlatform)
//{
//	if(m_Stage!=Normal)
//		return _CResVertexOperator::Operate(Vertex,SVertex,pAuxBasePlatform);
//	bool MaybeExisted=m_PrevValueAlreadyDefined==Maybe;
//	const _CSimpleVertexContainer* p0Vertex=ANY_VERTEX;
//	unsigned Succ0AuxId=0;
//	unsigned Succ1AuxId=0;
//	const _CSimpleVertexContainer* p1Vertex=ANY_VERTEX;
//	//string Vertex0Path,Vertex1Path;
//	MVIDS Vertex0MVId,Vertex1MVId;
//	//const _CComponent* pComp=Vertex.GetComponent();
//	//switch(m_CurrentPath.SetComponent(pComp).Comp(*m_RemPathIterator))
//	_CExactOutContainer* pVert0ExactOutContainer=NULL;
//	_CExactOutContainer* pVert1ExactOutContainer=NULL;
//	switch(CompVertex(Vertex,SVertex))
//	{
//	case -1:
//		{
//			const _CSimpleVertexContainer& Vert1=FixSubTree(Vertex.Get1Succ(),false);//?
//			p1Vertex=m_pPreviousRealSimpleVertex;
//			Succ1AuxId=m_CurrAuxId;
//			_CSimpleVertexContainer Vert0=TraceDescendant(true,Vertex,p0Vertex,Succ0AuxId,Vertex0MVId,pVert0ExactOutContainer);
//			//_CSimpleVertexContainer Vert0=Vertex.Get0Succ().DoUnaryOpertationForWholeTree(*this,true);
//			_CSimpleVertexContainer Res;
//			if(MaybeExisted)
//			{
//				//if(SimilarVertList.FitTheResult(m_CurrAuxId,Res,m_CurrentMVIds,p1Vertex,Vertex1MVId,Succ1AuxId,
//				//	p0Vertex,Vertex0MVId,Succ1AuxId))
//				//	return Res;
//			}
//			else
//				m_CurrAuxId=0;
//			Res=DoTheOperation(Vertex,SVertex,Vert1,Vert0);
//			m_pDescSimpleVertex1=p1Vertex;
//			m_Succ0AuxId=Succ0AuxId;
//			m_Succ1AuxId=Succ1AuxId;
//			m_pDescSimpleVertex0=p0Vertex;
//			m_Vert0MVId=Vertex0MVId;
//			m_Vert1MVId=Vertex1MVId;
//			m_pVert0ExactContainer=pVert0ExactOutContainer;
//			m_pVert1ExactContainer=pVert1ExactOutContainer;
//			return Res;
//		}
//	case 0:
//		{
//			m_RemPathIterator++;
//			const _CSimpleVertexContainer& Vert0=FixSubTree(Vertex.Get0Succ(),true);
//			p0Vertex=m_pPreviousRealSimpleVertex;
//			Succ0AuxId=m_CurrAuxId;
//			_CSimpleVertexContainer Vert1=TraceDescendant(false,Vertex,p1Vertex,Succ1AuxId,Vertex1MVId,pVert1ExactOutContainer);
//			//_CSimpleVertexContainer Vert1=Vertex.Get1Succ().DoUnaryOpertationForWholeTree(*this,false);
//			m_RemPathIterator--;
//			_CSimpleVertexContainer Res;
//			if(MaybeExisted)
//			{
//				//if(SimilarVertList.FitTheResult(m_CurrAuxId,Res,m_CurrentMVIds,p1Vertex,Vertex1MVId,Succ1AuxId,
//				//	p0Vertex,Vertex0MVId,Succ1AuxId))
//				//	return Res;
//			}
//			else
//				m_CurrAuxId=0;
//			Res=DoTheOperation(Vertex,SVertex,Vert1,Vert0);
//			m_pDescSimpleVertex1=p1Vertex;
//			m_Succ0AuxId=Succ0AuxId;
//			m_Succ1AuxId=Succ1AuxId;
//			m_pDescSimpleVertex0=p0Vertex;
//			m_Vert0MVId=Vertex0MVId;
//			m_Vert1MVId=Vertex1MVId;
//			m_pVert0ExactContainer=pVert0ExactOutContainer;
//			m_pVert1ExactContainer=pVert1ExactOutContainer;
//			return Res;
//		}
//	case 1:
//			m_pDescSimpleVertex0=m_pDescSimpleVertex1=ANY_VERTEX;
//			return FixSubTree(SVertex,false);
//	default:
//		;
//	}
//	return ZeroSVertex;
//}
_CSimpleVertexContainer _CPathRemoveOperator::Operate(const _CVertex& Vertex, 
													  const _CSimpleVertexContainer& SVertex)
{
	if(m_Stage!=Normal)
		return _CResVertexOperator::Operate(Vertex,SVertex);
	switch(CompVertex(Vertex,SVertex))
	{
	case -1:
		{
			const _CSimpleVertexContainer& Vert1=FixSubTree(Vertex.Get1Succ(),false);//?
			_CSimpleVertexContainer Vert0=TraceDescendant(true,Vertex);
			_CSimpleVertexContainer Res;
			return DoTheOperation(Vertex,SVertex,Vert1,Vert0);
		}
	case 0:
		{
			m_RemPathIterator++;
			const _CSimpleVertexContainer& Vert0=FixSubTree(Vertex.Get0Succ(),true);
			_CSimpleVertexContainer Vert1=TraceDescendant(false,Vertex);
			//_CSimpleVertexContainer Vert1=Vertex.Get1Succ().DoUnaryOpertationForWholeTree(*this,false);
			m_RemPathIterator--;
			return DoTheOperation(Vertex,SVertex,Vert1,Vert0);
		}
	case 1:
			return FixSubTree(SVertex,false);
	default:
		;
	}
	return ZeroSVertex;
}

//long long GetTypedHash(long long& Core, const _CWeightResult& WR)
//{
//	return WR.GetHash(Core);
//}

//bool operator==(const _CWeightResult& Left,const _CWeightResult& Right)
//{
//	return Left.operator ==(Right);
//}