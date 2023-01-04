#pragma once
//#include "StdAfx.h"
//#include "Components.h"
//#include "Circuit.h"
//#include "MainCircuit.h"
//#include "SubCircuit.h"
//#include <map>
//#include "SubCircuitSocket.h"
#include "MVID.h"
#include "PrjException.h"

class _CCircuit;
class _CSubCircuit;
class _CMainCircuit;
class _CComponent;
class _CSubCircuitSocket;

template<class CompType, class SocketType> 
class _CAbstractComponentPath
{
public:
	typedef list<CompType*> PATH;
protected:
	typedef typename PATH::iterator PATH_iterator;
	typedef PATH_iterator POSPointer; 
	typedef typename PATH::reverse_iterator PATHRPointer;
	typedef typename PATH::const_iterator PATH_const_iterator;
	PATH m_Path;
	//_CMainCircuit* m_pBaseCircuit;
	_CCircuit* m_pBaseCircuit;
	bool m_HasComponent;
	template<class ComponentPath>
		friend class _CAbstractComplementStore;
	//friend class _CConstComplementStore;
	friend class _CMainCircuit;
	friend class _CCircuit;
	friend class _CPathTraitor;
	//friend void Path2ConstPath(_CConstComponentPath& Desy, _CComponentPath& Source);
	string m_OryginalPath;
	const string* m_pShortName;
	_CAbstractComponentPath(_CComponent* pBasicComponent) :m_HasComponent(true), m_pShortName(NULL)
	{
		ASSERTPDD(!pBasicComponent->IsMacroDependant());
		m_pBaseCircuit = pBasicComponent->GetOwnerCircuit();
		m_Path.pop_front(pBasicComponent);
	}
	_CCircuit* /*Past circuit*/ PushFrontSubCirSocket(_CComponent* pSocket)
	{
		if (!pSocket->IsMacroDependant())
			RISEPDD(eWrongComponentType, "Subcircuit socket is expected");
		_CCircuit* pPrevBase = m_pBaseCircuit;
		m_pBaseCircuit = pSocket->GetOwnerCircuit();
		m_Path.push_front(pSocket);
		return pPrevBase;
	}
	void PopFrontSubCirSocket()
	{
		ASSERTPDD(m_Path.size() > 1);
		m_Path.pop_front();
		m_pBaseCircuit = dynamic_cast<_CComponent*>(m_Path.front())->GetOwnerCircuit();
	}
public:
	~_CAbstractComponentPath()
	{
	}
	typedef _CAbstractComponentPath<CompType,SocketType> MComponentPath;
	const PATH& GetPathList() const { return m_Path; }
	//_CAbstractComponentPath(_CMainCircuit* pBaseCircuit) :m_pBaseCircuit(pBaseCircuit),
	//	m_HasComponent(false), m_pShortName(NULL) {}
	_CAbstractComponentPath(_CCircuit* pBaseCircuit) :m_pBaseCircuit(pBaseCircuit),
		m_HasComponent(false), m_pShortName(NULL) {}
	//_CAbstractComponentPath(_CMainCircuit* pBaseCircuit, const string& StrPath)
	//{
	//	ParsePath(pBaseCircuit, StrPath);
	//}
	_CAbstractComponentPath(_CCircuit* pBaseCircuit, const string& StrPath)
	{
		ParsePath(pBaseCircuit, StrPath);
	}
	_CAbstractComponentPath(const MComponentPath& Source) { CopyAll(Source); }

	//SocketType* GetTopMostSocket() const
	//{
	//	if(m_Path.empty())
	//		return NULL;
	//	PATH::const_iterator it=(--(--m_Path.end()));
	//	if(typeid(**it)!=typeid(SocketType))
	//		return NULL;
	//	else
	//		return dynamic_cast<SocketType*>(*it);
	//}

	//SocketType* GetBottomMostSocket() const
	//{
	//	return m_Path.empty()?NULL:dynamic_cast<SocketType*>(m_Path.front());
	//}

	CompType* GetComponent() const {return m_HasComponent?m_Path.back():NULL;}


	inline MComponentPath& operator=(const MComponentPath& Path)
	{
		CopyAll(Path);
		return *this;
	}

