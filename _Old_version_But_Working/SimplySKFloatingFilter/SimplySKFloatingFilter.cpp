// SimplySKFloatingFilter.cpp : Defines the entry point for the console application.
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

#define _CFMB
#define _ASYMETRIC true
#define _FLOAT_GROUND true

#define _Kud1 1
#define _Kud2 0 
#define _Kuc 0

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		CPDDEngineManager Manager; //Enginer managers instance
		long hEngine = Manager.GetPDDEngine(0, true, true); //Creating Engine. 0 - the number of engine (can be more), true - s-expanded form, true - order optymalization
		long long hCircuit = Manager.GetNewCircuit(hEngine); //The main circuit initialization	
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);// The output results file
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");//Errors, performance time, info file
		Manager.DeclareGlobalRefNode(hCircuit, "0");// Declaration of the reference node. Not obligatory. However, it simplifies models of idealized  active devices

		//if (Manager.SetCompPriority(hCircuit, 0) < 0)
		//	PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "k", "7", "8", "5", "6", 0, 10) < 0)
			PERFORM_ERROR;
		//if (Manager.ClearCompPriority(hCircuit) < 0)
		//	PERFORM_ERROR;
#ifdef _CFMB
		if (Manager.AddVoltageFollower(hCircuit, "0", "7", "8", "0") < 0)
			PERFORM_ERROR;
#endif
		if (Manager.AddResistor(hCircuit, _ASYMETRIC ? "R1a" : "R1", "1", "3", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _ASYMETRIC ? "R1b" : "R1", "2", "4", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.SetCompPriority(hCircuit, 0) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _ASYMETRIC ? "R2a" : "R2", "3", "5", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _ASYMETRIC ? "R2b" : "R2", "4", "6", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.ClearCompPriority(hCircuit) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, _ASYMETRIC ? "C1a" : "C1", "3", "7", 1e-9) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, _ASYMETRIC ? "C1b" : "C1", "4", "8", 1e-9) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.SetCompPriority(hCircuit, 0) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, _ASYMETRIC ? "C2a" : "C2", "5", _FLOAT_GROUND ? "g" : "0", 1e-9) < 0) //VCVS definition. 0 is power of s operator to model some dynamic. Here - no dynamic
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, _ASYMETRIC ? "C2b" : "C2", "6", _FLOAT_GROUND ? "g" : "0", 1e-9) < 0) //VCVS definition. 0 is power of s operator to model some dynamic. Here - no dynamic
			PERFORM_ERROR;
		if (Manager.ClearCompPriority(hCircuit) < 0)
			PERFORM_ERROR;

#if _Kud1
		long long hCofactorD_D = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } } });
		if (hCofactorD_D < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)
		long long hCofactorD_Dt = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "7", "0" }, { "7", "0" } } });
		if (hCofactorD_Dt < 0)
			PERFORM_ERROR;
		long long hCofactorD_Dt2 = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "8", "0" }, { "8", "0" } } });
		if (hCofactorD_Dt2 < 0)
			PERFORM_ERROR;


		long long hCofactorN_D = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "7", "8" } }});
		if (hCofactorN_D < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (2)
#endif
#if _Kud2 || _Kuc
		long long hCofactorD_C = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "2", "0" } } });
		if (hCofactorD_C < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (3)
#endif
#if _Kuc
		long long hCofactorN_C = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "7", "8" } } });
		if (hCofactorN_C < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (4)
#endif
#if _Kud2
		long long hCofactorN_DS = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "7", "8" } }, { { "2", "0" }, { "2", "-1" } } });
		if (hCofactorN_DS < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (5)
#endif
#if _Kud1
		long long K_ud1 = Manager.DefineTransferFunctionSimp("K_ud1", hCofactorN_D, hCofactorD_D);
		if (K_ud1<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 1. See (6)
#endif
#if _Kud2
		long long K_ud2 = Manager.DefineTransferFunctionSimp("K_ud2", hCofactorN_DS, hCofactorD_C);
		if (K_ud2<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 2. See (7)
#endif
#if _Kuc
		long long K_uc = Manager.DefineTransferFunctionSimp("K_uc", hCofactorN_C, hCofactorD_C);
		if (K_uc<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 2. See (8)
#endif

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
