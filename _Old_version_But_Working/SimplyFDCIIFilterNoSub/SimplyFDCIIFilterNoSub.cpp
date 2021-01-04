// SimplyFDCIIFilterNoSub.cpp : Defines the entry point for the console application.
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

using  ModelType = enum{ ideal, tracking };
const ModelType ModelTypeInstance = tracking;
const bool _RESISTORS = false;
const bool _SYMETRIC = false;
const bool _SIMP = false;
const bool _opt = true;
int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		CPDDEngineManager Manager; //Enginer managers instance
		long hEngine = Manager.GetPDDEngine(0, true, _opt); //Creating Engine. 0 - the number of engine (can be more), true - s-expanded form, true - order optymalization
		long long hCircuit = Manager.GetNewCircuit(hEngine); //The main circuit initialization	
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);// The output results file
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");//Errors, performance time, info file
		Manager.DeclareGlobalRefNode(hCircuit, "0");// Declaration of the reference node. Not obligatory. However, it simplifies models of idealized  active devices

		_CStrNodes OutBoundaryNodes({ "y1", "y2", "y3", "y4", "x1", "x2", "z1", "z2" });

		long hFDCIIId = Manager.CreateNewSubcircuit(hCircuit, "FDCII_Id", OutBoundaryNodes, nullptr, true);
		if (hFDCIIId < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "1", "y3", "y1", "y2", hFDCIIId) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "y4", "2", "y1", "y2", hFDCIIId) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "3", "0", "1", "0", hFDCIIId) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "4", "0", "2", "0", hFDCIIId) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentFollower(hCircuit, "z1", "0", "x1", "3", hFDCIIId) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentFollower(hCircuit, "z2", "0", "x2", "4", hFDCIIId) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hFDCIIId) < 0)
			PERFORM_ERROR;



		long hFDCIITE = Manager.CreateNewSubcircuit(hCircuit, "FDCII_TE", OutBoundaryNodes, nullptr, true);
		if (hFDCIITE < 0)
			PERFORM_ERROR;
		//if (Manager.AddVoltageGain(hCircuit, "\\alpha_12", "1", "y3", "y1", "y2", 0, 1, hFDCIITE) < 0)
		//	PERFORM_ERROR;
		//if (Manager.AddVoltageGain(hCircuit, "\\alpha_12", "y4", "2", "y1", "y2", 0, 1, hFDCIITE) < 0)
		//	PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_1", "1'", "y3", "y1", "0", 0, 1, hFDCIITE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_2", "1'", "1", "y2", "0", 0, 1, hFDCIITE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_1", "y4", "2'", "y1", "0", 0, 1, hFDCIITE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_12", "2", "2'", "y2", "0", 0, 1, hFDCIITE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_3", "3", "0", "1", "0", 0, 1, hFDCIITE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_4", "4", "0", "2", "0", 0, 1, hFDCIITE) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta_1", "z1", "0", "x1", "3", 0, 1, hFDCIITE) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta_2", "z2", "0", "x2", "4", 0, 1, hFDCIITE) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hFDCIITE) < 0)
			PERFORM_ERROR;

		//long hSubModel = Manager.CreateNewSubcircuit(hCircuit, "Sub", OutBoundaryNodes, nullptr, true);
		//if (hSubModel < 0)
		//	PERFORM_ERROR;
		//_CStrNodes Connections({ "y1 = y1", "y2 = y2", "y3 = y3", "y4 = y4", "x1 = x1", "x2 = x2", "z1 = z1", "z2 = z2" });
		//if (Manager.IncludeSubcircuit(hCircuit, "S", ModelTypeInstance == ideal ? hFDCIIId : hFDCIITE, hSubModel, Connections) < 0)
		//	PERFORM_ERROR;
		//if (Manager.AddResistor(hCircuit, _RESISTORS ? (_SYMETRIC ? "R" : "Ra") : (_SYMETRIC ? "G" : "Ga"), "x1", "0", 1e3, hSubModel, !_RESISTORS) < 0)
		//	PERFORM_ERROR;
		//if (Manager.AddResistor(hCircuit, _RESISTORS ? (_SYMETRIC ? "R" : "Rb") : (_SYMETRIC ? "G" : "Gb"), "x2", "0", 1e3, hSubModel, !_RESISTORS) < 0)
		//	PERFORM_ERROR;
		//if (Manager.ForceModelCalculation(hCircuit, hSubModel) < 0)
		//	PERFORM_ERROR;

		long hChosenModel = ModelTypeInstance == ideal ? hFDCIIId : hFDCIITE;

		_CStrNodes Connections1({ "y1 = 1", "y2 = 2", "y3 = 4", "y4 = 3", "x1 = 8", "x2 = 7", "z1 = 3", "z2 = 4" });
		_CStrNodes Connections2({ "y1 = 0", "y2 = 0", "y3 = 3", "y4 = 4", "x1 = 10", "x2 = 9", "z1 = 5", "z2 = 6" });
		_CStrNodes Connections3({ "y1 = 0", "y2 = 0", "y3 = 5", "y4 = 6", "x1 = 12", "x2 = 11", "z1 = 8", "z2 = 7" });
		if (Manager.IncludeSubcircuit(hCircuit, "A", hChosenModel, Connections1) < 0)
			PERFORM_ERROR;
		if (Manager.IncludeSubcircuit(hCircuit, "B", hChosenModel, Connections2) < 0)
			PERFORM_ERROR;
		if (Manager.IncludeSubcircuit(hCircuit, "C", hChosenModel, Connections3) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _RESISTORS ? (_SYMETRIC ? "R1" : "R1a") : (_SYMETRIC ? "G1" : "G1a"), "8", "0", 1e3, !_RESISTORS) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _RESISTORS ? (_SYMETRIC ? "R1" : "R1b") : (_SYMETRIC ? "G1" : "G1b"), "7", "0", 1e3, !_RESISTORS) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _RESISTORS ? (_SYMETRIC ? "R2" : "R2a") : (_SYMETRIC ? "G2" : "G2a"), "10", "0", 1e3, !_RESISTORS) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _RESISTORS ? (_SYMETRIC ? "R2" : "R2b") : (_SYMETRIC ? "G2" : "G2b"), "9", "0", 1e3, !_RESISTORS) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _RESISTORS ? (_SYMETRIC ? "R3" : "R3a") : (_SYMETRIC ? "G3" : "G3a"), "12", "0", 1e3, !_RESISTORS) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, _RESISTORS ? (_SYMETRIC ? "R3" : "R3b") : (_SYMETRIC ? "G3" : "G3b"), "11", "0", 1e3, !_RESISTORS) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, _SYMETRIC ? "C1" : "C1a", "3", "0", 1e-9) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, _SYMETRIC ? "C1" : "C1b", "4", "0", 1e-9) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, _SYMETRIC ? "C2" : "C2a", "5", "0", 1e-9) < 0)
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, _SYMETRIC ? "C2" : "C2b", "6", "0", 1e-9) < 0)
			PERFORM_ERROR;
		if (_SIMP){
			if (Manager.AddResistor(hCircuit, "R", "1", "0", 1e9, false) < 0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "R", "2", "0", 1e9, false) < 0)
				PERFORM_ERROR;
		}

		if (_SIMP){
			long long hCofactorD = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } } });;
			if (hCofactorD < 0)
				PERFORM_ERROR;
			long long hCofactorBP = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "3", "4" } } });
			if (hCofactorBP < 0)
				PERFORM_ERROR;
			long long hCofactorLP = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "5", "6" } } });
			if (hCofactorBP < 0)
				PERFORM_ERROR;

			long long BP = Manager.DefineTransferFunctionSimp("Ku_BP", hCofactorBP, hCofactorD);
			if (BP<0)
				PERFORM_ERROR;
			long long LP = Manager.DefineTransferFunctionSimp("Ku_LP", hCofactorLP, hCofactorD);
			if (LP<0)
				PERFORM_ERROR;
		}
		else {
			long long hCofactorD = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "1", "0" } }, { { "2", "0" }, { "2", "0" } } });;
			if (hCofactorD < 0)
				PERFORM_ERROR;
			long long hCofactorBP = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "3", "4" } }, { { "2", "0" }, { "1", "-2" } } });
			if (hCofactorBP < 0)
				PERFORM_ERROR;
			long long hCofactorLP = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "5", "6" } }, { { "2", "0" }, { "1", "-2" } } });
			if (hCofactorBP < 0)
				PERFORM_ERROR;

			long long hCofactorBPC = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "3", "4" } } });
			if (hCofactorBPC < 0)
				PERFORM_ERROR;
			long long hCofactorLPC = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "5", "6" } } });
			if (hCofactorBPC < 0)
				PERFORM_ERROR;

			long long BP = Manager.DefineTransferFunctionSimp("Ku_BP", hCofactorBP, hCofactorD);
			if (BP<0)
				PERFORM_ERROR;
			long long LP = Manager.DefineTransferFunctionSimp("Ku_LP", hCofactorLP, hCofactorD);
			if (LP<0)
				PERFORM_ERROR;
			long long BPC = Manager.DefineTransferFunctionSimp("Ku_BPC", hCofactorBPC, hCofactorD);
			if (BP<0)
				PERFORM_ERROR;
			long long LPC = Manager.DefineTransferFunctionSimp("Ku_LPC", hCofactorLPC, hCofactorD);
			if (LP<0)
				PERFORM_ERROR;
		}
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
