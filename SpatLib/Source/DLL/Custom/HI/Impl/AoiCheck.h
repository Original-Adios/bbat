#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CAoiCheck : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAoiCheck)
public:
    CAoiCheck(void);
    virtual ~CAoiCheck(void);

protected:
    virtual SPRESULT __PollAction (void);
};
