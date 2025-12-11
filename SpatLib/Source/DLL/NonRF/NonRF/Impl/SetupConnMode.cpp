#include "StdAfx.h"
#include "SetupConnMode.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSetupConnMode)

///
CSetupConnMode::CSetupConnMode(void)
: m_nMode(0)
{
}

CSetupConnMode::~CSetupConnMode(void)
{
}

BOOL CSetupConnMode::LoadXMLConfig(void)
{
    m_strMode = GetConfigValue(L"Option:ConnectMode", L"USB");
    if (m_strMode == L"USB")
    {
        m_nMode = 0;
    }
    else if (m_strMode == L"AP(WIFI)")
    {
        m_nMode = 1;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

SPRESULT CSetupConnMode::__PollAction(void)
{
    SPRESULT res = SP_apSetupConnMode(m_hDUT, m_nMode);
    NOTIFY("SetupConnMode", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "%s", _W2CA(m_strMode.c_str()));
    return res;
}

