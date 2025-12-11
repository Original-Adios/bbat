#pragma once
#include "SpatBase.h"

//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

    SPRESULT SendAT(
        LPCSTR lpszAT, std::string& strRsp, 
        uint32 u32RetryCount = 3, uint32 u32Interval = 200, uint32 u32TimeOut = TIMEOUT_3S
        );
};
