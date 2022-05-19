#include "StdAfx.h"
#include "norm_float.h"
#include <complex>
#include <math.h>
#include "PrjException.h"
#include "time_stamp.h"

NumericType EmptyFloat=0.0;

bool IsEqual(long double Left, long double Right)
{
	return abs(Left-Right)<=numeric_limits<long double>::epsilon();
}

template<>
const NumericType& GetConstZero(/*const NumericType& Dummy*/)
{
	return EmptyFloat;
}

_CSparsePolynomial ZeroSparsePolynomial;

template<>
const _CSparsePolynomial& GetConstZero(/*const _CSparsePolynomial& Dummy*/)
{
	return ZeroSparsePolynomial;
}

constexpr norm_float::norm_float(long double ld,long Mantise) : m_Base(ld),m_Mantise(Mantise)
{
	normalize();
}

//norm_float::~norm_float(void)
//{
//}

constexpr norm_float& norm_float::normalize()
{
	if(m_Base!=0.0)
	{
		while(m_Base<=-10 || m_Base>=10.0)
		{
			m_Base/=10.0;
			m_Mantise++;
		}
		while(m_Base>-1.0 && m_Base<1.0)
		{
			m_Base*=10;
			m_Mantise--;
		}
	}
	else
		m_Mantise=0;
	return *this;
}
norm_float& norm_float::de_normalize(long DesiredMantise)
{
	if(m_Base!=0.0)
	{
		while(m_Mantise<DesiredMantise)
		{
			m_Base/=10;
			m_Mantise++;
		}
		while(m_Mantise>DesiredMantise)
		{
			m_Base*=10;
			m_Mantise--;
		}
	}
	else
		m_Mantise=DesiredMantise;
	return *this;
}

norm_float& norm_float::operator=(const norm_float& Source)
{
	m_Base=Source.m_Base;
	m_Mantise=Source.m_Mantise;
	return *this;
}
norm_float& norm_float::operator=(long double Value)
{
	m_Base=Value;
	m_Mantise=0;
	normalize();
	return *this;
}

void norm_float::Add(const norm_float& Right)
{
	if(IsEqual(m_Base,0.0))
	{
		m_Base=Right.m_Base;
		m_Mantise=Right.m_Mantise;
	}
	else if(!IsZero(Right))
	{
		if(m_Mantise>Right.m_Mantise)
		{
			norm_float R(Right);
			R.de_normalize(m_Mantise);
			m_Base+=R.m_Base;
		}
		else
		{
			de_normalize(Right.m_Mantise);
			m_Base+=Right.m_Base;
		}
		normalize();
	}
}

void norm_float::Subtract(const norm_float& Right)
{
	if(IsEqual(m_Base,0.0))
	{
		m_Base=-Right.m_Base;
		m_Mantise=Right.m_Mantise;
	}
	else if(!IsZero(Right))
	{
		if(m_Mantise>Right.m_Mantise)
		{
			norm_float R(Right);
			R.de_normalize(m_Mantise);
			m_Base-=R.m_Base;
		}
		else
		{
			de_normalize(Right.m_Mantise);
			m_Base-=Right.m_Base;
		}
		normalize();
	}
}

void norm_float::Multiply(const norm_float& Right)
{
	m_Base*=Right.m_Base;
	m_Mantise+=Right.m_Mantise;
	normalize();
}

void norm_float::Divide(const norm_float& Right)
{
	if(IsEqual(Right.m_Base,0.0))
		RISEPDD(eIllegalOperation,"Division by 0");
	m_Base/=Right.m_Base;
	m_Mantise-=Right.m_Mantise;
	normalize();
}

norm_float norm_float::operator+(const norm_float& R) const
{
	norm_float L(*this);
	L+=R;
	return L;
}

norm_float norm_float::operator-(const norm_float& R) const
{
	norm_float L(*this);
	L-=R;
	return L;
}

norm_float norm_float::operator*(const norm_float& R) const
{
	norm_float L(*this);
	L*=R;
	return L;
}

