#include "StdAfx.h"
#include "EnableArmlog.h"

//
IMPLEMENT_RUNTIME_CLASS(CEnableArmlog)

CEnableArmlog::CEnableArmlog(void)
{
}
CEnableArmlog::~CEnableArmlog(void)
{
}

SPRESULT CEnableArmlog::__PollAction(void)
{
    CHKRESULT(PrintErrorMsg(SP_EnableArmLog(m_hDUT, TRUE), "Enable Armlog Old", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Enable Armlog Old", LEVEL_ITEM);
    return SP_OK;
}

BOOL CEnableArmlog::LoadXMLConfig(void)
{
    return TRUE;
}
