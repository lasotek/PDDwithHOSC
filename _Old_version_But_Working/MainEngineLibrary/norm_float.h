#pragma once
#define _USE_MATH_DEFINES
#include <string>
#include <iostream>
#include <complex>
#include <math.h>
#include <map>
#include <limits>
#include "AbstractResTables.h"
#include "AbstractOperationDataContainer.h"
#include "binary_filer.h"
#include "roots_poly.h"

bool IsEqual(long double Left, long double Right);

class norm_float
{
public:
	constexpr norm_float(long double ld=0.0, long Mantise=0);
	norm_float(const norm_float& Source):m_Base(Source.m_Base),m_Mantise(Source.m_Mantise) {}
	//~norm_float(void);
	bool operator==(const norm_float& Right) const {return m_Base==Right.m_Base && 
		IsEqual(m_Mantise,Right.m_Mantise);}
	bool operator!=(const norm_float& Right) const {return !operator==(Right);}
	bool operator<(const norm_float& Right) const {return Comp(Right)<0;}
	bool operator<=(const norm_float& Right) const {return Comp(Right)<=0;}
	bool operator>(const norm_float& Right) const {return Comp(Right)>0;}
	bool operator>=(const norm_float& Right) const {return Comp(Right)>=0;}
	norm_float& operator=(const norm_float& Source);
	norm_float& operator=(long double Value);
	norm_float& operator+=(const norm_float& Right) {Add(Right);return *this;}
	norm_float& operator-=(const norm_float& Right) {Subtract(Right);return *this;}
	norm_float& operator*=(const norm_float& Right) {Multiply(Right);return *this;}
	norm_float& operator/=(const norm_float& Right) {Divide(Right);return *this;}
	constexpr norm_float operator-() const {return norm_float(-m_Base,m_Mantise);}
	norm_float operator+() const {return *this;}
	norm_float operator+(const norm_float& R) const;
	constexpr norm_float operator-(const norm_float& R) const;
	norm_float operator*(const norm_float& R) const;
	norm_float operator/(const norm_float& R) const;
	string ToString() const;
	bool IsOne() const {return m_Mantise==0 && IsEqual(m_Base,1.0);}
	long double ToLD(long Shift=0) const;
	//basic_ostream<char, char_traits<char> >& operator<<(basic_ostream<char, char_traits<char> >& stream)
	//{
	//	stream<<ToString();
	//	return stream;
	//}
	friend long long GetTypedHash(long long& Core,const norm_float& Value);
	friend txt_stream& operator<<(txt_stream& stream, const norm_float& Value);
	friend short sgn(const norm_float& Value);
	friend norm_float abs(const norm_float& Value);
	friend norm_float sqrt(const norm_float& Value);
	friend _binary_filer& operator<<(_binary_filer& filer, const norm_float& Value);
	friend _binary_filer& operator>>(_binary_filer& filer, norm_float& Value);
	long double GetBase() const {return m_Base;}
	long GetMantise() const {return m_Mantise;}
	short ConversetableToLongDouble() const 
	{
		if(m_Mantise<LDBL_MIN_10_EXP)
			return -1;
		if(m_Mantise>LDBL_MAX_10_EXP)
			return 1;
		return 0;
	}
	long CommonMantise(const norm_float& Other, bool Sqrtable) const;
	//static norm_float epsilon() { return norm_float(1.0, numeric_limits<long>::min()); }
protected:
	short Comp(const norm_float& Right) const;
	void Add(const norm_float& Right);
	void Subtract(const norm_float& Right);
	void Multiply(const norm_float& Right);
	void Divide(const norm_float& Right);
	constexpr norm_float& normalize();
	norm_float& de_normalize(long DesiredMantise);
	long double m_Base;
	long m_Mantise;
};

inline
norm_float operator*(const long double& Left, const norm_float& Right)
{
	norm_float RR(Right);
	return RR*=Left;
}

inline
norm_float operator+(const long double& Left, const norm_float& Right)
{
	norm_float RR(Right);
	return RR+=Left;
}

inline
norm_float operator-(const long double& Left, const norm_float& Right)
{
	norm_float LL(Left);
	return LL-=Right;
}

inline
norm_float operator/(const long double& Left, const norm_float& Right)
{
	norm_float LL(Left);
	return LL/=Right;
}

inline
bool operator==(const long double& Left, const norm_float& Right)
{
	norm_float LL(Left);
	return LL==Right;
}

inline
bool operator>(const long double& Left, const norm_float& Right)
{
	norm_float LL(Left);
	return LL>Right;
}

inline
bool operator>=(const long double& Left, const norm_float& Right)
{
	norm_float LL(Left);
	return LL>=Right;
}

inline
bool operator<(const long double& Left, const norm_float& Right)
{
	norm_float LL(Left);
	return LL<Right;
}

inline
bool operator<=(const long double& Left, const norm_float& Right)
{
	norm_float LL(Left);
	return LL<=Right;
}

//basic_ostream<char, char_traits<char> >& operator<<(basic_ostream<char, char_traits<char> >& stream, const norm_float& Value);
//
//_binary_filer& operator<<(_binary_filer& filer, const norm_float& Value);
//_binary_filer& operator>>(_binary_filer& filer, norm_float& Value);
//
//long long GetTypedHash(long long& Core, const norm_float& Value);
//short sgn(const norm_float& Value);
//norm_float abs(const norm_float& Value);
//norm_float sqrt(const norm_float& Value);

#define WITH_NORM_FLOAT

#ifdef WITH_NORM_FLOAT
typedef norm_float NumericType;
#else
typedef long double NumericType;
#endif

string ToString(const long double& Value);
string ToString(const norm_float& Value);

extern NumericType EmptyFloat;

template<typename NumType>
bool IsOne(const NumType& Value)
{
	return Value==1.0;
}

template<>
bool IsOne(const norm_float& Value);

template<typename NumType>
complex<NumType>& MultByImag(complex<NumType>& ComplexValue,const NumType& Imag)
{
	NumType _OldRe=ComplexValue.real()*Imag;
	ComplexValue.real(-Imag*ComplexValue.imag());
	ComplexValue.imag(_OldRe);
	return ComplexValue;
}

