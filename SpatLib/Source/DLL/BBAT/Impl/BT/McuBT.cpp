#include "StdAfx.h"
#include "McuBT.h"

//
IMPLEMENT_RUNTIME_CLASS(CMcuBT)

//////////////////////////////////////////////////////////////////////////
CMcuBT::CMcuBT(void)
{
    m_nMinRssiSpec = 0;
}

CMcuBT::~CMcuBT(void)
{
}

SPRESULT CMcuBT::__PollAction(void)
{
 CHKRESULT(RunReadRssi());
}
BOOL CMcuBT::LoadXMLConfig(void)
{
    m_nMinRssiSpec = GetConfigValue(L"Option:Min_RSSI_Number", -85);
    return TRUE;
}

SPRESULT CMcuBT::RunReadRssi()
{
    int arrRssiValue[MAXMCUBTNUMBER] = { 0 };

    CHKRESULT(PrintErrorMsg(SP_BBAT_Mcu_BtReadRssi(m_hDUT, (int8)m_nMinRssiSpec, arrRssiValue), "MCU BT Read Rssi Cmd", LEVEL_ITEM));

    size_t nMaxRssiApIndex = 0;
    int nMaxRssiValue = arrRssiValue[0];
    for (size_t it = 0; it < MAXMCUBTNUMBER; it++)
    {
        if (nMaxRssiValue < arrRssiValue[it])
        {
            nMaxRssiValue = arrRssiValue[it];
            nMaxRssiApIndex = it;
        }

        _UiSendMsg("BT Read Rssi Test", LEVEL_REPORT, m_nMinRssiSpec, arrRssiValue[it], MAXBTRSSISPEC, nullptr, -1, nullptr,
            "AP = %d; Rssi = %d", it + 1, arrRssiValue[it]);
    }

    _UiSendMsg("BT Read Rssi Test", LEVEL_ITEM, m_nMinRssiSpec, arrRssiValue[nMaxRssiApIndex], MAXBTRSSISPEC, nullptr, -1, nullptr,
        "AP = %d; MaxRssi = %d", nMaxRssiApIndex + 1, arrRssiValue[nMaxRssiApIndex]);

    BOOL bResult = IN_RANGE(m_nMinRssiSpec, arrRssiValue[nMaxRssiApIndex], MAXBTRSSISPEC);
    return  bResult ? SP_OK : SP_E_BBAT_BT_VALUE_FAIL;
}


