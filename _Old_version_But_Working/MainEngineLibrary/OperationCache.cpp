#include "StdAfx.h"
#include "OperationCache.h"
#include "PrjException.h"
#include "Circuit.h"


//void _COperationCache::CacheTheResult(_COperationDataContainer* pContainer)
//{
//	_COperationDataContainer* pOldResult=FindExisting(pContainer);
//	if(pOldResult!=NULL)
//		RISEPDD(eIllegalOperation,"_COperationCache::CacheTheResult: the result already exist.\n"
//			                    "Use FindResult before a determination of the result.");
//	PushInCache(pContainer);
//}
//
//_CSExpandedVertices* _COperationCache::FindResult(_COperationDataContainer* pContainer)
//{
//	_COperationDataContainer* pResContainer=FindExisting(pContainer);
//	//return pResContainer==NULL?StaticEmptyVertices:pResContainer->GetVertexResult();
//	return pResContainer==NULL?NULL:pResContainer->GetVertexResult();
//}
//
//_CSExpandedVertices* _COperationCache::FindOrRegisterResult(_COperationDataContainer* pContainer)
//{
//	_COperationDataContainer* pResContainer=FindExisting(pContainer);
//	if(pResContainer==NULL)
//	{
//		pResContainer=pContainer->AllocateCopy();
//		ForceIntoCache(pResContainer);
//	}
//	return pResContainer->GetVertexResult();
//}
//
//void _COperationCache::RegisterResult(_COperationDataContainer* pContainer)
//{
//	_COperationDataContainer* pResContainer=pContainer->AllocateCopy();
//	ForceIntoCache(pResContainer);
//}

long _CRealGraphDevelopement::DetermineHashKey(long Core)
{
	long long Res=Core;
	Res*=HASH_FACTOR;
	Res^=m_InCofId;
	Res*=HASH_FACTOR;
	Res=m_Leaves.GetHash(Res);
	Res*=HASH_FACTOR;
	Res^=(long)(*m_ModelVetex);
	Res*=HASH_FACTOR;
	Res=Hash_value(Res,m_SocketName);
	return (long)Res;
}

bool _CRealGraphDevelopement::IsEqualTo(const _COperationDataContainer& Right)
{
	const _CRealGraphDevelopement& RRight=*dynamic_cast<const _CRealGraphDevelopement*>(&Right);
	if(m_InCofId!=RRight.m_InCofId)
		return false;
	if(m_Leaves!=RRight.m_Leaves)
		return false;
	if(m_ModelVetex!=RRight.m_ModelVetex)
		return false;
	if(m_SocketName!=RRight.m_SocketName)
		return false;
	return true;
}

