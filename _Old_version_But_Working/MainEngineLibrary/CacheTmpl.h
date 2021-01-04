#pragma once
#include "stdafx.h"
#include <hash_map>
#include <list>
#include "PrjException.h"
#include "binary_filer.h"

using namespace std;
using namespace stdext;

template<typename Type>
long GetHash(Type& Obj) {return  (unsigned long)Obj;}


template<class StoredObject>   //StoredClass - Should be pointer at heap 
class /*NOVTTABLE*/ _CCacheTmpl
{
protected:
	typedef pair<long long, StoredObject*> OBJECT;
	typedef hash_multimap<long,OBJECT> HASH2OBJECT_MAP;
	typedef typename HASH2OBJECT_MAP::size_type size_type;
	typedef typename HASH2OBJECT_MAP::iterator  HASH2OBJECT_MAP_ITERATOR;
	typedef typename HASH2OBJECT_MAP::const_iterator  HASH2OBJECT_MAP_C_ITERATOR;
	typedef pair<typename HASH2OBJECT_MAP_C_ITERATOR, typename HASH2OBJECT_MAP_C_ITERATOR> HASH2OBJECT_MAP_BOUNDS;
	HASH2OBJECT_MAP m_Hash2ObjectMap;
	typedef hash_map<long long,StoredObject*> INDEX2OBJECT_MAP;
	typedef typename INDEX2OBJECT_MAP::iterator INDEX2OBJECT_MAP_ITERATOR;
	typedef typename INDEX2OBJECT_MAP::const_iterator INDEX2OBJECT_MAP_C_ITERATOR;
	typedef typename INDEX2OBJECT_MAP::reverse_iterator INDEX2OBJECT_MAP_R_ITERATOR;
	INDEX2OBJECT_MAP m_Index2ObjectMap;
	long long m_Id;
public:
	virtual void ProcessIndex(long long Index, StoredObject* pObject) {}
	StoredObject* FindExisting(StoredObject* pObject) 
	{
		long HashValue=GetHash(*pObject);
		HASH2OBJECT_MAP_BOUNDS b_it=m_Hash2ObjectMap.equal_range(HashValue);
		for(HASH2OBJECT_MAP_C_ITERATOR it=b_it.first;it!=b_it.second;it++)
		{
			if(*(it->second.second)==*pObject)
				return it->second.second;
		}
		return NULL;
	}

