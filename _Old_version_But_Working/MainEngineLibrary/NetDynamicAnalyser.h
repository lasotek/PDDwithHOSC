#pragma once
#include "norm_float.h"
#include "MainCircuit.h"
#include "SubCircuit.h"
#include "Circuit.h"
#include "TwoPorts.h"

typedef unsigned short MOMENT_ORDER;

//class _CNetCircuit : public _CCircuit
//{
//public:
//	_CNetCircuit():_CCircuit(true,false),Name(NCName),PathName(_PATH_STR(NCName)) {}
//	virtual _CSubCircuit* GetSubcircuit(long Id) override { return NULL;}
//	virtual _CSubCircuit* GetSubcircuit(const string& name) override {return NULL;}
//	virtual long GetCircuitId(const _CCircuit* pCircuit) override {return -1;}
//	virtual bool IsSExpanded() override {return false;}
//	virtual _CCircuit* GetCircuit(long Id) override {return this;}
//	virtual const string& GetName() const override {return Name;}
//	virtual const path_str& GetModelsPath() override {return PathName;}
//	virtual _CSubCircuit* PrepareSubcircuitFromName(const string& Name, const path_str& ModelFile) override {return NULL;}
//	virtual size_t GetOuterVertexIndex(_CModelVertex* pMVertex) override { return 0;}
//	virtual void GetOuterVertex(_CSExpandedVertices*& pVertices, _CModelVertex* pMVertex, _CSExpandedDescendends& TerminalVertices) override {}
//	virtual void WriteHeader(iostream& stream) override {}
//	virtual _CSExpandedVertices* GetUniqueSEExpandedVertices(_CSExpandedVertices& Source) override {return NULL;}
//	virtual const string* GetUniqueContextStringName(const string* String) override {return &EmptyString;}
//	virtual const string& GetUniqueContextStringName(const string& String) override {return EmptyString;}
//	virtual const string& GetGlobalModelFileName() const override {return EmptyString;}
//	virtual const string& GetGlobalCircuitFileName() const override {return EmptyString;}
//	virtual const string& GetGlobalRaportFileName() const override {return EmptyString;}
//	virtual long FindSubId(const string& Name) override {return 0;}
//	//virtual void PrepareNewCircuitAnalyser() override {}
//protected:
//	static char* NCName;
//	string Name;
//	path_str PathName;
//};
class _CNetDynamicAnalyser;

class _CPassiveComponentCont
{
public:
	_CPassiveComponentCont(const string& name,bool ConsiderInDerivative=true):
	  m_ConsiderInDerivative(ConsiderInDerivative),m_name(name),m_pMainCircuit(NULL),m_hTraitor(-1) {}
	virtual ~_CPassiveComponentCont();
	virtual void AddComponent(_CCircuit* pCircuit)=0;
	virtual void CreateCompTraitor(_CMainCircuit* pMainCircuit);
	virtual bool GetDerivative(long CofId,_CSparsePolynomial& Result);
	virtual long double GetValue() {return 0.0;}
protected:
	string m_name;
	bool m_ConsiderInDerivative;
	_CMainCircuit* m_pMainCircuit;
	long m_hTraitor;
	friend class _CPassiveComponentBuff;
};

class _CAbstractPathologicalComponent : public _CPassiveComponentCont
{
public:
	_CAbstractPathologicalComponent():_CPassiveComponentCont(EmptyString,false) {}
	virtual void CreateCompTraitor(_CMainCircuit* pMainCircuit) override {}
	virtual bool GetDerivative(long CofId,_CSparsePolynomial& Result) override {return false;}
};

class _CAbstractNumericalComponent : public _CPassiveComponentCont
{
public:
	_CAbstractNumericalComponent(const string& name,long double Value,bool ConsiderInDerivative=true):
	  _CPassiveComponentCont(name,ConsiderInDerivative), m_Value(Value) {}
	virtual long double GetValue() override {return m_Value;}
protected:
	long  double m_Value;
};

