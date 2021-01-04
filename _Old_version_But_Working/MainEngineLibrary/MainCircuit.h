#pragma once
#include "StdAfx.h"
#include <string>
#include "Cofactor.h"
#include "Vertex.h"
#include "SubCircuit.h"
#include "SubCircuitSocket.h"
#include "UnaryOperators.h"
#include "AbstractResTables.h"
#include "norm_float.h"
#include <set>
#include <list>
#include "UniqStringBuffer.h"
#include "PathTraitor.h"
#include "FlatVertex.h"
#include "SimplifierEngine.h"
#include "Factorizer.h"
using namespace std;
using namespace stdext;

class _CPDDEngine;

typedef _CUniqStringBuffer _CContextStringBuffer;


class _CMainCircuit : public _CCircuit
{
public:
	_CMainCircuit(_CPDDEngine* MyEngine, bool OrderOptimalization,bool UseNewAnalyser);
	~_CMainCircuit(void);
	virtual void SetOptimizeComponentOrder(bool Optimize);
	//long DefineNewSubcircuit(const string& name, const _CStrNodes& BoundNodes, const SIZE_T_SET* pDeletionAdmissible, bool ForceNewIfExist);
	long DefineNewSubcircuit(const string& name, const _CStrNodes& BoundNodes, const SIZE_T_RANGE* pDeletionAdmissible, bool ForceNewIfExist);
	_CSubCircuit* DefineNewNetDynSubcircuit(const string& name,bool ForceNewIfExist);
	//long TryToGetSubcircuitFromLibrary(const string& name, const _CStrNodes& BoundNodes,const SIZE_T_SET* pDeletionAdmissible);
	long TryToGetSubcircuitFromLibrary(const string& name, const _CStrNodes& BoundNodes,const SIZE_T_RANGE* pDeletionAdmissible);
	virtual _CSubCircuit* GetSubcircuit(const string& name);
	virtual _CSubCircuit* GetSubcircuit(long Id);
	long GetCircuitId(const _CCircuit* pCircuit);
	virtual _CCircuit* GetCircuit(long Id) override;
	_CCofactor* GetCofactor(unsigned long Id);
	void DoTheCalculations();
	long DeclareGlobalRefNode(const string& sRefNode);
	const path_str& GetModelsPath();
	_CSubCircuit* PrepareSubcircuitFromName(const string& Name, const path_str& ModelFile);
	const string& GetName() const {return EmptyString;}
	void GetOuterVertex(_CSExpandedVertices*& pVertices, _CModelVertex* pMVertex, _CSExpandedDescendends& TerminalVertices);
	virtual size_t GetOuterVertexIndex(_CModelVertex* pMVertex);
	virtual short GetOuterVertexSign(_CModelVertex* pMVertex);
	void DetermineSimplePDD(bool sCExpanded, bool Approximated, const NumericType& Accuracy);
	void DetermineFlatSimplifiedPDD(long TrId, bool TryToFactorize);
	//void TestFlatPDDRemoval(long TrId);
	bool WriteFlatSimplifiedPDDEntry(iostream& Stream, long TrId, size_t CofId);
	void WriteFlatSimplifiedPDD(long TrId, bool TryToFactorize,bool Write1Level,const string& FileName, bool Append);
	size_t GetPowerOfCofFlatVertices(long TrId,long CofId);
	void FactorizedCofFlatVertices2Stream(iostream& Stream,long TrId,long CofId);
	void FactorizedCofFlat1LevelVertices2Stream(iostream& Stream,long TrId,long CofId);
	void PrepareAndWriteFlatSimplifiedPDDDiagrams(long TrId, bool Factorize,const string& FileName, bool Append);
	void WriteFactorizedPDDList(long TrId,const string& FileName, bool Append);
	//void WriteFlatSimplifiedSoP(long TrId, bool TryToFactorize,const string& FileName, bool Append);
	//void WriteFlatSimplifiedFeasibleDividers(long TrId,const string& FileName, bool Append);
	long WriteModelsToTextFile(const string& FileName);
	//long WriteCircuitToTextFile(const string& FileName, bool DevelopeOrdered, bool ComputeNumerical);
	void WriteHeader(iostream& stream) { stream<<"Main Circuit\n";}
	bool IsSExpanded();
	void WriteOrderedSimpleVertices(iostream& stream);
	//long GetSPowerCoefficient(unsigned long CofId, unsigned short sPower, NumericType& Result,const string& Context=EmptyString); 
	//long double GetSPowerCoefficient(_CCofactor* pCofactor, unsigned short sPower)
	//{
	//	return GetSPowerCoefficient(GetSVerticesIndexForCofactor(pCofactor),sPower);
	//}
	//void WriteSNumericalResults(iostream& stream);
	//void ReComputeSNmumericalResults();
	//inline void ResetSPowerCoeffCache() {m_SPowerCoeffCache.clear();}
	//bool ModifyParameter(const string& ParamPath, long double NewValue);
	//long GetNumericalCofactorToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, NumericType& Result,const string& BaseContext=EmptyString);
	//long GetNumericalReminderToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, NumericType& Result,const string& BaseContext=EmptyString);
	long GetDirectNumericalCofactorToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, _CSparsePolynomial* pResult,const string** pResContext=NULL,const string& BaseContext=EmptyString);
	long GetDirectNumericalReminderToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, _CSparsePolynomial* pResult,const string** pResContext=NULL,const string& BaseContext=EmptyString);
	
	long CreateCofactorToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, const string& BaseContext, const string*& pOutputContext);
	long CreateReminderToComponent(unsigned long CofId, unsigned short sPower, const string& ParameterPath, const string& BaseContext, const string*& pOutputContext);
	long WriteCoefVertices(iostream& stream, unsigned long CofId, unsigned short sPower, const string& ParameterPath);
	long AppendToFileCoefVertices(const string& FileName, unsigned long CofId, unsigned short sPower, const string& Context);
	//const string& GetParentContext(const string& ChildContext);
	//virtual const string* GetParentContext(const string& Child, size_t &ParentNo) {
	//	return m_ContextStrings.GetParentNo(Child,ParentNo);
	//}
	//long FindTheLeastSignificant(unsigned long CofId, unsigned short sPower, const string& Context, long double* pNumericalResult,VECTOR_STRING* pResultPath,string* pStrResult);//returns shortest path Id or Error if <0
	void ClearNemericalDependentsInCache(_CComponentPath& Path);
	void Path2CPath(_CConstComponentPath& CPath,_CComponentPath& Path);
	//long GetTheLeastSignificantPath(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& NumericalResult,_CLSPath& Path);
	//long GetTheLeastSignificantPathPtc(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& NumericalResult,_CLSPath& Path);
	long RemovePath(unsigned long CofId, unsigned short sPower, const string& Context, _CLSPath& Path, const string*& pOutputContext);
	//long RemoveTheLeastSigmificantPath(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& AbsolutDecrease, const string*& pOutputContext);
	//long RemoveTheLeastSigmificantPathPtc(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& RelativePtcDecrease, const string*& pOutputContext);
	//long RemoveTheLeastSigmificantPaths(unsigned long CofId, unsigned short sPower, const string& Context, const NumericType& MaxDecreasePtc, NumericType& ResultDecreasePtc,const string*& pOutputContext,const char* FileToRegister=NULL, bool RegOnlyFinalResult=true);
	//long GetDirectNmericalPolynomial(unsigned long CofId, const string& Context, _CSPolynomial*& pOutPolynomial);
	//long GetDirectNmericalPolynomial(unsigned long CofId, const string& Context, _CSparsePolynomial*& pOutPolynomial);
	long GetDirectNmericalPolynomial(unsigned long CofId, const string& Context, _CSparsePolynomial& OutPolynomial);
	////long GetDirectNmericalPolynomial2(unsigned long CofId, const string& Context, _CSparsePolynomial*& pOutPolynomial);
	//long GetNumericalCofactorAndReminder(unsigned long CofId, long hPathTraitor, _CSparsePolynomial*& pResult);
	long GetNumericalCofactorAndReminder(unsigned long CofId, long hPathTraitor, _CSparsePolynomial& Result);
	long GetDirectNmericalValue(unsigned long CofId, unsigned short sPower, const string& Context, NumericType& Value);
	long GetDirectNmericalPolynomialSize(unsigned long CofId, const string& Context);
	//long WriteDirectNmericalPolynomial(unsigned long CofId, const string& Context,const string& FileName, bool Append, _CSparsePolynomial** ppPolResult=NULL, bool TestDistribution=true, long double UpperAngFrequency=1e9);
	long WriteDirectNmericalPolynomial(unsigned long CofId, const string& Context,const string& FileName, bool Append, _CSparsePolynomial* pPolResult=NULL, bool FindZeroes=false, bool TestDistribution=true, long double UpperAngFrequency=1e9);
	long WriteDirectNmericalPolynomial(unsigned long CofId, const string& Context, iostream& stream, _CSparsePolynomial* pPolResult = NULL, bool FindZeroes = false, bool TestDistribution = true, long double UpperAngFrequency = 1e9);
	//long WriteDirectNmericalPolynomial(unsigned long CofId, long hPathTraitor,const string& FileName, bool Append, _CSparsePolynomial** ppPolResult=NULL, bool TestDistribution=true, long double UpperAngFrequency=1e9);
	long WriteDirectNmericalPolynomial(unsigned long CofId, long hPathTraitor,const string& FileName, bool Append, _CSparsePolynomial* pPolResult=NULL, bool TestDistribution=true, long double UpperAngFrequency=1e9);
	long WriteIntermediateNumericPolynomials(const string& FileName,bool Append);
	long PerformFullCofRemTest(const string& ElementPath, const string& OutTexFileName, bool CheckResIntegrity, bool Append, bool TestDistribution=true, long double UpperAngFrequency=1e9);
	virtual _CSExpandedVertices* GetUniqueSEExpandedVertices(_CSExpandedVertices& Source)
	{
		return m_SExpandedUniqueSet.GetUnique(Source);
	}
	virtual void ClearDevelopingVerticesCache() ;
	void ClearNumResultCaches();
	virtual const string* GetUniqueContextStringName(const string* String)
	{
		return &m_ContextStrings.GetUniqueString(*String);
	}
	virtual const string& GetUniqueContextStringName(const string& String)
	{
		return m_ContextStrings.GetUniqueString(String);
	}
	long GetFreePathTraitor(const string& BaseContext=EmptyString); 
	const _CComponent* AddParamReminder(long TrId, const string& ElementPath,const _CSubModelNumericPattern** ppCompPatt=NULL);
	const _CComponent* AddParamDivider(long TrId, const string& ElementPath,const _CSubModelNumericPattern** ppCompPatt=NULL);
	void SetApproximationCriterion(long TrId,
								   bool Exact=true,
 								   const NumericType& Accurracy=DBL_EPSILON,
								   const NumericType& HighFrequency=1e6, 
								   const NumericType& LowFrequency=0.0,
								   short MaxPower=numeric_limits<short>::max(),
								   short MinPower=0);
	long ReleasePathTraitor(long TrId);
	long ClearPathTraitor(long TrId);
	long TestPathTraitor(long TrId);
	virtual void CacheTheResult(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket);
	//virtual _CSubModelNumericPattern* FindResults(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket);
	//virtual void RegisterResult(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket) override;
	//virtual _CDirectNumContainer* FindExisting(_CDirectNumContainer* pCacheContainer,_CSubCircuitSocket* pMasterSocket) override;
	void SetGlobalModelFileName(const string& Name,bool ClearOld);
	void SetGlobalCircuitFileName(const string& Name,bool ClearOld);
	void SetGlobalRaportFileName(const string& Name,bool ClearOld);
	virtual const string& GetGlobalModelFileName() const { return m_ModelFileName;}
	virtual const string& GetGlobalCircuitFileName() const { return m_CircuitFileName;}
	virtual const string& GetGlobalRaportFileName() const { return m_RaportFileName;}
	_CFlatVertex* PushIntoCache(_CFlatVertex* pObject);
	//_CFlatVerticesResContainer* FindExisting(_CFlatVerticesResContainer* pContainer)
	//{
	//	return dynamic_cast<_CFlatVerticesResContainer*>(m_FlatVericesResCache.FindExisting(pContainer));
	//}
	_CFlatVerticesResContainer2* FindExisting(_CFlatVerticesResContainer2* pContainer)
	{
		return dynamic_cast<_CFlatVerticesResContainer2*>(m_FlatVericesResCache2.FindExisting(pContainer));
	}
	void RegisterFlatVerticesResContainer2(_CFlatVerticesResContainer2* pContainer)
	{
		m_FlatVericesResCache2.RegisterResult(pContainer);
	}
	_CFlatVertex* RegisterFlatVertex(_CFlatVertex* pContainer)
	{
		return m_FlatVertexCache.PushInCache(pContainer,true);
	}
	//_CFlatVerticesResContainer* FindOrRegisterResult(_CFlatVerticesResContainer* pContainer)
	//{
	//	return dynamic_cast<_CFlatVerticesResContainer*>(m_FlatVericesResCache.FindOrRegisterResult(pContainer));
	//}
	//void RegisterResult(_CFlatVerticesResContainer* pContainer)
	//{
	//	m_FlatVericesResCache.RegisterResult(pContainer);
	//}
	const COMPONENTS_PATH& GetUnique(const COMPONENTS_PATH& Source)
	{
		return m_CompPathStorage.GetUnique(Source);
	}
	//void NotifySkipped(const COMPONENTS_PATH* pPath)
	//{
	//	m_FlatVertexCache.NotifySkipped(pPath);
	//}
	_CSubModelNumericPattern* GetPatrernIfExist(const _CModelVerticesPath& Path)
	{
		return m_SubModelNumericParrernStorage.GetPatrernIfExist(Path);
	}
	bool ForcePattern(const _CModelVerticesPath& Path, _CSubModelNumericPattern*& pOutPattern)
	{
		return m_SubModelNumericParrernStorage.ForcePattern(Path,pOutPattern);
	}
	bool UnTouchFlatVertex(const _CFlatVertex* pVertex)
	{
		bool Res=false;
		m_FlatVertexCache.UnTouchVertex(pVertex,Res);
		return Res;
	}
	void TouchFlatVertex(const _CFlatVertex* pVertex)
	{
		m_FlatVertexCache.TouchVertex(pVertex);
	}
	long DefineTransferFunctionSimp(const string& Name,unsigned  long NumeratorCof, unsigned  long  DenominatorCof);
	long RaportTransferFunctionValuesLog(long long hTransfer, const string& Context, int LowestDec, int HighestDec, size_t NoOfPointsPerDec, bool Transfer2MagPhase = false);
	long RaportTransferFunctionValuesLin(long long hTransfer, const string& Context, long double LowestAngFreq, long double HighestAngFreq, size_t NoOfGlobalPoints, bool Transfer2MagPhase = false);
	long AddExtraTransferFunctionSimp(long PairId, unsigned long DenominatorCof);
	const string& GetModelFileName() { return m_ModelFileName; }
	const string& GetCircuitFileName() { return m_CircuitFileName; }
	const string& GetRaportFileName() { return m_RaportFileName; }
