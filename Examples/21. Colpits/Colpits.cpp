// Hartley.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define PERFORM_ERROR 		{\
			string str;\
									while((str=(string)LastErrorInfo)!="")\
				printf("%s\n",str.c_str());\
			goto end_of;\
								}

#define ACC 0.0000000000

#define CONDUCTANCES

#define RANGE 0

//#define FORCED

//#define MACRO

int _tmain(int argc, _TCHAR* argv[])
{
#if _DEBUG
	_CrtMemState cms1, cms2, cms3;
	_CrtMemCheckpoint(&cms1);
#endif
	try
	{
		int dst = numeric_limits<size_t>::digits;
		int dul = numeric_limits<unsigned long long>::digits;

		CPDDEngineManager Manager;
#ifdef FORCED
		long hEngine = Manager.GetPDDEngine(0, true, true, true);
#else
		long hEngine = Manager.GetPDDEngine(0, true, true, true);
#endif
		long long hCircuit = Manager.GetNewCircuit(hEngine);
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");
		Manager.DeclareGlobalRefNode(hCircuit, "0");

		if (Manager.SetCompPriority(hCircuit, 0) < 0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "R_B", "B", "0", 1e3, false) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C1", "B", "0", 1e-3) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C2", "C", "0", 1e-3) < 0)
			PERFORM_ERROR;
		if (Manager.AddInductor(hCircuit, "L_0", "B", "C", 1e-9) < 0)
			PERFORM_ERROR;
		if (Manager.ClearCompPriority(hCircuit) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R_E", "E", "0", 100, false) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "h_ix", "B", "E'", 750, false) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "h_fe", "C", "E", "E'", "E", 0, 100) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "h_oe", "C", "E", 750, true) < 0)
			PERFORM_ERROR;

		long long hCofactor_D = Manager.CreateNewCofactor(hCircuit);
		if (hCofactor_D < 0)
			PERFORM_ERROR;



		if (Manager.DoTheCalculation(hCircuit)<0)
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0)
			PERFORM_ERROR;

		//if(Manager.WriteDirectNmericalPolynomial(hCofactor_aa,EmptyString,"Models.txt",true,false,RANGE)<0)
		//	PERFORM_ERROR;
		//if(Manager.WriteDirectNmericalPolynomial(hCofactor_ab,EmptyString,"Models.txt",true,false,RANGE)<0)
		//	PERFORM_ERROR;

		long PT = 0;
		if ((PT = Manager.GetFreePathTraitor(hCircuit))<0)
			PERFORM_ERROR;
		if (Manager.SetApproximationCriterion(hCircuit, PT, true, ACC, RANGE, 0)<0)
			PERFORM_ERROR;
		if (Manager.WriteFlatSimplifiedPDD(hCircuit, PT, true, true, "Models.txt")<0)
			PERFORM_ERROR;
		//if(Manager.WriteFlatSimplifiedFeasibleDividers(hCircuit,PT,"Models.txt")<0)
		//	PERFORM_ERROR;
		//if(Manager.WriteFlatSimplifiedSoP(hCircuit,PT,true,"Models.txt")<0)
		//	PERFORM_ERROR;
		//if(Manager.TestFlatPDDRemoval(hCircuit,PT)<0)
		//	PERFORM_ERROR;
		//if(Manager.WriteFlatSimplifiedPDD(hCircuit,PT,"Models.txt")<0)
		//	PERFORM_ERROR;
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
