// SimpleBOCCISoliman.cpp : Defines the entry point for the console application.
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
const ModelType ModelTypeInstance = trucking_asym;

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
		if (ModelTypeInstance == build_in)
		{
			//_CStrNodes Connections1({ "x = 6", "y = 0", "zp = 10", "zn = 1" });
			//_CStrNodes Connections2({ "x = 5", "y = 4", "zp = 9", "zn = 6" });
			//_CStrNodes Connections3({ "x = 4", "y = 1", "zp = 2", "zn = 8" });
			//_CStrNodes Connections4({ "x = 3", "y = 2", "zp = 7", "zn = 6" });
			if (Manager.AddBOCII(hCircuit, "6", "0", "10", "1", CONV_MOCCII) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "5", "1", "9", "6", CONV_MOCCII) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "4", "1", "2", "8", CONV_MOCCII) < 0)
				PERFORM_ERROR;
			if (Manager.AddBOCII(hCircuit, "3", "2", "7", "6", CONV_MOCCII) < 0)
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
			_CStrNodes Connections2({ "x = 5", "y = 4", "zp = 9", "zn = 6" });
			_CStrNodes Connections3({ "x = 4", "y = 1", "zp = 2", "zn = 8" });
			_CStrNodes Connections4({ "x = 3", "y = 2", "zp = 7", "zn = 6" });

			long ChosenModel = ModelTypeInstance == ideal ? hBOCCII : (ModelTypeInstance == trucking_sym ? hBOCCII_TE : hBOCCII_TE2); // The model selection

			if (Manager.IncludeSubcircuit(hCircuit, "M1", ChosenModel, Connections1) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M2", ChosenModel, Connections2) < 0)
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M3", ChosenModel, Connections3) < 0) //Including of 2 instances of the same mode;
				PERFORM_ERROR;
			if (Manager.IncludeSubcircuit(hCircuit, "M4", ChosenModel, Connections4) < 0)
				PERFORM_ERROR;
		}
#ifdef _RESITORS
		if (Manager.AddResistor(hCircuit, "R1", "5", "0", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2", "4", "0", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R3", "3", "0", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
#else
		if (Manager.AddResistor(hCircuit, "G1", "5", "0", 1e-3, true) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "G2", "4", "0", 1e-3, true) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "G3", "3", "0", 1e-3, true) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
#endif
		if (Manager.AddCapacitor(hCircuit, "C1", "1", "0", 1e-9) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C2", "2", "0", 1e-9) < 0) //VCVS definition. 0 is power of s operator to model some dynamic. Here - no dynamic
			PERFORM_ERROR;


		long long hCofactorD = Manager.CreateNewCofactor(hCircuit, { { { "10", "0" }, { "10", "0" } },
																	 { { "9", "0" }, { "9", "0" } },
																	 { { "8", "0" }, { "8", "0" } },
																	 { { "7", "0" }, { "7", "0" } }});
		if (hCofactorD < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)

		long long hCofactorN1 = Manager.CreateNewCofactor(hCircuit, { { { "6", "0" }, { "10", "0" } },
																	  { { "9", "0" }, { "9", "0" } },
																	  { { "8", "0" }, { "8", "0" } },
																	  { { "7", "0" }, { "7", "0" } } });
		if (hCofactorN1 < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)

		long long hCofactorN2 = Manager.CreateNewCofactor(hCircuit, { { { "10", "0" }, { "10", "0" } },
																	  { { "6", "0" }, { "9", "0" } },
																	  { { "8", "0" }, { "8", "0" } },
																	  { { "7", "0" }, { "7", "0" } } });
		if (hCofactorN2 < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)

		long long hCofactorN3 = Manager.CreateNewCofactor(hCircuit, { { { "10", "0" }, { "10", "0" } },
																	  { { "9", "0" }, { "9", "0" } },
																	  { { "6", "0" }, { "8", "0" } },
																	  { { "7", "0" }, { "7", "0" } } });
		if (hCofactorN3 < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)

		long long hCofactorN4 = Manager.CreateNewCofactor(hCircuit, { { { "10", "0" }, { "10", "0" } },
																	  { { "9", "0" }, { "9", "0" } },
																	  { { "8", "0" }, { "8", "0" } },
																	  { { "6", "0" }, { "7", "0" } } });
		if (hCofactorN4 < 0)
			PERFORM_ERROR;



		long long K_i1 = Manager.DefineTransferFunctionSimp("K_i1", hCofactorN1, hCofactorD);
		if (K_i1<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 1. See (4)
		long long K_i2 = Manager.DefineTransferFunctionSimp("K_i2", hCofactorN2, hCofactorD);
		if (K_i2<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 1. See (4)
		long long K_i3 = Manager.DefineTransferFunctionSimp("K_i3", hCofactorN3, hCofactorD);
		if (K_i3<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 1. See (4)
		long long K_i4 = Manager.DefineTransferFunctionSimp("K_i4", hCofactorN4, hCofactorD);
		if (K_i4<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 1. See (4)



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
