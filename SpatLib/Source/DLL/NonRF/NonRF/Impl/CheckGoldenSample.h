#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckGoldenSample : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckGoldenSample)
public:
    CCheckGoldenSample(void);
    virtual ~CCheckGoldenSample(void);

private:
    SPRESULT __PollAction(void);
};
