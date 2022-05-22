#include "StdAfx.h"
#include "RedefParamValues.h"
#include "SubCircuit.h"
#include "MainCircuit.h"

//long long _CRedefParamValues::GetHush(long long Core,_CSubCircuit* pOwner) const
//{
//	for(const_iterator it=begin(),_e=end();it!=_e;it++)
//	{
//		Core*=HASH_FACTOR;
//		Core^=/*(long long)*/pOwner->GetComponentIndex(it->first);
//		Core*=HASH_CORE;
//		Core^=GetTypedHash(Core,it->second);
//	}
//	return Core;
//}
//
//void _CRedefParamValues::Store(_binary_filer& Filer,_CSubCircuit* pOwner) const 
//{
//	size_t _size=size(),_size2=0;
//	long long spos=Filer.pos();
//	Filer<<_size;
//	for(const_iterator it=begin();it!=end();it++)
//	{
//		_size2++;
//		Filer<<pOwner->GetComponentIndex(it->first)<<it->second;
//	}
//	if(_size!=_size2)
//	{
//		long long epos=Filer.pos();
//		Filer.set(spos);
//		Filer<<_size2;
//		Filer.set(epos);
//	}
//}
//
//void _CRedefParamValues::Load(_binary_filer& Filer,_CSubCircuit* pOwner) 
//{
//	clear();
//	size_t _size;
//	Filer>>_size;
//	for(size_t i=0;i<_size;i++)
//	{
//		long lComponent;
//		long double ldValue;
//		Filer>>lComponent>>ldValue;
//		_CComponent* pComponent=pOwner->GetComponentFromIndex(lComponent);
//		insert(value_type(pComponent,ldValue));
//	}
//}
//
//void _CRedefParamValues::WriteToText(iostream& stream)
//{
//	if(empty())
//		stream<<"Default set of parameters\n";
//	else
//		for(iterator it=begin(),_e=end();it!=_e;it++)
//		{
//			stream<<it->first->GetParameterName()<<" has been changed into "<<it->second<<'\n';
//		}
//}

//_binary_filer& operator<<(_binary_filer& filer, const _CRedefParamValues& Value)
//{
//	return filer;
//}
//
//_binary_filer& operator>>(_binary_filer& filer, const _CRedefParamValues& Value)
//{
//	return filer;
//}

//_CSparsePolynomialCache::MAP::iterator _CSparsePolynomialCache::GetDATA(const string* Context, const _CRedefParamValues& RedefParams)
//{
//	long long Hash = GetStrHash(*Context);
//	Hash = RedefParams.GetHush(Hash, m_pOwner);
//	pair<MAP::iterator, MAP::iterator> range = m_Map.equal_range(Hash);
//	for (MAP::iterator it = range.first; it != range.second; it++)
//		if (it->second.GetContext() == Context && it->second.GetRedefParamValues() == RedefParams)
//			return it;
//	return m_Map.end();
//}
_CSparsePolynomialCache::MAP2::iterator _CSparsePolynomialCache::GetDATA(const string* Context, const _CCompRedefEntry* pRedefParams)
{
	long long Hash = GetStrHash(*Context);
	Hash *= HASH_FACTOR;
	Hash ^= (long long)pRedefParams;
	//Hash = RedefParams.GetHash(Hash);
	pair<MAP2::iterator, MAP2::iterator> range = m_Map2.equal_range(Hash);
	for (MAP2::iterator it = range.first; it != range.second; it++)
		if (it->second.GetContext() == Context && it->second.GetRedefParamValues() == pRedefParams)
			return it;
	return m_Map2.end();
}

_CSubModelNumericPattern& _CSparsePolynomialCache::GetExisting(const string* Context,
	const _CCompRedefEntry* pRedefParams)
{
	MAP2::iterator it = GetDATA(Context, pRedefParams);
	if (it == m_Map2.end())
	{
		long long Hash = GetStrHash(*Context);
		Hash *= HASH_FACTOR;
		Hash ^= (long long)pRedefParams;
		//Hash = RedefParams.GetHash(Hash);
		it = m_Map2.insert(MAP2::value_type(Hash, DATA2(Context, pRedefParams)));
	}
	return it->second.GetSubModelNumericPattern();
}

//_CSubModelNumericPattern& _CSparsePolynomialCache::GetExisting(const string* Context,
//	const _CRedefParamValues& RedefParams,
//	_CSubCircuitSocket*& pOriginalSocket)
//{
//	long long Hash = GetStrHash(*Context);
//	Hash = RedefParams.GetHush(Hash, m_pOwner);
//	//pair<MAP::iterator,MAP::iterator> range=m_Map.equal_range(Hash);
//	//for(MAP::iterator it=range.first;it!=range.second;it++)
//	//	if(it->second.GetContext()==Context && it->second.GetRedefParamValues()==RedefParams)
//	//		return it->second.GetSubModelNumericPattern();
//	MAP::iterator it = GetDATA(Context, RedefParams);
//	if (it == m_Map.end())
//		it = m_Map.insert(MAP::value_type(Hash, DATA(Context, RedefParams, pOriginalSocket)));
//	else
//		pOriginalSocket = it->second.GetOriginalSocket();
//	return it->second.GetSubModelNumericPattern();
//}

