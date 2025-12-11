#pragma once
#include "SpatBase.h"

//////////////////////////////////////////////////////////////////////////
class CDummyGSM : public CSpatBase
{
    DECLARE_RUNTIME_CLASS(CDummyGSM)
public:
    CDummyGSM(void);
    virtual ~CDummyGSM(void);

protected:
    virtual SPRESULT __PollAction(void);
};
