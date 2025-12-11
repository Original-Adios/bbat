#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_NcComplete_New : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_NcComplete_New)
public:
    CMES_NcComplete_New(void);
    virtual ~CMES_NcComplete_New(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
