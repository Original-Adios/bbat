#include "StdAfx.h"
#include "UIS6760_GPIO.h"

//
IMPLEMENT_RUNTIME_CLASS(CUIS6760_GPIO)

//////////////////////////////////////////////////////////////////////////
CUIS6760_GPIO::CUIS6760_GPIO(void)
: m_bUart_ADC(FALSE)
, m_bLED(FALSE)
, m_bPA_LNA(FALSE)
{
}

CUIS6760_GPIO::~CUIS6760_GPIO(void)
{
}

BOOL CUIS6760_GPIO::LoadXMLConfig(void)
{
    m_bUart_ADC = GetConfigValue(L"Option:UART_ADC", 0);
    m_bLED = GetConfigValue(L"Option:LED", 0);
    m_bPA_LNA = GetConfigValue(L"Option:PA_LNA", 0);

    return TRUE;
}

SPRESULT CUIS6760_GPIO::__PollAction(void)
{
	/*
	Bug 1131374 - [Simba][UIS6760] 支持外设测试

	GPIO 信息：
	名称			GPIO NO.			模块引脚
	Uart3_TXD		1					16
	ADC				ADC					1
	reset 按键		reset 				reset 按键
	LED_Mode		36					68
	EXT_PA_EN		EXT_PA_EN			59
	EXT_LNA_EN    	EXT_LNA_EN			60

	*/
	if (m_bUart_ADC)
	{
		CHKRESULT(TestUart_ADC());
	}
	
	if (m_bLED)
	{
		CHKRESULT(TestLED());
	}

	if (m_bPA_LNA)
	{
		CHKRESULT(TestPA_LNA());
	}

    return SP_OK;
}

SPRESULT CUIS6760_GPIO::TestUart_ADC(void)
{
	//TestGroupUart_ADC:
	//1.	Uart3
	//2.	ADC
	//这两个做互联测试，Uart3做set动作，adc0做read动作

    SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_GPIO;
	GPIOWriteSend DataSend;
	DataSend.iOperate = BBAT_GPIO::Write;
	DataSend.iGPIONumber = BBAT_GPIO_NO::Uart3_TXD;

	//BYTE bySend[1024] = {0x01};
	//BYTE byRecv[1024] = {0x00};
	//bySend[0] = 1;  //write
	//bySend[1] = 1;  //GPIO NO.

	PC_ADC_T pc_adc_t;
	memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
	pc_adc_t.nChannel = 0;	

	//拉高
	DataSend.iGPIOValue = BBAT_GPIO_VALUE::PullUp;
	res = SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), nullptr, 0, TIMEOUT_3S);
	_UiSendMsg("SetUART3", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, nullptr, -1, nullptr, "GPIO = %d; Value = %d", DataSend.iGPIONumber, DataSend.iGPIOValue);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}

	res = SP_GetADC(m_hDUT, &pc_adc_t);
	if (res != SP_OK)
	{
		_UiSendMsg("SendADCCmd", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "Channel = %d", pc_adc_t.nChannel);
		return res;
	}
    else
    {	
		_UiSendMsg("ReadADC", LEVEL_ITEM, 1000, pc_adc_t.adc, 3000, nullptr, -1, nullptr, "Channel = %d", pc_adc_t.nChannel);
		if(pc_adc_t.adc < 1000 || pc_adc_t.adc > 3000) //mV
        {
            return SP_E_BBAT_ADC_VALUE_FAIL;
        }
    }

	return res;
}