norm_float norm_float::operator/(const norm_float& R) const
{
	norm_float L(*this);
	L/=R;
	return L;
}
long double norm_float::ToLD(long Shift) const
{
	norm_float Res(*this);
	Res.m_Mantise+=Shift;
	Res.de_normalize(0);
	return Res.m_Base;
}

template<>
bool IsOne(const norm_float& Value)
{
	return Value.IsOne();
}

//bool operator==(const norm_float& Right,const norm_float& Left)
//{
//	return Right.operator==(Left);
//}
//
//bool operator!=(const norm_float& Right,const norm_float& Left)
//{
//	return !Right.operator==(Left);
//}
//
//bool operator<(const norm_float& Right,const norm_float& Left)
//{
//	return Right.operator<(Left);
//}
//
//bool operator<=(const norm_float& Right,const norm_float& Left)
//{
//	return Right.operator<=(Left);
//}
//
//bool operator>(const norm_float& Right,const norm_float& Left)
//{
//	return Right.operator>(Left);
//}
//
//bool operator>=(const norm_float& Right,const norm_float& Left)
//{
//	return Right.operator>=(Left);
//}
//
//bool operator==(const norm_float& A, const norm_float& B)
//{
//	return A.operator ==(B);
//}
//bool operator!=(const norm_float& A, const norm_float& B)
//{
//	return !operator==(A,B);
//}

string norm_float::ToString() const
{
	char Buf[255];
	if(m_Mantise==0)
		sprintf_s(Buf,sizeof(Buf),"%lf",m_Base);
	else
		sprintf_s(Buf,sizeof(Buf),"%lfe%ld",m_Base,m_Mantise);
	return Buf;
}

string ToString(const long double& Value)
{
	char Buf[255];
	sprintf_s(Buf,sizeof(Buf),"%lf",Value);
	return Buf;
}

string ToString(const norm_float& Value)
{
	return Value.ToString();
}

short norm_float::Comp(const norm_float& Right) const
{
	if(IsEqual(m_Base,0.0) && IsEqual(Right.m_Base,0.0))
		return 0;
	short m_sgn=sgn(*this),
		r_sgn=sgn(Right);
	if(IsEqual(m_Base,0.0))
		return -r_sgn;
	if(IsEqual(Right.m_Base,0.0))
		return m_sgn;
	if(m_sgn!=r_sgn)
		return m_sgn<r_sgn?-1:1;
	short r=0;
	if(m_Mantise<Right.m_Mantise)
		r=-1;
	else if(m_Mantise>Right.m_Mantise)
		r=1;
	if(r!=0)
	{
		if(m_sgn<0)
			r=-r;
		return r;
	}
	if(IsEqual(m_Base,Right.m_Base))
		return 0;
	else if (m_Base<Right.m_Base)
		return -1;
	else return 1;
}

txt_stream& operator<<(txt_stream& stream, const norm_float& Value)
{
	const long double& Base=Value.m_Base;
	const long& Mantise=Value.m_Mantise;
	stream<<Base;
	if(Mantise!=0)
		stream<<"e"<<Mantise;
	return stream;
}

_binary_filer& operator<<(_binary_filer& filer, const norm_float& Value)
{
	filer<<Value.m_Mantise<<Value.m_Base;
	return filer;
}

_binary_filer& operator>>(_binary_filer& filer, norm_float& Value)
{
	filer>>Value.m_Mantise>>Value.m_Base;
	return filer;
}


long long GetTypedHash(long long& Core, const norm_float& Value)
{
	Core*=HASH_FACTOR;
	Core=GetTypedHash(Core,Value.m_Base);
	Core*=HASH_FACTOR;
	Core^=Value.m_Mantise;
	return Core;
}

short sgn(const norm_float& Value)
{
	return sgn(Value.m_Base);
}

norm_float abs(const norm_float& Value)
{
	return norm_float(abs(Value.m_Base),Value.m_Mantise);
}

