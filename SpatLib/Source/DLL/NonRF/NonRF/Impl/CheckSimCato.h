#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckSimCato : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckSimCato)
public:
    CCheckSimCato(void);
    virtual ~CCheckSimCato(void);

private:
    virtual SPRESULT __PollAction(void);
};
