#pragma once
#include "FlatVertex.h"

class _CAbstractFlatValueCont;

//typedef hash_set<ULONGLONG> TOUCH_VECTOR;
typedef hash_set<const _CAbstractFlatValueCont*> TOUCH_VECTOR;

class _CFlatValueUniqueList;

class _CAbstractFlatValueCont
{
public:
	_CAbstractFlatValueCont()/*:m_Hash(0)*/ {}
	typedef enum{tFlatOneCont,tFlatSingleValueCont,tFlatProductCont,tFlatSumCont} ContType;
	virtual ~_CAbstractFlatValueCont() {} 
	//ULONGLONG GetHash(ULONGLONG Core=HASH_CORE)
	//{
	//	if(m_Hash==0)
	//		m_Hash=CalcHash(Core);
	//	return m_Hash;

	//}
	virtual bool IsEqual(const _CAbstractFlatValueCont* pLeft)=0;
	virtual ContType GetType() const =0;
	virtual void TouchMe(TOUCH_VECTOR& Vector) const {}
	virtual void Entry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const {}
	virtual void Entry2String(string& OutStr,const _CFlatValueUniqueList& UniqueList,bool ReportMyName=true) const {}
	virtual void SubEntry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const;
	virtual void SubEntry2String(string& OutStr, const _CFlatValueUniqueList& UniqueList) const;
	virtual void OneLevelEntry2Stream(txt_stream& stream) const {}
	virtual void OneLevelEntry2String(string& OutStr) const {}
	virtual ULONGLONG GetHash(ULONGLONG Core=HASH_CORE)=0;
protected:
	//ULONGLONG m_Hash;
	friend class _CFlatProductCont;
	friend class _CFlatSumCont;
};

typedef pair<short,_CAbstractFlatValueCont*> SIGNED_TERM;

void MultiplyByScalar(SIGNED_TERM& Vector, short Scalar);

#define ZERO_TERM SIGNED_TERM(0,NULL)

class _CFlatSingleValueCont;
class _CFlatProductCont;
class _CFlatSumCont;

class _CFlatValueUniqueList;
class _CFlatResultDevCache
{
public:
	_CFlatResultDevCache(_CFlatValueUniqueList& FlatValueUniqueList):m_FlatValueUniqueList(FlatValueUniqueList) {}
	bool Register(const _CFlatVertex* pVertex, SIGNED_TERM Result);
	SIGNED_TERM GetRegistered(const _CFlatVertex* pVertex);
	bool GetRegistered(short& Sgn,_CFlatProductCont& Product,const _CFlatVertex* pVertex);
	bool Register(const _CFlatVertex* pVertex, short Sgn,const _CFlatProductCont& Product);
	void Clear() {m_Map.clear();}
protected:
	typedef hash_map<const _CFlatVertex*, SIGNED_TERM> MAP;
	MAP m_Map;
	_CFlatValueUniqueList& m_FlatValueUniqueList;
};

class _CFlatValueUniqueList
{
public:
	_CFlatValueUniqueList():m_Counter(2) {}
	~_CFlatValueUniqueList();
	_CFlatSingleValueCont* GetUnique(_CFlatSingleValueCont*& pNewSource);
	_CFlatProductCont* GetUnique(_CFlatProductCont*& pNewSource);
	_CFlatSumCont* GetUnique(_CFlatSumCont*& pNewSource);
	ULONGLONG GetId(const _CAbstractFlatValueCont* pCont) const;//0= invalid
	//_CAbstractFlatValueCont* GetPtr(ULONGLONG Id);
	void ClearGarbage(TOUCH_VECTOR& Used);
	void List2Stream(txt_stream& stream) const;
	_CFlatSingleValueCont* Path2ValueCont(const COMPONENTS_PATH* pPath, size_t Power);
protected:
	//typedef list<_CAbstractFlatValueCont*> PTR_LIST;
	//PTR_LIST m_List;
	_CAbstractFlatValueCont* GetUnique(_CAbstractFlatValueCont** ppNewSource);
	typedef hash_multimap<ULONGLONG,_CAbstractFlatValueCont*> MAP;
	MAP m_Map;
	typedef hash_map<const _CAbstractFlatValueCont*,ULONGLONG> ORDER_MAP;
	ULONGLONG m_Counter;
	ORDER_MAP m_OrderMap;
	typedef hash_map<ULONGLONG,_CAbstractFlatValueCont*> ORDER_MAP2;
	ORDER_MAP2 m_OrderMap2;
	friend class _CTraitor;
	friend class _CFlatSumCont;
};


