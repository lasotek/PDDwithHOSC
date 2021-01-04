// SimpleBOCCISoliman2.cpp : Defines the entry point for the console application.
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

//#define _RESITORS

using  ModelType = enum{ ideal, trucking_sym, trucking_asym, build_in };
const ModelType ModelTypeInstance = trucking_sym;

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
		_CStrNodes OutBoundaryNodes({"1", "2", "3", "4", "5"});
		long hDen = Manager.CreateNewSubcircuit(hCircuit, "Den", OutBoundaryNodes, nullptr, true);//The idealized model made of VF & CF
		if (hDen < 0)
			PERFORM_ERROR;
		long hN1 = Manager.CreateNewSubcircuit(hCircuit, "N1", OutBoundaryNodes, nullptr, true);//The idealized model made of VF & CF
		if (hN1 < 0)
			PERFORM_ERROR;
		long hN2 = Manager.CreateNewSubcircuit(hCircuit, "N2", OutBoundaryNodes, nullptr, true);//The idealized model made of VF & CF
		if (hN2 < 0)
			PERFORM_ERROR;
		long hN3 = Manager.CreateNewSubcircuit(hCircuit, "N3", OutBoundaryNodes, nullptr, true);//The idealized model made of VF & CF
		if (hN3 < 0)
			PERFORM_ERROR;
		long hN4 = Manager.CreateNewSubcircuit(hCircuit, "N4", OutBoundaryNodes, nullptr, true);//The idealized model made of VF & CF
		if (hN4 < 0)
			PERFORM_ERROR;

		if (ModelTypeInstance == build_in)
		{
			//_CStrNodes Connections1({ "x = 6", "y = 0", "zp = 10", "zn = 1" });
			//_CStrNodes Connections2({ "x = 5", "y = 4", "zp = 9", "zn = 6" });
			//_CStrNodes Connections3({ "x = 4", "y = 1", "zp = 2", "zn = 8" });
			//_CStrNodes Connections4({ "x = 3", "y = 2", "zp = 7", "zn = 6" });
			if (Manager.AddBOCII(hCircuit, "6", "0", "10", "1", CONV_MOCCII, hDen) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "5", "1", "9", "6", CONV_MOCCII, hDen) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "4", "1", "2", "8", CONV_MOCCII, hDen) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "3", "2", "7", "6", CONV_MOCCII, hDen) < 0)
				PERFORM_ERROR;

			if (Manager.AddBOCII(hCircuit, "6", "0", "10", "1", CONV_MOCCII, hN1) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "5", "1", "9", "6", CONV_MOCCII, hN1) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "4", "1", "2", "8", CONV_MOCCII, hN1) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "3", "2", "7", "6", CONV_MOCCII, hN1) < 0)
				PERFORM_ERROR;

			if (Manager.AddBOCII(hCircuit, "6", "0", "10", "1", CONV_MOCCII, hN2) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "5", "1", "9", "6", CONV_MOCCII, hN2) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "4", "1", "2", "8", CONV_MOCCII, hN2) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "3", "2", "7", "6", CONV_MOCCII, hN2) < 0)
				PERFORM_ERROR;

			if (Manager.AddBOCII(hCircuit, "6", "0", "10", "1", CONV_MOCCII, hN3) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "5", "1", "9", "6", CONV_MOCCII, hN3) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "4", "1", "2", "8", CONV_MOCCII, hN3) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "3", "2", "7", "6", CONV_MOCCII, hN3) < 0)
				PERFORM_ERROR;

			if (Manager.AddBOCII(hCircuit, "6", "0", "10", "1", CONV_MOCCII, hN4) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "5", "1", "9", "6", CONV_MOCCII, hN4) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "4", "1", "2", "8", CONV_MOCCII, hN4) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "3", "2", "7", "6", CONV_MOCCII, hN4) < 0)
				PERFORM_ERROR;
		}
		else
		{
			_CStrNodes BounadryNodes({ "x", "y", "zp", "zn" });
			long hBOCCII = Manager.CreateNewSubcircuit(hCircuit, "BOCCII", BounadryNodes, nullptr, true);//The idealized model made of VF & CF
			if (hBOCCII < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "1", "0", "y", "0", hBOCCII) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "zp", "0", "2", "1", hBOCCII) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "0", "zn", "x", "2", hBOCCII) < 0)
				PERFORM_ERROR;
			if (Manager.ForceModelCalculation(hCircuit, hBOCCII) < 0)
				PERFORM_ERROR;

			long hBOCCII_TE = Manager.CreateNewSubcircuit(hCircuit, "BOCCII_TE", BounadryNodes, nullptr, true); //The model with the tracking error, but the same for both outputs
			if (hBOCCII_TE < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "\\alpha", "1", "0", "y", "0", 0, 1, hBOCCII_TE) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "\\beta", "zp", "0", "2", "1", 0, 1, hBOCCII_TE) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "\\beta", "0", "zn", "x", "2", 0, 1, hBOCCII_TE) < 0)
				PERFORM_ERROR;
			if (Manager.ForceModelCalculation(hCircuit, hBOCCII_TE) < 0)
				PERFORM_ERROR;

			long hBOCCII_TE2 = Manager.CreateNewSubcircuit(hCircuit, "BOCCII_TE2", BounadryNodes, nullptr, true); //The model with trucking errors different for each output
			if (hBOCCII_TE2 < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "\\alpha", "1", "0", "y", "0", 0, 1, hBOCCII_TE2) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "\\beta_p", "zp", "0", "2", "1", 0, 1, hBOCCII_TE2) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "\\beta_n", "0", "zn", "x", "2", 0, 1, hBOCCII_TE2) < 0)
				PERFORM_ERROR;
			if (Manager.ForceModelCalculation(hCircuit, hBOCCII_TE2) < 0)
				PERFORM_ERROR;


			//_CStrNodes Connections1({ "x = 6", "y = 0", "zp = 10", "zn = 1" });
			//_CStrNodes Connections2({ "x = 5", "y = 4", "zp = 9", "zn = 6" });
			//_CStrNodes Connections3({ "x = 4", "y = 1", "zp = 2", "zn = 8" });
			//_CStrNodes Connections4({ "x = 3", "y = 2", "zp = 7", "zn = 6" });
			_CStrNodes Connections1({ "x = 6", "y = 0", "zp = 10", "zn = 1" });
			_CStrNodes Connections2({ "x = 5", "y = 1", "zp = 9", "zn = 6" });
			_CStrNodes Connections3({ "x = 4", "y = 1", "zp = 2", "zn = 8" });
			_CStrNodes Connections4({ "x = 3", "y = 2", "zp = 7", "zn = 6" });

			long ChosenModel = ModelTypeInstance == ideal ? hBOCCII : (ModelTypeInstance == trucking_sym ? hBOCCII_TE : hBOCCII_TE2); // The model selection

			if (Manager.IncludeSubcircuit(hCircuit, "M1", ChosenModel, hDen, Connections1) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M2", ChosenModel, hDen, Connections2) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M3", ChosenModel, hDen, Connections3) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M4", ChosenModel, hDen, Connections4) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;

			if (Manager.IncludeSubcircuit(hCircuit, "M1", ChosenModel, hN1, Connections1) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M2", ChosenModel, hN1, Connections2) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M3", ChosenModel, hN1, Connections3) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M4", ChosenModel, hN1, Connections4) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;

			if (Manager.IncludeSubcircuit(hCircuit, "M1", ChosenModel, hN2, Connections1) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M2", ChosenModel, hN2, Connections2) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M3", ChosenModel, hN2, Connections3) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M4", ChosenModel, hN2, Connections4) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;

			if (Manager.IncludeSubcircuit(hCircuit, "M1", ChosenModel, hN3, Connections1) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M2", ChosenModel, hN3, Connections2) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M3", ChosenModel, hN3, Connections3) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M4", ChosenModel, hN3, Connections4) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;

			if (Manager.IncludeSubcircuit(hCircuit, "M1", ChosenModel, hN4, Connections1) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M2", ChosenModel, hN4, Connections2) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M3", ChosenModel, hN4, Connections3) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M4", ChosenModel, hN4, Connections4) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
		}




		//long long hCofactorD = Manager.CreateNewCofactor(hCircuit, { { { "10", "0" }, { "10", "0" } },
		//{ { "9", "0" }, { "9", "0" } },
		//{ { "8", "0" }, { "8", "0" } },
		//{ { "7", "0" }, { "7", "0" } } });
		//if (hCofactorD < 0)
		//	PERFORM_ERROR;
		//The common HOSC in denominator. See (1)
		if (Manager.AddPathologicalPair(hCircuit, "10", "0", "10", "0", false, false, hDen) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "9", "0", "9", "0", false, false, hDen) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "8", "0", "8", "0", false, false, hDen) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "7", "0", "7", "0", false, false, hDen) < 0)
			PERFORM_ERROR;

		//long long hCofactorN1 = Manager.CreateNewCofactor(hCircuit, { { { "6", "0" }, { "10", "0" } },
		//{ { "9", "0" }, { "9", "0" } },
		//{ { "8", "0" }, { "8", "0" } },
		//{ { "7", "0" }, { "7", "0" } } });
		//if (hCofactorN1 < 0)
		//	PERFORM_ERROR;
		//The common HOSC in denominator. See (1)
		if (Manager.AddPathologicalPair(hCircuit, "6", "0", "10", "0", false, false, hN1) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "9", "0", "9", "0", false, false, hN1) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "8", "0", "8", "0", false, false, hN1) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "7", "0", "7", "0", false, false, hN1) < 0)
			PERFORM_ERROR;

		//long long hCofactorN2 = Manager.CreateNewCofactor(hCircuit, { { { "10", "0" }, { "10", "0" } },
		//{ { "6", "0" }, { "9", "0" } },
		//{ { "8", "0" }, { "8", "0" } },
		//{ { "7", "0" }, { "7", "0" } } });
		//if (hCofactorN2 < 0)
		//	PERFORM_ERROR;
		//The common HOSC in denominator. See (1)
		if (Manager.AddPathologicalPair(hCircuit, "10", "0", "10", "0", false, false, hN2) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "6", "0", "9", "0", false, false, hN2) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "8", "0", "8", "0", false, false, hN2) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "7", "0", "7", "0", false, false, hN2) < 0)
			PERFORM_ERROR;

		//long long hCofactorN3 = Manager.CreateNewCofactor(hCircuit, { { { "10", "0" }, { "10", "0" } },
		//{ { "9", "0" }, { "9", "0" } },
		//{ { "6", "0" }, { "8", "0" } },
		//{ { "7", "0" }, { "7", "0" } } });
		//if (hCofactorN3 < 0)
		//	PERFORM_ERROR;
		//The common HOSC in denominator. See (1)
		if (Manager.AddPathologicalPair(hCircuit, "10", "0", "10", "0", false, false, hN3) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "9", "0", "9", "0", false, false, hN3) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "6", "0", "8", "0", false, false, hN3) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "7", "0", "7", "0", false, false, hN3) < 0)
			PERFORM_ERROR;

		//long long hCofactorN4 = Manager.CreateNewCofactor(hCircuit, { { { "10", "0" }, { "10", "0" } },
		//{ { "9", "0" }, { "9", "0" } },
		//{ { "8", "0" }, { "8", "0" } },
		//{ { "6", "0" }, { "7", "0" } } });
		//if (hCofactorN4 < 0)
		//	PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "10", "0", "10", "0", false, false, hN4) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "9", "0", "9", "0", false, false, hN4) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "8", "0", "8", "0", false, false, hN4) < 0)
			PERFORM_ERROR;
		if (Manager.AddPathologicalPair(hCircuit, "6", "0", "7", "0", false, false, hN4) < 0)
			PERFORM_ERROR;

		if (Manager.ForceModelCalculation(hCircuit, hDen) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hN1) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hN2) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hN3) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hN4) < 0)
			PERFORM_ERROR;


		//if (Manager.DoTheCalculation(hCircuit)<0) //The nain circuit compution
		//	PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0) // WWrite down PDD structure to the file
			PERFORM_ERROR;

		//if (Manager.WriteDirectNmericalPolynomial(hCofactorD, EmptyString, "Models.txt", true, false, true, RANGE)<0)
		//	PERFORM_ERROR;
		//The numerical denominator s-Expanded form calculation, true - append results to the file, false - do not sensitivity analysis, true - determine roots of polynomial

		//long PT = 0;
		//if ((PT = Manager.GetFreePathTraitor(hCircuit))<0) // Initialization of internal structure for simplifiaction and SoE form generation. Obligatory even in case of exact analysis
		//	PERFORM_ERROR;
		//if (Manager.SetApproximationCriterion(hCircuit, PT, true, ACC, HIGH_FREQ, LOW_FREQ)<0) // SoE form gheneration for exact forulae (true)
		//	PERFORM_ERROR;
		//if (Manager.WriteFlatSimplifiedPDD(hCircuit, PT, true, true, "Models.txt")<0) // Writting result to the file
		//	PERFORM_ERROR;
		//if (Manager.ReleasePathTraitor(hCircuit, PT)<0) //Internal structure realiseing
		//	PERFORM_ERROR;
	}
	catch (exception& e)
	{
		cout << "Uncatced exception: " << e.what();
	}
end_of:
	return 0;
}
