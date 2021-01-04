// RCLeadder.cpp : Defines the entry point for the console application.
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

		_CStrNodes Boundary({"I","O"});
		long hMacro = Manager.CreateNewSubcircuit(hCircuit, "M", Boundary, nullptr, true);
		if (hMacro < 0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "R", "I", "2", 1e3, hMacro, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R", "2", "O", 1e3, hMacro, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		//if (Manager.AddResistor(hCircuit, "R", "3", "4", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
		//	PERFORM_ERROR;
		//if (Manager.AddResistor(hCircuit, "R", "4", "5", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
		//	PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C", "2", "0", 1e-9,hMacro) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C", "O", "0", 1e-9,hMacro) < 0)
			PERFORM_ERROR;
		//if (Manager.AddCapacitor(hCircuit, "C", "4", "0", 1e-9) < 0)
		//	PERFORM_ERROR;
		//if (Manager.AddCapacitor(hCircuit, "C", "5", "0", 1e-9) < 0)
		//	PERFORM_ERROR;
		_CStrNodes Connections({"I = I", "O = O"});
		if (Manager.IncludeSubcircuit(hCircuit, "M", hMacro, Connections) < 0)
			PERFORM_ERROR;

		long long hCofactorD = Manager.CreateNewCofactor(hCircuit, { { { "I", "0" }, { "I", "0" } } });
		if (hCofactorD < 0)
			PERFORM_ERROR;
		long long hCofactorN = Manager.CreateNewCofactor(hCircuit, { { { "I", "0" }, { "O", "0" } } });
		if (hCofactorN < 0)
			PERFORM_ERROR;

		long long K_ud = Manager.DefineTransferFunctionSimp("K_ud1", hCofactorN, hCofactorD);
		if (K_ud<0)
			PERFORM_ERROR;


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

