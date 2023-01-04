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
constexpr auto HFAlso = false;

constexpr auto NO_LF = true;

constexpr auto Priority = true;

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


		if (Priority && Manager.SetCompPriority(hCircuit, 3) < 0)
			PERFORM_ERROR;
		if (Manager.AddTransconductance(hCircuit, "g_m", "4", "5", "3", "5", 0, 10e-3) < 0)
			PERFORM_ERROR;
		if (Priority && Manager.SetCompPriority(hCircuit, 1) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "g_bb" : "r_bb", "2", "3", 100e-6, RG) < 0)
			PERFORM_ERROR;
		if (Priority && Manager.SetCompPriority(hCircuit, 4) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, FULLR ? "r_pi" : "g_pi", "3", "5", 100e-6, !FULLR) < 0)
			PERFORM_ERROR;
		if (Priority && Manager.SetCompPriority(hCircuit, 5) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, FULLR ? "r_o" : "g_o", "4", "5", 10e-6, !FULLR) < 0)
			PERFORM_ERROR;
		if (Priority && Manager.SetCompPriority(hCircuit, 2) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "G_C" : "R_C", "4", "0", 20e-6, RG) < 0)
			PERFORM_ERROR;
		//if (Priority && Manager.SetCompPriority(hCircuit, 2) < 0)
		//	PERFORM_ERROR;
		//if (Manager.AddResistor(hCircuit, RG ? "G_L" : "R_L", "4", "0", 20e-6, RG) < 0)
		//	PERFORM_ERROR;
		if (Priority && Manager.SetCompPriority(hCircuit, 0) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "G_B" : "R_B", "2", "0", 20e-6, RG) < 0)
			PERFORM_ERROR;
		if (Priority && Manager.SetCompPriority(hCircuit, 6) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, RG ? "G_E" : "R_E1", "5", "0", 20e-6, RG) < 0)
			PERFORM_ERROR;
		{
			long long hCofactor_D = Manager.CreateNewCofactor(hCircuit);
			if (hCofactor_D < 0)
				PERFORM_ERROR;
			long long hCofactor_D11 = Manager.CreateNewCofactor(hCircuit, { { { "2", "0" }, { "2", "0" } } });
			if (hCofactor_D11 < 0)
				PERFORM_ERROR;

			long long hCofactor_D17 = Manager.CreateNewCofactor(hCircuit, { { { "2", "0" }, { "4", "0" } } });
			if (hCofactor_D17 < 0)
				PERFORM_ERROR;

			long long hCofactor_D77 = Manager.CreateNewCofactor(hCircuit, { { { "4", "0" }, { "4", "0" } } });
			if (hCofactor_D77 < 0)
				PERFORM_ERROR;
			long long hCofactor_D1177 = Manager.CreateNewCofactor(hCircuit, { { { "2", "0" }, { "2", "0" } }, { { "4", "0" }, { "4", "0" } } });
			if (hCofactor_D1177 < 0)
				PERFORM_ERROR;

			long long hCofactor_D15 = Manager.CreateNewCofactor(hCircuit, { { { "2", "0" }, { "5", "0" } } });
			if (hCofactor_D15 < 0)
				PERFORM_ERROR;

			long long hCofactor_D55 = Manager.CreateNewCofactor(hCircuit, { { { "5", "0" }, { "5", "0" } } });
			if (hCofactor_D15 < 0)
				PERFORM_ERROR;
			long long hCofactor_D1155 = Manager.CreateNewCofactor(hCircuit, { { { "2", "0" }, { "2", "0" } }, { { "5", "0" }, { "5", "0" } } });
			if (hCofactor_D1155 < 0)
				PERFORM_ERROR;

			long long K_u1 = Manager.DefineTransferFunctionSimp("K_u1", hCofactor_D17, hCofactor_D11);
			if (K_u1 < 0)
				PERFORM_ERROR;
			long long M1 = Manager.DefineTransferFunctionSimp("M1", hCofactor_D17, hCofactor_D);
			if (M1 < 0)
				PERFORM_ERROR;
			long long ZI = Manager.DefineTransferFunctionSimp("Z_I", hCofactor_D11, hCofactor_D);
			if (ZI < 0)
				PERFORM_ERROR;
			long long ZOI1 = Manager.DefineTransferFunctionSimp("Z_O_I1", hCofactor_D77, hCofactor_D);
			if (ZOI1 < 0)
				PERFORM_ERROR;
			long long ZOU1 = Manager.DefineTransferFunctionSimp("Z_O_U1", hCofactor_D1177, hCofactor_D11);
			if (ZOU1 < 0)
				PERFORM_ERROR;
			long long N1 = Manager.DefineTransferFunctionSimp("N_1", hCofactor_D17, hCofactor_D77);
			if (N1 < 0)
				PERFORM_ERROR;
			long long KI1 = Manager.DefineTransferFunctionSimp("K_i1", hCofactor_D17, hCofactor_D1177);
			if (KI1 < 0)
				PERFORM_ERROR;

			long long K_u2 = Manager.DefineTransferFunctionSimp("K_u2", hCofactor_D15, hCofactor_D11);
			if (K_u2 < 0)
				PERFORM_ERROR;
			long long M2 = Manager.DefineTransferFunctionSimp("M2", hCofactor_D15, hCofactor_D);
			if (M2 < 0)
				PERFORM_ERROR;
			long long ZOI2 = Manager.DefineTransferFunctionSimp("Z_O_I2", hCofactor_D55, hCofactor_D);
			if (ZOI2 < 0)
				PERFORM_ERROR;
			long long ZOU2 = Manager.DefineTransferFunctionSimp("Z_O_U2", hCofactor_D1155, hCofactor_D11);
			if (ZOU2 < 0)
				PERFORM_ERROR;
			long long N2 = Manager.DefineTransferFunctionSimp("N_2", hCofactor_D15, hCofactor_D55);
			if (N2 < 0)
				PERFORM_ERROR;
			long long KI2 = Manager.DefineTransferFunctionSimp("K_i2", hCofactor_D15, hCofactor_D1155);
			if (KI2 < 0)
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
