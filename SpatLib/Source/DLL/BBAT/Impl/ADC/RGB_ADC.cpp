#include "StdAfx.h"
#include "RGB_ADC.h"

//
IMPLEMENT_RUNTIME_CLASS(CRGB_ADC)

//////////////////////////////////////////////////////////////////////////
CRGB_ADC::CRGB_ADC(void)
{
	for(int iCount = 0;iCount<3;iCount++)
	{
		m_bRGB[iCount]=0;
		ADC_Channel[iCount]=0;
		Upspec[iCount]=0;
		Downspec[iCount]=0;
	}
	m_bKeyOut1 = 0;
	m_iDeltaSpec = 100;
}

CRGB_ADC::~CRGB_ADC(void)
{
}
SPRESULT CRGB_ADC::__PollAction(void)
{
	SPRESULT res = SP_OK;	
	
	BYTE bySend[1024] = {0x01};
	PC_ADC_T pc_adc_t;
	//red
	if(m_bRGB[0])
	{
		//eDeviceCmd = DEVICE_AUTOTEST_MISC;
		//bySend[0]=9; //0:lcd backlight,   2:keypad backlight,  3: vibrator 9£ºrgb
		//bySend[1]=0;  //0:red 1:green 2:blue
		//bySend[2]=1; 

		//res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 3, NULL, NULL );
		//NOTIFY("RGB_ADC_Red", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Cmd = %d", eDeviceCmd);
		//if(res != SP_OK)
		//{
		//	return SP_E_BBAT_CMD_FAIL;
		//}

		CHKRESULT(RGBOperate(RED, OPEN, "RGB Red Open"));
		Sleep(500);
		CHKRESULT(GetADC(ADC_Channel[0]));
		CHKRESULT(RGBOperate(RED, CLOSE, "RGB Red Close"));
		Sleep(500);
		//memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
		//pc_adc_t.nChannel = ADC_Channel[0];	
		//res = SP_GetADC(m_hDUT, &pc_adc_t);
		//if (res != SP_OK)
		//{
		//	NOTIFY("SendADCCmd", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Channel = %d", pc_adc_t.nChannel);
		//	return res;
		//}
		//else
		//{	
		//	NOTIFY("GetADC", LEVEL_ITEM, Downspec[0], pc_adc_t.adc, Upspec[0], NULL, -1, NULL, "Channel = %d,Value = %d,Spec:%d - %d", pc_adc_t.nChannel,pc_adc_t.adc,Downspec[0],Upspec[0]);
		//	if(pc_adc_t.adc < Downspec[0] || pc_adc_t.adc > Upspec[0]) //mV
		//	{
		//		return SP_E_BBAT_ADC_VALUE_FAIL;
		//	}
		//}
		//close
		/*bySend[0]=9; 
		bySend[1]=0; 
		bySend[2]=0; 
		res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 3, NULL, NULL );
		NOTIFY("RGB_ADC_Close", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Cmd = %d", eDeviceCmd);
		if(res != SP_OK)
		{
			return SP_E_BBAT_CMD_FAIL;
		}*/
		
	}
	//green
	if(m_bRGB[1])
	{
		//eDeviceCmd = DEVICE_AUTOTEST_MISC;
		//bySend[0]=9; //0:lcd backlight,   2:keypad backlight,  3: vibrator 9£ºrgb
		//bySend[1]=1;  //0:red 1:green 2:blue
		//bySend[2]=1; 

		//res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 3, NULL, NULL );
		//NOTIFY("RGB_ADC_Green", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Cmd = %d", eDeviceCmd);
		//if(res != SP_OK)
		//{
		//	return SP_E_BBAT_CMD_FAIL;
		//}
		//Sleep(500);

		//memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
		//pc_adc_t.nChannel = ADC_Channel[1];	
		//res = SP_GetADC(m_hDUT, &pc_adc_t);
		//if (res != SP_OK)
		//{
		//	NOTIFY("SendADCCmd", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Channel = %d", pc_adc_t.nChannel);
		//	return res;
		//}
		//else
		//{	
		//	NOTIFY("GetADC", LEVEL_ITEM, Downspec[1], pc_adc_t.adc, Upspec[1], NULL, -1, NULL, "Channel = %d,Value = %d,Spec:%d - %d", pc_adc_t.nChannel,pc_adc_t.adc,Downspec[1],Upspec[1]);
		//	if(pc_adc_t.adc < Downspec[1] || pc_adc_t.adc > Upspec[1]) //mV
		//	{
		//		return SP_E_BBAT_ADC_VALUE_FAIL;
		//	}
		//}
		////close
		//bySend[0]=9; 
		//bySend[1]=0; 
		//bySend[2]=0; 
		//res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 3, NULL, NULL );
		//NOTIFY("RGB_ADC_Close", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Cmd = %d", eDeviceCmd);
		//if(res != SP_OK)
		//{
		//	return SP_E_BBAT_CMD_FAIL;
		//}
		//Sleep(500);
		CHKRESULT(RGBOperate(GREEN, OPEN, "RGB Green Open"));
		Sleep(500);
		CHKRESULT(GetADC(ADC_Channel[1]));
		CHKRESULT(RGBOperate(GREEN, CLOSE, "RGB Green Close"));
		Sleep(500);
	}
	
	//blue
	if(m_bRGB[2])
	{
		//eDeviceCmd = DEVICE_AUTOTEST_MISC;
		//bySend[0]=9; //0:lcd backlight,   2:keypad backlight,  3: vibrator 9£ºrgb
		//bySend[1]=2;  //0:red 1:green 2:blue
		//bySend[2]=1; 

		//res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 3, NULL, NULL );
		//NOTIFY("RGB_ADC_Blue", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Cmd = %d", eDeviceCmd);
		//if(res != SP_OK)
		//{
		//	return SP_E_BBAT_CMD_FAIL;
		//}
		//Sleep(500);

		//memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
		//pc_adc_t.nChannel = ADC_Channel[2];	
		//res = SP_GetADC(m_hDUT, &pc_adc_t);
		//if (res != SP_OK)
		//{
		//	NOTIFY("SendADCCmd", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Channel = %d", pc_adc_t.nChannel);
		//	return res;
		//}
		//else
		//{	
		//	NOTIFY("GetADC", LEVEL_ITEM, Downspec[2], pc_adc_t.adc, Upspec[2], NULL, -1, NULL, "Channel = %d,Value = %d,Spec:%d - %d", pc_adc_t.nChannel,pc_adc_t.adc,Downspec[2],Upspec[2]);
		//	if(pc_adc_t.adc < Downspec[2] || pc_adc_t.adc > Upspec[2]) //mV
		//	{
		//		return SP_E_BBAT_ADC_VALUE_FAIL;
		//	}
		//}
		////close
		//bySend[0]=9; 
		//bySend[1]=0; 
		//bySend[2]=0; 
		//res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 3, NULL, NULL );
		//NOTIFY("RGB_ADC_Close", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Cmd = %d", eDeviceCmd);
		//if(res != SP_OK)
		//{
		//	return SP_E_BBAT_CMD_FAIL;
		//}
		//Sleep(500);
		CHKRESULT(RGBOperate(BLUE, OPEN, "RGB Blue Open"));
		Sleep(500);
		CHKRESULT(GetADC(ADC_Channel[2]));
		CHKRESULT(RGBOperate(BLUE, CLOSE, "RGB Blue Close"));
		//Sleep(500);
	}
	
	//keyout1
	if(m_bKeyOut1)
	{
		int iOpenValue,iCloseValue;
		//eDeviceCmd = DEVICE_AUTOTEST_GPIO;
		//bySend[0] = 1;  //write
		//bySend[1] = 13;
		////¿ª
		//bySend[2] = 1;
		//res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 3, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_3S);
		//NOTIFY("Keyout1 Open", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "GPIO = %d; Value = %d", bySend[1], bySend[2]);
		//if (res != SP_OK)
		//{
		//	return SP_E_BBAT_CMD_FAIL;
		//}
		CHKRESULT(GPIOWrite(PullUp, "Keyout1 Open"));
		Sleep(500);

		memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
		pc_adc_t.nChannel = ADC_Channel[2];	
		res = SP_GetADC(m_hDUT, &pc_adc_t);
		if (res != SP_OK)
		{
			NOTIFY("SendADCCmd", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "Channel = %d", pc_adc_t.nChannel);
			return res;
		}
		iOpenValue = pc_adc_t.adc;
		
		//close
		/*bySend[2] = 0;
		res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 3, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_3S);
		NOTIFY("Keyout1 Close", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "GPIO = %d; Value = %d", bySend[1], bySend[2]);
		if (res != SP_OK)
		{
			return SP_E_BBAT_CMD_FAIL;
		}*/
		CHKRESULT(GPIOWrite(PullDown, "Keyout1 Close"));
		Sleep(500);
		
		memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
		pc_adc_t.nChannel = ADC_Channel[2];	
		res = SP_GetADC(m_hDUT, &pc_adc_t);
		if (res != SP_OK)
		{
			NOTIFY("SendADCCmd", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "Channel = %d", pc_adc_t.nChannel);
			return res;
		}
		iCloseValue = pc_adc_t.adc;

		NOTIFY("Keyout1 Delta", LEVEL_ITEM, m_iDeltaSpec, (iCloseValue - iOpenValue), 9999, nullptr, -1, nullptr, "Delta is %d,Spec is %d", (iCloseValue - iOpenValue),m_iDeltaSpec);
		if((iCloseValue - iOpenValue)<m_iDeltaSpec) //mV
		{
			return SP_E_BBAT_ADC_VALUE_FAIL;
		}
	}
	return SP_OK;

}
BOOL CRGB_ADC::LoadXMLConfig(void)
{
	//red
	m_bRGB[0] = GetConfigValue(L"Option:Red:Select", 0);
	ADC_Channel[0] = (uint8)GetConfigValue(L"Option:Red:ADC_Channel", 0);
	Upspec[0] = GetConfigValue(L"Option:Red:Upspec", 0);
	Downspec[0] = GetConfigValue(L"Option:Red:Downspec", 0);	
	//green
	m_bRGB[1] = GetConfigValue(L"Option:Green:Select", 0);
	ADC_Channel[1] = (uint8)GetConfigValue(L"Option:Green:ADC_Channel", 0);
	Upspec[1] = GetConfigValue(L"Option:Green:Upspec", 0);
	Downspec[1] = GetConfigValue(L"Option:Green:Downspec", 0);	
	//Blue
	m_bRGB[2] = GetConfigValue(L"Option:Blue:Select", 0);
	ADC_Channel[2] = (uint8)GetConfigValue(L"Option:Blue:ADC_Channel", 0);
	Upspec[2] = GetConfigValue(L"Option:Blue:Upspec", 0);
	Downspec[2] = GetConfigValue(L"Option:Blue:Downspec", 0);	
	//keyout1
	m_bKeyOut1 = GetConfigValue(L"Option:KeyOUT1:Select", 0);
	m_iDeltaSpec = GetConfigValue(L"Option:KeyOUT1:Deltaspec", 0);

	return TRUE;
}

