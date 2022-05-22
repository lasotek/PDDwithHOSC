// SimplyMetaModel.cpp : Defines the entry point for the console application.
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

#define _MULTI_STAGE_EXAMPLE //Switch to 2 stage example

constexpr bool TWO_STAGES = true;
constexpr bool NO_PAR = false;

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		CPDDEngineManager Manager; //Enginer managers instance
		long hEngine = Manager.GetPDDEngine(0, true, true, true); //Creating Engine. 0 - the number of engine (can be more), true - s-expanded form, true - order optymalization
		long long hCircuit = Manager.GetNewCircuit(hEngine); //The main circuit initialization	
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);// The output results file
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");//Errors, performance time, info file
		//Manager.DeclareGlobalRefNode(hCircuit, "0");// Declaration of the reference node. Not obligatory. However, it simplifies models of idealized  active devices

		_CStrNodes BoundaryNodes({ "B", "G", "D", "S" });//Boundary nodes definition
		long MosId = Manager.CreateNewSubcircuit(hCircuit, "MOS", BoundaryNodes, nullptr, true);
		if (MosId < 0)
			PERFORM_ERROR;
		//Subcircuit creation. Because of historical burden subcircuit has conection with the mian circuit. 
		//In the target version it should be changed. 

		//if (Manager.AddOpAmp(hCircuit, "O", "Y", "X", hModel) < 0) //The groundned output op-amp as a nullor, needs the declaration of reference node.
		//	PERFORM_ERROR;
		if (TWO_STAGES && !NO_PAR)
		{
			_CStrNodes BoundaryNodes({ "B", "G", "D", "S" });//Boundary nodes definition
			long MosIntId = Manager.CreateNewSubcircuit(hCircuit, "MOSInt", BoundaryNodes, nullptr, true);
			if (MosIntId < 0)
				PERFORM_ERROR;

			if (Manager.SetCompPriority(hCircuit, MosIntId, 0) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_gd", "G", "D", 15e-15, MosIntId) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_gs", "G", "S", 0.2e-9, MosIntId) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_gb", "B", "G", 30e-15, MosIntId) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_db", "B", "D", 20e-15, MosIntId) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_sb", "B", "S", 40e-15, MosIntId) < 0)
				PERFORM_ERROR;
			if (Manager.ClearCompPriority(hCircuit, MosIntId) < 0)
				PERFORM_ERROR;
			if (Manager.AddTransconductance(hCircuit, "g_m", "D", "S", "G", "S", 0, 1e-3, MosIntId) < 0)
				PERFORM_ERROR;
			if (Manager.AddTransconductance(hCircuit, "g_mb", "D", "S", "B", "S", 0, 0.2e-3, MosIntId) < 0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "g_ds", "D", "S", 10e-6, MosIntId, true) < 0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "g_bd", "D", "B", 20e-6, MosIntId, true) < 0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "g_bs", "B", "S", 20e-6, MosIntId, true) < 0)
				PERFORM_ERROR;
			if (Manager.ForceModelCalculation(hCircuit, MosIntId) < 0)
				PERFORM_ERROR;

			_CStrNodes Connections({ "G = G", "B = B", "S = S'", "D = D'" });
			if (Manager.IncludeSubcircuit(hCircuit, "I", MosIntId, MosId,Connections) < 0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "g_d", "D'", "D", 1.0 / 5.0, MosId, true) < 0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "g_s", "S'", "S", 1.0 / 5.0, MosId, true) < 0)
				PERFORM_ERROR;
		}
		else
		{

			if (Manager.SetCompPriority(hCircuit, MosId, 0) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_gd", "G", "D'", 15e-15, MosId) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_gs", "G", "S'", 0.2e-9, MosId) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_gb", "B", "G", 30e-15, MosId) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_db", "B", "D'", 20e-15, MosId) < 0)
				PERFORM_ERROR;
			if (Manager.AddCapacitor(hCircuit, "C_sb", "B", "S'", 40e-15, MosId) < 0)
				PERFORM_ERROR;
			if (Manager.ClearCompPriority(hCircuit, MosId) < 0)
				PERFORM_ERROR;
			if (Manager.AddTransconductance(hCircuit, "g_m", "D'", "S'", "G", "S'", 0, 1e-3, MosId) < 0)
				PERFORM_ERROR;
			if (Manager.AddTransconductance(hCircuit, "g_mb", "D'", "S'", "B", "S'", 0, 0.2e-3, MosId) < 0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "g_ds", "D'", "S'", 10e-6, MosId, true) < 0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "g_bd", "D'", "B", 20e-6, MosId, true) < 0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "g_bs", "B", "S'", 20e-6, MosId, true) < 0)
				PERFORM_ERROR;
			if (NO_PAR)
			{
				if (Manager.AddNullor(hCircuit, "D", "D'", "D", "D'", MosId))
					PERFORM_ERROR;
				if (Manager.AddNullor(hCircuit, "S", "S'", "S", "S'", MosId))
					PERFORM_ERROR;
			}
			else
			{
				if (Manager.AddResistor(hCircuit, "g_d", "D'", "D", 1.0 / 5.0, MosId, true) < 0)
					PERFORM_ERROR;
				if (Manager.AddResistor(hCircuit, "g_s", "S'", "S", 1.0 / 5.0, MosId, true) < 0)
					PERFORM_ERROR;
			}
		}
		if (Manager.ForceModelCalculation(hCircuit, MosId) < 0)
			PERFORM_ERROR;
		// Force meta-vertex determination immediatelly. The meta-vertex creation is lazy algorythm by default. It is not created if nodbody includes it so far.
		// The analysis create the binary model stored at HD. It could be reused many times without recalculation.
		// However, the current version has some bug, which is a result of the chenging format.
		// That is why in this example we cannot reuse the meta-vertex. The 1-stage example and multistage example
		// must be compiled separately with the swicth _MULTI_STAGE_EXAMPLE on and off.

		_CStrNodes Connections({ "G = G", "B = S", "S = S", "D = D" });
		if (Manager.IncludeSubcircuit(hCircuit, "M", MosId, Connections) < 0)
			PERFORM_ERROR;
		//The definition of meta-compoenet conection in a for <boundary node> = <master circuit node>
		// The meta component insertion to the main circuit
		if (Manager.AddResistor(hCircuit, "G_g", "G", "0", 1.0 / 10e6, true) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "G_d", "D", "0", 1.0 / 1e3, true) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "G_s", "S", "0", 1.0 / 100, true) < 0)
			PERFORM_ERROR;

		long long hCofactorD_D = Manager.CreateNewCofactor(hCircuit, { { { "G", "0" }, { "G", "0" } } });
		if (hCofactorD_D < 0)
			PERFORM_ERROR;
		//The HOSC in denominator for differential gain. See (4)

		long long hCofactorN_D1 = Manager.CreateNewCofactor(hCircuit, { { { "G", "0" }, { "D", "0" } } });
		if (hCofactorN_D1 < 0)
			PERFORM_ERROR;
		//The HOSC in numerator for differential gain. See (5)

		long long hCofactorN_D2 = Manager.CreateNewCofactor(hCircuit, { { { "G", "0" }, { "S", "0" } } });
		if (hCofactorN_D2 < 0)
			PERFORM_ERROR;
		//The HOSC in numerator for differential gain. See (5)


		long long K_u_1 = Manager.DefineTransferFunctionSimp("K_u_1", hCofactorN_D1, hCofactorD_D);
		if (K_u_1 < 0)
			PERFORM_ERROR;
		//Voltage to voltage differential gain. See (9)

		long long K_u_2 = Manager.DefineTransferFunctionSimp("K_u_d2", hCofactorN_D2, hCofactorD_D);
		if (K_u_2 < 0)
			PERFORM_ERROR;
		//Voltage to voltage differential gain. See (10)


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
