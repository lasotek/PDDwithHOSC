#pragma once
#include <string>
using namespace std;
#ifdef _UNICODE
wstring current_dir();
#else
string current_dir();
#endif