void _CSparsePolynomialCache::NotifyReadyNew(const string* Context, const _CCompRedefEntry* pRedefParams)
{
	InitLibrary();
	MAP2::iterator it = GetDATA(Context, pRedefParams);
	ASSERTPDD(it != m_Map2.end());
	_binary_filer Filer(_binary_filer::OPEN_MODE::o_random, *m_pFilePath, true);
	Filer.end();
	RegisterPresInFile(Filer, it);
	m_EOFPos = Filer.pos();
	Filer.set(m_CounterPos);
	size_t NoOfRes;
	Filer >> NoOfRes;
	Filer.set(m_CounterPos);
	Filer << ++NoOfRes;
}
//void _CSparsePolynomialCache::NotifyReadyNew(const string* Context, const _CRedefParamValues& RedefParams)
//{
//	InitLibrary();
//	MAP::iterator it = GetDATA(Context, RedefParams);
//	ASSERTPDD(it != m_Map.end());
//	_binary_filer Filer(_binary_filer::o_random, *m_pFilePath, true);
//	Filer.end();
//	RegisterPresInFile(Filer, it);
//	m_EOFPos = Filer.pos();
//	Filer.set(m_CounterPos);
//	size_t NoOfRes;
//	Filer >> NoOfRes;
//	Filer.set(m_CounterPos);
//	Filer << ++NoOfRes;
//}

//void _CSparsePolynomialCache::Register(const _CRedefParamValues& RedefParams,const _CSubModelNumericPattern* Polynomial)
//{
//	//long long Hash=RedefParams.GetHush(HASH_CORE);
//	//m_Map.insert(MAP::value_type(Hash,MAIN_DATA(RedefParams,Polynomial)));
//}

//void _CSparsePolynomialCache::RegisterPresInFile(_binary_filer& Filer,
//	_CSparsePolynomialCache::MAP::iterator& it)
//{
//	m_PrevShifterPos = m_EOFPos;
//	Filer.inc(sizeof(m_PrevShifterPos));
//	Filer << (it->first);
//	it->second.Store(Filer, m_pOwner);
//	long long EOFPos = Filer.pos();
//	Filer.set(m_PrevShifterPos);
//	Filer << EOFPos;
//	m_EOFPos = EOFPos;
//	Filer.set(m_EOFPos);
//}
void _CSparsePolynomialCache::RegisterPresInFile(_binary_filer& Filer,
	_CSparsePolynomialCache::MAP2::iterator& it)
{
	m_PrevShifterPos = m_EOFPos;
	Filer.inc(sizeof(m_PrevShifterPos));
	Filer << (it->first);
	it->second.Store(Filer, m_pOwner);
	long long EOFPos = Filer.pos();
	Filer.set(m_PrevShifterPos);
	Filer << EOFPos;
	m_EOFPos = EOFPos;
	Filer.set(m_EOFPos);
}

void _CSparsePolynomialCache::Load(_binary_filer& Filer, _CCompRedefEntry& RedefEntry)
{
	InitLibrary();
	Filer.set(m_CounterPos);
	size_t NoOfRes = 0;
	Filer >> NoOfRes;
	long long NextPolynomialPos = 0;
	for (size_t i = 0; i<NoOfRes; i++)
	{
		m_PrevShifterPos = Filer.pos();
		Filer >> NextPolynomialPos;
		long long Hash = 0;
		Filer >> Hash;
		DATA2& data = m_Map2.insert(MAP2::value_type(Hash, DATA2()))->second;
		data.Load(Filer, m_pOwner,RedefEntry);
		long long CurPos = Filer.pos();
		ASSERTPDD(CurPos == NextPolynomialPos);
	}
	m_EOFPos = Filer.pos();
}
//void _CSparsePolynomialCache::Load(_binary_filer& Filer)
//{
//	InitLibrary();
//	Filer.set(m_CounterPos);
//	size_t NoOfRes = 0;
//	Filer >> NoOfRes;
//	long long NextPolynomialPos = 0;
//	for (size_t i = 0; i<NoOfRes; i++)
//	{
//		m_PrevShifterPos = Filer.pos();
//		Filer >> NextPolynomialPos;
//		long long Hash = 0;
//		Filer >> Hash;
//		DATA& data = m_Map.insert(MAP::value_type(Hash, DATA()))->second;
//		data.Load(Filer, m_pOwner);
//		long long CurPos = Filer.pos();
//		ASSERTPDD(CurPos == NextPolynomialPos);
//	}
//	m_EOFPos = Filer.pos();
//}

