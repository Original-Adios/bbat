#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_CheckFlow : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_CheckFlow)
public:
    CMES_CheckFlow(void);
    virtual ~CMES_CheckFlow(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
