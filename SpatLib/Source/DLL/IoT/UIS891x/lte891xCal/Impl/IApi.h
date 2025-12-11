#pragma once

#include "uni_error_def.h"

class IApi
{
public:
    virtual SPRESULT Run() = 0;
    virtual void ClearData() = 0;
};
