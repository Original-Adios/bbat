#include "StdAfx.h"
#include "891xADC.h"

//
IMPLEMENT_RUNTIME_CLASS(C891xADC)

//////////////////////////////////////////////////////////////////////////
C891xADC::C891xADC(void)
{
}

C891xADC::~C891xADC(void)
{
}

BOOL C891xADC::LoadXMLConfig(void)
{
	//Group Count
	m_iGroupCount = GetConfigValue(L"Param:ADC:GroupCount", 3);
	if (m_iGroupCount > MAX_ADC_GROUP)
	{
		m_iGroupCount = MAX_ADC_GROUP;
	}
	//
	for (int iGroupIndex = 0; iGroupIndex < m_iGroupCount; iGroupIndex++)
	{
		wchar_t sztemp[100];
		wsprintf(sztemp, L"Param:ADC:Group%d:Channel", iGroupIndex + 1);
		m_AdcGroup[iGroupIndex].iChannel = (uint8)GetConfigValue(sztemp, 1);
		wsprintf(sztemp, L"Param:ADC:Group%d:LowLimit", iGroupIndex + 1);
		m_AdcGroup[iGroupIndex].iLowLimit = GetConfigValue(sztemp, 1000);
		wsprintf(sztemp, L"Param:ADC:Group%d:UpLimit", iGroupIndex + 1);
		m_AdcGroup[iGroupIndex].iUpLimit = GetConfigValue(sztemp, 3000);

		if (!IN_RANGE(FIRST_ADC_CHANNEL, m_AdcGroup[iGroupIndex].iChannel, LAST_ADC_CHANNEL))
		{
			return FALSE;
		}
	}
	return TRUE;
}

SPRESULT C891xADC::GetADC(uint8 iChannel, uint8 iScale, int& dResult)
{
	
	//DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_RESERVE_6;  //15
	//unsigned char szRecv[2] = { 0 };
	//char szSend[2] = { 0 };
	//szSend[0] = iChannel; //
	//szSend[1] = iScale;	//Scale£»0£º1.250V£¬1£º2.444V£¬2£º3.333V£¬3£º5.000V£¨default£©¡£.

	SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_RESERVE_6;  //15
	ADCValue DataSend;
	DataSend.iChannel = iChannel;
	DataSend.iScale = iScale;

	ADCData DataRecv;
	memset(&DataRecv, 0xff, sizeof(DataRecv));
	res = SP_AutoBBTest(m_hDUT, SubType, &DataSend , sizeof(DataSend), &DataRecv, sizeof(DataRecv), Timeout_PHONECOMMAND_BBAT);
	if (SP_OK != res)
	{
		return res;
	}
	dResult = DataRecv.iCurrent[1] * 256 + DataRecv.iCurrent[0];
	return SP_OK;
}

SPRESULT C891xADC::__PollAction(void)
{
	for (int iGroupIndex = 0; iGroupIndex < m_iGroupCount; iGroupIndex++)
	{

		int iAdc = 0;
		SPRESULT res = GetADC(m_AdcGroup[iGroupIndex].iChannel, 3, iAdc);
		if (SP_OK != res)
		{
			_UiSendMsg("Get Adc", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "Channel = %d", m_AdcGroup[iGroupIndex].iChannel);
			return res;
		}
		//
		if (!IN_RANGE(m_AdcGroup[iGroupIndex].iLowLimit, iAdc, m_AdcGroup[iGroupIndex].iUpLimit))
		{
			_UiSendMsg("Adc out of range", LEVEL_ITEM, m_AdcGroup[iGroupIndex].iLowLimit, iAdc, m_AdcGroup[iGroupIndex].iUpLimit, nullptr, -1, nullptr, "Channel = %d", m_AdcGroup[iGroupIndex].iChannel);
			return SP_E_BBAT_ADC_VALUE_FAIL;
		}
		char szTemp[50];
		sprintf_s(szTemp, "Get ADC[Channel=%d]", m_AdcGroup[iGroupIndex].iChannel);
		_UiSendMsg(szTemp, LEVEL_ITEM, m_AdcGroup[iGroupIndex].iLowLimit, iAdc, m_AdcGroup[iGroupIndex].iUpLimit, nullptr, -1, nullptr, "");
	}
	return SP_OK;
}

