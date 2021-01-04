#include "StdAfx.h"
#include "time_stamp.h"

time_stamp::time_stamp(const string& FileName, const string& Description):
m_file_name(FileName), m_Descripton(Description)
{
	m_Begining=clock();
	QueryPerformanceFrequency(&m_Frequency);
	QueryPerformanceCounter(&m_StartTick);
}

time_stamp::~time_stamp(void)
{
	LARGE_INTEGER m_EndTick;
	QueryPerformanceCounter(&m_EndTick);
	double interval=(double)(m_EndTick.QuadPart-m_StartTick.QuadPart)/m_Frequency.QuadPart;
	//clock_t m_End;
	//m_End=clock();
	//double interval=(double)(m_End-m_Begining)/CLOCKS_PER_SEC;
	fstream file(m_file_name.c_str(),ios::out | ios::app);
	file<<endl<<m_Descripton<<" - "<<interval<<" s"<<endl;
}
