#include "StdAfx.h"
#include "CheckXFastCharge.h"

//
IMPLEMENT_RUNTIME_CLASS(CCheckXFastCharge)

//////////////////////////////////////////////////////////////////////////
CCheckXFastCharge::CCheckXFastCharge(void)
{
    m_iCurrentUpSpec = 0;
    m_iCurrentDownSpec = 0;
    m_iChipCount = 0;
    m_fVoltage = 0.0f;
    m_iDelayTime_ms = 0;
}

CCheckXFastCharge::~CCheckXFastCharge(void)
{
}

SPRESULT CCheckXFastCharge::__PollAction(void)
{
    FastChargeReturn DataReturn;
    memset(&DataReturn, 0, sizeof(DataReturn));

    CHKRESULT(RunOpen());
    CHKRESULT(RunSetVBUS(m_fVoltage));
    Sleep(m_iDelayTime_ms);
    CHKRESULT(RunReadCurrent(&DataReturn));
    CHKRESULT(RunCheckCurrent(&DataReturn));
    CHKRESULT(RunSetVBUS(5.0f));
    CHKRESULT(RunClose());

    return SP_OK;
}

BOOL CCheckXFastCharge::LoadXMLConfig(void)
{
    m_iCurrentUpSpec = GetConfigValue(L"Option:Current_UpSpec", 0);
    m_iCurrentDownSpec = GetConfigValue(L"Option:Current_DownSpec", 0);
    m_iChipCount = GetConfigValue(L"Option:Chip_Count", 0);
    m_fVoltage = (float)GetConfigValue(L"Option:Voltage_Set", 0.0f);
    m_iDelayTime_ms = GetConfigValue(L"Param:DelayTime_ms", 0);

    if (!IN_RANGE(7.5, m_fVoltage, 9.5))
    {
        SendCommonCallback(L"Voltage_Set must be in rang 7.5V-9.5V");
        return FALSE;
    }
    
    return TRUE;
}

SPRESULT CCheckXFastCharge::RunOpen()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_FastChargeOpen(m_hDUT), "FastChargeOpen", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "FastChargeOpen", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CCheckXFastCharge::RunSetVBUS(float iVoltage)
{
    SPRESULT res = SP_OK;
    res = m_pDCSource->SetVoltage(iVoltage, DC_CHANNEL_VBUS);
    if (res)
    {
        _UiSendMsg("Set VBUS", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "VBUS = %g V", iVoltage);
        return res;
    }
    _UiSendMsg("Set VBUS", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr, "VBUS = %g V", iVoltage);

    return res;
}

SPRESULT CCheckXFastCharge::RunReadCurrent(FastChargeReturn* iDataReturn)
{
    SPRESULT res = SP_OK;
    res = SP_BBAT_FastChargeReadCurrent(m_hDUT, iDataReturn);
    if (res)
    {
        PrintErrorMsg(res, "FastChargeReadCurrent", LEVEL_ITEM);
        RunSetVBUS(5.0f);
        return res;
    }
    PrintSuccessMsg(SP_OK, "FastChargeReadCurrent", LEVEL_ITEM);

    return res;
}

SPRESULT CCheckXFastCharge::RunCheckCurrent(FastChargeReturn* iDataReturn)
{
    if (iDataReturn->iChipCount != m_iChipCount)
    {
        PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "CheckX Fast Charge: Chip Count Fail", LEVEL_ITEM);
        RunSetVBUS(5.0f);
        return SP_E_BBAT_VALUE_FAIL;
    }
    PrintSuccessMsg(SP_OK, "CheckX Fast Charge: Chip Count Pass", LEVEL_ITEM);

    int it = 0;
    while (it < iDataReturn->iChipCount)
    {
        if (iDataReturn->iCurrent[it]<m_iCurrentDownSpec || iDataReturn->iCurrent[it]>m_iCurrentUpSpec)
        {
            _UiSendMsg("CheckX Fast Charge: Current Fail", LEVEL_ITEM,
                m_iCurrentDownSpec, iDataReturn->iCurrent[it], m_iCurrentUpSpec, nullptr, -1, "mA",
                "Chip id = %d", it + 1);
            RunSetVBUS(5.0f);
            return SP_E_BBAT_VALUE_FAIL;
        }
        _UiSendMsg("CheckX Fast Charge: Current Pass", LEVEL_ITEM,
            m_iCurrentDownSpec, iDataReturn->iCurrent[it], m_iCurrentUpSpec, nullptr, -1, "mA",
            "Chip id = %d", it + 1);
        it++;
    }

    return SP_OK;
}

SPRESULT CCheckXFastCharge::RunClose()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_FastChargeClose(m_hDUT), "FastChargeClose", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "FastChargeClose", LEVEL_ITEM);

    return SP_OK;
}