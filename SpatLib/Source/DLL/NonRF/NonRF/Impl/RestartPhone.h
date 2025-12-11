#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CRestartPhone : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CRestartPhone)
public:
    CRestartPhone(void);
    virtual ~CRestartPhone(void);

protected:
    virtual SPRESULT __PollAction(void);
};
