#include "StdAfx.h"
#include "UMW5621_Single_Pull.h"

//
IMPLEMENT_RUNTIME_CLASS(CUMW5621_Single_Pull)

//////////////////////////////////////////////////////////////////////////
CUMW5621_Single_Pull::CUMW5621_Single_Pull(void)
//Bug 1225329 
:m_iGPIONo(0)
,m_iLevel(0)
{
}

CUMW5621_Single_Pull::~CUMW5621_Single_Pull(void)
{
}

SPRESULT CUMW5621_Single_Pull::__PollAction(void)
{
	SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_GPIO;
	BYTE bySend[1024] = {0x01};
	BYTE byRecv[1024] = {0x00};
	bySend[0] = 3;  //¶¨ÖÆÏî
	bySend[1] = (BYTE)m_iGPIONo; //
	bySend[2] = (BYTE)m_iLevel; //

	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 3, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_3S);
	NOTIFY("Single_Pull", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "GPIO = %d ,Level = %d", m_iGPIONo,m_iLevel);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
    return res;
}
BOOL CUMW5621_Single_Pull::LoadXMLConfig(void)
{
	m_iGPIONo = GetConfigValue(L"Option:GPIO_Number", 0);
	m_iLevel = GetConfigValue(L"Option:Single_Pull_Level", 0);
	return TRUE;
}