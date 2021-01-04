// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#if _DEBUG

//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif _DEBUG

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <vector>
#include <map>
#include <hash_map>
#include <stdexcept>
#include <string>
#include <xstring>
#include <deque>
#include <list>
#include <set>
#include <hash_set>
#include <utility>
#include <float.h>
#include <fstream>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <typeinfo.h>
#include <algorithm>
#include <limits>
#include <functional>
#include <tuple>
#include <typeinfo>

// TODO: reference additional headers your program requires here
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
using namespace std;
using namespace stdext;

#define HASH_CORE			2166136261U
#define HASH_FACTOR			16777619U

#define NOVTTABLE _declspec(novtable) 
class _CErrorQue 
{
public:
	_CErrorQue(unsigned MaxDepth=20):m_MaxDepth(MaxDepth)
	{
	
	}
	~_CErrorQue();
	void ResetDepth(unsigned MaxDepth);
	_CErrorQue& operator=(const string& message)
	{
		insert_message(message);
		return *this;
	}
	_CErrorQue& operator=(const char* message)
	{
		insert_message(message);
		return *this;
	}
	void Clear() {m_MainList.clear();}
	string ReportAll();
	string PopMessage();
	operator string() {return PopMessage();}
protected:
	void insert_message(const string& message);
	typedef list<string> T_LIST;
	typedef T_LIST::iterator T_ITERATOR;
	typedef T_LIST::reverse_iterator T_REV_ITERATOR;
	T_LIST m_MainList;
	unsigned m_MaxDepth;
};

long long GetTypedHash(long long& Core,const string& Str);
long long GetTypedHash(long long& Core,const long double& ldValue);
unsigned long GetStrHash(const string& Str);
unsigned long GetStrHash(const char* Str);
extern _CErrorQue LastErrorInfo;
extern const string range_error_msg;
extern const string invalid_engine_msg;
extern const string invalid_circuit_msg;

long long Hash_value(long long& Core, const string& Str);

#ifdef _UNICODE
#define	 path_str wstring
#else
#define	path_str string
#endif

wstring str2wstr(const string& str);
wstring str2wstr(const wstring& str);
#ifdef _UNICODE
#define _PATH_STR(str) str2wstr(str)
#else
#define _PATH_STR(str) str
#endif

string wstr2str(const wstring& str);
string wstr2str(const string& str);

#ifdef _UNICODE
#define _PLAIN_STR(str) wstr2str(str)
#else
#define _PLAIN_STR(str) str
#endif

template<class Type>
inline void ABS(Type& a)
{
	if(a<0) a=-a;
}

extern string EmptyString;
extern string CofString;
extern string RemString;
extern string VertexString;
extern string NumericalString;
extern string InvalidString;

unsigned long long _l2ll(unsigned long Hi, unsigned long Low);
unsigned long _Lo(unsigned long long  V);
unsigned long _Hi(unsigned long long  V);
unsigned long _compress(unsigned long long V);

typedef vector<string> VECTOR_STRING;

string long2str(long long Value);
string ulong2str(unsigned long long Value);
string short2str(short Value);
string Mult2Str(short Multiplier);
void XOR(long long& L, long long R); 
//short sgn(const long double& Value);
short Max2PowerDiv(short Value);

typedef basic_ostream<char, char_traits<char> > txt_stream;

template<class _Kty, class _Pr, class _Alloc >
bool IsIn(const	set<_Kty,_Pr,_Alloc>& SET, const _Kty Key)
{
	return SET.find(Key)!=SET.end();
}

template<class _Kty, class _Pr, class _Alloc >
bool IsIn(const	hash_set<_Kty,_Pr,_Alloc>& SET, const _Kty Key)
{
	return SET.find(Key)!=SET.end();
}

typedef set<size_t> SIZE_T_SET;
//class time_stamp : public fstream
//{
//public:
//
//protected:
//	time_t m_Begining;
//	string m_OperationDescription;
//}
typedef pair<size_t,size_t> SIZE_T_RANGE;
bool IsInRange(const SIZE_T_RANGE& Range,size_t Value);

