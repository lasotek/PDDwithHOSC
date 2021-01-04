#pragma once
#include "Components.h"
#include <hash_map>
#include "norm_float.h"
#include "binary_filer.h"
#include "CompRedefiner.h"
class _CSubCircuit;
//class _CRedefParamValues: public hash_map<const _CComponent*,NumericType> 
//{
//public:
//	long long GetHush(long long Core,_CSubCircuit* pOwner) const;
//	void Store(_binary_filer& Filer,_CSubCircuit* pOwner) const; 
//	void Load(_binary_filer& Filer,_CSubCircuit* pOwner); 
//	void WriteToText(iostream& stream);
//	//friend _binary_filer& operator<<(_binary_filer& filer, const _CRedefParamValues& Value);
//	//friend _binary_filer& operator>>(_binary_filer& filer, const _CRedefParamValues& Value);
//};
class _CPathRedefParamValues : public hash_map<string, NumericType>
{
public:
	long long GetHush(long long Core, _CSubCircuit* pOwner) const;
	void Store(_binary_filer& Filer, _CSubCircuit* pOwner) const;
	void Load(_binary_filer& Filer, _CSubCircuit* pOwner);
	void WriteToText(iostream& stream);
	//friend _binary_filer& operator<<(_binary_filer& filer, const _CRedefParamValues& Value);
	//friend _binary_filer& operator>>(_binary_filer& filer, const _CRedefParamValues& Value);
};

typedef hash_map<string, _CSubModelNumericPattern> PATH_2_NUM_PAT;

class _CSparsePolynomialCache
{
public:
	_CSparsePolynomialCache():m_pOwner(NULL),m_PrevShifterPos(-1), m_EOFPos(-1), m_CounterPos(-1) ,m_pFilePath(NULL) {}
	~_CSparsePolynomialCache() {}
	_CSubModelNumericPattern& GetExisting(const string* Context, const _CCompRedefEntry* pRedefParams);
	//_CSubModelNumericPattern& GetExisting(const string* Context, const _CRedefParamValues& RedefParams, _CSubCircuitSocket*& pOriginalSocket);
	//void Register(const _CRedefParamValues& RedefParams,const _CSubModelNumericPattern* Polynomial);
	//void NotifyReadyNew(const string* Context, const _CRedefParamValues& RedefParams);
	void NotifyReadyNew(const string* Context, const _CCompRedefEntry* pRedefParams);
	void Load(_binary_filer& Filer,_CCompRedefEntry& RedefEntry);
	void Store(_binary_filer& Filer);
	void clear() 
	{
		m_Map2.clear();
		//m_Map.clear();
	}
	void WriteIntermediateNumericPolynomials(iostream& stream);
protected:
	friend class  _CCircuit;
	void InitLibrary();
	friend class _CSubCircuit;
	void SetOwner(_CSubCircuit* pOwner) {m_pOwner=pOwner; }
	//typedef pair<_CRedefParamValues,const _CSubModelNumericPattern*> MAIN_DATA;
	//class DATA
	//{
	//public:
	//	DATA():m_Context(NULL),m_pOwnerSocket(NULL) {}
	//	DATA(const string* Context,
	//		const _CRedefParamValues& RedefParamValues,_CSubCircuitSocket* pOwnerSocket):m_Context(Context),
	//		m_RedefParamValues(RedefParamValues),m_pOwnerSocket(pOwnerSocket) {}
	//	DATA(const DATA& Source):m_Context(Source.m_Context), m_RedefParamValues(Source.m_RedefParamValues),
	//		m_SubModelNumericPattern(Source.m_SubModelNumericPattern),m_pOwnerSocket(Source.m_pOwnerSocket) {}
	//	const string*& GetContext() {return m_Context;}
	//	const _CRedefParamValues& GetRedefParamValues() {return m_RedefParamValues;}
	//	_CSubModelNumericPattern& GetSubModelNumericPattern() {return m_SubModelNumericPattern;}
	//	void Load(_binary_filer& Filer,_CSubCircuit* pOwner);
	//	void Store(_binary_filer& Filer,_CSubCircuit* pOwner);
	//	void WriteIntermediateNumericPolynomials(iostream& stream);
	//	_CSubCircuitSocket* GetOriginalSocket() {return m_pOwnerSocket;}
	//protected:
	//	const string* m_Context;
	//	_CRedefParamValues m_RedefParamValues;
	//	_CSubModelNumericPattern m_SubModelNumericPattern;
	//	_CSubCircuitSocket* m_pOwnerSocket;
	//};
	class DATA2
	{
	public:
		DATA2() :m_Context(nullptr), m_pRedefParamValues(nullptr) {}
		DATA2(const string* Context, const _CCompRedefEntry* pRedefParamValues) : m_Context(Context),
			m_pRedefParamValues(pRedefParamValues) {}
		DATA2(const DATA2& Source) :m_Context(Source.m_Context), m_pRedefParamValues(Source.m_pRedefParamValues),
			m_SubModelNumericPattern(Source.m_SubModelNumericPattern) {}
		const string*& GetContext() { return m_Context; }
		const _CCompRedefEntry* GetRedefParamValues() { return m_pRedefParamValues; }
		void Load(_binary_filer& Filer, _CSubCircuit* pOwner,_CCompRedefEntry& CompRedefEntry);
		void Store(_binary_filer& Filer, _CSubCircuit* pOwner);
		void WriteIntermediateNumericPolynomials(iostream& stream);
		_CSubModelNumericPattern& GetSubModelNumericPattern() {return m_SubModelNumericPattern;}
	protected:
		const string* m_Context;
		const _CCompRedefEntry* m_pRedefParamValues;//na wskaz
		_CSubModelNumericPattern m_SubModelNumericPattern;
	};
	//typedef pair<_CRedefParamValues,_CSubModelNumericPattern> MAIN_DATA;
	//typedef hash_multimap<long long, MAIN_DATA> MAP;
	//typedef hash_multimap<long long, DATA> MAP;
	//MAP m_Map;
	typedef hash_multimap<long long, DATA2> MAP2;
	MAP2 m_Map2;
	_CSubCircuit* m_pOwner;
	//MAP::iterator GetDATA(const string* Context, const _CRedefParamValues& RedefParams);
	MAP2::iterator GetDATA(const string* Context, const _CCompRedefEntry* pRedefParams);
	//void RegisterPresInFile(_binary_filer& Filer, MAP::iterator& it);
	void RegisterPresInFile(_binary_filer& Filer, MAP2::iterator& it);
	long long m_PrevShifterPos;
	long long m_EOFPos;
	long long m_CounterPos;
	const mstr* m_pFilePath;
};

