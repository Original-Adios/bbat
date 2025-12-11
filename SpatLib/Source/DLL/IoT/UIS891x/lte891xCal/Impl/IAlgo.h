#pragma once

#include "cellular_def.h"
#include "uni_error_def.h"

class IAlgo
{
public:
	virtual SPRESULT Init() = 0;
	virtual SPRESULT Run() = 0;
    virtual void SetAntCa(RF_ANT_E Ant, LTE_CA_E Ca) = 0;
};