typedef unsigned long long ULONGLONG;

class _CDescSigns2 : public vector<short>
{
public:
	_CDescSigns2(short N=0):vector<short>(N,0) {}
};

typedef set<unsigned short> SHORT_SET;

short CommonDivider(short& Left, short& Right);

void FillStream(txt_stream& stream, size_t Counter, char C);

unsigned long long Coordinates2ULL(pair<size_t,size_t> COORDINATES);

template<class Type>
class self_exp_vector : public vector<Type>
{
public:
	~self_exp_vector() {}
	self_exp_vector():vector<Type> () {}
	self_exp_vector(size_type _Count):vector<Type> (_Count) {}
	self_exp_vector(size_type _Count, const Type& _Val):vector<Type> (_Count, _Val) {}
	Type& force_at(size_t Index)
	{
		if(Index>=size())
			resize(Index+1,Type());
		return at(Index);
	}
	const Type* smart_at(size_t Index) const
	{
		if(Index>=size())
			return NULL;
		return &at(Index);
	}
	void ClearAll()
	{
		for(iterator it=begin();it!=end();it++)
			(*it).ClearAll();
	}
};

typedef map<int,int> INT_INT_MAP;
typedef multimap<int,int> INT_INT_MULT_MAP;

class _CExchangMapWraper
{
public:
	_CExchangMapWraper(const INT_INT_MAP& Map):m_Map(Map) {}
	bool ExchangeIfNeede(int& Value) const;
protected:
	const INT_INT_MAP& m_Map;
};

template<typename T>
short sgn(const T& Value)
{
	return Value<0?-1:1;
}

typedef vector<unsigned> COMPONENT_PINS;
typedef list<unsigned> COMPONENT_PINS_LIST;
typedef set<unsigned> COMPONENT_PINS_SET;
typedef map<unsigned,unsigned> PINS_COLOCATIONS;
typedef map<int,int> SIGNED_PINS_COLOCATIONS;
typedef pair<unsigned,unsigned> TWO_GRAPH_PIN;
typedef vector<TWO_GRAPH_PIN> TWO_GRAPH_PINS;

template <typename ElementType>
bool IsInList(const list<ElementType>& List, const ElementType& Element)
{
	return find(List.begin(),List.end(),Element)!=List.end();
}


typedef set<int> INT_SET;

template<typename Item>
void Vector2InvertedHashdMap(vector<Item>& Vector,hash_map<Item,size_t>& Map,size_t FirstIndex=0)
{
	for(size_t i=0;i<Vector.size();i++)
		Map.insert(hash_map<Item,size_t>::value_type(Vector[i],FirstIndex+i));
}

typedef list<int> INT_LIST;


template<typename TYPE>
void list_intersec(const list<TYPE>& List1, const list<TYPE>& List2, list<TYPE>& Res)
{
	vector<TYPE> AuxVector;
	AuxVector.resize(min(List1.size(),List2.size()));
	vector<TYPE>::iterator _ResIt=set_intersection(List1.begin(),List1.end(),List2.begin(),List2.end(),AuxVector.begin());
	Res.assign(AuxVector.begin(),_ResIt);
}

template<typename TYPE>
void list_intersec(list<TYPE>& List1Res, const list<TYPE>& List2)
{
	vector<TYPE> AuxVector;
	AuxVector.resize(min(List1Res.size(),List2.size()));
	vector<TYPE>::iterator _ResIt=set_intersection(List1Res.begin(),List1Res.end(),List2.begin(),List2.end(),AuxVector.begin());
	List1Res.assign(AuxVector.begin(),_ResIt);
}

