#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CGetChipInfo : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CGetChipInfo)
public:
    CGetChipInfo(void);
    virtual ~CGetChipInfo(void);

protected:
    virtual SPRESULT __PollAction (void);
};
