#include "StdAfx.h"
#include "UMP5865_GPIO_GetLevel.h"

//
IMPLEMENT_RUNTIME_CLASS(CUMP5865_GPIO_GetLevel)

//////////////////////////////////////////////////////////////////////////
CUMP5865_GPIO_GetLevel::CUMP5865_GPIO_GetLevel(void)
//Bug 1225350
:m_iGPIONo(0)
,m_iCompare_Value(0)
{
}

CUMP5865_GPIO_GetLevel::~CUMP5865_GPIO_GetLevel(void)
{
}

SPRESULT CUMP5865_GPIO_GetLevel::__PollAction(void)
{
	SPRESULT res = SP_OK;
	//DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_GPIO;
	//BYTE bySend[1024] = {0x01};
	//BYTE byRecv[1024] = {0x00};
	//bySend[0] = 0;  //read
	//bySend[1] = (BYTE)m_iGPIONo;  //read
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_GPIO;
	GPIOReadValue DataSend;
	DataSend.iGPIONumber = (int8)m_iGPIONo;
	DataSend.iOperate = GPIO_READ;
	GPIOData DataRecv;
	memset(&DataRecv, 0, sizeof(DataRecv));
	res = SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), &DataRecv, sizeof(DataRecv), TIMEOUT_3S);
	if (res != SP_OK)
	{
		NOTIFY("UMP5865_GPIO_GetLevel", LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "GPIO = %d", DataSend.iGPIONumber);
		return SP_E_BBAT_CMD_FAIL;
	}
    else
    {
        NOTIFY("UMP5865_GPIO_GetLevel", LEVEL_ITEM, m_iCompare_Value, DataRecv.GPIOVal, m_iCompare_Value, nullptr, -1, nullptr, "GPIO = %d", DataSend.iGPIONumber);
        if (DataSend.iGPIONumber != m_iCompare_Value)
        {
            return SP_E_BBAT_GPIO_VALUE_FAIL;
        }
    }
    return res;
}
BOOL CUMP5865_GPIO_GetLevel::LoadXMLConfig(void)
{
    m_iGPIONo = GetConfigValue(L"Option:GPIO_Number", 0);
	m_iCompare_Value = GetConfigValue(L"Option:Compare_Value", 0);
	return TRUE;
}