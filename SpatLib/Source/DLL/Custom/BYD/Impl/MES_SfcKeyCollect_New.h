#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_SfcKeyCollect_New : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_SfcKeyCollect_New)
public:
    CMES_SfcKeyCollect_New(void);
    virtual ~CMES_SfcKeyCollect_New(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
