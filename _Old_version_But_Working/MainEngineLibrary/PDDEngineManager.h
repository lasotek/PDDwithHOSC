#pragma once
#include "stdafx.h"
#include <vector>
#include <deque>
#include "PDDEngine.h"
#include "NodesManager.h"
#include "norm_float.h"

using namespace std;
class CPDDEngineManager 
{
public:
	CPDDEngineManager(void):m_CofactorCounter(0) {}
	~CPDDEngineManager(void);
	long GetPDDEngine(ULONG hAppInstance,bool sExpanded, bool ComponentOrderOptimization,bool UseNewAnalyser=true);
	int ReleasePDDEngine(long hEngineInstance);
	int ReleaseMyPDDEngines(ULONG hAppInstance);
	long long GetNewCircuit(long hEngineInstance);
	long EngineError(long long hCircuit);
	long CircuitError(long long hCircuit);
	//long CreateNewSubcircuit(long long hCircuit, string SubName, const _CStrNodes& BoundaryNodes,const SIZE_T_SET* pDeletionAdmissible=NULL,bool ForceNewIfExist=false);
	long CreateNewSubcircuit(long long hCircuit, string SubName, const _CStrNodes& BoundaryNodes,const SIZE_T_RANGE* pDeletionAdmissible=NULL,bool ForceNewIfExist=false);
	//long CreateSubcircuitFromLibrary(long long hCircuit, string SubName, const _CStrNodes& BoundaryNodes,const SIZE_T_SET* pDeletionAdmissible=NULL);
	long CreateSubcircuitFromLibrary(long long hCircuit, string SubName, const _CStrNodes& BoundaryNodes,const SIZE_T_RANGE* pDeletionAdmissible=NULL);
	long SetOrederOptimization(long long hCircuit, bool Optimize);

	long long CreateNewCofactor(long long hCircuit);
	long CreateNewCofactor(long long hCircuit, long hSubcircuitId);
	long CreateNewCofactor(long long hCircuit, const string& SubcircuitName);

	typedef pair < string, string > COF_DELETION;
	typedef pair< COF_DELETION, COF_DELETION> COF_DELETION_PAIR;
	typedef vector<COF_DELETION_PAIR> DELETIONS;

	long long CreateNewCofactor(long long hCircuit, const DELETIONS& newDels);
	long CreateNewCofactor(long long hCircuit, long hSubcircuitId, const DELETIONS& newDels);
	long CreateNewCofactor(long long hCircuit, const string& SubcircuitName, const DELETIONS& newDels);

	long AddPairOfDeletions(long long hCofactor, const string& sp, const string& sr, const string& sk, const string& sl);
	long AddPairOfDeletions(long long hCircuit, long hSubcircuitId, long hCofactor, const string& sp, const string& sr, const string& sk, const string& sl);
	long AddPairOfDeletions(long long hCircuit, const string& SubcircuitName, long hCofactor, const string& sp, const string& sr, const string& sk, const string& sl);

	long long DefineTransferFunctionSimp(const string& Name, long long hNumerator, long long hDenominator);
	long RaportTransferFunctionValuesLog(long long hCircuit, long long hTransfer, const string& Context,int LowestDec, int HighestDec, size_t NoOfPointsPerDec, bool Transfer2MagPhase = false);
	long RaportTransferFunctionValuesLin(long long hCircuit, long long hTransfer, const string& Context, long double LowestAngFreq, long double HighestAngFreq, unsigned long NoOfGlobalPoints, bool Transfer2MagPhase = false);
	long AddExtraTransferFunctionSimp(long long PairId, long long hExtraCofactor);
	long DeclareGlobalRefNode(long long hCircuit, const string& sRefNode);
	long DoTheCalculation(long long hCircuit);
	long DetermineSimplePDD(long long hCircuit, bool sCExpanded,bool Approximated=false,long double Accuracy=0.001);

	long DetermineFlatSimplifiedPDD(long long hCircuit, long TrId, bool TryToFactorize);

