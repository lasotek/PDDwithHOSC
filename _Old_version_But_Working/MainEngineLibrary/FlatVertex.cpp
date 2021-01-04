#include "StdAfx.h"
#include "FlatVertex.h"
#include "MainCircuit.h" 
#include "Vertex.h"
#include "Components.h"
#include "PathTraitor.h"
#include "CompRedefiner.h"

static SIGNED_COMPONENT_PATH NULL_PATH(nullptr,0,1);

//bool IsOnlyOneDescendant(const DESCENDANTS& Descs, size_t& NonZeroDesc)
//{
//	size_t Index = 0;
//	for (; Index < Descs.size(); Index++)
//		if (Descs[Index] != nullptr)
//			break;
//	for (size_t NextIndex = Index + 1; NextIndex < Descs.size(); NextIndex++)
//		if (Descs[NextIndex] != nullptr)
//			return false;
//	NonZeroDesc = Index;
//	return true;
//}
//
//bool IsHollow(const DESCENDANTS& Descs)
//{
//	size_t Pos = 0;
//	bool OnlyOne = IsOnlyOneDescendant(Descs, Pos);
//	return OnlyOne && Pos == 0;
//}

bool EachHasBitZero(const MULTIPLIERS& Mults,short Shift)
{
	bool CanBeShifted = false;
	for (short Mult : Mults)
		if (Mult != 0)
			if ((Mult & Shift) == 0)
				CanBeShifted = true;
			else
				return false;
	return CanBeShifted;
}

short ExtractCommonFactor(MULTIPLIERS& Mults, bool HighestPositive = false)
{
	list<short> Sgns(Mults.begin(), Mults.end());
	short nwd = NWD(Sgns,!HighestPositive);
	if (nwd != 1 && nwd != 0)
		for (auto& Mult : Mults)
			Mult /= nwd;
	return nwd;

	//short Sgn = 1, Shift = 0;
	//bool ExtractMinus = (HighestPositive && Mults[Mults.size() - 1] < 0) ||
	//	(!HighestPositive && Mults[0] < 0);
	//while (EachHasBitZero(Mults, (short)1<<Shift))
	//	Shift++;
	//if (Shift > 0 || ExtractMinus)
	//{
	//	Sgn <<= Shift;
	//	if (ExtractMinus) Sgn *= -1;
	//	for (short& Mult : Mults)
	//	{
	//		Mult >>= Shift;
	//		if (ExtractMinus)
	//			Mult *= -1;
	//	}
	//}
	//return Sgn;
}
bool HasCommonDescSgn(const MULTIPLIERS& Mults, short& CommonMult)
{
	if (Mults.empty())
		return false;
	CommonMult = Mults.front();
	if (CommonMult == 1)
		return false;
	for (short Mult : Mults)
		if (Mult != CommonMult)
			return false;
	return true;
}

MULTIPLIERS& operator/=(MULTIPLIERS& Mults, short Divider)
{
	for (short& Mult : Mults)
		Mult /= Divider;
	return Mults;
}

_CFlatVertex::_CFlatVertex(size_t NoOfDesc) : m_pComponentsPath(nullptr), 
m_Descendants(NoOfDesc, nullptr), m_Multipliers(NoOfDesc,1)
{
}

_CFlatVertex::_CFlatVertex(const DESCENDANTS& Descendants, const MULTIPLIERS& Multipliers, 
	const COMPONENTS_PATH& ComponentsPath) : m_Descendants(Descendants), m_Multipliers(Multipliers),
	m_pComponentsPath(&ComponentsPath)
{
}

//_CFlatVertex::_CFlatVertex(const _CFlatVertex* Descendent1,
//						   const _CFlatVertex* Descendent0,
//						   short Multiplier1,
//						   short Multiplier0,
//						   const COMPONENTS_PATH& ComponentsPath):
//							m_1Descendent(Descendent1),
//							m_0Descendent(Descendent0),
//							m_1Multiplier(Multiplier1),
//							m_0Multiplier(Multiplier0),
//							m_pComponentsPath(&ComponentsPath)
//{
//}

_CFlatVertex::_CFlatVertex(const _CFlatVertex& Source):
m_Descendants(Source.m_Descendants), m_Multipliers(Source.m_Multipliers), m_pComponentsPath(Source.m_pComponentsPath)
{
}

string _CFlatVertex::ToString(const _CFlatVertexCache& MyCache) const
{
	if(this==&OneFlatVertex)
		return "";
	long long Me=pDesc2Index(this,MyCache);
	ASSERTPDD(Me>=0);
	string Res="#";
	Res+=long2str(Me);
	Res+=": ";
	Res+=m_pComponentsPath->ToString();
	Res+='[';
	bool First = true;
	ASSERTPDD(m_Descendants.size() == m_Multipliers.size());

	for (size_t i = m_Descendants.size(); i > 0; i--)
	{
		long long Desc = pDesc2Index(m_Descendants[i-1], MyCache);
		ASSERTPDD(Desc >= 0);
		short Mult = m_Multipliers[i - 1];
		if (First)
			First = false;
		else
			Res += ", ";
		if (Mult != 1)
		{
			Res += '(';
			Res += short2str(Mult);
			Res += ")*";
		}
		Res += '#';
		Res += long2str(Desc);
	}
	Res+="]";
	return Res;
}
void _CFlatVertex::ToStream(txt_stream& stream, const _CFlatVertexCache& MyCache, size_t& NoOfMults, size_t& NoOfAdds, size_t& NoOfMultsNonOne) const
{
	if (this == &OneFlatVertex)
		return;
	long long Me = pDesc2Index(this, MyCache);
	ASSERTPDD(Me >= 0);
	stream << "#";
	stream << Me;
	stream << ": ";
	stream << m_pComponentsPath->ToString();
	stream << '[';
	bool First = true;
	ASSERTPDD(m_Descendants.size() == m_Multipliers.size());

	for (size_t i = m_Descendants.size(); i > 0; i--)
	{
		long long Desc = pDesc2Index(m_Descendants[i - 1], MyCache);
		ASSERTPDD(Desc >= 0);
		short Mult = m_Multipliers[i - 1];
		if (First)
			First = false;
		else
			stream << ", ";
		if (Desc == 0)
			stream << "#0";
		else
		{
			if (Mult != 1)
			{
				stream << '(';
				stream << Mult;
				stream << ")*";
			}
			stream << '#';
			stream << Desc;
		}
	}
	stream << "]";
}

long long _CFlatVertex::pDesc2Index(const _CFlatVertex* pDesc,const _CFlatVertexCache& MyCache) const
{
	if(pDesc==NULL)
		return 0;
	if(pDesc==&OneFlatVertex)
		return 1;
	return MyCache.GetIndex(pDesc);
}

bool _CFlatVertex::operator==(const _CFlatVertex& Right) const
{
	return
		m_Descendants == Right.m_Descendants &&
		m_Multipliers == Right.m_Multipliers &&
		m_pComponentsPath == Right.m_pComponentsPath;
}

long _CFlatVertex::DetermineHashKey(long Core)
{
	if(m_pComponentsPath!=NULL)
		Core=(long)m_pComponentsPath->GetHash(Core);
	for (auto pDesc : m_Descendants)
	{
		Core *= HASH_FACTOR;
		Core ^= (long)pDesc;
	}
	for (auto Mult : m_Multipliers)
	{
		Core *= HASH_FACTOR;
		Core ^= Mult;
	}
	return (long)Core;
}

_CFlatVertex OneFlatVertex/*(NULL)*/;

_CFlatVertexWalker::_CFlatVertexWalker(txt_stream& stream):m_stream(stream)
{
	m_Sgns.push_back(1);
}

void _CFlatVertex::WalkFlatResult(_CFlatVertexWalker& Walker) const
{
	for (size_t i = 0; i < m_Descendants.size(); i++)
		Walker.ProcessNextVertex(m_Descendants[i], m_Multipliers[i], i > 0);
}

void _CFlatVertexWalker::ProcessNextVertex(const _CFlatVertex* pNextVertex, short LocalMultiplier, short CompPower)
{
	if(pNextVertex==NULL)
		return;
	if(pNextVertex==&OneFlatVertex)
		ListOutTheTerm(LocalMultiplier);
	else
	{
		if (CompPower>0)
			m_List.push_back(ENTRY(pNextVertex,CompPower));
		m_Sgns.push_back(LocalMultiplier*m_Sgns.back());
		pNextVertex->WalkFlatResult(*this);
		m_Sgns.pop_back();
		if (CompPower>0)
			m_List.pop_back();
	}
}

void _CFlatVertexWalker::ProcessNextVertex(const _CFlatVertex* pNextVertex,short LocalMultiplier)
{
	if(pNextVertex==NULL)
		return;
	if(pNextVertex==&OneFlatVertex)
		ListOutTheTerm(LocalMultiplier);
	else
	{
		m_Sgns.push_back(LocalMultiplier*m_Sgns.back());
		ProcessNextVertex(pNextVertex->m_Descendants[0], pNextVertex->m_Multipliers[0]);
		for (size_t i = 1; i < pNextVertex->m_Descendants.size(); i++)
		{
			m_List.push_back(ENTRY(pNextVertex,i));
			ProcessNextVertex(pNextVertex->m_Descendants[i], pNextVertex->m_Multipliers[i]);
			m_List.pop_back();
		}
		m_Sgns.pop_back();
	}
}

void _CFlatVertexWalker::ListOutTheTerm(short LocalMultiplier)
{
	short Sgn=LocalMultiplier*m_Sgns.back();
	if(Sgn==1)
		m_stream<<'+';
	else if(Sgn==-1)
		m_stream<<'-';
	else
		m_stream<<Sgn<<'*';
	if(m_List.empty())
	{
		m_stream<<"1";
	}
	else
	{
		bool first=true;
		for(MAIN_LIST::iterator it=m_List.begin();it!=m_List.end();it++)
		{
			if(first)
				first=false;
			else
				m_stream<<'*';
			m_stream<<it->first->CompPathStr();
			short Power = it->second;
			if (Power != 1)
				m_stream << "^(" << Power << ")";
		}
	}
	m_stream<<endl;
}

_CSExpFlatVertices::_CSExpFlatVertices(INIT_TYPE Type):m_IsEmpty(true),m_MaxPower(0)
{
	if(Type==EMPTY)
		return;
	m_IsEmpty=false;
	if(Type==ONE || Type==MINUS_ONE)
	{
		m_Map[0].pVertex=&OneFlatVertex;
		m_Map[0].Sgn=(Type==ONE?1:-1);
	}
}

_CSExpFlatVerticesFactorized::_CSExpFlatVerticesFactorized(_CSExpFlatVertices::INIT_TYPE Type):m_IsEmpty(true),m_MaxPower(0)
{
	if(Type==_CSExpFlatVertices::EMPTY)
		return;
	m_IsEmpty=false;
	if(Type==_CSExpFlatVertices::ONE || Type==_CSExpFlatVertices::MINUS_ONE)
	{
		//m_Map[0].m_List.push_back(&OneFlatVertex);
		//m_Map[0].Sgn=(Type==_CSExpFlatVertices::ONE?1:-1);
	}
}

_CSExpFlatVerticesFactorized::_CSExpFlatVerticesFactorized(const _CSExpFlatVertices& Source,_CMainCircuit* pMainCircuit)
:m_MaxPower(Source.m_MaxPower), m_IsEmpty(Source.m_IsEmpty),m_pMainCircuit(pMainCircuit)
{
	for(_CSExpFlatVertices::MAP_TYPE::const_iterator it=Source.m_Map.begin(),_e=Source.m_Map.end();
		it!=_e;it++)
		m_Map.insert(MAP_TYPE::value_type(it->first,_CFactorizedVertexContainer(it->second,m_pMainCircuit)));
}

_CSExpFlatVertices ZeroFlatVertices(_CSExpFlatVertices::ZERO);

_CSExpFlatVertices OneFlatVertices(_CSExpFlatVertices::ONE);
_CSExpFlatVertices MinusOneFlatVertices(_CSExpFlatVertices::MINUS_ONE);


//_CMultLevelSExpFlatVertices& _CContextSExpFlatVertices::GetSExpFlatResVertices(size_t CofId,const string* Context)
_CSExpFlatVerticesFactorized& _CContextSExpFlatVertices::GetSExpFlatResVertices(size_t CofId,const string* Context)
{
	VECTOR& Vector=m_Map[Context];
	_CSExpFlatVerticesFactorized& Res=Vector.force_at(CofId);
	Res.m_pMainCircuit=m_pMainCircuit;
	return Res;
}


_CSubCircuitSocket* _CModelVerticesPath::GetParentSocket() const
{
	LIST::const_reverse_iterator rit=m_List.rbegin();
	if(rit==m_List.rend())
		return NULL;
	const _CModelVertex* pVertex=*rit;
	_CComponent* pComponent=pVertex->GetPComponent();
	if(pComponent->IsMacroDependant())
		return dynamic_cast<_CSubCircuitSocket*>(pComponent);
	rit++;
	if(rit==m_List.rend())
		return NULL;
	pVertex=*rit;
	pComponent=pVertex->GetPComponent();
	if(pComponent->IsMacroDependant())
		return dynamic_cast<_CSubCircuitSocket*>(pComponent);
	RISEPDD(eWrongCondition,"Only the last token in the ring can be simply component");
}

const _CModelVertex* _CModelVerticesPath::GetParentMetaVertex() const
{
	LIST::const_reverse_iterator rit=m_List.rbegin();
	if(rit==m_List.rend())
		return NULL;
	const _CModelVertex* pVertex=*rit;
	_CComponent* pComponent=pVertex->GetPComponent();
	if(pComponent->IsMacroDependant())
		return pVertex;
	rit++;
	if(rit==m_List.rend())
		return NULL;
	pVertex=*rit;
	pComponent=pVertex->GetPComponent();
	if(pComponent->IsMacroDependant())
		return pVertex;
	RISEPDD(eWrongCondition,"Only the last token in the ring can be simply component");
}

//const NumericType& _CModelVerticesPath::GetNumericValue(_CCompRedefEntry& Entry) const;
const NumericType& _CModelVerticesPath::GetNumericValue(_CCompRedefEntry& Entry) const
{
	const _CModelVertex* pTopVertex=m_List.back();
	_CComponent* pComponent=pTopVertex->GetPComponent();
	if(pComponent->IsMacroDependant())
		RISEPDD(eWrongCondition,"Top ordinary component expected");
	_CSubCircuitSocket* pParentSocket=GetParentSocket();	
	return Entry.GetcompNumericValue(pComponent);
	//return pParentSocket==NULL?pComponent->GetParameterValue():pParentSocket->GetParameterCurrentValue(pComponent);
}

