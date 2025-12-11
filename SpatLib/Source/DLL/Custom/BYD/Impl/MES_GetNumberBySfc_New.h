#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_GetNumberBySfc_New : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_GetNumberBySfc_New)
public:
    CMES_GetNumberBySfc_New(void);
    virtual ~CMES_GetNumberBySfc_New(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
