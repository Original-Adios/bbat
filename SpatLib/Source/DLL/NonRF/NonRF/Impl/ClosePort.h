#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CClosePort : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CClosePort)
public:
    CClosePort(void);
    virtual ~CClosePort(void);

protected:
    virtual SPRESULT __PollAction(void);
};