	_CCacheTmpl():m_Id(0)
	{
	}
	~_CCacheTmpl()
	{	
		for(INDEX2OBJECT_MAP_ITERATOR it=m_Index2ObjectMap.begin();it!=m_Index2ObjectMap.end();it++)
			delete (StoredObject*)it->second;
	}
	StoredObject* PushInCache(StoredObject*& pObject,bool DeleteIfExist=false)
	{
		StoredObject* pOldObject=FindExisting(pObject);
		if(pOldObject!=NULL)
		{
			if(DeleteIfExist)
			{
				delete pObject;
				pObject=NULL;
			}
			return pOldObject;
		}

		ProcessIndex(ForceIntoCache(pObject),pObject);
		return pObject;
	}
	long long ForceIntoCache(StoredObject* pObject)
	{
		long HashValue=GetHash(*pObject);
		m_Hash2ObjectMap.insert(HASH2OBJECT_MAP::value_type(HashValue,OBJECT(m_Id,pObject)));
		m_Index2ObjectMap.insert(INDEX2OBJECT_MAP::value_type(m_Id,pObject));
		return m_Id++;
	}
	StoredObject*& GetAt(long long Index)
	{
		INDEX2OBJECT_MAP_ITERATOR x2t_mit=m_Index2ObjectMap.find(Index);
		if(x2t_mit==m_Index2ObjectMap.end())
		{
			static StoredObject* null=NULL;
			return null;
			//char Buf[20];
			//sprintf_s(Buf,sizeof(Buf)," %I64i ",Index);
			//throw out_of_range((string("Index")+Buf+"is out of range").c_str());
		}
		else
			return x2t_mit->second;
	}
	StoredObject*& operator[](long long Index) {return GetAt(Index);}
	StoredObject* operator[](long long Index) const {return GetAt(Index);}
	bool RemoveByIndex(long long Index,bool Delete)
	{
		INDEX2OBJECT_MAP_ITERATOR x2t_mit=m_Index2ObjectMap.find(Index);
		if(x2t_mit==m_Index2ObjectMap.end())
			return false;
		long HashValue=GetHash(*x2t_mit->second);
		HASH2OBJECT_MAP_ITERATOR lh2o_it=m_Hash2ObjectMap.lower_bound(HashValue),
			uh2o_it=m_Hash2ObjectMap.upper_bound(HashValue);
		for(HASH2OBJECT_MAP_ITERATOR h2o_it=lh2o_it;h2o_it!=uh2o_it;h2o_it++)
			if(h2o_it->second.first==Index)
			{
				if(Delete)
					delete x2t_mit->second;
				m_Index2ObjectMap.erase(x2t_mit);
				m_Hash2ObjectMap.erase(h2o_it);
				return true;
			}
		ASSERTPDD(true);
		return false;
	}
	long long GetIndexOf(StoredObject* pObject, bool ByContents)
	{
		long HashValue=GetHash(*pObject);
		HASH2OBJECT_MAP_ITERATOR lh2o_it=m_Hash2ObjectMap.lower_bound(HashValue),
			uh2o_it=m_Hash2ObjectMap.upper_bound(HashValue);
		for(HASH2OBJECT_MAP_ITERATOR h2o_it=lh2o_it;h2o_it!=uh2o_it;h2o_it++)
		{
			if(ByContents)
			{
				if(*(h2o_it->second.second)==*pObject)
					return h2o_it->second.first;
			}
			else
			{
				if((h2o_it->second.second)==pObject)
					return h2o_it->second.first;
			}
		}
		RISEPDD(out_of_range,"Cannot find the Index for this object");
	}
	//long long GetIndexOf(const StoredObject* pObject)
	//{
	//	long HashValue=GetHash(*pObject);
	//	HASH2OBJECT_MAP_ITERATOR lh2o_it=m_Hash2ObjectMap.lower_bound(HashValue),
	//		uh2o_it=m_Hash2ObjectMap.upper_bound(HashValue);
	//	for(HASH2OBJECT_MAP_ITERATOR h2o_it=lh2o_it;h2o_it!=uh2o_it;h2o_it++)
	//	{
	//		if((h2o_it->second.second)==pObject)
	//			return h2o_it->second.first;
	//	}
	//	RISEPDD(out_of_range,"Cannot find the Index for this object");
	//}
	long long PushInCacheIndex(StoredObject*& pObject,bool DeleteIfExist=false)
	{
		return GetIndexOf(pObject=PushInCache(pObject,DeleteIfExist),true);
	}
	template<class _Pred>
	void RemoveByCondition(_Pred Pred)
	{
		for(HASH2OBJECT_MAP_ITERATOR h2o_it=m_Hash2ObjectMap.begin(),_h2o_e=m_Hash2ObjectMap.end();h2o_it!=h2o_e;)
			if(Pred(*(h2o_it->second.second)))
			{
				INDEX2OBJECT_MAP_ITERATOR if_it=m_Index2ObjectMap.find(h2o_it->second.first);
				ASSERTPDD(if_it!=m_Index2ObjectMap.end());
				m_Index2ObjectMap.erase(if_it);
				delete (*StoredObject) h2o_it->second.second;
				h2o_it=m_Hash2ObjectMap.erase(h20_it);
			}
			else
				h2o_it++;
	}
	bool RemoveByContent(StoredObject& PaternObject)
	{
		long HashValue=GetHash(PaternObject);
		HASH2OBJECT_MAP_ITERATOR lh2o_it=m_Hash2ObjectMap.lower_bound(HashValue),
			uh2o_it=m_Hash2ObjectMap.upper_bound(HashValue);
		for(HASH2OBJECT_MAP_ITERATOR h2o_it=lh2o_it;h2o_it!=uh2o_it;h2o_it++)
		{
			if(*(h2o_it->second.second)==PaternObject)
			{
				INDEX2OBJECT_MAP_ITERATOR x2o_it=m_Index2ObjectMap.find(h2o_it->second.first);
				ASSERTPDD(x2o_it!=m_Index2ObjectMap.end());
				delete (StoredObject*)x2o_it->second;
				m_Hash2ObjectMap.erase(h2o_it);
				m_Index2ObjectMap.erase(x2o_it);
				return true;
			}
		}
		return false;
	}
	void ClearGarbage()
	{
		for(HASH2OBJECT_MAP_ITERATOR h2o_it=m_Hash2ObjectMap.begin();
			h2o_it!=m_Hash2ObjectMap.end();)
		{
			if(h2o_it->second.second->IsUseless())
			{
				INDEX2OBJECT_MAP_ITERATOR x2o_it=m_Index2ObjectMap.find(h2o_it->second.first);
				ASSERTPDD(x2o_it!=m_Index2ObjectMap.end());
				delete (StoredObject*)x2o_it->second;
				HASH2OBJECT_MAP_ITERATOR rh2o_it=h2o_it;
				h2o_it++;
				m_Hash2ObjectMap.erase(rh2o_it);
				m_Index2ObjectMap.erase(x2o_it);
			}
			else
				h2o_it++;
		}
	}
	void Clear(bool FreeObjects=true)
	{
		if(FreeObjects)
			for(INDEX2OBJECT_MAP_R_ITERATOR rit=m_Index2ObjectMap.rbegin();rit!=m_Index2ObjectMap.rend();rit++)
				delete (StoredObject*)rit->second;
		m_Index2ObjectMap.clear();
		m_Hash2ObjectMap.clear();
		m_Id=0;
	}
	size_type size() const {return m_Index2ObjectMap.size();}
	long long MaxId() {return m_Id;}
};


