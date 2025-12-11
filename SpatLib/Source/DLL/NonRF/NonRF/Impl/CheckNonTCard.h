#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckNonTCard : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckNonTCard)
public:
    CCheckNonTCard(void);
    virtual ~CCheckNonTCard(void);

private:
    virtual SPRESULT __PollAction(void);
};
