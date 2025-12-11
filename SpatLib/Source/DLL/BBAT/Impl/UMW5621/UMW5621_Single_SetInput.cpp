#include "StdAfx.h"
#include "UMW5621_Single_SetInput.h"

//
IMPLEMENT_RUNTIME_CLASS(CUMW5621_Single_SetInput)

//////////////////////////////////////////////////////////////////////////
//Bug 1225316 
CUMW5621_Single_SetInput::CUMW5621_Single_SetInput(void)
: m_iGPIONo(0)
, m_iLevel(0)
{
	
}

CUMW5621_Single_SetInput::~CUMW5621_Single_SetInput(void)
{
}

SPRESULT CUMW5621_Single_SetInput::__PollAction(void)
{
	SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_GPIO;
	BYTE bySend[1024] = {0x01};
	BYTE byRecv[1024] = {0x00};
	bySend[0] = 2;  //¶¨ÖÆÏî
	bySend[1] = (BYTE)m_iGPIONo; //

	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_3S);
	NOTIFY("Single_Set_Input", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "GPIO = %d ", m_iGPIONo);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
    return res;
}
BOOL CUMW5621_Single_SetInput::LoadXMLConfig(void)
{
	m_iGPIONo = GetConfigValue(L"Option:GPIO_Number", 0);
	m_iLevel = GetConfigValue(L"Option:Single_Pull_Level", 0);
	return TRUE;
}