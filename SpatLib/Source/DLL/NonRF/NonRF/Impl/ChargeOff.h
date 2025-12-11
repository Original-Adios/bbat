#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CChargeOff : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CChargeOff)
public:
    CChargeOff(void);
    virtual ~CChargeOff(void);

private:
    virtual SPRESULT __PollAction(void);
};