	short Comp(const MVIDS& MVId) const
	{
		return Comp(MComponentPath(m_pBaseCircuit,MVId));
	}

	short Comp(const MComponentPath& Right) const
	{
		ASSERTPDD(m_pBaseCircuit==Right.m_pBaseCircuit);
		PATH_const_iterator lit=m_Path.begin(),_lend=m_Path.end(),
			rit=Right.m_Path.begin(), _rend=Right.m_Path.end();
		_CCircuit* pCurrCircuit=(_CCircuit*) m_pBaseCircuit;
		while(lit!=_lend && rit!=_rend)
		{
			CompType* pLeft=*lit;
			CompType* pRight=*rit;
			short Res=pCurrCircuit->CompLocalComponents(pLeft,pRight);
			if(Res!=0 || !pLeft->IsMacroDependant())
				return Res;
			ASSERTPDD(pRight->IsMacroDependant());
			SocketType* pLSocket=dynamic_cast<SocketType*>(pLeft);
			SocketType* pRSocket=dynamic_cast<SocketType*>(pRight);
			pCurrCircuit=pLSocket->GetModelCircuit();
			ASSERTPDD(pCurrCircuit==pRSocket->GetModelCircuit());
			lit++;
			rit++;
		}
		if(lit==_lend && rit==_rend)
			return 0;
		else if(rit==_rend)
			return -1;
		else
			return 1;
		//throw eIllegalOperation("_CComponentPath::Comp - Some component path is wrong");
	}

	bool operator==(const MComponentPath& Right) const {return Comp(Right)==0;}
	bool operator!=(const MComponentPath& Right) const {return Comp(Right)!=0;}
	bool operator>(const MComponentPath& Right) const {return Comp(Right)>0;}
	bool operator<(const MComponentPath& Right) const {return Comp(Right)<0;}
	bool operator>=(const MComponentPath& Right) const {return Comp(Right)>=0;}
	bool operator<=(const MComponentPath& Right) const {return Comp(Right)==0;}
	_CAbstractComponentPath& SubmergeSocket(SocketType* pNextLevel)
	{
		if(m_HasComponent)
		{
			m_Path.pop_back();
			m_HasComponent=false;
			m_pShortName=NULL;
		}
		m_Path.push_back(pNextLevel);
		m_OryginalPath.clear();
		return *this;
	}
	_CAbstractComponentPath& EmergeSocket()
	{
		ASSERTPDD(!m_Path.empty());
		if(m_HasComponent)
		{
			m_Path.pop_back();
			m_HasComponent=false;
			m_pShortName=NULL;
		}
		m_Path.pop_back();
		m_OryginalPath.clear();
		return *this;
	}
	SocketType* GetCurrentSocket() const
	{
		PATH::const_reverse_iterator it=m_Path.rbegin();
		if(m_HasComponent)
			it++;
		return it==m_Path.rend()?NULL:dynamic_cast<SocketType*> (*it);
		//if(m_HasComponent)
		//	it=(--(--(m_Path.end())));
		//else
		//	it=(--m_Path.end());
		//return it==m_Path.begin()?NULL:reinterpret_cast<SocketType*> (*it);
	}
	inline SocketType* GetTopMostSocket() const 
	{
		return GetCurrentSocket();
	}

