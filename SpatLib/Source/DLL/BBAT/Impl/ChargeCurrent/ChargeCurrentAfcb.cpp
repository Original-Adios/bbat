#include "StdAfx.h"
#include "ChargeCurrentAfcb.h"
#include "ModeSwitch.h"

//
IMPLEMENT_RUNTIME_CLASS(CChargeCurrent)

CChargeCurrent::CChargeCurrent(void)
{
}
CChargeCurrent::~CChargeCurrent(void)
{
}

SPRESULT CChargeCurrent::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return ChargeAction(m_eCurrentType);
}

BOOL CChargeCurrent::LoadXMLConfig(void)
{
    int nIndex = GetComboxValue(lpwVoltage, 2, L"Option:PowerVoltage");
    if (nIndex == 2)
    {
        return FALSE;
    }
    m_eVoltage = (ChargeVoltage)nIndex;
    nIndex = GetComboxValue(m_lpwCurrentType, 3, L"Option:CurrentType");
    if (nIndex == 3)
    {
        return FALSE;
    }
    m_eCurrentType = (BASE_CURRENT_TYPE)nIndex;
    m_dCurrentDown = GetConfigValue(L"Option:DownCurrent", 0.0);
    m_dCurrentUp = GetConfigValue(L"Option:UpCurrent", 0.0);
    m_nDeepSleep_Delay = GetConfigValue(L"Param:DeepSleep_Delay", 6000);
    return TRUE;
}

SPRESULT CChargeCurrent::ChargeAction(BASE_CURRENT_TYPE eCurrentType)
{
    switch (eCurrentType)
    {
    case BASE_POWERON_CURRENT:
        CHKRESULT(AFCB_PowerCurrentGet());
        break;
    case BASE_CHARGE_CURRENT:
        CHKRESULT(Phone_ChargeOpen());
        Sleep(500);
        CHKRESULT(AFCB_PowerCurrentGet());
        CHKRESULT(Phone_ChargeClose());
        break;
    case BASE_SLEEP_CURRENT:
        m_pModeSwitch->DeepSleep();
        CHKRESULT(PowerOn(FALSE));
        Sleep(m_nDeepSleep_Delay);
        CHKRESULT(AFCB_PowerCurrentGet());
        CHKRESULT(PowerOn(TRUE));
        break;
    default:
        break;
    }
    return SP_OK;
}

SPRESULT CChargeCurrent::Phone_ChargeOpen()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_ChargeOpen(m_hDUT),
        "Phone: Charge Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: Charge Open", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CChargeCurrent::AFCB_PowerCurrentGet()
{
    float fCurrent = 0.0;
    if (Charge_4V == m_eVoltage)
    {
        RETURNSPRESULT(m_pObjFunBox->FB_Power_Supply_GetCurrent_X(&fCurrent), "G3: 4V Current Get");
    }
    else if (Charge_12V == m_eVoltage)
    {
        RETURNSPRESULT(m_pObjFunBox->FB_Charge_GetCurrent12_X(&fCurrent), "G3: 12V Current Get");
    }
    else
    {
        LogFmtStrA(SPLOGLV_ERROR, "PowerVoltage setting error!");
        return SP_E_FAIL;
    }

    _UiSendMsg("Power Current", LEVEL_ITEM,
        m_dCurrentDown, (double)fCurrent, m_dCurrentUp,
        nullptr, -1, nullptr,
        "Current = %.2f", fCurrent);
    if (!IN_RANGE(m_dCurrentDown, (double)fCurrent, m_dCurrentUp))
    {
        return SP_E_BBAT_VALUE_FAIL;
    }

    return SP_OK;
}

SPRESULT CChargeCurrent::Phone_ChargeClose()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_ChargeClose(m_hDUT),
        "Phone: Charge Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: Charge Close", LEVEL_ITEM);

    return SP_OK;
}
