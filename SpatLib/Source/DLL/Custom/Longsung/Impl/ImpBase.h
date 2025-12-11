#pragma once
#include "SpatBase.h"
#include "../drv/SLDriver.h"

//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

    SPRESULT GetSLDriverFromShareMemory(void);
    SPRESULT SetSLDriverIntoShareMemory(void);

protected:
    CSLDriver* m_pSLDrv;
};