class _CResistorComponentCont : public _CAbstractNumericalComponent
{
public:
	_CResistorComponentCont(
		const string& name, 
		const string& NodeA, 
		const string& NodeB,
		bool IsConductance,
		long double Value,
		bool ConsiderInDerivative=true):
			_CAbstractNumericalComponent(name,Value,ConsiderInDerivative),
			m_NodeA(NodeA),
			m_NodeB(NodeB),
			m_IsConductance(IsConductance){}
	virtual ~_CResistorComponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		pCircuit->AddResistor(m_name,m_NodeA,m_NodeB,m_IsConductance,m_Value);
	}
protected:
	string m_NodeA; 
	string m_NodeB;
	bool m_IsConductance;
};

class _CTransonductanceComponentCont : public _CAbstractNumericalComponent
{
public:
	_CTransonductanceComponentCont(
		const string& name,
		const string& OutP,
		const string& OutM, 
		const string& InP, 
		const string& InM,
		long double Value,
		bool ConsiderInDerivative=true):
			_CAbstractNumericalComponent(name,Value,ConsiderInDerivative),
			m_OutP(OutP),
			m_OutM(OutM),
			m_InP(InP),
			m_InM(InM){}
	virtual ~_CTransonductanceComponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		pCircuit->AddTransconductance(
			m_name,m_OutP,m_OutM,m_InP,m_InM,m_Value,0);
	}
protected:
	string m_OutP;
	string m_OutM; 
	string m_InP; 
	string m_InM;
};

class _CTransimpedanceComponentCont : public _CAbstractNumericalComponent
{
public:
	_CTransimpedanceComponentCont(
		const string& name,
		const string& OutP,
		const string& OutM, 
		const string& InP, 
		const string& InM,
		long double Value,
		bool ConsiderInDerivative=true):
			_CAbstractNumericalComponent(name,Value,ConsiderInDerivative),
			m_OutP(OutP),
			m_OutM(OutM),
			m_InP(InP),
			m_InM(InM) {}
	virtual ~_CTransimpedanceComponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		pCircuit->AddTransimpedance(
			m_name,m_OutP,m_OutM,m_InP,m_InM,m_Value,0);
	}
protected:
	string m_OutP;
	string m_OutM; 
	string m_InP; 
	string m_InM;
};

class _CVoltageGainComponentCont : public _CAbstractNumericalComponent
{
public:
	_CVoltageGainComponentCont(
		const string& name,
		const string& OutP,
		const string& OutM, 
		const string& InP, 
		const string& InM,
		long double Value,
		bool ConsiderInDerivative=true):
			_CAbstractNumericalComponent(name,Value,ConsiderInDerivative),
			m_OutP(OutP),
			m_OutM(OutM),
			m_InP(InP),
			m_InM(InM) {}
	virtual ~_CVoltageGainComponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		pCircuit->AddVoltageGain(
			m_name,m_OutP,m_OutM,m_InP,m_InM,m_Value,0);
	}
protected:
	string m_OutP;
	string m_OutM; 
	string m_InP; 
	string m_InM;
};

class _CCurrentGainComponentCont : public _CAbstractNumericalComponent
{
public:
	_CCurrentGainComponentCont(
		const string& name,
		const string& OutP,
		const string& OutM, 
		const string& InP, 
		const string& InM,
		long double Value,
		bool ConsiderInDerivative=true):
			_CAbstractNumericalComponent(name,Value,ConsiderInDerivative),
			m_OutP(OutP),
			m_OutM(OutM),
			m_InP(InP),
			m_InM(InM){}
	virtual ~_CCurrentGainComponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		pCircuit->AddCurrentGain(
			m_name,m_OutP,m_OutM,m_InP,m_InM,m_Value,0);
	}
protected:
	string m_OutP;
	string m_OutM; 
	string m_InP; 
	string m_InM;
};

//long AddNullor(const string& OutP, const string& OutM, const string& InP, const string& InM);
class _CNullorComponentCont : public _CAbstractPathologicalComponent
{
public:
	_CNullorComponentCont(
		const string& OutP,
		const string& OutM, 
		const string& InP, 
		const string& InM):
			m_OutP(OutP),
			m_OutM(OutM),
			m_InP(InP),
			m_InM(InM) {}
	virtual ~_CNullorComponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		pCircuit->AddNullor(
			m_OutP,m_OutM,m_InP,m_InM);
	}
