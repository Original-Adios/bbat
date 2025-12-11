#include "StdAfx.h"
#include "PowerOff.h"

IMPLEMENT_RUNTIME_CLASS(CPowerOff)
CPowerOff::CPowerOff(void)
{
}

CPowerOff::~CPowerOff(void)
{
}

SPRESULT CPowerOff::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    SPRESULT spRlt = SP_OK;
    SPRESULT spRltTemp = SP_OK;
    if (m_bPowerOff)
    {
        spRltTemp = ChargeOn(FALSE, Charge_4V);
        spRlt |= spRltTemp;
        Sleep(30);
        spRltTemp = ChargeOn(FALSE, Charge_12V);
        spRlt |= spRltTemp;
        Sleep(30);
        spRltTemp = PowerOn(FALSE);
        spRlt |= spRltTemp;
        Sleep(30);
    }
    m_pObjFunBox->TestStart(FALSE);

    return spRlt;
}

BOOL CPowerOff::LoadXMLConfig(void)
{
    m_bPowerOff = GetConfigValue(L"Option:PowerOff", 1);
    LPCWSTR lpwVoltage[2] =
    {
        L"4V",
        L"12V"
    };
    int nIndex = GetComboxValue(lpwVoltage, 2, L"Option:PowerVoltage");
    if (nIndex == 2)
    {
        return FALSE;
    }
    m_eVoltage = (ChargeVoltage)nIndex;
    return TRUE;
}
