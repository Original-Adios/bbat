#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckBatteryCapacity : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckBatteryCapacity)
public:
    CCheckBatteryCapacity(void);
    virtual ~CCheckBatteryCapacity(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    double m_dLowLimit;
    double m_dUppLimit;
};
