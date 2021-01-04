#include "StdAfx.h"
#include "Factorizer.h"
#include "Cofactor.h"



_CFlatOneCont FlatOneCont;


ULONGLONG _CFlatOneCont::GetHash(ULONGLONG Core) 
{
	ULONGLONG Res=Core;
	Res*=HASH_FACTOR;
	//Res^=m_SgnMult;
	//Res*=HASH_FACTOR;
	Res^=((ULONGLONG) (&FlatOneCont));
	return Res;
}

ULONGLONG CP2Hash(const COMPONENTS_PATH* Value, ULONGLONG Core,size_t Power)
{
	ULONGLONG Res=Core;
	Core *= HASH_CORE;
	Core *= Power;
	Res *= HASH_FACTOR;
	Res^=((ULONGLONG) Value);
	return Res; 

}

ULONGLONG _CFlatSingleValueCont::GetHash(ULONGLONG Core) 
{
	//Core *= HASH_CORE;
	//Core *= m_Power;
	return CP2Hash(m_Value,Core,m_Power); 
}

ULONGLONG _CFlatProductCont::GetHash(ULONGLONG Core)
{
	ULONGLONG Res=Core;
	//Res*=HASH_FACTOR;
	//Res^=m_SgnMult;
	//Res*=HASH_FACTOR;
	//Res^=(ULONGLONG)'P';
	for(PTR_LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		Res*=HASH_FACTOR;
		Res^=(ULONGLONG)(*it);
	}
	return Res; 
}

ULONGLONG _CFlatSumCont::GetHash(ULONGLONG Core)
{
	ULONGLONG Res=Core;
	//Res*=HASH_FACTOR;
	//Res^=m_SgnMult;
	//Res*=HASH_FACTOR;
	//Res^=(ULONGLONG)'S';
	for(PTR_LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		SIGNED_TERM& Term=*it;
		Res*=HASH_FACTOR;
		Res^=Term.first;
		Res*=HASH_FACTOR;
		Res^=(ULONGLONG)Term.second;
	}
	return Res; 
}

bool _CFlatOneCont::IsEqual(const _CAbstractFlatValueCont* pLeft)
{
	return pLeft==&FlatOneCont;
}


