#include "StdAfx.h"
#include "AdcAfcb.h"

//
IMPLEMENT_RUNTIME_CLASS(CAdcVoltageAfcb)

//////////////////////////////////////////////////////////////////////////
CAdcVoltageAfcb::CAdcVoltageAfcb(void)
{

}

CAdcVoltageAfcb::~CAdcVoltageAfcb(void)
{
    
}

SPRESULT CAdcVoltageAfcb::__PollAction(void)
{   
    FUNBOX_INIT_CHECK();
    return AdcAction();
}

BOOL CAdcVoltageAfcb::LoadXMLConfig(void)
{
    m_eAdcCh = (AdcChannel)GetConfigValue(L"Option:AdcChannel", 0);

    m_dUpSpec = GetConfigValue(L"Option:UpSpec", 0.0);
    m_dDownSpec = GetConfigValue(L"Option:DownSpec", 0.0);
    return TRUE;
}

SPRESULT CAdcVoltageAfcb::AdcAction()
{
    CHKRESULT(AdcEnable(TRUE));
    SPRESULT Res = SP_OK;
    for (int i = 0; i < 3; i++)
    {
        RETRY_PRINT(i, "Get Voltage Retry")
        Res = GetVoltage();
        if (Res == SP_OK)
        {
            break;
        }
    }

    CHKRESULT(AdcEnable(FALSE));
    return Res;
}

SPRESULT CAdcVoltageAfcb::AdcEnable(BOOL bStatus)
{
    LPCSTR lpTemp[2] =
    {
        "G3: ADC Disable",
        "G3: ADC Enable"
    };

    RETURNSPRESULT(m_pObjFunBox->FB_Common_ADC_En_Set_X(bStatus), lpTemp[bStatus]);
    return SP_OK;
}

SPRESULT CAdcVoltageAfcb::GetVoltage()
{
    float fVoltage = 0.0;
    char szInfo[128] = { 0 };
    sprintf_s(szInfo, sizeof(szInfo), ("G3: ADC%i Get Voltage"), (int)m_eAdcCh);
    if (m_eAdcCh >= FB_ADC0 && m_eAdcCh <= FB_ADC7)
    {
        RETURNSPRESULT(m_pObjFunBox->FB_Common_ADC_GetVoltage_X((FUNBOX_ADC_INDEX)m_eAdcCh, &fVoltage), szInfo);
    }
    else
    {
        PrintErrorMsg(SP_E_BBAT_CMD_FAIL, "Wrong ADC Channel", LEVEL_ITEM);
        return SP_E_BBAT_CMD_FAIL;
    }
    m_dVoltage = (double)fVoltage;
    CHKRESULT(CheckVoltage());
    return SP_OK;
}

SPRESULT CAdcVoltageAfcb::CheckVoltage()
{
    _UiSendMsg("Check Voltage", LEVEL_ITEM,
        m_dDownSpec, m_dVoltage, m_dUpSpec,
        nullptr, -1, nullptr,
        "Voltage = %.2f", m_dVoltage);
    if (!IN_RANGE(m_dDownSpec, m_dVoltage, m_dUpSpec))
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}
