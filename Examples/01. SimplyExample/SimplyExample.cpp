// SimplyExample.cpp : Defines the entry point for the console application.
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

		if (Manager.AddResistor(hCircuit, "R_s", "1", "2", 100, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R_i", "2", "5", 1e6, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R_o", "3", "4", 10, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R_f", "5", "0", 100, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C_i", "2", "5", 1e-9) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C_f", "2", "3", 1e-12) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "A", "5", "3", "2", "5", 0, 10) < 0) //VCVS definition. 0 is power of s operator to model some dynamic. Here - no dynamic
			PERFORM_ERROR;


		long long hCofactorD = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "1", "0" } }, { { "4", "5" }, { "4", "5" } } });
		if (hCofactorD < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)


		long long hCofactorN_vo_vi = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "3", "5" } }, { { "4", "5" }, { "4", "5" } } });
		if (hCofactorN_vo_vi < 0)
			PERFORM_ERROR;
		//Volatge to voltage numerator HOSC. See (2)

		long long hCofactorN_vo_ii = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "1", "0" } }, { { "4", "5" }, { "4", "5" } }, { {"2", "0" }, {"3", "5" } } });
		if (hCofactorN_vo_ii < 0)
			PERFORM_ERROR;
		//Volatge to current numerator HOSC. See (3)

		long long hCofactorN_io_vi = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "4", "5" } } });
		if (hCofactorN_io_vi < 0)
			PERFORM_ERROR;
		//Current to voltage numerator HOSC. See (4)

		long long hCofactorN_io_ii = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "1", "0" } }, { { "2", "0" }, { "4", "5" } } });
		if (hCofactorN_io_ii < 0)
			PERFORM_ERROR;
		//Current to current numerator HOSC. See (5)


		long long K_vo_vi = Manager.DefineTransferFunctionSimp("K_vo_vi", hCofactorN_vo_vi, hCofactorD);
		if (K_vo_vi<0)
			PERFORM_ERROR;
		//Volatge to voltage numerator HOSC. See (6)

		long long M_vo_ii = Manager.DefineTransferFunctionSimp("M_vo_ii", hCofactorN_vo_ii, hCofactorD);
		if (M_vo_ii<0)
			PERFORM_ERROR;
		//Volatge to current numerator HOSC. See (7)

		long long N_io_vi = Manager.DefineTransferFunctionSimp("N_io_vi", hCofactorN_io_vi, hCofactorD);
		if (N_io_vi<0)
			PERFORM_ERROR;
		//Current to voltage numerator HOSC. See (8)

		long long K_io_ii = Manager.DefineTransferFunctionSimp("K_io_ii", hCofactorN_io_ii, hCofactorD);
		if (K_io_ii<0)
			PERFORM_ERROR;
		//Current to current numerator HOSC. See (9)

		if (Manager.DoTheCalculation(hCircuit)<0) //The nain circuit compution
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0) // WWrite down PDD structure to the file
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorD, EmptyString, "Models.txt", true, false, true, RANGE)<0)
			PERFORM_ERROR;
		//The numerical denominator s-Expanded form calculation, true - append results to the file, false - do not sensitivity analysis, true - determine roots of polynomial

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
