#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CFactoryReset : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CFactoryReset)
public:
    CFactoryReset(void);
    virtual ~CFactoryReset(void);

private:
    virtual SPRESULT __PollAction(void);
};
