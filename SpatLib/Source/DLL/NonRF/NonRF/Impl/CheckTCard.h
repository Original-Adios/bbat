#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckTCard : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckTCard)
public:
    CCheckTCard(void);
    virtual ~CCheckTCard(void);

private:
    virtual SPRESULT __PollAction(void);
};