template<typename NumType>
complex<NumType> ComplexSPower(const NumType& Factor, size_t Power,const NumType& AngFrequency)
{
	complex<NumType> Res(Factor,0.0);
	for(size_t i=0;i<Power;i++)
		MultByImag(Res,AngFrequency);
	return Res;
}
typedef complex<norm_float> NormFloatComplex;
//typedef complex<NumericType> NumComplex;

norm_float abs(const NormFloatComplex& Value);
norm_float arg(const NormFloatComplex& Value);

//norm_float abs()
template<typename Type>
bool IsZero(const Type& Value)
{
	return Value.empty();
}
template<typename EntryType>
bool IsZero(const vector<EntryType>& Vector)
{
	for (auto& Entry:Vector)
		if (!IsZero(Entry))
			return false;
	return true;
}

template<typename EntryType>
size_t ChooseTheBiggestOne(const vector<EntryType>& Vector)
{
	size_t CurrI = 0;
	EntryType CurrMax(0.0);
	for (size_t i = 0;i<Vector.size();i++)
		if (abs(Vector[i]) > CurrMax)
		{
			CurrI = i;
			CurrMax = abs(Vector[i]);
		}
	return CurrI;
}

template<typename EntryType>
bool SelectIfBigEnough(const vector<EntryType>& Vector, EntryType Threshold, vector<bool> ChooseRes)
{
	ASSERTPDD(Vector.size() == ChooseRes.size());
	bool Chosen = false;
	EntryType GlobalValue(0.0);
	for (auto& Entry : Vector)
		GlobalValue += abs(Entry);
	for (size_t i = 0; i < Vector.size(); i++)
		if (abs(Vector[i]) / GlobalValue >= Threshold)
		{
			ChooseRes[i] = true;
			Chosen = true;
		}
	return Chosen;
}

template<>
bool IsZero(const NumericType& Value);

template<typename Type>
const Type& GetConstZero();

template<>
const NumericType& GetConstZero();