_CFlatValueUniqueList::~_CFlatValueUniqueList()
{
	for(MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
		delete it->second;
}


bool _CFlatSingleValueCont::IsEqual(const _CAbstractFlatValueCont* pLeft) 
{
	const _CFlatSingleValueCont* ppLeft=dynamic_cast<const _CFlatSingleValueCont*>(pLeft);
	if (ppLeft)
		return m_Power == ppLeft->m_Power && m_Value == ppLeft->m_Value;
	return false;
}

bool _CFlatProductCont::IsEqual(const _CAbstractFlatValueCont* pLeft) 
{
	const _CFlatProductCont* ppLeft=dynamic_cast<const _CFlatProductCont*>(pLeft);
	if(ppLeft)
		return /*m_SgnMult==ppLeft->m_SgnMult &&*/ m_List==ppLeft->m_List;
	return false;
}

bool _CFlatSumCont::IsEqual(const _CAbstractFlatValueCont* pLeft) 
{
	const _CFlatSumCont* ppLeft=dynamic_cast<const _CFlatSumCont*>(pLeft);
	if(ppLeft)
		return /*m_SgnMult==ppLeft->m_SgnMult && */m_List==ppLeft->m_List;
	return false;
}

ULONGLONG _CFlatValueUniqueList::GetId(const _CAbstractFlatValueCont* pCont) const
{
	if(pCont==&FlatOneCont)
		return 1;
	ORDER_MAP::const_iterator it=m_OrderMap.find(pCont);
	return it==m_OrderMap.end()?0:it->second;
}

//_CAbstractFlatValueCont* _CFlatValueUniqueList::GetPtr(ULONGLONG Id)
//{
//	if(Id==1)
//		return &FlatOneCont;
//	ORDER_MAP2::iterator it=m_OrderMap2.find(Id);
//	return it==m_OrderMap2.end()?NULL:it->second;
//}

_CFlatSingleValueCont* _CFlatValueUniqueList::Path2ValueCont(const COMPONENTS_PATH* pPath, size_t Power=1)
{
	ULONGLONG Hash=CP2Hash(pPath,HASH_CORE,Power);
	pair<MAP::iterator,MAP::iterator> Range=m_Map.equal_range(Hash);
	for(MAP::iterator it=Range.first;it!=Range.second;it++)
	{
		_CAbstractFlatValueCont* pOldSource=it->second;
		if(pOldSource->GetType()!=_CAbstractFlatValueCont::tFlatSingleValueCont)
			continue;
		return dynamic_cast<_CFlatSingleValueCont*> (pOldSource);
	}
	string estr=string("Ooops! Impossible! Component ")+pPath->ToString()+" not found";
	RISEPDD(eWrongCondition,estr.c_str());
}

_CAbstractFlatValueCont* _CFlatValueUniqueList::GetUnique(_CAbstractFlatValueCont** ppNewSource)
{
	ASSERTPDD(ppNewSource!=NULL && (*ppNewSource)!=NULL);
	_CAbstractFlatValueCont* pNewSource=*ppNewSource;
	if(pNewSource==&FlatOneCont)
		return pNewSource;
	//if(pNewSource->GetType()==_CAbstractFlatValueCont::tFlatOneCont && pNewSource->IsSgn1())
	//{
	//	delete pNewSource;
	//	return &FlatOneCont;
	//}
	ULONGLONG Hash=pNewSource->GetHash();
	pair<MAP::iterator,MAP::iterator> Range=m_Map.equal_range(Hash);
	for(MAP::iterator it=Range.first;it!=Range.second;it++)
	{
		_CAbstractFlatValueCont* pOldSource=it->second;
		if(pNewSource->IsEqual(pOldSource))
		{
			delete pNewSource;
			(*ppNewSource)=NULL;
			return pOldSource;
		}
	}
	m_Map.insert(MAP::value_type(Hash,pNewSource));
	m_OrderMap.insert(ORDER_MAP::value_type(pNewSource,m_Counter));
	m_OrderMap2.insert(ORDER_MAP2::value_type(m_Counter++,pNewSource));
	return pNewSource;
}

_CFlatSingleValueCont* _CFlatValueUniqueList::GetUnique(_CFlatSingleValueCont*& pNewSource)
{
	_CAbstractFlatValueCont* pASource=pNewSource;
	_CFlatSingleValueCont* pResult=dynamic_cast<_CFlatSingleValueCont*> (GetUnique(&pASource));
	if(pASource==NULL)
		pNewSource=NULL;
	return pResult;
}

_CFlatProductCont* _CFlatValueUniqueList::GetUnique(_CFlatProductCont*& pNewSource)
{
	_CAbstractFlatValueCont* pASource=pNewSource;
	_CFlatProductCont* pResult=dynamic_cast<_CFlatProductCont*> (GetUnique(&pASource));
	if(pASource==NULL)
		pNewSource=NULL;
	return pResult;
}

_CFlatSumCont* _CFlatValueUniqueList::GetUnique(_CFlatSumCont*& pNewSource)
{
	_CAbstractFlatValueCont* pASource=pNewSource;
	_CFlatSumCont* pResult=dynamic_cast<_CFlatSumCont*> (GetUnique(&pASource));
	if(pASource==NULL)
		pNewSource=NULL;
	return pResult;
}


bool _CFlatResultDevCache::Register(const _CFlatVertex* pVertex, SIGNED_TERM Result)
{
	MAP::iterator fit=m_Map.find(pVertex);
	if(fit!=m_Map.end())
		return false;
	m_Map.insert(MAP::value_type(pVertex,Result));
	return true;
}

SIGNED_TERM _CFlatResultDevCache::GetRegistered(const _CFlatVertex* pVertex)
{
	MAP::iterator fit=m_Map.find(pVertex);
	return fit==m_Map.end()?SIGNED_TERM(0,NULL):fit->second;
}

bool _CFlatResultDevCache::Register(const _CFlatVertex* pVertex, short Sgn,const _CFlatProductCont& Product)
{
	MAP::iterator fit=m_Map.find(pVertex);
	if(fit!=m_Map.end())
		return false;
	if(Product.m_List.size()==1)
		m_Map.insert(MAP::value_type(pVertex,SIGNED_TERM(Sgn,Product.m_List.front())));
	else
	{
		_CFlatProductCont* pNew=new _CFlatProductCont(Product);
		_CFlatProductCont* pUnique=m_FlatValueUniqueList.GetUnique(pNew);
		m_Map.insert(MAP::value_type(pVertex,SIGNED_TERM(Sgn,pUnique)));
	}
	return true;
}

bool _CFlatResultDevCache::GetRegistered(short& Sgn,_CFlatProductCont& Product,const _CFlatVertex* pVertex)
{
	MAP::iterator fit=m_Map.find(pVertex);
	if(fit==m_Map.end())
		return false;
	Sgn=fit->second.first;
	_CAbstractFlatValueCont* pCont=fit->second.second;
	if(pCont->GetType()==_CAbstractFlatValueCont::tFlatProductCont)
	{
		Product=*dynamic_cast<_CFlatProductCont*>(pCont);
	}
	else
	{
		Product.m_List.clear();
		Product.m_List.push_back(pCont);
	}
	return true;
}

bool _CFlatResultFactorizer::NotifyFactorizedValue(size_t CofId,size_t sPower, const _CFlatVertex* pVertex, short GlobalSgn)
{
	//m_MaxCofId=max(m_MaxCofId,CofId);
	//m_MaxSPower=max(m_MaxCofId,sPower);
	SIGNED_TERM& Res=m_ResMap.GetTerm(CofId,sPower);
	if(Res==ZERO_TERM)
	{
		_CFlatProductCont ResProduct;
		GetFactorizedValue(Res.first,ResProduct,pVertex);
		Res.first*=GlobalSgn;
		if(ResProduct.m_List.size()==1)
			Res.second=ResProduct.m_List.front();
		else
		{
			_CFlatProductCont* pNew=new _CFlatProductCont(ResProduct);
			Res.second=m_ValueUniqueList.GetUnique(pNew);
		}
	}
	else 
		return false;
	return true;
}

void _CFlatValueUniqueList::ClearGarbage(TOUCH_VECTOR& Used)
{
	for(MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;)
		if(Used.find(it->second)!=Used.end())
			it++;
		else
		{
			_CAbstractFlatValueCont* pCont=it->second;
			ORDER_MAP::iterator oit=m_OrderMap.find(pCont);
			ORDER_MAP2::iterator oit2=m_OrderMap2.find(oit->second);
			delete it->second;
			m_OrderMap.erase(oit);
			m_OrderMap2.erase(oit2);
			it=m_Map.erase(it);
		}
}

void _CFlatResultFactorizer::ClearGarbage()
{
	TOUCH_VECTOR Vector;
	m_ResMap.TouchResults(Vector);
	//for(RES_MAP::iterator it=m_ResMap.begin(),_e=m_ResMap.end();it!=_e;it++)
	//	it->second.second->TouchMe(Vector);
	m_ValueUniqueList.ClearGarbage(Vector);
	m_ResDevCache.Clear();
}

void _CFlatValueUniqueList::List2Stream(txt_stream& stream) const
{
	ORDER_MAP2::const_iterator _e=m_OrderMap2.end();
	for(size_t i=2;i<m_Counter;i++)
	{
		ORDER_MAP2::const_iterator it=m_OrderMap2.find(i);
		if(it==_e)
			continue;
		it->second->Entry2Stream(stream,*this);
	}
}

//void _CFlatResultFactorizer::PrepareAuxList(AUX_LIST& AuxList) const
//{
//	AuxList.clear();
//	for(RES_MAP::const_iterator it=m_ResMap.begin(),_e=m_ResMap.end();it!=_e;it++)
//	{
//		const _CAbstractFlatValueCont* pCont=it->second.second;
//		ULONGLONG Id=m_ValueUniqueList.GetId(pCont);
//		AuxList.insert(AUX_LIST::value_type(Id,pCont));
//	}
//}

void _CFlatResultFactorizer::Regular2Stream(txt_stream& stream) 
{
	stream<<"Developed values are:"<<endl;
	AUX_LIST List;
	PrepareAuxList(List);
	for(AUX_LIST::const_iterator it=List.begin(),_e=List.end();it!=_e;it++)
	{
		stream<<'X'<<it->first<<" = ";
		it->second->OneLevelEntry2Stream(stream/*,m_ValueUniqueList*/);
		stream<<endl;
	}
	//m_AuxList.clear();
}

void _CFlatResultFactorizer::Init()
{
	fstream file("Comp.txt",ios::out | ios::app);
	file<<m_PathStorage.ToString();
	m_PathStorage.PathSt2PathOrdr(m_PathOrder);
	for(COMP_PATH_ORDER::iterator it=m_PathOrder.begin(),_e=m_PathOrder.end();it!=_e;it++)
	{
		_CFlatSingleValueCont *pCont=new _CFlatSingleValueCont(it->first,1);
		m_ValueUniqueList.GetUnique(pCont);
	}
	//m_ResDevCache.Clear();
}

void _CFlatResultFactorizer::GetFactorizedValue(short& Sgn,_CFlatProductCont& OutRes,const _CFlatVertex* pVertex)
{
	if(pVertex==&OneFlatVertex)
	{
		Sgn=1;
		OutRes.m_List.clear();
		OutRes.m_List.push_back(&FlatOneCont);
		return;
	}
	bool IsMultiInheriter=m_FlatVertexCache.IsMultiInheriter(pVertex);
	if(IsMultiInheriter && m_ResDevCache.GetRegistered(Sgn,OutRes,pVertex))
		return;
	size_t MaxDesc = pVertex->GetNoOfDescs();

	ASSERTPDD(MaxDesc > 1);
	const _CFlatVertex* pMainNextVertex = pVertex->GetDescendant(--MaxDesc);
	while (pMainNextVertex == nullptr)
		pMainNextVertex = pVertex->GetDescendant(--MaxDesc);
	GetFactorizedValue(Sgn, OutRes, pMainNextVertex);
	_CFlatSingleValueCont* pCurrValueCont = m_ValueUniqueList.Path2ValueCont(pVertex->GetPath());
	//OutRes.MultplyBy(pCurrValueCont, m_MultTraitor);
	Sgn *= pVertex->GetMult(MaxDesc);

	for (size_t i = MaxDesc; i > 0; i--)
	{
		OutRes.MultplyBy(pCurrValueCont,m_MultTraitor);
		//Sgn *= pVertex->GetMult(MaxDesc - 1);
		const _CFlatVertex* pNextVertex = pVertex->GetDescendant(i - 1);
		if (pNextVertex == nullptr)
			continue;
		short AuxSgn = 1;
		_CFlatProductCont AuxCont;
		GetFactorizedValue(AuxSgn, AuxCont, pNextVertex);
		AuxSgn *= pVertex->GetMult(i - 1);
		Add(Sgn, OutRes, AuxSgn, AuxCont);
	}
	if(IsMultiInheriter)
		m_ResDevCache.Register(pVertex,Sgn,OutRes);
}

//SIGNED_TERM _CFlatResultFactorizer::GetFactorizedValue(const _CFlatVertex* pVertex)
//{
//	if(pVertex==&OneFlatVertex)
//		return SIGNED_TERM(1,&FlatOneCont);
//	SIGNED_TERM Result;
//	bool IsMultiInheriter=m_FlatVertexCache.IsMultiInheriter(pVertex);
//	if(IsMultiInheriter)
//	{
//		Result=m_ResDevCache.GetRegistered(pVertex);
//		if(Result!=SIGNED_TERM(0,nullptr))
//			return Result;
//	}
//	const _CFlatVertex* p1NextVertex=pVertex->Get1Descendent();
//	short Mult1=pVertex->Get1Mult();
//	const _CFlatVertex* p0NextVertex=pVertex->Get0Descendent();
//	short Mult0=pVertex->Get0Mult();
//	SIGNED_TERM lRes=p0NextVertex==NULL?ZERO_TERM:GetFactorizedValue(p0NextVertex);
//	lRes.first*=Mult0;
//	SIGNED_TERM hRes=p1NextVertex==NULL?ZERO_TERM:GetFactorizedValue(p1NextVertex);
//	hRes.first*=Mult1;
//	//_CFlatSingleValueCont* pNewValueCont=new _CFlatSingleValueCont(pVertex->GetPath());
//	try
//	{
//		_CFlatSingleValueCont* pCurrValueCont=m_ValueUniqueList.Path2ValueCont(pVertex->GetPath());
//		//_CFlatSingleValueCont* pCurrValueCont=m_ValueUniqueList.GetUnique(pNewValueCont);
//		Result=MultBySingleValue(hRes,SIGNED_TERM(1,pCurrValueCont));
//		if(lRes!=ZERO_TERM)
//			Result=Add(Result,lRes);
//	if(IsMultiInheriter)
//		m_ResDevCache.Register(pVertex,Result);
//	}
//	catch(exception)
//	{
//		//delete pNewValueCont;
//		throw;
//	}
//	return Result;
//}

SIGNED_TERM _CFlatResultFactorizer::MultBySingleValue(SIGNED_TERM InputValue, SIGNED_TERM SimpMultiplier)
{
	short Sgn=InputValue.first*SimpMultiplier.first;
	switch(InputValue.second->GetType())
	{
	case _CAbstractFlatValueCont::tFlatOneCont: 
		return SIGNED_TERM(Sgn,SimpMultiplier.second);
	case _CAbstractFlatValueCont::tFlatSingleValueCont: 
	case _CAbstractFlatValueCont::tFlatSumCont: 
		{
			//_CFlatMultTraitor Traitor(m_ValueUniqueList);
			_CFlatProductCont* pNew=new _CFlatProductCont(InputValue.second,SimpMultiplier.second,m_MultTraitor);
			return SIGNED_TERM(Sgn,m_ValueUniqueList.GetUnique(pNew));
		}
	case _CAbstractFlatValueCont::tFlatProductCont: 
		{
			//_CFlatMultTraitor Traitor(m_ValueUniqueList);
			_CFlatProductCont* pInp=dynamic_cast<_CFlatProductCont*>(InputValue.second);
			ASSERTPDD(pInp!=NULL);
			_CFlatProductCont* pNew=new _CFlatProductCont(*pInp,SimpMultiplier.second,m_MultTraitor);
			return SIGNED_TERM(Sgn,m_ValueUniqueList.GetUnique(pNew));
		}
	}
	RISEPDD(eWrongCondition,"Illegal FlatValueContainer");
}

//_CFlatSumCont::_CFlatSumCont(PTR_LIST& Left, PTR_LIST& Right,_CFlatResultFactorizer& Factorizer)
//{
//}

void _CFlatSumCont::MultiplyByScalar(PTR_LIST& List, short Scalar)
{
	if(Scalar!=1)
		for(PTR_LIST::iterator it=List.begin(),_e=List.end();it!=_e;it++)
			(*it).first*=Scalar;
}

_CFlatSumCont::_CFlatSumCont(SIGNED_TERM Left, SIGNED_TERM Right, _CFlatSumTraitor& SumTraitor):
m_List(Left.second->GetType()==_CAbstractFlatValueCont::tFlatSumCont?
	   dynamic_cast<_CFlatSumCont*> (Left.second)->m_List:
	   PTR_LIST(1,Left))
{
	//_CFlatSumTraitor Traitor(Factorizer.m_ValueUniqueList);
	PTR_LIST RightList(Right.second->GetType()==_CAbstractFlatValueCont::tFlatSumCont?
	   dynamic_cast<_CFlatSumCont*> (Right.second)->m_List:
	   PTR_LIST(1,Right));
	if(Left.second->GetType()==_CAbstractFlatValueCont::tFlatSumCont)
		MultiplyByScalar(m_List,Left.first);
	if(Right.second->GetType()==_CAbstractFlatValueCont::tFlatSumCont)
		MultiplyByScalar(RightList,Right.first);
	m_List.merge(RightList,SumTraitor);
	//if(Left.second->GetType()==_CAbstractFlatValueCont::tFlatSumCont)
	//{
	//	_CFlatSumCont* psLeft=dynamic_cast<_CFlatSumCont*> (Left.second);
	//	if(Left.first==1)
	//		m_List=psLeft->m_List;
	//	else
	//		for(PTR_LIST::iterator it=psLeft->m_List.begin(),_e=psLeft->m_List.end();it!=_e;it++)
	//		{
	//			SIGNED_TERM& S=*it;
	//			m_List.push_back(SIGNED_TERM(S.first*Left.first,S.second));
	//		}
	//	if(Right.second->GetType()==_CAbstractFlatValueCont::tFlatSumCont)
	//	{
	//		_CFlatSumCont* psRight=dynamic_cast<_CFlatSumCont*>(Right.second);
	//		PTR_LIST m_TmpList;
	//		if(Right.first==1)
	//			m_TmpList=psRight->m_List;
	//		else
	//			for(PTR_LIST::iterator it=psRight->m_List.begin(),_e=psRight->m_List.end();it!=_e;it++)
	//			{
	//				SIGNED_TERM& S=*it;
	//				m_TmpList.push_back(SIGNED_TERM(S.first*Right.first,S.second));
	//			}
	//		m_List.merge(m_TmpList,SumTraitor);
	//	}
	//	else
	//	{
	//		m_List.push_front(Right);
	//	}
	//}
	//else
	//{
	//	if(Right.second->GetType()==_CAbstractFlatValueCont::tFlatSumCont)
	//	{
	//		_CFlatSumCont* psRight=dynamic_cast<_CFlatSumCont*>(Right.second);
	//		if(Right.first==1)
	//			m_List=psRight->m_List;
	//		else
	//			for(PTR_LIST::iterator it=psRight->m_List.begin(),_e=psRight->m_List.end();it!=_e;it++)
	//			{
	//				SIGNED_TERM& S=*it;
	//				m_List.push_back(SIGNED_TERM(S.first*Right.first,S.second));
	//			}
	//		m_List.push_front(Left);
	//	}
	//	else
	//	{
	//		m_List.push_front(Right);
	//		m_List.push_front(Left);
	//	}
	//}
}

_CFlatProductCont::_CFlatProductCont(PTR_LIST& Left, _CAbstractFlatValueCont* pRight,_CFlatMultTraitor& Traitor):m_List(Left)
{
	PTR_LIST AuxList(1,pRight);
	m_List.merge(AuxList,Traitor);
}

void _CFlatResultFactorizer::Add(short& MainSgn,_CFlatProductCont& MainTerm, short AuxSgn, _CFlatProductCont& AuxTerm)
{
	short SgnLeft=MainSgn,
			SgnRight=AuxSgn;
	MainSgn=CommonDivider(SgnLeft,SgnRight);
	_CFlatProductCont::PTR_LIST &MainList=MainTerm.m_List,
		&AuxList=AuxTerm.m_List,
		OutMainList;
	ExtractCF(MainList,AuxList,OutMainList);
	if(OutMainList.size()==0 && AuxList.size()==0)
		return;
	_CAbstractFlatValueCont* pLeftFact=NULL;
	_CAbstractFlatValueCont* pRightFact=NULL;
	size_t _size=0;
	if((_size=OutMainList.size())==0)
		pLeftFact=&FlatOneCont;
	else if(_size==1)
		pLeftFact=OutMainList.front();
	else
	{
		_CFlatProductCont* pNew=new _CFlatProductCont(OutMainList);
		pLeftFact=m_ValueUniqueList.GetUnique(pNew);
	}
	if((_size=AuxList.size())==0)
		pRightFact=&FlatOneCont;
	else if(_size==1)
		pRightFact=AuxList.front();
	else
	{
		_CFlatProductCont* pNew=new _CFlatProductCont(AuxList);
		pRightFact=m_ValueUniqueList.GetUnique(pNew);
	}
	_CFlatSumCont* pNewSum=new _CFlatSumCont(SIGNED_TERM(SgnLeft,pLeftFact),
		SIGNED_TERM(SgnRight,pRightFact),m_SumTraitor);
	_CFlatSumCont* pSum=m_ValueUniqueList.GetUnique(pNewSum);
	MainTerm.MultplyBy(pSum,m_MultTraitor);
}

SIGNED_TERM _CFlatResultFactorizer::Add(SIGNED_TERM ILeft,SIGNED_TERM IRight)
{
	_CFlatProductCont::PTR_LIST AuxLeft,AuxRight,Left, Right, Common;
	const _CFlatProductCont::PTR_LIST *pLeft=NULL, *pRight=NULL;
	short SgnLeft=ILeft.first,
			SgnRight=IRight.first,
			ComSgn=CommonDivider(SgnLeft,SgnRight);
	Abstract2List(pLeft,ILeft.second,AuxLeft);
	Abstract2List(pRight,IRight.second,AuxRight);
	//Abstract2List(Left,ILeft.second);
	//Abstract2List(Right,IRight.second);
	ExtractCF(*pLeft,*pRight,Left,Right,Common);
	//ExtractCF(Left,Right,Common);
	_CAbstractFlatValueCont* pLeftFact=NULL;
	_CAbstractFlatValueCont* pRightFact=NULL;
	size_t _size=0;
	if((_size=Left.size())==0)
		pLeftFact=&FlatOneCont;
	else if(_size==1)
		pLeftFact=Left.front();
	else
	{
		_CFlatProductCont* pNew=new _CFlatProductCont(Left);
		pLeftFact=m_ValueUniqueList.GetUnique(pNew);
	}
	if((_size=Right.size())==0)
		pRightFact=&FlatOneCont;
	else if(_size==1)
		pRightFact=Right.front();
	else
	{
		_CFlatProductCont* pNew=new _CFlatProductCont(Right);
		pRightFact=m_ValueUniqueList.GetUnique(pNew);
	}
	//_CFlatSumTraitor SumTraitor(m_ValueUniqueList);
	_CFlatSumCont* pNewSum=new _CFlatSumCont(SIGNED_TERM(SgnLeft,pLeftFact),
		SIGNED_TERM(SgnRight,pRightFact),m_SumTraitor);
	_CFlatSumCont* pSum=m_ValueUniqueList.GetUnique(pNewSum);
	if(Common.empty())
		return SIGNED_TERM(ComSgn,pSum);
	//_CFlatMultTraitor MultTraitor(m_ValueUniqueList);
	_CFlatProductCont* pNewProduct=new _CFlatProductCont(Common,pSum,m_MultTraitor);
	return SIGNED_TERM(ComSgn,m_ValueUniqueList.GetUnique(pNewProduct));
}

//void _CFlatResultFactorizer::Abstract2List(_CFlatProductCont::PTR_LIST& List,_CAbstractFlatValueCont* pInputValue)
//{
//	if(pInputValue->GetType()==_CAbstractFlatValueCont::tFlatProductCont)
//	{
//		_CFlatProductCont* pProduct=dynamic_cast<_CFlatProductCont*>(pInputValue);
//		ASSERTPDD(pProduct!=NULL);
//		List=pProduct->m_List;
//	}
//	else
//	{
//		List.clear();
//		List.push_back(pInputValue);
//		return;
//	}
//}

void _CFlatResultFactorizer::Abstract2List(const _CFlatProductCont::PTR_LIST*& pList,_CAbstractFlatValueCont* pInputValue,_CFlatProductCont::PTR_LIST& Aux)
{
	if(pInputValue->GetType()==_CAbstractFlatValueCont::tFlatProductCont)
	{
		_CFlatProductCont* pProduct=dynamic_cast<_CFlatProductCont*>(pInputValue);
		ASSERTPDD(pProduct!=NULL);
		pList=&(pProduct->m_List);
	}
	else
	{
		Aux.clear();
		Aux.push_back(pInputValue);
		pList=&Aux;
	}
}

void _CFlatResultFactorizer::ExtractCF(_CFlatProductCont::PTR_LIST& InOutMainList, _CFlatProductCont::PTR_LIST& InOutAuxList,
	_CFlatProductCont::PTR_LIST& OutDiffMainList)
{
	OutDiffMainList.clear();
	_CFlatProductCont::PTR_LIST::iterator it_m=InOutMainList.begin(),_em=InOutMainList.end();
	_CFlatProductCont::PTR_LIST::iterator it_a=InOutAuxList.begin(),_ea=InOutAuxList.end();
	do
	{
		ULONGLONG Id_m=it_m==_em?0:m_ValueUniqueList.GetId(*it_m);
		ULONGLONG Id_a=it_a==_ea?0:m_ValueUniqueList.GetId(*it_a);
		while(Id_m>Id_a)
		{
			OutDiffMainList.push_back(*it_m);
			it_m=InOutMainList.erase(it_m);
			Id_m=it_m==_em?0:m_ValueUniqueList.GetId(*it_m);
		}
		if(Id_m==Id_a && Id_m!=0)
		{
			it_a=InOutAuxList.erase(it_a);
			it_m++;
			Id_m=it_m==_em?0:m_ValueUniqueList.GetId(*it_m);
			Id_a=it_a==_ea?0:m_ValueUniqueList.GetId(*it_a);
		}
		while(Id_m<Id_a)
		{
			it_a++;
			Id_a=it_a==_ea?0:m_ValueUniqueList.GetId(*it_a);
		}
	}
	while (it_m!=_em || it_a!=_ea);


}

void _CFlatResultFactorizer::ExtractCF(const _CFlatProductCont::PTR_LIST& InList1,
										const _CFlatProductCont::PTR_LIST& InList2,
										_CFlatProductCont::PTR_LIST& OutList1,
										_CFlatProductCont::PTR_LIST& OutList2,
										_CFlatProductCont::PTR_LIST& Common)
{
	Common.clear();
	OutList1.clear();
	OutList2.clear();
	_CFlatProductCont::PTR_LIST::const_iterator it1=InList1.begin(),_e1=InList1.end();
	_CFlatProductCont::PTR_LIST::const_iterator it2=InList2.begin(),_e2=InList2.end();
	ULONGLONG Id1=it1==_e1?0:m_ValueUniqueList.GetId(*it1);
	ULONGLONG Id2=it2==_e2?0:m_ValueUniqueList.GetId(*it2);
	do
	{
		while(Id1>Id2)
		{
			OutList1.push_back(*(it1++));
			Id1=it1==_e1?0:m_ValueUniqueList.GetId(*it1);
		}
		if(Id1==Id2 && Id1!=0)
		{
			Common.push_back(*(it1++));
			it2++;
			Id1=it1==_e1?0:m_ValueUniqueList.GetId(*it1);
			Id2=it2==_e2?0:m_ValueUniqueList.GetId(*it2);
		}
		while(Id1<Id2)
		{
			OutList2.push_back(*(it2++));
			Id2=it2==_e2?0:m_ValueUniqueList.GetId(*it2);
		}
	}
	while(it1!=_e1 || it2!=_e2);
}

//void _CFlatResultFactorizer::ExtractCF(_CFlatProductCont::PTR_LIST& List1,
//									   _CFlatProductCont::PTR_LIST& List2,
//									   _CFlatProductCont::PTR_LIST& Common)
//{
//	Common.clear();
//	size_t _size1=List1.size(),
//		_size2=List2.size();
//	_CFlatProductCont::PTR_LIST *pMainList=NULL, *pSubsetList=NULL; 
//	if(_size1>_size2)
//	{
//		pMainList=&List1;
//		pSubsetList=&List2;
//	}
//	else
//	{
//		pMainList=&List2;
//		pSubsetList=&List1;
//	}
//	_CFlatProductCont::PTR_LIST::iterator it_sub=pSubsetList->begin(),
//		_e_sub=pSubsetList->end();
//	while(it_sub!=_e_sub)
//	{
//		_CFlatProductCont::PTR_LIST::iterator it=pMainList->begin(),
//			_e_main=pMainList->end();
//		for(;it!=_e_main;it++)
//			if(*it==*it_sub)
//			{
//				Common.push_back(*it);
//				it=pMainList->erase(it);
//				it_sub=pSubsetList->erase(it_sub);
//				break;
//			}
//		if(it==_e_main && it_sub!=_e_sub)
//			it_sub++;
//	}
//}


_CFlatProductCont::_CFlatProductCont(const _CFlatProductCont& Source,
									 _CAbstractFlatValueCont* pMultiplier,
									 _CFlatMultTraitor& Traitor):m_List(Source.m_List)
{
	PTR_LIST Aux(1,pMultiplier);
	m_List.merge(Aux,Traitor);
	//m_List.push_front(pMultiplier);
}

_CFlatProductCont::_CFlatProductCont(_CAbstractFlatValueCont* pLeft,
									 _CAbstractFlatValueCont* pRight,
									 _CFlatMultTraitor& Traitor):m_List(1,pLeft)
{
	PTR_LIST Aux(1,pRight);
	m_List.merge(Aux,Traitor);
	//if(Traitor.operator ()(pLeft,pRight))
	//	m_List.push_back(pRight);
	//else
	//	m_List.push_front(pRight);
}

_CFlatProductCont& _CFlatProductCont::MultplyBy(_CAbstractFlatValueCont* pMultiplier,_CFlatMultTraitor& Traitor)
{
	if(m_List.size()==1 && m_List.front()->GetType()==tFlatOneCont)
		m_List.clear();
	switch(pMultiplier->GetType())
	{
	case tFlatOneCont: break;
	case tFlatSingleValueCont:
	case tFlatSumCont:
		{
			PTR_LIST AuxList(1,pMultiplier);
			m_List.merge(AuxList,Traitor);
			break;
		}
	case tFlatProductCont:
		{
			PTR_LIST AuxList(dynamic_cast<_CFlatProductCont*>(pMultiplier)->m_List);
			m_List.merge(AuxList,Traitor);
		}
	}
	return *this;
}

void _CFlatSingleValueCont::TouchMe(TOUCH_VECTOR& Vector) const
{
	Vector.insert(this);
}

void _CFlatProductCont::TouchMe(TOUCH_VECTOR& Vector) const
{
	if(Vector.insert(this).second)
		for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
			(*it)->TouchMe(Vector);
}

void _CFlatSumCont::TouchMe(TOUCH_VECTOR& Vector) const
{
	if(Vector.insert(this).second)
		for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
			(*it).second->TouchMe(Vector);
}

//_CFlatResultFactorizer::SIGNED_ID _CFlatResultFactorizer::GetValueId(size_t CofId,size_t sPower) const
//{
//	SIGNED_ID Res(0,0);
//	RES_MAP::const_iterator it=m_ResMap.find(ENTRY(CofId,sPower));
//	if(it!=m_ResMap.end())
//	{
//		Res.first=it->second.first;
//		Res.second=m_ValueUniqueList.GetId(it->second.second);
//	}
//	return Res;
//}

//SIGNED_TERM _CFlatResultFactorizer::GetTerm(size_t CofId,size_t sPower) const
//{
//	RES_MAP::const_iterator it=m_ResMap.find(ENTRY(CofId,sPower));
//	return it!=m_ResMap.end()?it->second:ZERO_TERM;
//}

void _CAbstractFlatValueCont::SubEntry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const
{
	ULONGLONG EntryId=UniqueList.GetId(this);
	stream<<'X'<<EntryId;
}
void _CAbstractFlatValueCont::SubEntry2String(string& OutStr, const _CFlatValueUniqueList& UniqueList) const 
{
	ULONGLONG EntryId=UniqueList.GetId(this);
	OutStr+="X";
	OutStr+=ulong2str(EntryId);
}

void _CFlatOneCont::SubEntry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const
{
	stream<<1;
}

void _CFlatSingleValueCont::SubEntry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const
{
	OneLevelEntry2Stream(stream);
}
void _CFlatSingleValueCont::SubEntry2String(string& OutStr, const _CFlatValueUniqueList& UniqueList) const 
{
	OneLevelEntry2String(OutStr);
}

void _CFlatSingleValueCont::OneLevelEntry2Stream(txt_stream& stream) const 
{
	stream << m_Value->ToString();
	if (m_Power != 1)
		stream << "^(" << m_Power << ")";
}

void _CFlatSingleValueCont::OneLevelEntry2String(string& OutStr) const 
{
	OutStr += m_Value->ToString();
	if (m_Power != 1)
	{
		OutStr += "^(";
		OutStr += ulong2str(m_Power);
		OutStr += ")";
	}
}


void _CFlatProductCont::Entry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const
{
	ULONGLONG Id=UniqueList.GetId(this);
	stream<<'X'<<Id<<" = ";
	bool first=true;
	for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		if(first)
			first=false;
		else
			stream<<" * ";
		(*it)->SubEntry2Stream(stream,UniqueList);
	}
	stream<<endl;
}

