// SimpModelControlledSourcesImprovedkR.cpp : Defines the entry point for the console application.
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
const ModelType ModelTypeInstance = trucking_sym;

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
		//Manager.DeclareGlobalRefNode(hCircuit, "0");// Declaration of the reference node. Not obligatory. 
		// However, it force 0 node to each defined subcircuit. Here omitted to check the fully floating VCCS

		_CStrNodes FBCFOANodes({ "0", "xp", "xn", "yp", "yn", "wp", "wn", "zp", "zn" });
		long hFBCFOA = Manager.CreateNewSubcircuit(hCircuit, "FBCFOA", FBCFOANodes, nullptr, true);
		if (hFBCFOA < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "xp'", "xn'", "yp", "yn", hFBCFOA) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "0", "xn'", "xp'", "0", hFBCFOA) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentFollower(hCircuit, "zp", "0", "xp", "xp'", hFBCFOA) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentFollower(hCircuit, "zn", "0", "xn", "xn'", hFBCFOA) < 0)
			PERFORM_ERROR;
#ifdef _CFMB_OUT
		if (Manager.AddVoltageFollower(hCircuit, "wp", "wn", "zp", "zn", hFBCFOA) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "0", "wn", "wp", "0", hFBCFOA) < 0)
			PERFORM_ERROR;
#else
		if (Manager.AddVoltageFollower(hCircuit, "wp", "0", "zp", "0", hFBCFOA) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "wn", "0", "zn", "0", hFBCFOA) < 0)
			PERFORM_ERROR;
#endif
		if (Manager.AddResistor(hCircuit, "Rm", "zp", "0", 10000, hFBCFOA, false) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "Rm", "zn", "0", 10000, hFBCFOA, false) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hFBCFOA) < 0)
			PERFORM_ERROR;
		//Follower model of FDCFOA

		long hFBCFOA_TE = Manager.CreateNewSubcircuit(hCircuit, "FBCFOA_TE", FBCFOANodes, nullptr, true);
		if (hFBCFOA_TE < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_1", "xp'", "xn'", "yp", "yn", 0, 1, hFBCFOA_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\chi_1", "0", "xn'", "xp'", "0", 0, 1, hFBCFOA_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta_1", "zp", "0", "xp", "xp'", 0, 1, hFBCFOA_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta_2", "zn", "0", "xn", "xn'", 0, 1, hFBCFOA_TE) < 0)
			PERFORM_ERROR;
#ifdef _CFMB_OUT
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_2", "wp", "wn", "zp", "zn", 0, 1, hFBCFOA_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\chi_2", "0", "wn", "wp", "0", 0, 1, hFBCFOA_TE) < 0)
			PERFORM_ERROR;
#else
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_21", "wp", "0", "zp", "0", 0, 1, hFBCFOA_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_22", "wn", "0", "zn", "0", 0, 1, hFBCFOA_TE) < 0)
			PERFORM_ERROR;
