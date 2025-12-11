#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CAssertUE : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAssertUE)
public:
    CAssertUE(void);
    virtual ~CAssertUE(void);

protected:
    virtual SPRESULT __PollAction(void);
};