void _CFlatProductCont::Entry2String(string& OutStr,const _CFlatValueUniqueList& UniqueList,bool ReportMyName) const
{
	if(ReportMyName)
	{
		ULONGLONG Id=UniqueList.GetId(this);
		OutStr+='X';
		OutStr+=ulong2str(Id);
		OutStr+=" = ";
	}
	if(m_List.empty())
	{
		OutStr+="1";
	}
	else
	{
		bool first=true;
		for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		{
			if(first)
				first=false;
			else
				OutStr+=" * ";
			(*it)->SubEntry2String(OutStr,UniqueList);
		}
	}
	if(ReportMyName)
		OutStr+='\n';
}

void _CFlatProductCont::OneLevelEntry2Stream(txt_stream& stream) const 
{
	bool first=true;
	for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		if(first)
			first=false;
		else
			stream<<" * ";
		(*it)->OneLevelEntry2Stream(stream);
	}
}

void _CFlatProductCont::OneLevelEntry2String(string& OutStr) const 
{
	bool first=true;
	for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		if(first)
			first=false;
		else
			OutStr+=" * ";
		(*it)->OneLevelEntry2String(OutStr);
	}
}

void _CFlatSumCont::Entry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const
{
	ULONGLONG Id=UniqueList.GetId(this);
	stream<<'X'<<Id<<" = ";
	bool first=true;
	for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		bool Signed=false;
		short Sgn=(*it).first;
		if(Sgn<0)
		{
			stream<<" - ";
			Sgn=abs(Sgn);
			Signed=true;
			first=false;
		} 
		else if(first)
			first=false;
		else
			stream<<" + ";
		if(Sgn!=1)
		{
			stream<<'('<<Sgn<<")*";
			Signed=true;
		}
		//if(first)
		//	first=false;
		//else if(!Signed)
		//	stream<<" + ";
		const _CAbstractFlatValueCont* pCont=(*it).second;
		pCont->SubEntry2Stream(stream,UniqueList);
	}
	stream<<endl;
}