const _CSubModelNumericPattern* _CModelVerticesPath::GetTopSubModelNumericPatter() const
{
	//const _CModelVertex* pMetaVertex=GetParentMetaVertex();
	//if(pMetaVertex==NULL)
	//	//return &OnePattern;
	//	return NULL;
	//NUM_PATT_MAP::const_iterator it=m_Map.find(pMetaVertex);
	//ASSERTPDD(it!=m_Map.end());
	//return it->second;
	return m_NumPatList.empty()?NULL:m_NumPatList.back();
}

void _CModelVerticesPath::Step2NextVertex(const _CModelVertex* pNextVertex,_CSubModelNumericPattern* pNumPatt)
{
	//m_List.push_back(pNextVertex);//poprawiæ
	//if(pNumPatt!=NULL && pNextVertex->IsMacroDependent())
	//	m_Map.insert(NUM_PATT_MAP::value_type(pNextVertex,pNumPatt));
	m_List.push_back(pNextVertex);//poprawiæ
	if(/*pNumPatt!=NULL &&*/ pNextVertex->IsMacroDependent())
		m_NumPatList.push_back(pNumPatt);
}
void _CModelVerticesPath::StepOutOneVertex()
{
	const _CModelVertex* pVertex=m_List.back();
	//if(pVertex->IsMacroDependent())
	//	m_Map.erase(pVertex);
	if(pVertex->IsMacroDependent())
		m_NumPatList.pop_back();
	m_List.pop_back();
}

void _CModelVerticesPath::LeaveMetaVertex2(LIST& StoredHeep,const _CSubModelNumericPattern*& pTopNumPatt)
{
	//StoredHeep.clear();//poprawiæ
	//ASSERTPDD(!m_List.empty());
	//const _CModelVertex* pVertex=m_List.back();
	//ASSERTPDD( pVertex->IsMacroDependent());
	//StoredHeep.push_front(pVertex);
	//m_List.pop_back();
	//NUM_PATT_MAP::iterator it=m_Map.find(pVertex);
	//pTopNumPatt=(it==m_Map.end())?&OnePattern:it->second;
	StoredHeep.clear();//poprawiæ
	ASSERTPDD(!m_List.empty());
	const _CModelVertex* pVertex=m_List.back();
	ASSERTPDD( pVertex->IsMacroDependent());
	StoredHeep.push_front(pVertex);
	m_List.pop_back();
	pTopNumPatt=m_NumPatList.back();
	m_NumPatList.pop_back();
}
void _CModelVerticesPath::LeaveMetaVertex(const _CModelVertex*& pTopMacro,const _CSubModelNumericPattern*& pTopNumPatt)
{
	ASSERTPDD(!m_List.empty());
	pTopMacro=m_List.back();
	ASSERTPDD( pTopMacro->IsMacroDependent());
	m_List.pop_back();
	pTopNumPatt=m_NumPatList.back();
	m_NumPatList.pop_back();
}

void _CModelVerticesPath::Return2MetaVertex(const _CModelVertex* pTopMacro,const _CSubModelNumericPattern* pTopNumPatt)
{
	ASSERTPDD(pTopMacro->IsMacroDependent());
	m_List.push_back(pTopMacro);
	m_NumPatList.push_back(pTopNumPatt);
	//m_List.insert(m_List.end(),StoredHeep.begin(),StoredHeep.end());
}

//void _CModelVerticesPath::LeaveMetaVertex(LIST& StoredHeep,const _CSubModelNumericPattern*& pTopNumPatt)
//{
//	StoredHeep.clear();//poprawiæ
//	const _CModelVertex* pVertex=m_List.back();
//	StoredHeep.push_front(pVertex);
//	m_List.pop_back();
//	while(!pVertex->IsMacroDependent())
//	{
//		pVertex=m_List.back();
//		StoredHeep.push_front(pVertex);
//		m_List.pop_back();
//	}
//	pVertex=GetParentMetaVertex();
//	if(pVertex==NULL)
//		pTopNumPatt=NULL;
//	else
//	{
//		NUM_PATT_MAP::iterator it=m_Map.find(pVertex);
//		pTopNumPatt=(it==m_Map.end())?&OnePattern:it->second;
//	}
//}

void _CModelVerticesPath::Return2MetaVertex(LIST& StoredHeep)
{
	//poprawiæ
	m_List.insert(m_List.end(),StoredHeep.begin(),StoredHeep.end());
}

//void _CModelVerticesTraitor::Step2NextVertex(const _CModelVertex* pNextVertex)
//{
//	ASSERTPDD(pNextVertex!=NULL);
//	if(m_List.empty() || m_List.back().back()->IsMacroDependent())
//		m_List.push_back(IN_LIST());
//	m_List.back().push_back(pNextVertex);
//	//if(pNextVertex->IsMacroDependent())
//	//	m_List.push_back(IN_LIST());
//}
//
//void _CModelVerticesTraitor::StepOutOneVertex()
//{
//	m_List.back().pop_back();
//	if(m_List.back().empty())
//		m_List.pop_back();
//}
//
//void _CModelVerticesTraitor::GetVerticesPath(VERTICES_PATH& Path) const
//{
//	for(GLOBAL_LIST::const_iterator it=m_List.begin();it!=m_List.end();it++)
//	{
//		const IN_LIST& InList=*it;
//		if(!InList.empty())
//			Path.push_back(InList.back());
//	}
//}
//
//_CSubCircuitSocket* _CModelVerticesTraitor::GetParentSocket() const
//{
//	GLOBAL_LIST::const_reverse_iterator rit=m_List.rbegin();
//	if(rit==m_List.rend())
//		return NULL;
//	rit++;
//	if(rit==m_List.rend())
//		return NULL;
//	const _CModelVertex* pVertex=(*rit).back();
//	_CComponent* pComponent=pVertex->GetPComponent();
//	if(pComponent->IsMacroDependant())
//		return dynamic_cast<_CSubCircuitSocket*>(pComponent);
//	RISEPDD(eWrongCondition,"Only the last token in the ring can be simply component");
//	//GLOBAL_LIST::const_iterator it=(--m_List.end());
//	//if(it==m_List.begin())
//	//	return NULL;
//	//const _CModelVertex* pVertex=(*it).back();
//	//_CComponent* pComponent=pVertex->GetPComponent();
//	//ASSERTPDD(pComponent->IsMacroDependant());
//	//return dynamic_cast<_CSubCircuitSocket*>(pComponent);
//}
//
//VERTICES_PATH::VERTICES_PATH(const _CModelVerticesTraitor& Traitor)
//{
//	Traitor.GetVerticesPath(*this);
//}
//
//bool VERTICES_PATH::HasBasicComponent() const
//{
//	ASSERTPDD(!empty());
//	return !(back()->IsMacroDependent());
//}
//
long long _CModelVerticesPath::GetHash(long long Core) const
{
	long long Res=Core;
	for(LIST::const_iterator it=m_List.begin();it!=m_List.end();it++)
	{
		Res*=HASH_FACTOR;
		Res^=(long long)(*it);
	}
	return Res;
}

//void COMPONENTS_PATH::Copy(const VERTICES_PATH& Source)
void COMPONENTS_PATH::Copy(const _CModelVerticesPath& Source)
{
	clear();
	for(_CModelVerticesPath::LIST::const_iterator it=Source.m_List.begin();
		it!=Source.m_List.end();it++)
		push_back((*it)->GetPComponent());
}

COMPONENTS_PATH::COMPONENTS_PATH(const _CModelVerticesPath& Source)
{
	Copy(Source);
}
COMPONENTS_PATH::COMPONENTS_PATH(const _CModelVerticesPath::LIST& TraitorLIST)
{
	clear();
	for(_CModelVerticesPath::LIST::const_iterator it=TraitorLIST.begin();
		it!=TraitorLIST.end();it++)
		push_back((*it)->GetPComponent());
}
//COMPONENTS_PATH::COMPONENTS_PATH(const _CModelVerticesTraitor& Traitor)
//{
//	VERTICES_PATH Tmp(Traitor);
//	Copy(Tmp);
//}

string COMPONENTS_PATH::ToString() const
{
	string Res;
	bool IsNext=false;
	for(const_iterator it=begin();it!=end();it++)
	{
		const string& PNStr=(*it)->GetParameterName();
		if(PNStr.empty())
			continue;
		if(IsNext)
			Res+='.';
		else
			IsNext=true;
		Res+=PNStr;
	}
	return Res;
}

long long  COMPONENTS_PATH::GetHash(long long Core) const
{
	//long long Core=HASH_CORE;
	return Hash_value(Core,ToString());
}

const NumericType& COMPONENTS_PATH::GetNumericValue(_CCompRedefEntry& RedefEntry) const
{
	const_reverse_iterator it=rbegin();
	if(it!=rend())
	{
		const _CComponent* pTopComponent=*it;
		if(!pTopComponent->IsMacroDependant())
		{
			it++;
			if(it==rend())
				return pTopComponent->GetParameterValue();
			const _CComponent* pCSocket=*it;
			if(pCSocket->IsMacroDependant())
			{
				const _CSubCircuitSocket* pSocket=
					dynamic_cast<const _CSubCircuitSocket*>(pCSocket);
				//return pTopComponent->GetParameterValueInSocket(pSocket);
				return RedefEntry.GetcompNumericValue(pTopComponent);
			}
		}
	}
	RISEPDD(eWrongCondition,"Wrong COMPONENTS_PATH format");
}

const NumericType& COMPONENTS_PATH::GetNumericValue() const
{
	if (!empty())
	{
		const _CComponent* pComp = back();
		if (!pComp->IsMacroDependant())
			return pComp->GetParameterValue();
	}
	RISEPDD(eWrongCondition, "Simply componentnt expected");
}

//_CFlatVerticesResContainer::_CFlatVerticesResContainer(_CModelVerticesTraitor& Traitor,/*size_t Level,*/const string* Context):
//							m_Context(Context)/*,m_Level(Level)*/
//{
//	Traitor.GetVerticesPath(m_Path);
//}


//bool _CFlatVerticesResContainer::IsEqualTo(const _CAbstractOperationDataContainer<_CMultLevelSExpFlatVertices>& Right)
//{
//	const _CFlatVerticesResContainer& RRight=*(dynamic_cast<const _CFlatVerticesResContainer*>(&Right));
//	return RRight.m_Context==m_Context && RRight.m_Path==m_Path;
//}

bool _CFlatVerticesResContainer2::IsEqualTo(const _CAbstractOperationDataContainer<const _CFlatVertex*>& Right)
{
	const _CFlatVerticesResContainer2& RRight=*(dynamic_cast<const _CFlatVerticesResContainer2*>(&Right));
	return RRight.m_Context==m_Context && RRight.m_Path==m_Path && RRight.m_Power==m_Power && m_Inaccuracy<=RRight.m_Inaccuracy;
}

long _CFlatVerticesResContainer2::DetermineHashKey(long Core)
{
	long long HC=Core;
	Hash_value(HC,*m_Context);
	HC*=HASH_FACTOR;
	HC^=m_Power;
	return (long)m_Path.GetHash(HC);
}

bool _CFlatVerticesResContainer::IsEqualTo(const _CAbstractOperationDataContainer<_CSExpFlatVertices>& Right)
{
	const _CFlatVerticesResContainer& RRight=*(dynamic_cast<const _CFlatVerticesResContainer*>(&Right));
	return /*RRight.m_Level==m_Level &&*/ RRight.m_Context==m_Context && RRight.m_Path==m_Path && RRight.m_Limiter==m_Limiter;
}

long _CFlatVerticesResContainer::DetermineHashKey(long Core)
{
	long long HC=Core;
	//HC*=HASH_FACTOR;
	//HC^=m_Level;
	return (long)m_Path.GetHash(m_Limiter.GetHash(Hash_value(HC,*m_Context)));
}

void _CFlatVerticesResContainer::ReleaseResult()
{
	//m_UsageCounter--;
	if(--m_UsageCounter==0)
		CachItToHD();
}

_CFlatVerticesResCache::_CFlatVerticesResCache()
{
	char Buf[30];
	sprintf_s(Buf,30,"%p",this);
	string s=string("_CFlatVerticesResCache")+Buf+".tmp";
	m_PathTempFile=_PATH_STR(s);
}

_CFlatVerticesResContainer* _CFlatVerticesResCache::FindOrRegisterResult(_CFlatVerticesResContainer* pContainer)
{
	if(!m_tmp_file.IsOpen())
		m_tmp_file.open(_binary_filer::o_temporary,m_PathTempFile);
	pContainer->m_p_tmp_file=&m_tmp_file;
	//_CFlatVerticesResContainer* pRes= dynamic_cast<_CFlatVerticesResContainer*> (_CAbstractOperationCache<_CMultLevelSExpFlatVertices>::FindOrRegisterResult(pContainer));
	_CFlatVerticesResContainer* pRes= dynamic_cast<_CFlatVerticesResContainer*> (_CAbstractOperationCache<_CSExpFlatVertices>::FindOrRegisterResult(pContainer));
	if(pRes==pContainer && pContainer->m_UsageCounter==0)
		pRes->CachItToHD();
	return pRes;
}
void _CFlatVerticesResCache::RegisterResult(_CFlatVerticesResContainer* pContainer)
{
	if(!m_tmp_file.IsOpen())
		m_tmp_file.open(_binary_filer::o_temporary,m_PathTempFile);
	pContainer->m_p_tmp_file=&m_tmp_file;
	//_CAbstractOperationCache<_CMultLevelSExpFlatVertices>::RegisterResult(pContainer);
	_CAbstractOperationCache<_CSExpFlatVertices>::RegisterResult(pContainer);
	if(pContainer->m_UsageCounter==0)
		pContainer->CachItToHD();
}

void _CFlatVerticesResCache::HibernateAll()
{
	for(INDEX2OBJECT_MAP_ITERATOR it=m_Index2ObjectMap.begin(),_e=m_Index2ObjectMap.end();it!=_e;it++)
	{
		dynamic_cast<_CFlatVerticesResContainer*>(it->second)->Hibernate();
	}
}

//_CFlatVerticesResCacheKiller::~_CFlatVerticesResCacheKiller()
//{
//	for(iterator it=begin(),_e=end();it!=_e;it++)
//	{
//		_CFlatVerticesResContainer* pContainer=*it;
//		if(pContainer!=NULL)
//			pContainer->ReleaseResult();
//	}
//}

