#include "stdafx.h"
#include "PathTraitor.h"
#include "PrjException.h"
#include "..\PDDErrorsAndConst.h"
#include "MainCircuit.h"
#include "SubCircuitSocket.h"

string _CApproxCriterion::GetContext() const
{
	string Res="App(";
	Res+=ToString(m_Accurracy);
	Res+='\\';
	Res+=ToString(m_HighFreguency);
	Res+='\\';
	Res+=ToString(m_LowFrequency);
	Res+='\\';
	Res+=short2str(m_MaxPower);
	Res+='\\';
	Res+=short2str(m_MinPower);
	Res+=')';
	return Res;
}

string _CApproxCriterion::GetDescString() const
{
	string Res="Approximated 1-level tree for the accurracy=";
	Res+=ToString(m_Accurracy);
	Res+=" , range angular frequency from ";
	Res+=ToString(m_LowFrequency);
	Res+=" to ";
	Res+=ToString(m_HighFreguency);
	Res+='.';
	if(m_MinPower!=0 || m_MaxPower!=numeric_limits<short>::max())
	{
		Res+=" s-Power artificially limited to: ";
		Res+=ulong2str(m_MinPower);
		Res+=" ÷ ";
		Res+=ulong2str(m_MaxPower);
		Res+='.';
	}
	Res+="\n";
	return Res;
}

void _CApproxCriterion::WriteToTextFile(txt_stream& stream) const
{
	stream<<"Approximated 1-level tree for the accurracy=";
	stream<<m_Accurracy;
	stream<<" , range angular frequency from ";
	stream<<m_LowFrequency;
	stream<<" to ";
	stream<<m_HighFreguency;
	stream<<'.';
	if(m_MinPower!=0 || m_MaxPower!=numeric_limits<short>::max())
	{
		stream<<" s-Power artificially limited to: ";
		stream<<m_MinPower;
		stream<<" ÷ ";
		stream<<m_MaxPower;
		stream<<'.';
	}
	stream<<endl;
}
txt_stream& operator<<(txt_stream& stream, const _CApproxCriterion& Appr)
{
	Appr.WriteToTextFile(stream);
	return stream;
}

_CPathTraitor::_CPathTraitor(_CMainCircuit* pBaseCircuit,const string& BaseContext):m_pBaseCircuit(pBaseCircuit),
	m_Tree(m_pBaseCircuit),m_BaseContext(BaseContext), m_pApproximationCriterion(NULL)
{
	ReStartFollower();
}
_CPathTraitor::_CPathTraitor(const _CPathTraitor& Source):m_pBaseCircuit(Source.m_pBaseCircuit),
	m_Tree(Source.m_Tree), m_BaseContext(Source.m_BaseContext)
{
	if(Source.m_pApproximationCriterion==NULL)
		m_pApproximationCriterion=NULL;
	else
		m_pApproximationCriterion=new _CApproxCriterion(*Source.m_pApproximationCriterion);
	ReStartFollower();
}
_CPathTraitor::~_CPathTraitor()
{
	if(m_pApproximationCriterion!=NULL)
		delete m_pApproximationCriterion;
}

const _CComponent* _CPathTraitor::AddDivider(const string& ElementPath, _CCompRedefEntry& Entry, const _CSubModelNumericPattern** ppCompPatt)
{
	const _CComponent* pRes = AddOperation(Divider, ElementPath, Entry, ppCompPatt);
	ReStartFollower();
	return pRes;
}
const _CComponent* _CPathTraitor::AddReminder(const string& ElementPath, _CCompRedefEntry& Entry, const _CSubModelNumericPattern** ppCompPatt)
{
	const _CComponent* pRes = AddOperation(Reminder, ElementPath, Entry, ppCompPatt);
	ReStartFollower();
	return pRes;
}

void _CPathTraitor::ClearApproximation()
{
	if(m_pApproximationCriterion!=NULL)
	{
		delete m_pApproximationCriterion;
		m_pApproximationCriterion=NULL;
	}
}
void _CPathTraitor::SetApproximationCriterion(bool Exact,
											  const NumericType& Accurracy,
											  const NumericType& HighFrequency, 
											  const NumericType& LowFrequency,
											  short MaxPower,
											  short MinPower)
{
	if(ShouldBeApproximated())
		ClearApproximation();
	m_pApproximationCriterion= new _CApproxCriterion(Exact,Accurracy,HighFrequency,LowFrequency,MaxPower,MinPower);
}