long norm_float::CommonMantise(const norm_float& Other, bool Sqrtable) const
{
	
	long oMantise=Other.m_Mantise;
	long MaxMantise=max(m_Mantise,oMantise);
	long MinMantise=min(m_Mantise,oMantise);
	long CommonMantise=0;
	if((MaxMantise-MinMantise) > (LDBL_MAX_10_EXP+LDBL_MIN_10_EXP))
		 CommonMantise=MaxMantise;
	else
		CommonMantise=(MaxMantise-MinMantise)>>1;
	if(Sqrtable && (CommonMantise & 1L)!=0)
		CommonMantise &=-1L;
	return CommonMantise;
}

norm_float sqrt(const norm_float& Value)
{
	long NewMantise=Value.m_Mantise;
	norm_float NewValue(Value);
	if((NewMantise & 1L)!=0)
	{
		NewMantise &= ~1L;
		NewValue.de_normalize(NewMantise);
	}
	NewValue.m_Base=sqrt(NewValue.m_Base);
	NewValue.m_Mantise/=2;
	NewValue.normalize();
	return NewValue;
}

norm_float abs(const NormFloatComplex& Value)
{
	norm_float& R=Value.real();
	norm_float& I=Value.imag();
	if(IsZero(R))
		return abs(I);
	if(IsZero(I))
		return abs(R);
	return sqrt(R*R+I*I);
}

norm_float arg(const NormFloatComplex& Value)
{
	norm_float& R=Value.real();
	norm_float& I=Value.imag();
	norm_float& ATAN=I/R;
	long double phase=atan(ATAN.ToLD());
	if(R<0.0)
		if(I<0)
			phase=phase-3.1415926535897932384626433832795028842L;
		else
			phase=phase+3.1415926535897932384626433832795028842L;
	return phase;
}

