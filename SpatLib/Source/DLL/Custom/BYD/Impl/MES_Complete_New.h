#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_Complete_New : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_Complete_New)
public:
    CMES_Complete_New(void);
    virtual ~CMES_Complete_New(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