protected:
	string m_OutP;
	string m_OutM; 
	string m_InP; 
	string m_InM;
};

//long AddPathologicalPair(const string& OutP, const string& OutM, const string& InP, 
// const string& InM,bool CurrentMirror=false,bool VoltageMirror=false);
class _CPathologicalPairomponentCont : public _CAbstractPathologicalComponent
{
public:
	_CPathologicalPairomponentCont(
		const string& OutP,
		const string& OutM, 
		const string& InP, 
		const string& InM,
		bool CurrentMirror,
		bool VoltageMirror):
			m_OutP(OutP),
			m_OutM(OutM),
			m_InP(InP),
			m_InM(InM),
			m_CurrentMirror(CurrentMirror),
			m_VoltageMirror(VoltageMirror){}
	virtual ~_CPathologicalPairomponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		pCircuit->AddPathologicalPair(
			m_OutP,m_OutM,m_InP,m_InM,m_CurrentMirror,m_VoltageMirror);
	}
protected:
	string m_OutP;
	string m_OutM; 
	string m_InP; 
	string m_InM;
	bool m_CurrentMirror;
	bool m_VoltageMirror;
};

//long AddOpAmp(const string& Out, const string& II, const string& NI);
class _COpAmpComponentCont : public _CAbstractPathologicalComponent
{
public:
	_COpAmpComponentCont(
		const string& Out,
		const string& II, 
		const string& NI): 
			m_Out(Out),
			m_II(II),
			m_NI(NI) {}
	virtual ~_COpAmpComponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		pCircuit->AddOpAmp(m_Out,m_II,m_NI);
	}
protected:
	string m_Out;
	string m_II; 
	string m_NI; 
};

//long AddCurrentConveyor(const string& X, const string& Y, const string& Z,short Type);
class _CCurrentConveyorComponentCont : public _CAbstractPathologicalComponent
{
public:
	_CCurrentConveyorComponentCont(
		const string& X,
		const string& Y, 
		const string& Z, 
		short Type):
			m_X(X),
			m_Y(Y),
			m_Z(Z),
			m_Type(Type) {}
	virtual ~_CCurrentConveyorComponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		pCircuit->AddCurrentConveyor(m_X,m_Y,m_Z,m_Type);
	}
protected:
	string m_X;
	string m_Y; 
	string m_Z; 
	short m_Type;
};

//long IncludeSubcircuit(const string& InstanceName,
//const string& ModelName, const _CStrNodes& Connections);
//long IncludeSubcircuit(const string& InstanceName, long ModelID, 
//const _CStrNodes& Connections);
class _CSubcircuitComponentCont : public _CPassiveComponentCont
{
public:
	_CSubcircuitComponentCont(
		bool& IsSymetric,
		const string& InstanceName,
		const string& ModelName,
		const _CStrNodes& Connections,
		bool ConsiderInDerivative=false):
			m_IsSymetric(IsSymetric),
			_CPassiveComponentCont(InstanceName,ConsiderInDerivative),
			m_ModelName(ModelName),
			m_Connections(Connections),
			m_ModelID(-1) {}
	_CSubcircuitComponentCont(
		bool& IsSymetric,
		const string& InstanceName,
		long ModelID,
		const _CStrNodes& Connections):
			m_IsSymetric(IsSymetric),
			_CPassiveComponentCont(InstanceName),
			m_Connections(Connections),
			m_ModelID(ModelID) {}
	virtual ~_CSubcircuitComponentCont() {}
	virtual void AddComponent(_CCircuit* pCircuit) override
	{
		if(m_ModelName.empty())
		{
			pCircuit->IncludeSubcircuit(m_name,m_ModelID,m_Connections);
			_CSubCircuit* pSubCircuit=pCircuit->GetSubcircuit(m_ModelID);
			m_IsSymetric=pSubCircuit->IsModelSymetric();
		}
		else
		{
			pCircuit->IncludeSubcircuit(m_name,m_ModelName,m_Connections);
			_CSubCircuit* pSubCircuit=pCircuit->GetSubcircuit(m_ModelName);
			m_IsSymetric=pSubCircuit->IsModelSymetric();
		}
	}
	virtual void CreateCompTraitor(_CMainCircuit* pMainCircuit) override
	{
		RISEPDD(eWrongCondition,"Not Implenented");
	}
	virtual bool GetDerivative(long CofId,_CSparsePolynomial& Result) override
	{
		RISEPDD(eWrongCondition,"Not Implenented");
	}
protected:
	string m_ModelName;
	_CStrNodes m_Connections;
	long m_ModelID;
	bool& m_IsSymetric;
};