protected:
	long RaportTransferFunctionValues(const string& Name, const string& Context, NumericVector& FreqVector, size_t NumId, size_t DenId, bool Transfer2MagPhase);
	virtual void PrepareNewCircuitAnalyser() override
	{
		m_pNewAnalyxer=new _CCircuitAnalyzer(this,m_IdealComponentsList,m_LocalComponentsList,true,NULL,
			m_Cofactors);
	}
	string m_ModelFileName;
	string m_CircuitFileName;
	string m_RaportFileName;
	_CPathTraitor& GetExistedPathTraitor(long TrId);
	//void RegisterInFile(const char* FileName,string ParameterName,long double Value);
	//long DeterminPathWeights(unsigned long CofId, unsigned short sPower, const string& Context, long double& NumericalResult,_CLSPath& Path);
	long FindSubId(const string& Name);
	typedef pair<long,_CSubCircuit*> SUBCIRCUIT_DATA;
	typedef hash_map<string, SUBCIRCUIT_DATA> SUBCIRCUIT_MAP;
	typedef SUBCIRCUIT_MAP::iterator SUBCIRCUIT_ITERATOR;
	typedef SUBCIRCUIT_MAP::reverse_iterator SUBCIRCUIT_RITERATOR;
	typedef SUBCIRCUIT_MAP::value_type SUBCIRCUIT_VALUE_TYPE;
	SUBCIRCUIT_MAP m_SubcircuitMap;
	long m_SubcircuitCounter;
	typedef hash_map<long,string> INDECES_MAP;
	typedef INDECES_MAP::iterator INDECES_ITERATOR;
	typedef INDECES_MAP::reverse_iterator INDECES_RITERATOR;
	typedef INDECES_MAP::value_type INDECES_VALUE_TYPE;
	INDECES_MAP m_IndecesMap;
	_CPDDEngine* m_pEngine;
	//_CNumericalSCofactors m_NumericalSCofactors;
	//_CSPowerCoeffOperator::_CResCache m_SPowerCoeffCache;
	_CVertexCofactorReminderOperator::_CResCache m_VCRCache;
	//_CPathWeightOperator::_CResCache m_WeightCache;
	//_CCofRemResultCache m_CofRemResultCache;
	_CConstComplementStore m_ConstPathStore;
	_CComplementStore m_PathStore;
	_CContextStringBuffer m_ContextStrings;
	_CSuplementStorage m_DataSuplements;
	//_CCofRemResultStorage m_ResNumericalSCofactors;
	//typedef _CAbstractResTables<_CWeightResult> WEIGHT_RES;
	//WEIGHT_RES m_WeightRes;
	typedef _CNumericalContainer<NumericType> DOUBLE_RES;
	typedef _CNumResTable<NumericType> DOUBLE_RES_TABLE;
	DOUBLE_RES_TABLE m_NumResults;
	_CSExpandedUniqeSet m_SExpandedUniqueSet;
	//typedef hash_map<_CModelVertex*,_CSPolynomial> NUMERIC_POLYNOMIAL;
	//typedef hash_map<_CModelVertex*,_CSparsePolynomial> NUMERIC_POLYNOMIAL;
	//class NUM_POLY_ITEM_ID : public pair<const string*,_CModelVertex*>
	//{
	//public:
	//	NUM_POLY_ITEM_ID(const string* Context,_CModelVertex* pVertex):
	//	  pair<const string*,_CModelVertex*>(Context,pVertex) {}
	//	  operator size_t() const {return (((size_t)HASH_CORE*HASH_FACTOR)^(size_t)first)*HASH_FACTOR^(size_t)second;}
	//};
	typedef pair<const string*,_CModelVertex*> NUM_POLY_ITEM_ID;
	typedef map<NUM_POLY_ITEM_ID,_CSparsePolynomial> NUMERIC_POLYNOMIAL;
	NUMERIC_POLYNOMIAL m_NumericPolynomials;
	typedef hash_map<long,_CPathTraitor> PATH_TRAITORS;
	PATH_TRAITORS m_PathTraitors;
	//_CPathTraitor::FOLLOWER m_TestFollower;
	//_CDirectNumCache m_DirNumCache;
	_CFlatVertexCache m_FlatVertexCache;
	_CContextSExpFlatVertices m_ContextSExpFlatVertices;
	//_CFlatVerticesResCache m_FlatVericesResCache;
	_CFlatVerticesResCache2 m_FlatVericesResCache2;
	_CCompPathStorage m_CompPathStorage;
	_CSubModelNumericParrernStorage m_SubModelNumericParrernStorage;
	_CFlatResultFactorizer m_FlatResultFactorizer;
	friend class _CNewSimplifierData;
public:
	void WriteSimplifiedTransferFunctions(txt_stream& file) {m_FlatResultFactorizer.WriteTransferFunctions(file);}
	void WriteSimplifiedTransferFunction(size_t TrId,txt_stream& file) {m_FlatResultFactorizer.WriteTransferFunction(TrId,file);}
};

