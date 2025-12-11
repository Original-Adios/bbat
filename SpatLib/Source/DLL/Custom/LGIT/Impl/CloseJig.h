#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCloseJig : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCloseJig)
public:
    CCloseJig(void);
    virtual ~CCloseJig(void);

protected:
    virtual SPRESULT __PollAction(void);
};
