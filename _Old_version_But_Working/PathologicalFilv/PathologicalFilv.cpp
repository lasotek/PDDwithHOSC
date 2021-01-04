// PathologicalFilv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <limits>
#define PERFORM_ERROR 		{\
			string str;\
									while((str=(string)LastErrorInfo)!="")\
				printf("%s\n",str.c_str());\
			goto end_of;\
								}

#define ACC 0.0000000000

#define CONDUCTANCES

#define RANGE 0

//#define FORCED

//#define MACRO

int _tmain(int argc, _TCHAR* argv[])
{
#if _DEBUG
	_CrtMemState cms1, cms2, cms3;
	_CrtMemCheckpoint(&cms1);
#endif
	try
	{
		int dst = numeric_limits<size_t>::digits;
		int dul = numeric_limits<unsigned long long>::digits;

		CPDDEngineManager Manager;
#ifdef FORCED
		long hEngine = Manager.GetPDDEngine(0, true, true, true);
#else
		long hEngine = Manager.GetPDDEngine(0, true, false, true);
#endif
		long long hCircuit = Manager.GetNewCircuit(hEngine);
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");
		Manager.DeclareGlobalRefNode(hCircuit, "0");
#ifdef MACRO
		_CStrNodes Connections;
		_CStrNodes BoundaryNodes;
		BoundaryNodes.push_back("C1");
		BoundaryNodes.push_back("C2");
		BoundaryNodes.push_back("V1");
		BoundaryNodes.push_back("V2");
		long VMCM = 0;
		if ((VMCM = Manager.CreateNewSubcircuit(hCircuit, "VMCM", BoundaryNodes, NULL, true))<0)
			PERFORM_ERROR;
		if (Manager.AddNullor(hCircuit, "C1", "5", "V1", "7", VMCM)<0)
			PERFORM_ERROR;
		//if(Manager.AddNullor(hCircuit,"8","7","5","6",VMCM)<0)
		if (Manager.AddNullor(hCircuit, "7", "8", "5", "6", VMCM)<0)
			PERFORM_ERROR;
		if (Manager.AddNullor(hCircuit, "C2", "6", "V2", "8", VMCM)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "1a", "5", "0", 1, VMCM, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "1b", "6", "0", 1, VMCM, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "1c", "7", "0", 1, VMCM, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "1d", "8", "0", 1, VMCM, false)<0)
			PERFORM_ERROR;
#endif
#ifdef FORCED
		if (Manager.AddResistor(hCircuit, "g1", "0", "1", 1, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g7", "0", "7", 7, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g28", "6", "7", 28, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g13", "1", "7", 13, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g20", "3", "5", 20, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g4", "0", "4", 4, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g10", "1", "4", 10, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g25", "4", "7", 25, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g19", "3", "4", 19, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g3", "0", "3", 3, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g23", "4", "5", 23, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g5", "0", "5", 5, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g6", "0", "6", 6, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g22", "3", "7", 22, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g18", "2", "7", 18, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g9", "1", "3", 9, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g14", "2", "3", 14, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g2", "0", "2", 2, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g8", "1", "2", 8, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g16", "2", "5", 16, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g11", "1", "5", 11, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g17", "2", "6", 17, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g27", "5", "7", 27, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g12", "1", "6", 12, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g15", "2", "4", 15, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g24", "4", "6", 24, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g21", "3", "6", 21, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g26", "5", "6", 26, true)<0)
			PERFORM_ERROR;
#else
#ifdef CONDUCTANCES
		if (Manager.AddResistor(hCircuit, "g1", "0", "1", 1, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g2", "0", "2", 2, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g3", "0", "3", 3, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g4", "0", "4", 4, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g5", "0", "5", 5, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g6", "0", "6", 6, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g7", "0", "7", 7, true)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "g8", "1", "2", 8, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g9", "1", "3", 9, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g10", "1", "4", 10, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g11", "1", "5", 11, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g12", "1", "6", 12, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g13", "1", "7", 13, true)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "g14", "2", "3", 14, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g15", "2", "4", 15, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g16", "2", "5", 16, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g17", "2", "6", 17, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g18", "2", "7", 18, true)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "g19", "3", "4", 19, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g20", "3", "5", 20, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g21", "3", "6", 21, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g22", "3", "7", 22, true)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "g23", "4", "5", 23, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g24", "4", "6", 24, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g25", "4", "7", 25, true)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "g26", "5", "6", 26, true)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "g27", "5", "7", 27, true)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "g28", "6", "7", 28, true)<0)
			PERFORM_ERROR;
