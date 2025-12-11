#include "StdAfx.h"
#include "UMW5621_All_Set.h"

//
IMPLEMENT_RUNTIME_CLASS(CUMW5621_All_Set)

//////////////////////////////////////////////////////////////////////////
CUMW5621_All_Set::CUMW5621_All_Set(void)
//Bug 1225370 
:m_iLevel(0)
{
}

CUMW5621_All_Set::~CUMW5621_All_Set(void)
{
}

SPRESULT CUMW5621_All_Set::__PollAction(void)
{
	SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_GPIO;
	BYTE bySend[1024] = {0x01};
	BYTE byRecv[1024] = {0x00};
	bySend[0] = 5;  //
	bySend[1] = (BYTE)m_iLevel; //

	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_3S);
	NOTIFY("All_Set", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Level = %d", m_iLevel);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
    return res;
}
BOOL CUMW5621_All_Set::LoadXMLConfig(void)
{
	m_iLevel = GetConfigValue(L"Option:All_Set_Level", 0);
	return TRUE;
}