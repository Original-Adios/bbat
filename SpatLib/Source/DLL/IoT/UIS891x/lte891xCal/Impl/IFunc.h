#pragma once
#include "uni_error_def.h"

class IFunc
{
public:
	virtual SPRESULT PreInit() = 0;
    virtual ~IFunc(void)=0 {};
};