void _CFlatVerticesSPowerLimiter::SetSPowerAllowed(size_t EntryNo, unsigned short sPower)
{
	m_Map[EntryNo].SetAllowed(sPower);
}

bool _CFlatVerticesSPowerLimiter::IsSPowerAllowed(size_t EntryNo, unsigned short sPower) const
{
	MAP_TYPE::const_iterator it=m_Map.find(EntryNo);
	if(it==m_Map.end()) return false;
	return it->second.IsAllowed(sPower);
}

bool _CFlatVerticesSPowerLimiter::IsEntryAllowed(size_t EntryNo) const
{
	MAP_TYPE::const_iterator it=m_Map.find(EntryNo);
	return (it!=m_Map.end());
}

void _CFlatVerticesSPowerLimiter::CancelSPowerAllowed(size_t EntryNo, unsigned short sPower)
{
	MAP_TYPE::iterator it=m_Map.find(EntryNo);
	if(it==m_Map.end()) return;
	_CSPowerLimiter& Limiter=it->second;
	Limiter.CancelAllowed(sPower);
	if(Limiter.IsEmpty())
		m_Map.erase(it);
}

void _CFlatVerticesSPowerLimiter::SetAllowedRange(size_t EntryNo, unsigned short sMinPower, unsigned short sMaxPower)
{
	//sMaxPower=min(sMaxPower,m_MaxPower);
	_CSPowerLimiter& Limiter=m_Map[EntryNo];
	for(unsigned short i=sMinPower;i<=sMaxPower;i++)
		Limiter.SetAllowed(i);
}

void _CFlatVerticesSPowerLimiter::CancelAllowedRange(size_t EntryNo, unsigned short sMinPower, unsigned short sMaxPower)
{
	//sMaxPower=min(sMaxPower,m_MaxPower);
	MAP_TYPE::iterator it=m_Map.find(EntryNo);
	if(it==m_Map.end()) return;
	_CSPowerLimiter& Limiter=it->second;
	for(unsigned short i=sMinPower;i<=sMaxPower;i++)
		Limiter.CancelAllowed(i);
	if(Limiter.IsEmpty())
		m_Map.erase(it);
}

void _CSPowerLimiter::InitGlobal(const _CSparsePolynomial* pNumResult, const _CApproxCriterion* pAppCriterion,size_t FullSize)
{
	m_Set.clear();
	if(pAppCriterion->IsExact())
	{
		for(size_t i=0;i<=FullSize;i++)
			SetAllowed(i);
		return;
	}
	//_CSparsePolynomial::EnergyDistribution DistL,DistH;
	_CSparsePolynomial::MagnitudeDistribution MagL,MagH;
	//pNumResult->GetEnergyDistribution(DistL,pAppCriterion->GetLowFrequency());
	//pNumResult->GetEnergyDistribution(DistH,pAppCriterion->GetHighFrequency());
	pNumResult->GetMagnitudeDistributionReIm(MagL,pAppCriterion->GetLowFrequency());
	pNumResult->GetMagnitudeDistributionReIm(MagH,pAppCriterion->GetHighFrequency());
	NumericType Accuracy=(pAppCriterion->GetAccurracy());
	//m_MaxPower=DistL.size();
	//ASSERTPDD(m_MaxPower==DistH.size());
	//for(_CSparsePolynomial::EnergyDistribution::size_type i=0;i<DistL.size();i++)
	for(_CSparsePolynomial::MagnitudeDistribution::size_type i=0;i<MagL.size();i++)
	{
		//if(DistL[i]<Accuracy && DistH[i]<Accuracy)
		//	continue;
		if(!pAppCriterion->IsExact() &&
			abs(MagL[i].real())<Accuracy && 
			abs(MagL[i].imag())<Accuracy && 
			abs(MagH[i].real())<Accuracy && 
			abs(MagH[i].imag())<Accuracy)
			continue;
		SetAllowed(i);
	}
}

_CFlatVerticesSPowerLimiter::_CFlatVerticesSPowerLimiter(const _CSparsePolynomial* pNumResult,
													   const _CApproxCriterion* pAppCriterion)/*:m_MaxPower(pNumResult->get_order())*/
{
	_CSparsePolynomial::EnergyDistribution DistL,DistH;
	pNumResult->GetEnergyDistribution(DistL,pAppCriterion->GetLowFrequency());
	pNumResult->GetEnergyDistribution(DistH,pAppCriterion->GetHighFrequency());
	NumericType Accuracy=(pAppCriterion->GetAccurracy());
	//m_MaxPower=DistL.size();
	//ASSERTPDD(m_MaxPower==DistH.size());
	for(_CSparsePolynomial::EnergyDistribution::size_type i=0;i<DistL.size();i++)
	{
		if(DistL[i]<Accuracy && DistH[i]<Accuracy)
			continue;
		m_Map[0].SetAllowed(i);
	}
}

void _CFlatVerticesSPowerLimiter::WorkOutNew(const _CFlatVerticesSPowerLimiter& TopLimiter,
											 const _CSubModelNumericPattern& TopPattern,
											 const _CSparsePolynomial& CompPattern,
											 const _CSubModelNumericPattern& DescPattern,
											 const _CApproxCriterion& AppCriterion)
{
	NumericType Accuracy=AppCriterion.GetAccurracy();
	for(_CSubModelNumericPattern::MAP_TYPE::const_iterator it=DescPattern.m_Map.begin();
		it!=DescPattern.m_Map.end();it++)
	{
		size_t Level=it->first;
		const _CSparsePolynomial& DescPoly=it->second; 
		for(_CSparsePolynomial::MAP_TYPE::const_iterator dp_it=DescPoly.m_Map.begin();
			dp_it!=DescPoly.m_Map.end();dp_it++)
		{
			size_t dp_power=dp_it->first;
			const NumericType& dp_value=dp_it->second;
			for(_CSparsePolynomial::MAP_TYPE::const_iterator cp_it=CompPattern.m_Map.begin();
				cp_it!=CompPattern.m_Map.end();cp_it++)
			{
				size_t cp_power=cp_it->first;
				const NumericType& cp_value=cp_it->second;
				size_t res_power=cp_power+dp_power;
				if(!TopLimiter.IsSPowerAllowed(Level,res_power))
					continue;
				const NumericType& res_value=TopPattern.get2(Level,res_power);
				if(abs(dp_value*cp_value/res_value)<Accuracy)
					continue;
				SetSPowerAllowed(Level,dp_power);
			}
		}
	}
}

_CSPowerLimiter ZeroSPowertLimiter;

const _CSPowerLimiter& _CFlatVerticesSPowerLimiter::GetSmartEntryLimiter(size_t EntryNo) const
{
	MAP_TYPE::const_iterator it=m_Map.find(EntryNo);
	if(it==m_Map.end())
		return ZeroSPowertLimiter;
	return it->second;
}

bool _CSPowerLimiter::IsAllowed(unsigned short sPower) const
{
	//pair<SET::const_iterator,SET::const_iterator> Range=m_Set.equal_range(sPower);
	return (m_Set.find(sPower)!=m_Set.end());
	//return (*m_Set.equal_range(sPower).first)==sPower;
}
long long _CSPowerLimiter::GetHash(long long Core) const
{
	long long Res=Core;
	for(SHORT_SET::const_iterator it=m_Set.begin();it!=m_Set.end();it++)
	{
		Res*=HASH_FACTOR;
		Res^=*it;
	}
	return Res;
}

void _CSPowerLimiter::WorkOutNew(const _CSPowerLimiter& TopLimiter,
		const _CSparsePolynomial& TopPattern,
		const _CSparsePolynomial& CompPattern,
		const _CSparsePolynomial& DescPattern,
		const _CApproxCriterion& AppCriterion)
{
	m_Set.clear();
	NumericType Accuracy=AppCriterion.GetAccurracy();
	for(_CSparsePolynomial::const_iterator dp_it=DescPattern.m_Map.begin();
		dp_it!=DescPattern.m_Map.end();
		dp_it++)
	{
			size_t dp_power=dp_it->first;
			const NumericType& dp_value=dp_it->second;
			for(_CSparsePolynomial::MAP_TYPE::const_iterator cp_it=CompPattern.m_Map.begin();
				cp_it!=CompPattern.m_Map.end();cp_it++)
			{
				size_t cp_power=cp_it->first;
				const NumericType& cp_value=cp_it->second;
				size_t res_power=cp_power+dp_power;
				if(!TopLimiter.IsAllowed(res_power))
					continue;
				const NumericType& res_value=TopPattern.get(res_power);
				if(abs(dp_value*cp_value/res_value)<Accuracy)
					continue;
				SetAllowed(dp_power);
			}
	}
}

bool _CFlatVerticesSPowerLimiter::IsEmpty() const
{
	for(MAP_TYPE::const_iterator it=m_Map.begin();it!=m_Map.end();it++)
		if(!it->second.IsEmpty())
			return false;
	return true;
}

_CMultLevelSExpFlatVertices ZeroMultLevelSExpFlatVertices(_CMultLevelSExpFlatVertices::ZERO);
_CMultLevelSExpFlatVertices OneMultLevelSExpFlatVertices(_CMultLevelSExpFlatVertices::ONE);

_CMultLevelSExpFlatVertices::_CMultLevelSExpFlatVertices(INIT_TYPE Type):
self_exp_vector<_BaseType> (),m_Type(Type) 
{
}

//_CSExpFlatVertices ZeroSExpFlatVertices(_CSExpFlatVertices::ZERO);//?
//_CSExpFlatVertices OneSExpFlatVertices(_CSExpFlatVertices::ONE);//?

_CSExpFlatVertices* _CMultLevelSExpFlatVertices::get_smart(size_t Index) const
{
	if(m_Type==ZERO || Index>=size())
		return &ZeroFlatVertices;
	if(m_Type==ONE)
		return &OneFlatVertices;
	_CSExpFlatVertices* Res=at(Index);
	return /*Res==NULL?&ZeroFlatVertices:*/Res;
}

const _CSExpFlatVertices::_CVertexContainer* _CSExpFlatVertices::getFlatCont(size_t Index) const
{
	if(m_IsEmpty)
		return NULL;
	MAP_TYPE::const_iterator it=m_Map.find(Index);
	if(it==m_Map.end())
		return NULL;
	return &(it->second);
}

/*const*/ _CFactorizedVertexContainer* _CSExpFlatVerticesFactorized::getFlatCont(size_t Index) /*const*/
{
	if(m_IsEmpty)
		return NULL;
	MAP_TYPE::/*const_*/iterator it=m_Map.find(Index);
	if(it==m_Map.end())
		return NULL;
	return &(it->second);
}


void _CSExpFlatVertices::putFlatCont(size_t Index,/*const*/ _CVertexContainer* pCont)
{
	if(pCont==NULL || pCont->pVertex==NULL)
	{
		MAP_TYPE::iterator it=m_Map.find(Index);
		if(it!=m_Map.end())
		{
			m_Map.erase(it);
			if(m_Map.empty())
				m_IsEmpty=true;
			if(Index==m_MaxPower)
			{
				m_MaxPower=0;
				for(MAP_TYPE::iterator it=m_Map.begin();it!=m_Map.end();it++)
					m_MaxPower=max(m_MaxPower,it->first);
			}
		}
	}
	else
	{
		m_IsEmpty=false;
		m_MaxPower=max(m_MaxPower,Index);
		m_Map[Index]=*pCont;
	}
}

void _CSExpFlatVerticesFactorized::putFlatCont(size_t Index,/*const*/ _CFactorizedVertexContainer* pCont)
{
	if(pCont==NULL || pCont->m_List.empty())
	{
		MAP_TYPE::iterator it=m_Map.find(Index);
		if(it!=m_Map.end())
		{
			m_Map.erase(it);
			if(m_Map.empty())
				m_IsEmpty=true;
			if(Index==m_MaxPower)
			{
				m_MaxPower=0;
				for(MAP_TYPE::iterator it=m_Map.begin();it!=m_Map.end();it++)
					m_MaxPower=max(m_MaxPower,it->first);
			}
		}
	}
	else
	{
		m_IsEmpty=false;
		m_MaxPower=max(m_MaxPower,Index);
		_CFactorizedVertexContainer& Entry=m_Map[Index];
		Entry=*pCont;
		Entry.m_pMainCircuit=m_pMainCircuit;
	}
}


//const _CFlatVertex* _CSExpFlatVertices::getFlatVertex(size_t Index) const
//{
//	if(m_IsEmpty)
//		return NULL;
//	MAP_TYPE::const_iterator it=m_Map.find(Index);
//	if(it==m_Map.end())
//		return NULL;
//	return it->second.pVertex;
//}
//void _CSExpFlatVertices::putFlatVertex(size_t Index,const _CFlatVertex* pVertex)
//{
//	if(pVertex==NULL)
//	{
//		MAP_TYPE::iterator it=m_Map.find(Index);
//		if(it!=m_Map.end())
//		{
//			m_Map.erase(it);
//			if(m_Map.empty())
//				m_IsEmpty=true;
//			if(Index==m_MaxPower)
//			{
//				m_MaxPower=0;
//				for(MAP_TYPE::iterator it=m_Map.begin();it!=m_Map.end();it++)
//					m_MaxPower=max(m_MaxPower,it->first);
//			}
//		}
//	}
//	else
//	{
//		m_IsEmpty=false;
//		m_MaxPower=max(m_MaxPower,Index);
//		m_Map[Index].pVertex=pVertex;
//	}
//}

