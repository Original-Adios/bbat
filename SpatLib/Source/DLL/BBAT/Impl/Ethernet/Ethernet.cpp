#include "StdAfx.h"
#include "Ethernet.h"
//

IMPLEMENT_RUNTIME_CLASS(CEthernet)

//////////////////////////////////////////////////////////////////////////
CEthernet::CEthernet(void)
{
}

CEthernet::~CEthernet(void)
{
}

SPRESULT CEthernet::__PollAction(void)
{
    if (m_nUSXGMII)
    {
        CHKRESULT(EthernetLoopTest_USXGMII());
    }

    if (m_nRGMII)
    {
        CHKRESULT(EthernetLoopTest_RGMII());
    }

    return SP_OK;
}

BOOL CEthernet::LoadXMLConfig(void)
{
    m_nRGMII = GetConfigValue(L"Option:RGMII", 0);
    m_nUSXGMII = GetConfigValue(L"Option:USXGMII", 0);
    return TRUE;
}

SPRESULT CEthernet::EthernetLoopTest_RGMII(void)
{
    char szRecv[128] = { 0 };
    char szInfo[128] = { 0 };

    sprintf_s(szInfo, sizeof(szInfo), "Ethernet RGMII Prerun");
    CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "AT+ETHUP=2", TRUE, szRecv, sizeof(szRecv), nullptr, TIMEOUT_10S), szInfo, LEVEL_ITEM));
    CHECK_AT_COMMAND_RESPONSE("AT+ETHUP=2", szRecv, "OK");

    sprintf_s(szInfo, sizeof(szInfo), "Ethernet RGMII Loop Test");
    CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "AT+SPLOOPBACK=ETH,2", TRUE, szRecv, sizeof(szRecv), nullptr, TIMEOUT_10S), szInfo, LEVEL_ITEM));
    CHECK_AT_COMMAND_RESPONSE("AT+SPLOOPBACK=ETH,2", szRecv, "OK");

    PrintSuccessMsg(SP_OK, szInfo, LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CEthernet::EthernetLoopTest_USXGMII(void)
{
    char szRecv[128] = { 0 };
    char szInfo[128] = { 0 };

    sprintf_s(szInfo, sizeof(szInfo), "Ethernet USXGMII Loop Test");
    CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "AT+SPLOOPBACK=ETH,1", TRUE, szRecv, sizeof(szRecv), nullptr, TIMEOUT_10S), szInfo, LEVEL_ITEM));
    CHECK_AT_COMMAND_RESPONSE("AT+SPLOOPBACK=ETH,1", szRecv, "OK");

    PrintSuccessMsg(SP_OK, szInfo, LEVEL_ITEM);

    return SP_OK;
}
