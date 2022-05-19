#pragma once
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <TCHAR.H>
#include "PrjException.h"
#ifdef _UNICODE
#define  mstr wstring
#else
#define mstr string
#endif

extern const char* DSP;

extern const char* UEF;

extern const char* WFF;


unsigned long stream_key(const char* Name);
#define _ID_STREAM(Name) (#Name)
//#define INIT_STREAMABLE(ClassName)\
//{\
//	m_KeyId=stream_key(#ClassName);\
//}

#define DECLARE_CLASS_NAME(ClassName)\
	virtual const char* GetClassNameId() const {return #ClassName;}
//	virtual const char* GetClassNameId() const\
//{\
//	return typeid(this).raw_name();\
//}

constexpr auto MAX_STREAMABLE = 30;

#define INIT_STREAMABLE(ClassName) _streamable(#ClassName)

#define DECLARE_DYNAMIC_CREATION(ClassName) \
	static _streamable* GetObject##ClassName();\
	DECLARE_CLASS_NAME(ClassName);\
	class RR##ClassName\
	{\
	public:\
		RR##ClassName()\
		{\
			_streamable::ManageClasses(S_OPER::o_Register,#ClassName,ClassName::GetObject##ClassName);\
		}\
	};\
	static RR##ClassName m_RR##ClassName;

#define IMPLEMENT_DYNAMIC_CREATION(ClassName) \
	_streamable* ClassName::GetObject##ClassName()\
		{return dynamic_cast<_streamable*> (new ClassName);}\
	ClassName::RR##ClassName ClassName::m_RR##ClassName;

//	
class _streamable;	
typedef _streamable* (*_pCreateObj)();
typedef struct aREG_ITEM{const char* name;_pCreateObj funct;} REG_ITEM;
//typedef pair<string,_pCreateObj> REG_ITEM2;
class _CStreamRegistry : public unordered_map<string,_pCreateObj>
{
public:
	_CStreamRegistry()
	{
	}
	~_CStreamRegistry()
	{
	}
};

typedef unordered_map<const char*,_pCreateObj> REGISTRY;

class _binary_filer;

class _streamable
{
public:
	_streamable() {}
	~_streamable() {}
	bool IsStored() {return m_Stored;}
//	_streamable(const char* sId) {m_KeyId=GetHash(sId);}
protected:
	enum class S_OPER{o_Register, o_FindAllocator};
	virtual void Store(_binary_filer& Filer) 
	{
		m_Stored=true;
	}
	virtual void Load(_binary_filer& Filer) 
	{
		m_Stored=true;
	}
	friend class _binary_filer;
	void _Store(_binary_filer& Filer);
	void _Load(_binary_filer& Filer);
	unsigned long m_KeyId=0;
//	static _CStreamRegistry m_CRegistry;
	static _pCreateObj ManageClasses(S_OPER Oper,string name,_pCreateObj func);
	DECLARE_DYNAMIC_CREATION(_streamable);
	bool m_Stored = false;
};

class _co_streamable
{
public:
	virtual void DoSomeExtras(_streamable* pstrObj)=0;
};

class _CModelHeader
{
public:
	~_CModelHeader()
	{
	}
	_CModelHeader(bool ToWrite);
	bool WrongHeader();
	char Head[4] = {};
	long long InterfaceOff = 0;
	long long NumericOff = 0;
	long long Res1 = 0, Res2 = 0, Res3 = 0, Res4 = 0;
};

class _CNumericHeader
{
public:
	_CNumericHeader():NoOfResults(0) {}
	long long NoOfResults;
	//long long FirstResultOffset;
};

class _CNextNumericHeader
{
public:
	_CNextNumericHeader():NextResultOffset(0) {}
	long long NextResultOffset;
};

class _binary_filer
{
public:
	enum class OPEN_MODE{ o_read = 0, o_write, o_append, o_random, o_temporary };
	_binary_filer() {}
	_binary_filer(OPEN_MODE mode, const mstr& FileName,bool short_mode);
#ifdef _UNICODE
	_binary_filer(OPEN_MODE mode, const string& FileName,bool short_mode);
#endif
	~_binary_filer(void);
	void open(OPEN_MODE mode, const mstr& FileName=_T(""));
	void close();
	void begin() {_fseeki64(m_stream,0,SEEK_SET);}
	void end() {fseek(m_stream,0,SEEK_END);}
#ifdef WIN32
	void set(long long offset) {_fseeki64(m_stream,offset,SEEK_SET);}
	long long length();
	long long pos() {return _ftelli64(m_stream);}
	void inc(long long offset) {_fseeki64(m_stream,offset,SEEK_CUR);}
	void dec(long long offset) {_fseeki64(m_stream,-offset,SEEK_CUR);}
#else
	void set(long offset) {fseek(m_stream,offset,SEEK_SET);}
	long length();
	long pos() {return ftell(m_stream);}
#endif
	//template<typename Type>
	//_binary_filer& operator<<(const Type& Value)
	//{
	//	if(fwrite(&Value,sizeof(Type),1,m_stream)<1)
	//		RISEPDD(eFilerProblem,DSP);
	//	return *this;
	//}
	//template<typename Type>
	//_binary_filer& operator>>(Type& Value)
	//{
	//	if(fread(&Value,sizeof(Type),1,m_stream)<1)
	//		RISEPDD(eFilerProblem,UEF);
	//	return *this;
	//}
	_binary_filer& operator<<(bool b);
	_binary_filer& operator>>(bool& b);
	_binary_filer& operator<<(char i);
	_binary_filer& operator>>(char& i);
	_binary_filer& operator<<(unsigned char i) {return operator<<((int) i);}
	_binary_filer& operator>>(unsigned char& i) {return operator>>(*((int*)&i));}
	_binary_filer& operator<<(short i);
	_binary_filer& operator>>(short& i);
	_binary_filer& operator<<(unsigned short i){return operator<<((short)i);}
	_binary_filer& operator>>(unsigned short& i){return operator>>(*((short*)&i));}
	_binary_filer& operator<<(int i);
	_binary_filer& operator>>(int& i);
	_binary_filer& operator<<(unsigned i){return operator<<((int)i);}
	_binary_filer& operator>>(unsigned& i){return operator>>(*((int*)&i));}
	_binary_filer& operator<<(long i);
	_binary_filer& operator>>(long& i);
	_binary_filer& operator<<(unsigned long i) {return operator<<((long)i);}
	_binary_filer& operator>>(unsigned long& i) {return operator>>(*((long*)&i));}
	_binary_filer& operator<<(long long i);
	_binary_filer& operator>>(long long& i);
	_binary_filer& operator<<(unsigned long long i) {return operator<<((long long)i);}
	_binary_filer& operator>>(unsigned long long& i) {return operator>>(*((long long*)&i));}
	_binary_filer& operator<<(float x);
	_binary_filer& operator>>(float& X);
	_binary_filer& operator<<(double i);
	_binary_filer& operator>>(double& i);
	_binary_filer& operator<<(long double i);
	_binary_filer& operator>>(long double& i);
	_binary_filer& operator<<(_streamable& cc);
	_binary_filer& operator>>(_streamable& cc);
	_binary_filer& operator<<(const string& str);
	_binary_filer& operator<<(const char* Str) {return operator<<(string(Str));}
	_binary_filer& operator>>(string& str);
	_binary_filer& operator<<(const wstring& str);
	_binary_filer& operator<<(const wchar_t* Str) {return operator<<(wstring(Str));}
	_binary_filer& operator>>(wstring& str);
	_binary_filer& operator<<(const _CModelHeader& Header);
	_binary_filer& operator>>(_CModelHeader& Header);
	_binary_filer& operator<<(const _CNumericHeader& Header);
	_binary_filer& operator>>(_CNumericHeader& Header);
	void StoreDynamicClass(_streamable* pClass);
	_streamable* RetriveDynamicClass(_co_streamable* pExtraOperator=NULL);
	bool IsShortMode() {return m_ShortMode;}
	inline bool IsOpen() {return m_State!=STATE::f_close;}
	string Name() {return _PLAIN_STR(m_FileName);}
protected:
	enum class STATE {f_close = 0,f_open_read,f_open_write} ;
	FILE* m_stream{};
	STATE m_State = STATE::f_close;
	mstr m_FileName{};
	bool m_ShortMode{};
};

class _CTempCacheFiler : public _binary_filer
{
public:
	void open(_binary_filer::OPEN_MODE mode, const mstr& FileName);
};