	long AddResistor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		bool AsConductance);
	long AddResistor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		const string& SubcircuitName, bool AsConductance);
	long AddResistor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		long SubcircuitId, bool AsConductance);

	long AddCapacitor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value);
	long AddCapacitor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		const string& SubcircuitName);
	long AddCapacitor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		long SubcircuitId);

	long AddInductor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value);
	long AddInductor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		const string& SubcircuitName);
	long AddInductor(long long hCircuit,const string& name,const string& NodeA, const string& NodeB, long double Value,
		long SubcircuitId);

	long AddTransconductance(long long hCircuit,const string& name,const string& OutP, const string& OutM,
		const string& InP, const string& InM, int sPower, long double Value);
	long AddTransconductance(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
		const string& InP, const string& InM, int sPower, long double Value, const string& SubcircuitName);
	long AddTransconductance(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
		const string& InP, const string& InM, int sPower, long double Value,long SubcircuitId);

	long AddTransimpedance(long long hCircuit,const string& name,const string& OutP, const string& OutM,
		const string& InP, const string& InM, int sPower, long double Value);
	long AddTransimpedance(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
		const string& InP, const string& InM, int sPower, long double Value, const string& SubcircuitName);
	long AddTransimpedance(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
		const string& InP, const string& InM, int sPower, long double Value,long SubcircuitId);

	long AddVoltageGain(long long hCircuit,const string& name,const string& OutP, const string& OutM,
		const string& InP, const string& InM, int sPower, long double Value);
	long AddVoltageGain(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
		const string& InP, const string& InM, int sPower, long double Value, const string& SubcircuitName);
	long AddVoltageGain(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
		const string& InP, const string& InM, int sPower, long double Value,long SubcircuitId);

	long AddCurrentGain(long long hCircuit,const string& name,const string& OutP, const string& OutM,
		const string& InP, const string& InM, int sPower, long double Value);
	long AddCurrentGain(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
		const string& InP, const string& InM, int sPower, long double Value, const string& SubcircuitName);
	long AddCurrentGain(long long hCircuit,const string& name,const string& OutP, const string& OutM, 
		const string& InP, const string& InM, int sPower, long double Value,long SubcircuitId);

	long AddNullor(long long hCircuit,const string& OutP, const string& OutM,const string& InP, const string& InM);
	long AddNullor(long long hCircuit,const string& OutP, const string& OutM, const string& InP, const string& InM,
		const string& SubcircuitName);
	long AddNullor(long long hCircuit,const string& OutP, const string& OutM, const string& InP, const string& InM,
		long SubcircuitId);

	long AddDVCCII(long long hCircuit, const string& Ix, const string& Iy1, const string& Iy2, const string& Iz1, const string& Iz2);
	long AddDVCCII(long long hCircuit, const string& Ix, const string& Iy1, const string& Iy2, const string& Iz1, const string& Iz2, const string& SubcircuitName);
	long AddDVCCII(long long hCircuit, const string& Ix, const string& Iy1, const string& Iy2, const string& Iz1, const string& Iz2, long SubcircuitId);

	long AddFDCCII(long long hCircuit, const string& Vx1, const string& Vx2,
		const string& Vy1, const string& Vy2, const string& Vy3, const string& Vy4,
		const string& Iz1, const string& Iz2);
	long AddFDCCII(long long hCircuit, const string& Vx1, const string& Vx2,
		const string& Vy1, const string& Vy2, const string& Vy3, const string& Vy4,
		const string& Iz1, const string& Iz2, const string& SubcircuitName);
	long AddFDCCII(long long hCircuit, const string& Vx1, const string& Vx2,
		const string& Vy1, const string& Vy2, const string& Vy3, const string& Vy4,
		const string& Iz1, const string& Iz2, long SubcircuitId);

	long AddPathologicalPair(long long hCircuit,const string& OutP, const string& OutM,const string& InP, const string& InM,bool CurrentMirror,bool VoltageMirror);
	long AddPathologicalPair(long long hCircuit,const string& OutP, const string& OutM, const string& InP, const string& InM,bool CurrentMirror,bool VoltageMirror,
		const string& SubcircuitName);
	long AddPathologicalPair(long long hCircuit,const string& OutP, const string& OutM, const string& InP, const string& InM,bool CurrentMirror,bool VoltageMirror,
		long SubcircuitId);

	long AddVoltageFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM);
	long AddVoltageFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM,
		const string& SubcircuitName);
	long AddVoltageFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM,
		long SubcircuitId);

	long AddCurrentFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM);
	long AddCurrentFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM,
		const string& SubcircuitName);
	long AddCurrentFollower(long long hCircuit, const string& OutP, const string& OutM, const string& InP, const string& InM,
		long SubcircuitId);

	long AddOpAmp(long long hCircuit,const string& Out, const string& II, const string& NI);
	long AddOpAmp(long long hCircuit,const string& Out, const string& II, const string& NI,const string& SubcircuitName);
	long AddOpAmp(long long hCircuit,const string& Out, const string& II, const string& NI,long SubcircuitId);

	long AddCurrentConveyor(long long hCircuit,const string& X, const string& Y, const string& Z, short Type);
	long AddCurrentConveyor(long long hCircuit,const string& X, const string& Y, const string& Z, short Type, const string& SubcircuitName);
	long AddCurrentConveyor(long long hCircuit,const string& X, const string& Y, const string& Z, short Type, long SubcircuitId);

	long AddBOCII(long long hCircuit, const string& X, const string& Y, const string& Zp, const string& Zm, short Type);
	long AddBOCII(long long hCircuit, const string& X, const string& Y, const string& Zp, const string& Zm, short Type, const string& SubcircuitName);
	long AddBOCII(long long hCircuit, const string& X, const string& Y, const string& Zp, const string& Zm, short Type, long SubcircuitId);

	long AddFDCFOA_RM_INF(long long hCircuit, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
		const string& Izp, const string& Izn, const string& Vwp, const string& Vwn);
	long AddFDCFOA_RM_INF(long long hCircuit, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
		const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, const string& SubcircuitName);
	long AddFDCFOA_RM_INF(long long hCircuit, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
		const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long SubcircuitId);

	long AddFDCFOA_RM(long long hCircuit, const string& name, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
		const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long double Value);
	long AddFDCFOA_RM(long long hCircuit, const string& name, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
		const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long double Value, const string& SubcircuitName);
	long AddFDCFOA_RM(long long hCircuit, const string& name, const string& Vyp, const string& Vyn, const string& Ixp, const string& Ixn,
		const string& Izp, const string& Izn, const string& Vwp, const string& Vwn, long double Value, long SubcircuitId);

	long IncludeSubcircuit(long long hCircuit, const string& InstanceName, const string& ModelName, const _CStrNodes& Connections, long InternalCofactor=1);
	long IncludeSubcircuit(long long hCircuit, const string& InstanceName, const string& ModelName, const string& SubcircuitName, const _CStrNodes& Connections, long InternalCofactor = 1);
	long IncludeSubcircuit(long long hCircuit, const string& InstanceName, const string& ModelName, long SubcircuitId, const _CStrNodes& Connections, long InternalCofactor = 1);
	long IncludeSubcircuit(long long hCircuit, const string& InstanceName, long ModelID, const _CStrNodes& Connections, long InternalCofactor = 1);
	long IncludeSubcircuit(long long hCircuit, const string& InstanceName, long ModelID, const string& SubcircuitName, const _CStrNodes& Connections, long InternalCofactor = 1);
	long IncludeSubcircuit(long long hCircuit, const string& InstanceName, long ModelID, long SubcircuitId, const _CStrNodes& Connections, long InternalCofactor = 1);
	long ForceModelCalculation(long long hCircuit,long ModelID,bool IgnoreSgnPropagation=false);
	long WriteModelsToTextFile(long long hCircuit, const string& FileName);
	//long WriteCircuitToTextFile(long long hCircuit, const string& FileName, bool DevelopeOrdered=true, bool ComputeNumerical=true);
	//long GetSPowerCoefficient(NumericType& Result, long long hCircuit, unsigned long CofId, unsigned short sPower,const string& Context=EmptyString);
	long ModifyComponentParameter(long long hCircuit, const string& InstancePath, long double NewValue);
	long ModifyComponentParameter(long long hCircuit, const string& InstancePath, long double NewValue, const string& SubcircuitName);
	long ModifyComponentParameter(long long hCircuit, const string& InstancePath, long double NewValue, long SubcircuitId);
	//long GetNumericalCofactorToComponent(NumericType& Result, long long hCircuit, unsigned long CofId, unsigned short sPower, const string& ParameterPath);
	//long GetNumericalReminderToComponent(NumericType& Result, long long hCircuit, unsigned long CofId, unsigned short sPower, const string& ParameterPath);
	long GetDirectNumericalCofactorToComponent(_CSparsePolynomial* pResult, long long hCofactor, unsigned short sPower, const string& ParameterPath, const string** pResContext=NULL,const string& BaseContext=EmptyString);
	long GetDirectNumericalReminderToComponent(_CSparsePolynomial* pResult, long long hCofactor, unsigned short sPower, const string& ParameterPath, const string** pResContext=NULL,const string& BaseContext=EmptyString);
	long GetDirectNumericalToComponent(_CSparsePolynomial* pResult, long long hCofactor, unsigned short sPower, const string& ParameterPath, const string** pResContext=NULL,const string& BaseContext=EmptyString);
	long CreateCofactorToComponent(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& ParameterPath, const string& BaseContext, const string*& pOutputContext);
	long CreateReminderToComponent(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& ParameterPath, const string& BaseContext, const string*& pOutputContext);
	long AppendToFileCoefVertices(long long hCircuit,const string& FileName, unsigned long CofId, unsigned short sPower, const string& ParameterPath);
	//long FindTheLeastSignificant(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context,long double* pNumericalResult=NULL,VECTOR_STRING* pResultPath=NULL,string* pStrResult=NULL);//returns shortest path Id or Error if <0
	//long GetTheLeastSignificantPath(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, NumericType& NumericalResult,_CLSPath& Path);
	//long GetTheLeastSignificantPathPtc(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, NumericType& NumericalResult,_CLSPath& Path);
	long RemovePath(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, _CLSPath& Path, const string*& pOutputContext);
	//long RemoveTheLeastSigmificantPath(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, NumericType& AbsolutDecrease, const string*& pOutputContext);
	//long RemoveTheLeastSigmificantPathPtc(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, NumericType& RelativePtcDecrease, const string*& pOutputContext);
	//long RemoveTheLeastSigmificantPaths(long long hCircuit,unsigned long CofId, unsigned short sPower, const string& Context, const NumericType& MaxDecreasePtc, NumericType& ResultDecreasePtc,const string*& pOutputContext,const char* FileToRegister=NULL, bool RegOnlyFinalResult=true);
	long GetDirectNmericalValue(long long hCofactor, unsigned short sPower, const string& Context, NumericType& Value);
	//long GetDirectNmericalPolynomial(long long hCircuit,unsigned long CofId, const string& Context, _CSPolynomial*& pOutPolynomial);
	//long GetDirectNmericalPolynomial(long long hCofactor,const string& Context, _CSparsePolynomial*& pOutPolynomial);
	long GetDirectNmericalPolynomial(long long hCofactor,const string& Context, _CSparsePolynomial& OutPolynomial);
	//long GetNumericalCofactorAndReminder(_CSparsePolynomial*& pResult, long long hCofactor, long hPathTraitor);
	long GetNumericalCofactorAndReminder(_CSparsePolynomial& Result, long long hCofactor, long hPathTraitor);
	long GetDirectNmericalPolynomialSize(long long hCofactor, const string& Context);
	long WriteDirectNmericalPolynomial(long long hCofactor, const string& Context,const string& FileName, bool Append=true, bool TestDistribution=false,bool FindZeroes=false,long double UpperAngFrequency=1e3);
	long WriteDirectNmericalPolynomial(long long hCofactor, long hPathTraitor,const string& FileName, bool Append=true, bool TestDistribution=false,long double UpperAngFrequency=1e3);
	long WriteIntermediateNumericPolynomials(long long hCircuit, const string& FileName, bool Append=true);
	long WriteFlatSimplifiedPDD(long long hCircuit, long TrId, bool TryToFactorize,bool Write1Level,const string& FileName, bool Append=true);
	//long WriteFlatSimplifiedSoP(long long hCircuit, long TrId, bool TryToFactorize,const string& FileName, bool Append=true);
	//long WriteFlatSimplifiedFeasibleDividers(long long hCircuit, long TrId, const string& FileName, bool Append=true);
	//long TestFlatPDDRemoval(long long hCircuit, long TrId);

	long PerformFullCofRemTest(long long hCircuit, const string& ElementPath, const string& OutTexFileName, bool CheckResIntegrity=true,bool Append=true, bool TestDistribution=false,long double UpperAngFrequency=1e3);
	long GetFreePathTraitor(long long hCircuit,const string& BaseConetx=EmptyString); 
	long AddParamReminder(long long hCircuit, long TrId, const string& ElementPath);
	long AddParamDivider(long long hCircuit, long TrId, const string& ElementPath);
	long ReleasePathTraitor(long long hCircuit, long TrId);
	long ClearPathTraitor(long long hCircuit, long TrId);
	long TestPathTraitor(long long hCircuit, long TrId);
	long SetApproximationCriterion(long long hCircuit,
								   long TrId,
								   bool Exact=true,
								   const NumericType& Accurracy=DBL_EPSILON,
								   const NumericType& HighFrequency=1e6, 
								   const NumericType& LowFrequency=0.0,
								   short MaxPower=numeric_limits<short>::max(),
								   short MinPower=0);

	long SetGlobalModelFileName(long long hCircuit, const string& Name,bool ClearOld=false);
	long SetGlobalCircuitFileName(long long hCircuit, const string& Name,bool ClearOld=false);
	long SetGlobalRaportFileName(long long hCircuit, const string& Name,bool ClearOld=false);
	_CNetDynamicAnalyser* GetTestAnalyser(string NetName, bool BySubcircuit,string ReferenceNode=EmptyString);
	_CUnlineraBlackBoxEngine* GetNewUnlinearBlackBox();
	long SetCompPriority(long long hCircuit, size_t Priority);
	long SetCompPriority(long long hCircuit, const string& SubcircuitName, size_t Priority);
	long SetCompPriority(long long hCircuit, long SubcircuitId, size_t Priority);
	long ClearCompPriority(long long hCircuit) { return SetCompPriority(hCircuit,numeric_limits<size_t>::max()); }
	long ClearCompPriority(long long hCircuit, const string& SubcircuitName) { return SetCompPriority(hCircuit, SubcircuitName, numeric_limits<size_t>::max()); }
	long ClearCompPriority(long long hCircuit, long SubcircuitId) { return SetCompPriority(hCircuit, SubcircuitId, numeric_limits<size_t>::max()); }