#endif
		//if (Manager.AddResistor(hCircuit, "Rm", "zp", "0", 10000, hFBCFOA_TE, false) < 0)
		//	PERFORM_ERROR;
		//if (Manager.AddResistor(hCircuit, "Rm", "zn", "0", 10000, hFBCFOA_TE, false) < 0)
		//	PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hFBCFOA_TE) < 0)
			PERFORM_ERROR;
		//Tracking error model of FDCFOA

		long hModelToUse = ModelTypeInstance == ideal ? hFBCFOA : hFBCFOA_TE;

		_CStrNodes BoundaryNodes({ "0", "p", "r", "k", "l" });
		long hFloatVCVS = Manager.CreateNewSubcircuit(hCircuit, "FloatVCVS", BoundaryNodes, nullptr, true);
		if (hFloatVCVS < 0)
			PERFORM_ERROR;
		_CStrNodes FDCFOAConnections({ "0 = 0", "xp = xp", "xn = xn", "yp = k", "yn = l",
			"wp = p", "wn = r", "zp = zp", "zn = zn" });
		if (Manager.IncludeSubcircuit(hCircuit, "FD", hModelToUse, hFloatVCVS, FDCFOAConnections) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R", "xp", "xn", 1e3, hFloatVCVS, false) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "k1*R", "zp", "0", 1e3, hFloatVCVS, false) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "k2*R", "0", "zn", 1e3, hFloatVCVS, false) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hFloatVCVS) < 0)
			PERFORM_ERROR;
		// Floating VCVS with CFMB based on model above

		_CStrNodes BoundaryTest({ "p", "r", "k", "l" });
		long hFloatVCVSTest = Manager.CreateNewSubcircuit(hCircuit, "TestFloatVCVS", BoundaryTest, nullptr, true);
		if (hFloatVCVSTest < 0)
			PERFORM_ERROR;
		_CStrNodes TestConnections({ "p = p", "r = r", "k = k", "l = l" });
		if (Manager.IncludeSubcircuit(hCircuit, "T", hFloatVCVS, hFloatVCVSTest, TestConnections) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hFloatVCVSTest) < 0)
			PERFORM_ERROR;
		// Only to check, if it get the floating VCVS meta-vertex if gnd (0) node is not connected. 
		// It almost work, but the gain is 2k instead of k. 

		_CStrNodes MainConnections({ "0 = 0", "p = 7", "r = 8", "k = 5", "l = 6" });
		if (Manager.SetCompPriority(hCircuit, 0) < 0)
			PERFORM_ERROR;
		if (Manager.IncludeSubcircuit(hCircuit, "K", hFloatVCVS, MainConnections) < 0)
			PERFORM_ERROR;
		if (Manager.ClearCompPriority(hCircuit) < 0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R1", "1", "3", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R1", "2", "4", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2", "3", "5", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2", "4", "6", 1e3, false) < 0) //Resistor definition. false - not as a conductance. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C1", "3", "7", 1e-9) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C1", "4", "8", 1e-9) < 0) //Capacitor definition. Initial value in case of some numerical computation
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C2", "5", "g", 1e-9) < 0) //VCVS definition. 0 is power of s operator to model some dynamic. Here - no dynamic
			PERFORM_ERROR;
		if (Manager.AddCapacitor(hCircuit, "C2", "6", "g", 1e-9) < 0) //VCVS definition. 0 is power of s operator to model some dynamic. Here - no dynamic
			PERFORM_ERROR;
		// Test if the model works. It works, excepte wrong value of k. However, in original Sanchez-Lopez example, 
		// node g was connected to the FDCFOA reference node, which was wrong.

		long long hCofactorD_D = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } } });
		if (hCofactorD_D < 0)
			PERFORM_ERROR;
		//The common HOSC in denominator. See (1)


		long long hCofactorN_D = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "7", "8" } } });
		if (hCofactorN_D < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (2)

		long long hCofactorD_C = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "2", "0" } } });
		if (hCofactorD_C < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (3)

		long long hCofactorN_C = Manager.CreateNewCofactor(hCircuit, { { { "1", "2" }, { "1", "2" } }, { { "2", "0" }, { "7", "8" } } });
		if (hCofactorN_C < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (4)

		long long hCofactorN_DS = Manager.CreateNewCofactor(hCircuit, { { { "1", "0" }, { "7", "8" } }, { { "2", "0" }, { "2", "-1" } } });
		if (hCofactorN_DS < 0)
			PERFORM_ERROR;
		//Current-to-current numerator HOSC. See (5)

		long long K_ud1 = Manager.DefineTransferFunctionSimp("K_ud1", hCofactorN_D, hCofactorD_D);
		if (K_ud1<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 1. See (6)

		long long K_ud2 = Manager.DefineTransferFunctionSimp("K_ud2", hCofactorN_DS, hCofactorD_C);
		if (K_ud2<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 2. See (7)

		long long K_uc = Manager.DefineTransferFunctionSimp("K_uc", hCofactorN_C, hCofactorD_C);
		if (K_uc<0)
			PERFORM_ERROR;
		//Current-to-current transfer function 2. See (8)


		if (Manager.DoTheCalculation(hCircuit)<0) //The nain circuit compution
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0) // WWrite down PDD structure to the file
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactorD_D, EmptyString, "Models.txt", true, false, true, RANGE)<0)
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