template<typename Type>
class sparse_poly 
{
public:
	sparse_poly():m_MaxPower(0) {}
	~sparse_poly() {}
	sparse_poly(const sparse_poly& Source):m_Map(Source.m_Map),m_MaxPower(Source.m_MaxPower) {}
	void remove(size_t Index)
	{
		iterator it=m_Map.find(Index);
		if(it==m_Map.end()) return;
		m_Map.erase(it);
	}
	const Type& get(size_t Index) const
	{
		const_iterator it=m_Map.find(Index);
		if(it==m_Map.end()) return GetConstZero<Type>();
		return it->second;
	}
	const Type& put(size_t Index,const Type& Value)
	{
		//if(Value==Zero)
		if(IsZero(Value))
			remove(Index);
		else
		{
			m_Map.insert(value_type(Index,Value));
			m_MaxPower=max(m_MaxPower,Index);
		}
		return Value;
	}
	sparse_poly operator-()
	{
		sparse_poly Res;
		iterator _begin=m_Map.begin(),_end=m_Map.end();
		for(iterator it=_begin;it!=_end;it++)
		{
			size_t Index=it->first;
			Type& V=it->second;
			Res.put(Index,-V);
		}
	}
	sparse_poly& operator=(const sparse_poly& S)
	{
		m_Map=S.m_Map;
		m_MaxPower=S.m_MaxPower;
		return *this;
	}
	sparse_poly& CopyWithLimit(const sparse_poly& S,size_t MaxPower)
	{
		m_Map.clear();
		for(sparse_poly::const_iterator it=S.m_Map.begin(),_e=S.m_Map.end();it!=_e;it++)
		{
			size_t P=it->first;
			//if(P<=MaxPower)
			//	m_Map.insert(sparse_poly::value_type(P,it->second));
			if(P>MaxPower)
				break;
			m_Map.insert(sparse_poly::value_type(P,it->second));
		}
		m_MaxPower=min(S.m_MaxPower,MaxPower);
		return *this;
	}
	sparse_poly& operator+=(const sparse_poly& R)
	{
		for(const_iterator it=R.m_Map.begin(),_e=R.m_Map.end();it!=_e;it++)
			add_index(it->first,it->second);
		return *this;
	}
	sparse_poly operator+(const sparse_poly& R) const
	{
		sparse_poly Res(*this);
		return Res.operator +=(R);
	}
	sparse_poly& operator-=(const sparse_poly& R)
	{
		for(const_iterator it=R.m_Map.begin(),_e=R.m_Map.end();it!=_e;it++)
			substract_index(it->first,it->second);
		return *this;
	}
	sparse_poly& operator*=(const sparse_poly& R)
	{
		*this=this->operator *(R);
		return *this;
	}
	sparse_poly& operator*=(const Type& R)
	{
		for(MAP_TYPE::iterator it=m_Map.begin();it!=m_Map.end();it++)
			it->second*=R;
		return *this;
	}
	sparse_poly operator*(const sparse_poly& R) const
	{
		sparse_poly Res;
		size_t _lsize=m_Map.size(),_rsize=R.m_Map.size();
		const MAP_TYPE& Big=_lsize>_rsize?m_Map:R.m_Map;
		const MAP_TYPE& Small=_lsize>_rsize?R.m_Map:m_Map;
		for(const_iterator lit=Small.begin(),_le=Small.end();lit!=_le;lit++)
			for(const_iterator rit=Big.begin(),_re=Big.end();rit!=_re;rit++)
				Res.add_index(lit->first+rit->first,lit->second*rit->second);
		return Res;
	};
	void MultWithLimit(const sparse_poly& L,const sparse_poly& R,size_t Limit) 
	{
		ClearAll();
		size_t _lsize=L.m_Map.size(),_rsize=R.m_Map.size();
		const MAP_TYPE& Big=_lsize>_rsize?L.m_Map:R.m_Map;
		const MAP_TYPE& Small=_lsize>_rsize?R.m_Map:L.m_Map;
		for(const_iterator lit=Small.begin(),_le=Small.end();lit!=_le;lit++)
			if(lit->first>Limit)
				break;
			else for(const_iterator rit=Big.begin(),_re=Big.end();rit!=_re;rit++)
			{
				size_t Power=lit->first+rit->first;
				if(Power>Limit)
					break;
				add_index(Power,lit->second*rit->second);
			}
	}
	void MultWithLimit(sparse_poly& Result,const sparse_poly& R,size_t Limit) const
	{
		Result.ClearAll();
		size_t MaxPower=min(Limit,m_MaxPower+R.m_MaxPower);
		size_t _lsize=m_Map.size(),_rsize=R.m_Map.size();
		const MAP_TYPE& Big=_lsize>_rsize?m_Map:R.m_Map;
		const MAP_TYPE& Small=_lsize>_rsize?R.m_Map:m_Map;
		for(const_iterator lit=Small.begin(),_le=Small.end();lit!=_le;lit++)
			if(lit->first>Limit)
				break;
			else for(const_iterator rit=Big.begin(),_re=Big.end();rit!=_re;rit++)
			{
				size_t Power=lit->first+rit->first;
				if(Power>Limit)
					break;
				Result.add_index(Power,lit->second*rit->second);
			}
	}
	void MultForOnePower(sparse_poly& Result,const sparse_poly& R,size_t Limit) const
	{
		Result.ClearAll();
		size_t _lsize=min(Limit,m_Map.size()),_rsize=min(Limit,R.m_Map.size());
		const MAP_TYPE& Big=_lsize>_rsize?m_Map:R.m_Map;
		const MAP_TYPE& Small=_lsize>_rsize?R.m_Map:m_Map;
		for(const_iterator lit=Small.begin(),_le=Small.end();lit!=_le;lit++)
			for(const_iterator rit=Big.begin(),_re=Big.end();rit!=_re;rit++)
			{
				size_t Power=lit->first+rit->first;
				if(Power!=Limit)
					continue;
				Result.add_index(Power,lit->second*rit->second);
			}

	}
	void ClearAll() {m_Map.clear();m_MaxPower=0;}
	size_t size() const {return m_Map.size();}
	sparse_poly& MultiplyBySPower(NumericType Value,unsigned short sPower)
	{
		sparse_poly R;
		R.put(sPower,Value);
		return operator *=(R);
	}
	bool empty() const {return m_Map.empty();}
	size_t get_order() const {return m_MaxPower;}
	void WriteToTextStream(iostream& stream) const
	{
		if(empty())
			stream<<"0";
		else
			for(size_t i=m_MaxPower;;i--)
			{
				const NumericType& Value=get(i);
				if(Value==EmptyFloat) 
					if(i==0)
						break;
					else 
						continue;
				stream<<((Value < EmptyFloat)?" - ":" + ");
				if(i>0)
					stream<<"s^"<<i<<"*";
				stream<<abs(Value);
				if(i==0) break;
			}
		//stream<<'\n';
	}
	class EnergyDistribution : public vector<Type>
	{
	};
	typedef Type NumericType;
	typedef complex<Type> NumComplex;
	class MagnitudeDistribution : public vector<NumComplex>
	{
	};
	class MagnitudeDistributionExt : public vector<NumComplex>
	{
	public:
		Type GetGlobalRe() { return 0.0;}
		Type GetGlobalIm() { return 0.0; }
	};
	NumComplex freq_response(const Type& AngFrequency)
	{
		
		NumComplex Res(get(m_MaxPower),0.0);
		for(size_t i=m_MaxPower;i>0;i--)
		{
			MultByImag(Res,AngFrequency);
			NumericType a=get(i-1);
			if(!IsZero(a))
				Res+=a;
		}
		return Res;
	}
	void GetMagnitudeDistribution(MagnitudeDistribution& Distribution, const Type& MaxAngFrequency)
	{
		Distribution.clear();//Rozdzieliæ global
		Distribution.resize(m_MaxPower+1);
		NumComplex CompAngFreq(1.0,0.0);
		Type Global=0.0;
		for(size_t i=0;i<=m_MaxPower;i++,MultByImag(CompAngFreq,MaxAngFrequency))
		{
			const Type& Value=get(i);
			if(IsZero(Value))
				continue;
			NumComplex& Component=Distribution[i];
			Component=CompAngFreq*Value;
			Type& R=Component.real();
			Type& I=Component.imag();
			Global+=(R*R+I*I);
		}
		if(Global!=0.0)
		{
			Global=sqrt(Global);
			for(size_t i=0;i<=m_MaxPower;i++)
			{
				NumComplex& Component=Distribution[i];
				Component*=100;
				Component/=Global;
			}
		}
	}
	void GetMagnitudeDistributionReIm2(MagnitudeDistribution& Distribution, const Type& MaxAngFrequency) const
	{
		Distribution.clear();//Rozdzieliæ global
		Distribution.resize(m_MaxPower+1);
		NumComplex CompAngFreq(1.0,0.0);
		Type GlobalRe=0.0,
			GlobalIm=0.0;
		for(size_t i=0;i<=m_MaxPower;i++,MultByImag(CompAngFreq,MaxAngFrequency))
		{
		}
	}
	void GetMagnitudeDistributionReIm(MagnitudeDistribution& Distribution, const Type& MaxAngFrequency) const
	{
		Distribution.clear();//Rozdzieliæ global
		Distribution.resize(m_MaxPower+1);
		NumComplex CompAngFreq(1.0,0.0);
		Type GlobalRe=0.0,
			GlobalIm=0.0;
		for(size_t i=0;i<=m_MaxPower;i++,MultByImag(CompAngFreq,MaxAngFrequency))
		{
			const Type& Value=get(i);
			if(IsZero(Value))
				continue;
			NumComplex& Component=Distribution[i];
			Component=CompAngFreq*Value;
			Type& R=Component.real();
			Type& I=Component.imag();
			GlobalRe+=(R*R);
			GlobalIm+=(I*I);
		}
		GlobalRe=sqrt(GlobalRe);
		GlobalIm=sqrt(GlobalIm);
		for(size_t i=0;i<=m_MaxPower;i++)
		{
			NumComplex& Component=Distribution[i];
			//Component*=100;
			if(GlobalRe!=0.0)
				Component.real(Component.real()/=GlobalRe);
			if(GlobalIm!=0.0)
				Component.imag(Component.imag()/=GlobalIm);
		}
	}
	void GetEnergyDistribution(EnergyDistribution& Distribution, const Type& MaxAngFrequency) const
	{
		Distribution.clear();
		Distribution.resize(m_MaxPower+1);
		Type AngFreq=1.0;
		Type Global=0.0;
		for(size_t i=0;i<=m_MaxPower;i++)
		{
			Type& Component=Distribution[i];
			Type Value=abs(get(i));
			if(!IsZero(Value))
			{
				Component=Value*Value*AngFreq;
				Global+=Component;
			}
			AngFreq*=MaxAngFrequency;
			AngFreq*=MaxAngFrequency;
		}
		if(Global!=0.0)
			for(size_t i=0;i<=m_MaxPower;i++)
			{
				Type& Component=Distribution[i];
				//Component*=100.0;
				Component/=Global;
			}
	}
	bool IsOne() const
	{
		if(m_Map.size()!=1)
			return false;
		MAP_TYPE::const_iterator it=m_Map.find(0);
		if(it==m_Map.end())
			return false;
		return ::IsOne(it->second);
	}
	void CopyWithPowerLimit(sparse_poly& OutElement, const sparse_poly& InElement,size_t MaxPower)
	{
		OutElement.ClearAll();
		for(sparse_poly::const_iterator it=InElement.m_Map.begin(),_e=InElement.m_Map.end();it!=_e;it++)
			if(it->first<=MaxPower)
				add_index(it->first,it->second);
	}
	long long GetHash(long long Core = HASH_CORE) const
	{
		for (auto& Elem : m_Map)
		{
			Core *= HASH_FACTOR;
			Core ^= Elem.first;
			Core = GetTypedHash(Core,Elem.second);
		}
		return Core;
	}
	bool operator==(const sparse_poly& Right) const
	{
		return m_MaxPower == Right.m_MaxPower && m_Map == Right.m_Map;
	}
protected:
	typedef typename /*hash_*/map<size_t,Type,hash_compare<size_t,less<size_t>>> MAP_TYPE;
	typedef typename MAP_TYPE::iterator iterator;
	typedef typename MAP_TYPE::const_iterator const_iterator;
	size_t m_MaxPower;
	const Type& add_index(size_t Index,const Type& Value)
	{
		iterator it=m_Map.find(Index);
		if(it==m_Map.end())
		{
			if(IsZero(Value))
				return GetConstZero<Type>();
			else
				return put(Index,Value);
		}
		else
		{
			Type& Res=it->second;
			Res+=Value;
			if(IsZero(Res))
			{
				m_Map.erase(it);
				return GetConstZero<Type>(/*Res*/);
			}
			else
				return Res;
		}
	}
	const Type& substract_index(size_t Index,const Type& Value)
	{
		Type MValue=-Value;
		return add_index(Index,MValue);
	}
	const Type& multiply_index(size_t Index,const Type& Value)
	{
		if(Value==Zero)
		{
			remove(Index);
			return Zero;
		}
		iterator it=m_Map.find(Index);
		if(it==m_Map.end())
			return Zero;
		Type& Res=it->second;
		Res*=Value;
		return Res;
	}
	const Type& divide_index(size_t Index,const Type& Value)
	{
		if(Value==Zero)
			RISEPDD(eIllegalOperation,"Divsion by 0");
		iterator it=m_Map.find(Index);
		if(it==m_Map.end())
			return Zero;
		Type& Res=it->second;
		Res/=Value;
		return Res;
	}
	typedef typename MAP_TYPE::value_type value_type;
	MAP_TYPE m_Map;
	template<typename Type>
		friend _binary_filer& operator<<(_binary_filer& filer, const sparse_poly<Type>& Value);
	template<typename Type>
		friend _binary_filer& operator>>(_binary_filer& filer, sparse_poly<Type>& Value);
	friend class _CFlatVerticesSPowerLimiter;
	friend class _CSPowerLimiter;
	//friend class multi_sparse_poly<Type>;
};

