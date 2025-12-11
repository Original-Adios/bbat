#include "StdAfx.h"
#include "EnableArmLog.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CEnableArmLog)

///
CEnableArmLog::CEnableArmLog(void)
: m_bEnable(TRUE)
{
}

CEnableArmLog::~CEnableArmLog(void)
{
}

BOOL CEnableArmLog::LoadXMLConfig(void)
{
    m_bEnable = (BOOL)GetConfigValue(L"Option:EnableArmLog", 0);
    return TRUE;
}

SPRESULT CEnableArmLog::__PollAction(void)
{ 
    SPRESULT res = SP_EnableArmLog(m_hDUT, m_bEnable);
    if (SP_OK == res && m_bEnable)
    {
        // ArmLogel playback need CP version 
        CHAR szVer[1000] = {0};
        SP_GetSWVer(m_hDUT, CP, szVer, sizeof(szVer));
    }

    NOTIFY(m_bEnable ? "EnableArmLog" : "DisableArmLogel", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL);
    return res;
}
