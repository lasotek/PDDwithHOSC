#pragma once
#include <fstream>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include <windows.h>

class time_stamp
{
public:
	time_stamp(const string& FileName, const string& Description);
	~time_stamp(void);
protected:
	clock_t m_Begining;
	LARGE_INTEGER m_StartTick;
	LARGE_INTEGER m_Frequency;
	string m_Descripton;
	string m_file_name;
};