template<typename NumType>
bool IsOne(const sparse_poly<NumType>& Value)
{
	return Value.IsOne();
}


//template<typename Type>
//basic_ostream<char, char_traits<char> >& operator<<(basic_ostream<char, char_traits<char> >& stream, const typename sparse_poly<Type>::EnergyDistribution& ED)
//{
//	return stream;
//}

template<typename Type>
_binary_filer& operator<<(_binary_filer& filer, const sparse_poly<Type>& Value)
{
	filer<<Value.m_MaxPower<<Value.m_Map.size();
	for(sparse_poly<Type>::const_iterator it=Value.m_Map.begin();it!=Value.m_Map.end();it++)
	{
		filer<<it->first<<it->second;
	}
	return filer;
}

template<typename Type>
_binary_filer& operator>>(_binary_filer& filer, sparse_poly<Type>& Value)
{
	Value.ClearAll();
	size_t Size;
	filer>>Value.m_MaxPower>>Size;
	for(size_t i=0;i<Size;i++)
	{
		size_t Index;
		filer>>Index;
		Type& sValue=Value.m_Map.insert(sparse_poly<Type>::MAP_TYPE::value_type(Index,Type())).first->second;
		filer>>sValue;//dopisaæ
	}
	return filer;
}

template<typename Type>
long long GetTypedHash(long long Core, const sparse_poly<Type>& Poly)
{
	return Poly.GetHash(Core);
}

typedef sparse_poly<NumericType> _CSparsePolynomial;
typedef VectComplex<NumericType> NormVectComplex;

typedef vector<NumericType> NumericVector;

void SpPoly2Roots(const _CSparsePolynomial& InPoly,NormVectComplex& Roots);

void PrepareLinAngFreqSpace(NumericVector& Space, NumericType FFrom, NumericType FTo, size_t NoIntrervals);
void PrepareLogAngFreqSpace(NumericVector& Space, int MinDec, int MaxDec, size_t NoIntrervalsInDec);

typedef complex<NumericType> NumericComplex;

void DetermineFreqsValues(NormVectComplex& Results, _CSparsePolynomial& PolynomialS, const NumericVector& FreqVector);

