// SimplyDVCII.cpp : Defines the entry point for the console application.
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

		_CStrNodes BounadryNodes({ "x", "yp", "yn" , "zp", "zn" });
		long hDVCII = Manager.CreateNewSubcircuit(hCircuit, "DVCII", BounadryNodes, nullptr, true);//The idealized model made of VF & CF
		if (hDVCII < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "1", "2", "yp", "0", hDVCII) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "0", "2", "yn", "0", hDVCII) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentFollower(hCircuit, "zp", "zn", "x", "1", hDVCII) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hDVCII) < 0)
			PERFORM_ERROR;

		long hDVCII_TE = Manager.CreateNewSubcircuit(hCircuit, "DVCII_TE", BounadryNodes, nullptr, true);//The model with the tracking error
		if (hDVCII_TE < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha","1", "2", "yp", "0", 0, 1, hDVCII_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha", "0", "2", "yn", "0", 0, 1, hDVCII_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta", "zp", "0", "1'", "1", 0, 1, hDVCII_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta", "0", "zn", "x", "1'", 0, 1, hDVCII_TE) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hDVCII_TE) < 0)
			PERFORM_ERROR;

		long hDVCII_TEA = Manager.CreateNewSubcircuit(hCircuit, "DVCII_TEA", BounadryNodes, nullptr, true);//The model with the tracking error
		if (hDVCII_TEA < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_p", "1", "2", "yp", "0", 0, 1, hDVCII_TEA) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_n", "0", "2", "yn", "0", 0, 1, hDVCII_TEA) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta_p", "zp", "0", "1'", "1", 0, 1, hDVCII_TEA) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta_n", "0", "zn", "x", "1'", 0, 1, hDVCII_TEA) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hDVCII_TEA) < 0)
			PERFORM_ERROR;



		_CStrNodes Connections1({ "x = 4", "yp = 5", "yn = 1", "zp = 3", "zn = 0" });
		_CStrNodes Connections2({ "x = 6", "yp = 2", "yn = 3", "zp = 5", "zn = 0" });

		long ChosenModel = ModelTypeInstance == ideal ? hDVCII : (ModelTypeInstance == trucking_sym ? hDVCII_TE : hDVCII_TEA); // The model selection

		if (Manager.IncludeSubcircuit(hCircuit, "M1", ChosenModel, Connections1) < 0) //Including of 2 instances of the same mode;
			PERFORM_ERROR;
		if (Manager.IncludeSubcircuit(hCircuit, "M2", ChosenModel, Connections2) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R1", "3", "0", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2", "6", "0", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2", "4", "0", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C1", "3", "0", 1e-9) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C2", "5", "0", 1e-9) < 0) //VCVS definition. 0 is power of s operator to model some dynamic. Here - no dynamic
			PERFORM_ERROR;


		long long hCofactorD = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "1", "0" } }, { { "2", "0" }, { "2", "0" } } });
		if (hCofactorD < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)


		long long hCofactorN_U1 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "3", "0" } }, { { "2", "0" }, { "2", "0" } } });
		if (hCofactorN_U1 < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (2)

		long long hCofactorN_U2 = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "1", "0" } }, { { "2", "0" }, { "3", "0" } } });
		if (hCofactorN_U2 < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (3)


		long long K_u1 = Manager.DefineTransferFunctionSimp("K_u1", hCofactorN_U1, hCofactorD);
		if (K_u1<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 1. See (4)

		long long K_u2 = Manager.DefineTransferFunctionSimp("K_u2", hCofactorN_U2, hCofactorD);
		if (K_u2<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 2. See (5)


		if (Manager.DoTheCalculation(hCircuit)<0) //The nain circuit compution
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0) // WWrite down PDD structure to the file
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorD, EmptyString, "Models.txt", true, false, true, RANGE)<0)
			PERFORM_ERROR;
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