class _CFlatOneCont : public _CAbstractFlatValueCont
{
public:
	//virtual ULONGLONG CalcHash(ULONGLONG Core=HASH_CORE) override;
	virtual ULONGLONG GetHash(ULONGLONG Core=HASH_CORE) override;
	virtual bool IsEqual(const _CAbstractFlatValueCont* pLeft) override;
	virtual ContType GetType() const override {return tFlatOneCont; }
	virtual void SubEntry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const override;
	virtual void SubEntry2String(string& OutStr, const _CFlatValueUniqueList& UniqueList) const override
	{
		OutStr+="1";
	}
	virtual void OneLevelEntry2Stream(txt_stream& stream) const override
	{
		stream<<1;
	}
	virtual void OneLevelEntry2String(string& OutStr) const override 
	{ 
		OutStr+="1";
	}
};

class _CFlatSingleValueCont : public _CAbstractFlatValueCont
{
public:
	_CFlatSingleValueCont(const COMPONENTS_PATH* Value, size_t Power):m_Value(Value), m_Power(Power) {}
	_CFlatSingleValueCont(const _CFlatSingleValueCont& Source): m_Value(Source.m_Value), m_Power(Source.m_Power) {}
	virtual ~_CFlatSingleValueCont() {}
	virtual ULONGLONG GetHash(ULONGLONG Core=HASH_CORE) override;
	virtual bool IsEqual(const _CAbstractFlatValueCont* pLeft) override;
	virtual ContType GetType() const override {return tFlatSingleValueCont; }
	virtual void TouchMe(TOUCH_VECTOR& Vector) const override;
	virtual void SubEntry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const override;
	virtual void SubEntry2String(string& OutStr, const _CFlatValueUniqueList& UniqueList) const override;
	virtual void OneLevelEntry2Stream(txt_stream& stream) const override;
	virtual void OneLevelEntry2String(string& OutStr) const override;
protected:
	const COMPONENTS_PATH* m_Value;
	size_t m_Power;
};

class _CFlatMultTraitor
{
public:
	_CFlatMultTraitor(_CFlatValueUniqueList& FlatValueUniqueList):m_FlatValueUniqueList(FlatValueUniqueList) {}
	bool operator()(_CAbstractFlatValueCont* pLeft,_CAbstractFlatValueCont* pRight)
	{
		return m_FlatValueUniqueList.GetId(pLeft)>m_FlatValueUniqueList.GetId(pRight);
	}
protected:
	_CFlatValueUniqueList& m_FlatValueUniqueList;
};

