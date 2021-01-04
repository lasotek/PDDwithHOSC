#include "stdafx.h"
#include "DirectNumCache.h"

bool _CDirectNumContainer::IsEqualTo(const _CAbstractDirectNumContainer& Right)
{
	const _CDirectNumContainer& R=*dynamic_cast<const _CDirectNumContainer*>(&Right);
	return m_Context == R.m_Context && m_pVertex == R.m_pVertex && m_pRedefEntry == R.m_pRedefEntry;
}

long _CDirectNumContainer::DetermineHashKey(long Core)
{
	long long HKey = Core;
	HKey *= HASH_FACTOR;
	HKey ^= (long long)m_Context;
	HKey *= HASH_FACTOR;
	HKey ^= (long long)m_pVertex;
	HKey *= HASH_FACTOR;
	HKey ^= (long long)m_pRedefEntry;
	return (long)HKey;
}


_CDirectNumCache::_CDirectNumCache()
{
	char Buf[30];
	sprintf_s(Buf,30,"%p",this);
	string s=string("_CDirectNumCache")+Buf+".tmp";
	m_PathTempFile=_PATH_STR(s);
	//m_tmp_file.open(_binary_filer::o_temporary,m_PathTempFile);
}

void _CDirectNumCache::ReportHitTest()
{
	char Buf[30];
	sprintf_s(Buf,30,"%p",this);
	string s=string("_CDirectNumCache")+Buf+".txt";
	mstr& FileName=_PATH_STR(s);
	fstream file(FileName.c_str(),ios::out | ios::app);
	typedef map<unsigned long long,unsigned long long> SUMMARIZER;
	SUMMARIZER Summarizer;
	file<<"The winer gets "<<m_MaxAccessCounter<<" accesses"<<endl; 
	for(INDEX2OBJECT_MAP_ITERATOR it=m_Index2ObjectMap.begin();it!=m_Index2ObjectMap.end();it++)
	{
		const long long& Index=it->first;
		_CDirectNumContainer* pCont=dynamic_cast<_CDirectNumContainer*>(it->second);
		long long VIndex=pCont->GetVertexIndex();
		const string& Name=pCont->GetVertexAssociatedName();
		unsigned long HitCounter=pCont->AccessCounter();
		file<<Index<<". VetexId = "<<VIndex<<" ("<< Name<<") hited "<<HitCounter<<" times"<<endl;
		Summarizer[HitCounter]++;
	}
	file<<"Let us sum it up:\n";
	for(SUMMARIZER::iterator it=Summarizer.begin(),_e=Summarizer.end();it!=_e;it++)
	{
		file<<it->second<<" cached values were accessed "<<it->first<<" times\n"; 
	}
}

_CDirectNumContainer* _CDirectNumCache::FindOrRegisterResult(_CDirectNumContainer* pContainer)
{
	if(!m_tmp_file.IsOpen())
		m_tmp_file.open(_binary_filer::o_temporary,m_PathTempFile);
	pContainer->m_p_tmp_file=&m_tmp_file;
	_CDirectNumContainer* pRes= dynamic_cast<_CDirectNumContainer*> (_CAbstractOperationCache<_CSubModelNumericPattern>::FindOrRegisterResult(pContainer));
	if(pRes==pContainer && pContainer->m_UsageCounter==0)
		pRes->CachItToHD();
	return pRes;
}

void _CDirectNumCache::RegisterResult(_CDirectNumContainer* pContainer)
{
	if(!m_tmp_file.IsOpen())
		m_tmp_file.open(_binary_filer::o_temporary,m_PathTempFile);
	pContainer->m_p_tmp_file=&m_tmp_file;
	_CAbstractOperationCache<_CSubModelNumericPattern>::RegisterResult(pContainer);
	if(pContainer->m_UsageCounter==0)
		pContainer->CachItToHD();
}

void _CDirectNumCache::HibernateAll()
{
	for(INDEX2OBJECT_MAP_ITERATOR it=m_Index2ObjectMap.begin(),_e=m_Index2ObjectMap.end();it!=_e;it++)
	{
		dynamic_cast<_CDirectNumContainer*>(it->second)->Hibernate();
	}
}

_CDirectNumContainer& _CDirectNumContainer::operator=(const _CDirectNumContainer& Left)
{
	m_AccessCounter=Left.m_AccessCounter;
	m_pVertex=Left.m_pVertex;
	m_Context=Left.m_Context;
	m_PosInFile=Left.m_PosInFile;
	m_p_tmp_file=Left.m_p_tmp_file;
	m_pRes = Left.m_pRes;
	//if(m_pRes!=NULL)
	//	delete m_pRes;
	//if (Left.m_pRes != NULL)
	//	m_pRes=new _CSubModelNumericPattern(*Left.m_pRes);
	//else
	//	m_pRes=NULL;
	return *this;
}

void _CDirectNumContainer::ReleaseResult()
{
	//m_UsageCounter--;
	if(--m_UsageCounter==0)
		CachItToHD();
}

void _CDirectNumContainer::CachItToHD()
{
	ASSERTPDD(m_pRes!=nullptr);
	if (m_PosInFile < 0)
	{
		m_p_tmp_file->end();
		m_PosInFile = m_p_tmp_file->pos();
		(*m_p_tmp_file) << (*m_pRes);
	}
	m_pRes.reset();
	//delete m_pRes;
	//m_pRes=NULL;
}

void _CDirectNumContainer::Hibernate()
{
	if(m_pRes==nullptr)
		return;
	if(m_PosInFile<0)
	{
		m_p_tmp_file->end();
		m_PosInFile=m_p_tmp_file->pos();
		(*m_p_tmp_file)<<(*m_pRes);
	}
	//delete m_pRes;
	//m_pRes=NULL;
	m_pRes.reset();
	m_UsageCounter=0;
}

const char* FP="Some problems with temporary file";
_CSubModelNumericPattern* _CDirectNumContainer::GetVertexResult() 
{
	if(m_pRes==nullptr)
	{
		//m_pRes = new _CSubModelNumericPattern;
		m_pRes = make_unique<_CSubModelNumericPattern>();
		try
		{
			m_p_tmp_file->set(m_PosInFile);
			(*m_p_tmp_file)>>(*m_pRes);
			//m_UsageCounter++;

		}
		catch(eFilerProblem)
		{
			//delete m_pRes;
			//m_pRes=NULL;
			m_pRes.reset();
			RISEPDD(eFilerProblem,FP);
		}
	}
	m_UsageCounter++;
	return m_pRes.get();
}

//void _CTempCacheFiler::open(_binary_filer::OPEN_MODE mode, const mstr& FileName)
//{
//	_binary_filer::open(mode,FileName);
//}

_CDirNumContainers::~_CDirNumContainers()
{
	for(iterator it=begin(),_e=end();it!=_e;it++)
	{
		_CDirectNumContainer* pContainer=*it;
		if(pContainer!=NULL)
			pContainer->ReleaseResult();
	}
}
