#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CEnableArmLog : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CEnableArmLog)
public:
    CEnableArmLog(void);
    virtual ~CEnableArmLog(void);

protected:
    override BOOL LoadXMLConfig(void);
    override SPRESULT __PollAction(void);

private:
    BOOL m_bEnable;
};
