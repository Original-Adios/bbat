#include "StdAfx.h"
#include "GPS.h"

//
IMPLEMENT_RUNTIME_CLASS(CGps)

//////////////////////////////////////////////////////////////////////////
CGps::CGps(void)
{
    m_nMinApNumber = 0;
    m_nMinRssiSpec = 0;
    m_nReadTimes = 0;
    m_nGpsOpen = 0;
    m_nGpsClose = 0;
    m_nGpsSearch = 0;
    m_nReadApCount = 0;
}

CGps::~CGps(void)
{
}

SPRESULT CGps::__PollAction(void)
{
    SPRESULT SearchRes = SP_OK;
    if (m_nGpsOpen)
    {
        CHKRESULT(RunOpen());
        Sleep(DELAY_BEFOREGPSSEARCH_MS);
        CHKRESULT(RunSearch());
        Sleep(3000);
    }
    if (m_nGpsSearch)
    {
        SearchRes = ReadAction();
    }
    if (m_nGpsClose)
    {
        CHKRESULT(RunClose());
    }
    return SearchRes;
}
BOOL CGps::LoadXMLConfig(void)
{
    m_nMinApNumber = GetConfigValue(L"Option:Min_AP_Number", 1);
    m_nMinRssiSpec = GetConfigValue(L"Option:Min_RSSI_Number", 35);
    m_nReadTimes = GetConfigValue(L"Option:Read_Times", 2);

    m_nGpsOpen = GetConfigValue(L"Option:GPS_OPEN", 1);
    m_nGpsClose = GetConfigValue(L"Option:GPS_CLOSE", 1);
    m_nGpsSearch = GetConfigValue(L"Option:GPS_SEARCH", 1);

    return TRUE;
}


SPRESULT CGps::RunOpen()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_GpsOpen(m_hDUT),
        "GPS Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "GPS Open", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CGps::RunSearch()
{
    Sleep(2000);
    CHKRESULT(PrintErrorMsg(SP_BBAT_GpsSearch(m_hDUT), "GPS Search", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "GPS Search", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CGps::RunRead()
{
    Sleep(2000);
    CHKRESULT(PrintErrorMsg(SP_BBAT_GpsRead(m_hDUT, &m_nReadApCount),
        "GPS Read", LEVEL_ITEM));

    _UiSendMsg("GPS Read AP Test", LEVEL_ITEM, m_nMinApNumber, m_nReadApCount, MAXBYTE, nullptr, -1, nullptr,
        "AP = %d; Spec = %d", m_nReadApCount, m_nMinApNumber);

    if (m_nReadApCount < m_nMinApNumber)
    {
        return SP_E_BBAT_GPS_VALUE_FAIL;
    }

    return SP_OK;
}

SPRESULT CGps::RunClose()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_GpsClose(m_hDUT), "GPS Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "GPS Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CGps::RunRssi(int nRetrytimes)
{
    Sleep(2000);
    int arrRssiValue[MAXGPSNUMBER] = { 0 };
    SPRESULT res = SP_BBAT_GpsReadRssi(m_hDUT, arrRssiValue);
    if (SP_OK != res && nRetrytimes == 0)
    {
        PrintErrorMsg(res, "GPS Read Rssi Cmd", LEVEL_ITEM);
    }

    if (m_nMinApNumber == 0)
    {
        _UiSendMsg("GPS Read Rssi", LEVEL_ITEM,
            NOLOWLMT, arrRssiValue[0], NOUPPLMT, nullptr, -1, nullptr);
        return SP_OK;
    }

    m_nReadApCount = arrRssiValue[0];

    BOOL bLogLevel = !IN_RANGE(m_nMinApNumber, m_nReadApCount, NOUPPLMT) && nRetrytimes > 0;
    _UiSendMsg("GPS Read AP Count", bLogLevel ? LEVEL_REPORT : LEVEL_ITEM, m_nMinApNumber, m_nReadApCount, NOUPPLMT, nullptr, -1, nullptr,
        "AP Count= %d; Spec = %d", m_nReadApCount, m_nMinApNumber);

    BOOL bResult = IN_RANGE(m_nMinApNumber, m_nReadApCount, NOUPPLMT);
    if (!bResult)
    {
        return SP_E_BBAT_GPS_VALUE_FAIL;
    }

    size_t nMaxRssiApIndex = 1;
    int nMaxRssiValue = arrRssiValue[1];
    for (size_t it = 1; it <= m_nReadApCount && it <= MAXGPSNUMBER; it++)
    {
        if (nMaxRssiValue < arrRssiValue[it])
        {
            nMaxRssiValue = arrRssiValue[it];
            nMaxRssiApIndex = it;
        }

        _UiSendMsg("GPS Read Rssi Test", LEVEL_REPORT, m_nMinRssiSpec, arrRssiValue[it], NOUPPLMT, nullptr, -1, nullptr,
            "AP = %d; Rssi = %d", it, arrRssiValue[it]);
    }

    bLogLevel = !IN_RANGE(m_nMinRssiSpec, arrRssiValue[nMaxRssiApIndex], NOUPPLMT) && nRetrytimes > 0;
    _UiSendMsg("GPS Read Rssi Test", bLogLevel ? LEVEL_REPORT : LEVEL_ITEM, m_nMinRssiSpec, arrRssiValue[nMaxRssiApIndex], NOUPPLMT, nullptr, -1, nullptr,
        "AP = %d; MaxRssi = %d", nMaxRssiApIndex, arrRssiValue[nMaxRssiApIndex]);

    bResult = IN_RANGE(m_nMinRssiSpec, arrRssiValue[nMaxRssiApIndex], NOUPPLMT);
    return  bResult ? SP_OK : SP_E_BBAT_GPS_VALUE_FAIL;
}

SPRESULT CGps::ReadAction()
{
    SPRESULT Read = SP_OK;
    for (int it = m_nReadTimes-1; it > -1; it--)
    {
        LogFmtStrA(SPLOGLV_INFO, "ReadAction %d times ", m_nReadTimes - it);
        Read = RunRssi(it);
        if (Read == SP_OK)
        {
            PrintSuccessMsg(SP_OK, "BBAT GPS Test : Read Pass", LEVEL_ITEM);
            break;
        }
    }
    if (Read != SP_OK)
    {
        _UiSendMsg("Gps Read", LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr,
            "Read Fail in %d Times", m_nReadTimes);
        return Read;
    }

    return SP_OK;
}