txt_stream& operator<<(txt_stream& stream, const _CSparsePolynomial::MagnitudeDistribution& MD);
txt_stream& operator<<(txt_stream& stream, const _CSparsePolynomial::EnergyDistribution& ED);

template<>
const _CSparsePolynomial& GetConstZero();

template<class NumType>
class  multi_sparse_poly: public sparse_poly<sparse_poly<NumType>>
{
public:
	//typedef sparse_poly<sparse_poly<NumType>> _MyType;
	~multi_sparse_poly() {}
	typedef sparse_poly<NumType> element_poly; 
	typedef sparse_poly<element_poly> multi_poly;
	enum {MONE_TERMINAL=-1,ZERO_TERMINAL=0,ONE_TERMINAL=1,H_DESC=2, L_DESC=3};
	multi_sparse_poly(size_t Type=ZERO_TERMINAL) 
	{
		if(Type!=ZERO_TERMINAL)
		{
			element_poly poly;
			poly.put(0,Type==MONE_TERMINAL?-1.0:1.0);
			if(Type==H_DESC || Type==L_DESC)
			{
				element_poly poly0;
				if(Type==H_DESC)
				{
					put(0,poly);
					put(1,poly0);
				}
				else
				{
					put(0,poly0);
					put(1,poly);
				}
			}
			else
				put(0,poly);
		}
	}
	multi_sparse_poly(const multi_sparse_poly& Source):multi_poly(Source) {}
	multi_sparse_poly(const multi_poly& Source):multi_poly(Source) {}
	void SetZero() {ClearAll();}
	void SetLeaf(bool Reset=true, size_t Index=0, short Sgn=1)
	{
		if(Reset) ClearAll();
		element_poly poly;
		poly.put(0,Sgn);
		put(Index,poly);
	}
	void SetOne(bool Reset=true, size_t Index=0)
	{
		if(Reset) ClearAll();
		element_poly poly;
		poly.put(0,1);
		put(Index,poly);
	}
	void SetMinusOne(bool Reset=true, size_t Index=0)
	{
		if(Reset) ClearAll();
		element_poly poly;
		poly.put(0,-1);
		put(Index,poly);
	}
	multi_sparse_poly& operator*=(const element_poly& R)
	{
		iterator _b=m_Map.begin(),_e=m_Map.end();
		for(iterator it=_b;it!=_e;it++)
			it->second*=R;
		return *this;
	}
	multi_sparse_poly operator*(const element_poly& R) const
	{
		multi_sparse_poly L(*this);
		L*=R;
		return L;
	}
	sparse_poly& operator*=(const NumType& R)
	{
		for(MAP_TYPE::iterator it=m_Map.begin();it!=m_Map.end();it++)
			it->second*=R;
		return *this;
	}
	void WriteToTextStream(iostream& stream) const
	{
		if(empty())
			stream<<"0";
		else
			for(size_t i=0;i<=m_MaxPower;i++)
			{
				const element_poly& Element=get(i);
				if(Element.empty()) 
						continue;
				stream<<"E"<<i<<" = ";
				Element.WriteToTextStream(stream);
				stream<<endl;
			}
		stream<<'\n';
	}
	const NumType& get2(size_t Level, size_t Power) const
	{
		const element_poly& Element=get(Level);
		if(IsZero(Element)) return GetConstZero<NumType>();
		return Element.get(Power);
	}
	element_poly ScalarProduct(const multi_sparse_poly& right) const
	{
		element_poly Res;
		size_t lsize=size(),
			rsize=right.size();
		//multi_poly& mp=lsize>rsize?right:*this;
		if(rsize<lsize)
			return right.ScalarProduct(*this);
		for(const_iterator it=m_Map.begin();it!=m_Map.end();it++)
		{
			size_t Index=it->first;
			const element_poly& poly=it->second;
			Res+=poly*right.get(Index);
		}
		return Res;
	}
	void ScalarProduct(element_poly& Result, const multi_sparse_poly& right) const
	{
		size_t lsize=size(),
			rsize=right.size();
		//multi_poly& mp=lsize>rsize?right:*this;
		if(rsize<lsize)
		{
			right.ScalarProduct(Result,*this);
			return;
		}
		Result.ClearAll();
		for(const_iterator it=m_Map.begin();it!=m_Map.end();it++)
		{
			size_t Index=it->first;
			const element_poly& poly=it->second;
			Result+=right.IsOne()?poly:poly*right.get(Index);
		}
	}
	void ScalarProduct2(element_poly& Result, const multi_sparse_poly& right) const
	{
		size_t lsize=size(),
			rsize=right.size();
		//multi_poly& mp=lsize>rsize?right:*this;
		if(rsize<lsize)
		{
			right.ScalarProduct(Result,*this);
			return;
		}
		Result.ClearAll();
		for(const_iterator it=m_Map.begin();it!=m_Map.end();it++)
		{
			size_t Index=it->first;
			const element_poly& poly=it->second;
			Result+=right.IsOne()?poly:poly*right.get(Index);
		}
	}
	void ScalarProduct(element_poly& Result, const multi_sparse_poly& right,size_t PowerLimit) const
	{
		size_t lsize=size(),
			rsize=right.size();
		//multi_poly& mp=lsize>rsize?right:*this;
		if(rsize<lsize)
		{
			right.ScalarProduct(Result,*this,PowerLimit);
			return;
		}
		Result.ClearAll();
		for(const_iterator it=m_Map.begin();it!=m_Map.end();it++)
		{
			size_t Index=it->first;
			const element_poly& poly=it->second;
			element_poly LocalRes;
			poly.MultWithLimit(LocalRes,right.get(Index),PowerLimit);
			Result+=LocalRes;
			//Result+=right.IsOne()?poly:poly*right.get(Index);
		}
	}
	typedef multi_sparse_poly<NumType> _CSubModelNumericPattern;
	typedef self_exp_vector<const _CSubModelNumericPattern*> _CDescModelNumericPattern;
	void NewScaler(const multi_sparse_poly& OldScaler,const _CDescModelNumericPattern& DescScalers)
	{
		m_Map.clear();
		for(size_t i=0;i<DescScalers.size();i++)
		{
			const multi_sparse_poly* pDesc=DescScalers[i];
			if(pDesc==NULL)
				continue;
			OldScaler.ScalarProduct(m_Map[i],*pDesc);
		}
	}
	void NewScaler(const multi_sparse_poly& OldScaler,const _CDescModelNumericPattern& DescScalers,
		const _CDescSigns2& Sgns)
	{
		m_Map.clear();
		for(size_t i=0;i<DescScalers.size();i++)
		{
			const multi_sparse_poly* pDesc=DescScalers[i];
			if(pDesc==NULL)
				continue;
			OldScaler.ScalarProduct(m_Map[i],*pDesc);
			m_Map[i]*=Sgns[i];
		}
	}
	void NewScaler(const multi_sparse_poly* pOldScaler,const _CDescModelNumericPattern& DescScalers,
		const _CDescSigns2& Sgns)
	{
		m_Map.clear();
		for(size_t i=0;i<DescScalers.size();i++)
		{
			const multi_sparse_poly* pDesc=DescScalers[i];
			if(pDesc==NULL)
				continue;
			element_poly OutPoly;
			if(pOldScaler==NULL)
			{
				ASSERTPDD(pDesc->size()==1);
				OutPoly=pDesc->get(0);
			}
			else
				for(multi_sparse_poly::const_iterator it=pDesc->m_Map.begin(),_e=pDesc->m_Map.end();it!=_e;it++)
				{
					size_t Index=it->first;
					multi_sparse_poly::const_iterator sit=pOldScaler->m_Map.find(Index);
					if(sit==pOldScaler->m_Map.end())
						continue;
					OutPoly+=(it->second*sit->second);
				}
			OutPoly*=Sgns[i];
			if(!OutPoly.empty())
				put(i,OutPoly);
		}
	}
	void NewScaler(const multi_sparse_poly& OldScaler,const _CDescModelNumericPattern& DescScalers,
		const _CDescSigns2& Sgns,size_t MaxPower)
	{
		m_Map.clear();
		for(size_t i=0;i<DescScalers.size();i++)
		{
			const multi_sparse_poly* pDesc=DescScalers[i];
			if(pDesc==NULL)
				continue;
			OldScaler.ScalarProduct(m_Map[i],*pDesc,MaxPower);
			m_Map[i]*=Sgns[i];
		}
	}
	void NewScaler(const multi_sparse_poly* pOldScaler,const _CDescModelNumericPattern& DescScalers,
		const _CDescSigns2& Sgns,size_t MaxPower)//poprawiæ
	{
		m_Map.clear();
		for(size_t i=0;i<DescScalers.size();i++)
		{
			const multi_sparse_poly* pDesc=DescScalers[i];
			if(pDesc==NULL || pDesc->empty())
				continue;
			element_poly OutPoly;
			if(pOldScaler==NULL)
			{
				ASSERTPDD(pDesc->size()==1);
				OutPoly.CopyWithLimit(pDesc->get(0),MaxPower);
			}
			else
				for(multi_sparse_poly::const_iterator it=pDesc->m_Map.begin(),_e=pDesc->m_Map.end();it!=_e;it++)
				{
					size_t Index=it->first;
					multi_sparse_poly::const_iterator sit=pOldScaler->m_Map.find(Index);
					if(sit==pOldScaler->m_Map.end())
						continue;
					element_poly Aux;
					it->second.MultWithLimit(Aux,sit->second,MaxPower);
					OutPoly+=Aux;
				}
			OutPoly*=Sgns[i];
			if(!OutPoly.empty())
				put(i,OutPoly);
		}
	}
	void CopyMultiWithPowerLimit(multi_sparse_poly& OutPoly, const multi_sparse_poly& InPoly,size_t MaxPower)
	{
		OutPoly.ClearAll();
		for(multi_sparse_poly::const_iterator it=InPoly.m_Map.begin(),_e=InPoly.m_Map.end();it!=_e;it++)
		{
			sparse_poly& OutElement=OutPoly[it->first];
			CopyWithPowerLimit(OutElement,InPoly->second,MaxPower);
		}
	}
	multi_sparse_poly(const multi_sparse_poly& OldScaler,const _CDescModelNumericPattern& DescScalers)
	{
		NewScaler(OldScaler,DescScalers);
	}
protected:
	friend class _CFlatVerticesSPowerLimiter;
	//multi_sparse_poly& operator*=(const multi_sparse_poly& R)
	//{
	//	return *this;
	//}
	multi_sparse_poly operator*(const multi_sparse_poly& R) const
	{
		multi_sparse_poly L(*this);
		return L;
	}
};

