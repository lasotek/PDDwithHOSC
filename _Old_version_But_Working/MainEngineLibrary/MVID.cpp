#include "StdAfx.h"
#include "MVID.h"


MVIDS::~MVIDS(void)
{
}

size_t MVIDS::GetHash(size_t& Core) const
{
	for(const_iterator it=begin();it!=end();it++)
	{
		Core*=HASH_FACTOR;
		Core^=(*it);
	}
	return Core;
}

size_t MVIDS::GetHashLess(size_t& Core) const
{
	size_t _size=size();
	if(_size>0) --_size;
	for(size_t i=0;i<_size;i++)
	{
		Core*=HASH_FACTOR;
		Core^=at(i);
	}
	return Core;
}
size_t MVIDS::GetHashRev(size_t& Core) const
{
	size_t _size=size();
	Core*=HASH_FACTOR;
	Core^=_size;
	if(_size>0)
	{
		Core*=HASH_FACTOR;
		Core^=back();
	}
	return Core;
}

bool MVIDS::RevEq(const MVIDS& Right) const
{
	size_t _size=size();
	size_t _rsize=Right.size();
	if(_size!=_rsize)
		return false;
	return back()==Right.back();
}

bool MVIDS::AlmostEq(const MVIDS& Right) const
{
	size_t _size=size();
	size_t _rsize=Right.size();
	if(_size!=_rsize)
		return false;
	if(_size>0) --_size;
	for(size_t i=0;i<_size;i++)
		if(at(i)!=Right[i])
			return false;
	return true;
}

//bool operator==(const MVIDS& Right) const
//{
//	size_t _size=size();
//	size_t _rsize=Right.size();
//	if(_size!=_rsize)
//		return false;
//	for(size_t i=0;i<_size;i++)
//		if(at(i)!=Right[i])
//			return false;
//	return true;
//}

string MVIDS::ToStr() const
{
	if(size()==0)
		return "";
	string Res="(";
	for(const_iterator it=begin();it!=end();it++)
	{
		Res+=ulong2str(*it);
		Res+='.';
	}
	Res[Res.size()-1]=')';
	return Res;
}