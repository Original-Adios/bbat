#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class COpenJig : public CImpBase
{
    DECLARE_RUNTIME_CLASS(COpenJig)
public:
    COpenJig(void);
    virtual ~COpenJig(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL    LoadXMLConfig(void);
};
