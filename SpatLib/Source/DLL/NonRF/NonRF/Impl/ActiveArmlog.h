#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CActiveArmLog : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CActiveArmLog)
public:
    CActiveArmLog(void);
    virtual ~CActiveArmLog(void);

protected:
    virtual SPRESULT __PollAction(void);

private:
    BOOL m_bEnable;
};
