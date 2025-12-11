#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CQueryCurrMode : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CQueryCurrMode)
public:
    CQueryCurrMode(void);
    virtual ~CQueryCurrMode(void);

protected:
    virtual SPRESULT __PollAction(void);
};
