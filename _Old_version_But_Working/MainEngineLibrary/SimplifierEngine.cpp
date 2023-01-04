#include "StdAfx.h"
#include "SimplifierEngine.h"
#include "MainCircuit.h"

_CPreFlatVertexList EmptyList;
NumericType OneValue(1.0);
_CPreFlatVertexContainer OnePreFlatVertex(EmptyList,OneValue,0);

ULONGLONG ToHash(const MVerticesList& List)
{
	ULONGLONG Res=HASH_CORE;
	Res*=HASH_FACTOR;
	Res^=List.size();
	for(MVerticesList::const_iterator it=List.begin(), _e=List.end();it!=_e;it++)
	{
		Res*=HASH_FACTOR;
		Res^=(ULONGLONG)(*it);
	}
	return Res;
}

//double _CPreFlatVertexContainer::GetParameterValue()
//{
//	return 0.0;
//}

bool _CPreFlatMap::ReturnedToOld(bool NewOne)
{
	if(m_OldTrace)
	{
		if(NewOne)
			m_OldTrace=false;
	}
	else
	{
		if(!NewOne)
		{
			m_OldTrace=true;
			return true;
		}
	}
	return false;
}

_CPreFlatMap::TRACE_RESULTS _CPreFlatMap::DetermineTraceResult(bool CurrentNew)
{
	bool RToOld=ReturnedToOld(CurrentNew);
	if(RToOld)
		return TERMINAL_REACHED;
	if(CurrentNew)
		return NEW_ONE;
	return STILL_OLD;
}


_CPreFlatMap::TRACE_RESULTS _CPreFlatMap::PreparePreFlatVC(const _CModelVerticesPath& Path, size_t SPower, size_t MaxPower, _CPreFlatVertexContainer*& pResult, _CCompRedefEntry& Entry)
{
	ASSERTPDD(SPower<=MaxPower);
	_CPreFlatVertexList* pList=NULL;
	bool ListNew=ForceAt(Path,pList);//
	pResult=&pList->at(SPower,Path,Entry);
	return DetermineTraceResult(pResult->IsNew());
}

//_CPreFlatMap::TRACE_RESULTS _CPreFlatMap::PreparePreFlatVC(_CNewSimplifierData& Data,_CPreFlatVertexContainer*& pResult)
//{
//	const _CModelVerticesPath& Path=Data.ModelVerticesPath;
//	_CPreFlatVertexList* pList=NULL;
//	const NumericType& Value=Path.GetNumericValue();
//	bool ListNew=ForceAt(Path,pList);//
//	//_CSimplificationOrdinateur::COORDINATES CTest(17,23);
//	//unsigned long long Test=Coordinates2ULL(CTest);
//	pResult=&pList->at(Data.m_TopCoordinates,Path);
//	bool NewOne=pResult->IsNew();
//	bool RToOld=ReturnedToOld(NewOne);
//	if(RToOld)
//	{
//		//TerminalVerticesPath=Path;
//		return TERMINAL_REACHED;
//	}
//	if(NewOne)
//		return NEW_ONE;
//	return STILL_OLD;
//}

NumericType& _CPreFlatMap::_UpdateValues(_CPreFlatVertexContainer* pTopContainer, 
	_CMainCircuit* pMainCircuit, _CCompRedefEntry& RedefEntry)
{
 	static NumericType One(1.0);
	static NumericType Zero(0.0);
	if(pTopContainer==NULL)
		return Zero;
	if(pTopContainer==&OnePreFlatVertex)
		return One;
	if(!pTopContainer->m_NeedToBeUpdated)
		return pTopContainer->m_CurrentTopValue;
	NumericType& TopValue=pTopContainer->m_CurrentTopValue;
	TopValue=0.0;
	size_t NoOfDescs = pTopContainer->NoOfDescs();
	_CCompRedefEntry& LevelEntry = RedefEntry[pTopContainer->VerticesPath()];
	const NumericType& Value = pTopContainer->m_ParamValue;
	NumericType AuxValue(1.0);
#ifdef _DEBUG
	NumericType Value2 = LevelEntry.IsNumeric() ? LevelEntry.GetNumericValue() : COMPONENTS_PATH(pTopContainer->GetVerticesPath()).GetNumericValue();
	bool IsEqual = Value2 == Value;
#endif
	for (size_t i = 0; i < NoOfDescs; i++, AuxValue *= Value)
	{
		NumericType& DCurrentValue = pTopContainer->CurrentDescValue(i);
		DCurrentValue = _UpdateValues(pTopContainer->CurrentDesc(i), pMainCircuit, RedefEntry);
		DCurrentValue *= pTopContainer->CurrentSgn(i);
		if (!IsZero(DCurrentValue))
			if (i == 0)
				TopValue += DCurrentValue;
			else
				TopValue += (DCurrentValue*Value);
	}
	pTopContainer->m_NeedToBeUpdated=false;
	return TopValue;
}

