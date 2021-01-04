#pragma once
#include "binary_filer.h"

class _CHashable
{
public:
	~_CHashable() 
	{
	}
	_CHashable():m_HashKey(0),m_IsRegistered(false),m_HashDetermined(UNDEFINED) {}
	operator long()
	{
		if(!HashIsDetermined())
		{
			m_HashKey=DetermineHashKey();
			m_HashDetermined=DETERMINED;
		}
//		if(m_HashKey==0) ++m_HashKey;
		return m_HashKey;
	}
	//bool IsRegistered() {return m_IsRegistered;}
	//void DestroyIfUnregistered()
	//{
	//	DestroyMembers();
	//	if(!m_IsRegistered)
	//		delete this;
	//}
	bool IsRegistered() const {return m_IsRegistered;}
	bool HashIsDetermined() {return m_HashDetermined==DETERMINED || m_HashDetermined==FIXED;}
	bool HashIsFixed() {return m_HashDetermined==FIXED;}
	void InvalidateHash() 
	{
		if(m_HashDetermined!=FIXED)
			m_HashDetermined=UNDEFINED;
	}
	//bool IsRegistered() const {return m_IsRegistered;}
	//bool HashIsDetermined() {return m_HashDetermined;}
	//void InvalidateHash() {m_HashDetermined=false;}
	void ForceHash(long Hash_key)
	{
		m_HashDetermined=FIXED;
		m_HashKey=Hash_key;
	}
protected:
//	virtual void DestroyMembers() {}
	virtual void Store(_binary_filer& Filer)
	{
		Filer<<m_IsRegistered;
		Filer<<(short)m_HashDetermined;
		Filer<<m_HashKey;
	}
	virtual void Load(_binary_filer& Filer)
	{
		Filer>>m_IsRegistered;
		short Aux=0;
		Filer>>Aux;
		m_HashDetermined=(DETERM_STATE)Aux;
		Filer>>m_HashKey;
	}
	virtual long DetermineHashKey(long Core=HASH_CORE)=0;
	long m_HashKey;
	bool m_IsRegistered;
	typedef enum{UNDEFINED=0,DETERMINED,FIXED} DETERM_STATE;
	DETERM_STATE m_HashDetermined;
	friend class _CCircuit;
	friend class _CPredictionResult;
};

class _CInUseCheckpoint
{
public:

	_CInUseCheckpoint(void) : m_UsageCounter(0)
	{
	}

	~_CInUseCheckpoint(void)
	{
	}
	virtual void TouchMe() 
	{
		m_UsageCounter++;
		TouchDepended();
	}
	virtual void UnTouchMe()
	{
		m_UsageCounter--;
		UnTouchDepended();
	}
	virtual bool IsUseless() {return m_UsageCounter==0;}
protected:
	virtual void Store(_binary_filer& Filer)
	{
		if(Filer.IsShortMode())
			return;
		Filer<<m_UsageCounter;
	}
	virtual void Load(_binary_filer& Filer)
	{
		if(Filer.IsShortMode())
			return;
		Filer>>m_UsageCounter;
	}
	virtual void TouchDepended() {}
	virtual void UnTouchDepended() {}
	unsigned long m_UsageCounter;
};