const _CComponent* _CPathTraitor::AddOperation(OP_TYPE Type, const string& strPath, _CCompRedefEntry& Redef, const _CSubModelNumericPattern** ppCompPatt)
{
		_string_list strlist(strPath,".");
		_CComponent* pComponent=m_Tree.AddOperPath(Type,strlist.begin(),strlist.end(),Redef,ppCompPatt);
		//if(ppCompPatt!=NULL && *ppCompPatt==NULL)
		//{
		//	_CSimplyComponent* pSComponent=dynamic_cast<_CSimplyComponent*> (pComponent);
		//	ASSERTPDD(pSComponent!=NULL);
		//	*ppCompPatt=pSComponent->GetMyBaseNumPattern(Redef);
		//}
		return pComponent;
}

bool _CPathTraitor::_AbstractPredicator::operator() (const _CTreeElement& Left,const _CTreeElement& Right) const
{
	return m_ParentTreeElement->GetParentCircuit()->CompLocalComponents(Left.GetRootComponent(),Right.GetRootComponent())<0;
}

//bool _CPathTraitor::_CTreeElement::_Traitor::operator() (const _CTreeElement& Left,const _CTreeElement& Right) const
//{
//	ASSERTPDD(m_pRootComponent->IsMacroDependant());
//	return dynamic_cast<_CSubCircuitSocket*>(m_pRootComponent)->
//		GetModelCircuit()->CompLocalComponents(Left.m_pRootComponent,Right.m_pRootComponent)<0;
//}
//
//bool _CPathTraitor::_CTreeRoot::_Traitor::operator()  (const _CTreeElement& Left,const _CTreeElement& Right) const
//{
//	return m_pMainCircuit->CompLocalComponents(Left.GetRootComponent(),Right.GetRootComponent())<0;
//}
const char* WChain="The component path should has a form <macro>.<macro>. ... .<macro>.<basic component name>";
//void _CPathTraitor::_CTreeRoot::AddOperPath(OP_TYPE Type, _string_list::iterator B, const _string_list::iterator& E)
//{
//	if(B==E)
//		return;
//	string& name=*B;
//	_CComponent* pComponent=m_pMainCircuit->FindComponent(name);
//	if(pComponent==NULL)
//		RISEPDD(eWrongName,(string("The name: ")+name+" is not a proper name of component in the main circuit").c_str());
//	_CTreeElement NewElement(pComponent), *pRealNewElement=NULL;
//	SET::iterator it=m_Descendants.find(NewElement);
//	if(it!=m_Descendants.end())
//		pRealNewElement=&(*it);
//	else
//		pRealNewElement=&(*(m_Descendants.insert(NewElement).first));
//	if(Type==Divider)
//		pRealNewElement->SetDivider();
//	else if(Type==Reminder)
//		pRealNewElement->SetReminder();
//	else
//		RISEPDD(eIllegalOperation,"Parameter Type can be only: Divider or Reminder");
//	B++;
//	bool WrongForm=pComponent->IsMacroDependant()?(B==E?true:false):(B==E?false:true);
//	if(WrongForm)
//		RISEPDD(eWrongName,WChain);
//	pRealNewElement->AddOperPath(Type,B,E);
//}
//
//void _CPathTraitor::_CTreeElement::AddOperPath(OP_TYPE Type, _string_list::iterator B, const _string_list::iterator& E)
//{
//	if(B==E)
//		return;
//	string& name=*B;
//	_CComponent* pComponent=dynamic_cast<_CSubCircuitSocket*>(m_pRootComponent)->
//		GetModelCircuit()->FindComponent(name);
//	if(pComponent==NULL)
//		RISEPDD(eWrongName,(string("The name: ")+name+" is not a proper name of component in the main circuit").c_str());
//	_CTreeElement NewElement(pComponent), *pRealNewElement=NULL;
//	SET::iterator it=m_Descendants.find(NewElement);
//	if(it!=m_Descendants.end())
//		pRealNewElement=&(*it);
//	else
//		pRealNewElement=&(*(m_Descendants.insert(NewElement).first));
//	if(Type==Divider)
//		pRealNewElement->SetDivider();
//	else if(Type==Reminder)
//		pRealNewElement->SetReminder();
//	else
//		RISEPDD(eIllegalOperation,"Parameter Type can be only: Divider or Reminder");
//	B++;
//	bool WrongForm=pComponent->IsMacroDependant()?(B==E?true:false):(B==E?false:true);
//	if(WrongForm)
//		RISEPDD(eWrongName,WChain);
//	pRealNewElement->AddOperPath(Type,B,E);
//}