SPRESULT CUIS6760_GPIO::TestLED(void)
{
	//TestGroupLED:	
	//3. reset 暂时无法控制此管脚 暂定不测试
	//4. LED_MODE 自测试 GPIO拉高拉低  GPIO号36

    SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_GPIO;
	GPIOWriteSend DataSendWrite;
	DataSendWrite.iGPIONumber = BBAT_GPIO_NO::LED_Mode;
	//BYTE bySend[1024] = {0x01};
	//BYTE byRecv[1024] = {0x00};
	//bySend[0] = 1;  //write
	//bySend[1] = 36; //GPIO NO.

	//拉低
	DataSendWrite.iGPIOValue = BBAT_GPIO_VALUE::PullDown; //0	
	res = SP_AutoBBTest(m_hDUT, SubType, &DataSendWrite, sizeof(DataSendWrite), nullptr, 0, TIMEOUT_3S);
	_UiSendMsg("SetLED", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, nullptr, -1, nullptr,
		"GPIO = %d; Value = %d", DataSendWrite.iGPIONumber, DataSendWrite.iGPIOValue);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
	
	GPIOReadSend DataSendRead;
	DataSendRead.iGPIONumber = BBAT_GPIO_NO::LED_Mode;
	GPIOReadRecv DataRecv;
	res = SP_AutoBBTest(m_hDUT, SubType, &DataSendRead, sizeof(DataSendRead), &DataRecv, sizeof(DataRecv), TIMEOUT_3S);
	if (res != SP_OK)
	{
		_UiSendMsg("GetLEDCmd", LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "GPIO = %d", DataSendRead.iGPIONumber);
		return SP_E_BBAT_CMD_FAIL;
	}
    else
    {
		_UiSendMsg("GetLED", LEVEL_ITEM, 0, DataSendRead.iOperate, 0, nullptr, -1, nullptr, "GPIO = %d", DataSendRead.iGPIONumber);
        if (DataSendRead.iOperate != 0)
        {
            return SP_E_BBAT_GPIO_VALUE_FAIL;
        }
    }

	//拉高
	DataSendWrite.iGPIOValue = BBAT_GPIO_VALUE::PullUp;
	res = SP_AutoBBTest(m_hDUT, SubType, &DataSendWrite, sizeof(DataSendWrite), nullptr, 0, TIMEOUT_3S);
	_UiSendMsg("SetLED", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, nullptr, -1, nullptr, "GPIO = %d; Value = %d", DataSendWrite.iGPIONumber, DataSendWrite.iGPIOValue);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}

	  //read
	res = SP_AutoBBTest(m_hDUT, SubType,& DataSendRead, sizeof(DataSendRead), &DataRecv, sizeof(DataRecv), TIMEOUT_3S);
	if (res != SP_OK)
	{
		_UiSendMsg("GetLEDCmd", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "GPIO = %d", DataSendRead.iGPIONumber);
		return SP_E_BBAT_CMD_FAIL;
	}
    else
    {
		_UiSendMsg("GetLED", LEVEL_ITEM, 1, DataRecv.iGPIONumber, 1, nullptr, -1, nullptr, "GPIO = %d", DataSendRead.iGPIONumber);
        if (DataRecv.iGPIONumber != 1)
        {
            return SP_E_BBAT_GPIO_VALUE_FAIL;
        }
    }

	return res;
}

SPRESULT CUIS6760_GPIO::TestPA_LNA(void)
{
	//TestGroupPA_LNA:	
	//3. reset 暂时无法控制此管脚 暂定不测试
	//4. LED_MODE 自测试 GPIO拉高拉低  GPIO号36
	/*
	DBC需求	EXT_PA_SET1	7E xx xx xx xx 09 00 38 17 11 01 7E	PA 拉高	
		7E xx xx xx xx 08 00 38 xx 7E.		00 pass  01 fail
	EXT_PA_SET0	7E xx xx xx xx 09 00 38 17 11 00 7E	PA 拉低	
		7E xx xx xx xx 09 00 38 00 7E		00 pass  01 fail
	EXT_LNA_READ	7E xx xx xx xx 09 00 38 17 11 02 7E	LNA读取	
		7E xx xx xx xx 09 00 38 00 xx 7E		xx 00为读到低，01为读到高
	*/
    SPRESULT res = SP_OK;

	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_RESERVE_5;
	BYTE bySend[1024] = {0x01};
	BYTE byRecv[1024] = {0x00};
	bySend[0] = 17; //write
	bySend[1] = 4;  //GPIO NO.
	
	//拉低
	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_3S);
	_UiSendMsg("SetLNA", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Value = 0");
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
	Sleep(200);
	//read
	eDeviceCmd = DEVICE_AUTOTEST_GPIO;
	bySend[0] = 0;  //write
	bySend[1] = 36; //GPIO NO.
	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_3S);
	if (res != SP_OK)
	{
		_UiSendMsg("Get LED Value", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
		return SP_E_BBAT_CMD_FAIL;
	}
    else
    {
		_UiSendMsg("Get LED Value", LEVEL_ITEM, 0, byRecv[0], 0, NULL, -1, NULL);
        if (byRecv[0] != 0)
        {
            return SP_E_BBAT_GPIO_VALUE_FAIL;
        }
    }
	Sleep(500);

	//拉高
	eDeviceCmd = DEVICE_AUTOTEST_RESERVE_5;
	bySend[0] = 17; //write
	bySend[1] = 3; //GPIO NO.
	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_3S);
	_UiSendMsg("SetLNA", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Value = 1");
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
	Sleep(200);
	//read
	eDeviceCmd = DEVICE_AUTOTEST_GPIO;
	bySend[0] = 0;  //write
	bySend[1] = 36; //GPIO NO.
	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_3S);
	if (res != SP_OK)
	{
		_UiSendMsg("Get LED Value", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
		return SP_E_BBAT_CMD_FAIL;
	}
    else
    {
		_UiSendMsg("Get LED Value", LEVEL_ITEM, 1, byRecv[0], 1, NULL, -1, NULL);
        if (byRecv[0] != 1)
        {
            return SP_E_BBAT_GPIO_VALUE_FAIL;
        }
    }
	return res;
}
