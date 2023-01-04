// SimplyMetaModel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define PERFORM_ERROR 		{\
			string str;\
									while((str=(string)LastErrorInfo)!="")\
				printf("%s\n",str.c_str());\
			goto end_of;\
								}

constexpr auto ACC = 0.001;
constexpr auto LOW_FREQ = 0.0;
constexpr auto HIGH_FREQ = 1e6;

constexpr auto ONLY_C_DEPENDENT = false;
constexpr auto FULLR = true;
constexpr auto RG = !FULLR && false;

constexpr auto S_EXPANDED = false;
constexpr auto HFAlso = true;

constexpr auto NO_LF = true;
constexpr auto CONDUCTANCE = false;

int _tmain(int argc, _TCHAR* argv[])
{
#if _DEBUG
	_CrtMemState cms1, cms2, cms3;
	_CrtMemCheckpoint(&cms1);
#endif
	try
	{
		CPDDEngineManager Manager; //Enginer managers instance
		long hEngine = Manager.GetPDDEngine(0, true, true, true); //Creating Engine. 0 - the number of engine (can be more), true - s-expanded form, true - order optymalization
		long long hCircuit = Manager.GetNewCircuit(hEngine); //The main circuit initialization	
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);// The output results file
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");//Errors, performance time, info file
		Manager.DeclareGlobalRefNode(hCircuit, "0");// Declaration of the reference node. Not obligatory. However, it simplifies models of idealized  active devices

		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G1" : "R1", "0", "1", CONDUCTANCE ? 1.0 / 12 : 12, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G2" : "R2", "2", "1", CONDUCTANCE ? 1.0 / 6 : 6, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G3" : "R3", "2", "3", CONDUCTANCE ? 1.0 / 9 : 9, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G4" : "R4", "0", "3", CONDUCTANCE ? 1.0 / 10 : 10, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G5" : "R5", "3", "4", CONDUCTANCE ? 1.0 / 8 : 8, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G6" : "R6", "2", "5", CONDUCTANCE ? 1.0 / 7 : 7, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G7" : "R7", "1", "6", CONDUCTANCE ? 1.0 / 5 : 5, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G8" : "R8", "0", "7", CONDUCTANCE ? 1.0 / 11 : 5, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G9" : "R9", "4", "7", CONDUCTANCE ? 1.0 / 3 : 3, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G10" : "R10", "4", "5", CONDUCTANCE ? 1.0 / 2 : 2, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G11" : "R11", "5", "6", CONDUCTANCE ? 1.0 / 1 : 1, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G12" : "R12", "6", "7", CONDUCTANCE ? 1.0 / 4 : 4, CONDUCTANCE) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G13" : "R13", "0", "5", CONDUCTANCE ? 1.0 / 13 : 13, CONDUCTANCE) < 0)
			PERFORM_ERROR;

		long long hCofactorD_D = Manager.CreateNewCofactor(hCircuit);
		if (hCofactorD_D < 0)
			PERFORM_ERROR;

		long long hCofactorN_1 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "1", "0" } } });
		if (hCofactorN_1 < 0)
			PERFORM_ERROR;

		long long hCofactorN_2 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "2", "1" } } });
		if (hCofactorN_2 < 0)
			PERFORM_ERROR;

		long long hCofactorN_3 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "2", "3" } } });
		if (hCofactorN_3 < 0)
			PERFORM_ERROR;

		long long hCofactorN_4 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "3", "0" } } });
		if (hCofactorN_4 < 0)
			PERFORM_ERROR;

		long long hCofactorN_5 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "4", "3" } } });
		if (hCofactorN_5 < 0)
			PERFORM_ERROR;

		long long hCofactorN_6 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "5", "2" } } });
		if (hCofactorN_6 < 0)
			PERFORM_ERROR;

		long long hCofactorN_7 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "6", "1" } } });
		if (hCofactorN_7 < 0)
			PERFORM_ERROR;

		long long hCofactorN_8 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "7", "0" } } });
		if (hCofactorN_8 < 0)
			PERFORM_ERROR;

		long long hCofactorN_9 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "4", "7" } } });
		if (hCofactorN_9 < 0)
			PERFORM_ERROR;

		long long hCofactorN_10 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "5", "4" } } });
		if (hCofactorN_10 < 0)
			PERFORM_ERROR;

		long long hCofactorN_11 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "5", "6" } } });
		if (hCofactorN_11 < 0)
			PERFORM_ERROR;

		long long hCofactorN_12 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "6", "7" } } });
		if (hCofactorN_12 < 0)
			PERFORM_ERROR;

		long long hCofactorN_13 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "5", "0" } } });
		if (hCofactorN_13 < 0)
			PERFORM_ERROR;


		long long M1 = Manager.DefineTransferFunctionSimp("M1", hCofactorN_1, hCofactorD_D);
		if (M1 < 0)
			PERFORM_ERROR;

		long long M2 = Manager.DefineTransferFunctionSimp("M2", hCofactorN_2, hCofactorD_D);
		if (M2 < 0)
			PERFORM_ERROR;

		long long M3 = Manager.DefineTransferFunctionSimp("M3", hCofactorN_3, hCofactorD_D);
		if (M3 < 0)
			PERFORM_ERROR;

		long long M4 = Manager.DefineTransferFunctionSimp("M4", hCofactorN_4, hCofactorD_D);
		if (M4 < 0)
			PERFORM_ERROR;

		long long M5 = Manager.DefineTransferFunctionSimp("M5", hCofactorN_5, hCofactorD_D);
		if (M5 < 0)
			PERFORM_ERROR;

		long long M6 = Manager.DefineTransferFunctionSimp("M6", hCofactorN_6, hCofactorD_D);
		if (M6 < 0)
			PERFORM_ERROR;

		long long M7 = Manager.DefineTransferFunctionSimp("M7", hCofactorN_7, hCofactorD_D);
		if (M7 < 0)
			PERFORM_ERROR;

		long long M8 = Manager.DefineTransferFunctionSimp("M8", hCofactorN_8, hCofactorD_D);
		if (M8 < 0)
			PERFORM_ERROR;

		long long M9 = Manager.DefineTransferFunctionSimp("M9", hCofactorN_9, hCofactorD_D);
		if (M9 < 0)
			PERFORM_ERROR;

		long long M10 = Manager.DefineTransferFunctionSimp("M10", hCofactorN_10, hCofactorD_D);
		if (M10 < 0)
			PERFORM_ERROR;

		long long M11 = Manager.DefineTransferFunctionSimp("M11", hCofactorN_11, hCofactorD_D);
		if (M11 < 0)
			PERFORM_ERROR;

		long long M12 = Manager.DefineTransferFunctionSimp("M12", hCofactorN_12, hCofactorD_D);
		if (M12 < 0)
			PERFORM_ERROR;

		long long M13 = Manager.DefineTransferFunctionSimp("M13", hCofactorN_13, hCofactorD_D);
		if (M7 < 0)
			PERFORM_ERROR;


		if (Manager.DoTheCalculation(hCircuit) < 0) //The nain circuit compution
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt") < 0) // WWrite down PDD structure to the file
			PERFORM_ERROR;
		if (Manager.WriteDirectNmericalPolynomial(hCofactorD_D, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_1, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_2, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_3, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_4, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_5, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_6, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_7, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_8, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_9, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_10, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_11, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_12, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorN_13, EmptyString, "Models.txt", true, false, true, 0) < 0)
			PERFORM_ERROR;


		long PT = 0;
		if ((PT = Manager.GetFreePathTraitor(hCircuit)) < 0) // Initialization of internal structure for simplifiaction and SoE form generation. Obligatory even in case of exact analysis
			PERFORM_ERROR;
		if (Manager.SetApproximationCriterion(hCircuit, PT, true, ACC, HIGH_FREQ, LOW_FREQ) < 0) // SoE form gheneration for exact forulae (true)
			PERFORM_ERROR;
		if (Manager.WriteFlatSimplifiedPDD(hCircuit, PT, true, true, "Models.txt") < 0) // Writting result to the file
			PERFORM_ERROR;
		if (Manager.ReleasePathTraitor(hCircuit, PT) < 0) //Internal structure realiseing
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
