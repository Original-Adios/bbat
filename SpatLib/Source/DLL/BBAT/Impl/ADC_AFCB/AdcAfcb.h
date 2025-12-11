#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
enum AdcChannel
{
    ADC0,
    ADC1,
    ADC2,
    ADC3,
    ADC4,
    ADC5,
    ADC6,
    ADC7,
    MAX_ADC
};

class CAdcVoltageAfcb : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAdcVoltageAfcb)
public:
    CAdcVoltageAfcb(void);
    virtual ~CAdcVoltageAfcb(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
    SPRESULT AdcAction();
private:
    SPRESULT AdcEnable(BOOL bStatus);
    SPRESULT GetVoltage();
    SPRESULT CheckVoltage();

    double m_dVoltage = 0.0;
    AdcChannel m_eAdcCh = ADC0;
    double m_dUpSpec = 0.0;
    double m_dDownSpec = 0.0;
};