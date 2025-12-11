#pragma once
#include "ImpBase.h"
#include "ActionApp.h"

//////////////////////////////////////////////////////////////////////////
class CDeActiveArmLog : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CDeActiveArmLog)
public:
    CDeActiveArmLog(void);
    virtual ~CDeActiveArmLog(void);

protected:
    virtual SPRESULT __PollAction(void);

private:
    BOOL m_bEnable;
    INT  m_nArmlogTimeOut = 30000;
};