string _CSExpFlatVertices::ToString(const _CFlatVertexCache& MyCache) const
{
	string Res;
	bool first=true;
	for(size_t i=m_MaxPower;i>0;i--)
	{
		MAP_TYPE::const_iterator it=m_Map.find(i);
		if(it==m_Map.end())
			continue;
		long long Id=MyCache.GetIndex(it->second.pVertex);
		if(Id<0)
			continue;//???
		if(first)
			first=false;
		else
			Res+=" + ";
		Res+="s^";
		Res+=ulong2str(i);
		Res+='*';
		short Sgn=it->second.Sgn;
		if(Sgn!=1)
		{
			Res+='(';
			Res+=long2str(Sgn);
			Res+=")*";
		}
		Res+="(#";
		Res+=long2str(Id);
		Res+=')';
	}
	MAP_TYPE::const_iterator it=m_Map.find(0);
	if(it!=m_Map.end())
	{
		long long Id=MyCache.GetIndex(it->second.pVertex);
		if(Id>=0)
		{
			if(first)
				first=false;
			else
				Res+=" + ";
			short Sgn=it->second.Sgn;
			if(Sgn!=1)
			{
				Res+='(';
				Res+=long2str(Sgn);
				Res+=")*";
			}
			Res+="(#";
			Res+=long2str(Id);
			Res+=')';
		}
	}
	return Res;
}
string _CSExpFlatVerticesFactorized::ToString(const _CFlatVertexCache& MyCache) const
{
	string Res;
	bool first=true;
	for(size_t i=m_MaxPower;i>0;i--)
	{
		MAP_TYPE::const_iterator it=m_Map.find(i);
		if(it==m_Map.end())
			continue;
		if(first)
			first=false;
		else
			Res+=" + ";
		Res+="s^";
		Res+=ulong2str(i);
		Res+='*';
		short Sgn=it->second.Sgn;
		if(Sgn!=1)
		{
			Res+='(';
			Res+=long2str(Sgn);
			Res+=")*";
		}
		const _CFactorizedVertexContainer::FACTORED_LIST& m_List=it->second.m_List;
		bool first2=true;
		for(_CFactorizedVertexContainer::FACTORED_LIST::const_iterator fit=m_List.begin(),_fe=m_List.end();
			fit!=_fe;fit++)
		{
			if(first2)
				first2=false;
			else
				Res+='*';
			long long Id=MyCache.GetIndex(*fit);
			Res+="(#";
			Res+=long2str(Id);
			Res+=')';
		}
	}
	MAP_TYPE::const_iterator it=m_Map.find(0);
	if(it!=m_Map.end())
	{
		if(first)
			first=false;
		else
			Res+=" + ";
		short Sgn=it->second.Sgn;
		if(Sgn!=1)
		{
			Res+='(';
			Res+=long2str(Sgn);
			Res+=")*";
		}
		const _CFactorizedVertexContainer::FACTORED_LIST& m_List=it->second.m_List;
		bool first2=true;
		for(_CFactorizedVertexContainer::FACTORED_LIST::const_iterator fit=m_List.begin(),_fe=m_List.end();
			fit!=_fe;fit++)
		{
			if(first2)
				first2=false;
			else
				Res+='*';
			long long Id=MyCache.GetIndex(*fit);
			Res+="(#";
			Res+=long2str(Id);
			Res+=')';
		}
	}
	return Res;
}

void _CSExpFlatVertices::ToStream(txt_stream& stream,const _CFlatVertexCache& MyCache) const
{
	bool first=true;
	for(size_t i=m_MaxPower;i>0;i--)
	{
		MAP_TYPE::const_iterator it=m_Map.find(i);
		if(it==m_Map.end())
			continue;
		long long Id=MyCache.GetIndex(it->second.pVertex);
		if(Id<0)
			continue;//???
		if(first)
			first=false;
		else
			stream<<" + ";
		stream<<"s^";
		stream<<i;
		stream<<'*';
		short Sgn=it->second.Sgn;
		if(Sgn!=1)
		{
			stream<<'(';
			stream<<Sgn;
			stream<<")*";
		}
		stream<<"(#";
		stream<<Id;
		stream<<')';
	}
	MAP_TYPE::const_iterator it=m_Map.find(0);
	if(it!=m_Map.end())
	{
		long long Id=MyCache.GetIndex(it->second.pVertex);
		if(Id>=0)
		{
			if(first)
				first=false;
			else
				stream<<" + ";
			short Sgn=it->second.Sgn;
			if(Sgn!=1)
			{
				stream<<'(';
				stream<<Sgn;
				stream<<")*";
			}
			stream<<"(#";
			stream<<Id;
			stream<<')';
		}
	}
}

void _CSExpFlatVerticesFactorized::ToStream(txt_stream& stream,const _CFlatVertexCache& MyCache) const
{
	bool first=true;
	for(size_t i=m_MaxPower;i>0;i--)
	{
		MAP_TYPE::const_iterator it=m_Map.find(i);
		if(it==m_Map.end())
			continue;
		if(first)
			first=false;
		else
			stream<<" + ";
		stream<<"s^";
		stream<<i;
		stream<<'*';
		short Sgn=it->second.Sgn;
		if(Sgn!=1)
		{
			stream<<'(';
			stream<<Sgn;
			stream<<")*";
		}
		const _CFactorizedVertexContainer::FACTORED_LIST& m_List=it->second.m_List;
		bool first2=true;
		for(_CFactorizedVertexContainer::FACTORED_LIST::const_iterator fit=m_List.begin(),_fe=m_List.end();
			fit!=_fe;fit++)
		{
			if(first2)
				first2=false;
			else
				stream<<'*';
			long long Id=MyCache.GetIndex(*fit);
			stream<<"(#";
			stream<<Id;
			stream<<')';
		}
		stream<<endl;
	}
	MAP_TYPE::const_iterator it=m_Map.find(0);
	if(it!=m_Map.end())
	{
		if(first)
			first=false;
		else
			stream<<" + ";
		short Sgn=it->second.Sgn;
		if(Sgn!=1)
		{
			stream<<'(';
			stream<<Sgn;
			stream<<")*";
		}
		const _CFactorizedVertexContainer::FACTORED_LIST& m_List=it->second.m_List;
		bool first2=true;
		for(_CFactorizedVertexContainer::FACTORED_LIST::const_iterator fit=m_List.begin(),_fe=m_List.end();
			fit!=_fe;fit++)
		{
			if(first2)
				first2=false;
			else
				stream<<'*';
			long long Id=MyCache.GetIndex(*fit);
			stream<<"(#";
			stream<<Id;
			stream<<')';
		}
		stream<<endl;
	}
	else
		stream<<"(#0)"<<endl;
}

void _CSExpFlatVertices::SoPToStream(txt_stream& stream) const
{
	for(size_t i=m_MaxPower+1;i>0;i--)
	{
		size_t sp=i-1;
		MAP_TYPE::const_iterator it=m_Map.find(sp);
		if(it==m_Map.end())
			continue;
		stream<<"s^"<<sp<<'*'<<endl;
		const _CVertexContainer& Cont=it->second;
		_CFlatVertexWalker VW(stream);
		VW.ProcessNextVertex(Cont.pVertex,Cont.Sgn);
		//Cont.pVertex->WalkFlatResult(_CFlatVertexWalker(stream,Cont.Sgn));
		stream<<endl;
	}
	stream<<endl<<endl;
}

void _CSExpFlatVerticesFactorized::SoPToStream(txt_stream& stream) const
{
	for(size_t i=m_MaxPower+1;i>0;i--)
	{
		size_t sp=i-1;
		MAP_TYPE::const_iterator it=m_Map.find(sp);
		if(it==m_Map.end())
			continue;
		stream<<"s^"<<sp<<'*'<<endl;
		const _CFactorizedVertexContainer& Cont=it->second;
		_CFlatVertexWalker VW(stream);
		bool first=true;
		if(Cont.Sgn!=1)
			stream<<"("<<Cont.Sgn<<")*";
		for(_CFactorizedVertexContainer::FACTORED_LIST::const_iterator it=Cont.m_List.begin(),_e=Cont.m_List.end();it!=_e;it++)
		{
			if(first)
			{
				stream<<'(';
				first=false;
			}
			else 
				stream<<"*(";
			const _CFlatVertex* pVertex=*it;
			VW.ProcessNextVertex(pVertex,1);
			//pVertex->WalkFlatResult(_CFlatVertexWalker(stream));
			stream<<')';
		}
		//VW.ProcessNextVertex(Cont.pVertex,Cont.Sgn);
		//Cont.pVertex->WalkFlatResult(_CFlatVertexWalker(stream,Cont.Sgn));
		stream<<endl;
	}
	stream<<endl<<endl;
}

_CSExpFlatVertices& _CSExpFlatVertices::operator*=(short Multiplier)
{
	if(Multiplier!=1)
	{
		for(MAP_TYPE::iterator it=m_Map.begin();it!=m_Map.end();it++)
			it->second.Sgn*=Multiplier;
	}
	return *this;
}
_CSExpFlatVerticesFactorized& _CSExpFlatVerticesFactorized::operator*=(short Multiplier)
{
	if(Multiplier!=1)
	{
		for(MAP_TYPE::iterator it=m_Map.begin();it!=m_Map.end();it++)
			it->second.Sgn*=Multiplier;
	}
	return *this;
}

const _CMultLevelSExpFlatVertices* _CLeafsSExpVertices::get_smart(size_t Index) const
{
	if(Index>=size())
		return &ZeroMultLevelSExpFlatVertices;
	return at(Index);
}

//const _CSExpFlatVertices* _CLeafsSExpVertices::get_smart(size_t Index) const
//{
//	if(Index>=size())
//		return &ZeroFlatVertices;
//	return at(Index);
//}

//void _CReversFlatVertexConfig::Insert(const _CFlatVertex* pFlatVertex)
//{
//	ASSERTPDD(pFlatVertex!=NULL);
//	m_Sorted=false;
//	const _CFlatVertex* pDesc0=pFlatVertex->Get0Descendent();
//	const _CFlatVertex* pDesc1=pFlatVertex->Get1Descendent();
//	//m_Map[pDesc0].insert(FLAT_PRED(pFlatVertex,D0));
//	//m_Map[pDesc1].insert(FLAT_PRED(pFlatVertex,D1));
//
//	m_Map2[FLAT_PRED(pDesc0,D0)].push_back(pFlatVertex);
//	m_Map2[FLAT_PRED(pDesc1,D1)].push_back(pFlatVertex);
//}
//
//void _CReversFlatVertexConfig::VerticesFromDescendants(const _CFlatVertex* pDesc1,
//													   const _CFlatVertex* pDesc0,
//													   FLAT_VERT_LIST& ResultList)
//{
//	if(!m_Sorted)
//	{
//		Sort();
//		m_Sorted=true;
//	}
//	ResultList.clear();
//	MAP2::iterator it1=m_Map2.find(FLAT_PRED(pDesc1,D1));
//	if(it1==m_Map2.end())
//		return;
//	MAP2::iterator it0=m_Map2.find(FLAT_PRED(pDesc0,D0));
//	if(it0==m_Map2.end())
//		return;
//	FLAT_VERT_LIST& Descs1=it1->second;
//	FLAT_VERT_LIST& Descs0=it0->second;
//	FLAT_VERT_VECTOR VectRes(min(Descs1.size(),Descs0.size()));
//	FLAT_VERT_VECTOR::iterator ResIt=set_intersection(Descs1.begin(),
//		Descs1.end(),Descs0.begin(),Descs0.end(),VectRes.begin());
//	ResultList.assign(VectRes.begin(),ResIt);
//}
//
//void _CReversFlatVertexConfig::FindFeasibleDividers(_CDividerSet& Result)
//{
//	Result.clear();
//	MAP2::iterator it0s=m_Map2.find(FLAT_PRED(NULL,D0));
//	if(it0s==m_Map2.end())
//		return;
//	FLAT_VERT_LIST& Desc0s=it0s->second;
//	for(FLAT_VERT_LIST::iterator it0=Desc0s.begin();it0!=Desc0s.end();it0++)
//	{
//		const _CFlatVertex* pCurrentVertex=*it0;
//		FLAT_VERT_LIST& Rosette=Result.GetNew();
//		Rosette.push_back(pCurrentVertex);
//		AddBump(Result,Rosette);
//	}
//}
//void _CReversFlatVertexConfig::AddBump(_CDividerSet& GlobalSet,FLAT_VERT_LIST& CurrentList)
//{
//	const _CFlatVertex* pCurrentVertex=CurrentList.back();
//	FLAT_VERT_LIST Bumps;
//	VerticesFromDescendants(pCurrentVertex->Get1Descendent(),pCurrentVertex,Bumps);
//	if(Bumps.empty())
//		return;
//	FLAT_VERT_LIST::iterator it=Bumps.begin();
//	const _CFlatVertex* pMainBump=*it++;
//	for(;it!=Bumps.end();it++)
//	{
//		FLAT_VERT_LIST& Rossette=GlobalSet.GetNew(&CurrentList);
//		Rossette.push_back(*it);
//		AddBump(GlobalSet,Rossette);
//	}
//	CurrentList.push_back(pMainBump);
//	AddBump(GlobalSet,CurrentList);
//}
//
//void _CReversFlatVertexConfig::PrepareDividersPaths()
//{
//	if(!m_CPDividersReady)
//	{
//		m_CPDividers.clear();
//		PrepFeasibleDivieders();
//		for(_CDividerSet::iterator it=m_FeasableDividers.begin(),_e=m_FeasableDividers.end();
//			it!=_e;it++)
//		{
//			const FLAT_VERT_LIST& List=*it;
//			CP_SET PathSet(m_Comp);
//			for(FLAT_VERT_LIST::const_iterator lit=List.begin(),_le=List.end();lit!=_le;lit++)
//			{
//				const _CFlatVertex* pVertex=*lit;
//				const COMPONENTS_PATH* Path=pVertex->GetPath();
//				short Sgn=pVertex->Get1Mult();
//				PathSet.insert(SIGNED_COMPONENT_PATH(Path,Sgn));
//			}
//			m_CPDividers.insert(PathSet);
//		}
//		m_CPDividersReady=true;
//	}
//}
//
//string _CReversFlatVertexConfig::ToStringFeasibleDevidersPaths() 
//{
//	PrepareDividersPaths();
//	string Res="In shorts:\n";
//	size_t Counter=0;
//	for(CP_DIVIDERS::const_iterator sit=m_CPDividers.begin(),_se=m_CPDividers.end();sit!=_se;sit++,Counter++)
//	{
//		Res+='#';
//		Res+=long2str(Counter);
//		Res+=": ";
//		const CP_SET& Set=*sit;
//		bool first=true;
//		for(CP_SET::const_iterator pit=Set.begin(),_pe=Set.end();pit!=_pe;pit++)
//		{
//			const SIGNED_COMPONENT_PATH& SgnPath=*pit;
//			const COMPONENTS_PATH* pPath=SgnPath.first;
//			short Sgn=SgnPath.second;
//			if(first)
//				first=false;
//			else
//				Res+=", ";
//			if(Sgn!=1)
//				Res+="("+short2str(Sgn)+")*";
//			Res+=pPath->ToString();
//		}
//		Res+='\n';
//	}
//	return Res;
//}
//
//void _CReversFlatVertexConfig::ToStreamFeasibleDevidersPaths(txt_stream& stream) 
//{
//	PrepareDividersPaths();
//	stream<<"In shorts:"<<endl;
//	size_t Counter=0;
//	for(CP_DIVIDERS::const_iterator sit=m_CPDividers.begin(),_se=m_CPDividers.end();sit!=_se;sit++,Counter++)
//	{
//		stream<<'#'<<Counter<<": ";
//		const CP_SET& Set=*sit;
//		bool first=true;
//		for(CP_SET::const_iterator pit=Set.begin(),_pe=Set.end();pit!=_pe;pit++)
//		{
//			const SIGNED_COMPONENT_PATH& SgnPath=*pit;
//			const COMPONENTS_PATH* pPath=SgnPath.first;
//			short Sgn=SgnPath.second;
//			if(first)
//				first=false;
//			else
//				stream<<", ";
//			if(Sgn!=1)
//				stream<<"("<<Sgn<<")*";
//			stream<<pPath->ToString();
//		}
//		stream<<endl;
//	}
//}
//
//string _CReversFlatVertexConfig::_CDividerSet::ToString(const _CFlatVertexCache& MyCache) const
//{
//	string Res="Possible dividers\n";
//	size_t Counter=0;
//	for(const_iterator it=begin();it!=end();it++,Counter++)
//	{
//		const FLAT_VERT_LIST& List=*it;
//		Res+="[";
//		Res+=long2str(Counter);
//		Res+="]: ";
//		bool first=true;
//		for(FLAT_VERT_LIST::const_iterator lit=List.begin();lit!=List.end();lit++)
//		{
//			if(first)
//				first=false;
//			else
//				Res+=", ";
//			Res+=(*lit)->ToString(MyCache);
//		}
//		Res+='\n';
//	}
//	return Res;
//}
//
//void _CReversFlatVertexConfig::_CDividerSet::ToStream(txt_stream& stream,const _CFlatVertexCache& MyCache) const
//{
//	stream<<"Possible dividers"<<endl;
//	size_t Counter=0;
//	for(const_iterator it=begin();it!=end();it++,Counter++)
//	{
//		const FLAT_VERT_LIST& List=*it;
//		stream<<"["<<Counter<<"]: ";
//		bool first=true;
//		for(FLAT_VERT_LIST::const_iterator lit=List.begin();lit!=List.end();lit++)
//		{
//			if(first)
//				first=false;
//			else
//				stream<<", ";
//			(*lit)->ToStream(stream,MyCache);
//		}
//		stream<<endl;
//	}
//}
//
//
//void _CReversFlatVertexConfig::Sort()
//{
//	for(MAP2::iterator it=m_Map2.begin();it!=m_Map2.end();it++)
//		//sort(it->second.begin(),it->second.end());
//		it->second.sort();
//}

