// TT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define PERFORM_ERROR 		{\
			string str;\
															while((str=(string)LastErrorInfo)!="")\
				printf("%s\n",str.c_str());\
			goto end_of;\
																																}

#define _ALL
#define ACC 0.0000000000

#define CONDUCTANCES

#define RANGE 1e9

#define _GM

#define _PAR_CAP

#define _BETA
//#define FORCED

#define _CONDUCTANCE true 

int _tmain(int argc, _TCHAR* argv[])
{
#if _DEBUG
	_CrtMemState cms1, cms2, cms3;
	_CrtMemCheckpoint(&cms1);
#endif
	try
	{

		CPDDEngineManager Manager;
		long hEngine = Manager.GetPDDEngine(0, true, true, true);
		long long hCircuit = Manager.GetNewCircuit(hEngine);
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");
		Manager.DeclareGlobalRefNode(hCircuit, "0");

		if (Manager.AddResistor(hCircuit, _CONDUCTANCE ? "G" : "R1", "1", "3", _CONDUCTANCE?1e-3: 1e3, _CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _CONDUCTANCE ? "G" : "R2", "2", "3", _CONDUCTANCE ? 1e-3 : 1e3, _CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _CONDUCTANCE ? "n*G" : "R3", "4", "0", _CONDUCTANCE ? 1e-3 : 1e3, _CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C", "1", "4", 1e-6) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C", "2", "4", 1e-6) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "n*C", "3", "0", 1e-6) < 0)
			PERFORM_ERROR;



		long long hCofacotr_Dab = Manager.CreateNewCofactor(hCircuit);
		if (hCofacotr_Dab < 0)
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofacotr_Dab, "1", "0", "2", "0") < 0)
			PERFORM_ERROR;

		long long hCofacotr_Daa = Manager.CreateNewCofactor(hCircuit);
		if (hCofacotr_Daa < 0)
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofacotr_Daa, "1", "0", "1", "0") < 0)
			PERFORM_ERROR;

		if (Manager.DefineTransferFunctionSimp("K_u", hCofacotr_Dab, hCofacotr_Daa) < 0)
			PERFORM_ERROR;

		if (Manager.DoTheCalculation(hCircuit)<0)
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0)
			PERFORM_ERROR;

		//if (Manager.WriteDirectNmericalPolynomial(hCofactor_D, EmptyString, "Models.txt", true, false, true, RANGE)<0)
		//	PERFORM_ERROR;
		//if (Manager.WriteDirectNmericalPolynomial(hCofactor_LP, EmptyString, "Models.txt", true, false, true, RANGE)<0)
		//	PERFORM_ERROR;
		//if (Manager.WriteDirectNmericalPolynomial(hCofactor_BP, EmptyString, "Models.txt", true, false, true, RANGE)<0)
		//	PERFORM_ERROR;
		//if (Manager.WriteDirectNmericalPolynomial(hCofactor_HP, EmptyString, "Models.txt", true, false, true, RANGE)<0)
		//	PERFORM_ERROR;

		long PT = 0;
		if ((PT = Manager.GetFreePathTraitor(hCircuit))<0)
			PERFORM_ERROR;
		if (Manager.SetApproximationCriterion(hCircuit, PT, true)<0)
			PERFORM_ERROR;
		if (Manager.WriteFlatSimplifiedPDD(hCircuit, PT, true, true, "Models.txt")<0)
			PERFORM_ERROR;
		if (Manager.ReleasePathTraitor(hCircuit, PT)<0)
			PERFORM_ERROR;
	}
	catch (exception& e)
	{
		cout << "Uncatced exception: " << e.what();
	}
end_of:
#if _DEBUG
	_CrtMemCheckpoint(&cms2);
	if (_CrtMemDifference(&cms3, &cms1, &cms2))
		_CrtMemDumpStatistics(&cms3);
#endif
	return 0;
}
