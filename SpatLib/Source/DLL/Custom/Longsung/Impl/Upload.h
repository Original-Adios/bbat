#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CUpload : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUpload)
public:
    CUpload(void);
    virtual ~CUpload(void);

protected:
    virtual SPRESULT __PollAction(void);
};
