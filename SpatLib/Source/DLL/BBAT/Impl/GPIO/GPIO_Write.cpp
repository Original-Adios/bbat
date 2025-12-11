#include "StdAfx.h"
#include "GPIO_Write.h"

//
IMPLEMENT_RUNTIME_CLASS(CGPIO_Write)

//////////////////////////////////////////////////////////////////////////
CGPIO_Write::CGPIO_Write(void)
//Bug 1225318 
:m_iGPIONo(0)
,m_iLevel(0)
{
}

CGPIO_Write::~CGPIO_Write(void)
{
}

SPRESULT CGPIO_Write::__PollAction(void)
{
	SPRESULT res = SP_OK;
	//DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_GPIO;
	//BYTE bySend[1024] = {0x01};
	//BYTE byRecv[1024] = {0x00};
	//bySend[0] = 1;  //write
	//bySend[1] = (BYTE)m_iGPIONo; //GPIO NO.

	//À­µÍ
	//bySend[2] = (BYTE)m_iLevel; //0	
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_GPIO;
	GPIOWriteValue DataSend;
	DataSend.iOperate = BBAT_GPIO::Write;
	DataSend.iGPIONumber = (int8)m_iGPIONo;
	DataSend.iGPIOValue = (int8)m_iLevel;

	res = SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), nullptr, 0, TIMEOUT_3S);
	_UiSendMsg("GPIO_Write", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, nullptr, -1, nullptr,
		"GPIO = %d; Value = %d", DataSend.iGPIONumber, DataSend.iGPIOValue);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
    return res;
}
BOOL CGPIO_Write::LoadXMLConfig(void)
{
    m_iGPIONo = GetConfigValue(L"Option:GPIO_Number", 0);
	m_iLevel = GetConfigValue(L"Option:Set_Level", 0);
	return TRUE;
}