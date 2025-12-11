#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CWriteGuid : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteGuid)
public:
    CWriteGuid(void);
    virtual ~CWriteGuid(void);

protected:
    virtual SPRESULT __PollAction (void);
};
