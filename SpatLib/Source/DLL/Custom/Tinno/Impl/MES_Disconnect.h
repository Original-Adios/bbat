#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_Disconnect : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_Disconnect)
public:
    CMES_Disconnect(void);
    virtual ~CMES_Disconnect(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
