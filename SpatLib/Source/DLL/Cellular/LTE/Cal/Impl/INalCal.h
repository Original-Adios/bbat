#pragma once

#include "INal.h"

using namespace std;

class INalCal : public INAL
{
public:
    virtual SPRESULT GetConfig() = 0;
    virtual SPRESULT InitResult() = 0;
    virtual SPRESULT ClearResult() = 0;
    virtual SPRESULT SetResult() = 0;

    virtual BOOL CheckScan() = 0;
};
