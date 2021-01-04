// SimplFloatAmp.cpp : Defines the entry point for the console application.
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

#define CFMB
//#define TRACKING_ERROR
//#define SYMETRIC
//#define ONLY_ONE

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		CPDDEngineManager Manager; //Enginer managers instance
		long hEngine = Manager.GetPDDEngine(0, true, true); //Creating Engine. 0 - the number of engine (can be more), true - s-expanded form, true - order optymalization
		long long hCircuit = Manager.GetNewCircuit(hEngine); //The main circuit initialization	
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);// The output results file
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");//Errors, performance time, info file
#ifndef ONLY_ONE
		Manager.DeclareGlobalRefNode(hCircuit, "0");// Declaration of the reference node. Not obligatory. However, it simplifies models of idealized  active devices
#endif

		//if (Manager.SetCompPriority(hCircuit, 0) < 0)
		//	PERFORM_ERROR;
		if (Manager.AddNullor(hCircuit, "O1", "O2", "I1", "I2") < 0)
			PERFORM_ERROR;
#ifdef CFMB
#ifdef TRACKING_ERROR
		if (Manager.AddVoltageGain(hCircuit, "\\alpha", "0", "O2", "O1", "0", 0, 1) < 0)
			PERFORM_ERROR;
#else
		if (Manager.AddVoltageFollower(hCircuit, "0", "O2", "O1", "0") < 0)
			PERFORM_ERROR;
#endif
#endif
#ifdef SYMETRIC
		if (Manager.AddResistor(hCircuit, "R1", "In1", "I1", 100, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2", "I1", "O1", 1e6, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		//if (Manager.ClearCompPriority(hCircuit) < 0)
		//	PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R1", "In2", "I2", 10, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2", "I2", "O2", 10, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		//if (Manager.SetCompPriority(hCircuit, 0) < 0)
		//	PERFORM_ERROR;
		//if (Manager.ClearCompPriority(hCircuit) < 0)
		//	PERFORM_ERROR;
#else
		if (Manager.AddResistor(hCircuit, "R1a", "In1", "I1", 100, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2a", "I1", "O1", 1e6, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		//if (Manager.ClearCompPriority(hCircuit) < 0)
		//	PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R1b", "In2", "I2", 10, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2b", "I2", "O2", 10, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		//if (Manager.SetCompPriority(hCircuit, 0) < 0)
		//	PERFORM_ERROR;
		//if (Manager.ClearCompPriority(hCircuit) < 0)
		//	PERFORM_ERROR;
#endif

		long long hCofactorDaa1 = Manager.CreateNewCofactor(hCircuit, { { { "In1", "In2" }, { "In1", "In2" } } });
		if (hCofactorDaa1 < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)

		long long hCofactorNab1 = Manager.CreateNewCofactor(hCircuit, { { { "In1", "In2" }, { "O1", "O2" } } });
		if (hCofactorNab1 < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)
#ifndef ONLY_ONE
		long long hCofactorDaac1 = Manager.CreateNewCofactor(hCircuit, { { { "In1", "0" }, { "In1", "0" } }, { { "In1", "In2" }, { "In1", "In2" } } });
		if (hCofactorDaac1 < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)

		long long hCofactorNabc1 = Manager.CreateNewCofactor(hCircuit, { { { "In1", "0" }, { "O1", "O2" } }, { { "In1", "In2" }, { "In1", "In2" } } });
		if (hCofactorNabc1 < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)

		long long hCofactorNab2 = Manager.CreateNewCofactor(hCircuit, { { { "In1", "0" }, { "O1", "O2" } }, { { "In2", "0" }, { "In1", "-In2" } } });
		if (hCofactorNab2 < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)
#endif
		long long K_u1 = Manager.DefineTransferFunctionSimp("K_ud1", hCofactorNab1, hCofactorDaa1);
		if (K_u1<0)
			PERFORM_ERROR;
		//Volatge to voltage numerator HOSC. See (6)
#ifndef ONLY_ONE
		long long K_uc = Manager.DefineTransferFunctionSimp("K_uc", hCofactorNabc1, hCofactorDaac1);
		if (K_uc<0)
			PERFORM_ERROR;
		//Volatge to voltage numerator HOSC. See (6)

		long long K_ud2 = Manager.DefineTransferFunctionSimp("K_ud2", hCofactorNab2, hCofactorDaac1);
		if (K_ud2<0)
			PERFORM_ERROR;
		//Volatge to voltage numerator HOSC. See (6)
#endif
		if (Manager.DoTheCalculation(hCircuit)<0) //The nain circuit compution
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0) // WWrite down PDD structure to the file
			PERFORM_ERROR;

		//if (Manager.WriteDirectNmericalPolynomial(hCofactorD, EmptyString, "Models.txt", true, false, true, RANGE)<0)
		//	PERFORM_ERROR;
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
