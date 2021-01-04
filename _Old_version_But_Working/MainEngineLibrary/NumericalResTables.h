#pragma once
#include <vector>
using namespace std;
using namespace stdext;

//typedef pair<bool,long double> _CNumericalSEntry;
//
//class _CNumericalSCoeff : public vector<_CNumericalSEntry> 
//{
//public:
//	bool IsSCoeffDefined(unsigned Index) {return Index>=size()?false:at(Index).first;}
//	bool GetSCoeff(unsigned Index, long double& Value);
//	void SetSCoeff(unsigned Index, const long double& Value);
//	void ClearIt();
//};
//
//class _CNumericalSCofactors : public vector<_CNumericalSCoeff>
//{
//public:
//	void Reset(size_type NewSize) {resize(0);resize(NewSize);}
//	void ClearIt();
//};
