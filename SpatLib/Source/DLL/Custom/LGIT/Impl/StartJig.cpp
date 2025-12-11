#include "StdAfx.h"
#include "StartJig.h"
#include "Jig.h"

//
IMPLEMENT_RUNTIME_CLASS(CStartJig)
//////////////////////////////////////////////////////////////////////////
CStartJig::CStartJig(void)
: m_dwPort(0)
, m_dwBaud(115200)
{
}

CStartJig::~CStartJig(void)
{
}

BOOL CStartJig::LoadXMLConfig(void)
{
    m_dwPort = (DWORD)GetConfigValue(L"Option:Port", 0);
    if (0 == m_dwPort)
    {
        SendCommonCallback(L"Invalid <Option:Port> Configuration!");
        return FALSE;
    }

    m_dwBaud = (DWORD)GetConfigValue(L"Option:Baudrate", 115200);

    return TRUE;
}

SPRESULT CStartJig::__PollAction(void)
{
    CJig Jig(GetISpLogObject());
    JIG_ERROR e = Jig.Start(m_dwPort, m_dwBaud);
    if (JIG_OK == e)
    {
        // Set JIG UART port and baudrate to share memory for CCloseJig action
        CHKRESULT(SetShareMemory(LGIT_ShareMemory_JigPort, (CONST VOID* )&m_dwPort, sizeof(m_dwPort)));
        CHKRESULT(SetShareMemory(LGIT_ShareMemory_JigBaud, (CONST VOID* )&m_dwBaud, sizeof(m_dwBaud)));

        NOTIFY("StartJig", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
        return SP_OK;
    }
    else
    {
        NOTIFY("StartJig", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, Jig.GetErrMsg(e));
        return SP_E_JIG_ERROR;
    }
}

