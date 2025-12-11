#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CfnMesComplete : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CfnMesComplete)
public:
    CfnMesComplete(void);
    virtual ~CfnMesComplete(void);

protected:
    virtual SPRESULT __PollAction(void);
};
