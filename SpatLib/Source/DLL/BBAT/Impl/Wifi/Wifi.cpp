#include "StdAfx.h"
#include "Wifi.h"

//
IMPLEMENT_RUNTIME_CLASS(CWifi)

//////////////////////////////////////////////////////////////////////////
CWifi::CWifi(void)
{
    m_nClose = 0;
    m_nOpen = 0;
    m_nRead = 0;
    m_nChannel = 0;

    m_nMinApNumber = 0;
    m_nMinRssiSpec = 0;
    m_nSearchTimes = 0;
    m_nReadApCount = 0;
}

CWifi::~CWifi(void)
{
}

SPRESULT CWifi::__PollAction(void)
{
    SPRESULT Res = SP_OK;
    if (m_nOpen)
    {
        CHKRESULT(RunOpen());
    }
    if (m_nRead)
    {
        Sleep(DELAY_BEFOREWIFISEARCH_MS);
        Res = SearchAction();
    }

    if (m_nClose)
    {
        CHKRESULT(RunClose());
    }
    return Res;
}
BOOL CWifi::LoadXMLConfig(void)
{
    m_nOpen = GetConfigValue(L"Option:Wifi_Open", 0);
    m_nClose = GetConfigValue(L"Option:Wifi_Close", 0);
    m_nRead = GetConfigValue(L"Option:Wifi_Read", 0);
    m_nChannel = GetConfigValue(L"Option:Wifi_5G", 0);

    m_nMinApNumber = GetConfigValue(L"Option:Min_AP_Number", 1);
    m_nMinRssiSpec = GetConfigValue(L"Option:Min_RSSI_Number", 35);
    m_nSearchTimes = GetConfigValue(L"Option:Search_Times", 2);
    return TRUE;
}

SPRESULT CWifi::RunOpen()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_WifiOpen(m_hDUT), "Wifi Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Wifi Open", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CWifi::RunSearch()
{
    CHKRESULT(SP_BBAT_WifiSearch(m_hDUT));
    return SP_OK;
}

SPRESULT CWifi::RunRead()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_WifiRead(m_hDUT, &m_nReadApCount), "Wifi Read Cmd", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Wifi Read Cmd", LEVEL_ITEM);

    _UiSendMsg("Wifi Read Test", LEVEL_ITEM, m_nMinApNumber, m_nReadApCount, MAXBYTE, nullptr, -1, nullptr,
        "AP = %d; Spec = %d", m_nReadApCount, m_nMinApNumber);
    if (m_nReadApCount < m_nMinApNumber)
    {
        return SP_E_BBAT_WIFI_VALUE_FAIL;
    }

    return SP_OK;
}

SPRESULT CWifi::RunClose()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_WifiClose(m_hDUT), "Wifi Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Wifi Close", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CWifi::RunReadRssi()
{
    int arrRssiValue[MAXWIFINUMBER] = { 0 };
    CHKRESULT(PrintErrorMsg(SP_BBAT_WifiReadRssi(m_hDUT, arrRssiValue), "Wifi Read Rssi Cmd", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Wifi Read Rssi Cmd", LEVEL_ITEM);

    if (m_nMinApNumber == 0)
    {
        _UiSendMsg("WiFi Read Rssi Test", LEVEL_ITEM,
            NOLOWLMT, arrRssiValue[0], NOUPPLMT, nullptr, -1, nullptr);
        return SP_OK;
    }

    size_t nMaxRssiApIndex = 0;
    int nMaxRssiValue = arrRssiValue[0];
    for (size_t it = 0; it < m_nReadApCount && it < MAXWIFINUMBER; it++)
    {
        if (nMaxRssiValue < arrRssiValue[it])
        {
            nMaxRssiValue = arrRssiValue[it];
            nMaxRssiApIndex = it;
        }

        _UiSendMsg("WiFi Read Rssi Test", LEVEL_REPORT, m_nMinRssiSpec, arrRssiValue[it], MAXWIFIRSSISPEC, nullptr, -1, nullptr,
            "AP = %d; Rssi = %d", it + 1, arrRssiValue[it]);
    }

    _UiSendMsg("WiFi Read Rssi Test", LEVEL_ITEM, m_nMinRssiSpec, arrRssiValue[nMaxRssiApIndex], MAXWIFIRSSISPEC, nullptr, -1, nullptr,
        "AP = %d; MaxRssi = %d", nMaxRssiApIndex + 1, arrRssiValue[nMaxRssiApIndex]);

    BOOL bResult = IN_RANGE(m_nMinRssiSpec, arrRssiValue[nMaxRssiApIndex], MAXWIFIRSSISPEC);
    return  bResult ? SP_OK : SP_E_BBAT_WIFI_VALUE_FAIL;
}

SPRESULT CWifi::RunChannel()
{
    BOOL bChannel[2];
    int iRssiValue[2];
    memset(&bChannel, FALSE, sizeof(bChannel));

    CHKRESULT(PrintErrorMsg(SP_BBAT_WifiChannel(m_hDUT, bChannel, iRssiValue), "Wifi Channel Cmd", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Wifi Channel Cmd", LEVEL_ITEM);

    if (!bChannel[Channel_2G])
    {
        PrintErrorMsg(SP_E_BBAT_WIFI_VALUE_FAIL, "WiFi 2.4G has not been searched!", LEVEL_ITEM);
    }

    _UiSendMsg("WiFi 2.4G  Channel Test", LEVEL_ITEM, m_nMinRssiSpec, iRssiValue[0], MAXWIFIRSSISPEC, nullptr, -1, nullptr,
        "Rssi = %d", iRssiValue[0]);

    if (!bChannel[Channel_5G])
    {
        PrintErrorMsg(SP_E_BBAT_WIFI_VALUE_FAIL, "WiFi 5G has not been searched!", LEVEL_ITEM);
    }

    _UiSendMsg("WiFi 5G Channel Test", LEVEL_ITEM, m_nMinRssiSpec, iRssiValue[1], MAXWIFIRSSISPEC, nullptr, -1, nullptr,
        "Rssi = %d", iRssiValue[1]);

    if (bChannel[0] && bChannel[1] && IN_RANGE(m_nMinRssiSpec, iRssiValue[0], MAXWIFIRSSISPEC) &&
        IN_RANGE(m_nMinRssiSpec, iRssiValue[1], MAXWIFIRSSISPEC))
    {        
        return SP_OK;
    }
    else
    {
        return SP_E_BBAT_WIFI_VALUE_FAIL;
    }
}

SPRESULT CWifi::SearchAction()
{
    SPRESULT Search = SP_OK;

    for (int it = 0; it < m_nSearchTimes; it++)
    {
        Search = RunSearch();
        if (Search == SP_OK)
        {
            PrintSuccessMsg(SP_OK, "BBAT Wifi Test : Search", LEVEL_ITEM);
            break;
        }
    }
    if (Search != SP_OK)
    {
        _UiSendMsg("Wifi Search", LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr,
            "Search Fail in %d Times", m_nSearchTimes);
        return Search;
    }

    CHKRESULT(RunRead());
    CHKRESULT(RunReadRssi());
    if (m_nChannel)
    {
        CHKRESULT(RunChannel());
    }
    return SP_OK;
}