_CComponent* _CPathTraitor::_AbstractTreeElement::AddOperPath(OP_TYPE Type, _string_list::iterator B, const _string_list::iterator& E, _CCompRedefEntry& Redef, const _CSubModelNumericPattern** ppCompPatt)
{
	if(B==E)
	{
		if(ppCompPatt!=NULL)
			*ppCompPatt=NULL;
		return NULL;
	}
	string& name=*B;
	_CComponent* pComponent=GetParentCircuit()->FindComponent(name);
	if(pComponent==NULL)
		RISEPDD(eWrongName,(string("The name: ")+name+" is not a proper name of component in the main circuit").c_str());
	_CTreeElement NewElement(pComponent), *pRealNewElement=NULL;
	SET::iterator it=m_Descendants.find(NewElement);
	if(it!=m_Descendants.end())
		pRealNewElement = (_CTreeElement*)&(*it);
	else
		pRealNewElement = (_CTreeElement*)&(*(m_Descendants.insert(NewElement).first));
	if(Type==Divider)
		pRealNewElement->SetDivider();
	else if(Type==Reminder)
		pRealNewElement->SetReminder();
	else
		RISEPDD(eIllegalOperation,"Parameter Type can be only: Divider or Reminder");
	B++;
	bool WrongForm=pComponent->IsMacroDependant()?(B==E?true:false):(B==E?false:true);
	if(WrongForm)
		RISEPDD(eWrongName,WChain);
	_CComponent* pOutComponent = pRealNewElement->AddOperPath(Type, B, E, Redef[pComponent], ppCompPatt);
	if(pOutComponent==NULL)
	{
		pOutComponent = pComponent;
		if (ppCompPatt != NULL && *ppCompPatt == NULL)
		{
			_CSimplyComponent* pSComponent = dynamic_cast<_CSimplyComponent*>(pOutComponent);
			ASSERTPDD(pSComponent != NULL);
			//_CSubCircuitSocket* pSocket=dynamic_cast<_CSubCircuitSocket*>(pComponent);
			//ASSERTPDD(pSocket!=NULL);
			*ppCompPatt = pSComponent->GetMyBaseNumPattern(Redef[pSComponent]);
		}
	}
	return pOutComponent;
}

void _CPathTraitor::_CTreeElement::GetOperStrSymbol(string& Oper)
{
	if(!m_Descendants.empty())
		switch(m_Operation)
		{
		case DIVIDER: Oper="[/D]";
		case REMINDER: Oper="[/R]";
		default:
			ASSERTPDD(true);
		}
}

string _CPathTraitor::_CTreeElement::GetStrPath() const
{
	string Res;
	if(!m_Descendants.empty())
	{
		//(--m_Descendants.end())->GetOperStrSymbol(Res);
		Res=".";
		for(SET::iterator it=m_Descendants.begin(),_e=m_Descendants.end();it!=_e;it++)
			Res+=it->GetStrPath();
	}
	else
		switch(m_Operation)
		{
		case DIVIDER: Res="[/D]";break;
		case REMINDER: Res="[/R]";break;
		default:
			ASSERTPDD(false);
		}
	return string("(")+m_pRootComponent->GetParameterName()+Res+")";
}

_CCircuit* _CPathTraitor::_CTreeElement::GetParentCircuit()
{
	return dynamic_cast<_CSubCircuitSocket*>(m_pRootComponent)->GetModelCircuit();
}

_CCircuit* _CPathTraitor::_CTreeRoot::GetParentCircuit()
{
	return m_pMainCircuit;
}