#else
		if (Manager.AddResistor(hCircuit, "r1", "0", "1", 1.0 / 1, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r2", "0", "2", 1.0 / 2, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r3", "0", "3", 1.0 / 3, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r4", "0", "4", 1.0 / 4, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r5", "0", "5", 1.0 / 5, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r6", "0", "6", 1.0 / 6, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r7", "0", "7", 1.0 / 7, false)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "r8", "1", "2", 1.0 / 8, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r9", "1", "3", 1.0 / 9, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r10", "1", "4", 1.0 / 10, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r11", "1", "5", 1.0 / 11, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r12", "1", "6", 1.0 / 12, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r13", "1", "7", 1.0 / 13, false)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "r14", "2", "3", 1.0 / 14, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r15", "2", "4", 1.0 / 15, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r16", "2", "5", 1.0 / 16, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r17", "2", "6", 1.0 / 17, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r18", "2", "7", 1.0 / 18, false)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "r19", "3", "4", 1.0 / 19, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r20", "3", "5", 1.0 / 20, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r21", "3", "6", 1.0 / 21, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r22", "3", "7", 1.0 / 22, false)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "r23", "4", "5", 1.0 / 23, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r24", "4", "6", 1.0 / 24, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r25", "4", "7", 1.0 / 25, false)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "r26", "5", "6", 1.0 / 26, false)<0)
			PERFORM_ERROR;
		if (Manager.AddResistor(hCircuit, "r27", "5", "7", 1.0 / 27, false)<0)
			PERFORM_ERROR;

		if (Manager.AddResistor(hCircuit, "r28", "6", "7", 1.0 / 28, false)<0)
			PERFORM_ERROR;
#endif
#endif
#ifdef MACRO
		Connections.clear();
		Connections.push_back("C1 = 0");
		Connections.push_back("C2 = 1");
		Connections.push_back("V1 = 1");
		Connections.push_back("V2 = 2");
		if (Manager.IncludeSubcircuit(hCircuit, "M1", VMCM, Connections))
			PERFORM_ERROR;

		Connections.clear();
		Connections.push_back("C1 = 2");
		Connections.push_back("C2 = 3");
		Connections.push_back("V1 = 3");
		Connections.push_back("V2 = 4");
		if (Manager.IncludeSubcircuit(hCircuit, "M2", VMCM, Connections))
			PERFORM_ERROR;

		Connections.clear();
		Connections.push_back("C1 = 6");
		Connections.push_back("C2 = 5");
		Connections.push_back("V1 = 5");
		Connections.push_back("V2 = 4");
		if (Manager.IncludeSubcircuit(hCircuit, "M3", VMCM, Connections))
			PERFORM_ERROR;
#else
		if (Manager.AddCurrentConveyor(hCircuit, "1", "2", "0", CONV_ICCIIp)<0)
			PERFORM_ERROR;
		if (Manager.AddCurrentConveyor(hCircuit, "3", "4", "2", CONV_ICCIIp)<0)
			PERFORM_ERROR;
		if (Manager.AddCurrentConveyor(hCircuit, "5", "4", "6", CONV_ICCIIp)<0)
			PERFORM_ERROR;
#endif

		long long hCofactor_ab = Manager.CreateNewCofactor(hCircuit);
		if (hCofactor_ab<0)
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_ab, "7", "0", "6", "7"))
			PERFORM_ERROR;

		long long hCofactor_aa = Manager.CreateNewCofactor(hCircuit);
		if (hCofactor_aa<0)
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_aa, "7", "0", "7", "0"))
			PERFORM_ERROR;

		long long Ku = Manager.DefineTransferFunctionSimp("Ku", hCofactor_ab, hCofactor_aa);
		if (Ku<0)
			PERFORM_ERROR;
		if (Manager.DoTheCalculation(hCircuit)<0)
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0)
			PERFORM_ERROR;

		if (Manager.WriteDirectNmericalPolynomial(hCofactor_aa, EmptyString, "Models.txt", true, false, RANGE)<0)
			PERFORM_ERROR;
		if (Manager.WriteDirectNmericalPolynomial(hCofactor_ab, EmptyString, "Models.txt", true, false, RANGE)<0)
			PERFORM_ERROR;

		long PT = 0;
		if ((PT = Manager.GetFreePathTraitor(hCircuit))<0)
			PERFORM_ERROR;
		if (Manager.SetApproximationCriterion(hCircuit, PT, true ,ACC, RANGE, 0)<0)
			PERFORM_ERROR;
		if (Manager.WriteFlatSimplifiedPDD(hCircuit, PT, true, true, "Models.txt")<0)
			PERFORM_ERROR;
		//if(Manager.WriteFlatSimplifiedFeasibleDividers(hCircuit,PT,"Models.txt")<0)
		//	PERFORM_ERROR;
		//if(Manager.WriteFlatSimplifiedSoP(hCircuit,PT,true,"Models.txt")<0)
		//	PERFORM_ERROR;
		//if(Manager.TestFlatPDDRemoval(hCircuit,PT)<0)
		//	PERFORM_ERROR;
		//if(Manager.WriteFlatSimplifiedPDD(hCircuit,PT,"Models.txt")<0)
		//	PERFORM_ERROR;
		if (Manager.ReleasePathTraitor(hCircuit, PT)<0)
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