template<typename TYPE>
bool has_intersec(const list<TYPE>& List1, const list<TYPE>& List2)
{
	list<TYPE>::const_iterator it1=List1.begin(),_e1=List1.end();
	list<TYPE>::const_iterator it2=List2.begin(),_e2=List2.end();
	while(it1!=_e1 && it2!=_e2)
	{
		if(*it1<*it2)
			++it1;
		else if(*it1>*it2)
			++it2;
		else
			return true;
	}
	return false;
	//list<TYPE> Intersect;
	//list_intersec(List1,List2,Intersect);
	//return !Intersect.empty();
}


template<typename TYPE>
void list_union(const list<TYPE>& List1, const list<TYPE>& List2, list<TYPE>& Res)
{
	vector<TYPE> AuxVector;
	AuxVector.resize(List1.size()+List2.size());
	vector<TYPE>::iterator _ResIt=set_union(List1.begin(),List1.end(),List2.begin(),List2.end(),AuxVector.begin());
	Res.assign(AuxVector.begin(),_ResIt);
}

template<typename TYPE>
void list_union(list<TYPE>& List1Res, const list<TYPE>& List2)
{
	vector<TYPE> AuxVector;
	AuxVector.resize(List1Res.size()+List2.size());
	vector<TYPE>::iterator _ResIt=set_union(List1Res.begin(),List1Res.end(),List2.begin(),List2.end(),AuxVector.begin());
	List1Res.assign(AuxVector.begin(),_ResIt);
}

template<typename TYPE>
void list_differ(const list<TYPE>& List1, const list<TYPE>& List2, list<TYPE>& Res)
{
	vector<TYPE> AuxVector;
	AuxVector.resize(max(List1.size(),List2.size()));
	vector<TYPE>::iterator _ResIt=set_difference(List1.begin(),List1.end(),List2.begin(),List2.end(),AuxVector.begin());
	Res.assign(AuxVector.begin(),_ResIt);
}

template<typename TYPE>
void list_differ(list<TYPE>& List1Res, const list<TYPE>& List2)
{
	vector<TYPE> AuxVector;
	AuxVector.resize(max(List1Res.size(),List2.size()));
	vector<TYPE>::iterator _ResIt=set_difference(List1Res.begin(),List1Res.end(),List2.begin(),List2.end(),AuxVector.begin());
	List1Res.assign(AuxVector.begin(),_ResIt);
}

template<typename ORD_TYPE>
bool ord_list_remove(list<ORD_TYPE>& List, ORD_TYPE Item)
{
	list<ORD_TYPE>::iterator it=List.begin(),_e=List.end();
	while(it!=_e && *it<Item) it++;
	if(it==_e || *it>Item) return false;
	List.erase(it);
	return true;
}

template<typename IntType>
IntType NWD(IntType A, IntType B)
{
	if (abs(A) < abs(B))
		swap(A, B);
	IntType R = A % B;
	while (R != 0)
	{
		A = B;
		B = R;
		R = A % B;
	}
	return B;
}
template<typename IntType, typename... Rest>
IntType NWD(IntType A, IntType B, Rest... rest)
{
	return NWD(NWD(A, B), rest...);
}

template<typename IntType>
IntType NWD(list<IntType>& List, bool LowestPositive = true)
{
	//list<IntType> Stack(List.begin(), List.end());
	List.remove_if([](short E){return E == 0; });
	if (List.empty())
		return 1;
	short _sgn = LowestPositive ? List.front() : List.back();
	while (List.size() > 1)
	{
		IntType B = abs(List.back());
		List.pop_back();
		IntType A = abs(List.back());
		List.pop_back();
		List.push_back(NWD(A, B));
	}
	return _sgn<0 ? -abs(List.front()) : abs(List.front());
}


//template<typename Item>
//void List2InvertedHashdMap(const list<Item>& List,hash_map<Item,size_t>& Map,size_t FirstIndex=0)
//{
//	for(list<Item>::const_iterator it=List.begin(),_e=List.end();it!=_e;it++,FirstIndex++)
//		Map.insert(hash_map<Item,size_t>::value_type(*it,FirstIndex));
//}
