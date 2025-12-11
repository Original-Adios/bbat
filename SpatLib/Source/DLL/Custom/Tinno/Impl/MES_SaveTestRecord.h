#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_SaveTestRecord : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_SaveTestRecord)
public:
    CMES_SaveTestRecord(void);
    virtual ~CMES_SaveTestRecord(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