class _CFlatProductCont : public _CAbstractFlatValueCont
{
public:
	_CFlatProductCont() {}
	_CFlatProductCont(_CAbstractFlatValueCont* pSource):
		m_List(pSource->GetType()==tFlatProductCont?
			dynamic_cast<_CFlatProductCont*>(pSource)->m_List:
			PTR_LIST(1,pSource)) {}
	_CFlatProductCont(const _CFlatProductCont& Source):m_List(Source.m_List) {}
	_CFlatProductCont(const _CFlatProductCont& Source,_CAbstractFlatValueCont* pMultiplier,_CFlatMultTraitor& Traitor);
	_CFlatProductCont(_CAbstractFlatValueCont* pLeft,_CAbstractFlatValueCont* pRight,_CFlatMultTraitor& Traitor);
	//_CFlatProductCont() {}
	_CFlatProductCont(_CFlatSingleValueCont* pSingleValue) : m_List(PTR_LIST(1,pSingleValue))
	{
		//m_List.push_front(pSingleValue);
	}
	_CFlatProductCont& MultplyBy(_CAbstractFlatValueCont* pMultiplier,_CFlatMultTraitor& Traitor);
	virtual ~_CFlatProductCont() {}
	//virtual ULONGLONG CalcHash(ULONGLONG Core=HASH_CORE) override;
	virtual ULONGLONG GetHash(ULONGLONG Core=HASH_CORE) override;
	virtual bool IsEqual(const _CAbstractFlatValueCont* pLeft) override;
	virtual ContType GetType() const override {return tFlatProductCont; }
	virtual void TouchMe(TOUCH_VECTOR& Vector) const override;
	virtual void Entry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const override;
	virtual void Entry2String(string& OutStr,const _CFlatValueUniqueList& UniqueList,bool ReportMyName=true) const override;
	virtual void OneLevelEntry2Stream(txt_stream& stream) const override;
	virtual void OneLevelEntry2String(string& OutStr) const override;
protected:
	typedef list<_CAbstractFlatValueCont*> PTR_LIST;
	PTR_LIST m_List;
	friend class _CFlatSumCont;
	friend class _CFlatResultFactorizer;
	friend class _CFlatResultDevCache;
	_CFlatProductCont(PTR_LIST& Left, _CAbstractFlatValueCont* pRight,_CFlatMultTraitor& Traitor);
	_CFlatProductCont(PTR_LIST& List):m_List(List) {}
};

class _CFlatSumTraitor
{
public:
	_CFlatSumTraitor(_CFlatValueUniqueList& FlatValueUniqueList):m_FlatValueUniqueList(FlatValueUniqueList) {}
	bool operator()(SIGNED_TERM Left,SIGNED_TERM Right)
	{
		return m_FlatValueUniqueList.GetId(Left.second)>m_FlatValueUniqueList.GetId(Right.second);
	}
protected:
	_CFlatValueUniqueList& m_FlatValueUniqueList;
};

class _CFlatSumCont : public _CAbstractFlatValueCont
{
public:
	//_CFlatSumCont()/*:m_ParentisesCounter(0)*/ {}
	virtual ~_CFlatSumCont() {}
	//virtual ULONGLONG CalcHash(ULONGLONG Core=HASH_CORE) override;
	virtual ULONGLONG GetHash(ULONGLONG Core=HASH_CORE) override;
	virtual bool IsEqual(const _CAbstractFlatValueCont* pLeft) override;
	virtual ContType GetType() const override {return tFlatSumCont; }
	virtual void TouchMe(TOUCH_VECTOR& Vector) const override;
	virtual void Entry2Stream(txt_stream& stream,const _CFlatValueUniqueList& UniqueList) const override;
	virtual void Entry2String(string& OutStr,const _CFlatValueUniqueList& UniqueList,bool ReportMyName=true) const override;
	virtual void OneLevelEntry2Stream(txt_stream& stream) const override;
	virtual void OneLevelEntry2String(string& OutStr) const override;
protected:
	friend class _CFlatResultFactorizer;
	//typedef pair<short,_CAbstractFlatValueCont*> TERM;
	typedef list<SIGNED_TERM> PTR_LIST;
	void MultiplyByScalar(PTR_LIST& List, short Scalar);
	PTR_LIST m_List;
	//short m_SgnMult;
	//_CFlatSumCont(PTR_LIST& Left, PTR_LIST& Right,_CFlatResultFactorizer& Factorizer);
	//_CFlatSumCont(_CAbstractFlatValueCont* pLeft,_CAbstractFlatValueCont* pRight, short RightSgn,
	//	_CFlatResultFactorizer& Factorizer);
	_CFlatSumCont(SIGNED_TERM Left, SIGNED_TERM ight, _CFlatSumTraitor& SumTraitor);
	//size_t m_ParentisesCounter;
};

