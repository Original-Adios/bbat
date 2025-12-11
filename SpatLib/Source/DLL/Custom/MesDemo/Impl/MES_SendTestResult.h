#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_SendTestResult : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_SendTestResult)
public:
    CMES_SendTestResult(void);
    virtual ~CMES_SendTestResult(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