//NumericType _CSPolynomial::operator[](size_t Index) const
//{
//	return out_of_scope(Index)?EmptyFloat:vector<NumericType>::at(Index);
//}
//
//NumericType& _CSPolynomial::operator[](size_t Index)
//{
//	return force_at(Index);
//}
//_CSPolynomial& _CSPolynomial::operator+=(const _CSPolynomial& Right)
//{
//	size_t N=max(size(),Right.size());
//	for(size_t i=0;i<N;i++)
//		force_at(i)+=Right[i];
//	return *this;
//}
//_CSPolynomial& _CSPolynomial::operator-=(const _CSPolynomial& Right)
//{
//	size_t N=max(size(),Right.size());
//	for(size_t i=0;i<N;i++)
//		force_at(i)-=Right[i];
//	return *this;
//}
//
//_CSPolynomial& _CSPolynomial::operator*=(const _CSPolynomial& Right)
//{
//	ASSERTPDD(size()>0 && Right.size()>0);
//	if(NeedFFT())
//		FFTMultiplication(Right);
//	else
//		NaivMultiplication(Right);
//	return *this;
//}
//
//_CSPolynomial& _CSPolynomial::MultiplyByS(unsigned short sPower)
//{
//	if(sPower>0)
//		insert(begin(),sPower,0.0);
//	return *this;
//}
//
//_CSPolynomial& _CSPolynomial::MultiplyByConst(NumericType Value)
//{
//	for(iterator it=begin(),_e=end();it!=_e;it++)
//	{
//		NumericType& Item=*it;
//		if(Item!=0.0)
//			Item*=Value;
//	}
//	return *this;
//}
//
//void _CSPolynomial::NaivMultiplication(const _CSPolynomial& Right)
//{
//	time_stamp Stamp("Polynomials.txt","Naive Multiplication");
//	size_t SL=size(), SR=Right.size();
//	if(SL==0 || SR==0) return;
//	size_t N=SL+SR-1;
//	_CSPolynomial OutPoly(N);
//	for(size_t i=0;i<N;i++)
//		for(size_t j=0;j<=i;j++)
//		{
//			size_t k=i-j;
//			if(j<SL && k<SR)
//				OutPoly[i]+=at(j)*Right[k];
//		}
//	assign(OutPoly.begin(),OutPoly.end());
//}
//
//void _CSPolynomial::FFTMultiplication(const _CSPolynomial& Right)
//{
//	time_stamp Stamp("Polynomials.txt","FFT Multiplication");
//	_FFT fft(*this,Right);
//	fft.DoTheMultiplication();
//}
//
//
//_CSPolynomial::_FFT::_FFT(_CSPolynomial& Left, const _CSPolynomial& Right):
//m_Left(Left), m_Right(Right),m_OneRoots(Left.size(),Right.size()) 
//{
//	//m_RealSize=m_Left.size()+m_Right.size()-1;
//	//size_t m_fftSize=1;
//	//while(m_fftSize<m_RealSize)
//	//	m_fftSize<<=1;
//	//m_LeftFFT.resize(m_fftSize);
//	//m_RightFFT.resize(m_fftSize);
//	//size_t rootSize=m_fftSize>>1;
//	//m_OneRoots.resize(rootSize);
//	//for(size_t i=0;i<rootSize;i++)
//	//{
//	//	long double Arg=2*i*M_PI/m_fftSize;
//	//	m_OneRoots[i]=polar((long double)1.0,Arg);
//	//	//m_OneRoots[i]=ComplexNumericTypeD(cos(Arg),sin(Arg));
//	//}
//}
//
//void _CSPolynomial::_FFT::DoTheMultiplication()
//{
//	size_t _size=FFTSize();
//	_fft_vector FFTLeft(_size), FFTRight(_size),OutRes(_size);
//	DetermineFFTLevel(m_Left,FFTLeft,0,0);
//	DetermineFFTLevel(m_Right,FFTRight,0,0);
//	FFTLeft*=FFTRight;
//	IDetermineFFTLevel(FFTLeft,OutRes,0,0);
//	size_t _RealSize=RealSize();
//	NumericType	ShouldBeZero=0.0;
//	for(size_t i=_RealSize;i<_size;i++)
//	{
//		ShouldBeZero=max(ShouldBeZero,abs(OutRes[i].real()/_size));
//		ShouldBeZero=max(ShouldBeZero,abs(OutRes[i].imag()/_size));
//	}
//	for(size_t i=0;i<_RealSize;i++)
//		ShouldBeZero=max(ShouldBeZero,abs(OutRes[i].imag()/_size));
//	m_Left.resize(_RealSize);
//	for(size_t i=0;i<_RealSize;i++)
//	{
//		NumericType Value=OutRes[i].real()/_size;
//		m_Left[i]=abs(Value)<=ShouldBeZero?0.0:Value;
//	}
//
//}
//
//void _CSPolynomial::_FFT::DetermineFFTLevel(const _CSPolynomial& OrgPoly, _fft_vector& ResPoly,size_t Level, size_t BaseIndex)
//{
//	size_t _size=ResPoly.size()>>1;
//	if(_size==0)
//	{
//		ResPoly[0]=OrgPoly[BaseIndex];
//		return;
//	}
//	size_t BaseIndex1=BaseIndex | ((size_t)1<<Level);
//	_fft_vector Poly0(_size), Poly1(_size);
//	DetermineFFTLevel(OrgPoly,Poly0,Level+1,BaseIndex);
//	DetermineFFTLevel(OrgPoly,Poly1,Level+1,BaseIndex1);
//	for(size_t i=0;i<_size;i++)
//	{
//		short sgn=1;
//		//ComplexNumericTypeD& Mult=m_OneRoots.get_power(sgn,i,2*Level);
//		ComplexNumericType& Mult=m_OneRoots.get_power(sgn,i,((size_t)1<<Level));
//		ResPoly[i]=Poly0[i]+(NumericType)sgn*Mult*Poly1[i];
//		ResPoly[_size+i]=Poly0[i]-(NumericType)sgn*Mult*Poly1[i];
//	}
//}
//
//void _CSPolynomial::_FFT::IDetermineFFTLevel(const _fft_vector& OrgPoly, _fft_vector& ResPoly,size_t Level, size_t BaseIndex)
//{
//	size_t _size=ResPoly.size()>>1;
//	if(_size==0)
//	{
//		ResPoly[0]=OrgPoly[BaseIndex];
//		return;
//	}
//	size_t BaseIndex1=BaseIndex | ((size_t)1<<Level);
//	_fft_vector Poly0(_size), Poly1(_size);
//	IDetermineFFTLevel(OrgPoly,Poly0,Level+1,BaseIndex);
//	IDetermineFFTLevel(OrgPoly,Poly1,Level+1,BaseIndex1);
//	for(size_t i=0;i<_size;i++)
//	{
//		short sgn=1;
//		//ComplexNumericTypeD& Mult=m_OneRoots.get_power(sgn,i,2*Level);
//		ComplexNumericType& Mult=m_OneRoots.get_power(sgn,-(int)i,((size_t)1<<Level));
//		ResPoly[i]=Poly0[i]+(NumericType)sgn*Mult*Poly1[i];
//		ResPoly[_size+i]=Poly0[i]-(NumericType)sgn*Mult*Poly1[i];
//	}
//}
//
//void _CSPolynomial::_FFT::_fft_vector::DotProduct(const _fft_vector& Right)
//{
//	ASSERTPDD(size()==Right.size());
//	iterator itl=begin();
//	const_iterator itr=Right.begin();
//	for(;itl!=end();itl++,itr++)
//		(*itl)*=(*itr);
//}
//
//_CSPolynomial::_FFT::_fft_vector_cof::_fft_vector_cof(size_t lSize, size_t rSize):
//m_RealSize(lSize+rSize-1),
//m_fftSize(round22power(lSize+rSize-1)),
////vector<ComplexNumericTypeD>(round22power(lSize+rSize-1)>>1)
//vector<ComplexNumericType>(round22power(lSize+rSize-1)>>1)
//{
//	for(size_t k=0,_size=m_fftSize>>1;k<_size;k++)
//	{
//		long double Arg=2*k*M_PI/m_fftSize;
//		at(k)=polar((long double)1.0,Arg);
//	}
//}
//
//size_t _CSPolynomial::_FFT::_fft_vector_cof::round22power(size_t size)
//{
//	size_t OutValue=1;
//	while(OutValue<size)
//		OutValue<<=1;
//	return OutValue;
//}
//
////_CSPolynomial::_FFT::ComplexNumericTypeD& _CSPolynomial::_FFT::_fft_vector_cof::get_value(short& sgn, size_t Index)
//_CSPolynomial::_FFT::ComplexNumericType& _CSPolynomial::_FFT::_fft_vector_cof::get_value(short& sgn, size_t Index)
//{
//	ASSERTPDD(Index<m_fftSize);
//	size_t Half=m_fftSize>>1;
//	sgn=Index<Half?1:-1;
//	if(Index>=Half)
//		Index-=Half;
//	return at(Index);
//}
//
bool IsZero(const NumericType& Value) {return Value==0.0;}

