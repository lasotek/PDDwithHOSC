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

constexpr auto NO_LF = false;

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		CPDDEngineManager Manager; //Enginer managers instance
		long hEngine = Manager.GetPDDEngine(0, true, true, true); //Creating Engine. 0 - the number of engine (can be more), true - s-expanded form, true - order optymalization
		long long hCircuit = Manager.GetNewCircuit(hEngine); //The main circuit initialization	
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);// The output results file
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");//Errors, performance time, info file
		Manager.DeclareGlobalRefNode(hCircuit, "0");// Declaration of the reference node. Not obligatory. However, it simplifies models of idealized  active devices


		if (!ONLY_C_DEPENDENT) {
			//if (Manager.SetCompPriority(hCircuit, 0) < 0)
			//	PERFORM_ERROR;
			if (S_EXPANDED)
			{
				if (NO_LF)
				{
					if (Manager.AddNullor(hCircuit, "1", "2", "1", "2") < 0)
						PERFORM_ERROR;
					if (Manager.AddNullor(hCircuit, "4", "7", "4", "7") < 0)
						PERFORM_ERROR;
					if (Manager.AddNullor(hCircuit, "6", "0", "6", "0") < 0)
						PERFORM_ERROR;
				}
				else
				{
					if (Manager.AddCapacitor(hCircuit, "C_S_1", "1", "2", 1e-6) < 0)
						PERFORM_ERROR;
					if (Manager.AddCapacitor(hCircuit, "C_S_2", "4", "7", 1e-6) < 0)
						PERFORM_ERROR;
					if (Manager.AddCapacitor(hCircuit, "C_E", "6", "0", 750e-6) < 0)
						PERFORM_ERROR;
				}
				if (HFAlso)
				{
					if (Manager.AddCapacitor(hCircuit, "C_pi", "3", "5", 1e-12) < 0)
						PERFORM_ERROR;
					if (Manager.AddCapacitor(hCircuit, "C_mu", "3", "4", 1e-12) < 0)
						PERFORM_ERROR;
				}

			}
			else
			{
				if (NO_LF)
				{
					if (Manager.AddNullor(hCircuit, "1", "2", "1", "2") < 0)
						PERFORM_ERROR;
					if (Manager.AddNullor(hCircuit, "5", "7", "4", "7") < 0)
						PERFORM_ERROR;
					if (Manager.AddNullor(hCircuit, "6", "0", "6", "0") < 0)
						PERFORM_ERROR;
				}
				else
				{
					if (Manager.AddResistor(hCircuit, "sC_S_1", "1", "2", 1e-6, true) < 0)
						PERFORM_ERROR;
					if (Manager.AddResistor(hCircuit, "sC_S_2", "4", "7", 1e-6, true) < 0)
						PERFORM_ERROR;
					if (Manager.AddResistor(hCircuit, "sC_E", "6", "0", 750e-6, true) < 0)
						PERFORM_ERROR;
				}
				if (HFAlso)
				{
					if (Manager.AddResistor(hCircuit, "sC_pi", "3", "5", 1e-12, true) < 0)
						PERFORM_ERROR;
					if (Manager.AddResistor(hCircuit, "sC_mu", "3", "4", 1e-12, true) < 0)
						PERFORM_ERROR;
				}

			}
			//if (Manager.ClearCompPriority(hCircuit) < 0)
			//	PERFORM_ERROR;
		}
		if (Manager.AddTransconductance(hCircuit, "g_m", "4", "5", "3", "5", 0, 10e-3) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "g_bb" : "r_bb", "2", "3", 100e-6, RG) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, FULLR ? "r_pi" : "g_pi", "3", "5", 100e-6, !FULLR) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, FULLR ? "r_o" : "g_o", "4", "5", 10e-6, !FULLR) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "G_C" : "R_C", "4", "0", 20e-6, RG) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "G_L" : "R_L", "7", "0", 20e-6, RG) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "G_B" : "R_B", "2", "0", 20e-6, RG) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "G_E1" : "R_E1", "5", "6", 20e-6, RG) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "G_E2" : "R_E2", "6", "0", 20e-6, RG) < 0)
			PERFORM_ERROR;
		{
			long long hCofactor_D = Manager.CreateNewCofactor(hCircuit);
			if (hCofactor_D < 0)
				PERFORM_ERROR;
			long long hCofactor_D11 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "1", "0" } } });
			if (hCofactor_D11 < 0)
				PERFORM_ERROR;

			long long hCofactor_D17 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "7", "0" } } });
			if (hCofactor_D17 < 0)
				PERFORM_ERROR;
			long long hCofactor_D77 = Manager.CreateNewCofactor(hCircuit, { { { "7", "0" }, { "7", "0" } } });
			if (hCofactor_D77 < 0)
				PERFORM_ERROR;
			long long hCofactor_D1177 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "1", "0" } }, { { "7", "0" }, { "7", "0" } } });
			if (hCofactor_D1177 < 0)
				PERFORM_ERROR;

			long long K_u = Manager.DefineTransferFunctionSimp("K_u", hCofactor_D17, hCofactor_D11);
			if (K_u < 0)
				PERFORM_ERROR;
			long long M = Manager.DefineTransferFunctionSimp("M", hCofactor_D17, hCofactor_D);
			if (M < 0)
				PERFORM_ERROR;
			long long ZI = Manager.DefineTransferFunctionSimp("Z_I", hCofactor_D11, hCofactor_D);
			if (ZI < 0)
				PERFORM_ERROR;
			long long ZOI = Manager.DefineTransferFunctionSimp("Z_O_I", hCofactor_D77, hCofactor_D);
			if (ZOI < 0)
				PERFORM_ERROR;
			long long ZOU = Manager.DefineTransferFunctionSimp("Z_O_U", hCofactor_D1177, hCofactor_D11);
			if (ZOU < 0)
				PERFORM_ERROR;
			long long N = Manager.DefineTransferFunctionSimp("N", hCofactor_D17, hCofactor_D77);
			if (N < 0)
				PERFORM_ERROR;
			long long KI = Manager.DefineTransferFunctionSimp("N", hCofactor_D17, hCofactor_D1177);
			if (KI < 0)
				PERFORM_ERROR;
		}


		if (Manager.DoTheCalculation(hCircuit) < 0) //The nain circuit compution
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt") < 0) // WWrite down PDD structure to the file
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
	return 0;
}
