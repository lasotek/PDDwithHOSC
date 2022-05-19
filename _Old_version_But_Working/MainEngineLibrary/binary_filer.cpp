#include "StdAfx.h"
#include "binary_filer.h"
#include <atlconv.h>
#include <atlbase.h>

const char* DSP="Problem with data storing";

const char* UEF="Wrong file format";

//_CStreamRegistry _streamable::m_CRegistry;

_pCreateObj _streamable::ManageClasses(S_OPER Oper,string name,_pCreateObj func)
{
	static _CStreamRegistry CRegistry;
	switch(Oper)
	{
	case S_OPER::o_Register:
		CRegistry.insert(_CStreamRegistry::value_type(name,func));
		return NULL;
	case S_OPER::o_FindAllocator:
		{
			_CStreamRegistry::iterator it=CRegistry.find(name);
			if(it==CRegistry.end())
				RISEPDD(eFilerProblem,UEF);
			return it->second;
		}
	default:
		RISEPDD(eIllegalOperation,"Not proper parameter");
	}
}

unsigned long stream_key(const char* Name) 
{
	return GetStrHash(Name);
}

_CModelHeader::_CModelHeader(bool ToWrite)
{
	if(!ToWrite)
		return;
	//memset(this,0,sizeof(*this));
	Head[0]='P';Head[1]='D';Head[2]='D';Head[3]='M';
}
bool _CModelHeader::WrongHeader()
{
	return Head[0]!='P' || Head[1]!='D' || Head[2]!='D' || Head[3]!='M';
}

_binary_filer::_binary_filer(OPEN_MODE mode, const mstr& FileName, bool short_mode) : m_ShortMode(short_mode)
{
	open(mode, FileName);
}

#ifdef _UNICODE
_binary_filer::_binary_filer(OPEN_MODE mode, const string& FileName,bool short_mode):
m_stream(NULL),m_State(STATE::f_close),m_ShortMode(short_mode)
{
	USES_CONVERSION;
	wstring wFileName(CA2T(FileName.c_str()));
	open(mode,wFileName);
}
#endif
_binary_filer::~_binary_filer(void)
{
	if(IsOpen())
		close();
}
void _binary_filer::open(OPEN_MODE mode, const mstr& FileName)
{
	if(IsOpen())
		close();
	if(FileName.length()!=0)
		m_FileName=FileName;
	else
		if(m_FileName.length()==0)
			RISEPDD(eFilerProblem,"The file should have some name");
	mstr str_mode;
	switch(mode)
	{
	case OPEN_MODE::o_write:str_mode=_T("wb"); break;
	case OPEN_MODE::o_read:str_mode=_T("rb"); break;
	case OPEN_MODE::o_append:str_mode=_T("ab");break;
	case OPEN_MODE::o_random:str_mode=_T("r+bR");break;
	case OPEN_MODE::o_temporary:str_mode=_T("w+bRD");break;
	}
	errno_t t;
#ifdef _UNICODE
	t=_wfopen_s(&m_stream,m_FileName.c_str(),str_mode.c_str());
#else
	t=fopen_s(&m_stream,m_FileName.c_str(),str_mode.c_str());
#endif
	if(t!=0 && mode==OPEN_MODE::o_append)
#ifdef _UNICODE
		t=_wfopen_s(&m_stream,m_FileName.c_str(),_T("wb"));
#else
		t=fopen_s(&m_stream,m_FileName.c_str(),"wb");
#endif
	if(t!=0)
		RISEPDD(eFilerProblem,"Cannot open the file");
	m_State=mode==OPEN_MODE::o_read?STATE::f_open_read:STATE::f_open_write;
}

void _binary_filer::close()
{
	if(IsOpen())
	{
		fflush(m_stream);
		fclose(m_stream);
		m_State=STATE::f_close;
	}
}

#ifdef WIN32
long long _binary_filer::length()
{
	long long p=_ftelli64(m_stream);
	_fseeki64(m_stream,0,SEEK_END);
	long long l=_ftelli64(m_stream);
	_fseeki64(m_stream,p,SEEK_SET);
	return l;
}

#else
long _binary_filer::length()
{
	long p=ftell(m_stream);
	fseek(m_stream,0,SEEK_END);
	long l=ftell(m_stream);
	fseek(m_stream,p,SEEK_SET);
	return l;
}

#endif

