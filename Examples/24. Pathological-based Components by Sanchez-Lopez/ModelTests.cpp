// ModelTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define PERFORM_ERROR 		{\
			string str;\
			while((str=(string)LastErrorInfo)!="")\
				printf("%s\n",str.c_str());\
			goto end_of;\
		}

#define ACC 0.001

#define RESISTANCES

#define MAIN_CRICUIT_AS_A_MODEL

#define RANGE 1e6

int _tmain(int argc, _TCHAR* argv[])
{
#if _DEBUG
	_CrtMemState cms1, cms2, cms3;
 	_CrtMemCheckpoint(&cms1);
#endif
	try
	{
		CPDDEngineManager Manager;
		long hEngine=Manager.GetPDDEngine(0,true,false,true);
		long long hCircuit=Manager.GetNewCircuit(hEngine);
		Manager.SetGlobalModelFileName(hCircuit,"Models.txt",true);
		Manager.SetGlobalRaportFileName(hCircuit,"Models.txt");
		Manager.DeclareGlobalRefNode(hCircuit,"0");
		//_CStrNodes Connections;
		_CStrNodes BoundaryNodes;

		//==================FBCFOANew2
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("y1");
			BoundaryNodes.push_back("y2");
			BoundaryNodes.push_back("x1");
			BoundaryNodes.push_back("x2");
			BoundaryNodes.push_back("z1");
			BoundaryNodes.push_back("z2");
			BoundaryNodes.push_back("w1");
			BoundaryNodes.push_back("w2");
			BoundaryNodes.push_back("c");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCFOANew2", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.SetCompPriority(hCircuit, Model, 0) < 0)
				PERFORM_ERROR;
			//if (Manager.AddResistor(hCircuit, "Rm", "z1", "c", 1e6, Model, false) < 0)
			//	PERFORM_ERROR;
			//if (Manager.AddResistor(hCircuit, "Rm", "z2", "c", 1e6, Model, false) < 0)
			//	PERFORM_ERROR;
			if (Manager.ClearCompPriority(hCircuit, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kx", "1", "c", "y1", "c", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kx", "x1'", "1", "c", "y2", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kx", "c", "2", "y1", "c", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kx", "2", "x2'", "c", "y2", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "kz", "z1", "z2'", "x1", "x1'", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "kz", "z2'", "z1", "x2", "x2'", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddNullor(hCircuit, "z2'", "z2", "z2'", "c", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kw", "3", "c", "z1", "c", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kw", "w1", "3", "c", "z2", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kw", "c", "4", "z1", "c", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kw", "4", "w2'", "c", "z2", 0, 0.5, Model) < 0)
				PERFORM_ERROR;

			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCFOANew2
		////==================FBCFOANew2NoC
		//{
		//	BoundaryNodes.clear();
		//	BoundaryNodes.push_back("y1");
		//	BoundaryNodes.push_back("y2");
		//	BoundaryNodes.push_back("x1");
		//	BoundaryNodes.push_back("x2");
		//	BoundaryNodes.push_back("z1");
		//	BoundaryNodes.push_back("z2");
		//	BoundaryNodes.push_back("w1");
		//	BoundaryNodes.push_back("w2");
		//	long Model = 0;
		//	if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCFOANew2NoC", BoundaryNodes, NULL, true))<0)
		//		PERFORM_ERROR;
		//	if (Manager.SetCompPriority(hCircuit, Model, 0) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.ClearCompPriority(hCircuit, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageGain(hCircuit, "kx", "1", "c", "y1", "c", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageGain(hCircuit, "kx", "x1'", "1", "c", "y2", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageGain(hCircuit, "kx", "c", "2", "y1", "c", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageGain(hCircuit, "kx", "2", "x2'", "c", "y2", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddCurrentGain(hCircuit, "kz", "z1", "z2'", "x1", "x1'", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddCurrentGain(hCircuit, "kz", "z2'", "z1", "x2", "x2'", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddNullor(hCircuit, "z2'", "z2", "z2'", "c", Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageGain(hCircuit, "kw", "3", "c", "z1", "c", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageGain(hCircuit, "kw", "w1", "3", "c", "z2", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageGain(hCircuit, "kw", "c", "4", "z1", "c", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageGain(hCircuit, "kw", "4", "w2'", "c", "z2", 0, 0.5, Model) < 0)
		//		PERFORM_ERROR;

		//	if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
		//		PERFORM_ERROR;
		//}
		////==================FBCFOANew2NoC
		//==================FBCFOANew2gnd
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("y1");
			BoundaryNodes.push_back("y2");
			BoundaryNodes.push_back("x1");
			BoundaryNodes.push_back("x2");
			BoundaryNodes.push_back("z1");
			BoundaryNodes.push_back("z2");
			BoundaryNodes.push_back("w1");
			BoundaryNodes.push_back("w2");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCFOANew2gnd", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			//if (Manager.AddResistor(hCircuit, "Rm", "z1", "0", 1e6, Model, false) < 0)
			//	PERFORM_ERROR;
			//if (Manager.AddResistor(hCircuit, "Rm", "z2", "0", 1e6, Model, false) < 0)
			//	PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kx", "1", "0", "y1", "0", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kx", "x1'", "1", "0", "y2", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kx", "0", "2", "y1", "0", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kx", "2", "x2'", "0", "y2", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "kz", "z1", "z2'", "x1", "x1'", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "kz", "z2'", "z1", "x2", "x2'", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddNullor(hCircuit, "z2'", "z2", "z2", "0", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kw", "3", "0", "z1", "0", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kw", "w1", "3", "0", "z2", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kw", "0", "4", "z1", "0", 0, 0.5, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "kw", "4", "w2'", "0", "z2", 0, 0.5, Model) < 0)
				PERFORM_ERROR;

			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCFOANew2gnd
		//==================FBCCIINew
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("y1");
			BoundaryNodes.push_back("y2");
			BoundaryNodes.push_back("x1");
			BoundaryNodes.push_back("x2");
			BoundaryNodes.push_back("z1");
			BoundaryNodes.push_back("z2");
			//BoundaryNodes.push_back("c");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCCIINew", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "z1", "c", "c", "z2", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "z1", "c", "x1", "x1'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "z2", "c", "x2", "x2'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x1'", "c", "y1", "c", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x2'", "c", "y2", "c", Model) < 0)
				PERFORM_ERROR;

			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCIINew
		//==================FBCCIIINew
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("y1");
			BoundaryNodes.push_back("y2");
			BoundaryNodes.push_back("x1");
			BoundaryNodes.push_back("x2");
			BoundaryNodes.push_back("z1");
			BoundaryNodes.push_back("z2");
			//BoundaryNodes.push_back("c");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCCIIINew", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "z1'", "c", "c", "z2'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "z1'", "c", "x1", "x1'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "z2'", "c", "x2", "x2'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x1'", "c", "y1", "c", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x2'", "c", "y2", "c", Model) < 0)
				PERFORM_ERROR;

			if (Manager.AddCurrentFollower(hCircuit, "y1", "c", "z1", "z1'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "y2", "c", "z2", "z2'", Model) < 0)
				PERFORM_ERROR;

			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCIIINew
		//==================FBCCINew
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("y1");
			BoundaryNodes.push_back("y2");
			BoundaryNodes.push_back("x1");
			BoundaryNodes.push_back("x2");
			BoundaryNodes.push_back("z1");
			BoundaryNodes.push_back("z2");
			//BoundaryNodes.push_back("c");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCCINew", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "z1'", "c", "c", "z2'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "z1'", "c", "x1", "x1'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "z2'", "c", "x2", "x2'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x1'", "c", "y1", "c", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x2'", "c", "y2", "c", Model) < 0)
				PERFORM_ERROR;

			if (Manager.AddCurrentFollower(hCircuit, "y1", "c", "z1'", "z1", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "y2", "c", "z2'", "z2", Model) < 0)
				PERFORM_ERROR;

			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCIIINew
		//==================FBCFOANew
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("y1");
			BoundaryNodes.push_back("y2");
			BoundaryNodes.push_back("x1");
			BoundaryNodes.push_back("x2");
			BoundaryNodes.push_back("z1");
			BoundaryNodes.push_back("z2");
			BoundaryNodes.push_back("w1");
			BoundaryNodes.push_back("w2");
			long Model = 0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FBCFOANew",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			//if (Manager.AddPathologicalPair(hCircuit, "1", "0", "z1", "1", false, false, Model) < 0)
			//	PERFORM_ERROR;
			//if (Manager.AddPathologicalPair(hCircuit, "2", "0", "z2", "2", false, false, Model) < 0)
			//	PERFORM_ERROR;
			//if (Manager.AddVoltageFollower(hCircuit, "1", "0", "z1", "c", Model) < 0)
			//	PERFORM_ERROR;
			//if (Manager.AddVoltageFollower(hCircuit, "2", "0", "z1", "c", Model) < 0)
			//	PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "z1", "c", "c", "z2", Model) < 0)
				PERFORM_ERROR;
			//if (Manager.AddResistor(hCircuit, "Rm", "1", "c", 100e6, Model, false) < 0)
			//	PERFORM_ERROR;
			//if (Manager.AddResistor(hCircuit, "Rm", "2", "c", 100e6, Model, false) < 0)
			//	PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "w1", "c", "z1", "c", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "w2", "c", "z2", "c", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "z1", "c", "x1", "x1'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "z2", "c", "x2", "x2'", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x1'", "c", "y1", "c", Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x2'", "c", "y2", "c", Model) < 0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FBCFOANew


		//==================FBCCII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCCIIS", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;

			if (Manager.AddPathologicalPair(hCircuit, "3", "1", "1", "V_y_1", true, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "3", "2", "2", "V_y_2", false, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "4", "V_x_1", "V_x_1", "3", false, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "4", "V_x_2", "V_x_2", "3", false, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "I_z_2", "5", "5", "4", false, true, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "I_z_1", "6", "6", "4", false, false, Model)<0)
				PERFORM_ERROR;

			if (Manager.AddResistor(hCircuit, "1", "1", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "2", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "3", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "4", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "5", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "6", "0", 1, Model, false)<0)
				PERFORM_ERROR;

			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCII

		//==================FBCCII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCCIIS2", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;

			if (Manager.AddPathologicalPair(hCircuit, "3", "1", "1", "V_y_1", true, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "3", "2", "2", "V_y_2", false, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "4", "V_x_1", "V_x_1", "3", false, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "4", "V_x_2", "V_x_2", "3", true, true, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "I_z_2", "5", "5", "4", false, true, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "I_z_1", "6", "6", "4", false, false, Model)<0)
				PERFORM_ERROR;

			if (Manager.AddResistor(hCircuit, "1", "1", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "2", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "3", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "4", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "5", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "6", "0", 1, Model, false)<0)
				PERFORM_ERROR;

			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCII

		//==================FDCCII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_y_3");
			BoundaryNodes.push_back("V_y_4");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FDCCII", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;

			if (Manager.AddPathologicalPair(hCircuit, "7", "1", "1", "V_y_3", false, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "7", "2", "2", "V_y_2", true, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "7", "3", "3", "V_y_1", false, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "8", "4", "4", "V_y_1", true, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "8", "5", "5", "V_y_2", false, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "8", "6", "6", "V_y_4", false, false, Model)<0)
				PERFORM_ERROR;

			if (Manager.AddPathologicalPair(hCircuit, "I_z_2", "V_x_2", "V_x_2", "8", true, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "I_z_1", "V_x_1", "V_x_1", "7", true, false, Model)<0)
				PERFORM_ERROR;

			if (Manager.AddResistor(hCircuit, "1", "1", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "2", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "3", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "4", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "5", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "6", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "7", "0", 1, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "8", "0", 1, Model, false)<0)
				PERFORM_ERROR;

			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FDCCII
		//==================FBCCI
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FBCCIS",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","V_y_1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","V_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_1","V_x_1","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_2","V_x_2","3",true,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","5","5","4",false,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","6","6","4",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"V_y_1","7","7","5",false,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"V_y_2","7","7","6",true,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCI


		//==================FBCCIII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FBCCIIIS",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","V_y_1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","V_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_1","V_x_1","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_2","V_x_2","3",true,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","5","5","4",false,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","6","6","4",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"V_y_1","7","7","5",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"V_y_2","7","7","6",true,true,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCIII







		////==================FBCFOANew2
		//{
		//	BoundaryNodes.clear();
		//	BoundaryNodes.push_back("y1");
		//	BoundaryNodes.push_back("y2");
		//	BoundaryNodes.push_back("x1");
		//	BoundaryNodes.push_back("x2");
		//	BoundaryNodes.push_back("z1");
		//	BoundaryNodes.push_back("z2");
		//	BoundaryNodes.push_back("w1");
		//	BoundaryNodes.push_back("w2");
		//	long Model = 0;
		//	if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCFOANew2", BoundaryNodes, NULL, true))<0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageFollower(hCircuit, "1", "c", "z1", "0", Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageFollower(hCircuit, "2", "0", "z1", "0", Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageFollower(hCircuit, "1", "c", "c", "2", Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageFollower(hCircuit, "w1", "c", "z1", "0", Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageFollower(hCircuit, "w2", "c", "z2", "0", Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddCurrentFollower(hCircuit, "z1", "0", "x1", "x1'", Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddCurrentFollower(hCircuit, "z2", "0", "x2", "x2'", Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageFollower(hCircuit, "x1'", "c", "y1", "c", Model) < 0)
		//		PERFORM_ERROR;
		//	if (Manager.AddVoltageFollower(hCircuit, "x2'", "c", "y2", "c", Model) < 0)
		//		PERFORM_ERROR;

		//	if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
		//		PERFORM_ERROR;
		//}
		////==================FBCFOANew2
		//==================CCIp
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CCIp1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","Y",Model))
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CCIp
		//==================CCIp_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CCIp_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CCIp_Rx

		//==================CCIm
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CCIm1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","Y",Model))
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CCIm

		//==================CCIm_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CCIm_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CCIm_Rx

		//==================ICCIp
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIp1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","9",Model))
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIp
		//==================ICCIp_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIp_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIp_Rx


		//==================ICCIm
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIm1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIm

		//==================ICCIm_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIm_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIm_Rx

		//==================MOICCI
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Z1p");
			BoundaryNodes.push_back("Z1m");
			BoundaryNodes.push_back("Z2p");
			BoundaryNodes.push_back("Z2m");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"MOICCI1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1p","7","7","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1m","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2p","13","13","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2m","16","16","15",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"15","14","14","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","13","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","14","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","15","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","16","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================MOICCI
	

		//==================MOICCI_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Z1p");
			BoundaryNodes.push_back("Z1m");
			BoundaryNodes.push_back("Z2p");
			BoundaryNodes.push_back("Z2m");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"MOICCI_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","6","6","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1p","7","7","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1m","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2p","13","13","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2m","16","16","15",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"15","14","14","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","13","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","14","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","15","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","16","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================MOICCI_Rx

		//==================CIIp
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CIIp1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CIIp


		//==================CIIp_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CIIp_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CIIp_Rx

		//==================CIIm
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CIIm1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"Zp","4","4","Y",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","X","X","Y",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CIIm

		//==================CIIm_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CIIm_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","4","4","Y",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CIIm_Rx

		//==================ICCIIp
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIIp1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIIp

		//==================ICCIIp_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIIp_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIIp_Rx

		//==================ICCIIm
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIIm1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"Zm","4","4","9",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","X","X","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIIm

		//==================ICCIIm_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIIm_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","4","4","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIIm_Rx

		//==================MOICCII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Z1p");
			BoundaryNodes.push_back("Z1m");
			BoundaryNodes.push_back("Z2p");
			BoundaryNodes.push_back("Z2m");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"MOICCII1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1p","7","7","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1m","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2p","13","13","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2m","16","16","15",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"15","14","14","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","13","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","14","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","15","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","16","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================MOICCII

		//==================MOICCII_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Z1p");
			BoundaryNodes.push_back("Z1m");
			BoundaryNodes.push_back("Z2p");
			BoundaryNodes.push_back("Z2m");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"MOICCII_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1p","7","7","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1m","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2p","13","13","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2m","16","16","15",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"15","14","14","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","13","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","14","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","15","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","16","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================MOICCII_Rx

		//==================CCIIIp
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CCIIIp1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CCIIIp

		//==================CCIIIp_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CCIIIp_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CCIIIp_Rx

		//==================CIIIm
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CIIIm1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CIIIm

		//==================CIIIm_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CIIIm_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CIIIm_Rx

		//==================ICCIIIp
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIIIp1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIIIp

		//==================ICCIIIp_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zp");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICCIIIp_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zp","7","7","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICCIIIp_Rx

		//==================ICIIIm
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICIIIm1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICIIIm

		//==================ICIIIm_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Zm");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"ICIIIm_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","9",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"9","8","8","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Zm","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================ICIIIm_Rx

		//==================MOICCIII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Z1p");
			BoundaryNodes.push_back("Z1m");
			BoundaryNodes.push_back("Z2p");
			BoundaryNodes.push_back("Z2m");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"MOICCIII1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			//if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
			//	PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","X","X","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1p","7","7","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1m","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2p","13","13","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2m","16","16","15",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"15","14","14","5",Model))
				PERFORM_ERROR;

			//if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
			//	PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","13","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","14","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","15","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","16","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================MOICCIII

		//==================MOICCIII_Rx
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Z1p");
			BoundaryNodes.push_back("Z1m");
			BoundaryNodes.push_back("Z2p");
			BoundaryNodes.push_back("Z2m");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"MOICCIII_Rx1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"2","1","1","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Y","3","3","2",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"5","4","4","Y",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1p","7","7","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z1m","12","12","11",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"11","10","10","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2p","13","13","5",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"Z2m","16","16","15",Model))
				PERFORM_ERROR;
			if(Manager.AddNullor(hCircuit,"15","14","14","5",Model))
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rx","X","4",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","11","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","12","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","13","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","14","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","15","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","16","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================MOICCIII_Rx

		//==================OTRA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Ip");
			BoundaryNodes.push_back("Im");
			BoundaryNodes.push_back("Vo");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"OTRA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Ip","Ip","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Im","Im","0",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"0","Vo","Vo","1",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","1","0",10,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================OTRA

		//==================FDOTRA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("I_in_p");
			BoundaryNodes.push_back("I_in_m");
			BoundaryNodes.push_back("V_o_p");
			BoundaryNodes.push_back("V_o_m");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDOTRA",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","I_in_p","I_in_p","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","I_in_m","I_in_m","0",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"0","V_o_p","V_o_p","1",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"0","V_o_m","V_o_m","1",true,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","1","0",10,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDOTRA

		//==================FDOTRA2
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("i1");
			BoundaryNodes.push_back("i2");
			BoundaryNodes.push_back("o1");
			BoundaryNodes.push_back("o2");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FDOTRA2", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddTransimpedance(hCircuit, "R_m", "1", "0", "i1'", "0", 0, 10, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddTransimpedance(hCircuit, "R_m", "1", "o1", "i2'", "0", 0, 10, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddTransimpedance(hCircuit, "R_m", "0", "2", "i1", "i1'", 0, 10, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddTransimpedance(hCircuit, "R_m", "o2", "2", "i2", "i2'", 0, 10, Model) < 0)
				PERFORM_ERROR;



			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FDOTRA2
		//==================FDFOTRA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("I_xp");
			BoundaryNodes.push_back("I_xn");
			BoundaryNodes.push_back("I_zp");
			BoundaryNodes.push_back("I_zn");
			BoundaryNodes.push_back("V_wp");
			BoundaryNodes.push_back("V_wn");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDFOTRA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","I_xp","I_xp","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","I_xn","I_xn","0",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_zp","V_wp","V_wp","1",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_zn","V_zn","V_zn","1",false,true,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","1","0",10,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDFOTRA

		//==================FDCOA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("I_in_p");
			BoundaryNodes.push_back("I_in_m");
			BoundaryNodes.push_back("I_o_p");
			BoundaryNodes.push_back("I_o_m");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDCOA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","I_in_p","I_in_p","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","I_in_m","I_in_m","0",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_o_m","2","2","1",false,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_o_p","3","3","1",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1/A1","2","0",10,Model,true)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1/A2","3","0",10,Model,true)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDCOA

		//==================COA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Ip");
			BoundaryNodes.push_back("Im");
			BoundaryNodes.push_back("Io");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"COA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Ip","Ip","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Im","Im","0",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Io","2","2","1",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"g1","1","0",1,Model,true)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"g2","2","0",0.1,Model,true)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================COA

		//==================FOTRA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Ip");
			BoundaryNodes.push_back("Im");
			BoundaryNodes.push_back("Vw");
			BoundaryNodes.push_back("Iz");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FOTRA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Ip","Ip","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Im","Im","0",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Iz","Vw","Vw","1",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","1","0",10,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FOTRA

		//==================CFOA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Vp");
			BoundaryNodes.push_back("Vm");
			BoundaryNodes.push_back("Vo");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CFOA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Vm","Vm","Vp",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"0","Vo","Vo","1",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","1","0",10,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CFOA
		//==================CFOA2
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Vp");
			BoundaryNodes.push_back("Vm");
			BoundaryNodes.push_back("Vo");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CFOA21",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Vm","Vm","Vp",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","1",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"0","Vo","Vo","3",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","3","0",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",10,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CFOA2

		//==================CFBOTA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Vp");
			BoundaryNodes.push_back("Vm");
			BoundaryNodes.push_back("Io");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CFBOTA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Vm","Vm","Vp",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Io","2","2","1",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"g1","1","0",1,Model,true)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"g2","2","0",10,Model,true)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CFBOTA

		//==================CFC
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Vp");
			BoundaryNodes.push_back("Vm");
			BoundaryNodes.push_back("Vo");
			BoundaryNodes.push_back("Io");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CFC1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Vm","Vm","Vp",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Io","Vo","Vo","1",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","1","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CFC

		//==================FDOTRA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Ip");
			BoundaryNodes.push_back("Im");
			BoundaryNodes.push_back("Vop");
			BoundaryNodes.push_back("Vom");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDOTRA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Ip","Ip","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Im","Im","0",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"0","Vop","Vop","1",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"0","Vom","Vom","1",false,true,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","1","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDOTRA

		//==================FDCOA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Ip");
			BoundaryNodes.push_back("Im");
			BoundaryNodes.push_back("Iop");
			BoundaryNodes.push_back("Iom");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDCOA11",BoundaryNodes,NULL,true))<0)//
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Ip","Ip","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Im","Im","0",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Iop","2","2","1",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Iom","3","3","1",false,true,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"A1","2","0",1,Model,true)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"A2","3","0",1,Model,true)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDCOA

		//==================FDCOA2
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Ip");
			BoundaryNodes.push_back("Im");
			BoundaryNodes.push_back("Iop");
			BoundaryNodes.push_back("Iom");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDCOA2",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Ip","Ip","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Im","Im","0",true,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddTransconductance(hCircuit,"A","Iop","Iom","1","0",1,10,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDCOA2

		//==================FDFOTRA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Xp");
			BoundaryNodes.push_back("Xn");
			BoundaryNodes.push_back("Wp");
			BoundaryNodes.push_back("Wn");
			BoundaryNodes.push_back("Zp");
			BoundaryNodes.push_back("Zn");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDFOTRA11",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Xp","Xp","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","Xn","Xn","0",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Zp","Wp","Wp","1",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Zn","Wn","Wn","1",false,true,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","1","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDFOTRA

		//==================FDCFOA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_X_p");
			BoundaryNodes.push_back("V_X_n");
			BoundaryNodes.push_back("V_Y_p");
			BoundaryNodes.push_back("V_Y_n");
			BoundaryNodes.push_back("I_Z_p");
			BoundaryNodes.push_back("I_Z_n");
			BoundaryNodes.push_back("V_W_p");
			BoundaryNodes.push_back("V_W_n");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDCFOA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","V_Y_p",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","V_Y_n",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"I_Z_p","V_X_p","V_X_p","3",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_Z_n","V_X_n","V_X_n","3",true,true,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"0","V_W_p","V_W_p","I_Z_p",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"0","V_W_n","V_W_n","I_Z_n",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"g_m_1","I_Z_p","0",1,Model,true)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"g_m_2","I_Z_n","0",1,Model,true)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,true)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,true)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,true)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDCFOA


		//==================FDCFBOTA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Xp");
			BoundaryNodes.push_back("Xn");
			BoundaryNodes.push_back("Yp");
			BoundaryNodes.push_back("Yn");
			BoundaryNodes.push_back("Iop");
			BoundaryNodes.push_back("Ion");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDCFBOTA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","Yp",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","Yp",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"4","Xp","Xp","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","Xn","Xn","3",true,true,Model)<0)
				PERFORM_ERROR;
			
			if(Manager.AddTransconductance(hCircuit,"A","Iop","Ion","4","0",1,10,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDCFBOTA

		//==================FDOFC
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Xp");
			BoundaryNodes.push_back("Xn");
			BoundaryNodes.push_back("Yp");
			BoundaryNodes.push_back("Yn");
			BoundaryNodes.push_back("Zp");
			BoundaryNodes.push_back("Zn");
			BoundaryNodes.push_back("Wp");
			BoundaryNodes.push_back("Wn");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDOFC1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","Yp",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","Yp",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"4","Xp","Xp","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","Xn","Xn","3",true,true,Model)<0)
				PERFORM_ERROR;
			
			if(Manager.AddPathologicalPair(hCircuit,"Zp","Wp","Wp","4",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Zn","Wn","Wn","4",false,true,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"Rm","4","0",10,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDOFC

		//==================FDOFA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Xp");
			BoundaryNodes.push_back("Xn");
			BoundaryNodes.push_back("Yp");
			BoundaryNodes.push_back("Yn");
			BoundaryNodes.push_back("Zp");
			BoundaryNodes.push_back("Zn");
			BoundaryNodes.push_back("Wp");
			BoundaryNodes.push_back("Wn");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDOFA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"5","1","1","Xp",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","2","2","Xn",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddPathologicalPair(hCircuit,"6","3","3","Yp",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"6","4","4","Yn",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"Zn","7","7","Wp",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"Zp","8","8","Wn",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"Wp","Wn","5","6",true,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","Wp","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","Wn","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDOFA

		//==================FDVDBA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("Vp");
			BoundaryNodes.push_back("Vn");
			BoundaryNodes.push_back("Zp");
			BoundaryNodes.push_back("Zn");
			BoundaryNodes.push_back("Wp");
			BoundaryNodes.push_back("Wn");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDVDBA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"0","Wn","Wn","Zn",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"0","Wp","Wp","Zp",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddTransconductance(hCircuit,"gm","Zp","Zn","Vp","Vn",1,10,Model)<0)
				PERFORM_ERROR;


			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDVDBA

		//==================DDOFA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V1");
			BoundaryNodes.push_back("V2");
			BoundaryNodes.push_back("V3");
			BoundaryNodes.push_back("V4");
			BoundaryNodes.push_back("Iop");
			BoundaryNodes.push_back("Ion");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"DDOFA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"5","1","1","V1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","2","2","V2",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddPathologicalPair(hCircuit,"6","3","3","V3",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"6","4","4","V4",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddTransconductance(hCircuit,"gm","Iop","Ion","5","6",1,10,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================DDOFA

		//==================FBDDA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V1");
			BoundaryNodes.push_back("V2");
			BoundaryNodes.push_back("V3");
			BoundaryNodes.push_back("V4");
			BoundaryNodes.push_back("Vop");
			BoundaryNodes.push_back("Von");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FBDDA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"5","1","1","V1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","2","2","V2",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddPathologicalPair(hCircuit,"6","3","3","V3",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"6","4","4","V4",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddPathologicalPair(hCircuit,"Vop","Von","5","6",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FBDDA

		//==================DDOMA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V1");
			BoundaryNodes.push_back("V2");
			BoundaryNodes.push_back("V3");
			BoundaryNodes.push_back("V4");
			BoundaryNodes.push_back("Vop");
			BoundaryNodes.push_back("Von");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"DDOMA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"5","1","1","V1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","2","2","V2",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddPathologicalPair(hCircuit,"6","3","3","V3",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"6","4","4","V4",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddPathologicalPair(hCircuit,"Vop","Von","5","6",true,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================DDOMA

		//==================CDBA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("p");
			BoundaryNodes.push_back("n");
			BoundaryNodes.push_back("w");
			BoundaryNodes.push_back("z");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CDBA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"z","p","p","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"z","n","n","0",true,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddPathologicalPair(hCircuit,"0","w","w","z",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"Rm","z","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CDBA

		//==================CDTA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("I_x_1");
			BoundaryNodes.push_back("I_x_2");
			BoundaryNodes.push_back("I_w_1");
			BoundaryNodes.push_back("I_w_2");
			BoundaryNodes.push_back("I_z");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"CDTA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"I_z","I_x_1","I_x_1","0",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z","I_x_2","I_x_2","0",true,false,Model)<0)
				PERFORM_ERROR;


			//if(Manager.AddResistor(hCircuit,"Gm","1","0",1,Model,true)<0)
			//	PERFORM_ERROR;

			if(Manager.AddTransconductance(hCircuit,"G_m","I_w_1","I_w_2","I_z","0",0,10,Model)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================CDTA

		//==================DVCFOA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("yp");
			BoundaryNodes.push_back("yn");
			BoundaryNodes.push_back("x");
			BoundaryNodes.push_back("z");
			BoundaryNodes.push_back("w");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"DVCFOA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","yp",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","yn",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddPathologicalPair(hCircuit,"z","x","x","3",true,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"0","w","w","z",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================DVCFOA


		//==================DDCCFOA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("y1");
			BoundaryNodes.push_back("y2");
			BoundaryNodes.push_back("y3");
			BoundaryNodes.push_back("x");
			BoundaryNodes.push_back("z1");
			BoundaryNodes.push_back("z2");
			BoundaryNodes.push_back("w1");
			BoundaryNodes.push_back("w2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"DDCCFOA1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"4","1","1","y1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","2","2","y2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","3","3","y3",true,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"5","x","x","4",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"z1","6","6","5",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"z2","7","7","5",false,true,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"0","w1","w1","z1",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"0","w2","w2","z2",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================DDCCFOA

		//==================DDCCpm
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_y_3");
			BoundaryNodes.push_back("V_x");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"DDCCpm1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"5","1","1","V_y_1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","2","2","V_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","3","3","V_y_3",true,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"4","V_x","V_x","5",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","6","6","4",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","7","7","4",false,true,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================DDCCpm

		//==================DCCIIpm
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"DCCIIpm1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"1","V_x_1","V_x_1","V_y",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"1","V_x_2","V_x_2","V_y",true,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddTransconductance(hCircuit,"A","I_z_1","I_z_2","1","0",0,10.0,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================DCCIIpm

		//==================DXCCII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"DXCCII1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","V_x_1","V_x_1","V_y",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","V_x_2","V_x_2","V_y",true,true,Model)<0)
				PERFORM_ERROR;


			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================DXCCII

		//==================FDVDCCIIpm
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDVDCCIIpm1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","V_y_1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","V_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","V_x_1","V_x_1","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","V_x_2","V_x_2","3",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDVDCCIIpm

		//==================FBCCII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FBCCII1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","V_y_1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","V_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_1","V_x_1","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_2","V_x_2","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","5","5","4",false,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","6","6","4",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCII

		//==================FBCCII
		{
		BoundaryNodes.clear();
		BoundaryNodes.push_back("V_y_1");
		BoundaryNodes.push_back("V_y_2");
		BoundaryNodes.push_back("V_x_1");
		BoundaryNodes.push_back("V_x_2");
		BoundaryNodes.push_back("I_z_1");
		BoundaryNodes.push_back("I_z_2");
		long Model=0;
		if((Model=Manager.CreateNewSubcircuit(hCircuit,"FBCCII2",BoundaryNodes,NULL,true))<0)
		PERFORM_ERROR;

		if(Manager.AddPathologicalPair(hCircuit,"3","1","1","V_y_1",true,false,Model)<0)
		PERFORM_ERROR;
		if(Manager.AddPathologicalPair(hCircuit,"3","2","2","V_y_2",false,false,Model)<0)
		PERFORM_ERROR;
		if(Manager.AddPathologicalPair(hCircuit,"4","V_x_1","V_x_1","3",false,false,Model)<0)
		PERFORM_ERROR;
		if(Manager.AddPathologicalPair(hCircuit,"4","V_x_2","V_x_2","3",false,false,Model)<0)
		PERFORM_ERROR;
		if(Manager.AddPathologicalPair(hCircuit,"I_z_2","5","5","4",false,true,Model)<0)
		PERFORM_ERROR;
		if(Manager.AddPathologicalPair(hCircuit,"I_z_1","6","6","4",false,false,Model)<0)
		PERFORM_ERROR;

		if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
		PERFORM_ERROR;
		if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
		PERFORM_ERROR;
		if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
		PERFORM_ERROR;
		if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
		PERFORM_ERROR;
		if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
		PERFORM_ERROR;
		if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
		PERFORM_ERROR;

		if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
		PERFORM_ERROR;
		}
		//==================FBCCII

		//==================FDCCII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_y_3");
			BoundaryNodes.push_back("V_y_4");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FDCCII1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"7","1","1","V_y_3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"7","2","2","V_y_2",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"7","3","3","V_y_1",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"8","4","4","V_y_1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"8","5","5","V_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"8","6","6","V_y_4",false,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","V_x_2","V_x_2","8",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","V_x_1","V_x_1","7",true,false,Model)<0)
				PERFORM_ERROR;

			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FDCCII

		//==================FBCCI
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FBCCI1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","V_y_1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","V_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_1","V_x_1","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_2","V_x_2","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","5","5","4",false,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","6","6","4",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"V_y_1","7","7","5",false,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"V_y_2","7","7","5",true,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCI


		//==================FBCCIII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"FBCCIII1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"3","1","1","V_y_1",true,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"3","2","2","V_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_1","V_x_1","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"4","V_x_2","V_x_2","3",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","5","5","4",false,true,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","6","6","4",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"V_y_1","7","7","5",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"V_y_2","7","7","5",true,true,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCIII

		//==================DVCCII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("I_y_1");
			BoundaryNodes.push_back("I_y_2");
			BoundaryNodes.push_back("I_x");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"DVCCII1",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"2","1","1","I_y_1",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","3","3","2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","4","4","I_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"10","I_x","I_x","5",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","6","6","10",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"8","7","7","10",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_2","9","9","8",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","7","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","8","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","9","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================DVCCII

		//==================DVCCII2
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("I_y_1");
			BoundaryNodes.push_back("I_y_2");
			BoundaryNodes.push_back("I_x");
			BoundaryNodes.push_back("I_z_1");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"DVCCII2",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if(Manager.AddPathologicalPair(hCircuit,"2","1","1","I_y_1",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","3","3","2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"5","4","4","I_y_2",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"10","I_x","I_x","5",false,false,Model)<0)
				PERFORM_ERROR;
			if(Manager.AddPathologicalPair(hCircuit,"I_z_1","6","6","10",false,false,Model)<0)
				PERFORM_ERROR;


			if(Manager.AddResistor(hCircuit,"1","1","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","2","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","3","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","4","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","5","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","6","0",1,Model,false)<0)
				PERFORM_ERROR;
			if(Manager.AddResistor(hCircuit,"1","10","0",1,Model,false)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================DVCCII2

		//==================FBCCII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCCII", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x_1", "gnd", "V_y_1", "gnd", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "x_2", "gnd", "V_y_2", "gnd", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "I_z_1", "gnd", "V_x_1", "x_1", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "I_z_2", "gnd", "V_x_2", "x_2", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "R_m", "I_z_1", "gnd", 1e6, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "R_m", "I_z_2", "gnd", 1e6, Model, false)<0)
				PERFORM_ERROR;


			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCII

		//==================FBCCIII
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCCIII", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "y_1", "gnd", "2", "x_1", false, false, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "y_2", "gnd", "3", "x_2", false, false, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "2", "gnd", "V_y_1", "gnd", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "3", "gnd", "V_y_2", "gnd", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "x_1", "gnd", "V_y_1", "y_1", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "x_2", "gnd", "V_y_2", "y_2", Model)<0)
				PERFORM_ERROR;

			if (Manager.AddCurrentFollower(hCircuit, "I_z_1", "gnd", "V_x_1", "x_1", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "I_z_2", "gnd", "V_x_2", "x_2", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "R_m", "I_z_1", "gnd", 1e6, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "R_m", "I_z_2", "gnd", 1e6, Model, false)<0)
				PERFORM_ERROR;


			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCIII

		//==================FBCCI
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_y_1");
			BoundaryNodes.push_back("V_y_2");
			BoundaryNodes.push_back("V_x_1");
			BoundaryNodes.push_back("V_x_2");
			BoundaryNodes.push_back("I_z_1");
			BoundaryNodes.push_back("I_z_2");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FBCCI", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "y_1", "gnd", "2", "x_1", false, false, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "y_2", "gnd", "3", "x_2", false, false, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "2", "gnd", "V_y_1", "gnd", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddVoltageFollower(hCircuit, "3", "gnd", "V_y_2", "gnd", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "x_1", "gnd", "V_y_1", "y_1", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "x_2", "gnd", "y_2", "V_y_2", Model)<0)
				PERFORM_ERROR;

			if (Manager.AddCurrentFollower(hCircuit, "I_z_1", "gnd", "V_x_1", "x_1", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentFollower(hCircuit, "I_z_2", "gnd", "V_x_2", "x_2", Model)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "R_m", "I_z_1", "gnd", 1e6, Model, false)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "R_m", "I_z_2", "gnd", 1e6, Model, false)<0)
				PERFORM_ERROR;


			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FBCCI

		//==================OTRA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("I_p");
			BoundaryNodes.push_back("I_n");
			BoundaryNodes.push_back("V_O");
			BoundaryNodes.push_back("0");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "OTRA", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddTransimpedance(hCircuit, "R_m", "V_O", "1", "I_p", "0", 0, 1e6, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddTransimpedance(hCircuit, "R_m", "0", "1", "I_n", "0", 0, 1e6, Model) < 0)
				PERFORM_ERROR;


			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================OTRA

		//==================COA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("I_p");
			BoundaryNodes.push_back("I_n");
			BoundaryNodes.push_back("I_O");
			BoundaryNodes.push_back("0");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "COA", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "\\alpha", "I_O", "0", "I_p", "0", 0, 1e6, Model) < 0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "\\alpha", "0", "I_O", "I_n", "0", 0, 1e6, Model) < 0)
				PERFORM_ERROR;


			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================COA

		//==================MCFOA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("X");
			BoundaryNodes.push_back("Y");
			BoundaryNodes.push_back("Z");
			BoundaryNodes.push_back("W");
			long Model=0;
			if((Model=Manager.CreateNewSubcircuit(hCircuit,"MCFOA",BoundaryNodes,NULL,true))<0)
				PERFORM_ERROR;

			if (Manager.AddCurrentGain(hCircuit, "\\alpha", "Z", "0", "X", "5", 0, 1, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddCurrentGain(hCircuit, "\\beta", "Y", "0", "W", "6", 0, 1, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "\\gamma", "5", "0", "Y", "0", 0, 1, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddVoltageGain(hCircuit, "\\delta", "6", "0", "Z", "0", 0, 1, Model)<0)
				PERFORM_ERROR;

			if(Manager.ForceModelCalculation(hCircuit,Model,true)<0)
				PERFORM_ERROR;
		}
		//==================MCFOA
		//==================FDCFOA
		{
			BoundaryNodes.clear();
			BoundaryNodes.push_back("V_X_p");
			BoundaryNodes.push_back("V_X_n");
			BoundaryNodes.push_back("V_Y_p");
			BoundaryNodes.push_back("V_Y_n");
			BoundaryNodes.push_back("I_Z_p");
			BoundaryNodes.push_back("I_Z_n");
			BoundaryNodes.push_back("V_W_p");
			BoundaryNodes.push_back("V_W_n");
			long Model = 0;
			if ((Model = Manager.CreateNewSubcircuit(hCircuit, "FDCFOA11", BoundaryNodes, NULL, true))<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "3", "1", "1", "V_Y_p", true, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "3", "2", "2", "V_Y_n", false, false, Model)<0)
				PERFORM_ERROR;

			if (Manager.AddPathologicalPair(hCircuit, "I_Z_p", "V_X_p", "V_X_p", "3", true, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "I_Z_n", "V_X_n", "V_X_n", "3", true, true, Model)<0)
				PERFORM_ERROR;

			if (Manager.AddPathologicalPair(hCircuit, "0", "V_W_p", "V_W_p", "I_Z_p", false, false, Model)<0)
				PERFORM_ERROR;
			if (Manager.AddPathologicalPair(hCircuit, "0", "V_W_n", "V_W_n", "I_Z_n", false, false, Model)<0)
				PERFORM_ERROR;

			if (Manager.AddResistor(hCircuit, "g_m_1", "I_Z_p", "0", 1, Model, true)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "g_m_2", "I_Z_n", "0", 1, Model, true)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "1", "0", 1, Model, true)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "2", "0", 1, Model, true)<0)
				PERFORM_ERROR;
			if (Manager.AddResistor(hCircuit, "1", "3", "0", 1, Model, true)<0)
				PERFORM_ERROR;

			if (Manager.ForceModelCalculation(hCircuit, Model, true)<0)
				PERFORM_ERROR;
		}
		//==================FDCFOA
		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0)
			PERFORM_ERROR;

	}
	catch(exception& e)
	{
		cout<<"Uncatched exception: "<<e.what();
	}
end_of:
#if _DEBUG
	_CrtMemCheckpoint(&cms2);
	if(_CrtMemDifference(&cms3,&cms1,&cms2))
		_CrtMemDumpStatistics(&cms3);
#endif
	return 0;
}