//typedef multi_sparse_poly<NumericType> _CSubModelNumericPattern;
typedef multi_sparse_poly<NumericType>::_CSubModelNumericPattern _CSubModelNumericPattern;

//long long GetTypedHash(long long Core, const _CSubModelNumericPattern& NumPattern);

void CompValue2ModelNumPatt(_CSubModelNumericPattern& OutRes,NumericType Value, unsigned short Power);

class _CSubModelNumericPatternStreamContainer /*: public _streamable*/
{
public:
	_CSubModelNumericPatternStreamContainer(_CSubModelNumericPattern& Pat):
	m_SMNPat(Pat) {}
	/*virtual*/ void Store(_binary_filer& Filer);
	/*virtual*/ void Load(_binary_filer& Filer);
protected:
	//DECLARE_DYNAMIC_CREATION(_CSubModelNumericPatternStreamContainer);
	_CSubModelNumericPattern& m_SMNPat;
};

//class _CSubModelNumericPattern : public multi_sparse_poly<NumericType>, public _streamable
//{
//public:
//	_CSubModelNumericPattern(size_t Sort=ZERO_TERMINAL):
//	  multi_sparse_poly<NumericType>(Sort) {}
//    _CSubModelNumericPattern(const _CSubModelNumericPattern& Source):
//	  multi_sparse_poly<NumericType>(Source)
//	  {}
//    _CSubModelNumericPattern(const multi_sparse_poly<NumericType>& Source):
//	  multi_sparse_poly<NumericType>(Source)
//	  {}
//	virtual void Store(_binary_filer& Filer);
//	virtual void Load(_binary_filer& Filer);
//protected:
//	DECLARE_DYNAMIC_CREATION(_CSubModelNumericPattern);
//};