	bool PrepareLocalOperation(_CCircuit*& pOperCircuit,_CComponent*& pOperComponent) 
	{
		if(!m_HasComponent)
			return false;
		PATH::reverse_iterator it=(m_Path.rbegin());
		ASSERTPDD(it!=m_Path.rend());
		pOperComponent=(*it);
		it++;
		pOperCircuit=(it==m_Path.rend())?
			reinterpret_cast<_CCircuit*> (m_pBaseCircuit):
		    reinterpret_cast<_CCircuit*>(dynamic_cast<SocketType*> (*it)->GetModelCircuit(EmptyString));
		return true;
	}
	bool PreparFirstLocalData(POSPointer& LastLocalIt,_CCircuit*& pOperCircuit,_CComponent*& pOperComponent)
	{
		ASSERTPDD(m_HasComponent);
		LastLocalIt=m_Path.begin();
		if(LastLocalIt==m_Path.end())
			return false;
		pOperCircuit=m_pBaseCircuit;
		pOperComponent=reinterpret_cast<_CComponent*>(*LastLocalIt);
		return true;
	}
	bool PreparNextLocalData(POSPointer& LastLocalIt,_CCircuit*& pOperCircuit,_CComponent*& pOperComponent)
	{
		//Piszemy tu
		ASSERTPDD(m_HasComponent);
		if(LastLocalIt==m_Path.end())
			return false;
		CompType* pct=*LastLocalIt;
		if(!pct->IsMacroDependant())
			return false;
		pOperCircuit=dynamic_cast<SocketType*>(pct)->GetModelCircuit();
		LastLocalIt++;
		pOperComponent=reinterpret_cast<_CComponent*> (*LastLocalIt);
		return true;
	}
	bool PreparLastLocalData(PATHRPointer& LastLocalIt,_CCircuit*& pOperCircuit,_CComponent*& pOperComponent
		)
	{
		LastLocalIt=m_Path.rbegin();
		return PreparPrevLocalData(LastLocalIt,pOperCircuit,pOperComponent);
	}
	bool PreparPrevLocalData(PATHRPointer& LastLocalIt,_CCircuit*& pOperCircuit,_CComponent*& pOperComponent)
	{
		ASSERTPDD(m_HasComponent);
		if(LastLocalIt==m_Path.rend())
			return false;
		pOperComponent=*LastLocalIt;
		LastLocalIt++;
		if(LastLocalIt==m_Path.rend())
			pOperCircuit=m_pBaseCircuit;
		else
			pOperCircuit=dynamic_cast<SocketType*>(*LastLocalIt)->GetModelCircuit();
		return true;
	}
	_CAbstractComponentPath& SetComponent(CompType* pComponent)
	{
		if(m_HasComponent)
			*(--m_Path.end())=pComponent;
		else
		{
			m_Path.push_back(pComponent);
			m_HasComponent=true;
		}
		m_pShortName=&(pComponent->GetParameterName());
		return *this;
	}
	_CCircuit* GetBaseCircuit() const {return m_pBaseCircuit;}
	//const string& GetPathDescr() 
	string GetPathDescr() 
	{
		if(m_OryginalPath.empty())
			BuildPathDescription();
		if(m_pShortName!=NULL)
			return m_OryginalPath+"."+*m_pShortName;
		else
			return m_OryginalPath;
	}
	const string& GetPath()
	{
		if(m_OryginalPath.empty())
			BuildPathDescription();
		return m_OryginalPath;
	}
	_CAbstractComponentPath& ClearComponent()
	{
		if(m_HasComponent)
		{
			m_Path.pop_back();
			m_HasComponent=false;
			m_pShortName=NULL;
		}
		return *this;
	}
	size_t GetHash()
	{
		//return hash_value(GetPathDescr())*HASH_FACTOR^(size_t)m_HasComponent;
		return hash<string>()(GetPathDescr())*HASH_FACTOR^(size_t)m_HasComponent;
	}
	long long GetHash(long long& Core)
	{
		return Hash_value(Core,GetPathDescr())*HASH_FACTOR^(long long)m_HasComponent;
	}
	//bool operator==(const _CAbstractComponentPath& Right)
	//{
	//	if(m_pBaseCircuit!=Right.m_pBaseCircuit || m_HasComponent!=Right.m_HasComponent)
	//		return false;
	//	return m_Path==Right.m_Path;
	//}
	//_CMainCircuit* GetMainCircuit() {return m_pBaseCircuit;}
protected:
	_CAbstractComponentPath(_CCircuit* pBaseCircuit, const MVIDS& MVId) :
		//_CAbstractComponentPath(_CMainCircuit* pBaseCircuit, const MVIDS& MVId) :
		m_pBaseCircuit(pBaseCircuit), m_HasComponent(false), m_pShortName(NULL)
	{
		_CCircuit *pcBaseCircuit=m_pBaseCircuit;
		for(MVIDS::const_iterator it=MVId.begin();it!=MVId.end();it++)
		{
			const _CComponent* pComponent=pcBaseCircuit->Index2ModelVertex(*it)->GetPComponent();
			m_Path.push_back(pComponent);
			const _CSubCircuitSocket* pSocket=dynamic_cast<const _CSubCircuitSocket*> (pComponent);
			ASSERTPDD(pSocket!=NULL);
			pcBaseCircuit=pSocket->GetModelCircuit();
		}
	}
	void BuildPathDescription()
	{
		m_OryginalPath.clear();
		bool First=true;
		for(PATH::iterator it=m_Path.begin();it!=m_Path.end() && (*it)->IsMacroDependant();it++)
		{
			if(First)
				First=false;
			else
				m_OryginalPath+=".";
			m_OryginalPath+=(*it)->GetParameterName();
		}
	}
	//void ParsePath(_CMainCircuit* pBaseCircuit, const string& StrPath)
	void ParsePath(_CCircuit* pBaseCircuit, const string& StrPath)
	{
		_string_list strlist(StrPath,".");
		/*_CCircuit* pCurrCircuit=*/m_pBaseCircuit=pBaseCircuit;
		_CCircuit* pCurrCircuit=reinterpret_cast<_CCircuit*> (pBaseCircuit);
		_string_list::iterator comp_iter=strlist.end();
		--comp_iter;
		for(_string_list::iterator it=strlist.begin();it!=comp_iter;it++)
		{
			_CComponent* pComponent=pCurrCircuit->FindComponent(*it);
			if(pComponent==NULL || typeid(*pComponent)!=typeid(_CSubCircuitSocket))
				RISEPDD(eWrongName,(string("The name: ")+*it+" is not a subcircuit instance name").c_str());
			_CSubCircuitSocket* pSocket=dynamic_cast<_CSubCircuitSocket*>(pComponent);
			m_Path.push_back(pComponent);
			pCurrCircuit=pSocket->GetModelCircuit();
		}
		_CComponent* pLocalComponent=pCurrCircuit->FindComponent(*comp_iter);
		if(pLocalComponent==NULL)
			RISEPDD(eWrongName,(*comp_iter+" is not a component").c_str());
		if(typeid(*pLocalComponent)==typeid(_CSubCircuitSocket))
			RISEPDD(eWrongName,(*comp_iter+ " should be the name of a component differant to subcircuit instance").c_str());
		m_Path.push_back(pLocalComponent);
		m_pShortName=&(pLocalComponent->GetParameterName());
		m_HasComponent=true;
		BuildPathDescription();
		//m_OryginalPath=StrPath;
	}
	void CopyAll(const MComponentPath& Path)
	{
		m_pBaseCircuit=Path.m_pBaseCircuit;
		m_HasComponent=Path.m_HasComponent;
		m_Path=Path.m_Path;
		m_OryginalPath=Path.m_OryginalPath;
		m_pShortName=Path.m_pShortName;
	}
};

