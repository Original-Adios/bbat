#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////

enum BASE_CURRENT_TYPE
{
    BASE_POWERON_CURRENT = 0,
    BASE_CHARGE_CURRENT,
    BASE_SLEEP_CURRENT,
};

class CChargeCurrent : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CChargeCurrent)
public:
    CChargeCurrent(void);
    virtual ~CChargeCurrent(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
    LPCWSTR lpwVoltage[2] =
    {
        L"4V",
        L"12V"
    };

    LPCWSTR m_lpwCurrentType[3] =
    {
        L"PowerOnCurrent",
        L"ChargeCurrent",
        L"SleepCurrent",
    };

private:
    SPRESULT ChargeAction(BASE_CURRENT_TYPE eCurrentType);
    SPRESULT Phone_ChargeOpen();
    SPRESULT AFCB_PowerCurrentGet();
    SPRESULT Phone_ChargeClose();

    double m_dCurrentUp = 0.0;
    double m_dCurrentDown = 0.0;
    BASE_CURRENT_TYPE m_eCurrentType = BASE_POWERON_CURRENT;
    ChargeVoltage m_eVoltage = Charge_4V;
    int m_nDeepSleep_Delay = 6000;
};