// SimplyDDCCModel.cpp : Defines the entry point for the console application.
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

		_CStrNodes BounadryNodes({ "x", "y1", "y2", "y3","z" });
		long hDDCC = Manager.CreateNewSubcircuit(hCircuit, "DDCC", BounadryNodes, nullptr, true);//The idealized model made of VF & CF
		if (hDDCC < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "1", "2", "y1", "0", hDDCC) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "3", "2", "y2", "0", hDDCC) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageFollower(hCircuit, "3", "0", "y3", "0", hDDCC) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentFollower(hCircuit, "z", "0", "x", "1", hDDCC) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hDDCC) < 0)
			PERFORM_ERROR;

		long hDDCC_TE = Manager.CreateNewSubcircuit(hCircuit, "DDCC_TE", BounadryNodes, nullptr, true); //The model with the tracking error
		if (hDDCC_TE < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_1", "1", "2", "y1", "0", 0, 1, hDDCC_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_2", "3", "2", "y2", "0", 0, 1, hDDCC_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha_3", "3", "0", "y3", "0", 0, 1, hDDCC_TE) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta", "z", "0", "x", "1", 0, 1, hDDCC_TE) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hDDCC_TE) < 0)
			PERFORM_ERROR;

		long hDDCC_TES = Manager.CreateNewSubcircuit(hCircuit, "DDCC_TES", BounadryNodes, nullptr, true); //The model with the tracking error
		if (hDDCC_TES < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha", "1", "2", "y1", "0", 0, 1, hDDCC_TES) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha", "3", "2", "y2", "0", 0, 1, hDDCC_TES) < 0)
			PERFORM_ERROR;
		if (Manager.AddVoltageGain(hCircuit, "\\alpha", "3", "0", "y3", "0", 0, 1, hDDCC_TES) < 0)
			PERFORM_ERROR;
		if (Manager.AddCurrentGain(hCircuit, "\\beta", "z", "0", "x", "1", 0, 1, hDDCC_TES) < 0)
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hDDCC_TES) < 0)
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0) // Write down PDD structure to the file
			PERFORM_ERROR;

		// The aim was to obtain meta-vertices for the device. Thus, there are no computation of any trannsfer functions
	}
	catch (exception& e)
	{
		cout << "Uncatced exception: " << e.what();
	}
end_of:
	return 0;
}
