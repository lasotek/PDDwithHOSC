#pragma once
#include "Components.h"

class _CMutualCoils : public _CComponent
{
public:
	_CMutualCoils(_CCircuit* pOwner);
	virtual ~_CMutualCoils(void);
	virtual unsigned GetNoOfPorts() const {return 4;}
	virtual unsigned GetNoOfParametes() const {return 3;}
	virtual const string& GetParameterName() const=0;
	virtual long GetHash(long Core) const=0;
	virtual void PlugInComponent(_CIntegrityTable& IntegrityTable)=0;
	unsigned short sPower(int ParamId) {return 0;}
protected:
};
