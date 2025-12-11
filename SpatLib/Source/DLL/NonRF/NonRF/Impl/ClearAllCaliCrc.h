#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CClearAllCaliCrc : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CClearAllCaliCrc)
public:
    CClearAllCaliCrc(void);
    virtual ~CClearAllCaliCrc(void);

private:
    virtual SPRESULT __PollAction(void);
};
