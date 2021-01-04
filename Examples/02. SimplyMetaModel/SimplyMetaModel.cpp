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

//#define _MULTI_STAGE_EXAMPLE //Switch to 2 stage example

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

		_CStrNodes BoundaryNodes({"O", "I1", "I2", "I3"});//Boundary nodes definition
		long hModel = Manager.CreateNewSubcircuit(hCircuit, "op-ampCell", BoundaryNodes, nullptr, true);
		if (hModel < 0)
			PERFORM_ERROR;
		//Subcircuit creation. Because of historical burden subcircuit has conection with the mian circuit. 
		//In the target version it should be changed. 

		if (Manager.AddOpAmp(hCircuit, "O", "Y", "X", hModel) < 0) //The groundned output op-amp as a nullor, needs the declaration of reference node.
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R1", "I1", "X", 1e3, hModel, false) < 0) // as for the main circuit, but with additional model handler
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R2", "I2", "Y", 1e3, hModel, false) < 0) // as for the main circuit, but with additional model handler
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R3", "O", "Y", 1e3, hModel, false) < 0) // as for the main circuit, but with additional model handler
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "R4", "I3", "X", 1e3, hModel, false) < 0)// as for the main circuit, but with additional model handler
			PERFORM_ERROR;
		if (Manager.ForceModelCalculation(hCircuit, hModel) < 0) 
			PERFORM_ERROR;
			// Force meta-vertex determination immediatelly. The meta-vertex creation is lazy algorythm by default. It is not created if nodbody includes it so far.
			// The analysis create the binary model stored at HD. It could be reused many times without recalculation.
			// However, the current version has some bug, which is a result of the chenging format.
			// That is why in this example we cannot reuse the meta-vertex. The 1-stage example and multistage example
			// must be compiled separately with the swicth _MULTI_STAGE_EXAMPLE on and off.

#ifdef _MULTI_STAGE_EXAMPLE
		_CStrNodes BoundaryNodes3({ "O", "I"});
		//Boundary nodes of w new meta-vertex that is the combination of 2 another one
		long hModel2 = Manager.CreateNewSubcircuit(hCircuit, "2-op-ampCell", BoundaryNodes3, nullptr, true);
		if (hModel2 < 0)
			PERFORM_ERROR;

		_CStrNodes Connections1({ "O = O", "I1 = I", "I3 = I1" }); 
		//The definition of meta-compoenet conection in a for <boundary node> = <master circuit node>
		_CStrNodes Connections2({ "O = I1", "I1 = 0", "I2 = O", "I3 = 0" }); 
		//The definition of meta-compoenet conection in a for <boundary node> = <master circuit node>
		if (Manager.IncludeSubcircuit(hCircuit, "M1", hModel,hModel2, Connections1) < 0) // The meta component insertion to the main circuit
			PERFORM_ERROR;
		if (Manager.IncludeSubcircuit(hCircuit, "M2", hModel, hModel2, Connections2) < 0) // The meta component insertion to the main circuit
			PERFORM_ERROR;
		// IncludeSubcircuit combines 2 smaller meta-vertices into one bigger. Note the lack of I2 in Connections1
		// I2 is simply unconnected. None other method allows for leaving node of subcircuit unconnected

		// if (Manager.ForceModelCalculation(hCircuit, hModel2) < 0)
		// 	PERFORM_ERROR;
		// Commented to allow for lazy analysis. 

		_CStrNodes Connections3({ "O = O", "I = I"}); //The definition of meta-compoenet conection in a for <boundary node> = <master circuit node>
		if (Manager.IncludeSubcircuit(hCircuit, "M", hModel2, Connections3) < 0) // The meta component insertion to the main circuit
			PERFORM_ERROR;
		/* Model2 included to the main circuits */

		long long hCofactorD = Manager.CreateNewCofactor(hCircuit, { { { "I", "0" }, { "I", "0" } } });
		if (hCofactorD < 0)
			PERFORM_ERROR;
		//The HOSC in denominator. See (1)

		long long hCofactorN = Manager.CreateNewCofactor(hCircuit, { { { "I", "0" }, { "O", "0" } } });
		if (hCofactorN < 0)
			PERFORM_ERROR;
		//The HOSC in numerator. See (2)



		long long K_u = Manager.DefineTransferFunctionSimp("K_u", hCofactorN, hCofactorD);
		if (K_u<0)
			PERFORM_ERROR;
		//Volatge to voltage transfer function. See (3)
#else
		_CStrNodes Connections({"O = O", "I1 = I1", "I2 = I2", "I3 = 0"}); 
		if (Manager.IncludeSubcircuit(hCircuit, "M", hModel, Connections) < 0) 
			PERFORM_ERROR;
		//The definition of meta-compoenet conection in a for <boundary node> = <master circuit node>
		// The meta component insertion to the main circuit

		long long hCofactorD_D = Manager.CreateNewCofactor(hCircuit, { { { "I1", "I2" }, { "I1", "I2" } } });
		if (hCofactorD_D < 0)
			PERFORM_ERROR;
		//The HOSC in denominator for differential gain. See (4)

		long long hCofactorN_D = Manager.CreateNewCofactor(hCircuit, { { { "I1", "I2" }, { "O", "0" } } });
		if (hCofactorN_D < 0)
			PERFORM_ERROR;
		//The HOSC in numerator for differential gain. See (5)

		long long hCofactorD_C = Manager.CreateNewCofactor(hCircuit, { { { "I1", "I2" }, { "I1", "I2" } }, { { "I1", "0" }, { "I1", "0" } } });
		if (hCofactorD_C < 0)
			PERFORM_ERROR;
		//The HOSC in denominator for common gain. See (6)

		long long hCofactorN_C = Manager.CreateNewCofactor(hCircuit, { { { "I1", "I2" }, { "I1", "I2" } }, { { "I1", "0" }, { "O", "0" } } });
		if (hCofactorN_C < 0)
			PERFORM_ERROR;
		//The HOSC in numerator for common gain. See (7)


		long long hCofactorN_D2 = Manager.CreateNewCofactor(hCircuit, { { { "I1", "0" }, { "O", "0" } }, { { "I2", "0" }, { "I2", "-I1" } } });
		if (hCofactorN_D2 < 0)
			PERFORM_ERROR;
		//The HOSC in numerator for differential gain. See (8)


		long long K_u_d = Manager.DefineTransferFunctionSimp("K_u_d", hCofactorN_D, hCofactorD_D);
		if (K_u_d<0)
			PERFORM_ERROR;
		//Voltage to voltage differential gain. See (9)

		long long K_u_d2 = Manager.DefineTransferFunctionSimp("K_u_d2", hCofactorN_D2, hCofactorD_C);
		if (K_u_d<0)
			PERFORM_ERROR;
		//Voltage to voltage differential gain. See (10)

		long long K_u_c = Manager.DefineTransferFunctionSimp("K_u_c", hCofactorN_C, hCofactorD_C);
		if (K_u_c<0)
			PERFORM_ERROR;
		//Voltage to voltage common gain. See (11)

#endif

		if (Manager.DoTheCalculation(hCircuit)<0) //The nain circuit compution
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0) // WWrite down PDD structure to the file
			PERFORM_ERROR;


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
