// stdafx.cpp : source file that includes just the standard includes
// MainEngineLibrary.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <atlconv.h>
#include <atlbase.h>
#include <hash_map>
#include <limits>

string EmptyString;
string CofString("C;");
string RemString("R;");
string VertexString("V;");
string NumericalString("N;");
string InvalidString("_I_");

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
unsigned long GetStrHash(const char* Str)
{
	return hash_value(Str);
}

unsigned long GetStrHash(const string& Str)
{
	return hash_value(Str);
	//return GetStrHash(Str.c_str());
}

long long GetTypedHash(long long& Core,const string& Str)
{
	return Hash_value(Core,Str);
}

long long GetTypedHash(long long& Core, const long double& ldValue)
{
	union LD{long double ld;long long ll;} lD;
	memset(&lD,0,sizeof(lD));
	lD.ld=ldValue;
	return lD.ll;
}

wstring str2wstr(const string& str)
{
	USES_CONVERSION;
	wstring Res(CA2W(str.c_str()));
	return Res;
}

wstring str2wstr(const wstring& str) 
{
	return str;
}

string wstr2str(const wstring& str)
{
	USES_CONVERSION;
	string Res(CW2A(str.c_str()));
	return Res;
}

string wstr2str(const string& str)
{
	return str;
}

unsigned long long _l2ll(unsigned long Hi, unsigned long Low)
{
	return (unsigned long long)Hi<<(8*sizeof(Hi)) | Low;
}

unsigned long _Lo(unsigned long long  V)
{
	return (unsigned long)V;
}

unsigned long _Hi(unsigned long long  V)
{
	return (unsigned long)(V>>(4*sizeof(V)));
}

unsigned long _compress(unsigned long long V)
{
	return (_Hi(V)<<(4*sizeof(unsigned long)))^_Lo(V);
}
void XOR(long long& L, long long R)
{
	unsigned long Ll=_Lo((unsigned long long) L);
	unsigned long Lh=_Hi((unsigned long long) L);
	unsigned long Rl=_Lo((unsigned long long) R);
	unsigned long Rh=_Hi((unsigned long long) R);
	unsigned long long _Res=_l2ll(Lh ^ Rh, Ll ^ Rl);
	L=(long long)_Res;
}
string long2str(long long Value)
{
	char Buf[20];
	sprintf_s(Buf,sizeof(Buf),"%lld",Value);
	//string Res(Buf);
	return Buf;
}

string ulong2str(unsigned long long Value)
{
	char Buf[20];
	sprintf_s(Buf,sizeof(Buf),"%I64d",Value);
	return Buf;
}
string short2str(short Value)
{
	char Buf[20];
	sprintf_s(Buf,sizeof(Buf),"%hd",Value);
	return Buf;
}

string Mult2Str(short Multiplier)
{
	string Res;
	if(Multiplier==-1)
		Res="(-1)·";
	else if(Multiplier!=1 && Multiplier!=0)
		Res="("+long2str(Multiplier)+")·";
	return Res;
}


long long Hash_value(long long& Core, const string& Str)
{	// hash range of elements
	string::const_iterator _Begin=Str.begin(), _End=Str.end();
	while(_Begin != _End)
		Core = HASH_FACTOR * Core ^ (long long)*_Begin++;
	return (Core);
}

short Max2PowerDiv(short& Value)
{
	short Res = Value < 0 ? -Value : Value;
	if (Res != 1)
		Value /= Res;
	return Res;
	//short Res=1;
	//while((Value & (short)1)==0)
	//{
	//	Value>>=1;
	//	Res<<=1;
	//}
	//return Res;
}

short CommonDivider(short& Left, short& Right)
{
	short a=Left,b=Right,c=a%b;
	while(c!=0)
	{
		a=b;
		b=c;
		c=a%b;
	}
	Left/=b;
	Right/=b;
	return b;
}

void FillStream(txt_stream& stream, size_t Counter, char C)
{
	for(size_t i=0;i<Counter;i++)
		stream<<C;
}

unsigned long long Coordinates2ULL(pair<size_t,size_t> COORDINATES)
{
	if(numeric_limits<size_t>::digits*2<numeric_limits<unsigned long long>::digits)
		return (unsigned long long)(-1);
	unsigned long long Res=0;
	Res|=((unsigned long long)COORDINATES.second<<numeric_limits<size_t>::digits);
	Res|=COORDINATES.first;
	return Res;
}

bool IsInRange(const SIZE_T_RANGE& Range,size_t Value)
{
	return Value>=min(Range.first,Range.second) && Value<=max(Range.first,Range.second);
}

bool _CExchangMapWraper::ExchangeIfNeede(int& Value) const
{

	INT_INT_MAP::const_iterator it=m_Map.find(abs(Value));
	if(it==m_Map.end()) return false;
	Value=it->second*sgn(Value);
	return true;
}
