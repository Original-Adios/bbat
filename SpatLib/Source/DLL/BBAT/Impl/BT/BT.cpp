#include "StdAfx.h"
#include "BT.h"

//
IMPLEMENT_RUNTIME_CLASS(CBT)

//////////////////////////////////////////////////////////////////////////
CBT::CBT(void)
{
    m_nMinApNumber = 0;
    m_nMinRssiSpec = 0;
    m_nSearchTimes = 0;
    m_nBtOpen = 0;
    m_nBtClose = 0;
    m_nBtSearch = 0;
    m_nReadApCount = 0;
}

CBT::~CBT(void)
{
}

SPRESULT CBT::__PollAction(void)
{
    SPRESULT SearchRes = SP_OK;
    if (m_nBtOpen)
    {
        CHKRESULT(RunOpen());
    }
    if (m_nBtSearch)
    {
        Sleep(DELAY_BEFOREBTSEARCH_MS);
        SearchRes = SearchAction();
    }
    if (m_nBtClose)
    {
        CHKRESULT(RunClose());
    }

    return SearchRes;
}
BOOL CBT::LoadXMLConfig(void)
{
    m_nMinApNumber = GetConfigValue(L"Option:Min_AP_Number", 1);
    m_nMinRssiSpec = GetConfigValue(L"Option:Min_RSSI_Number", -85);
    m_nSearchTimes = GetConfigValue(L"Option:SEARCH_TIMES", 2);

    m_nBtOpen = GetConfigValue(L"Option:BT_OPEN", 1);
    m_nBtClose = GetConfigValue(L"Option:BT_CLOSE", 1);
    m_nBtSearch = GetConfigValue(L"Option:BT_SEARCH", 1);
    /*if (m_iSearchTimes < 2)
    {
        m_iSearchTimes = 2;
    }*/

    return TRUE;
}



SPRESULT CBT::RunRead()
{
    Sleep(2000);
    CHKRESULT(PrintErrorMsg(SP_BBAT_BtRead(m_hDUT, &m_nReadApCount), "Bt Read Cmd", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Bt Read Cmd", LEVEL_ITEM);

    _UiSendMsg("Bt Read Test", LEVEL_ITEM, m_nMinApNumber, m_nReadApCount, INT_LEAST8_MAX, nullptr, -1, nullptr,
        "AP = %d; Spec = %d", m_nReadApCount, m_nMinApNumber);

    if (m_nReadApCount < m_nMinApNumber)
    {
        return SP_E_BBAT_BT_VALUE_FAIL;
    }

    return SP_OK;
}

SPRESULT CBT::RunReadRssi()
{
    int arrRssiValue[MAXBTNUMBER] = { 0 };

    CHKRESULT(PrintErrorMsg(SP_BBAT_BtReadRssi(m_hDUT, arrRssiValue), "BT Read Rssi Cmd", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "BT Read Rssi Cmd", LEVEL_ITEM);

    if (m_nMinApNumber == 0)
    {
        _UiSendMsg("BT Read Rssi Test", LEVEL_ITEM,
            NOLOWLMT, arrRssiValue[0], NOUPPLMT, nullptr, -1, nullptr);
        return SP_OK;
    }

    size_t nMaxRssiApIndex = 0;
    int nMaxRssiValue = arrRssiValue[0];
    for (size_t it = 0; it < m_nReadApCount && it < MAXBTNUMBER; it++)
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

SPRESULT CBT::RunOpen()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_BtOpen(m_hDUT), "Bt Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Bt Open", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CBT::RunClose()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_BtClose(m_hDUT), "Bt Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Bt Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CBT::RunSearch()
{
    CHKRESULT(SP_BBAT_BtSearch(m_hDUT));

    return SP_OK;
}

SPRESULT CBT::SearchAction()
{
    SPRESULT Search = SP_OK;
    for (int it = 0; it < m_nSearchTimes; it++)
    {
        Sleep(2000); //有的BT设备开启后需要更多延时才能搜索
        Search = RunSearch();
        //Search = SP_E_BBAT_BT_VALUE_FAIL;
        if (Search == SP_OK)
        {
            PrintSuccessMsg(SP_OK, "BBAT BT Test : Search", LEVEL_ITEM);
            break;
        }
        //PrintSuccessMsg(SP_OK, "BBAT BT Test : Search Fail", LEVEL_ITEM);
    }
    if (Search != SP_OK)
    {
        _UiSendMsg("BT Search", LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr,
            "Search Fail in %d Times", m_nSearchTimes);
        return Search;
    }

    CHKRESULT(RunRead());
    CHKRESULT(RunReadRssi());

    return SP_OK;
}