bool/*IsNew*/ _CSimplificationOrdinateur::RegisterEntry(unsigned long CofId,unsigned long Power,_CModelVertex* pEntryModelVertex,NumericType V1, NumericType V2,short TopSgn)
{
	COORDINATES Coordinates(CofId,Power);
	MAP::iterator it=m_Map.find(Coordinates);
	if(it!=m_Map.end())
		return false;
	m_Map.insert(MAP::value_type(Coordinates,_CSimplificationEntry(pEntryModelVertex,V1,V2,TopSgn)));
	m_MaxCofId=max(m_MaxCofId,CofId);
	m_MaxPower=max(m_MaxPower,Power);
	return true;
}

_CSimplificationEntry* _CSimplificationOrdinateur::GetPEntry(unsigned long CofId,unsigned long Power)
{
	MAP::iterator it=m_Map.find(COORDINATES(CofId,Power));
	if(it==m_Map.end())
		return NULL;
	else
		return &(it->second);
}

void _CNewSimplifierData::FindSimplifications(_CCompRedefEntry& RedefEntry)
{
	SimplificationOrdinateur.FindSimplifications(*this, RedefEntry);
	SimplificationOrdinateur.TranslatePreFlat2Flat(*this,GetFlatVertexCache());
	SimplificationOrdinateur.MoveTo(OutContextSExpFlatVertices,&PathTraitor.StrApproxContext());
}