void _CFlatSumCont::Entry2String(string& OutStr,const _CFlatValueUniqueList& UniqueList,bool ReportMyName) const
{
	if(ReportMyName)
	{
		ULONGLONG Id=UniqueList.GetId(this);
		OutStr+='X';
		OutStr+= ulong2str(Id);
		OutStr+=" = ";
	}
	bool first=true;
	for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		bool Signed=false;
		short Sgn=(*it).first;
		if(Sgn<0)
		{
			OutStr+=" - ";
			Sgn=abs(Sgn);
			Signed=true;
		}
		if(Sgn!=1)
		{
			OutStr+='(';
			OutStr+=short2str(Sgn);
			OutStr+=")*";
			Signed=true;
		}
		if(first)
			first=false;
		else if(!Signed)
			OutStr+=" + ";
		const _CAbstractFlatValueCont* pCont=(*it).second;
		pCont->SubEntry2String(OutStr,UniqueList);
	}
	OutStr+='\n';
}

void _CFlatSumCont::OneLevelEntry2Stream(txt_stream& stream) const 
{
	stream<<'(';
	bool first=true;
	for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		bool Signed=false;
		short Sgn=(*it).first;
		if(Sgn<0)
		{
			stream<<" - ";
			Sgn=abs(Sgn);
			Signed=true;
		}
		if(first)
			first=false;
		else if(!Signed)
			stream<<" + ";
		if(Sgn!=1)
		{
			stream<<'('<<Sgn<<")*";
			Signed=true;
		}
		const _CAbstractFlatValueCont* pCont=(*it).second;
		pCont->OneLevelEntry2Stream(stream);
	}
	stream<<')';
}

