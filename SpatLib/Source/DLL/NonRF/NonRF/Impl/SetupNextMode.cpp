#include "StdAfx.h"
#include "SetupNextMode.h"
#include "RunMode.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSetupNextMode)

//////////////////////////////////////////////////////////////////////////
CSetupNextMode::CSetupNextMode(void)
{
    m_strMode = "";
}

CSetupNextMode::~CSetupNextMode(void)
{
}

BOOL CSetupNextMode::LoadXMLConfig(void)
{
    LPCWSTR lpszMode = GetConfigValue(L"Option:RunMode", L"");
    if (NULL == lpszMode)
    {
        return FALSE;
    }
    else
    {
        m_strMode = _W2CA(lpszMode);
        trimA(m_strMode);
        if (0 == m_strMode.length())
        {
            SendCommonCallback(L"Invalid Option:RunMode configuration!");
            return FALSE;
        }
    }

    return TRUE;
}

SPRESULT CSetupNextMode::__PollAction(void)
{
    INT nMode = CRunMode::QueryMode(m_strMode);
    if (nMode < 0)
    {
        NOTIFY("SetupNextRunMode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unknown mode = %s", m_strMode.c_str());
        return SP_E_PHONE_UNKNOWN_RUN_MODE;
    }

    SPRESULT res = SP_SetupNextRunMode(m_hDUT, nMode);
    NOTIFY("SetupNextRunMode", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "%s", m_strMode.c_str());
    return res;
}
