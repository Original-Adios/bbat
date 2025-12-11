#include "StdAfx.h"
#include "InterConnect.h"

//
IMPLEMENT_RUNTIME_CLASS(CInterConnect)

//////////////////////////////////////////////////////////////////////////
//Bug 1225316 
CInterConnect::CInterConnect(void)
{
	
}

CInterConnect::~CInterConnect(void)
{
}

SPRESULT CInterConnect::__PollAction(void)
{
	SPRESULT res = SP_OK;
	SPRESULT actionRes = SP_OK;
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_RESERVE_5;
	BYTE bySend[1024] = {0x01};
	BYTE byRecv[1024] = {0x00};
	bySend[0] = 16;  //
	bySend[1] = 6; //

	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_5S);
	NOTIFY("InterConnect Command", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, NULL);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
	else
	{
		if(byRecv[0] == 0)//pass
		{				
			NOTIFY("InterConnect Value", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "InterConnect Test Pass");
		}
		else
		{
			NOTIFY("InterConnect Value", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "InterConnect Fail, Fail GPIO No.:%d",byRecv[0]);
			actionRes = SP_E_FAIL;
		}
	}
    return actionRes;
}
BOOL CInterConnect::LoadXMLConfig(void)
{
	return TRUE;
}