#include "StdAfx.h"
#include "ModeChange.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CModeChange)

///
CModeChange::CModeChange(void)
{
}

CModeChange::~CModeChange(void)
{
}

BOOL CModeChange::LoadXMLConfig(void)
{
    __super::LoadXMLConfig();

    m_Options.m_u32DelayTime = GetConfigValue(L"Option:DelayTime", 0);

    return TRUE;
}

SPRESULT CModeChange::__PollAction(void)
{
    SPRESULT res = m_pModeSwitch->Reboot(m_Options.m_eMode, NULL, FALSE);
    NOTIFY("Change Mode", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Mode = 0x%X", DIAG_MODE(m_pModeSwitch->GetCurrentMode()));

    return res;
}
