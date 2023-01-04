// UA741.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define PERFORM_ERROR 		{\
			string str;\
												while((str=(string)LastErrorInfo)!="")\
				printf("%s\n",str.c_str());\
			goto end_of;\
																}

#define ACC 0.01

//#define RESISTANCES

#define HIGH 18E3
#define LOW 16

#define CONDUCTANCE true
//#define CONDUCTANCE false
#define EXACT false

#define WRITE_1_LEVEL false

#define WITHOUT_POSTPROCESSING 


int _tmain(int argc, _TCHAR* argv[])
{
#if _DEBUG
	_CrtMemState cms1, cms2, cms3;
	_CrtMemCheckpoint(&cms1);
#endif
	try
	{
		CPDDEngineManager Manager;
		long hEngine = Manager.GetPDDEngine(0, true, false, true);
		long long hCircuit = Manager.GetNewCircuit(hEngine);
		Manager.SetGlobalModelFileName(hCircuit, "Models.txt", true);
		Manager.SetGlobalRaportFileName(hCircuit, "Models.txt");
		//Manager.DeclareGlobalRefNode(hCircuit, "0");


		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_1" : "R_1", "9", "0", CONDUCTANCE ? 1.0L / 1 : 1, CONDUCTANCE)<0)
			PERFORM_ERROR; //r1	 9 	 0 			 1
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_2" : "R_2", "7", "0", CONDUCTANCE ? 1.0L / 50 : 50, CONDUCTANCE)<0)
			PERFORM_ERROR; //r2	 7 	 0 			 50
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_3" : "R_3", "10", "0", CONDUCTANCE ? 1.0L / 1 : 1, CONDUCTANCE)<0)
			PERFORM_ERROR; //r3	 10 	 0 			 1
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_4" : "R_4", "13", "0", CONDUCTANCE ? 1.0L / 3 : 3, CONDUCTANCE)<0)
			PERFORM_ERROR; //r4	 13 	 0 			 3
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_5" : "R_5", "11", "23", CONDUCTANCE ? 1.0L / 39 : 39, CONDUCTANCE)<0)
			PERFORM_ERROR; //r5	 11 	 23 			 39
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_6" : "R_6", "20", "21", CONDUCTANCE ? 1.0L / 0.027 : 0.027, CONDUCTANCE)<0)
			PERFORM_ERROR; //r6	 20 	 21 			 0.027
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_7" : "R_7", "21", "22", CONDUCTANCE ? 1.0L / 0.022 : 0.022, CONDUCTANCE)<0)
			PERFORM_ERROR;//r7	 21 	 22 			 0.022
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_8" : "R_8", "12", "0", CONDUCTANCE ? 1.0L / 0.1 : 0.1, CONDUCTANCE)<0)
			PERFORM_ERROR;//r8	 12 	 0 			 0.1
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_9" : "R_9", "15", "0", CONDUCTANCE ? 1.0L / 50 : 50, CONDUCTANCE)<0)
			PERFORM_ERROR;//r9	 15 	 0 			 50
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_10" : "R_10", "18", "19", CONDUCTANCE ? 1.0L / 40 : 40, CONDUCTANCE)<0)
			PERFORM_ERROR;//r10	 18 	 19 			 40
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_11" : "R_11", "14", "0", CONDUCTANCE ? 1.0L / 50 : 50, CONDUCTANCE)<0)
			PERFORM_ERROR;//r11	 14 	 0 			 50
		if (Manager.AddCapacitor(hCircuit, "C_f", "8", "16", 15)<0)
			PERFORM_ERROR;//cf	 8 	 16 			 15

		if (Manager.AddTransconductance(hCircuit, "gm_1", "25", "3", "24", "3", 0, 0.489)<0)
			PERFORM_ERROR; //gm1	 25 	 3 	 24 	 3 	 0.489
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_1" : "R_p_1", "24", "3", CONDUCTANCE ? 1.0L / 455 : 455, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp1	 24 	 3 			 455
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_1" : "R_x_1", "24", "1", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx1	 1 	 24 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_1" : "R_o_1", "25", "3", CONDUCTANCE ? 1.0L / 15100 : 15100, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro1	 25 	 3 			 15100
		if (Manager.AddCapacitor(hCircuit, "C_p_1", "24", "3", 1.65)<0)
			PERFORM_ERROR;//cp1	 24 	 3 			 1.65
		if (Manager.AddCapacitor(hCircuit, "C_u_1", "24", "25", 0.112)<0)
			PERFORM_ERROR;//cu1	 24 	 25 			 0.112
		if (Manager.AddCapacitor(hCircuit, "C_s_1", "0", "25", 1.42)<0)
			PERFORM_ERROR;//cs1	 25 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_1" : "R_c_1", "25", "2", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc1	 25 	 2 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_2", "27", "4", "26", "4", 0, 0.488)<0)
			PERFORM_ERROR; //gm2	 27 	 4 	 26 	 4 	 0.488
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_2" : "R_p_2", "26", "4", CONDUCTANCE ? 1.0L / 456 : 456, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp2	 26 	 4 			 456
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_2" : "R_x_2", "26", "1a", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx2	 0 	 26 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_2" : "R_o_2", "27", "4", CONDUCTANCE ? 1.0L / 15200 : 15200, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro2	 27 	 4 			 15200
		if (Manager.AddCapacitor(hCircuit, "C_p_2", "26", "4", 1.65)<0)
			PERFORM_ERROR;//cp2	 26 	 4 			 1.65
		if (Manager.AddCapacitor(hCircuit, "C_u_2", "26", "27", 0.112)<0)
			PERFORM_ERROR;//cu2	 26 	 27 			 0.112
		if (Manager.AddCapacitor(hCircuit, "C_s_2", "0", "27", 1.42)<0)
			PERFORM_ERROR;//cs2	 27 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_2" : "R_c_2", "27", "2", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc2	 27 	 2 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_3", "29", "3", "28", "3", 0, 0.514)<0)
			PERFORM_ERROR; //gm3	 29 	 3 	 28 	 3 	 0.514
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_3" : "R_p_3", "28", "3", CONDUCTANCE ? 1.0L / 165 : 165, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp3	 28 	 3 			 165
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_3" : "R_x_3", "28", "5", CONDUCTANCE ? 1.0L / 0.5 : 0.5, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx3	 5 	 28 			 0.5
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_3" : "R_o_3", "29", "3", CONDUCTANCE ? 1.0L / 4890 : 4890, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro3	 29 	 3 			 4890
		if (Manager.AddCapacitor(hCircuit, "C_p_3", "28", "3", 14.3)<0)
			PERFORM_ERROR;//cp3	 28 	 3 			 14.3
		if (Manager.AddCapacitor(hCircuit, "C_u_3", "28", "29", 0.342)<0)
			PERFORM_ERROR;//cu3	 28 	 29 			 0.342
		if (Manager.AddCapacitor(hCircuit, "C_s_3", "0", "5", 2.26)<0)
			PERFORM_ERROR;//cs3	 5 	 0 			 2.26
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_3" : "R_c_3", "29", "6", CONDUCTANCE ? 1.0L / 0.15 : 0.15, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc3	 29 	 6 			 0.15

		if (Manager.AddTransconductance(hCircuit, "gm_4", "31", "4", "30", "4", 0, 0.47)<0)
			PERFORM_ERROR; //gm4	 31 	 4 	 30 	 4 	 0.47
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_4" : "R_p_4", "30", "4", CONDUCTANCE ? 1.0L / 181 : 181, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp4	 30 	 4 			 181
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_4" : "R_x_4", "30", "5", CONDUCTANCE ? 1.0L / 0.5 : 0.5, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx4	 5 	 30 			 0.5
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_4" : "R_o_4", "31", "4", CONDUCTANCE ? 1.0L / 5340 : 5340, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro4	 31 	 4 			 5340
		if (Manager.AddCapacitor(hCircuit, "C_p_4", "30", "4", 13.1)<0)
			PERFORM_ERROR;//cp4	 30 	 4 			 13.1
		if (Manager.AddCapacitor(hCircuit, "C_u_4", "30", "31", 0.345)<0)
			PERFORM_ERROR;//cu4	 30 	 31 			 0.345
		if (Manager.AddCapacitor(hCircuit, "C_s_4", "0", "5", 2.26)<0)
			PERFORM_ERROR;//cs4	 5 	 0 			 2.26
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_4" : "R_c_4", "31", "8", CONDUCTANCE ? 1.0L / 0.15 : 0.15, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc4	 31 	 8 			 0.15

		if (Manager.AddTransconductance(hCircuit, "gm_5", "33", "9", "32", "9", 0, 0.484)<0)
			PERFORM_ERROR; //gm5	 33 	 9 	 32 	 9 	 0.484
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_5" : "R_p_5", "32", "9", CONDUCTANCE ? 1.0L / 427 : 427, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp5	 32 	 9 			 427
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_5" : "R_x_5", "32", "7", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx5	 7 	 32 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_5" : "R_o_5", "33", "9", CONDUCTANCE ? 1.0L / 14200 : 14200, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro5	 33 	 9 			 14200
		if (Manager.AddCapacitor(hCircuit, "C_p_5", "32", "9", 1.64)<0)
			PERFORM_ERROR;//cp5	 32 	 9 			 1.64
		if (Manager.AddCapacitor(hCircuit, "C_u_5", "32", "33", 0.273)<0)
			PERFORM_ERROR;//cu5	 32 	 33 			 0.273
		if (Manager.AddCapacitor(hCircuit, "C_s_5", "0", "33", 1.42)<0)
			PERFORM_ERROR;//cs5	 33 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_5" : "R_c_5", "33", "6", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc5	 33 	 6 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_6", "35", "10", "34", "10", 0, 0.484)<0)
			PERFORM_ERROR; //gm6	 35 	 10 	 34 	 10 	 0.484
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_6" : "R_p_6", "34", "10", CONDUCTANCE ? 1.0L / 427 : 427, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp6	 34 	 10 			 427
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_6" : "R_x_6", "34", "7", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx6	 7 	 34 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_6" : "R_o_6", "35", "10", CONDUCTANCE ? 1.0L / 14200 : 14200, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro6	 35 	 10 			 14200
		if (Manager.AddCapacitor(hCircuit, "C_p_6", "34", "10", 1.64)<0)
			PERFORM_ERROR;//cp6	 34 	 10 			 1.64
		if (Manager.AddCapacitor(hCircuit, "C_u_6", "34", "35", 0.247)<0)
			PERFORM_ERROR;//cu6	 34 	 35 			 0.247
		if (Manager.AddCapacitor(hCircuit, "C_s_6", "0", "35", 1.42)<0)
			PERFORM_ERROR;//cs6	 35 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_6" : "R_c_6", "35", "8", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc6	 35 	 8 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_7", "37", "7", "36", "7", 0, 0.47)<0)
			PERFORM_ERROR;//gm7	 37 	 7 	 36 	 7 	 0.47
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_7" : "R_p_7", "36", "7", CONDUCTANCE ? 1.0L / 510 : 510, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp7	 36 	 7 			 510
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_7" : "R_x_7", "36", "6", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx7	 6 	 36 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_7" : "R_o_7", "37", "7", CONDUCTANCE ? 1.0L / 16900 : 16900, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro7	 37 	 7 			 16900
		if (Manager.AddCapacitor(hCircuit, "C_p_7", "36", "7", 1.62)<0)
			PERFORM_ERROR;//cp7	 36 	 7 			 1.62
		if (Manager.AddCapacitor(hCircuit, "C_u_7", "36", "37", 0.09)<0)
			PERFORM_ERROR;//cu7	 36 	 37 			 0.09
		if (Manager.AddCapacitor(hCircuit, "C_s_7", "0", "37", 1.42)<0)
			PERFORM_ERROR;//cs7	 37 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_7" : "R_c_7", "37", "0", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc7	 37 	 0 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_8", "39", "0", "38", "0", 0, 0.882)<0)
			PERFORM_ERROR;//gm8	 39 	 0 	 38 	 0 	 0.882
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_8" : "R_p_8", "38", "0", CONDUCTANCE ? 1.0L / 71.9 : 71.9, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp8	 38 	 0 			 71.9
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_8" : "R_x_8", "38", "2", CONDUCTANCE ? 1.0L / 0.5 : 0.5, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx8	 2 	 38 			 0.5
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_8" : "R_o_8", "39", "0", CONDUCTANCE ? 1.0L / 2230 : 2230, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro8	 39 	 0 			 2230
		if (Manager.AddCapacitor(hCircuit, "C_p_8", "38", "0", 24.4)<0)
			PERFORM_ERROR;//cp8	 38 	 0 			 24.4
		if (Manager.AddCapacitor(hCircuit, "C_u_8", "38", "39", 1.05)<0)
			PERFORM_ERROR;//cu8	 38 	 39 			 1.05
		if (Manager.AddCapacitor(hCircuit, "C_s_8", "0", "2", 2.26)<0)
			PERFORM_ERROR;//cs8	 2 	 0 			 2.26
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_8" : "R_c_8", "39", "2", CONDUCTANCE ? 1.0L / 0.15 : 0.15, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc8	 39 	 2 			 0.15

		if (Manager.AddTransconductance(hCircuit, "gm_9", "41", "0", "40", "0", 0, 1.13)<0)
			PERFORM_ERROR;//gm9	 41 	 0 	 40 	 0 	 1.13
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_9" : "R_p_9", "40", "0", CONDUCTANCE ? 1.0L / 71.9 : 71.9, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp9	 40 	 0 			 71.9
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_9" : "R_x_9", "40", "2", CONDUCTANCE ? 1.0L / 0.5 : 0.5, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx9	 2 	 40 			 0.5
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_9" : "R_o_9", "41", "0", CONDUCTANCE ? 1.0L / 2230 : 2230, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro9	 41 	 0 			 2230
		if (Manager.AddCapacitor(hCircuit, "C_p_9", "40", "0", 31.2)<0)
			PERFORM_ERROR;//cp9	 40 	 0 			 31.2
		if (Manager.AddCapacitor(hCircuit, "C_u_9", "40", "41", 0.32)<0)
			PERFORM_ERROR;//cu9	 40 	 41 			 0.32
		if (Manager.AddCapacitor(hCircuit, "C_s_9", "0", "2", 2.26)<0)
			PERFORM_ERROR;//cs9	 2 	 0 			 2.26
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_9" : "R_c_9", "41", "5", CONDUCTANCE ? 1.0L / 0.15 : 0.15, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc9	 41 	 5 			 0.15

		if (Manager.AddTransconductance(hCircuit, "gm_10", "43", "13", "42", "13", 0, 1.2)<0)
			PERFORM_ERROR;//gm10	 43 	 13 	 42 	 13 	 1.2
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_10" : "R_p_10", "42", "13", CONDUCTANCE ? 1.0L / 181 : 181, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp10	 42 	 13 			 181
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_10" : "R_x_10", "23", "42", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx10	 23 	 42 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_10" : "R_o_10", "43", "13", CONDUCTANCE ? 1.0L / 6090 : 6090, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro10	 43 	 13 			 6090
		if (Manager.AddCapacitor(hCircuit, "C_p_10", "42", "13", 2.49)<0)
			PERFORM_ERROR;//cp10	 42 	 13 			 2.49
		if (Manager.AddCapacitor(hCircuit, "C_u_10", "42", "43", 0.112)<0)
			PERFORM_ERROR;//cu10	 42 	 43 			 0.112
		if (Manager.AddCapacitor(hCircuit, "C_s_10", "0", "43", 1.42)<0)
			PERFORM_ERROR;//cs10	 43 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_10" : "R_c_10", "43", "5", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc10	 43 	 5 			 0.3


		if (Manager.AddTransconductance(hCircuit, "gm_11", "45", "0", "44", "0", 0, 21.5)<0)
			PERFORM_ERROR;//gm11	 45 	 0 	 44 	 0 	 21.5
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_11" : "R_p_11", "44", "0", CONDUCTANCE ? 1.0L / 5.11 : 5.11, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp11	 44 	 0 			 5.11
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_11" : "R_x_11", "23", "44", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx11	 23 	 44 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_11" : "R_o_11", "45", "0", CONDUCTANCE ? 1.0L / 245 : 245, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro11	 45 	 0 			 245
		if (Manager.AddCapacitor(hCircuit, "C_p_11", "44", "0", 25.9)<0)
			PERFORM_ERROR;//cp11	 44 	 0 			 25.9
		if (Manager.AddCapacitor(hCircuit, "C_u_11", "44", "45", 0.447)<0)
			PERFORM_ERROR;//cu11	 44 	 45 			 0.447
		if (Manager.AddCapacitor(hCircuit, "C_s_11", "0", "45", 1.42)<0)
			PERFORM_ERROR;//cs11	 45 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_11" : "R_c_11", "45", "23", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc11	 45 	 23 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_12", "47", "0", "46", "0", 0, 15.3)<0)
			PERFORM_ERROR;//gm12	 47 	 0 	 46 	 0 	 15.3
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_12" : "R_p_12", "46", "0", CONDUCTANCE ? 1.0L / 0.813 : 0.813, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp12	 46 	 0 			 0.813
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_12" : "R_x_12", "11", "46", CONDUCTANCE ? 1.0L / 0.5 : 0.5, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx12	 11 	 46 			 0.5
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_12" : "R_o_12", "47", "0", CONDUCTANCE ? 1.0L / 81.2 : 81.2, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro12	 47 	 0 			 81.2
		if (Manager.AddCapacitor(hCircuit, "C_p_12", "46", "0", 419)<0)
			PERFORM_ERROR;//cp12	 46 	 0 			 419
		if (Manager.AddCapacitor(hCircuit, "C_u_12", "46", "47", 1.13)<0)
			PERFORM_ERROR;//cu12	 46 	 47 			 1.13
		if (Manager.AddCapacitor(hCircuit, "C_s_12", "0", "11", 2.26)<0)
			PERFORM_ERROR;//cs12	 11 	 0 			 2.26
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_12" : "R_c_12", "47", "11", CONDUCTANCE ? 1.0L / 0.15 : 0.15, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc12	 47 	 11 			 0.15

		if (Manager.AddTransconductance(hCircuit, "gm_13a", "49", "0", "48", "0", 0, 3.65)<0)
			PERFORM_ERROR;//gm13a	 49 	 0 	 48 	 0 	 3.65
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_13a" : "R_p_13a", "48", "0", CONDUCTANCE ? 1.0L / 3.78 : 3.78, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp13a	 48 	 0 			 3.78
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_13a" : "R_x_13a", "11", "48", CONDUCTANCE ? 1.0L / 0.1 : 0.1, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx13a	 11 	 48 			 0.1
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_13a" : "R_o_13a", "49", "0", CONDUCTANCE ? 1.0L / 22.9 : 22.9, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro13a	 49 	 0 			 22.9
		if (Manager.AddCapacitor(hCircuit, "C_p_13a", "48", "0", 100)<0)
			PERFORM_ERROR;//cp13a	 48 	 0 			 100
		if (Manager.AddCapacitor(hCircuit, "C_u_13a", "48", "49", 0.3)<0)
			PERFORM_ERROR;//cu13a	 48 	 49 			 0.3
		if (Manager.AddCapacitor(hCircuit, "C_s_13a", "0", "49", 2.26)<0)
			PERFORM_ERROR;//cs13a	 49 	 0 			 2.26
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_13a" : "R_c_13a", "49", "17", CONDUCTANCE ? 1.0L / 0.08 : 0.08, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc13a	 49 	 17 			 0.08

		if (Manager.AddTransconductance(hCircuit, "gm_13b", "51", "0", "50", "0", 0, 9.11)<0)
			PERFORM_ERROR;//gm13b	 51 	 0 	 50 	 0 	 9.11
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_13b" : "R_p_13b", "50", "0", CONDUCTANCE ? 1.0L / 1.62 : 1.62, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp13b	 50 	 0 			 1.62
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_13b" : "R_x_13b", "11", "50", CONDUCTANCE ? 1.0L / 0.16 : 0.16, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx13b	 11 	 50 			 0.16
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_13b" : "R_o_13b", "51", "0", CONDUCTANCE ? 1.0L / 9.17 : 9.17, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro13b	 51 	 0 			 9.17
		if (Manager.AddCapacitor(hCircuit, "C_p_13b", "50", "0", 250)<0)
			PERFORM_ERROR;//cp13b	 50 	 0 			 250
		if (Manager.AddCapacitor(hCircuit, "C_u_13b", "50", "51", 1)<0)
			PERFORM_ERROR;//cu13b	 50 	 51 			 1
		if (Manager.AddCapacitor(hCircuit, "C_s_13b", "0", "51", 2.26)<0)
			PERFORM_ERROR;//cs13b	 51 	 0 			 2.26
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_13b" : "R_c_13b", "51", "16", CONDUCTANCE ? 1.0L / 0.12 : 0.12, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc13b	 51 	 16 			 0.12

		if (Manager.AddTransconductance(hCircuit, "gm_14", "53", "20", "52", "20", 0, 6.07)<0)
			PERFORM_ERROR;//gm14	 53 	 20 	 52 	 20 	 6.07
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_14" : "R_p_14", "52", "20", CONDUCTANCE ? 1.0L / 67.4 : 67.4, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp14	 52 	 20 			 67.4
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_14" : "R_x_14", "17", "52", CONDUCTANCE ? 1.0L / 0.185 : 0.185, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx14	 17 	 52 			 0.185
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_14" : "R_o_14", "53", "20", CONDUCTANCE ? 1.0L / 1770 : 1770, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro14	 53 	 20 			 1770
		if (Manager.AddCapacitor(hCircuit, "C_p_14", "52", "20", 9.67)<0)
			PERFORM_ERROR;//cp14	 52 	 20 			 9.67
		if (Manager.AddCapacitor(hCircuit, "C_u_14", "52", "53", 0.485)<0)
			PERFORM_ERROR;//cu14	 52 	 53 			 0.485
		if (Manager.AddCapacitor(hCircuit, "C_s_14", "0", "53", 3.46)<0)
			PERFORM_ERROR;//cs14	 53 	 0 			 3.46
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_14" : "R_c_14", "53", "0", CONDUCTANCE ? 1.0L / 0.015 : 0.015, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc14	 53 	 0 			 0.015

		if (Manager.AddTransconductance(hCircuit, "gm_15", "55", "21", "54", "21", 0, 0.00000000005)<0)
			PERFORM_ERROR;//gm15	 55 	 21 	 54 	 21 	 0.00000000005
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_15" : "R_p_15", "54", "21", CONDUCTANCE ? 1.0L / 200000000000 : 200000000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp15	 54 	 21 			 200000000000
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_15" : "R_x_15", "20", "54", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx15	 20 	 54 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_15" : "R_o_15", "55", "21", CONDUCTANCE ? 1.0L / 30000000000 : 30000000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro15	 55 	 21 			 30000000000
		if (Manager.AddCapacitor(hCircuit, "C_p_15", "54", "21", 0.65)<0)
			PERFORM_ERROR;//cp15	 54 	 21 			 0.65
		if (Manager.AddCapacitor(hCircuit, "C_u_15", "54", "55", 0.36)<0)
			PERFORM_ERROR;//cu15	 54 	 55 			 0.36
		if (Manager.AddCapacitor(hCircuit, "C_s_15", "0", "55", 1.42)<0)
			PERFORM_ERROR;//cs15	 55 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_15" : "R_c_15", "55", "17", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc15	 55 	 17 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_16", "57", "15", "56", "15", 0, 0.571)<0)
			PERFORM_ERROR;//gm16	 57 	 15 	 56 	 15 	 0.571
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_16" : "R_p_16", "56", "15", CONDUCTANCE ? 1.0L / 418 : 418, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp16	 56 	 15 			 418
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_16" : "R_x_16", "8", "56", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx16	 8 	 56 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_16" : "R_o_16", "57", "15", CONDUCTANCE ? 1.0L / 13900 : 13900, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro16	 57 	 15 			 13900
		if (Manager.AddCapacitor(hCircuit, "C_p_16", "56", "15", 1.74)<0)
			PERFORM_ERROR;//cp16	 56 	 15 			 1.74
		if (Manager.AddCapacitor(hCircuit, "C_u_16", "56", "57", 0.09)<0)
			PERFORM_ERROR;//cu16	 56 	 57 			 0.09 
		if (Manager.AddCapacitor(hCircuit, "C_s_16", "0", "57", 1.42)<0)
			PERFORM_ERROR;//cs16	 57 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_16" : "R_c_16", "57", "0", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc16	 57 	 0 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_17", "59", "12", "58", "12", 0, 8.04)<0)
			PERFORM_ERROR;//gm17	 59 	 12 	 58 	 12 	 8.04
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_17" : "R_p_17", "58", "12", CONDUCTANCE ? 1.0L / 24.2 : 24.2, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp17	 58 	 12 			 24.2
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_17" : "R_x_17", "15", "58", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx17	 15 	 58 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_17" : "R_o_17", "59", "12", CONDUCTANCE ? 1.0L / 90 : 90, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro17	 59 	 12 			 90
		if (Manager.AddCapacitor(hCircuit, "C_p_17", "58", "12", 10.4)<0)
			PERFORM_ERROR;//cp17	 58 	 12 			 10.4
		if (Manager.AddCapacitor(hCircuit, "C_u_17", "58", "59", 0.09)<0)
			PERFORM_ERROR;//cu17	 58 	 59 			 0.09
		if (Manager.AddCapacitor(hCircuit, "C_s_17", "0", "59", 1.42)<0)
			PERFORM_ERROR;//cs17	 59 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_17" : "R_c_17", "59", "16", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc17	 59 	 16 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_18", "61", "19", "60", "19", 0, 2.89)<0)
			PERFORM_ERROR;//gm18	 61 	 19 	 60 	 19 	 2.89
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_18" : "R_p_18", "60", "19", CONDUCTANCE ? 1.0L / 66 : 66, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp18	 60 	 19 			 66
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_18" : "R_x_18", "18", "60", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx18	 18 	 60 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_18" : "R_o_18", "61", "19", CONDUCTANCE ? 1.0L / 2290 : 2290, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro18	 61 	 19 			 2290
		if (Manager.AddCapacitor(hCircuit, "C_p_18", "60", "19", 4.46)<0)
			PERFORM_ERROR;//cp18	 60 	 19 			 4.46
		if (Manager.AddCapacitor(hCircuit, "C_u_18", "60", "61", 0.273)<0)
			PERFORM_ERROR;//cu18	 60 	 61 			 0.273
		if (Manager.AddCapacitor(hCircuit, "C_s_18", "0", "61", 1.42)<0)
			PERFORM_ERROR;//cs18	 61 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_18" : "R_c_18", "61", "17", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc18	 61 	 17 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_19", "63", "18", "62", "18", 0, 0.628)<0)
			PERFORM_ERROR;//gm19	 63 	 18 	 62 	 18 	 0.628
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_19" : "R_p_19", "62", "18", CONDUCTANCE ? 1.0L / 326 : 326, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp19	 62 	 18 			 326
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_19" : "R_x_19", "17", "62", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx19	 17 	 62 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_19" : "R_o_19", "63", "18", CONDUCTANCE ? 1.0L / 1090 : 1090, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro19	 63 	 18 			 1090
		if (Manager.AddCapacitor(hCircuit, "C_p_19", "62", "18", 1.4)<0)
			PERFORM_ERROR;//cp19	 62 	 18 			 1.4
		if (Manager.AddCapacitor(hCircuit, "C_u_19", "62", "63", 0.36)<0)
			PERFORM_ERROR;//cu19	 62 	 63 			 0.36
		if (Manager.AddCapacitor(hCircuit, "C_s_19", "0", "63", 1.42)<0)
			PERFORM_ERROR;//cs19	 63 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_19" : "R_c_19", "63", "17", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc19	 63 	 17 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_20", "65", "22", "64", "22", 0, 5.2)<0)
			PERFORM_ERROR;//gm20	 65 	 22 	 64 	 22 	 5.2
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_20" : "R_p_20", "64", "22", CONDUCTANCE ? 1.0L / 19.6 : 19.6, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp20	 64 	 22 			 19.6
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_20" : "R_x_20", "19", "64", CONDUCTANCE ? 1.0L / 0.08 : 0.08, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx20	 19 	 64 			 0.08
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_20" : "R_o_20", "65", "22", CONDUCTANCE ? 1.0L / 457 : 457, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro20	 65 	 22 			 457
		if (Manager.AddCapacitor(hCircuit, "C_p_20", "64", "22", 144)<0)
			PERFORM_ERROR;//cp20	 64 	 22 			 144
		if (Manager.AddCapacitor(hCircuit, "C_u_20", "64", "65", 1.25)<0)
			PERFORM_ERROR;//cu20	 64 	 65 			 1.25
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_20" : "R_c_20", "65", "0", CONDUCTANCE ? 1.0L / 0.156 : 0.156, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc20	 65 	 0 			 0.156

		if (Manager.AddTransconductance(hCircuit, "gm_21", "67", "21", "66", "21", 0, 0.00000000008)<0)
			PERFORM_ERROR;//gm21	 67 	 21 	 66 	 21 	 0.00000000008
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_21" : "R_p_21", "66", "21", CONDUCTANCE ? 1.0L / 66000000000 : 66000000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp21	 66 	 21 			 66000000000
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_21" : "R_x_21", "22", "66", CONDUCTANCE ? 1.0L / 0.5 : 0.5, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx21	 22 	 66 			 0.5
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_21" : "R_o_21", "67", "21", CONDUCTANCE ? 1.0L / 670000000 : 670000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro21	 67 	 21 			 670000000
		if (Manager.AddCapacitor(hCircuit, "C_p_21", "66", "21", 1.05)<0)
			PERFORM_ERROR;//cp21	 66 	 21 			 1.05
		if (Manager.AddCapacitor(hCircuit, "C_u_21", "66", "67", 0.1)<0)
			PERFORM_ERROR;//cu21	 66 	 67 			 0.1
		if (Manager.AddCapacitor(hCircuit, "C_s_21", "0", "22", 2.26)<0)
			PERFORM_ERROR;//cs21	 22 	 0 			 2.26
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_21" : "R_c_21", "67", "14", CONDUCTANCE ? 1.0L / 0.15 : 0.15, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc21	 67 	 14 			 0.15

		if (Manager.AddTransconductance(hCircuit, "gm_22", "69", "0", "68", "0", 0, 0.000000000000002)<0)
			PERFORM_ERROR;//gm22	 69 	 0 	 68 	 0 	 0.000000000000002
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_22" : "R_p_22", "68", "0", CONDUCTANCE ? 1.0L / 200000000000 : 200000000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp22	 68 	 0 			 200000000000
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_22" : "R_x_22", "14", "68", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx22	 14 	 68 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_22" : "R_o_22", "69", "0", CONDUCTANCE ? 1.0L / 950000000 : 950000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro22	 69 	 0 			 950000000
		if (Manager.AddCapacitor(hCircuit, "C_p_22", "68", "0", 0.65)<0)
			PERFORM_ERROR;//cp22	 68 	 0 			 0.65
		if (Manager.AddCapacitor(hCircuit, "C_u_22", "68", "69", 0.3)<0)
			PERFORM_ERROR;//cu22	 68 	 69 			 0.3
		if (Manager.AddCapacitor(hCircuit, "C_s_22", "0", "69", 1.42)<0)
			PERFORM_ERROR;//cs22	 69 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_22" : "R_c_22", "69", "8", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc22	 69 	 8 			 0.3

		if (Manager.AddTransconductance(hCircuit, "gm_23a", "71", "19", "70", "19", 0,2.41)<0)
			PERFORM_ERROR;//gm23a	 71 	 19 	 70 	 19 	 2.41
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_23a" : "R_p_23a", "70", "19", CONDUCTANCE ? 1.0L / 131 : 131, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp23a	 70 	 19 			 131
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_23a" : "R_x_23a", "16", "70", CONDUCTANCE ? 1.0L / 1.1 : 1.1, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx23a	 16 	 70 			 1.1
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_23a" : "R_o_23a", "71", "19", CONDUCTANCE ? 1.0L / 998 : 998, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro23a	 71 	 19 			 998
		if (Manager.AddCapacitor(hCircuit, "C_p_23a", "70", "19", 65.7)<0)
			PERFORM_ERROR;//cp23a	 70 	 19 			 65.7
		if (Manager.AddCapacitor(hCircuit, "C_u_23a", "70", "71", 1.09)<0)
			PERFORM_ERROR;//cu23a	 70 	 71 			 1.09
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_23a" : "R_c_23a", "71", "0", CONDUCTANCE ? 1.0L / 0.17 : 0.17, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc23a	 71 	 0 			 0.17

		if (Manager.AddTransconductance(hCircuit, "gm_23b", "73", "8", "72", "8", 0, 0.000000000000002)<0)
			PERFORM_ERROR;//gm23b	 73 	 8 	 72 	 8 	 0.000000000000002
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_23b" : "R_p_23b", "72", "8", CONDUCTANCE ? 1.0L / 200000000000 : 200000000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp23b	 72 	 8 			 200000000000
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_23b" : "R_x_23b", "16", "72", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx23b	 16 	 72 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_23b" : "R_o_23b", "73", "8", CONDUCTANCE ? 1.0L / 950000000 : 950000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro23b	 73 	 8 			 950000000
		if (Manager.AddCapacitor(hCircuit, "C_p_23b", "72", "8", 1.9)<0)
			PERFORM_ERROR;//cp23b	 72 	 8 			 1.9
		if (Manager.AddCapacitor(hCircuit, "C_u_23b", "72", "73", 2.4)<0)
			PERFORM_ERROR;//cu23b	 72 	 73 			 2.4
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_23b" : "R_c_23b", "73", "0", CONDUCTANCE ? 1.0L / 0.1 : 0.1, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc23b	 73 	 0 			 0.1

		if (Manager.AddTransconductance(hCircuit, "gm_24", "75", "0", "74", "0", 0, 0.000000000000002)<0)
			PERFORM_ERROR;//gm24	 75 	 0 	 74 	 0 	 0.000000000000002
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_p_24" : "R_p_24", "74", "0", CONDUCTANCE ? 1.0L / 200000000000 : 200000000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//rp24	 74 	 0 			 200000000000
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_x_24" : "R_x_24", "14", "74", CONDUCTANCE ? 1.0L / 0.67 : 0.67, CONDUCTANCE)<0)
			PERFORM_ERROR;//rx24	 14 	 74 			 0.67
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_o_24" : "R_o_24", "75", "0", CONDUCTANCE ? 1.0L / 950000000 : 950000000, CONDUCTANCE)<0)
			PERFORM_ERROR;//ro24	 75 	 0 			 950000000
		if (Manager.AddCapacitor(hCircuit, "C_p_24", "74", "0", 0.65)<0)
			PERFORM_ERROR;//cp24	 74 	 0 			 0.65
		if (Manager.AddCapacitor(hCircuit, "C_u_24", "74", "75", 0.36)<0)
			PERFORM_ERROR;//cu24	 74 	 75 			 0.36
		if (Manager.AddCapacitor(hCircuit, "C_s_24", "0", "75", 1.42)<0)
			PERFORM_ERROR;//cs24	 75 	 0 			 1.42
		if (Manager.AddResistor(hCircuit, CONDUCTANCE ? "G_c_24" : "R_c_24", "75", "14", CONDUCTANCE ? 1.0L / 0.3 : 0.3, CONDUCTANCE)<0)
			PERFORM_ERROR;//rc24	 75 	 14 			 0.3

//inp	 1 	 0
//out	 21 	 0


		long long hCofactor_ab = Manager.CreateNewCofactor(hCircuit);
		if (hCofactor_ab<0)
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_ab, "1", "1a", "21", "0"))
			PERFORM_ERROR;

		long long hCofactor_aa = Manager.CreateNewCofactor(hCircuit);
		if (hCofactor_aa<0)
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_aa, "1", "1a", "1", "1a"))
			PERFORM_ERROR;

		long long Ku = Manager.DefineTransferFunctionSimp("Ku_s", hCofactor_ab, hCofactor_aa);
		if (Ku<0)
			PERFORM_ERROR;

		long long hCofactor_abC = Manager.CreateNewCofactor(hCircuit);
		if (hCofactor_abC<0)
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_abC, "1", "0", "1", "1a"))
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_abC, "1a", "0", "21", "0"))
			PERFORM_ERROR;

		long long hCofactor_abR = Manager.CreateNewCofactor(hCircuit);
		if (hCofactor_abR<0)
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_abR, "1", "0", "21", "0"))
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_abR, "1a", "0", "1", "-1a"))
			PERFORM_ERROR;

		long long hCofactor_aaC = Manager.CreateNewCofactor(hCircuit);
		if (hCofactor_aaC<0)
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_aaC, "1", "0", "1", "0"))
			PERFORM_ERROR;
		if (Manager.AddPairOfDeletions(hCofactor_aaC, "1a", "0", "1a", "0"))
			PERFORM_ERROR;

		long long KuR = Manager.DefineTransferFunctionSimp("Ku_R", hCofactor_abR, hCofactor_aaC);
		if (KuR<0)
			PERFORM_ERROR;
		long long KuC = Manager.DefineTransferFunctionSimp("Ku_C", hCofactor_abC, hCofactor_aaC);
		if (KuC<0)
			PERFORM_ERROR;

		if (Manager.DoTheCalculation(hCircuit)<0)
			PERFORM_ERROR;

		if (Manager.WriteModelsToTextFile(hCircuit, "Models.txt")<0)
			PERFORM_ERROR;


		if (Manager.WriteDirectNmericalPolynomial(hCofactor_ab, EmptyString, "Models.txt", true, false, true)<0)
			PERFORM_ERROR;
		if (Manager.WriteDirectNmericalPolynomial(hCofactor_aa, EmptyString, "Models.txt", true, false, true)<0)
			PERFORM_ERROR;
		if (Manager.WriteDirectNmericalPolynomial(hCofactor_abC, EmptyString, "Models.txt", true, false, true)<0)
			PERFORM_ERROR;
		if (Manager.WriteDirectNmericalPolynomial(hCofactor_abR, EmptyString, "Models.txt", true, false, true)<0)
			PERFORM_ERROR;
		if (Manager.WriteDirectNmericalPolynomial(hCofactor_aaC, EmptyString, "Models.txt", true, false, true)<0)
			PERFORM_ERROR;

		if (Manager.RaportTransferFunctionValuesLog(hCircuit, Ku, EmptyString, -2, 10, 10, true))
			PERFORM_ERROR;
		if (Manager.RaportTransferFunctionValuesLog(hCircuit, KuR, EmptyString, -2, 10, 10, true))
			PERFORM_ERROR;
		if (Manager.RaportTransferFunctionValuesLog(hCircuit, KuC, EmptyString, -2, 10, 10, true))
			PERFORM_ERROR;

#ifndef WITHOUT_POSTPROCESSING
		long PT = 0;
		if ((PT = Manager.GetFreePathTraitor(hCircuit))<0)
			PERFORM_ERROR;
		if (Manager.SetApproximationCriterion(hCircuit, PT, EXACT, ACC, HIGH, LOW)<0)
			PERFORM_ERROR;
		if (Manager.WriteFlatSimplifiedPDD(hCircuit, PT, true, WRITE_1_LEVEL, "Models.txt")<0)
			PERFORM_ERROR;
		if (Manager.ReleasePathTraitor(hCircuit, PT)<0)
			PERFORM_ERROR;
#endif
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

