#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CInitPhase : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CInitPhase)
public:
    CInitPhase(void);
    virtual ~CInitPhase(void);

protected:
    virtual SPRESULT __PollAction(void);
};