//typedef self_exp_vector<const _CSubModelNumericPattern*> _CDescModelNumericPattern;
typedef multi_sparse_poly<NumericType>::_CDescModelNumericPattern _CDescModelNumericPattern;
typedef self_exp_vector<short> _CDescSignsVector;

//typedef vector<const _CSubModelNumericPattern*> _CDescModelNumericPattern;
//typedef vector<short> _CDescSignsVector;

extern const _CSubModelNumericPattern ZeroPattern;
extern const _CSubModelNumericPattern OnePattern;
extern const _CSubModelNumericPattern MOnePattern;
extern const _CSubModelNumericPattern HDescFilterPattern;
extern const _CSubModelNumericPattern LDescFilterPattern;

//typedef complex<norm_float> norm_complex;

//NormFloatComplex sqrt(const NormFloatComplex& _Left);
//inline
//NormFloatComplex polar(const NormFloatComplex& Rho,const double& Theta)
//{
//	
//}
//NormFloatComplex operator/(const NormFloatComplex& _Left, const NormFloatComplex& _Right);


// CLASS numeric_limits<long double>
template <>
class numeric_limits<norm_float> : public _Num_float_base {
public:
	_NODISCARD static constexpr norm_float(min)() noexcept {
		//return norm_float(numeric_limits<long double>::min(), numeric_limits<long>::min());
		return norm_float(1.0L, numeric_limits<long>::min());
		//return LDBL_MIN;
	}

	_NODISCARD static constexpr norm_float(max)() noexcept {
		//return norm_float(numeric_limits<long double>::max(), numeric_limits<long>::max());
		return norm_float(1.0L, numeric_limits<long>::max());
		//return LDBL_MAX;
	}

	_NODISCARD static constexpr norm_float lowest() noexcept {
		return -(max)();
	}

	_NODISCARD static constexpr norm_float epsilon() noexcept {
		return LDBL_EPSILON;
	}

	_NODISCARD static constexpr norm_float round_error() noexcept {
		return 0.5L;
	}

	_NODISCARD static constexpr norm_float denorm_min() noexcept {
		return norm_float(numeric_limits<long double>::epsilon(),numeric_limits<long>::min());
		//return LDBL_TRUE_MIN;
	}

	_NODISCARD static constexpr norm_float infinity() noexcept {
		return norm_float(__builtin_huge_val(), numeric_limits<long>::min());
	}

	_NODISCARD static constexpr norm_float quiet_NaN() noexcept {
		return norm_float(__builtin_nan("0"),0);
	}

	_NODISCARD static constexpr norm_float signaling_NaN() noexcept {
		return norm_float(__builtin_nans("1"),0);
	}

	static constexpr int digits = LDBL_MANT_DIG;
	static constexpr int digits10 = LDBL_DIG;
	static constexpr int max_digits10 = 17;
	static constexpr int max_exponent = LDBL_MAX_EXP;
	static constexpr int max_exponent10 = LDBL_MAX_10_EXP;
	static constexpr int min_exponent = LDBL_MIN_EXP;
	static constexpr int min_exponent10 = LDBL_MIN_10_EXP;
};