//void _CFlatVertexCache::Test()
//{
//	//_CReversFlatVertexConfig::FLAT_VERT_LIST ResultList;
//	//m_ReversFlatVertexConfig.VerticesFromDescendants(&OneFlatVertex,NULL,ResultList);
//	//_CReversFlatVertexConfig::_CDividerSet Dividers;
//	//m_FeasableDividers.clear();
//	//m_ReversFlatVertexConfig.FindFeasibleDividers(m_FeasableDividers);
//}

void _CFlatVertexCache::ProcessIndex(long long Index, _CFlatVertex* pVertex)
{
	m_IdMap.insert(ID_MAP::value_type(pVertex,Index));
	m_ExtraDataMap.insert(EXTRA_DATA_MAP::value_type(pVertex,_CExtraData(Index)));
	if(!m_Factorisation)
	{
		size_t Index = 0;
		if(pVertex->IsOnlyOneDescendant(Index))
			if (Index!=0)
			{
				m_CPSet.insert(SIGNED_COMPONENT_PATH(pVertex->GetPath(),pVertex->GetMult(Index),Index));
				m_SCPMap.Include(_CSignedComponentPath(pVertex));
			}
		//RegisterNoZero(pVertex);
		//m_ReversFlatVertexConfig.Insert(pVertex);
	}
	TouchDescendants(pVertex);
}

bool _CFlatVertexCache::UnTouchVertex(const _CFlatVertex* pVertex,bool& IsFree)
{
	EXTRA_DATA_MAP::iterator it=m_ExtraDataMap.find(pVertex);
	if(it==m_ExtraDataMap.end())
		return false;
	IsFree=it->second.UnTouch();
	return true;
}

bool _CFlatVertexCache::TouchVertex(const _CFlatVertex* pVertex)
{
	EXTRA_DATA_MAP::iterator it=m_ExtraDataMap.find(pVertex);
	if(it==m_ExtraDataMap.end())
		return false;
	it->second.Touch();
	return true;
}

void _CFlatVertexCache::TouchDescendants(const _CFlatVertex* pVertex)
{
	size_t Size = pVertex->GetCompPower();
	for (size_t i = 0; i < Size; i++)
	{
		const _CFlatVertex* pDesc = pVertex->GetDescendant(i);
		if (pDesc != NULL && pDesc != &OneFlatVertex)
			if (!TouchVertex(pDesc))
				RISEPDD(eWrongCondition, "Unexpected condition");
	}
}

void _CFlatVertexCache::UnTouchDescendants(const _CFlatVertex* pVertex, BOOL_VECTOR& AreFreed)
{
	size_t Size = pVertex->GetCompPower();
	AreFreed.resize(Size);
	for (size_t i = 0; i < Size; i++)
	{
		bool Test = false;
		const _CFlatVertex* pDesc = pVertex->GetDescendant(i);
		if (pDesc != NULL && pDesc != &OneFlatVertex)
			if (!UnTouchVertex(pDesc, Test))
				RISEPDD(eWrongCondition, "Unexpected condition");
		AreFreed[i] = Test;
	}
}
void _CFlatVertexCache::TouchVertices(_CSExpFlatVertices& Vertices)
{
	Vertices.TouchVertices(*this);
}
void _CFlatVertexCache::TouchVertices(_CSExpFlatVerticesFactorized& Vertices)
{
	Vertices.TouchVertices(*this);
}

