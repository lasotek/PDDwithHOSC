#include "stdafx.h"
#include "PrjException.h"


const char* ErrorMessage(const char* Comment, const char* Filename, int LineNumber)
{
	static string str;
	string str3;
	char str2[10];
	sprintf_s(str2,sizeof(str2),"%i",LineNumber);
	str="File: ";
	str3=Filename;
	unsigned long pos = (unsigned long)str3.find_last_of("\\");
	str3.erase(0,pos+1);
	str+=str3;
	str+=", line: ";
	str+=str2;
	str+=" -- ";
	str+=Comment;
	return str.c_str();
}