template<class StoredObject>   
class  _CStreamableCacheTmpl : public _CCacheTmpl<StoredObject>,public _streamable,public _co_streamable
{
public:
	~_CStreamableCacheTmpl()
	{
	}
	_CStreamableCacheTmpl(bool StoreInReversOrder=false):m_StoreInReversOrder(StoreInReversOrder),
	m_LowerBound(0) {}
	void DoSomeExtras(_streamable* pMe) {}
	//void SetLowerBound(long long LowerBound=0) {m_LowerBound=LowerBound; }
	void Store(_binary_filer &Filer, long long lower_bound)
	{
		m_LowerBound=lower_bound;
		Store(Filer);
		m_LowerBound=0;
	}
protected:
	long long m_LowerBound;
	bool m_StoreInReversOrder;
	virtual void Store(_binary_filer &Filer/*, long long lower_bound=0*/)
	{
		Filer<<m_Index2ObjectMap.size();
		for(long long Index=m_StoreInReversOrder?m_Id-1:m_LowerBound;
			m_StoreInReversOrder?Index>=m_LowerBound:Index<m_Id;m_StoreInReversOrder?Index--:Index++)
		{
			INDEX2OBJECT_MAP_ITERATOR it=m_Index2ObjectMap.find(Index);
			if(it!=m_Index2ObjectMap.end())
			{
				Filer<<it->first;
				Filer<<*(it->second);
			}
		}
		Filer<<m_Id;
		_streamable::Store(Filer);
		m_LowerBound=0;
	}

	virtual void Load(_binary_filer& Filer)
	{
		Clear();
		INDEX2OBJECT_MAP::size_type size;
		Filer>>size;
		for(INDEX2OBJECT_MAP::size_type i=0;i<size;i++)
		{
			long long Index;
			Filer>>Index;
			StoredObject* pObject=dynamic_cast<StoredObject*>(Filer.RetriveDynamicClass(this));
			long HashValue=GetHash(*pObject);
			m_Hash2ObjectMap.insert(HASH2OBJECT_MAP::value_type(HashValue,OBJECT(Index,pObject)));
			m_Index2ObjectMap.insert(INDEX2OBJECT_MAP::value_type(Index,pObject));
		}
		Filer>>m_Id;
		_streamable::Load(Filer);
	}
};

