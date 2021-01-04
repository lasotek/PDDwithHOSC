#pragma once
#include "stdafx.h"
#include "NumericalResTables.h"

//*************************_CNumericalSCoeff
//bool _CNumericalSCoeff::GetSCoeff(unsigned Index, long double& Value)
//{
//	if(Index>=size())
//		return false;
//	_CNumericalSEntry& SE=at(Index);
//	if(!SE.first)
//		return false;
//	Value=SE.second;
//	return true;
//}
//void _CNumericalSCoeff::SetSCoeff(unsigned Index, const long double& Value)
//{
//	if(Index>=size())
//		resize(Index+1,_CNumericalSEntry(false,0.0));
//	_CNumericalSEntry Entry(true,Value);
//	at(Index)=Entry;
//}
//
//void _CNumericalSCoeff::ClearIt()
//{
//	for(iterator it=begin();it!=end();it++)
//		(*it).first=false;
//}
//
//void _CNumericalSCofactors::ClearIt()
//{
//	for(iterator it=begin();it!=end();it++)
//		(*it).ClearIt();
//}