typedef _CAbstractComponentPath<_CComponent,_CSubCircuitSocket> _CComponentPath;
typedef _CAbstractComponentPath<const _CComponent,const _CSubCircuitSocket> _CConstComponentPath;


//class _CComponentPath : public _CAbstractComponentPath<_CComponent,_CSubCircuitSocket>
//{
//public:
//	_CComponentPath(_CMainCircuit* pBaseCircuit):
//		_CAbstractComponentPath<_CComponent,_CSubCircuitSocket>(pBaseCircuit) {}
//	_CComponentPath(_CMainCircuit* pBaseCircuit, const string& StrPath):
//		_CAbstractComponentPath<_CComponent,_CSubCircuitSocket>(pBaseCircuit,StrPath) {}
//	_CComponentPath(const MComponentPath& Source):
//		_CAbstractComponentPath<_CComponent,_CSubCircuitSocket>(Source) {}
//};
//
//class _CConstComponentPath : public _CAbstractComponentPath<const _CComponent,const _CSubCircuitSocket>
//{
//public:
//	_CConstComponentPath(_CMainCircuit* pBaseCircuit):
//		_CAbstractComponentPath<const _CComponent,const _CSubCircuitSocket>(pBaseCircuit) {}
//	_CConstComponentPath(_CMainCircuit* pBaseCircuit, const string& StrPath):
//		_CAbstractComponentPath<const _CComponent,const _CSubCircuitSocket>(pBaseCircuit,StrPath) {}
//	_CConstComponentPath(const MComponentPath& Source):
//		_CAbstractComponentPath<const _CComponent,const _CSubCircuitSocket>(Source) {}
//};