template<> class  _Ctraits<norm_float>
	{	// complex traits for long double
public:
	typedef norm_float _Ty;

	//static _Ty __CLRCALL_OR_CDECL _Cosh(_Ty _Left, _Ty _Right)
	//	{	// return cosh(_Left) * _Right
	//	return (::_LCosh(_Left, _Right));
	//	}

	//static short __CLRCALL_OR_CDECL _Exp(_Ty *_Pleft, _Ty _Right, short _Exponent)
	//	{	// compute exp(*_Pleft) * _Right * 2 ^ _Exponent
	//	return (::_LExp(_Pleft, _Right, _Exponent));
	//	}

	//static _Ty  _Infv(_Ty)
	//static _Ty  _Infv()
	//	{	// return infinity
	//		return norm_float(::_LInf._Long_double,LONG_MAX);
	//	}

	//static bool  _Isinf(_Ty _Left)
	//	{	// test for infinity
	//		long double Base=_Left.GetBase();
	//		return (_Left.GetMantise() == LONG_MAX && ::_LDtest(&Base) == _INFCODE);
	//	}

	//static bool  _Isnan(_Ty _Left)
	//	{	// test for NaN
	//		long double Base=_Left.GetBase();
	//		return (_Left.GetMantise() == LONG_MIN && ::_LDtest(&Base) == _NANCODE);
	//	}

	////static _Ty  _Nanv(_Ty)
	//static _Ty  _Nanv()
	//	{	// return NaN
	//		return norm_float(::_LNan._Long_double,LONG_MIN);
	//	}

	//static _Ty __CLRCALL_OR_CDECL _Sinh(_Ty _Left, _Ty _Right)
	//	{	// return sinh(_Left) * _Right
	//	return (::_LSinh(_Left, _Right));
	//	}

	//static _Ty __CLRCALL_OR_CDECL atan2(_Ty _Yval, _Ty _Xval)
	//	{	// return atan(_Yval / _Xval)
	//	return (::atan2l(_Yval, _Xval));
	//	}

	static _Ty __CLRCALL_OR_CDECL cos(_Ty _Left)
		{	// return cos(_Left)
			return (::cosl(_Left.ToLD()));
		}

	//static _Ty __CLRCALL_OR_CDECL exp(_Ty _Left)
	//	{	// return exp(_Left)
	//	return (::expl(_Left));
	//	}

	static _Ty __CLRCALL_OR_CDECL ldexp(_Ty _Left, int _Exponent)
		{	// return _Left * 2 ^ _Exponent
			long double Base=_Left.GetBase();
			long Mantise=_Left.GetMantise();
			return (norm_float(::ldexpl(Base, _Exponent),Mantise));
		}

	//static _Ty __CLRCALL_OR_CDECL log(_Ty _Left)
	//	{	// return log(_Left)
	//	return (::logl(_Left));
	//	}

	//static _Ty __CLRCALL_OR_CDECL pow(_Ty _Left, _Ty _Right)
	//	{	// return _Left ^ _Right
	//	return (::powl(_Left, _Right));
	//	}

	static _Ty __CLRCALL_OR_CDECL sin(_Ty _Left)
		{	// return sin(_Left)
			return (::sinl(_Left.ToLD()));
		}

	static _Ty __CLRCALL_OR_CDECL sqrt(_Ty _Left)
		{	// return sqrt(_Left)
			return (::sqrt(_Left));
		}

	static _Ty _Abs(_Ty _Left) {
		// testing _Left < 0 would be incorrect when _Left is -0.0
		return (abs(_Left));
	}

	static _Ty _Copysign(_Ty _Magnitude, _Ty _Sign) {
		// testing _Sign < 0 would be incorrect when _Sign is -0.0
		return abs(_Magnitude)* sgn(_Sign);
	}
	static constexpr _Ty _Flt_eps() { // get epsilon
	//static _Ty _Flt_eps() { // get epsilon
		return _Ty(1.0, numeric_limits<long>::min());
	}
	static constexpr _Ty _Flt_norm_min() {
		return _Ty(1.0, numeric_limits<long>::min());
	}
	//static _Ty __CLRCALL_OR_CDECL tan(_Ty _Left)
	//	{	// return tan(_Left)
	//	return (::tanl(_Left));
	//	}
	};

	/*
	// CLASS _Ctraits<double>
template <>
class _Ctraits<double> {
public:
    using _Ty = double;

    static constexpr _Ty _Flt_eps() { // get epsilon
        return numeric_limits<double>::epsilon();
    }

    static constexpr _Ty _Flt_max() {
        return (numeric_limits<double>::max)();
    }

    static constexpr _Ty _Flt_norm_min() {
        return (numeric_limits<double>::min)();
    }

    static _Ty _Abs(_Ty _Left) {
        // testing _Left < 0 would be incorrect when _Left is -0.0
        return _CSTD fabs(_Left);
    }

    static _Ty _Cosh(_Ty _Left, _Ty _Right) { // return cosh(_Left) * _Right
        return _CSTD _Cosh(_Left, _Right);
    }

    static _Ty _Copysign(_Ty _Magnitude, _Ty _Sign) {
        // testing _Sign < 0 would be incorrect when _Sign is -0.0
        return _CSTD copysign(_Magnitude, _Sign);
    }

    static short _Exp(_Ty* _Pleft, _Ty _Right, short _Exponent) { // compute exp(*_Pleft) * _Right * 2 ^ _Exponent
        return _CSTD _Exp(_Pleft, _Right, _Exponent);
    }

    static constexpr _Ty _Infv() { // return infinity
        return numeric_limits<double>::infinity();
    }

    static bool _Isinf(_Ty _Left) { // test for infinity
        const auto _Uint = _Bit_cast<uint64_t>(_Left);
        return (_Uint & 0x7fffffffffffffffU) == 0x7ff0000000000000U;
    }

    static _CONSTEXPR20 bool _Isnan(_Ty _Left) {
        const auto _Uint = _Bit_cast<uint64_t>(_Left);
        return (_Uint & 0x7fffffffffffffffU) > 0x7ff0000000000000U;
    }

    static constexpr _Ty _Nanv() { // return NaN
        return numeric_limits<double>::quiet_NaN();
    }

    static bool _Signbit(_Ty _Left) {
        // testing _Left < 0 would be incorrect when _Left is -0.0
        return (_STD signbit)(_Left);
    }

    static _Ty _Sinh(_Ty _Left, _Ty _Right) { // return sinh(_Left) * _Right
        return _CSTD _Sinh(_Left, _Right);
    }

    static _Ty asinh(_Ty _Left) {
        if (_Isnan(_Left) || _Isinf(_Left) || _Left == 0) {
            return _Left;
        } else { // _Left finite nonzero
            const _Ty _Old_left = _Left;
            _Ty _Ans;

            _Left = _Abs(_Left);

            if (_Left < 2 / _Flt_eps()) {
                _Ans = log1p(_Left + _Left * _Left / (1 + sqrt(_Left * _Left + 1)));
            } else {
                _Ans = log(_Left) // _Left big, compute log(_Left+_Left)
                     + static_cast<_Ty>(0.69314718055994530941723212145817658L);
            }

            return _Copysign(_Ans, _Old_left);
        }
    }

    static _Ty atan2(_Ty _Yval, _Ty _Xval) { // return atan(_Yval / _Xval)
        return _CSTD atan2(_Yval, _Xval);
    }

    static _Ty cos(_Ty _Left) {
        return _CSTD cos(_Left);
    }

    static _Ty exp(_Ty _Left) {
        return _CSTD exp(_Left);
    }

    static _Ty ldexp(_Ty _Left, int _Exponent) { // return _Left * 2 ^ _Exponent
        return _CSTD ldexp(_Left, _Exponent);
    }

    static _Ty log(_Ty _Left) {
        return _CSTD log(_Left);
    }

    static _Ty log1p(_Ty _Left) { // return log(1 + _Left)
        if (_Isnan(_Left) || _Left == 0 || (_Isinf(_Left) && 0 < _Left)) {
            return _Left;
        } else if (_Left < -1) {
            return _Nanv();
        } else if (_Left == -1) {
            return -_Infv();
        } else if (_Left == 0) {
            return _Left;
        } else { // compute log(1 + _Left) with fixup for small _Left
            _Ty _Leftp1 = 1 + _Left;
            return log(_Leftp1) - ((_Leftp1 - 1) - _Left) / _Leftp1;
        }
    }

    static _Ty pow(_Ty _Left, _Ty _Right) {
        return _CSTD pow(_Left, _Right);
    }

    static _Ty sin(_Ty _Left) {
        return _CSTD sin(_Left);
    }

    static _Ty sqrt(_Ty _Left) {
        return _CSTD sqrt(_Left);
    }

    static _Ty tan(_Ty _Left) {
        return _CSTD tan(_Left);
    }

    static _Ty hypot(_Ty _Left, _Ty _Right) {
        return _CSTD hypot(_Left, _Right);
    }
};
*/
