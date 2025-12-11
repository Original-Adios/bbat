#include "StdAfx.h"
#include "GPIO_Read.h"

//
IMPLEMENT_RUNTIME_CLASS(CGPIO_Read)

//////////////////////////////////////////////////////////////////////////
CGPIO_Read::CGPIO_Read(void)
//Bug 1225350
:m_iGPIONo(0)
,m_iCompare_Value(0)
{
}

CGPIO_Read::~CGPIO_Read(void)
{
}

SPRESULT CGPIO_Read::__PollAction(void)
{
	SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_GPIO;
	GPIOValue DataSend;
	DataSend.iOperate = BBAT_GPIO::Read;
	DataSend.iGPIONumber = (BYTE)m_iGPIONo;

	GPIOData DataRecv;
	//BYTE bySend[1024] = {0x01};
	//BYTE byRecv[1024] = {0x00};
	//bySend[0] = 0;  //read
	//bySend[1] = (BYTE)m_iGPIONo;  //read
	res = SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), &DataRecv, sizeof(DataRecv), TIMEOUT_3S);
	if (res != SP_OK)
	{
		_UiSendMsg("GPIO_Read", LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "GPIO = %d", DataSend.iGPIONumber);
		return SP_E_BBAT_CMD_FAIL;
	}
    else
    {
		_UiSendMsg("GPIO_Read", LEVEL_ITEM, 0, DataRecv.GPIOVal, 1, nullptr, -1, nullptr, "GPIO = %d", DataSend.iGPIONumber);
        if (DataRecv.GPIOVal != m_iCompare_Value)
        {
			_UiSendMsg("GPIO_Compare Fail", LEVEL_ITEM, m_iCompare_Value, DataRecv.GPIOVal, m_iCompare_Value, nullptr, -1, nullptr);
            return SP_E_BBAT_GPIO_VALUE_FAIL;
        }
		_UiSendMsg("GPIO_Compare Pass", LEVEL_ITEM, m_iCompare_Value, DataRecv.GPIOVal, m_iCompare_Value, nullptr, -1, nullptr);
    }
	
    return res;
}
BOOL CGPIO_Read::LoadXMLConfig(void)
{
    m_iGPIONo = GetConfigValue(L"Option:GPIO_Number", 0);
	m_iCompare_Value = GetConfigValue(L"Option:Compare_Value", 0);
	return TRUE;
}