#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CPreEthernet : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CPreEthernet)
public:
    CPreEthernet(void);
    virtual ~CPreEthernet(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
};