void _CSExpFlatVerticesFactorized::TouchVertices(_CFlatVertexCache& Cache)
{
	for(MAP_TYPE::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
	{
		_CFactorizedVertexContainer::FACTORED_LIST& List=it->second.m_List;
		for(_CFactorizedVertexContainer::FACTORED_LIST::const_iterator cit=List.begin(),_ce=List.end();
			cit!=_ce;cit++)
				Cache.TouchVertex(*cit);
	}
}

void _CSExpFlatVertices::TouchVertices(_CFlatVertexCache& Cache)
{
	for(MAP_TYPE::iterator it=m_Map.begin(),_e=m_Map.end();it!=_e;it++)
		Cache.TouchVertex(it->second.pVertex);
}

void _CFlatVertexCache::ReleaseVertex(const _CFlatVertex* pVertex)
{
	try
	{
		if(pVertex==NULL || pVertex==&OneFlatVertex)
			return;
		bool Free=false;
		UnTouchVertex(pVertex,Free);
		//if(Free)
		//{
		//	ReleaseVertex(pVertex->Get1Descendent());
		//	ReleaseVertex(pVertex->Get0Descendent());
		//}
	}
	catch(eWrongCondition)
	{
	}
}

void _CFlatVertexCache::ReleaseAndKillVertex(const _CFlatVertex* pVertex)
{
	try
	{
		if(pVertex==NULL || pVertex==&OneFlatVertex)
			return;
		bool Free=false;
		UnTouchVertex(pVertex,Free);
		if(Free)
		{
			for (size_t i = 0; i < pVertex->GetCompPower();i++)
				ReleaseAndKillVertex(pVertex->GetDescendant(i));
			RemoveByReference(pVertex);
		}
	}
	catch(eWrongCondition)
	{
	}
}
void _CFlatVertexCache::RemoveByReference(const _CFlatVertex* pFlatVertex)
{
	EXTRA_DATA_MAP::iterator it=m_ExtraDataMap.find(pFlatVertex);
	if(it==m_ExtraDataMap.end())
		RISEPDD(eWrongCondition,"Wrong Flat Vertex Pointer");
	RemoveByIndex(it->second.GetId(),true);
	m_ExtraDataMap.erase(it);
}

bool _CFlatVertexCache::IsRealised(const _CFlatVertex* pVertex)
{
	EXTRA_DATA_MAP::iterator it=m_ExtraDataMap.find(pVertex);
	if(it==m_ExtraDataMap.end())
		RISEPDD(eWrongCondition,"Wrong Flat Vertex Pointer");
	return it->second.m_UsageCounter==0;
}

void _CFlatVertexCache::PrepareFeasDivList()
{
	//if(m_FeasDivList.size()==m_CPSet.size())
	//	return;
	//m_FeasDivList.clear();
	//for(_CPSet::iterator it=m_CPSet.begin(),_e=m_CPSet.end();it!=_e;it++)
	//{
	//	SIGNED_COMPONENT_PATH& SCP=*it;
	//	_CFlatVertex* pNewVertex=new _CFlatVertex(&OneFlatVertex,NULL,SCP.second,0,*SCP.first);
	//	m_FeasDivList.push_back(PushInCache(pNewVertex,true));
	//}
	if(m_FeasLevelDivList.size()!=0)
		return;
	SIGNED_COMPONENT_PATH_MULTILIST List;
	m_SCPMap.GetSolidPath(List);
	PathList2VertList(m_FeasLevelDivList,List);
}

void _CFlatVertexCache::ClearReleased()
{	
	//m_FeasDivList.clear();
	for(long long i=m_Id-1;i>1;i--)
	{
		INDEX2OBJECT_MAP_ITERATOR oit=m_Index2ObjectMap.find(i);
		if(oit==m_Index2ObjectMap.end())
			continue;
		_CFlatVertex* pVertex=oit->second;
		if(!IsRealised(pVertex))
			continue;
		bool IsFree1=false, IsFree0=false;
		BOOL_VECTOR BoolVect;
		UnTouchDescendants(pVertex,BoolVect);
		RemoveByIndex(i,true);
	}
}

//void _CFlatVertexCache::RegisterNoZero(_CFlatVertex* pVertex)
//{
//	if(m_CurrentCof<1)
//		RISEPDD(eWrongCondition,"m_CurrentCof cannot be 0");
//	ZERO_DESC_MAP& ZeroDescMap=m_ZeroDescMaps.force_at(m_CurrentCof-1);
//	const COMPONENTS_PATH* pPath=pVertex->GetPath();
//	bool NoZero=pVertex->NoZero();
//	ZERO_DESC_MAP::iterator it=ZeroDescMap.find(pPath);
//	if(it==ZeroDescMap.end())
//		ZeroDescMap.insert(ZERO_DESC_MAP::value_type(pPath,NoZero));
//	else
//		it->second=it->second && NoZero;
//}
//void _CFlatVertexCache::NotifySkipped(const COMPONENTS_PATH* pPath)
//{
//	if(m_CurrentCof<1)
//		RISEPDD(eWrongCondition,"m_CurrentCof cannot be 0");
//	ZERO_DESC_MAP& ZeroDescMap=m_ZeroDescMaps.force_at(m_CurrentCof-1);
//	ZERO_DESC_MAP::iterator it=ZeroDescMap.find(pPath);
//	if(it==ZeroDescMap.end())
//		ZeroDescMap.insert(ZERO_DESC_MAP::value_type(pPath,false));
//	else
//		it->second=false;
//}

//void _CFlatVertexCache::LeaveZeroDesc()
//{
//	for(ZERO_DESC_MAPS::iterator mit=m_ZeroDescMaps.begin();mit!=m_ZeroDescMaps.end();mit++)
//	{
//		ZERO_DESC_MAP& Map=(*mit);
//		for(ZERO_DESC_MAP::iterator it=Map.begin();it!=Map.end();)
//			if(!it->second)
//				it=Map.erase(it);
//			else
//				it++;
//	}
//}

long long _CFlatVertexCache::GetIndex(const _CFlatVertex* pVertex) const
{
	if(pVertex==&OneFlatVertex)
		return 1;
	ID_MAP::const_iterator it=m_IdMap.find(pVertex);
	if(it==m_IdMap.end())
		return -1;
	return it->second;
}

bool _CFlatVertexCache::IsMultiInheriter(const _CFlatVertex* pVertex) const
{
	if(pVertex==&OneFlatVertex)
		return true;
	EXTRA_DATA_MAP::const_iterator it=m_ExtraDataMap.find(pVertex);
	return (it!=m_ExtraDataMap.end())?it->second.IsMultiInheriter():false;
}

string _CFlatVertexCache::ToString() const
{
	string Res;
	for(size_t i=2;i<m_Id;i++)
	{
		INDEX2OBJECT_MAP_C_ITERATOR it=m_Index2ObjectMap.find(i);
		if(it==m_Index2ObjectMap.end())
			continue;
		Res+=it->second->ToString(*this);
		Res+='\n';
	}
	return Res;
}

void _CFlatVertexCache::ToStream(txt_stream& stream) const
{
	size_t GlobalMults = 0, GlobalAdds = 0, GlobalMultsNonOne=0;
	for(size_t i=2;i<m_Id;i++)
	{
		INDEX2OBJECT_MAP_C_ITERATOR it=m_Index2ObjectMap.find(i);
		if(it==m_Index2ObjectMap.end())
			continue;
		size_t LocalMults = 0, LocalAdds = 0, LocalMultsNonOne = 0;;
		it->second->ToStream(stream, *this,LocalMults,LocalAdds,LocalMultsNonOne);
		GlobalMults += LocalMults;
		GlobalAdds += LocalAdds;
		GlobalMultsNonOne += LocalMultsNonOne;
		stream<<endl;
	}
	stream << "Nof of multiplications is " << GlobalMults <<"("<<GlobalMultsNonOne<< ") and additions is " << GlobalAdds << endl;
}
//string _CFlatVertexCache::PotSimpToString(size_t CofId) const
//{
//	if(CofId>=m_ZeroDescMaps.size())
//		RISEPDD(out_of_range,"The CofId is out of range");
//	string Res;
//	const ZERO_DESC_MAP& Map=m_ZeroDescMaps[CofId];
//	for(ZERO_DESC_MAP::const_iterator it=Map.begin();it!=Map.end();it++)
//	{
//		Res+=it->first->ToString();
//		Res+="\n";
//	}
//	return Res;
//}
//
//void _CFlatVertexCache::PotSimpToStream(txt_stream& stream, size_t CofId) const
//{
//	if(CofId>=m_ZeroDescMaps.size())
//		return;
//	const ZERO_DESC_MAP& Map=m_ZeroDescMaps[CofId];
//	for(ZERO_DESC_MAP::const_iterator it=Map.begin();it!=Map.end();it++)
//	{
//		stream<<it->first->ToString();
//		stream<<endl;
//	}
//}

_CSExpFlatVertices* _CSExpFlatVerticesFilter::PutFiltered(_CSExpFlatVertices* pOrigin,short ExtraSgn)
{
	if(pOrigin==NULL || ExtraSgn==0)
		return NULL;
	if(ExtraSgn==1)
		return pOrigin;
	m_List.push_back(*pOrigin);
	_CSExpFlatVertices& Copy=m_List.back();
	Copy*=ExtraSgn;
	return &Copy;
}

//bool _CCompPathStorage::KeyCompare::operator() (const COMPONENTS_PATH& Left, const COMPONENTS_PATH& Right)
//{
//	COMPONENTS_PATH::const_iterator lit=Left.begin(),
//		rit=Right.begin(),
//		_le=Left.end(),
//		_re=Right.end();
//	_CCircuit* pCurCircuit=m_pMainCircuit;
//	for(;lit!=_le && rit!=_re;lit++,rit++)
//	{
//		short cmp_res=pCurCircuit->CompLocalComponents(*lit,*rit);
//		if(cmp_res<0)
//			return true;
//		else if(cmp_res>0)
//			return false;
//		const _CComponent *pLeftComp=(*lit);
//		const _CComponent *pRightComp=(*rit);
//		if(!pLeftComp->IsMacroDependant())
//			break;
//		pCurCircuit=dynamic_cast<const _CSubCircuitSocket*>(*lit)->GetModelCircuit();
//	}
//	return false;
//}

//short _CCompPathStorage::KeyCompare::PCompare(const COMPONENTS_PATH& Left, const COMPONENTS_PATH& Right)
//{
//	COMPONENTS_PATH::const_iterator lit=Left.begin(),
//		rit=Right.begin(),
//		_le=Left.end(),
//		_re=Right.end();
//	_CCircuit* pCurCircuit=m_pMainCircuit;
//	for(;lit!=_le && rit!=_re;lit++,rit++)
//	{
//		const _CComponent *pLeftComp=(*lit);
//		const _CComponent *pRightComp=(*rit);
//		short cmp_res=pCurCircuit->CompLocalComponents(pLeftComp,pRightComp);
//		if(cmp_res!=0 || !pLeftComp->IsMacroDependant() || !pRightComp->IsMacroDependant())
//			return cmp_res;
//		pCurCircuit=dynamic_cast<const _CSubCircuitSocket*>(*lit)->GetModelCircuit();
//	}
//	return lit==_le?(rit==_re?0:-1):1;
//}

short PCompare(const COMPONENTS_PATH& Left, const COMPONENTS_PATH& Right,_CMainCircuit* pMainCircuit)
{
	COMPONENTS_PATH::const_iterator lit=Left.begin(),
		rit=Right.begin(),
		_le=Left.end(),
		_re=Right.end();
	_CCircuit* pCurCircuit=pMainCircuit;
	for(;lit!=_le && rit!=_re;lit++,rit++)
	{
		const _CComponent *pLeftComp=(*lit);
		const _CComponent *pRightComp=(*rit);
		short cmp_res=pCurCircuit->CompLocalComponents(pLeftComp,pRightComp);
		if(cmp_res!=0 || !pLeftComp->IsMacroDependant() || !pRightComp->IsMacroDependant())
			return cmp_res;
		pCurCircuit=dynamic_cast<const _CSubCircuitSocket*>(*lit)->GetModelCircuit();
	}
	return lit==_le?(rit==_re?0:-1):1;
}
//short _CReversFlatVertexConfig::_CCP_Set_comp::Comp(const CP_SET& Left, const CP_SET& Right)
////bool _CReversFlatVertexConfig::_CCP_Set_comp::operator()(const CP_SET& Left, const CP_SET& Right)
//{
//	CP_SET::const_iterator lit=Left.begin(),
//		rit=Right.begin(),
//		_le=Left.end(),
//		_re=Right.end();
//	for(;lit!=_le && rit!=_re;lit++,rit++)
//	{
//		const SIGNED_COMPONENT_PATH& lPath=*lit;
//		const SIGNED_COMPONENT_PATH& rPath=*rit;
//		short cmp_res=PCompare(*lPath.first,*rPath.first,m_pMainCircuit);
//		if(cmp_res!=0)
//			return cmp_res;
//		if(lPath.second==rPath.second)
//			continue;
//		return lPath.second<rPath.second?-1:1;
//
//	}
//	return lit==_le?(rit==_re?0:-1):1;
//}

//const _CFlatVertex* _CFlatVertexCache::GetVertex(const _CFlatVertex* Descendent1,
//						   const _CFlatVertex* Descendent0,
//						   short Multiplier1,
//						   short Multiplier0,
//						   const COMPONENTS_PATH& ComponentsPath,
//						   short& TopSgn)
const _CFlatVertex* _CFlatVertexCache::GetVertex(
	const DESCENDANTS& Descendants,
	const MULTIPLIERS& Multipliers,
	const COMPONENTS_PATH& ComponentsPath,
	short& TopSgn)
{
	const _CFlatVertex* pResVertex=NULL;
	size_t Pin = 0;
	if (IsOnlyOneDescendant(Descendants, Pin))
	{
		if (Pin == 0)
		{
			TopSgn *= Multipliers[0];
			pResVertex = Descendants[0];
		}
	}
	else 
	{
		short Mult = 1;
		_CFlatVertex* pResVertexN = nullptr;
		if(HasCommonDescSgn(Multipliers,Mult))
			{
				TopSgn*=Mult;
				MULTIPLIERS OneMults(Multipliers.size(),1);
				pResVertexN = new _CFlatVertex(Descendants, OneMults, ComponentsPath);
			}
		else
			pResVertexN = new _CFlatVertex(Descendants, Multipliers, ComponentsPath);
		pResVertex = PushInCache(pResVertexN, true);
	}
	return pResVertex;
}

//To be return

//const _CFlatVertex* _CFlatVertexCache::MultiplyDiagrams(const _CFlatVertex* Left, const _CFlatVertex* Right, short& TopSgn)
//{
//	ASSERTPDD(ValidateVertex(Left));
//	ASSERTPDD(ValidateVertex(Right));
//	if(Left==NULL || Right==NULL)
//	{	TopSgn=0;
//		return NULL;
//	}
//	if(Left==&OneFlatVertex)
//		return Right;
//	if(Right==&OneFlatVertex)
//		return Left;
//	_CBinaryOperationContainer Cont(Left,Right);
//	const _CBinaryOperationContainer* pResCont=m_MultiplyCache.FindRegistered(Cont);
//	if(pResCont!=NULL)
//	{
//		TopSgn=pResCont->GetResMultiplier();
//		return pResCont->GetResult();
//	}
//	const COMPONENTS_PATH& LeftPath=*Left->GetPath();
//	const COMPONENTS_PATH& RightPath=*Right->GetPath();
//	short Comp=PCompare(LeftPath,RightPath,m_pMainCircuit);
//	const _CFlatVertex* p1Desc=NULL;
//	short Mult1=1;
//	const _CFlatVertex* p0Desc=NULL;
//	short Mult0=1;
//	const COMPONENTS_PATH* TopPath=NULL;
//	if(Comp<0)
//	{
//		Mult1=Left->Get1Mult();
//		Mult0=Left->Get0Mult();
//		p1Desc=MultiplyDiagrams(Left->Get1Descendent(),Right,Mult1);
//		p0Desc=MultiplyDiagrams(Left->Get0Descendent(),Right,Mult0);
//		TopPath=&LeftPath;
//	}
//	else if(Comp>0)
//	{
//		Mult1=Right->Get1Mult();
//		Mult0=Right->Get0Mult();
//		p1Desc=MultiplyDiagrams(Left,Right->Get1Descendent(),Mult1);
//		p0Desc=MultiplyDiagrams(Left,Right->Get1Descendent(),Mult0);
//		TopPath=&RightPath;
//	}
//	else
//		RISEPDD(eNotImplementedOperation,"I think about it.");
//	const _CFlatVertex* pResVertex=GetVertex(p1Desc,p0Desc,Mult1,Mult0,*TopPath,TopSgn);
//	Cont.SetResult(pResVertex,TopSgn);
//	m_MultiplyCache.Register(Cont);
//	return pResVertex;
//}
//const _CFlatVertex* _CFlatVertexCache::DivideByASingleValue(DIV_STATUS& Status, const _CFlatVertex* Left, const SIGNED_COMPONENT_PATH& Right, short& TopSgn)
//{
//	_CFlatVertex* pRightVertex=new _CFlatVertex(&OneFlatVertex,NULL,Right.second,0,*Right.first);
//	pRightVertex=PushInCache(pRightVertex,true);
//	const _CFlatVertex* pResVertex=NULL;
//	Status=DivideWithoutReminder(pResVertex,Left,pRightVertex,TopSgn,true);
//	return pResVertex;
//}

//To be return

//const _CFlatVertex* _CFlatVertexCache::CPSet2Rossette(const _CReversFlatVertexConfig::CP_SET& CpSet)
//{
//	const _CFlatVertex* pPrevVertex=NULL;
//	for(_CReversFlatVertexConfig::CP_SET::const_iterator it=CpSet.begin();it!=CpSet.end();it++)
//	{	
//		const SIGNED_COMPONENT_PATH& Right=*it;
//		_CFlatVertex* pNewVertex=new _CFlatVertex(&OneFlatVertex,pPrevVertex,Right.second,pPrevVertex==NULL?0:1,*Right.first);
//		pPrevVertex=PushInCache(pNewVertex,true);
//	}
//	return pPrevVertex;	
//}


//const _CFlatVertex* _CFlatVertexCache::DivideByARossette(DIV_STATUS& Status, const _CFlatVertex* Left, const _CReversFlatVertexConfig::CP_SET& Right, short& TopSgn)
//{
//	bool IsRossette=Right.size()>1;
//	const _CFlatVertex* RightF=CPSet2Rossette(Right);
//	const _CFlatVertex* Result=NULL;
//	Status=DivideWithoutReminder(Result,Left,RightF,TopSgn,false);
//	return Result;
//}

//_CFlatVertexCache::DIV_STATUS _CFlatVertexCache::DivideWithoutReminder(const _CFlatVertex*& Result,const _CFlatVertex* Left, const _CFlatVertex* Right, short& TopSgn)
//{
//	try
//	{
//		if(DivideWithoutReminderR(Result,Left,Right,TopSgn))
//			return Divider;
//		else
//			return HasReminder;
//	}
//	catch(excp)
//	{
//		return InCompatible;
//	}
//}

//To be return

//_CFlatVertexCache::DIV_STATUS _CFlatVertexCache::DivideWithoutReminder(const _CFlatVertex*& Result,const _CFlatVertex* Left, const _CFlatVertex* Right, short& TopSgn,const bool ReminderAlllowed)
//{
//	ASSERTPDD(ValidateVertex(Left));
//	ASSERTPDD(ValidateVertex(Right));
//	ASSERTPDD(Right!=NULL);
//	DIV_STATUS Status=Divider;
//	if(Left==Right)
//	{
//		Result=&OneFlatVertex;
//		TopSgn=1;
//	}
//	else if(Right==&OneFlatVertex)
//	{
//		Result=Left;
//		TopSgn=1;
//	}
//	else if(Left==NULL || Left==&OneFlatVertex)
//	{
//		Result=NULL;
//		TopSgn=0;
//		//Status=ReminderAlllowed?HasReminder:InCompatible;
//		if(Left!=NULL)
//			Status=HasReminder;
//	}
//	else
//	{
//		_CDivOperationContainer Cont(Left,Right);
//		const _CDivOperationContainer* pResCont=m_DivisionCache.FindRegistered(Cont);
//		if(pResCont!=NULL)
//		{
//			TopSgn=pResCont->GetResMultiplier();
//			Result=pResCont->GetResult();
//			Status=pResCont->GetStatus();
//		}
//		else
//		{
//			const COMPONENTS_PATH& LeftPath=*Left->GetPath();
//			const COMPONENTS_PATH& RightPath=*Right->GetPath();
//			short Comp=PCompare(LeftPath,RightPath,m_pMainCircuit);
//			Result=NULL;
//			if(Comp>0)
//			{
//				//Result=NULL;
//				TopSgn=0;
//				Status=HasReminder;
//			}
//			else if(Comp<0)
//			{
//				const _CFlatVertex* pDesc1=Left->Get1Descendent();
//				short Sgn1=Left->Get1Mult();
//				const _CFlatVertex* pDesc1Res=NULL;
//				short Sgn1Res=1;
//				DIV_STATUS LocalStatus=DivideWithoutReminder(pDesc1Res,pDesc1,Right,Sgn1Res,ReminderAlllowed);
//				if(LocalStatus==InCompatible)
//				{
//					Status=InCompatible;
//					goto Short_to_Registry;
//				}
//				if(LocalStatus==HasReminder)
//					Status=HasReminder;
//				if(!ReminderAlllowed && Status==HasReminder)
//					//goto Short_to_Registry;
//					return Status;
//
//				Sgn1*=Sgn1Res;
//				const _CFlatVertex* pDesc0=Left->Get0Descendent();
//				short Sgn0=Left->Get0Mult();
//				const _CFlatVertex* pDesc0Res=NULL;
//				short Sgn0Res=1;
//				LocalStatus=DivideWithoutReminder(pDesc0Res,pDesc0,Right,Sgn0Res,ReminderAlllowed);
//				if(LocalStatus==InCompatible)
//				{
//					Status=InCompatible;
//					goto Short_to_Registry;
//				}
//				if(LocalStatus==HasReminder)
//					Status=HasReminder;
//				if(!ReminderAlllowed && Status==HasReminder)
//					return Status;
//					//goto Short_to_Registry;
//
//				Sgn0*=Sgn0Res;
//				Result=GetVertex(pDesc1Res,pDesc0Res,Sgn1,Sgn0,LeftPath,TopSgn);
//			}
//			else
//			{
//				const _CFlatVertex* pDesc1Left=Left->Get1Descendent();
//				short Sgn1Left=Left->Get1Mult();
//				const _CFlatVertex* pDesc1Right=Right->Get1Descendent();
//				short Sgn1Right=Right->Get1Mult();
//				short Top1=1;
//				const _CFlatVertex* pRes1=NULL;
//				DIV_STATUS LocalStatus=DivideWithoutReminder(pRes1,pDesc1Left,pDesc1Right,Top1,ReminderAlllowed); 
//				if(LocalStatus==InCompatible)
//				{
//					Status=InCompatible;
//					goto Short_to_Registry;
//				}
//				if(LocalStatus==HasReminder)
//					Status=HasReminder;
//				if(!ReminderAlllowed && Status==HasReminder)
//					return Status;
//					//goto Short_to_Registry;
//				short Mult1=Sgn1Left*Top1/Sgn1Right;
//
//				const _CFlatVertex* pDesc0Left=Left->Get0Descendent();
//				short Sgn0Left=Left->Get0Mult();
//				const _CFlatVertex* pDesc0Right=Right->Get0Descendent();
//				short Sgn0Right=Right->Get0Mult();
//				short Top0=1;
//				const _CFlatVertex* pRes0=NULL;
//				if(pDesc0Right==NULL)
//				{
//					if(pDesc0Left!=NULL)
//						Status=HasReminder;
//				}
//				else
//				{
//					LocalStatus=DivideWithoutReminder(pRes0,pDesc0Left,pDesc0Right,Top0,ReminderAlllowed); 
//					if(LocalStatus==InCompatible)
//					{
//						Status=InCompatible;
//						goto Short_to_Registry;
//					}
//					if(LocalStatus==HasReminder)
//						Status=HasReminder;
//					if(!ReminderAlllowed && Status==HasReminder)
//						return Status;
//						//goto Short_to_Registry;
//					short Mult0=Sgn0Left*Top0/Sgn0Right;
//					if(Mult1!=Mult0 || pRes1!=pRes0)
//					{
//						Status=InCompatible;
//						goto Short_to_Registry;
//					}
//				}
//				Result=pRes1;
//				TopSgn=Mult1;
//			}
//Short_to_Registry:
//			Cont.SetResult(Result,TopSgn,Status);
//			m_DivisionCache.Register(Cont);
//		}
//	}
//	return Status;
//}

//To be return


//bool _CFlatVertexCache::DivideWithoutReminderR(const _CFlatVertex*& Result,const _CFlatVertex* Left, const _CFlatVertex* Right, short& TopSgn)
//{
//	ASSERTPDD(ValidateVertex(Left));
//	ASSERTPDD(ValidateVertex(Right));
//	ASSERTPDD(Left!=NULL);
//	bool WithoutReminder=true;
//	if(Left==Right)
//	{
//		Result=&OneFlatVertex;
//		TopSgn=1;
//	}
//	else if(Right==&OneFlatVertex)
//	{
//		Result=Left;
//		TopSgn=1;
//	}
//	else if(Left==&OneFlatVertex)
//	{
//		Result=NULL;
//		TopSgn=0;
//	}
//	else
//	{
//		_CDivOperationContainer Cont(Left,Right);
//		const _CDivOperationContainer* pResCont=m_DivisionCache.FindRegistered(Cont);
//		if(pResCont!=NULL)
//		{
//			TopSgn=pResCont->GetResMultiplier();
//			Result=pResCont->GetResult();
//			WithoutReminder=pResCont->GetWithoutReminder();
//		}
//		else
//		{
//			const COMPONENTS_PATH& LeftPath=*Left->GetPath();
//			const COMPONENTS_PATH& RightPath=*Right->GetPath();
//			short Comp=PCompare(LeftPath,RightPath,m_pMainCircuit);
//			if(Comp>0)
//			{
//				Result=NULL;
//				TopSgn=0;
//				WithoutReminder=false;
//			}
//			else if(Comp<0)
//			{
//				const _CFlatVertex* pDesc1=Left->Get1Descendent();
//				short Sgn1=Left->Get1Mult();
//				const _CFlatVertex* pDesc1Res=NULL;
//				short Sgn1Res=1;
//				bool LocalWithoutR=DivideWithoutReminderR(pDesc1Res,pDesc1,Right,Sgn1Res);
//				WithoutReminder=WithoutReminder && LocalWithoutR; 
//					
//				Sgn1*=Sgn1Res;
//				const _CFlatVertex* pDesc0=Left->Get0Descendent();
//				short Sgn0=Left->Get0Mult();
//				const _CFlatVertex* pDesc0Res=NULL;
//				short Sgn0Res=1;
//				LocalWithoutR=DivideWithoutReminderR(pDesc0Res,pDesc0,Right,Sgn0Res);
//				WithoutReminder=WithoutReminder && LocalWithoutR; 
//				Sgn0*=Sgn0Res;
//				Result=GetVertex(pDesc1Res,pDesc0Res,Sgn1,Sgn0,LeftPath,TopSgn);
//			}
//			else
//			{
//				const _CFlatVertex* pDesc1Left=Left->Get1Descendent();
//				short Sgn1Left=Left->Get1Mult();
//				const _CFlatVertex* pDesc1Right=Right->Get1Descendent();
//				short Sgn1Right=Right->Get1Mult();
//				short Top1=1;
//				const _CFlatVertex* pRes1=NULL;
//				bool LocalWithoutR=DivideWithoutReminderR(pRes1,pDesc1Left,pDesc1Right,Top1); 
//				WithoutReminder=WithoutReminder && LocalWithoutR; 
//				short Mult1=Sgn1Left*Top1/Sgn1Right;
//
//				const _CFlatVertex* pDesc0Left=Left->Get0Descendent();
//				short Sgn0Left=Left->Get0Mult();
//				const _CFlatVertex* pDesc0Right=Right->Get0Descendent();
//				short Sgn0Right=Right->Get0Mult();
//				short Top0=1;
//				const _CFlatVertex* pRes0=NULL;
//				if(pDesc0Right==NULL)
//				{
//					if(pDesc0Left!=NULL)
//						throw excp();
//				}
//				else
//				{
//					LocalWithoutR=DivideWithoutReminderR(pRes0,pDesc0Left,pDesc0Right,Top0); 
//					WithoutReminder=WithoutReminder && LocalWithoutR; 
//					short Mult0=Sgn0Left*Top0/Sgn0Right;
//					if(Mult1!=Mult0 || pRes1!=pRes0)
//						throw excp();
//					else
//					{
//						Result=pRes1;
//						TopSgn=Mult1;
//					}
//				}
//			}
//			Cont.SetResult(Result,TopSgn,WithoutReminder);
//			m_DivisionCache.Register(Cont);
//		}
//	}
//	return WithoutReminder;
//}

//To be return

//bool _CFlatVertexCache::IsDivider(const _CFlatVertex* Left, const _CFlatVertex* Right)
//{
//	return false;
//}
//
//void _CFlatVertexCache::FactorizeAll2(size_t NoOfCofacotors,_CContextSExpFlatVertices& ExpFlatVerticesFactorized,const string* Context)
//{
//	for(size_t i=0;i<NoOfCofacotors;i++)
//	{
//		_CSExpFlatVerticesFactorized& Vertices2Factorize=
//			ExpFlatVerticesFactorized.GetSExpFlatResVertices(i,Context);
//		for(size_t s=0;s<=Vertices2Factorize.size();s++)
//		{
//			_CFactorizedVertexContainer* pVertexContainer=Vertices2Factorize.sPowerCont[s];
//			if(pVertexContainer==NULL)
//				continue;
//			const _CFlatVertex*& pMainRoot=pVertexContainer->m_List.front();
//			if(!pVertexContainer->NotFactorize())
//			{
//			}
//		}
//	}
//}
//
//void _CFlatVertexCache::FactorizeAll(size_t Size,_CContextSExpFlatVertices& ExpFlatVerticesFactorized,const string* Context)
//{
//	//if(VertexContainer.NotFactorize())
//	//	return;
//	//m_Factorisation=true;
//	PrepareFeasDivList();
//	//size_t _size=Cofactors.size();
//	m_Factorisation=true;
//	for(FLAT_VERT_MULTILIST::iterator it=m_FeasLevelDivList.begin(),_e=m_FeasLevelDivList.end();it!=_e;it++)
//	{
//		ClearOperCache();
//		ClearReleased();
//		FLAT_VERT_LIST& SolidList=*it;
//		FLAT_VERT_MULTILIST::iterator _nit=it;
//		bool TheLast=(++_nit)==_e;
//		for(FLAT_VERT_LIST::iterator mit=SolidList.begin(),_me=SolidList.end();mit!=_me;mit++)
//		{
//			for(size_t i=0;i<Size;i++)
//			{
//				_CSExpFlatVerticesFactorized& Vertices2Factorize=
//					ExpFlatVerticesFactorized.GetSExpFlatResVertices(i,Context);
//				for(size_t s=0;s<=Vertices2Factorize.size();s++)
//				{
//					_CFactorizedVertexContainer* pVertexContainer=Vertices2Factorize.sPowerCont[s];
//					if(pVertexContainer==NULL)
//						continue;
//					const _CFlatVertex*& pMainRoot=pVertexContainer->m_List.front();
//					if(pVertexContainer!=NULL && !pVertexContainer->NotFactorize())
//					{
//						const _CFlatVertex* pDivider=*mit;
//						short TopSgn=1;
//						const _CFlatVertex* pDivResult=NULL;
//						DIV_STATUS Status=DivideWithoutReminder(pDivResult,pMainRoot,pDivider,TopSgn,true);
//						if(pDivResult!=NULL)
//							switch(Status)
//							{
//							case Divider:
//								{
//									TouchVertex(pDivider);
//									pVertexContainer->m_List.push_back(pDivider);
//									bool IsFree=false;
//									UnTouchVertex(pMainRoot,IsFree);
//									TouchVertex(pDivResult);
//									pMainRoot=pDivResult;
//									pVertexContainer->Sgn*=TopSgn;
//								};
//							case InCompatible: break;
//							case HasReminder:
//								{
//									const _CFlatVertex* pNewResult=NULL;
//									if(Divider==DivideWithoutReminder(pNewResult,pMainRoot,pDivResult,TopSgn,false))
//									{
//										TouchVertex(pNewResult);
//										pVertexContainer->m_List.push_back(pNewResult);
//										bool IsFree=false;
//										UnTouchVertex(pMainRoot,IsFree);
//										TouchVertex(pDivResult);
//										pMainRoot=pDivResult;
//										pVertexContainer->Sgn*=TopSgn;
//										break;
//									}
//								}
//							}
//					}
//					if(TheLast)
//					{
//						if(pMainRoot==&OneFlatVertex)
//							pVertexContainer->m_List.pop_front();
//						pVertexContainer->SetFactorized();
//					}
//				}
//			}
//		}
//	}
//	ClearDividers();
//	ClearOperCache();
//	ClearReleased();
//	m_Factorisation=false;
//}
//
//void _CFlatVertexCache::FactorizeMe2(_CFactorizedVertexContainer& VertexContainer)
//{
//	if(VertexContainer.NotFactorize())
//		return;
//	m_Factorisation=true;
//	PrepareFeasDivList();
//	const _CFlatVertex*& pMainRoot=VertexContainer.m_List.front();
//	for(FLAT_VERT_MULTILIST::iterator it=m_FeasLevelDivList.begin(),_e=m_FeasLevelDivList.end();it!=_e;it++)
//	{
//		FLAT_VERT_LIST& SolidList=*it;
//		for(FLAT_VERT_LIST::iterator mit=SolidList.begin(),_me=SolidList.end();mit!=_me;mit++)
//		{
//			const _CFlatVertex* pDivider=*mit;
//			short TopSgn=1;
//			const _CFlatVertex* pDivResult=NULL;
//			DIV_STATUS Status=DivideWithoutReminder(pDivResult,pMainRoot,pDivider,TopSgn,true);
//			if(pDivResult==NULL)
//				goto skip_higher;
//			switch(Status)
//			{
//			case Divider:
//				{
//					TouchVertex(pDivider);
//					VertexContainer.m_List.push_back(pDivider);
//					bool IsFree=false;
//					UnTouchVertex(pMainRoot,IsFree);
//					TouchVertex(pDivResult);
//					pMainRoot=pDivResult;
//					VertexContainer.Sgn*=TopSgn;
//				};
//			case InCompatible: goto skip_higher;
//			case HasReminder:
//				{
//					const _CFlatVertex* pNewResult=NULL;
//					if(Divider==DivideWithoutReminder(pNewResult,pMainRoot,pDivResult,TopSgn,false))
//					{
//						TouchVertex(pNewResult);
//						VertexContainer.m_List.push_back(pNewResult);
//						bool IsFree=false;
//						UnTouchVertex(pMainRoot,IsFree);
//						TouchVertex(pDivResult);
//						pMainRoot=pDivResult;
//						VertexContainer.Sgn*=TopSgn;
//						goto skip_higher;
//					}
//				}
//				//break;
//			}
//		}
//skip_higher:;
//	}
//	if(pMainRoot==&OneFlatVertex)
//		VertexContainer.m_List.pop_front();
//	m_Factorisation=false;
//	VertexContainer.SetFactorized();
//}
//
//void _CFlatVertexCache::FactorizeMe(_CFactorizedVertexContainer& VertexContainer)
//{
//	if(VertexContainer.m_List.size()>1)
//		return;
//	m_Factorisation=true;
//	PrepareFeasDivList();
//	const _CFlatVertex*& pMainRoot=VertexContainer.m_List.front();
//	//_CReversFlatVertexConfig::CP_DIVIDERS& FeasableDividers=m_ReversFlatVertexConfig.GetCPDividers();
//	//for(_CReversFlatVertexConfig::CP_DIVIDERS::iterator it=FeasableDividers.begin();
//	//	it!=FeasableDividers.end();it++)
//	for(FLAT_VERT_LIST::iterator it=m_FeasDivList.begin(),_e=m_FeasDivList.end();it!=_e;it++)
//	{
//		
//		//_CReversFlatVertexConfig::CP_SET& OneSingleDivider=*it;
//		//bool IsOneSingle=OneSingleDivider.size()==1;
//		//const _CFlatVertex* pDivider=CPSet2Rossette(OneSingleDivider);
//		const _CFlatVertex* pDivider=*it;
//		short TopSgn=1;
//		const _CFlatVertex* pDivResult=NULL;
//		//DIV_STATUS Status=DivideWithoutReminder(pDivResult,pMainRoot,pDivider,TopSgn,IsOneSingle);
//		DIV_STATUS Status=DivideWithoutReminder(pDivResult,pMainRoot,pDivider,TopSgn,true);
//		if(pDivResult==NULL)
//			continue;
//		//const _CFlatVertex* DivResult=DivideByARossette(Status,pMainRoot,OneSingleDivider,TopSgn);
//		switch(Status)
//		{
//		case Divider:
//			{
//				TouchVertex(pDivider);
//				VertexContainer.m_List.push_back(pDivider);
//				bool IsFree=false;
//				UnTouchVertex(pMainRoot,IsFree);
//				TouchVertex(pDivResult);
//				pMainRoot=pDivResult;
//				VertexContainer.Sgn*=TopSgn;
//			}
//			break;
//		case InCompatible: continue;
//		case HasReminder:
//			{
//				const _CFlatVertex* pNewResult=NULL;
//				if(Divider==DivideWithoutReminder(pNewResult,pMainRoot,pDivResult,TopSgn,false))
//				{
//					TouchVertex(pNewResult);
//					VertexContainer.m_List.push_back(pNewResult);
//					bool IsFree=false;
//					UnTouchVertex(pMainRoot,IsFree);
//					TouchVertex(pDivResult);
//					pMainRoot=pDivResult;
//					VertexContainer.Sgn*=TopSgn;
//				}
//			}
//			break;
//		}
//	}
//	if(pMainRoot==&OneFlatVertex)
//		VertexContainer.m_List.pop_front();
//	m_Factorisation=false;
//}
//

//To be return

bool _CFlatVertexCache::ValidateVertex(const _CFlatVertex* pVertex)
{
	if(pVertex==NULL || pVertex==&OneFlatVertex)
		return true;
	ID_MAP::iterator it=m_IdMap.find(pVertex);
	return (it!=m_IdMap.end());
}

unsigned long long _CFlatVertexCache::_CBinaryOperationContainer::GetHash()
{
	ULONGLONG Res=HASH_CORE;
	Res^=(ULONGLONG)m_LeftOperand;
	Res*=HASH_FACTOR;
	//Res^=(ULONGLONG)m_LeftMultiplier;
	//Res*=HASH_FACTOR;
	Res^=(ULONGLONG)m_RightOperand;
	Res*=HASH_FACTOR;
	//Res^=(ULONGLONG)m_RightMultiplier;
	//Res*=HASH_FACTOR;
	return Res;
}

//const _CFlatVertexCache::_CBinaryOperationContainer* _CFlatVertexCache::_CBinaryOperationCache::FindRegistered
//(_CFlatVertexCache::_CBinaryOperationContainer& Container)
//{
//	ULONGLONG Hash=Container.GetHash();
//	pair<MAP::iterator,MAP::iterator> range=m_Map.equal_range(Hash);
//	for(MAP::iterator it=range.first;it!=range.second;it++)
//	{
//		_CBinaryOperationContainer& ResCont=it->second;
//		if(ResCont==Container)
//			return &ResCont;
//	}
//	return NULL;
//}
//
//const _CFlatVertexCache::_CBinaryOperationContainer* _CFlatVertexCache::_CBinaryOperationCache::Register
//(_CFlatVertexCache::_CBinaryOperationContainer& Container)
//{
//	ASSERTPDD(FindRegistered(Container)==NULL);
//	ULONGLONG Hash=Container.GetHash();
//	return &(m_Map.insert(MAP::value_type(Hash,Container))->second);
//}

string _CFlatVertexCache::_CPSet::ToString() const
{
	string Res;
	Res+="Possible deviders:\n";
	size_t Counter=0;
	for(_CPSet::const_iterator sit=begin(),_se=end();sit!=_se;sit++,Counter++)
	{
		Res+='#';
		Res+=ulong2str(Counter);
		Res+=": ";
		const SIGNED_COMPONENT_PATH& SgnPath=*sit;
		const COMPONENTS_PATH* pPath=get<0>(SgnPath);
		short Sgn = get<1>(SgnPath);
		short Power = get<2>(SgnPath);
		if(Sgn!=1)
		{
			Res+="(";
			Res+=ulong2str(Sgn);
			Res+=")*";
		}
		Res+=pPath->ToString();
		if (Power != 1)
		{
			Res += "^(";
			Res += ulong2str(Power);
			Res += ")";
		}
		Res+='\n';
	}
	return Res;
}
void _CFlatVertexCache::_CPSet::ToStream(txt_stream& stream) const
{
	stream<<"Possible deviders:"<<endl;
	size_t Counter=0;
	for(_CPSet::const_iterator sit=begin(),_se=end();sit!=_se;sit++,Counter++)
	{
		stream << '#' << Counter << ": ";
		const SIGNED_COMPONENT_PATH& SgnPath = *sit;
		const COMPONENTS_PATH* pPath = get<0>(SgnPath);
		short Sgn = get<1>(SgnPath);
		short Power = get<2>(SgnPath);
		if (Sgn != 1)
			stream << "(" << Sgn << ")*";
		stream << pPath->ToString();
		if (Power!=1)
			stream << "^(" << Power << ")*";
		stream << endl;
	}
}

_CSignedComponentPath::_CSignedComponentPath(const _CFlatVertex* pVertex) : 
m_Next(nullptr,0,1)
{
	size_t OnlyNonZeroDesc = 0;
	if (pVertex->IsOnlyOneDescendant(OnlyNonZeroDesc))
		if (OnlyNonZeroDesc != 0)
		{
			get<0>(m_Current) = pVertex->GetPath();
			get<1>(m_Current) = pVertex->GetMult(OnlyNonZeroDesc);
			get<2>(m_Current) = OnlyNonZeroDesc;
			const _CFlatVertex* pNextVertex = pVertex->GetDescendant(OnlyNonZeroDesc);
			size_t OnlyNonZeroDescNext = 0;
			if (pNextVertex->IsOnlyOneDescendant(OnlyNonZeroDescNext))
				if (OnlyNonZeroDescNext != 0)
				{
					get<0>(m_Next) = pNextVertex->GetPath();
					get<1>(m_Next) = pNextVertex->GetMult(OnlyNonZeroDescNext);
					get<2>(m_Next) = OnlyNonZeroDescNext;
				}
		}
	//ASSERTPDD(pVertex->NoZero());
	//m_Current.first=pVertex->GetPath();
	//m_Current.second=pVertex->Get1Mult();
	//const _CFlatVertex* pNextVertex=pVertex->Get1Descendent();
	//if(pNextVertex->NoZero())
	//{
	//	m_Next.first=pNextVertex->GetPath();
	//	m_Next.second=pNextVertex->Get1Mult();
	//}
}

bool _CFlatVertexCache::_SCPMap::Include(_CSignedComponentPath& CompPath)
{
	SIGNED_COMPONENT_PATH& Current=CompPath.GetCurrent();
	SIGNED_COMPONENT_PATH& Next=CompPath.GetNext();
	pair<iterator,iterator> Range=equal_range(Current);
	for(iterator it=Range.first;it!=Range.second;it++)
		if(it->second==Next || Next==NULL_PATH)
			return false;
		else if(it->second==NULL_PATH)
		{
			it->second=Next;
			return true;
		}
	insert(value_type(Current,Next));
	return true;
}

void _CFlatVertexCache::_SCPMap::GetSolidPath(SIGNED_COMPONENT_PATH_MULTILIST& List)
{
	List.clear();
	for(iterator it=begin(),_e=end();it!=_e;it++)
	{
		const SIGNED_COMPONENT_PATH& Current=it->first;
		const SIGNED_COMPONENT_PATH& Next=it->second;
		List.push_back(SIGNED_COMPONENT_PATH_LIST());
		SIGNED_COMPONENT_PATH_LIST& LocalList=List.back();
		LocalList.push_back(Current);
		GetNextPathToken(List,LocalList,Next);
	}
}

void _CFlatVertexCache::_SCPMap::GetNextPathToken(SIGNED_COMPONENT_PATH_MULTILIST& GlobalLists,
			SIGNED_COMPONENT_PATH_LIST& CurrentList, 
			const SIGNED_COMPONENT_PATH& Next)
{
	//const SIGNED_COMPONENT_PATH& Next=CurIt->second;
	if(Next==NULL_PATH)
		return;
	pair<iterator,iterator> Range=equal_range(Next);
	if(Range.first==end())
		return;
	const SIGNED_COMPONENT_PATH& MainCurrent=Range.first->first;
	const SIGNED_COMPONENT_PATH& MainNext=Range.first->second;
	Range.first++;
	for(iterator it=Range.first;it!=Range.second;it++)
	{
		GlobalLists.push_back(CurrentList);
		SIGNED_COMPONENT_PATH_LIST& LocalCurrentList=GlobalLists.back();
		const SIGNED_COMPONENT_PATH& LocalCurrent=it->first;
		const SIGNED_COMPONENT_PATH& LocalNext=it->second;
		LocalCurrentList.push_back(LocalCurrent);
		GetNextPathToken(GlobalLists,LocalCurrentList,LocalNext);
	}
	CurrentList.push_back(MainCurrent);
	GetNextPathToken(GlobalLists,CurrentList,MainNext);
}

//bool _CFlatVertex::NoZero() const 
//{
//	return (this!=&OneFlatVertex) && (m_0Descendent==NULL || m_0Multiplier==0);
//}

bool _CFlatVertex::IsOnlyOneDescendant(size_t& NonZeroDesc) const
{
	if (this == &OneFlatVertex)
		return false;
	return ::IsOnlyOneDescendant(m_Descendants, NonZeroDesc);
}

void _CFlatVertexCache::PathList2VertList(FLAT_VERT_MULTILIST& Dest,SIGNED_COMPONENT_PATH_MULTILIST& Source)
{
	Dest.clear();
	for(SIGNED_COMPONENT_PATH_MULTILIST::iterator mit=Source.begin(),_me=Source.end();mit!=_me;mit++)
	{
		SIGNED_COMPONENT_PATH_LIST& TopList=*mit;
		Dest.push_back(FLAT_VERT_LIST());
		FLAT_VERT_LIST& CurrList=Dest.back();
		SIGNED_COMPONENT_PATH_LIST AuxList;
		for(SIGNED_COMPONENT_PATH_LIST::iterator it=TopList.begin(),_e=TopList.end();it!=_e;it++)
		{
			const SIGNED_COMPONENT_PATH& TopPath=*it;
			AuxList.push_front(TopPath);
			_CFlatVertex* pCurr1=&OneFlatVertex;
			for(SIGNED_COMPONENT_PATH_LIST::iterator iit=AuxList.begin(),_ie=AuxList.end();iit!=_ie;iit++)
			{
				const SIGNED_COMPONENT_PATH& CurPath=*iit;
				const COMPONENTS_PATH* pPath = get<0>(CurPath);
				short Sgn = get<1>(CurPath);
				short Power = get<2>(CurPath);
				DESCENDANTS DescVector(Power+1);
				MULTIPLIERS MultVector(Power+1);
				DescVector[Power] = pCurr1;
				MultVector[Power] = Sgn;
				_CFlatVertex* pNewVertex = new _CFlatVertex(DescVector, MultVector, *get<0>(CurPath));
				//_CFlatVertex* pNewVertex = new _CFlatVertex(pCurr1, NULL, get<1>(CurPath), 0, *get<0>(CurPath));
				pCurr1 = PushInCache(pNewVertex, true);
			}
			if(pCurr1!=&OneFlatVertex)
			{
				CurrList.push_back(pCurr1);
				TouchVertex(pCurr1);
			}
		}
	}
}

//_CSubModelNumericPattern* _CSubModelNumericParrernStorage::GetPatrernIfExist(const _CModelVerticesPath& Path)
//{
//	COMPONENTS_PATH CPath(Path);
//	const COMPONENTS_PATH& UniqPath=Path.GetBaseCircuit()->GetUnique(CPath);
//	MAP::iterator it=m_Map.find(&UniqPath);
//	return it==m_Map.end()?NULL:&(it->second);
//}
//bool _CSubModelNumericParrernStorage::ForcePattern(const _CModelVerticesPath& Path, _CSubModelNumericPattern*& pOutPattern)
//{
//	COMPONENTS_PATH CPath(Path);
//	const COMPONENTS_PATH& UniqPath=Path.GetBaseCircuit()->GetUnique(CPath);
//	MAP::iterator it=m_Map.find(&UniqPath);
//	bool Res=it==m_Map.end();
//	pOutPattern=&m_Map[&UniqPath];
//	return Res;
//}

_CFactorizedVertexContainer::_CFactorizedVertexContainer(short S,const _CFlatVertex* pVertex,_CMainCircuit* pMainCircuit):
Sgn(S),m_pMainCircuit(pMainCircuit),m_AllreadyFactorized(false) 
{
	if(pVertex!=NULL)
	{
		m_List.push_back(pVertex);
		m_pMainCircuit->TouchFlatVertex(pVertex);
	}
}
_CFactorizedVertexContainer::_CFactorizedVertexContainer(const _CFactorizedVertexContainer& Source):
Sgn(Source.Sgn), m_List(Source.m_List),m_pMainCircuit(Source.m_pMainCircuit) ,m_AllreadyFactorized(Source.m_AllreadyFactorized)
{
	TouchAll();
}
_CFactorizedVertexContainer::_CFactorizedVertexContainer(const _CSExpFlatVertices::_CVertexContainer& Source,_CMainCircuit* pMainCircuit):
Sgn(Source.Sgn),m_pMainCircuit(pMainCircuit),m_AllreadyFactorized(false)
{
	m_List.push_back(Source.pVertex);
	m_pMainCircuit->TouchFlatVertex(Source.pVertex);
	
}

void _CFactorizedVertexContainer::TouchAll()
{
	for(FACTORED_LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		m_pMainCircuit->TouchFlatVertex(*it);
}

void _CFactorizedVertexContainer::UnTouchAll()
{
	for(FACTORED_LIST::iterator it=m_List.begin(),_e=m_List.end();it!=_e;it++)
		m_pMainCircuit->UnTouchFlatVertex(*it);
}

_CFactorizedVertexContainer::~_CFactorizedVertexContainer()
{
	UnTouchAll();
}

bool _CContextSExpFlatVertices::IsContextDetermined(const string* Context)
{
	MAP::iterator it=m_Map.find(Context);
	return it!=m_Map.end();
}

void _CCompPathStorage::PathSt2PathOrdr(COMP_PATH_ORDER& PO) const
{
	PO.clear();
	size_t Counter=0;
	COMP_STG::const_iterator it=m_Stg.begin(),_e=m_Stg.end();
	for(;it!=_e;it++,Counter++)
		PO.insert(COMP_PATH_ORDER::value_type(&*it,Counter));
}

string _CCompPathStorage::ToString() const
{
	string Res;
	for(COMP_STG::const_iterator it=m_Stg.begin();it!=m_Stg.end();it++)
	{
		Res+=(*it).ToString();
		Res+='\n';
	}
	return Res;
}