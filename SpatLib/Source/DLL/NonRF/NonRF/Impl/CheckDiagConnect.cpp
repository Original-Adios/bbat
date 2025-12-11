#include "StdAfx.h"
#include "CheckDiagConnect.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckDiagConnect)

//////////////////////////////////////////////////////////////////////////
CCheckDiagConnect::CCheckDiagConnect(void)
: m_eCmd(LoadSwVer)
, m_dwTimeOut(0)
{
}

CCheckDiagConnect::~CCheckDiagConnect(void)
{
}

BOOL CCheckDiagConnect::LoadXMLConfig(void)
{
    LPCWSTR lpszCmd = GetConfigValue(L"Option:CheckCmd", L"LoadSwVer");
    if (NULL == lpszCmd)
    {
        return FALSE;
    }

    std::wstring strCmd = lpszCmd;
    if (strCmd == L"LoadSwVer")
    {
        m_eCmd = LoadSwVer;
    }
    else if (strCmd == L"AT")
    {
        m_eCmd = AT;
    }
    else
    {
        return FALSE;
    }

    m_dwTimeOut = GetConfigValue(L"Option:TimeOut", TIMEOUT_60S);
    return TRUE;
}

SPRESULT CCheckDiagConnect::__PollAction(void)
{
    UINT32 ulTimeOut = TIMEOUT_3S;
    CHKRESULT(SP_GetProperty(m_hDUT, SP_ATTR_TIMEOUT, 0, (LPVOID )&ulTimeOut));

    SPRESULT res = SP_OK;
    CSPTimer timer;
    do 
    {
        if (_IsUserStop())
        {
            return SP_E_USER_ABORT;
        }

        switch(m_eCmd)
        {
        case LoadSwVer:
            {
                CHAR szSwVer[2048] = {0};
                CHKRESULT(SP_SetProperty(m_hDUT, SP_ATTR_TIMEOUT, 0, (LPCVOID)TIMEOUT_3S));
                res = SP_GetSWVer(m_hDUT, CP, szSwVer, 2048);
                CHKRESULT(SP_SetProperty(m_hDUT, SP_ATTR_TIMEOUT, 0, (LPCVOID)ulTimeOut));
            }
            break;
        case AT:
            {
                CHAR szBuf[1024] = {0};
                res = SP_SendATCommand(m_hDUT, "AT", TRUE, szBuf, sizeof(szBuf), NULL, TIMEOUT_3S);
            }
            break;
        default:
            NOTIFY("CheckDiagConn", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unknown Cmd = %d", m_eCmd);
            return SP_E_INVALID_PARAMETER;
        }

        if (SP_OK == res)
        {
            break;
        }
        else
        {
            Sleep(TIMEOUT_1S);
        }

    } while(!timer.IsTimeOut(m_dwTimeOut));

    NOTIFY("CheckDiagConn", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL);
    return res;
}
