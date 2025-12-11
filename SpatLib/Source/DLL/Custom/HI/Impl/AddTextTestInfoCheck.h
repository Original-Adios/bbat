#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CAddTextTestInfoCheck : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAddTextTestInfoCheck)
public:
    CAddTextTestInfoCheck(void);
    virtual ~CAddTextTestInfoCheck(void);

protected:
    virtual SPRESULT __PollAction (void);
};
