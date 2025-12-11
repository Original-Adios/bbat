#include "StdAfx.h"
#include "ShortCircuit.h"

//
IMPLEMENT_RUNTIME_CLASS(CShortCircuit)

//////////////////////////////////////////////////////////////////////////
//Bug 1225316 
CShortCircuit::CShortCircuit(void)
{
	
}

CShortCircuit::~CShortCircuit(void)
{
}

SPRESULT CShortCircuit::__PollAction(void)
{
	SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_RESERVE_5;
	BYTE bySend[1024] = {0x01};
	BYTE byRecv[1024] = {0x00};
	bySend[0] = 16;  //
	bySend[1] = 7; //

	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_5S);
	NOTIFY("ShortCircuit command", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, NULL);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
	else
	{
		if(byRecv[0] == 0)//pass
		{				
			NOTIFY("ShortCircuit value", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "ShortCircuit Test Pass");
		}
		else
		{
			NOTIFY("ShortCircuit value", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShortCircuit Fail, Fail GPIO No.:%d",byRecv[0]);
			return SP_E_BBAT_VALUE_FAIL;
		}
	}
    return res;
}
BOOL CShortCircuit::LoadXMLConfig(void)
{
	return TRUE;
}