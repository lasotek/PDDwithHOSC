#pragma once

#include "stdafx.h"
#include "StrOperIdentifier.h"
//********************************_CStrIdentyfier
const string& _CStrOperIdentifier::GetStr()
{
	if(m_StrId.empty())
		CreateStrIdentyfier(m_StrId);
	return m_StrId;
}