//_CPathTraitor::FOLLOWER _CPathTraitor::GetStartFollower() 
//{
//	return FOLLOWER(&m_Tree,m_Tree.m_Descendants.begin());
//}
//
//const _CComponent* _CPathTraitor::Follower2Component(const FOLLOWER& Follower)
//{
//	if(Follower.second==Follower.first->m_Descendants.end())
//		return NULL;
//	_CTreeElement& Ele=*(Follower.second);
//	return Ele.m_pRootComponent;
//}
//short _CPathTraitor::Follower2Oper(const FOLLOWER& Follower)
//{
//	if(Follower.second==Follower.first->m_Descendants.end())
//		return -1;
//	_CTreeElement& Ele=*(Follower.second);
//	return Ele.m_Operation;
//}
//
//_CPathTraitor::OPER_DATA _CPathTraitor::Follower2Data(const FOLLOWER& Follower)
//{
//	if(Follower.second==Follower.first->m_Descendants.end())
//		return OPER_DATA(NULL,-1);
//	_CTreeElement& Ele=*(Follower.second);
//	return OPER_DATA(Ele.m_pRootComponent,Ele.m_Operation);
//}
//
//_CPathTraitor::COMP_RESULT _CPathTraitor::Compare(const FOLLOWER& Follower, const _CComponent* pComponent)
//{
//	try
//	{
//		if(Follower.second==Follower.first->m_Descendants.end())
//			return COMP_RESULT(-1,0);//?
//		_CTreeElement& Ele=*(Follower.second);
//		_CCircuit* pCircuit=Follower.first->GetParentCircuit();
//		short Res=pCircuit->CompLocalComponents(Ele.m_pRootComponent,pComponent);
//		return COMP_RESULT(Res,Ele.m_Operation);
//	}
//	catch(eIllegalOperation)
//	{
//		return COMP_RESULT(0,-1);
//	}
//}
//
//bool _CPathTraitor::GetNextFollower(FOLLOWER& Follower)
//{
//	_AbstractTreeElement::SET::iterator &it=Follower.second,
//		&e=Follower.first->m_Descendants.end();
//	if(it==e)
//		return false;
//	if((++it)==e)
//		return false;
//	return true;
//}
//
//bool _CPathTraitor::GetSubFollower(FOLLOWER& Follower)
//{
//	_AbstractTreeElement* &pTreeElement=Follower.first;
//	_AbstractTreeElement::SET &Descendants=pTreeElement->m_Descendants;
//	_AbstractTreeElement::SET::iterator &it=Follower.second;
//	if(it==Descendants.end())
//		return false;
//	pTreeElement=&(*it);
//	it=pTreeElement->m_Descendants.begin();
//	return true;
//}
//
//const string& _CPathTraitor::GetStrContext(const FOLLOWER& Follower)
//{
//	string Res;
//	_AbstractTreeElement::SET::iterator it=Follower.second,
//		e=Follower.first->m_Descendants.end();
//	for(;it!=e;it++)
//	{
//		_CTreeElement& Element=*it;
//		switch(Element.m_Operation)
//		{
//		case _AbstractTreeElement::DIVDER: Res+="D/";break;
//		case _AbstractTreeElement::REMINDER: Res+="R/";break;
//		case _AbstractTreeElement::BOTH: Res+="B/";break;
//		default: 
//			RISEPDD(eIllegalOperation,"Wrong Id of operation");
//		}
//		Res+=Element.m_pRootComponent->GetParameterName();
//		Res+=';';
//	}
//	return m_pBaseCircuit->GetUniqueContextStringName(*m_BaseContext+Res);
//}

void _CPathTraitor::ReStartFollower()
{
	m_List.clear();
	m_List.push_back(FOLLOWER(&m_Tree,m_Tree.m_Descendants.begin()));
}

void _CPathTraitor::Clear()
{
	m_Tree.Clear();
	ReStartFollower();
}

const _CComponent* _CPathTraitor::CurrentComponent()
{
	ASSERTPDD(!m_List.empty());
	FOLLOWER &Follower=m_List.back();
	if(Follower.second==Follower.first->m_Descendants.end())
		return NULL;
	const _CTreeElement& Ele=*(Follower.second);
	return Ele.m_pRootComponent;
}

short _CPathTraitor::CurrentOper()
{
	ASSERTPDD(!m_List.empty());
	FOLLOWER &Follower=m_List.back();
	if(Follower.second==Follower.first->m_Descendants.end())
		return -1;
	const _CTreeElement& Ele=*(Follower.second);
	return Ele.m_Operation;
}

_CPathTraitor::OPER_DATA _CPathTraitor::CurrentData()
{
	ASSERTPDD(!m_List.empty());
	FOLLOWER &Follower=m_List.back();
	if(Follower.second==Follower.first->m_Descendants.end())
		return OPER_DATA(NULL,-1);
	const _CTreeElement& Ele=*(Follower.second);
	return OPER_DATA(Ele.m_pRootComponent,Ele.m_Operation);
}