protected:
	typedef	pair<long,_CPDDEngine*> _CPDDEngineInstance;
	typedef vector<_CPDDEngineInstance*> _CEnginesVector;
	typedef _CEnginesVector::iterator ITERATOR;
		class CRemover
		{
		public:
			void RemoveAt(_CPDDEngineInstance*& pEngine);
		};
		class CInstanceRemover : public CRemover
		{
		public:
			CInstanceRemover(long hAppInstance):m_hAppInstance(hAppInstance),m_Counter(0) {}
			void operator() (_CPDDEngineInstance*& pEngine);
			operator int() {return m_Counter;}
		private:
			long m_hAppInstance;
			int  m_Counter;
		};
		class CAllRemover : public CRemover
		{
		public:
			void operator() (_CPDDEngineInstance*& pEngine) {RemoveAt(pEngine);}
		};
	_CEnginesVector m_Engines;
	inline long long MakeLongLong(long UpperLong, long LowerLong) 
		{return (((long long)UpperLong)<<sizeof(long)*8)+LowerLong;}
	inline long UpperLong(long long LongLong) {return (LongLong>>sizeof(long)*8);}
	inline long LowerLong(long long LongLong) {return ((LongLong<<sizeof(long)*8)>>sizeof(long)*8);}
	//inline long MakeLong(int UpperInt, int LowerInt) 
	//	{return (((long)UpperInt)<<sizeof(int)*8)+LowerInt;}
	//inline int UpperInt(long Long) {return (Long>>sizeof(int)*8);}
	//inline int LowerInt(long Long) {return ((Long<<sizeof(int)*8)>>sizeof(int)*8);}
	inline long long MakeCircuitId(long EnginId, long CircuitInEngine) {return MakeLongLong(EnginId,CircuitInEngine);}
	inline long GetEngineId(long long CircuitId) {return UpperLong(CircuitId);}
	inline long GetCircuitInEngine(long long CircuitId) {return LowerLong(CircuitId);}
	long hCircuit2Circuit(long long hCircuit, _CMainCircuit*& pCircuit);
	long hCircuit2SubCircuit(long long hCircuit, const string& SubName,_CSubCircuit*& pSubCircuit);
	long hCircuit2SubCircuit(long long hCircuit, long SubID,_CSubCircuit*& pSubCircuit);
	typedef pair<long long/*hCircuit*/,long/*Local hCofactor*/> GLOBAL_COFACTORS;
	typedef unordered_map<long long,GLOBAL_COFACTORS> COFACTORS_MAP; 
	COFACTORS_MAP m_Cofactors;
	typedef COFACTORS_MAP::iterator COF_MAP_ITERATOR;
	typedef COFACTORS_MAP::value_type COF_MAP_VALUE_TYPE;
	long hCoffactor2CCofactor(long long hCofactor,_CCofactor*& pCofactor);
	long hCofactor2CofId(long long hCofactor,unsigned long& CofId,long long& hCircuit);
	long long m_CofactorCounter;
};