class _CCofactor;

class _CFlatResultFactorizer
{
public:
	typedef pair<short,ULONGLONG> SIGNED_ID;
	_CFlatResultFactorizer(const _CCompPathStorage& PathStorage,const _CFlatVertexCache& FlatVertexCache):m_PathStorage(PathStorage),
		m_FlatVertexCache(FlatVertexCache),m_ResMap(m_ValueUniqueList),
		/*m_MaxCofId(0),m_MaxSPower(0),*/m_MultTraitor(m_ValueUniqueList), 
		m_SumTraitor(m_ValueUniqueList),m_ResDevCache(m_ValueUniqueList),
		m_Manager(m_ValueUniqueList,m_ResMap)
	{
		//PathStorage.PathSt2PathOrdr(m_PathOrder);
	}
	void Init();
	bool NotifyFactorizedValue(size_t CofId,size_t sPower, const _CFlatVertex* pVertex, short GlobalSgn);
	void ClearGarbage();
	void SoE2Stream(txt_stream& stream) const {m_ValueUniqueList.List2Stream(stream);}
	void Regular2Stream(txt_stream& stream) ;
	SIGNED_ID GetValueId(size_t CofId,size_t sPower) const
	{
		return m_ResMap.GetValueId(CofId,sPower);
	}
	SIGNED_TERM GetTerm(size_t CofId,size_t sPower) const
	{
		return m_ResMap.GetTerm(CofId,sPower);
	}
	void SubEntry2Stream(const _CAbstractFlatValueCont* pContener,txt_stream& stream) const
	{
		pContener->SubEntry2Stream(stream,m_ValueUniqueList);
	}
	void SubEntry2Stream1Level(const _CAbstractFlatValueCont* pContener,txt_stream& stream) const
	{
		pContener->OneLevelEntry2Stream(stream);
	}
	size_t DefineTransferFunctionSimp(const string& Name, size_t NumeratorCof, const _CCofactor* pNumCof, 
		size_t DenominatorCof, const _CCofactor* pDenCof)
	{
		return m_Manager.DefineTransferFunctionSimp(Name,NumeratorCof,pNumCof,DenominatorCof,pDenCof);
	}
	bool AddExtraTransferFunctionSimp(size_t PairId, size_t AuxCof, const _CCofactor* pAuxCof);
	void PrepareTrasferFuctions() {m_Manager.PrepareTrasferFuctions(); }
	void WriteTransferFunctions(txt_stream& stream) {m_Manager.ToStream(stream);}
	void WriteTransferFunction(size_t TrId, txt_stream& stream) {m_Manager.ToStream(TrId,stream);}
	bool TranslateTrans2DenAndNum(size_t FunctionNo, size_t& Num, size_t& Den)
	{
		return m_Manager.TranslateTrans2DenAndNum(FunctionNo, Num, Den);
	}
	const string& FuncName(size_t FunctionId) { return m_Manager.Name(FunctionId); }
protected:
	void GetFactorizedValue(short& Sgn,_CFlatProductCont& OutRes,const _CFlatVertex* pVertex);
	//SIGNED_TERM GetFactorizedValue(const _CFlatVertex* pVertex);
	SIGNED_TERM MultBySingleValue(SIGNED_TERM InputValue, SIGNED_TERM SimpMultiplier);
	SIGNED_TERM Add(SIGNED_TERM Left,SIGNED_TERM Right);
	void Add(short& MainSgn,_CFlatProductCont& MainTerm, short AuxSgn, _CFlatProductCont& AuxTerm); 
	//void Abstract2List(_CFlatProductCont::PTR_LIST& List,_CAbstractFlatValueCont* pInputValue);
	void Abstract2List(const _CFlatProductCont::PTR_LIST*& pList,_CAbstractFlatValueCont* pInputValue,_CFlatProductCont::PTR_LIST& Aux);
	//void ExtractCF(_CFlatProductCont::PTR_LIST& List1,
	//	_CFlatProductCont::PTR_LIST& List2,
	//	_CFlatProductCont::PTR_LIST& Common);
	void ExtractCF(_CFlatProductCont::PTR_LIST& InOutMainList, _CFlatProductCont::PTR_LIST& InOutAuxList,
		_CFlatProductCont::PTR_LIST& OutDiffMainList);
	void ExtractCF(const _CFlatProductCont::PTR_LIST& InList1,
		const _CFlatProductCont::PTR_LIST& InList2,
		_CFlatProductCont::PTR_LIST& OutList1,
		_CFlatProductCont::PTR_LIST& OutList2,
		_CFlatProductCont::PTR_LIST& Common);
	COMP_PATH_ORDER m_PathOrder;
	_CFlatResultDevCache m_ResDevCache;
	_CFlatValueUniqueList m_ValueUniqueList;
	typedef map<ULONGLONG, const _CAbstractFlatValueCont*> AUX_LIST;
	//AUX_LIST m_AuxList;
	class _CResMap
	{
	public:
		typedef map<size_t,SIGNED_TERM> COF_MAP;
		_CResMap(_CFlatValueUniqueList& ValueUniqueList):
		  m_MaxCofId(0),m_MaxSPower(0),m_ValueUniqueList(ValueUniqueList) {}
		SIGNED_TERM& GetTerm(size_t CofId,size_t SPower);
		void TouchResults(TOUCH_VECTOR& Vector);
		void PrepareAuxList(AUX_LIST& AuxList) const;
		SIGNED_ID GetValueId(size_t CofId,size_t sPower) const;
		SIGNED_TERM GetTerm(size_t CofId,size_t sPower) const;
		void GetCofCoeffs(size_t CofId,COF_MAP& OutMap) const;
	protected:
		typedef pair<size_t,size_t> ENTRY;
		typedef map<ENTRY,SIGNED_TERM> RES_MAP;
		RES_MAP m_ResMap;
		size_t m_MaxCofId;
		size_t m_MaxSPower;
		_CFlatValueUniqueList& m_ValueUniqueList;
	};
	_CResMap m_ResMap;
	friend class _CFlatSumCont;
	const _CCompPathStorage& m_PathStorage;
	void PrepareAuxList(AUX_LIST& AuxList) const 
	{
		m_ResMap.PrepareAuxList(AuxList);
	}
	//void ClearAuxList() {m_ResMap.clear();}
	_CFlatMultTraitor m_MultTraitor;
	_CFlatSumTraitor m_SumTraitor;
	const _CFlatVertexCache& m_FlatVertexCache;
	class _CCofResCont
	{
	public:
		_CCofResCont(_CFlatValueUniqueList& FlatValueUniqueList):m_FlatValueUniqueList(FlatValueUniqueList) {}
		void CopyAndTranslate(_CResMap::COF_MAP& CofMap);
		bool GetComStr(string& Str) const;
		bool GetSExpStr(string& Str) const;
		_CFlatProductCont m_CommonFactor;
	protected:
		void FindCommonFactor();
		ULONGLONG GetId(const _CAbstractFlatValueCont* pCont) {return m_FlatValueUniqueList.GetId(pCont); }
		typedef pair<short,_CFlatProductCont> SIGNED_MULT;
		typedef map<size_t,SIGNED_MULT,greater<size_t>> SPOWER_COEFFS;
		SPOWER_COEFFS m_SPowerCoeffs;
		_CFlatValueUniqueList& m_FlatValueUniqueList;
		class _CAuxCont
		{
		public:
			_CAuxCont(_CFlatProductCont::PTR_LIST& List,
				_CFlatValueUniqueList& FlatValueUniqueList):m_List(List),
				m_FlatValueUniqueList(FlatValueUniqueList)
			{
				m_It=m_List.begin();
				m_e=m_List.end();
				NextCurId();
			}
			_CAuxCont(const _CAuxCont& Source):m_List(Source.m_List), 
				m_FlatValueUniqueList(Source.m_FlatValueUniqueList)
			{
				m_It=m_List.begin();
				m_e=m_List.end();
				NextCurId();
			}
			void Move2Next()
			{
				m_It++;
				NextCurId();
			}
			void RemoveCurrent()
			{
				m_It=m_List.erase(m_It);
				NextCurId();
			}
			ULONGLONG FindId(ULONGLONG Id2Find)
			{
				while(m_CurrId>Id2Find)
					Move2Next();
				return m_CurrId;
			}
			ULONGLONG GetCurrentId() const {return m_CurrId;}
			_CAbstractFlatValueCont* GetCurrentFactor() const { return *m_It;}
		protected:
			ULONGLONG m_CurrId;
			_CFlatProductCont::PTR_LIST& m_List;
			_CFlatProductCont::PTR_LIST::iterator m_It,m_e;
			_CFlatValueUniqueList& m_FlatValueUniqueList;
			void NextCurId()
			{
				m_CurrId=(m_It)==m_e?0:m_FlatValueUniqueList.GetId(*m_It);
			}
		};
		typedef list<_CAuxCont> AUX_LIST;
		_CAuxCont* PrepareAuxList(AUX_LIST& List);
	};
	friend class _CCofResCont;
	class _CTransferFunctPair
	{
	public:
		_CTransferFunctPair(const string& Name, size_t DenCofId, const _CCofactor* pDenCof,
			size_t NumCofId, const _CCofactor* pNumCof, _CFlatValueUniqueList& FlatValueUniqueList):m_Name(Name), m_DenCofId(DenCofId), 
			m_pDenCof(pDenCof),m_NumCofId(NumCofId),m_pNumCof(pNumCof),m_FlatValueUniqueList(FlatValueUniqueList),
			Den(FlatValueUniqueList), Num(FlatValueUniqueList) {}
		void PrepareTrasferFuctions(_CFlatResultFactorizer::_CResMap& ResMap);
		void ToStream(txt_stream& stream);
		void GetNumAndCoffId(size_t& Num, size_t& Den) 
		{
			Num = m_NumCofId;
			Den = m_DenCofId;
		}
		const string& Name() const { return m_Name; }
	protected:
		//friend class _CCofResCont;
		string m_Name;
		size_t m_DenCofId, m_NumCofId;
		_CCofResCont Den,Num;
		const _CCofactor* m_pDenCof;
		const _CCofactor* m_pNumCof;
		_CFlatValueUniqueList& m_FlatValueUniqueList;
		void ReduceCommonFactors();
	};
	friend class _CTransferFunctPair;
	class _CFunctionPairsManager
	{
	public:
		_CFunctionPairsManager(_CFlatValueUniqueList& FlatValueUniqueList,_CFlatResultFactorizer::_CResMap& ResMap):m_Counter(0x10),
			m_FlatValueUniqueList(FlatValueUniqueList),m_ResMap(ResMap) {}
		size_t DefineTransferFunctionSimp(const string& Name,size_t NumeratorCof, const _CCofactor* pNumCof, 
			size_t DenominatorCof, const _CCofactor* pDenCof);
		void PrepareTrasferFuctions();
		void ToStream(txt_stream& stream);
		void ToStream(size_t TrId,txt_stream& stream);
		bool TranslateTrans2DenAndNum(size_t FunctionNo, size_t& Num, size_t& Den);
		const string& Name(size_t FunctionNo) const;
	protected:
		typedef hash_map<size_t,_CTransferFunctPair> FUNCTION_PAIRS;
		FUNCTION_PAIRS m_FunctionPairs;
		size_t m_Counter;
		_CFlatValueUniqueList& m_FlatValueUniqueList;
		//_CFlatResultFactorizer::RES_MAP& m_ResMap;
		_CFlatResultFactorizer::_CResMap& m_ResMap;
	};
	friend class _CFunctionPairsManager;
	_CFunctionPairsManager m_Manager;
};