void _CSparsePolynomialCache::Store(_binary_filer& Filer)
{
	InitLibrary();
	Filer.set(m_CounterPos);
	size_t NoOfRes = 0;
	Filer << NoOfRes;
	m_EOFPos = Filer.pos();
	for (MAP2::iterator it = m_Map2.begin(); it != m_Map2.end(); it++, NoOfRes++)
		RegisterPresInFile(Filer, it);
	m_EOFPos = Filer.pos();
	Filer.set(m_CounterPos);
	Filer << NoOfRes;
}
//void _CSparsePolynomialCache::Store(_binary_filer& Filer)
//{
//	InitLibrary();
//	Filer.set(m_CounterPos);
//	size_t NoOfRes = 0;
//	Filer << NoOfRes;
//	m_EOFPos = Filer.pos();
//	for (MAP::iterator it = m_Map.begin(); it != m_Map.end(); it++, NoOfRes++)
//		RegisterPresInFile(Filer, it);
//	m_EOFPos = Filer.pos();
//	Filer.set(m_CounterPos);
//	Filer << NoOfRes;
//}

void _CSparsePolynomialCache::InitLibrary()
{
	if(m_pFilePath==NULL)
		m_pFilePath=&(m_pOwner->GetSubcircuitModelPath());
	if(m_CounterPos==-1)
		m_CounterPos=m_pOwner->GetNumericOffset();
}
const string sDATA="_DATA";
//void _CSparsePolynomialCache::DATA::Load(_binary_filer& Filer,_CSubCircuit* pOwner)
//{
//	string s;
//	Filer>>s;
//	if(s!=sDATA)
//		RISEPDD(eFilerProblem,WFF);
//	Filer>>s;
//	m_Context=pOwner->GetUniqueContextStringName(&s);
//	m_RedefParamValues.Load(Filer,pOwner);
//	Filer>>m_SubModelNumericPattern;	
//}
//
//void _CSparsePolynomialCache::DATA::Store(_binary_filer& Filer,_CSubCircuit* pOwner)
//{
//	Filer<<sDATA;
//	Filer<<*m_Context;
//	m_RedefParamValues.Store(Filer,pOwner);
//	Filer<<m_SubModelNumericPattern;
//}

void _CSparsePolynomialCache::DATA2::Load(_binary_filer& Filer, _CSubCircuit* pOwner, _CCompRedefEntry& CompRedefEntry)
{
	string s;
	Filer >> s;
	if (s != sDATA)
		RISEPDD(eFilerProblem, WFF);
	Filer >> s;
	m_Context = pOwner->GetUniqueContextStringName(&s);
	CompRedefEntry.Load(Filer);
	size_t Id = 0;
	Filer >> Id;
	//m_pRedefParamValues = pOwner->GetMainTopCircuit()->GetComponentRedef().FindFromId(Id);
	//m_pRedefParamValues->Load(Filer);
	pOwner->m_CirCompRedefEntry.Load(Filer);
	Filer >> m_SubModelNumericPattern;
}

void _CSparsePolynomialCache::DATA2::Store(_binary_filer& Filer, _CSubCircuit* pOwner)
{
	Filer << sDATA;
	Filer << *m_Context;
	//Filer << m_pRedefParamValues->GetMyId();
	m_pRedefParamValues->Store(Filer);
	Filer << m_SubModelNumericPattern;
}

void _CSparsePolynomialCache::WriteIntermediateNumericPolynomials(iostream& stream)
{
	for (auto& Entry : m_Map2)
	{
		stream << '\n';
		Entry.second.WriteIntermediateNumericPolynomials(stream);
	}
}

//void _CSparsePolynomialCache::WriteIntermediateNumericPolynomials(iostream& stream)
//{
//	for (MAP::iterator it = m_Map.begin(), _e = m_Map.end(); it != _e; it++)
//	{
//		stream << '\n';
//		it->second.WriteIntermediateNumericPolynomials(stream);
//	}
//}

//void _CSparsePolynomialCache::DATA::WriteIntermediateNumericPolynomials(iostream& stream)
//{
//	if(!m_Context->empty())
//		stream<<"Context: ["<<*m_Context<<"]\n";
//	m_RedefParamValues.WriteToText(stream);
//	m_SubModelNumericPattern.WriteToTextStream(stream);
//}

void _CSparsePolynomialCache::DATA2::WriteIntermediateNumericPolynomials(iostream& stream)
{
	if (!m_Context->empty())
		stream << "Context: [" << *m_Context << "]\n";
	m_pRedefParamValues->WriteToText(stream);
	m_SubModelNumericPattern.WriteToTextStream(stream);
}