void _CSimplificationOrdinateur::FindSimplifications(_CNewSimplifierData& Data, _CCompRedefEntry& RedefEntry)
{
	for(MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
		it->second.FindSimplifications(Data.SetCordinates(it->first)/*,it->first.second*/,RedefEntry);
}

void _CSimplificationEntry::FindSimplifications(_CNewSimplifierData& Data/*,size_t Power*/, _CCompRedefEntry& RedefEntry)
{
	static char* FN="ItTest.txt";
	size_t a=Data.m_TopCoordinates.first;
	bool IsZero=m_ExpectedRange.IsZero();
	if(!Data.IsExact() && m_ExpectedRange.IsZero())
		return;
	Data.ResetGready();
	NumericType NumRes(0);
	NumericType PrevRes(0);
	{
		fstream file(FN,ios::out | ios::app);
		file<<endl<<"CofId ="<<Data.m_TopCoordinates.first<<"Power = "<<Data.m_TopCoordinates.second<<endl;
		file<<"Ezpected Range: "<<m_ExpectedRange.first<<" - "<<m_ExpectedRange.second<<endl;
	}
	do
	{
		PrevRes=NumRes;
		Data.StartTrace();
		m_OutSgn=m_TopSgn;
		auto TopPower=Data.m_TopCoordinates.second;
		m_pEntryModelVertex->PerformNewSimplyfyApprox(m_pOutputEntry, m_OutSgn, TopPower, Data, RedefEntry);
		Data.GreadyFinished();
		if(m_OutSgn!=0 && !Data.IsExact())
			NumRes=Data.PreFlatMap().UpdateValues(m_pOutputEntry,RedefEntry)*m_OutSgn;
		fstream file(FN,ios::out | ios::app);
		file<<"Next Value ="<<NumRes<<"\t Increase = "<<NumRes-PrevRes<<endl;
	}
	while(!Data.IsExact() && (!m_ExpectedRange.IsInBeetween(NumRes) && PrevRes!=NumRes));
	fstream file(FN,ios::out | ios::app);
	file<<"Final Value ="<<NumRes;
	file<<"\t Increase = "<<NumRes-PrevRes;
	file<<endl;
}

_CFlatVertexCache& _CNewSimplifierData::GetFlatVertexCache()
{
	return m_pMainCircuit->m_FlatVertexCache;
}

bool _CPreFlat2FlatCache::Register(_CPreFlatVertexContainer* PreFV,_CFlatVertex* FV,short FVSgn)
{
	return m_Map.insert(MAP::value_type(PreFV,OUT_PAIR(FV,FVSgn))).second;
}

_CPreFlat2FlatCache::OUT_PAIR _CPreFlat2FlatCache::GetExisted(_CPreFlatVertexContainer* PreFV)
{
	MAP::iterator it=m_Map.find(PreFV);
	return it==m_Map.end()?OUT_PAIR(NULL,0):it->second;
}


void _CSimplificationOrdinateur::TranslatePreFlat2Flat(_CNewSimplifierData& Data,_CFlatVertexCache& OutCache)
{
	for(MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
		it->second.TranslatePreFlat2Flat(Data,OutCache);
}

void _CSimplificationEntry::TranslatePreFlat2Flat(_CNewSimplifierData& Data,_CFlatVertexCache& OutCache)
{
	if(m_OutSgn==0 || m_pOutputEntry==nullptr) return;
	m_FinalOutSign=1;//m_OutSgn;
	m_pOutputEntry->CreateFlatVertex(Data,m_FinalFlatVertex,m_FinalOutSign);
	m_FinalOutSign*=m_OutSgn;
	OutCache.TouchVertex(m_FinalFlatVertex);
}

void _CPreFlatVertexContainer::CreateFlatVertex(_CNewSimplifierData& Data,  _CFlatVertex*& pResVertex, short& OutSgn)
{
	static char* FN = "CacheTest.txt";
	if (this == &OnePreFlatVertex)
	{
		pResVertex = &OneFlatVertex;
		OutSgn = 1;
		return;
	}
	_CPreFlat2FlatCache& PreFlat2FlatCache = Data.PF2FCache;
	_CPreFlat2FlatCache::OUT_PAIR Out_pair = PreFlat2FlatCache.GetExisted(this);
	if (Out_pair.first != nullptr)
	{
		pResVertex = Out_pair.first;
		OutSgn = Out_pair.second;
		return;
	}
	size_t NoDescs = NoOfDescs();
	WEEK_DESCENDANTS Descs(NoDescs, nullptr);
	MULTIPLIERS Signs(NoDescs,1);
	bool Empty = true;
	for (size_t i = 0; i < NoDescs; i++)
	{
		_CPreFlatVertexContainer* pCurrDesc = m_CurrDescs[i];
		if (pCurrDesc == nullptr)
		{
			Descs[i] = nullptr;
			Signs[i] = 0;
			continue;
		}
		pCurrDesc->CreateFlatVertex(Data, Descs[i], Signs[i]);
		Signs[i] *= m_Sgns[i];
		if (Descs[i] != nullptr && Signs[i]!=0)
			Empty = false;
#ifdef _DEBUG
		else
		{
			int y = 1;
			y *= 7;
		}
#endif
	}
	if (Empty)
	{
		pResVertex = NULL;
		OutSgn = 0;
	}
	else if (IsHollow(Descs))
	{
		pResVertex = Descs[0];
		OutSgn *= Signs[0];
	}
	else
	{
		_CFlatVertexCache& Cache = Data.GetFlatVertexCache();
		OutSgn *= ExtractCommonFactor(Signs, false);
		DESCENDANTS Descs2;
		Descs2.assign(Descs.begin(), Descs.end());
		_CFlatVertex* pNewVertex = new _CFlatVertex(Descs2, Signs,
			Data.GetMainCircuit()->GetUnique(COMPONENTS_PATH(GetVerticesPath())));
		pResVertex = Cache.PushInCache(pNewVertex, true);
	}
	PreFlat2FlatCache.Register(this, pResVertex, OutSgn);
}

//void _CPreFlatVertexContainer::CreateFlatVertex(_CNewSimplifierData& Data, _CFlatVertex*& pResVertex, short& OutSgn)
//{
//	static char* FN = "CacheTest.txt";
//	if (this == &OnePreFlatVertex)
//	{
//		pResVertex = &OneFlatVertex;
//		OutSgn = 1;
//		return;
//	}
//	_CPreFlat2FlatCache& PreFlat2FlatCache = Data.PF2FCache;
//	_CPreFlat2FlatCache::OUT_PAIR Out_pair = PreFlat2FlatCache.GetExisted(this);
//	if (Out_pair.first != nullptr)
//	{
//		pResVertex = Out_pair.first;
//		OutSgn = Out_pair.second;
//		//fstream file(FN,ios::out | ios::app);
//		//file<<"Read from cache _CPreFlatVertexContainer="<<this<<"\t pResVertex = "<<pResVertex<<"\t OutSgn = "<<OutSgn<<endl;
//		return;
//	}
//	if (m_Curr1Desc == nullptr)
//	{
//		//ASSERTPDD(m_Curr0Desc!=NULL);
//		if (m_Curr0Desc != NULL)
//		{
//			m_Curr0Desc->CreateFlatVertex(Data, pResVertex, OutSgn);
//			OutSgn *= m_0Sgn;
//		}
//		else
//		{
//			pResVertex = NULL;
//			OutSgn = 0;
//		}
//	}
//	else
//	{
//		_CFlatVertexCache& Cache = Data.GetFlatVertexCache();
//		_CFlatVertex* pDesc1 = NULL;
//		short Desc1Sgn = 1;//m_1Sgn;
//		_CFlatVertex* pDesc0 = NULL;
//		short Desc0Sgn = 1;//m_0Sgn;
//		m_Curr1Desc->CreateFlatVertex(Data, pDesc1, Desc1Sgn);
//		if (m_Curr0Desc == NULL)
//		{
//			Desc0Sgn = 0;
//			OutSgn = Desc1Sgn;
//			Desc1Sgn = 1;
//		}
//		else
//		{
//			m_Curr0Desc->CreateFlatVertex(Data, pDesc0, Desc0Sgn);
//		}
//		Desc1Sgn *= m_1Sgn;
//		Desc0Sgn *= m_0Sgn;
//		if (Desc1Sgn<0)
//		{
//			OutSgn = -OutSgn;
//			Desc1Sgn = -Desc1Sgn;
//			Desc0Sgn = -Desc0Sgn;
//		}
//		//if(Desc1Sgn!=0 && Desc0Sgn!=0)
//		if (pDesc1 == NULL)
//		{
//			OutSgn = Desc0Sgn;
//			pResVertex = pDesc0;
//		}
//		else
//		{
//			if (Desc1Sgn != 0 || Desc0Sgn != 0)
//				while ((Desc1Sgn & 1) == 0 && (Desc0Sgn & 1) == 0)
//				{
//					Desc1Sgn >>= 1;
//					Desc0Sgn >>= 1;
//					OutSgn <<= 1;
//				}
//			_CFlatVertex* pNewVertex = new _CFlatVertex(pDesc1, pDesc0, Desc1Sgn, Desc0Sgn,
//				Data.GetMainCircuit()->GetUnique(COMPONENTS_PATH(GetVerticesPath())));
//			pResVertex = Cache.PushInCache(pNewVertex, true);
//		}
//	}
//	PreFlat2FlatCache.Register(this, pResVertex, OutSgn);
//	//fstream file(FN,ios::out | ios::app);
//	//file<<"Cached _CPreFlatVertexContainer="<<this<<"\t pResVertex = "<<pResVertex<<"\t OutSgn = "<<OutSgn<<endl;
//}

void _CSimplificationOrdinateur::MoveTo(_CContextSExpFlatVertices& OutContextSExpFlatVertices,const string* Context)
{
	for(MAP::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
	{
		_CFactorizedVertexContainer CONT(it->second.m_FinalOutSign/**it->second.m_TopSgn*/,
			it->second.m_FinalFlatVertex,OutContextSExpFlatVertices.GetMainCircuit());
		OutContextSExpFlatVertices.
		GetSExpFlatResVertices(it->first.first-1,Context).
		putFlatCont(it->first.second,&CONT);
	}
}

bool/*IsNew*/ _CNewSimplifierData::RegisterEntry(unsigned long CofId,unsigned long Power,_CModelVertex* pEntryModelVertex,NumericType ExactValue, NumericType Tolerance,short TopSgn)
{
	ASSERTPDD(Tolerance>=0.0 && Tolerance<=1.0);
	if(pEntryModelVertex->Is0())
		return true;
	//if(IsZero(ExactValue))
	//	return true;
	NumericType V1=ExactValue*(NumericType(1.0)-Tolerance);
	NumericType V2=ExactValue*(NumericType(1.0)+Tolerance);
	if(SimplificationOrdinateur.RegisterEntry(CofId,Power,pEntryModelVertex,V1,V2,TopSgn))
	{
		m_MaxPower=max(m_MaxPower,Power);
		return true;
	}
	else return false;
}

void _CNewSimplifierData::NotifyParentsToBeUpdated(_CPreFlatVertexContainer* pDescVertex)
{
	pair<PARENT_MAP::iterator,PARENT_MAP::iterator> Range=m_ParentMap.equal_range(pDescVertex);
	for(PARENT_MAP::iterator it=Range.first;it!=Range.second;it++)
	{
		_CPreFlatVertexContainer* pParent=it->second;
		if(pParent->NotifyToBeUpdated())
			NotifyParentsToBeUpdated(pParent);

	}
}

bool _CNewSimplifierData::RegisterParent(_CPreFlatVertexContainer* pChildVertex,_CPreFlatVertexContainer* pParentVertex)
{
	if(pParentVertex==&OnePreFlatVertex)
		return false;
	pair<PARENT_MAP::iterator,PARENT_MAP::iterator> Range=m_ParentMap.equal_range(pChildVertex);
	for(PARENT_MAP::iterator it=Range.first;it!=Range.second;it++)
	{
		if(it->second==pParentVertex)
			return false;
	}
	m_ParentMap.insert(PARENT_MAP::value_type(pChildVertex,pParentVertex));
	return true;
}

void _CNewSimplifierData::RegisterParent(_CPreFlatVertexContainer* pParentVertex)
{
	for (_CPreFlatVertexContainer* pDesc : pParentVertex->m_CurrDescs)
		if (pDesc != nullptr)
		{
			RegisterParent(pDesc, pParentVertex);
		}
	//if(pParentVertex->m_Curr1Desc!=NULL)
	//	RegisterParent(pParentVertex->m_Curr1Desc,pParentVertex);
	//if(pParentVertex->m_Curr0Desc!=NULL)
	//	RegisterParent(pParentVertex->m_Curr0Desc,pParentVertex);
}

void _CNewSimplifierData::RegisterParentAndUpdateValues(_CPreFlatVertexContainer* pParentVertex,_CPreFlatMap::TRACE_RESULTS Res)
{
	if(Res==_CPreFlatMap::STILL_OLD && IsNewOneBack())
		SetFirstOldBack();
	if(Res==_CPreFlatMap::NEW_ONE || IsFirstOldBack())
	{
		RegisterParent(pParentVertex);
		//if(pParentVertex->m_Curr1Desc!=NULL)
		//	RegisterParent(pParentVertex->m_Curr1Desc,pParentVertex);
		//if(pParentVertex->m_Curr0Desc!=NULL)
		//	RegisterParent(pParentVertex->m_Curr0Desc,pParentVertex);
		if(IsFirstOldBack())
			SetJustBack();
	}
}

void _CNewSimplifierData::UpdateParentsValue(_CPreFlatVertexContainer* pChildVertexToBeChange)
{
	NumericType& Value=pChildVertexToBeChange->m_CurrentTopValue;
}

_CPreFlatVertexContainer& _CPreFlatVertexContainer::operator=(const _CPreFlatVertexContainer Source)
{
	m_ParentList = Source.m_ParentList;
	m_CurrDescs = Source.m_CurrDescs;
	//m_Curr0Desc=Source.m_Curr0Desc;
	//m_Curr1Desc=Source.m_Curr1Desc;
	m_CurrentDescValues = Source.m_CurrentDescValues;
	//m_0CurrentValue=Source.m_0CurrentValue;
	//m_1CurrentValue=Source.m_1CurrentValue;
	m_CurrentTopValue=Source.m_CurrentTopValue;
	m_Sgns = Source.m_Sgns;
	//m_1Sgn=Source.m_1Sgn;
	//m_0Sgn=Source.m_0Sgn;
	m_NeedToBeUpdated=m_NeedToBeUpdated;
	return *this;
}

MVerticesList& _CPreFlatVertexContainer::GetVerticesPath()
{
	return m_ParentList.GetVerticesPath();
}

const MVerticesList& _CPreFlatVertexContainer::VerticesPath() 
{
	return m_ParentList.VerticesPath(); 
}

//NumericType _CPreFlatVertexContainer::UpdateValue(_CPreFlatVertexContainer* pChild,NumericType Increase)
//{
//	if(pChild==m_Curr0Desc)
//	{
//	}
//	else if(pChild==m_Curr1Desc)
//	{
//
//	}
//	else
//		RISEPDD(eWrongCondition,"It is not my child");
//	return Increase;
//}

bool _CPreFlatVertexContainer::IsNew() const
{ 
	for (auto pDesc : m_CurrDescs)
		if (pDesc != nullptr)
			return false;
	for (auto Sgn : m_Sgns)
		if (Sgn != 1)
			return false;
	return true;
}

bool _CPreFlatVertexContainer::IsEmpty() const
{ 
	for (auto Sgn : m_Sgns)
		if (Sgn != 0)
			return false;
	return true; 
}

bool _CPreFlatVertexContainer::IsTerminal()
{
	if (this == &OnePreFlatVertex)
		return true;
	if (!IsHollow(m_CurrDescs))
		return false;
	if (m_CurrDescs[0] == nullptr)
		return false;
	return m_CurrDescs[0]->IsTerminal();
	//if (m_Curr1Desc != NULL)
	//	return false;
	//if (m_Curr0Desc != NULL)
	//	return m_Curr0Desc->IsTerminal();
	RISEPDD(eWrongCondition, "Ooops!");
}

//bool _CPreFlatVertexContainer::IsTerminal()
//{
//	if (this == &OnePreFlatVertex)
//		return true;
//	if (m_Curr1Desc != NULL)
//		return false;
//	if (m_Curr0Desc != NULL)
//		return m_Curr0Desc->IsTerminal();
//	RISEPDD(eWrongCondition, "Ooops!");
//}

_CPreFlatVertexContainer& _CPreFlatVertexList::at(size_t Power,const _CModelVerticesPath& Path,_CCompRedefEntry& RedefEntry)
{
	MAP::iterator it=m_Map.find(Power);
	if(it!=m_Map.end())
		return it->second;
	return m_Map.insert(MAP::value_type(Power, _CPreFlatVertexContainer(*this, Path.GetNumericValue(RedefEntry),Path.GetTopVertex()->NoOfDescendants()))).first->second;
}

//_CPreFlatVertexContainer& _CPreFlatVertexList::at(TOP_COORDINATERS& TopCoordinaters, const _CModelVerticesPath& Path, _CCompRedefEntry& RedefEntry)
//{
//	unsigned long long UC=Coordinates2ULL(TopCoordinaters);
//	MAP2::iterator it=m_Map2.find(UC);
//	if(it!=m_Map2.end())
//		return it->second;
//	return m_Map2.insert(MAP2::value_type(UC, _CPreFlatVertexContainer(*this, Path.GetNumericValue(RedefEntry)))).first->second;
//}

_CNewSimplifierData& _CNewSimplifierData::SetCordinates(const _CSimplificationOrdinateur::COORDINATES& Cordinates)
{
	m_TopCoordinates=Cordinates;
	unsigned long long ULLC=Coordinates2ULL(Cordinates);
	PRE_FLAT_MAP::iterator it=m_PreFlatMap.find(ULLC);
	if(it!=m_PreFlatMap.end())
		m_pPreFlatMap=&(it->second);
	else
		m_pPreFlatMap=&(m_PreFlatMap.insert(PRE_FLAT_MAP::value_type(ULLC,_CPreFlatMap(m_pMainCircuit))).first->second);
	return *this;
}