SPRESULT CRGB_ADC::RGBOperate(int iColor, int iOperate, const char* iPrintMsg)
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_MISC;
	MiscValue DataSend;
	DataSend.iMiscIndex = BBAT_MISC::RGBLight;
	DataSend.Color = (int8)iColor;
	DataSend.Operate = (int8)iOperate;

	SPRESULT res = SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), nullptr, 0, Timeout_PHONECOMMAND_BBAT);
	if (res != SP_OK)
	{
		_UiSendMsg("RGB_ADC Fail", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, iPrintMsg);
		return SP_E_BBAT_CMD_FAIL;
	}
	_UiSendMsg("RGB_ADC Pass", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr, iPrintMsg);
	return SP_OK;
}

SPRESULT CRGB_ADC::GetADC(uint8 iChannel)
{
	PC_ADC_T pc_adc_t;
	memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
	pc_adc_t.nChannel = iChannel;
	SPRESULT res = SP_GetADC(m_hDUT, &pc_adc_t);
	if (res != SP_OK)
	{
		_UiSendMsg("SendADCCmd", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "Channel = %d", pc_adc_t.nChannel);
		return res;
	}
	else
	{
		_UiSendMsg("GetADC", LEVEL_ITEM, Downspec[0], pc_adc_t.adc, Upspec[0], nullptr, -1, nullptr, "Channel = %d,Value = %d,Spec:%d - %d", pc_adc_t.nChannel, pc_adc_t.adc, Downspec[0], Upspec[0]);
		if (pc_adc_t.adc < Downspec[0] || pc_adc_t.adc > Upspec[0]) //mV
		{
			return SP_E_BBAT_ADC_VALUE_FAIL;
		}
	}
	return SP_OK;
}

SPRESULT CRGB_ADC::GPIOWrite(int iGPIOValue, const char* iPrintMsg)
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_GPIO;
	GPIOWriteValue DataSend;
	DataSend.iGPIONumber = 13;
	DataSend.iGPIOValue = (int8)iGPIOValue;
	SPRESULT res = SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), nullptr, 0, TIMEOUT_3S);
	_UiSendMsg(iPrintMsg, LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, nullptr, -1, nullptr,
		"GPIO = %d; Value = %d", DataSend.iGPIONumber, DataSend.iGPIOValue);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
	return SP_OK;
}
