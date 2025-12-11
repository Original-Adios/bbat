#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CSwitchDC : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSwitchDC)
public:
    CSwitchDC(void);
    virtual ~CSwitchDC(void);

protected:
    virtual SPRESULT __PollAction(void);
};
