#include "StdAfx.h"
#include "PreEthernet.h"
//

IMPLEMENT_RUNTIME_CLASS(CPreEthernet)

//////////////////////////////////////////////////////////////////////////
CPreEthernet::CPreEthernet(void)
{
}

CPreEthernet::~CPreEthernet(void)
{
}

SPRESULT CPreEthernet::__PollAction(void)
{
    char szRecv[128] = { 0 };
    char szInfo[128] = { 0 };
    sprintf_s(szInfo, sizeof(szInfo), "Ethernet Prerun");
    CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "AT+ETHUP=1", TRUE, szRecv, sizeof(szRecv), nullptr, TIMEOUT_10S), szInfo, LEVEL_ITEM));
    CHECK_AT_COMMAND_RESPONSE("AT+ETHUP=1", szRecv, "OK");

    PrintSuccessMsg(SP_OK, szInfo, LEVEL_ITEM);
    return SP_OK;
}

BOOL CPreEthernet::LoadXMLConfig(void)
{
    return TRUE;
}