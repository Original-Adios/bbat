#include "StdAfx.h"
#include "NTC_ADC.h"

//
IMPLEMENT_RUNTIME_CLASS(CNTC_ADC)

//////////////////////////////////////////////////////////////////////////
CNTC_ADC::CNTC_ADC(void)
:m_bADC2(0),
m_bADC4(0),
m_bADC6(0)
{
    for (int iCount = 0; iCount < 3; iCount++)
    {
        Upspec[iCount] = 0;
        Downspec[iCount] = 0;
    }
}

CNTC_ADC::~CNTC_ADC(void)
{
}
SPRESULT CNTC_ADC::__PollAction(void)
{
	SPRESULT res = SP_OK;	
	
	
	PC_ADC_T pc_adc_t;

	if(m_bADC2)
	{
		memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
		pc_adc_t.nChannel = 2;	
		res = SP_GetADC(m_hDUT, &pc_adc_t);
		if (res != SP_OK)
		{
			NOTIFY("SendADCCmd", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Channel = %d", pc_adc_t.nChannel);
			return res;
		}
		else
		{	
			NOTIFY("GetADC", LEVEL_ITEM, Downspec[0], pc_adc_t.adc, Upspec[0], NULL, -1, NULL, "Channel = %d,Value = %d,Spec:%d - %d", pc_adc_t.nChannel,pc_adc_t.adc,Downspec[0],Upspec[0]);
			if(pc_adc_t.adc < Downspec[0] || pc_adc_t.adc > Upspec[0]) //mV
			{
				return SP_E_BBAT_ADC_VALUE_FAIL;
			}
		}
	}
	if(m_bADC4)
	{
		Sleep(200);

		memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
		pc_adc_t.nChannel = 4;	
		res = SP_GetADC(m_hDUT, &pc_adc_t);
		if (res != SP_OK)
		{
			NOTIFY("SendADCCmd", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Channel = %d", pc_adc_t.nChannel);
			return res;
		}
		else
		{	
			NOTIFY("GetADC", LEVEL_ITEM, Downspec[1], pc_adc_t.adc, Upspec[1], NULL, -1, NULL, "Channel = %d,Value = %d,Spec:%d - %d", pc_adc_t.nChannel,pc_adc_t.adc,Downspec[1],Upspec[1]);
			if(pc_adc_t.adc < Downspec[1] || pc_adc_t.adc > Upspec[1]) //mV
			{
				return SP_E_BBAT_ADC_VALUE_FAIL;
			}
		}
	}

	if(m_bADC6)
	{
		Sleep(200);

		memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
		pc_adc_t.nChannel = 6;	
		res = SP_GetADC(m_hDUT, &pc_adc_t);
		if (res != SP_OK)
		{
			NOTIFY("SendADCCmd", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Channel = %d", pc_adc_t.nChannel);
			return res;
		}
		else
		{	
			NOTIFY("GetADC", LEVEL_ITEM, Downspec[2], pc_adc_t.adc, Upspec[2], NULL, -1, NULL, "Channel = %d,Value = %d,Spec:%d - %d", pc_adc_t.nChannel,pc_adc_t.adc,Downspec[2],Upspec[2]);
			if(pc_adc_t.adc < Downspec[2] || pc_adc_t.adc > Upspec[2]) //mV
			{
				return SP_E_BBAT_ADC_VALUE_FAIL;
			}
		}
	}
	return SP_OK;

}
BOOL CNTC_ADC::LoadXMLConfig(void)
{
	m_bADC2 = GetConfigValue(L"Option:ADCI2:Select", 0);
	m_bADC4 = GetConfigValue(L"Option:ADCI4:Select", 0);
	m_bADC6 = GetConfigValue(L"Option:ADCI6:Select", 0);
	Upspec[0] = GetConfigValue(L"Option:ADCI2:Upspec", 0);
	Downspec[0] = GetConfigValue(L"Option:ADCI2:Downspec", 0);	
	Upspec[1] = GetConfigValue(L"Option:ADCI4:Upspec", 0);
	Downspec[1] = GetConfigValue(L"Option:ADCI4:Downspec", 0);	
	Upspec[2] = GetConfigValue(L"Option:ADCI6:Upspec", 0);
	Downspec[2] = GetConfigValue(L"Option:ADCI6:Downspec", 0);	
	return TRUE;
}
