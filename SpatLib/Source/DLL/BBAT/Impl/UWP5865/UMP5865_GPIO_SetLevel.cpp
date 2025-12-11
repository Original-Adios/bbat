#include "StdAfx.h"
#include "UMP5865_GPIO_SetLevel.h"

//
IMPLEMENT_RUNTIME_CLASS(CUMP5865_GPIO_SetLevel)

//////////////////////////////////////////////////////////////////////////
CUMP5865_GPIO_SetLevel::CUMP5865_GPIO_SetLevel(void)
//Bug 1225318 
:m_iGPIONo(0)
,m_iLevel(0)
{
}

CUMP5865_GPIO_SetLevel::~CUMP5865_GPIO_SetLevel(void)
{
}

SPRESULT CUMP5865_GPIO_SetLevel::__PollAction(void)
{
	SPRESULT res = SP_OK;
	//DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_GPIO;
	//BYTE bySend[1024] = {0x01};
	//BYTE byRecv[1024] = {0x00};
	//bySend[0] = 1;  //write
	//bySend[1] = (BYTE)m_iGPIONo; //GPIO NO.

	////À­µÍ
	//bySend[2] = (BYTE)m_iLevel; //0	
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_GPIO;
	GPIOWriteValue DataSend;
	DataSend.iOperate = BBAT_GPIO::Write;
	DataSend.iGPIONumber = (int8)m_iGPIONo;
	DataSend.iGPIOValue = (int8)m_iLevel;

	res = SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), nullptr, 0, TIMEOUT_3S);
	_UiSendMsg("UMP5865_GPIO_SetLevel", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, nullptr, -1, nullptr, 
		"GPIO = %d; Value = %d", DataSend.iGPIONumber, DataSend.iGPIOValue);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
    return res;
}
BOOL CUMP5865_GPIO_SetLevel::LoadXMLConfig(void)
{
    m_iGPIONo = GetConfigValue(L"Option:GPIO_Number", 0);
	m_iLevel = GetConfigValue(L"Option:Set_Level", 0);
	return TRUE;
}