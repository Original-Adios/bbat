#pragma once
#include "ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CWriteCodes : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteCodes)
public:
    CWriteCodes(void);
    virtual ~CWriteCodes(void);

protected:
    SPRESULT __PollAction(void);
};
