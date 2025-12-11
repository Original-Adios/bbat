#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CDisConnect : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CDisConnect)
public:
    CDisConnect(void);
    virtual ~CDisConnect(void);

protected:
    virtual SPRESULT __PollAction(void);
};