class _CPassiveComponentBuff
{
public:
	_CPassiveComponentBuff():m_pMainCircuit(NULL) {}
	~_CPassiveComponentBuff() {clear();}
	void clear();
	void StoreContener(_CPassiveComponentCont* pCont) {m_List.push_back(pCont);}
	void AddComponents(_CCircuit* pCircuit);
	void Prepare4Derivatieves();
	void ReportDerivatives(iostream& file,long CofId);
	void ReportSensitivieties(iostream& file,long CofId, long ComDenCofId);
protected:
	typedef list<_CPassiveComponentCont*> LIST;
	LIST m_List;
	_CMainCircuit* m_pMainCircuit;
	friend class _CNetDynamicAnalyser;
};



class _CNetDynamicAnalyser
{
public:
	_CNetDynamicAnalyser(_CPDDEngine* pEngine,const string& NetName,bool BySubcircuit,string& CommonNodeName=EmptyString);//:m_pEngine(pEngine),m_pMainCircuit(NULL),m_pNetCircuit(NULL) {}
	~_CNetDynamicAnalyser();
	//void AddRLCSection(
	//	long double RValue,
	//	long double CValue,
	//	long double LValue,
	//	const string& SectionName, 
	//	const string& NodeInS, 
	//	const string& NodeOutS, 
	//	const string& NodeComS=EmptyString, 
	//	const string& CopledSectionName=EmptyString,
	//	long double MValue=0.0,
	//	long double CouplingCValue=0.0);
	void AddRLCSection(
		long double RValue,
		long double CValue,
		long double LValue,
		const string& SectionName, 
		const string& NodeInS, 
		const string& NodeOutS, 
		const string& NodeComS=EmptyString); 
	void CoupleSections(const string& S1, const string& S2, long double MValue, bool Capacitor=false, long double CCValue=0.0);
	void CoupleInductors(const string& L1, const string& L2, long double MValue);
	long AddResistor(const string& name, const string& NodeA, const string& NodeB,bool IsConductance=false,long double Value=0.0);
	long AddCapacitor(const string& name, const string& NodeA, const string& NodeB,long double Value=0.0);
	long AddInductor(const string& name, const string& NodeA, const string& NodeB,long double Value=0.0);
	long AddTransconductance(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value);
	long AddTransimpedance(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value);
	long AddVoltageGain(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value);
	long AddCurrentGain(const string& name,const string& OutP,const string& OutM, const string& InP, const string& InM,
		long double Value);
	long AddNullor(const string& OutP, const string& OutM, const string& InP, const string& InM);
	long AddPathologicalPair(const string& OutP, const string& OutM, const string& InP, const string& InM,bool CurrentMirror=false,bool VoltageMirror=false);
	long AddOpAmp(const string& Out, const string& II, const string& NI);
	long AddCurrentConveyor(const string& X, const string& Y, const string& Z,short Type);
	long IncludeSubcircuit(const string& InstanceName, const string& ModelName, const _CStrNodes& Connections);
	long IncludeSubcircuit(const string& InstanceName, long ModelID, const _CStrNodes& Connections);
	void DefineInputVS(const string& ValueName, long double Voltage,const string& HotNode,const string& ColdNode=EmptyString);
	void DefineOutputV(const string& ValueName, const string& HotNode,const string& ColdNode=EmptyString);
	void DeclareGlobalRefNode(const string& CommonNodeName)
	{
		m_pMainCircuit->DeclareGlobalRefNode(CommonNodeName);
		m_CommonNodeName=true;
	}
	void DetermineNetModel();
	void SetOutFileName(const string& Name,bool Append=false)
	{
		m_OutFileName=Name;
		m_pMainCircuit->SetGlobalModelFileName(Name,!Append);
	}
	void SetReportName(const string& Name)
	{
		m_ReportFileName=Name;
		m_pMainCircuit->SetGlobalRaportFileName(Name,false);
	}
	void WriteModelsToTextFile()
	{
		m_pMainCircuit->WriteModelsToTextFile(m_OutFileName);
	}
	void ReportMatricesComponents(bool NumericValue,bool FlatApprox,bool Factorise, bool FlatRes, bool AllDerivatieves,
								   bool Exact=true,
 								   const NumericType& Accurracy=DBL_EPSILON);
	void ForceCalculationWithConductance(bool Force = true) { m_ForceCaluclationWithConductance = Force; }
protected:
	bool m_ForceCaluclationWithConductance;
	_CPassiveComponentBuff m_PassiveCompBuffer;
	void ReportCofactor(iostream& Stream,
						unsigned long CofId,
						const string& CoffDescription, 
						bool NumericValue,bool FlatApprox,bool Factorise, bool FlatRes, bool AllDerivatieves,
						long AppId);
	//_CCofactor* CreateNewCofactor()
	//{
	//	return m_pMainCircuit->GetCofactor(m_pMainCircuit->CreateNewCofactor());
	//}
	string TranslateNode(int iNode)
	{
		m_pMainCircuit->TranslateNode(iNode);
	}
	class _COutput 
	{
	protected:
		int m_HotNode;
		int m_ColdNode;
		string m_ValueName;
	public:
		int HotNode() const {return m_HotNode;}
		int ColdNode() const {return m_ColdNode;}
		const string& ValueName() const {return m_ValueName;}
		_COutput(const string& ValueName,int HotNode, int ColdNode=0):
		m_HotNode(HotNode),m_ColdNode(ColdNode),m_ValueName(ValueName) {}
		_COutput(const _COutput& Source):
		m_HotNode(Source.m_HotNode),m_ColdNode(Source.m_ColdNode),m_ValueName(Source.m_ValueName) {}
	};
	class _CInput : public _COutput
	{
	protected:
		long double m_Value;
	public:
		const long double& Value() const {return m_Value;}
		_CInput(const string& ValueName,long double Value, int HotNode, int ColdNode=0):
		m_Value(Value),_COutput(ValueName,HotNode,ColdNode) {}
		_CInput(const _CInput& Source):
		m_Value(Source.m_Value),_COutput(Source) {}
	};
	int RegisterGlobalNodeName(const string& StrNode); 
	int RegisterNetNodeName(const string& StrNode); 
	void PairGlobalLocal(int GlobalNode, int LocalNode);
	_CPDDEngine* m_pEngine;
	_CMainCircuit* m_pMainCircuit;
	_CSubCircuit* m_pNetCircuit;
	string m_CommonNodeName;
	bool m_CommonNodeSet;
	template<typename Type>
	class _CComp
	{
	public:
		bool operator()(const Type& Left, const Type& Right) const
		{
			return Left.HotNode()<Right.HotNode() || Left.ColdNode()<Right.ColdNode();
		}
	};
	typedef set<_CInput,_CComp<_CInput> > INP_SET;
	INP_SET m_InpSet;
	INP_SET m_FeshInputs;
	typedef set<_COutput,_CComp<_COutput> > OUT_SET;
	OUT_SET m_FreshOuts;
	OUT_SET m_OutSet;
	class _CMoment
	{
	public:
		_CMoment(MOMENT_ORDER MyOrder):m_MyOrder(MyOrder) {}
	protected:
		MOMENT_ORDER m_MyOrder;
	};
	class _CStateVar : public _CInput
	{
	public:
		typedef enum {ST_VOLTAGE,ST_CURRENT} STATE_TYPE;
		_CStateVar(_CNetDynamicAnalyser& ParentAnalyer,const string& ValueName,STATE_TYPE Type, long double  Value,int HotNode,int ColdNode):
			m_StateType(Type), _CInput(ValueName,Value,HotNode,ColdNode),/*m_pCoupled(NULL),*/
			m_ParentAnalyzer(ParentAnalyer) {}
		~_CStateVar() 
		{
			//delete m_pCoupled;
		}
		_CStateVar(const _CStateVar& Source):
		m_StateType(Source.m_StateType), _CInput(Source),
			/*m_pCoupled(Source.m_pCoupled==NULL?NULL:new COUPLED(*Source.m_pCoupled)),*/
			m_ParentAnalyzer(Source.m_ParentAnalyzer) {}
		STATE_TYPE StateType() const {return m_StateType;}
		string Description() const {return (m_StateType==ST_VOLTAGE?"V_(":"I_(")+m_ValueName+")";}
		//bool AddLCoupling(_CStateVar* pState,long double Value);
	protected:
		STATE_TYPE m_StateType;
		//pair<_CStateVar*,long double> COUPLED_IND;
		//typedef unordered_map<_CStateVar*,long double> COUPLED;
		//COUPLED *m_pCoupled;
		typedef map<MOMENT_ORDER/*Order*/,_CMoment> MOMENTS;
		MOMENTS m_Moments;
		_CNetDynamicAnalyser& m_ParentAnalyzer;
		//SIGNED_TERM m_FlatRes;
	};
	class _CRLCDSectionMin
	{
	public:
		_CRLCDSectionMin(
			/*int GNodeIn, int LNodeIn,*/
			int GNodeOut, int LNodeOut,
			int GNodeCom, int LNodeCom,
			int GNodeM, int LNodeM,
			_CStateVar* pLState):
			//m_NodeIn(GNodeIn,LNodeIn),
			m_NodeOut(GNodeOut,LNodeOut),
			m_NodeM(GNodeM,LNodeM),
			m_NodeCom(GNodeCom,LNodeCom),
			m_pLState(pLState)
		{}
		_CRLCDSectionMin(const _CRLCDSectionMin& Source):
			/*m_NodeIn(Source.m_NodeIn),*/
			m_NodeOut(Source.m_NodeOut),
			m_NodeM(Source.m_NodeM),
			m_NodeCom(Source.m_NodeCom),
			m_pLState(Source.m_pLState)
		{}
		_CStateVar* GetLState() {return m_pLState;}
		//int LNodeIn() {return m_NodeIn.first;}
		int LNodeOut() {return m_NodeOut.first;}
		int LNodeCom() {return m_NodeCom.first;}
		int LNodeM() {return m_NodeM.first;}
		//int GNodeIn() {return m_NodeIn.second;}
		int GNodeOut() {return m_NodeOut.second;}
		int GNodeCom() {return m_NodeCom.second;}
		int GNodeM() {return m_NodeM.second;}
	protected:
		typedef pair<int,int> NODES;//first - local, second global
		//NODES m_NodeIn;
		NODES m_NodeOut;
		NODES m_NodeCom;
		NODES m_NodeM;
		_CStateVar* m_pLState;
	};
	//class _CRLCDSection
	//{
	//public:
	//	_CRLCDSection(const string& SectionName, 
	//		int GNodeIn, int LNodeIn,
	//		int GNodeOut, int LNodeOut,
	//		int GNodeCom, int LNodeCom,
	//		int GNodeM, int LNodeM,
	//		long double RValue,
	//		long double CValue,
	//		long double LValue,
	//		const _CRLCDSection* pCoupledSection=NULL,
	//		long double MValue=0.0,
	//		long double CouplingCValue=0.0);
	//	_CRLCDSection(const _CRLCDSection& Source);
	//	const string& SectionName() const {return m_SectionName;}
	//protected:
	//	string m_SectionName;
	//	typedef pair<int,int> NODES;//first - local, second global
	//	NODES m_NodeIn;
	//	NODES m_NodeOut;
	//	NODES m_NodeCom;
	//	NODES m_NodeM;
	//	long double m_RValue;
	//	long double m_CValue;
	//	long double m_LValue;
	//	const _CRLCDSection* m_pCoupledSection;
	//	long double m_MValue;
	//	long double m_CouplingCValue;
	//	NODES m_NodeCCoupled;
	//	friend class _CNetDynamicAnalyser;
	//	//m_NodeA InNode
	//	//m_NodeB MdNode
	//};
	//typedef unordered_map<string,_CRLCDSection> SECTION_MAP;
	typedef unordered_map<string,_CRLCDSectionMin> SECTION_MAP;
	SECTION_MAP m_SMap;
	string Translate(const _CInput& Input) const;
	string Translate(const _CStateVar& State,bool Input) const;
	class _CStateList : public map<string/*name*/,_CStateVar*>
	{
	public:
		typedef public map<string/*name*/,_CStateVar*> MY_TYPE;
		_CStateVar* RegisterState(_CNetDynamicAnalyser& ParentAnalyzer,const string& ValueName,
			_CStateVar::STATE_TYPE Type, 
			long double  Value,
			int HotNode,
			int ColdNode);
		~_CStateList();
	};
	//typedef list<_CStateVar> STATES;
	//STATES m_States;
	//STATES m_FreshStates;
	_CStateList m_States;
	_CStateList m_FreshStates;
	class _CValueName
	{
	public:
		_CValueName(const string& Name, long double Value):m_Name(Name),m_Value(Value) {}
		_CValueName(const _CValueName& Source):m_Name(Source.m_Name),m_Value(Source.m_Value) {}
	public:
		string m_Name;
		long double m_Value;
	};
	int m_InpGatesNo;
	int m_CurrentStatesNo;
	int DenominatorRank();
	typedef pair<_CStateVar*,_CStateVar*> A_STATE_COORDINATES;
	unsigned int m_iCommonDenCof;
	_CCofactor* m_pCommonDenCof;
	typedef map<A_STATE_COORDINATES,size_t> AI_COFACTOR_TABLE;
	AI_COFACTOR_TABLE m_AICofactors;
	//typedef map<A_STATE_COORDINATES,long> TRANS_DEF_TABLE;
	typedef AI_COFACTOR_TABLE TRANS_DEF_TABLE;
	TRANS_DEF_TABLE m_TransDefs;
	typedef map<A_STATE_COORDINATES,_CCofactor*> A_COFACTOR_TABLE;
	A_COFACTOR_TABLE m_ACofactors;
	typedef pair<_CCofactor*,short> COF_OF_A_RES;
	COF_OF_A_RES Cof_of_A(_CStateVar* Row,_CStateVar* Col);
	typedef pair<const _CInput*,const _CStateVar*> B_STATE_COORDINATES;
	typedef map<B_STATE_COORDINATES,size_t> BI_COFACTOR_TABLE;
	BI_COFACTOR_TABLE m_BICofactors;
	typedef BI_COFACTOR_TABLE TRANS_BASE_DEF_TABLE;
	TRANS_BASE_DEF_TABLE m_TransBaseDefs;
	typedef map<B_STATE_COORDINATES,_CCofactor*> B_COFACTOR_TABLE;
	B_COFACTOR_TABLE m_BCofactors;
	typedef map<A_STATE_COORDINATES,_CValueName> COUPLED_INDUCTORS;
	COUPLED_INDUCTORS m_CoupledInductorsMap;
	void CreateNewACofItems(_CStateVar& StateInp, _CStateVar& StateOut);
	void CreateNewBCofItems(const _CInput& Input, const _CStateVar& StateOut);
	void UpdateMatricesByNewInputGates();
	void UpdateMatricesByNewStates();
	void UpdateMatricesByNewOutputGates();
	bool m_Symetric;
	string m_NetName;
	class _CLocalIntNodes
	{
	public:
		_CLocalIntNodes():m_MaxLocalPin(0) {}
		void Connect(int LocalPin, int GlobalPin);
		_CIntNodes* ToIntNodes(_CIntNodes* pIntNodes);
		_CIntNodes* ToIntNodesOrdered(_CIntNodes* pIntNodes);
		operator const INT_INT_MAP&() {return m_Connections;}
	protected:
		INT_INT_MAP m_Connections;
		int m_MaxLocalPin;
	};
	_CLocalIntNodes m_iConnections;
	_CStrNodes m_Connections;
	string m_OutFileName;
	string m_ReportFileName;
	bool m_BySubcircuit;
};