void _CFlatSumCont::OneLevelEntry2String(string& OutStr) const
{
	OutStr+='(';
	bool first=true;
	for(PTR_LIST::const_iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
	{
		bool Signed=false;
		short Sgn=(*it).first;
		if(Sgn<0)
		{
			OutStr+=" - ";
			Sgn=abs(Sgn);
			Signed=true;
		}
		if(Sgn!=1)
		{
			OutStr+='(';
			OutStr+=short2str(Sgn);
			OutStr+=")*";
			Signed=true;
		}
		if(first)
			first=false;
		else if(!Signed)
			OutStr+=" + ";
		const _CAbstractFlatValueCont* pCont=(*it).second;
		pCont->OneLevelEntry2String(OutStr);
	}
	OutStr+=')';
}

SIGNED_TERM& _CFlatResultFactorizer::_CResMap::GetTerm(size_t CofId,size_t SPower)
{
	m_MaxCofId=max(m_MaxCofId,CofId);
	m_MaxSPower=max(m_MaxSPower,SPower);
	return m_ResMap[ENTRY(CofId,SPower)];
}
void _CFlatResultFactorizer::_CResMap::TouchResults(TOUCH_VECTOR& Vector)
{
	for(RES_MAP::iterator it=m_ResMap.begin(),_e=m_ResMap.end();it!=_e;it++)
		it->second.second->TouchMe(Vector);
}
void _CFlatResultFactorizer::_CResMap::PrepareAuxList(AUX_LIST& AuxList) const
{
	AuxList.clear();
	for(RES_MAP::const_iterator it=m_ResMap.begin(),_e=m_ResMap.end();it!=_e;it++)
	{
		const _CAbstractFlatValueCont* pCont=it->second.second;
		ULONGLONG Id=m_ValueUniqueList.GetId(pCont);
		AuxList.insert(AUX_LIST::value_type(Id,pCont));
	}
}
_CFlatResultFactorizer::SIGNED_ID _CFlatResultFactorizer::_CResMap::GetValueId(size_t CofId,size_t sPower) const
{
	SIGNED_ID Res(0,0);
	RES_MAP::const_iterator it=m_ResMap.find(ENTRY(CofId,sPower));
	if(it!=m_ResMap.end())
	{
		Res.first=it->second.first;
		Res.second=m_ValueUniqueList.GetId(it->second.second);
	}
	return Res;
}
SIGNED_TERM _CFlatResultFactorizer::_CResMap::GetTerm(size_t CofId,size_t sPower) const
{
	RES_MAP::const_iterator it=m_ResMap.find(ENTRY(CofId,sPower));
	return it!=m_ResMap.end()?it->second:ZERO_TERM;
}

size_t _CFlatResultFactorizer::_CFunctionPairsManager::DefineTransferFunctionSimp(const string& Name, size_t NumeratorCof, const _CCofactor* pNumCof, 
		size_t DenominatorCof, const _CCofactor* pDenCof)
{
	m_FunctionPairs.insert(FUNCTION_PAIRS::value_type(m_Counter,
		_CTransferFunctPair(Name,DenominatorCof,pDenCof,NumeratorCof,pNumCof,m_FlatValueUniqueList)));
	return m_Counter++;
}

void _CFlatResultFactorizer::_CCofResCont::CopyAndTranslate(_CResMap::COF_MAP& CofMap)
{
	for(_CResMap::COF_MAP::iterator it=CofMap.begin(),_e=CofMap.end();it!=_e;it++)
		m_SPowerCoeffs.insert(
			SPOWER_COEFFS::value_type(
				it->first,SIGNED_MULT(it->second.first,
					_CFlatProductCont(it->second.second))));
	FindCommonFactor();
}

bool _CFlatResultFactorizer::_CCofResCont::GetComStr(string& Str) const
{
	if(m_CommonFactor.m_List.empty())
	{
		Str+="1";
		return false;
	}
	else
	{
		m_CommonFactor.Entry2String(Str,m_FlatValueUniqueList,false);
		return true;
	}
}

bool _CFlatResultFactorizer::_CCofResCont::GetSExpStr(string& Str) const
{
	bool first=true;
	for(SPOWER_COEFFS::const_iterator it=m_SPowerCoeffs.begin(),_e=m_SPowerCoeffs.end();it!=_e;it++)
	{
		size_t SPower=it->first;
		short Mult=it->second.first;
		short Sgn=1;
		if(Mult<0)
		{	
			Mult=-Mult;
			Sgn=-1;
		}
		const _CFlatProductCont& Coeff=it->second.second;
		if(Sgn<0)
		{
			Str+=" - ";
			first=false;
		} else if(!first)
			Str+=" + ";
		else
			first=false;
		if(SPower>1)
		{
			Str+="s^(";
			Str+=ulong2str(SPower);
			Str+=")*";
		}
		else if(SPower==1)
			Str+="s*";
		if(Mult!=1)
		{
			Str+=short2str(Mult);
			Str+='*';
		}
		Coeff.Entry2String(Str,m_FlatValueUniqueList,false);
	}
	return Str=="1";
}

void _CFlatResultFactorizer::_CCofResCont::FindCommonFactor()
{
	AUX_LIST AuxList;
	_CAuxCont* pMainCont=PrepareAuxList(AuxList);
	while(pMainCont!=NULL)
	{
		_CAuxCont* pNewMainCont=NULL;
		ULONGLONG NewCurrId=numeric_limits<ULONGLONG>::max(),
			CurrentId=pMainCont->GetCurrentId();
		for(AUX_LIST::iterator it=AuxList.begin(),_e=AuxList.end();it!=_e;it++)
		{
			_CAuxCont& CurrCont=(*it);
			ULONGLONG FoundId=CurrCont.FindId(CurrentId);
			if(FoundId<CurrentId && FoundId<NewCurrId)
			{
				NewCurrId=FoundId;
				pNewMainCont=NewCurrId==0?NULL:&CurrCont;
			}
		}
		if(NewCurrId!=0 && pNewMainCont==NULL)
		{
			m_CommonFactor.m_List.push_back(pMainCont->GetCurrentFactor());
			for(AUX_LIST::iterator it=AuxList.begin(),_e=AuxList.end();it!=_e;it++)
			{
				_CAuxCont& CurrCont=(*it);
				CurrCont.RemoveCurrent();
				if(CurrCont.GetCurrentId()<NewCurrId)
				{
					NewCurrId=CurrCont.GetCurrentId();
					pMainCont=NewCurrId==0?NULL:&CurrCont;
				}
			}
		}
		else
			pMainCont=pNewMainCont;
	}
}

bool _CFlatResultFactorizer::_CFunctionPairsManager::TranslateTrans2DenAndNum(size_t FunctionNo, size_t& Num, size_t& Den)
{
	auto fit = m_FunctionPairs.find(FunctionNo);
	if (fit == m_FunctionPairs.end())
		return false;
	fit->second.GetNumAndCoffId(Num, Den);
	return true;
}
const string& _CFlatResultFactorizer::_CFunctionPairsManager::Name(size_t FunctionNo) const
{
	auto fit = m_FunctionPairs.find(FunctionNo);
	if (fit == m_FunctionPairs.end())
		return EmptyString;
	return fit->second.Name();
}

void _CFlatResultFactorizer::_CFunctionPairsManager::PrepareTrasferFuctions()
{
	for(FUNCTION_PAIRS::iterator it=m_FunctionPairs.begin(),_e=m_FunctionPairs.end();it!=_e;it++)
		it->second.PrepareTrasferFuctions(m_ResMap);
}
void _CFlatResultFactorizer::_CFunctionPairsManager::ToStream(size_t TrId,txt_stream& stream)
{
	FUNCTION_PAIRS::iterator it=m_FunctionPairs.find(TrId);
	if(it==m_FunctionPairs.end())
		return;
	it->second.ToStream(stream);
}

void _CFlatResultFactorizer::_CFunctionPairsManager::ToStream(txt_stream& stream)
{
	if(m_FunctionPairs.empty())
		return;
	stream<<endl<<"Transfer function after simplifications:"<<endl<<endl;
	for(FUNCTION_PAIRS::iterator it=m_FunctionPairs.begin(),_e=m_FunctionPairs.end();it!=_e;it++)
		it->second.ToStream(stream);
}

void _CFlatResultFactorizer::_CTransferFunctPair::ToStream(txt_stream& stream)
{
	string ComNumStr,ComDenStr,SPNumStr,SPDenStr;
	bool SkipDenCom=!Den.GetComStr(ComDenStr),
		SkipNumCom=!Num.GetComStr(ComNumStr),
		SkipDen=Den.GetSExpStr(SPDenStr),
		SkipNum=Num.GetSExpStr(SPNumStr);
	if(SkipDen && SkipNum && SkipDenCom & SkipNumCom)
	{
		stream<<m_Name<<" = 1\n";
		return;
	}
	bool SkipCommon= SkipDenCom && SkipNumCom,
		DownCommon= SkipDenCom && !SkipNumCom,
		SkipAll=SkipDen && SkipNum,
		Down=SkipDen && !SkipNum;
	size_t ComNumLen=SkipCommon?0:ComNumStr.size(),
		ComDenLen=SkipDenCom?0:ComDenStr.size(),
		SPNumLen=SkipAll?0:SPNumStr.size(),
		SPDenLen=SkipDen?0:SPDenStr.size(),
		ComLen=max(ComDenLen,ComNumLen),
		SPLen=max(SPNumLen,SPDenLen),
		Space11=m_Name.size()+3,
		Space12=SkipCommon?0:ComLen-ComNumLen+SkipAll?0:3,
		Space32=SkipCommon?0:ComLen-ComDenLen+SkipAll?0:3;
	if(!(SkipDen && SkipDenCom))
	{
		FillStream(stream,Space11,' ');
		if(SkipDenCom)
			FillStream(stream,ComLen,' ');
		else
		{
			stream<<ComNumStr;
			//FillStream(stream,ComLen-ComNumLen,' ');
		}
		if(!SkipDen)
		{
			FillStream(stream,ComLen-ComNumLen+5,' ');
			stream<<SPNumStr;
		}
		stream<<endl;
	}
	stream<<m_Name<<" = ";
	if(!SkipCommon)
	{
		if(DownCommon)
			stream<<'('<<ComNumStr<<')';
		else
			FillStream(stream,ComLen,'-');
	}
	if(!(SkipCommon || SkipAll))
		stream<<" * ";
	if(!SkipAll)
	{
		if(Down)
			stream<<'('<<SPNumStr<<')';
		else
			FillStream(stream,SPLen,'-');
	}
	stream<<endl;
	if(!(SkipDen && SkipDenCom))
	{
		FillStream(stream,Space11,' ');
		if(SkipDenCom)
			FillStream(stream,ComLen,' ');
		else
		{
			stream<<ComDenStr;
		}
		if(!SkipDen)
		{
			FillStream(stream,ComLen-ComDenLen+5,' ');
			stream<<SPDenStr;
		}
		stream<<endl;
	}

	stream<<endl;
}

void _CFlatResultFactorizer::_CTransferFunctPair::PrepareTrasferFuctions(_CFlatResultFactorizer::_CResMap& ResMap)
{
	_CResMap::COF_MAP NumCoffs,DenCoffs;
	ResMap.GetCofCoeffs(m_NumCofId,NumCoffs);
	ResMap.GetCofCoeffs(m_DenCofId,DenCoffs);
	Num.CopyAndTranslate(NumCoffs);
	Den.CopyAndTranslate(DenCoffs);
	ReduceCommonFactors();
}

void _CFlatResultFactorizer::_CTransferFunctPair::ReduceCommonFactors()
{
	_CFlatProductCont::PTR_LIST& NumList=Num.m_CommonFactor.m_List;
	_CFlatProductCont::PTR_LIST& DenList=Den.m_CommonFactor.m_List;

	_CFlatProductCont::PTR_LIST::iterator n_it=NumList.begin(),n_e=NumList.end();
	_CFlatProductCont::PTR_LIST::iterator d_it=DenList.begin(),d_e=DenList.end();
	ULONGLONG Id_n=n_it==n_e?0:m_FlatValueUniqueList.GetId(*n_it);
	ULONGLONG Id_d=d_it==d_e?0:m_FlatValueUniqueList.GetId(*d_it);
	do
	{
		while(Id_n>Id_d)
		{
			n_it++;
			Id_n=n_it==n_e?0:m_FlatValueUniqueList.GetId(*n_it);
		}
		while(Id_n<Id_d)
		{
			d_it++;
			Id_d=d_it==d_e?0:m_FlatValueUniqueList.GetId(*d_it);
		}
		if(Id_n==Id_d && Id_n!=0)
		{
			n_it=NumList.erase(n_it);	
			Id_n=n_it==n_e?0:m_FlatValueUniqueList.GetId(*n_it);
			d_it=DenList.erase(d_it);	
			Id_d=d_it==d_e?0:m_FlatValueUniqueList.GetId(*d_it);
		}
	}
	while(n_it!=n_e || d_it!=d_e);
}

void _CFlatResultFactorizer::_CResMap::GetCofCoeffs(size_t CofId,COF_MAP& OutMap) const
{
	for(RES_MAP::const_iterator it=m_ResMap.begin(),_e=m_ResMap.end();it!=_e;it++)
		if(it->first.first==CofId)
			OutMap.insert(COF_MAP::value_type(it->first.second,it->second));
}

_CFlatResultFactorizer::_CCofResCont::_CAuxCont* _CFlatResultFactorizer::_CCofResCont::PrepareAuxList(AUX_LIST& List)
{
	_CAuxCont* pRes=NULL;
	ULONGLONG MinId=numeric_limits<ULONGLONG>::max();
	for(SPOWER_COEFFS::iterator it=m_SPowerCoeffs.begin(),_e=m_SPowerCoeffs.end();it!=_e;it++)
	{
		List.push_back(_CAuxCont(it->second.second.m_List,m_FlatValueUniqueList));
		_CAuxCont* pAuxCont=&List.back();
		if(pAuxCont->GetCurrentId()<MinId)
		{
			pRes=pAuxCont;
			MinId=min(MinId,pAuxCont->GetCurrentId());
		}
	}
	return pRes;
}