_CPathTraitor::COMP_RESULT _CPathTraitor::Compare(const _CComponent* pComponent)
{
	ASSERTPDD(!m_List.empty());
	FOLLOWER &Follower=m_List.back();
	try
	{
		if(Follower.second==Follower.first->m_Descendants.end())
			return COMP_RESULT(-1,0);//?
		const _CTreeElement& Ele=*(Follower.second);
		_CCircuit* pCircuit=Follower.first->GetParentCircuit();
		short Res=pCircuit->CompLocalComponents(pComponent,Ele.m_pRootComponent);
		return COMP_RESULT(Res,Ele.m_Operation);
	}
	catch(eIllegalOperation)
	{
		return COMP_RESULT(0,-1);
	}
}

bool _CPathTraitor::NextFollower()
{
	ASSERTPDD(!m_List.empty());
	FOLLOWER Follower=m_List.back();
	_AbstractTreeElement::SET::iterator &it=Follower.second,
		&e=Follower.first->m_Descendants.end();
	if(it==e)
		return false;
	//if((++it)==e)
	//	return false;
	it++;
	m_List.push_back(Follower);
	return true;
}

bool _CPathTraitor::SubFollower()
{
	ASSERTPDD(!m_List.empty());
	FOLLOWER Follower=m_List.back();
	_AbstractTreeElement* &pTreeElement=Follower.first;
	_AbstractTreeElement::SET &Descendants=pTreeElement->m_Descendants;
	_AbstractTreeElement::SET::iterator &it=Follower.second;
	if(it==Descendants.end())
		return false;
	pTreeElement = (_AbstractTreeElement*)&(*it);
	it=pTreeElement->m_Descendants.begin();
	m_List.push_back(Follower);
	return true;
}

void _CPathTraitor::SideFollower()
{
	ASSERTPDD(!m_List.empty());
	FOLLOWER Follower=m_List.back();
	_AbstractTreeElement* &pTreeElement=Follower.first;
	_AbstractTreeElement::SET &Descendants=pTreeElement->m_Descendants;
	_AbstractTreeElement::SET::iterator &it=Follower.second;
	it=pTreeElement->m_Descendants.end();
	m_List.push_back(Follower);
}

const string& _CPathTraitor::StrContext2()
{
	ASSERTPDD(!m_List.empty());
	FOLLOWER &Follower=m_List.back();
	_AbstractTreeElement::SET::iterator it=Follower.second,
		e=Follower.first->m_Descendants.end();
	string Res;
	//if(ShouldBeApproximated())
	//	Res+=m_pApproximationCriterion->GetContext();
	for(;it!=e;it++)
		Res+=it->GetStrPath();
	return m_pBaseCircuit->GetUniqueContextStringName(m_BaseContext+Res);
}
const string& _CPathTraitor::StrApproxContext()
{
	ASSERTPDD(!m_List.empty());
	FOLLOWER &Follower=m_List.back();
	_AbstractTreeElement::SET::iterator it=Follower.second,
		e=Follower.first->m_Descendants.end();
	string Res;
	if(ShouldBeApproximated())
		Res+=m_pApproximationCriterion->GetContext();
	for(;it!=e;it++)
		Res+=it->GetStrPath();
	return m_pBaseCircuit->GetUniqueContextStringName(m_BaseContext+Res);
}

//const string& _CPathTraitor::StrContext()
//{
//	ASSERTPDD(!m_List.empty());
//	FOLLOWER &Follower=m_List.back();
//	string Res;
//	if(ShouldBeApproximated())
//		Res+=m_pApproximationCriterion->GetContext();
//	_AbstractTreeElement::SET::iterator it=Follower.second,
//		e=Follower.first->m_Descendants.end();
//	for(;it!=e;it++)
//	{
//		_CTreeElement& Element=*it;
//		switch(Element.m_Operation)
//		{
//		case /*_AbstractTreeElement::*/DIVIDER: Res+="D/";break;
//		case /*_AbstractTreeElement::*/REMINDER: Res+="R/";break;
//		case /*_AbstractTreeElement::*/BOTH: Res+="B/";break;
//		default: 
//			RISEPDD(eIllegalOperation,"Wrong Id of operation");
//		}
//		Res+=Element.m_pRootComponent->GetParameterName();
//		Res+=';';
//	}
//	return m_pBaseCircuit->GetUniqueContextStringName(m_BaseContext+Res);
//}

bool _CPathTraitor::PopLastLevel()
{
	FOLLOWER_LIST::iterator it=++m_List.begin();
	if(it==m_List.end())
		return false;
	m_List.pop_back();
	return true;
}
