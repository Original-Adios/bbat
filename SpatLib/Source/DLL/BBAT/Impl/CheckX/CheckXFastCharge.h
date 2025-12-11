#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckXFastCharge : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckXFastCharge)
public:
    CCheckXFastCharge(void);
    virtual ~CCheckXFastCharge(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    int m_iCurrentUpSpec;
    int m_iCurrentDownSpec;
    int m_iChipCount;
    float m_fVoltage;
    int m_iDelayTime_ms;

    SPRESULT RunOpen();
    SPRESULT RunSetVBUS(float iVoltage);
    SPRESULT RunReadCurrent(FastChargeReturn* iDataReturn);
    SPRESULT RunCheckCurrent(FastChargeReturn* DataRecv);
    SPRESULT RunClose();
};