//IMPLEMENT_DYNAMIC_CREATION(_CSubModelNumericPatternStreamContainer);
void _CSubModelNumericPatternStreamContainer::Store(_binary_filer& Filer)
{
	string S="_CSubModelNumericPattern";
	Filer<<S;
	Filer<<m_SMNPat;
}

void _CSubModelNumericPatternStreamContainer::Load(_binary_filer& Filer)
{
	string S;
	m_SMNPat.ClearAll();
	Filer>>S;
	if(S!="_CSubModelNumericPattern")
		RISEPDD(eFilerProblem,"Unexpected library format");
	Filer>>m_SMNPat;
}

void CompValue2ModelNumPatt(_CSubModelNumericPattern& OutRes,NumericType Value, unsigned short Power)
{
	OutRes.ClearAll();
}

txt_stream& operator<<(txt_stream& stream, const _CSparsePolynomial::MagnitudeDistribution& MD)
{
	stream<<"s-Coeff Relative Magnitude Distribution"<<endl;
	stream<<"\t\tRe\t\tIm"<<endl;
	for(size_t i=0;i<MD.size();i++)
	{
		const _CSparsePolynomial::NumComplex& Entry=MD[i];
		NumericType& Re=Entry.real();
		NumericType& Im=Entry.imag();
		stream<<"s^"<<i<<" = \t\t";
		if(!IsZero(Re))
			stream<<Re;
		if(!IsZero(Im))
			stream<<"\t\t"<<Im;
		stream<<endl;
	}
	return stream;
}

