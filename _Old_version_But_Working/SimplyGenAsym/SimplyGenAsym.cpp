// SimplyGenAsym.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define PERFORM_ERROR 		{\
			string str;\
																																				while((str=(string)LastErrorInfo)!="")\
				printf("%s\n",str.c_str());\
			goto end_of;\
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																}

#define ACC 0.001
#define LOW_FREQ 0
#define HIGH_FREQ 1e6

//#define RES

#define _NUMERATORS

#define MAIN_CRICUIT_AS_A_MODEL

#define RANGE 0

using  ModelType = enum{ ideal, trucking_sym, trucking_asym };
const ModelType ModelTypeInstance = ideal;

//#define _CFMB_OUT

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		CPDDEngineManager Manager; //Enginer managers instance
		long hEngine = Manager.GetPDDEngine(0, true, true); //Creating Engine. 0 - the number of engine (can be more), true - s-expanded form, true - order optymalization
		long long hCircuit = Manager.GetNewCircuit(hEngine); //The main circuit initialization	
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);// The output results file
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");//Errors, performance time, info file
		Manager.DeclareGlobalRefNode(hCircuit, "0");// Declaration of the reference node. Not obligatory. 
		// However, it force 0 node to each defined subcircuit. Here omitted to check the fully floating VCCS

		if (Manager.AddResistor(hCircuit, "R1a", "1", "3", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R1b", "2", "4", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2a", "3", "5", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2b", "4", "6", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C1a", "3", "7", 1e-9) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C1b", "4", "8", 1e-9) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C2a", "5", "g", 1e-9) < 0) //VCVS definition. 0 is power of s operator to model some dynamic. Here - no dynamic
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C2b", "6", "g", 1e-9) < 0) //VCVS definition. 0 is power of s operator to model some dynamic. Here - no dynamic
			PERFORM_ERROR;
		// Test if the model works. It works, excepte wrong value of k. However, in original Sanchez-Lopez example, 
		// node g was connected to the FDCFOA reference node, which was wrong.

		long long hCofactorD_D0 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorD_D0 < 0)
			PERFORM_ERROR;
		long long hCofactorD_D1 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "7", "0" }, { "8", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorD_D1 < 0)
			PERFORM_ERROR;
		long long hCofactorD_D2 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "8", "0" } } });
		if (hCofactorD_D2 < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)


		long long hCofactorN_D1 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "7", "8" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorN_D1 < 0)
			PERFORM_ERROR;
		long long hCofactorN_D2 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "7", "8" } }, { { "7", "0" }, { "8", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorN_D2 < 0)
			PERFORM_ERROR;
		long long hCofactorN_D3 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "7", "8" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "8", "0" } } });
		if (hCofactorN_D3 < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (2)

		long long hCofactorD_C1 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "2", "0" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorD_C1 < 0)
			PERFORM_ERROR;
		long long hCofactorD_C2 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "2", "0" } }, { { "7", "0" }, { "8", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorD_C2 < 0)
			PERFORM_ERROR;
		long long hCofactorD_C3 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "2", "0" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "8", "0" } } });
		if (hCofactorD_C3 < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (3)

		long long hCofactorN_C1 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "7", "8" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorN_C1 < 0)
			PERFORM_ERROR;
		long long hCofactorN_C2 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "7", "8" } }, { { "7", "0" }, { "8", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorN_C2 < 0)
			PERFORM_ERROR;
		long long hCofactorN_C3 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "7", "8" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "8", "0" } } });
		if (hCofactorN_C3 < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (4)

		long long hCofactorN_DS1 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "7", "8" } }, { { "2", "0" }, { "2", "-1" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorN_DS1 < 0)
			PERFORM_ERROR;
		long long hCofactorN_DS2 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "7", "8" } }, { { "2", "0" }, { "2", "-1" } }, { { "7", "0" }, { "8", "0" } }, { { "8", "0" }, { "5", "6" } } });
		if (hCofactorN_DS2 < 0)
			PERFORM_ERROR;
		long long hCofactorN_DS3 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "7", "8" } }, { { "2", "0" }, { "2", "-1" } }, { { "7", "0" }, { "7", "0" } }, { { "8", "0" }, { "8", "0" } } });
		if (hCofactorN_DS3 < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (5)


		if (Manager.DoTheCalculation(hCircuit)<0) //The nain circuit compution
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0) // WWrite down PDD structure to the file
			PERFORM_ERROR;

		//if (Manager.WriteDirectNmericalPolynomial(hCofactorD_D, EmptyString, "Models.txt", true, false, true, RANGE)<0)
		//	PERFORM_ERROR;
		// The numerical denominator s-Expanded form calculation, true - append results to the file, false - do not sensitivity analysis, true - determine roots of polynomial

		long PT = 0;
		if ((PT = Manager.GetFreePathTraitor(hCircuit))<0) // Initialization of internal structure for simplifiaction and SoE form generation. Obligatory even in case of exact analysis
			PERFORM_ERROR;
		if (Manager.SetApproximationCriterion(hCircuit, PT, true, ACC, HIGH_FREQ, LOW_FREQ)<0) // SoE form gheneration for exact forulae (true)
			PERFORM_ERROR;
		if (Manager.WriteFlatSimplifiedPDD(hCircuit, PT, true, true, "Models.txt")<0) // Writting result to the file
			PERFORM_ERROR;
		if (Manager.ReleasePathTraitor(hCircuit, PT)<0) //Internal structure realiseing
			PERFORM_ERROR;
	}
	catch (exception& e)
	{
		cout << "Uncatced exception: " << e.what();
	}
end_of:
	return 0;
}

