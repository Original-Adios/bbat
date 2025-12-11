#pragma once
#include "ImpBase.h"

#define MAX_OUTPUT_PARAM_LEN        (1024)

//////////////////////////////////////////////////////////////////////////
class CMES_GetinfoSN : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_GetinfoSN)
public:
    CMES_GetinfoSN(void);
    virtual ~CMES_GetinfoSN(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