txt_stream& operator<<(txt_stream& stream, const _CSparsePolynomial::EnergyDistribution& ED)
{
	stream<<"s-Coeff Relative Power Distribution"<<endl;
	for(size_t i=0;i<ED.size();i++)
	{
		const _CSparsePolynomial::NumericType& Entry=ED[i];
		stream<<"s^"<<i<<" = "<<Entry<<endl;
	}
	return stream;
}

#ifdef WITH_NORM_FLOAT
long DetermineRedPower(const _CSparsePolynomial& InPoly)
{
	int MINE=numeric_limits<long double>::min_exponent10;
	int MAXE=numeric_limits<long double>::max_exponent10;
	long minn=LONG_MAX;
	long maxn=LONG_MIN;
	for(size_t i=0;i<=InPoly.get_order();i++)
	{
		const norm_float& V=InPoly.get(i);
		long Mantise=V.GetMantise();
		minn=min(minn,Mantise);
		maxn=max(maxn,Mantise);
	}
	int RedPower=-maxn;
	if(minn+RedPower<MINE)
		RedPower=MINE-minn+5;
	if(maxn+RedPower>MAXE)
		RedPower=MAXE-maxn-5;
	return RedPower;
}
#endif

void SpPoly2Roots(const _CSparsePolynomial& InPoly,NormVectComplex& Roots)
{
	size_t Size=InPoly.get_order()+1;
	VectComplex<norm_float> ComplexInPoly(Size);
	Roots.resize(Size);
//#ifdef WITH_NORM_FLOAT
//	long Shift=DetermineRedPower(InPoly);
//	for(size_t i=0;i<Size;i++)
//	{
//		long double LDValue=InPoly.get(i).ToLD(Shift);
//		if(abs(LDValue)<=DBL_MIN)
//			LDValue=0.0;
//		ComplexInPoly[i]=LDValue;
//	}
//	size_t SizeN=Size;
//	while(SizeN>0 && ComplexInPoly[--SizeN].real()==0.0);
//	ComplexInPoly.resize(SizeN+1);
//	Roots.resize(SizeN+1);
//#else
	for(size_t i=0;i<Size;i++)
		ComplexInPoly[i]=InPoly.get(i);
//#endif
	zroots(ComplexInPoly,Roots,true);
}

//long long GetTypedHash(long long Core, const _CSubModelNumericPattern& NumPattern)
//{
//	return NumPattern.GetHash(Core);
//}

void PrepareLinAngFreqSpace(NumericVector& Space, NumericType FFrom, NumericType FTo, size_t NoIntrervals)
{
	Space.clear();
	NumericType D = (FTo - FFrom) / NoIntrervals;
	for (NumericType CV = FFrom; CV < FTo; CV += D)
		Space.push_back(CV);

}
void PrepareLogAngFreqSpace(NumericVector& Space, int MinDec, int MaxDec, size_t NoIntrervalsInDec)
{
	Space.clear();
	//long double ln10 = log(10.0L);
	long double D = 1.0L / NoIntrervalsInDec;
	for (long double CL = MinDec; CL < (long double)MaxDec; CL += D)
	{
		//long double RealValue = exp(CL*ln10);
		long double RealValue = pow(10,CL);
		Space.push_back(RealValue);
	}
}

void DetermineFreqsValues(NormVectComplex& Results, _CSparsePolynomial& PolynomialS, const NumericVector& FreqVector)
{
	Results.clear();
	int PolyOrder = PolynomialS.get_order();
	for (auto& FreqValue : FreqVector)
	{
		NumericComplex ComplexFreq(0.0L, FreqValue);
		NumericComplex Res;
		for (int PolyS = PolyOrder; PolyS >= 0; PolyS--)
		{
			Res *= ComplexFreq;
			Res += PolynomialS.get((size_t)PolyS);
		}
		Results.push_back(Res);
	}
	ASSERTPDD(Results.size() == FreqVector.size());
}
