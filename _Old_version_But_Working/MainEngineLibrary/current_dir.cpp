#include "StdAfx.h"
#include "current_dir.h"
#include <direct.h>
#include <TCHAR.H>
#ifdef _UNICODE
wstring current_dir()
{
	wstring Res;
	wchar_t* buffer;
	if((buffer=_wgetcwd(NULL,0))!=NULL)
	{
		Res.assign(buffer);
		free(buffer);
	}
	return Res;
}

#else
string current_dir()
{
	string Res;
	char* buffer;
	if((buffer=getcwd(NULL,0))!=NULL)
	{
		Res.assign(buffer);
		free(buffer);
	}
	return Res;
}
#endif