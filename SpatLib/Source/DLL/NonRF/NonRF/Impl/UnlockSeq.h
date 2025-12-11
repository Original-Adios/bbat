#pragma once
#include "ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CUnlockSeq : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUnlockSeq)
public:
    CUnlockSeq(void);
    virtual ~CUnlockSeq(void);

protected:
    virtual SPRESULT __PollAction (void);
};
