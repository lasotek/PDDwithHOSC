#pragma once
#include <limits>
#include <complex>
#include <vector>
#include <fstream>

//#ifndef NumericType
//#define long double NmumericType 
//#endif
using namespace std;

template<class NumericType>
class VectComplex : public std::vector< std::complex<NumericType > >
{
public:
	VectComplex(size_type _Count=0):std::vector< std::complex<NumericType > >(_Count) {}
	void Write2Stream(std::iostream& file,bool AsSPolynomial=false)
	{
		size_t s=0;
		for(iterator it=begin();it!=end();it++,s++)
		{	
			if(AsSPolynomial && s!=0)
				file<<"s^"<<s<<" * ";
			std::complex<NumericType >& Value=(*it);
			NumericType re=Value.real();
			NumericType im=Value.imag();
			bool bre=re!=0.0;
			bool bim=im!=0.0;
			if(bre)
				file<<re;
			if(bre && bim  && im>0.0)
				file<<"+";
			if(im<0.0)
			{
				file<<"-";
				im=-im;
			}
			if(bim)
				file<<"j*"<<im;
			file<<endl;
				
		}
	}
};

//#define VectComplex(T) std::vector< std::complex<T > >
typedef VectComplex<long double> VectLDComplex;


//#define NumericType long double

template<class NumericType>
void laguer(VectComplex<NumericType> &a, std::complex<NumericType> &x, int &its) 
{
	const unsigned MR=8,MT=10,MAXIT=MT*MR;
	const NumericType EPS=std::numeric_limits<long double>::epsilon();
	static const NumericType frac[MR+1]={0.0,0.5,0.25,0.75,0.13,0.38,0.62,0.88,1.0};
	std::complex<NumericType> dx,x1,b,d,f,g,h,sq,gp,gm,g2;
	unsigned long m=(unsigned long)a.size()-1;
	for(int iter=1;iter<=MAXIT;iter++) 
	{ 
		its=iter;
		b=a[m];
		NumericType err=abs(b);
		d=f=0.0;
		NumericType abx=abs(x);
		for (int j=m-1;j>=0;j--)
		{ 
			f=x*f+d; 
			d=x*d+b;
			b=x*b+a[j];
			err=abs(b)+abx*err;
		}
		err *= EPS;
		if (abs(b) <= err) return; 
		g=d/b; 
		g2=g*g;
		h=g2-f/b*NumericType(2.0);
		sq=sqrt(NumericType(m-1)*(NumericType(m)*h-g2));
		gp=g+sq;
		gm=g-sq;
		NumericType abp=abs(gp);
		NumericType abm=abs(gm);
		if (abp < abm) gp=gm;
		dx=std::max(abp,abm) > 0.0 ? 
			NumericType(m)/gp : 
			std::polar(abx+1,(NumericType)(iter));
		x1=x-dx;
		if (x == x1) return; 
		if (iter % MT != 0) 
			x=x1;
		else 
			x -= frac[iter/MT]*dx;
	}
	throw("too many iterations in laguer");
}


template<class NumericType>
void zroots(VectComplex<NumericType> &a, VectComplex<NumericType> &roots, bool polish)
{
	const NumericType EPS=1.0e-14; 
	int i = 0, its = 0;
	std::complex<NumericType> x,b,c;
	auto m=a.size()-1;
	VectComplex<NumericType> ad(a);
	for (long long j=m-1;j>=0;j--) 
	{
		x=0.0; 
		VectComplex<NumericType> ad_v(j+2); 
		for(int jj=0;jj<j+2;jj++) 
			ad_v[jj]=ad[jj];
		laguer(ad_v,x,its);
		if(abs(imag(x)) <= abs(real(x))*2.0*EPS)
			x=std::complex<NumericType>(real(x),0.0);
		if(abs(real(x)) <= abs(imag(x))*2.0*EPS)
			x=std::complex<NumericType>(0.0,imag(x));
		roots[j]=x;
		b=ad[j+1]; 
		for(long long jj=j;jj>=0;jj--) 
		{
			c=ad[jj];
			ad[jj]=b;
			b=x*b+c;
		}
	}
	if (polish)
		for (int j=0;j<m;j++) 
			laguer(a,roots[j],its); 
	for (int j=1;j<m;j++) 
	{ 
		x=roots[j]; 
		for (i=j-1;i>=0;i--) 
		{
			if (real(roots[i]) <= real(x)) 
				break;
			roots[i+1]=roots[i];
		}
		roots[i+1]=x;
	}
}