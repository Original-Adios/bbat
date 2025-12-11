#pragma once
#include "ImpBase.h"
#include <string>


//////////////////////////////////////////////////////////////////////////
class CWriteRtc : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteRtc)
public:
    CWriteRtc(void);
    virtual ~CWriteRtc(void);

protected:
    virtual SPRESULT __PollAction (void);
};
