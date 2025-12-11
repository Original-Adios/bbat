#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CLockSeq : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLockSeq)
public:
    CLockSeq(void);
    virtual ~CLockSeq(void);

protected:
    virtual SPRESULT __PollAction (void);
};