_binary_filer& _binary_filer::operator<<(bool b)
{
	if(fwrite(&b,sizeof(b),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(bool& b)
{
	if(fread(&b,sizeof(b),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}

_binary_filer& _binary_filer::operator<<(char i)
{
	if(fwrite(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(char& i)
{
	if(fread(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}
_binary_filer& _binary_filer::operator<<(int i)
{
	if(fwrite(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(int& i)
{
	if(fread(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}

_binary_filer& _binary_filer::operator<<(short i)
{
	if(fwrite(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(short& i)
{
	if(fread(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}

_binary_filer& _binary_filer::operator<<(long i)
{
	if(fwrite(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(long& i)
{
	if(fread(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}

_binary_filer& _binary_filer::operator<<(long long i)
{
	if(fwrite(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(long long& i)
{
	if(fread(&i,sizeof(i),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}
_binary_filer& _binary_filer::operator<<(_streamable& cc)
{
	cc._Store(*this);
	return *this;
}

_binary_filer& _binary_filer::operator>>(_streamable& cc)
{
	cc._Load(*this);
	return *this;
}

_binary_filer& _binary_filer::operator<<(float x)
{
	if(fwrite(&x,sizeof(x),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(float& x)
{
	if(fread(&x,sizeof(x),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}

_binary_filer& _binary_filer::operator<<(double x)
{
	if(fwrite(&x,sizeof(x),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(double& x)
{
	if(fread(&x,sizeof(x),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}

_binary_filer& _binary_filer::operator<<(long double x)
{
	if(fwrite(&x,sizeof(x),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(long double& x)
{
	if(fread(&x,sizeof(x),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}

const long HCS=GetStrHash("CS");
_binary_filer& _binary_filer::operator<<(const string& str)
{
	if(fwrite(&HCS,sizeof(HCS),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	size_t l=str.length();
	if(fwrite(&l,sizeof(size_t),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	if(fwrite(str.c_str(),sizeof(char),l,m_stream)<l)
		RISEPDD(eFilerProblem,DSP);

	return *this;
}
const char* WFF="Wrong file format";
_binary_filer& _binary_filer::operator>>(string& str)
{
	long Test;
	if(fread(&Test,sizeof(Test),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	if(Test!=HCS)
		RISEPDD(eFilerProblem,WFF);
	size_t l;
	if(fread(&l,sizeof(l),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	char* pStr=new char[l+1];
	if(fread(pStr,sizeof(char),l,m_stream)<l)
	{
		delete []pStr;
		RISEPDD(eFilerProblem,UEF);
	}
	pStr[l]=0;
	str=pStr;
	delete []pStr;
	return *this;
}

const long HWS=GetStrHash("WS");
_binary_filer& _binary_filer::operator<<(const wstring& str)
{
	if(fwrite(&HWS,sizeof(HWS),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	size_t l=str.length();
	if(fwrite(&l,sizeof(size_t),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	if(fwrite(str.c_str(),sizeof(wchar_t),l,m_stream)<l)
		RISEPDD(eFilerProblem,DSP);

	return *this;
}
_binary_filer& _binary_filer::operator>>(wstring& str)
{
	long Test;
	if(fread(&Test,sizeof(Test),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	if(Test!=HWS)
		RISEPDD(eFilerProblem,WFF);
	size_t l;
	if(fread(&l,sizeof(l),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	wchar_t* pStr=new wchar_t[l+1];
	if(fread(pStr,sizeof(wchar_t),l,m_stream)<l)
	{
		delete []pStr;
		RISEPDD(eFilerProblem,UEF);
	}
	pStr[l]=0;
	str=pStr;
	delete []pStr;
	return *this;
}

_binary_filer& _binary_filer::operator<<(const _CModelHeader& Header)
{
	if(fwrite(&Header,sizeof(Header),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(_CModelHeader& Header)
{
	if(fread(&Header,sizeof(Header),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}
_binary_filer& _binary_filer::operator<<(const _CNumericHeader& Header)
{
	if(fwrite(&Header,sizeof(Header),1,m_stream)<1)
		RISEPDD(eFilerProblem,DSP);
	return *this;
}

_binary_filer& _binary_filer::operator>>(_CNumericHeader& Header)
{
	if(fread(&Header,sizeof(Header),1,m_stream)<1)
		RISEPDD(eFilerProblem,UEF);
	return *this;
}

void _streamable::_Store(_binary_filer& Filer)
{
	Filer<<GetClassNameId();
	Store(Filer);
}

void _streamable::_Load(_binary_filer& Filer)
{
	string strId;
	Filer>>strId;
	if(strId!=GetClassNameId())
		RISEPDD(eFilerProblem,WFF);
	Load(Filer);
}
IMPLEMENT_DYNAMIC_CREATION(_streamable);


void _binary_filer::StoreDynamicClass(_streamable* pClass)
{
	(*this)<<(*pClass);
}

_streamable* _binary_filer::RetriveDynamicClass(_co_streamable* pExtraOperator)
{
	string strId;
	(*this)>>strId;
	_streamable* pRes=_streamable::ManageClasses(_streamable::S_OPER::o_FindAllocator,strId,NULL)();
	if(pRes==NULL)
		RISEPDD(eFilerProblem,UEF);
	if(pExtraOperator!=NULL)
		pExtraOperator->DoSomeExtras(pRes);
	pRes->Load(*this);
	return pRes;
}

//CRRegistry.insert(REGISTRY::value_type("_streamable",_streamable::GetObject);
void _CTempCacheFiler::open(_binary_filer::OPEN_MODE mode, const mstr& FileName)
{
	_binary_filer::open(mode,FileName);
}
