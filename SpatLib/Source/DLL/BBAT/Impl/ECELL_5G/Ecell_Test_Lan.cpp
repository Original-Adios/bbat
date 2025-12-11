#include "StdAfx.h"
#include "Ecell_Test_Lan.h"

//
IMPLEMENT_RUNTIME_CLASS(CEcell_Test_Lan)

//////////////////////////////////////////////////////////////////////////
//Bug 1225316 
CEcell_Test_Lan::CEcell_Test_Lan(void)
{
	
}

CEcell_Test_Lan::~CEcell_Test_Lan(void)
{
}

SPRESULT CEcell_Test_Lan::__PollAction(void)
{
	SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_RESERVE_5;
	BYTE bySend[1024] = {0x01};
	BYTE byRecv[1024] = {0x00};
	bySend[0] = 16;  //
	bySend[1] = 1; //

	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_10S);
	NOTIFY("Lan_Test(TimeOut:10s)", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, NULL);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
    return res;
}
BOOL CEcell_Test_Lan::LoadXMLConfig(void)
{
	return TRUE;
}