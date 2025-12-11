#include "StdAfx.h"
#include "Get_ADC.h"

//
IMPLEMENT_RUNTIME_CLASS(CGet_ADC)

//////////////////////////////////////////////////////////////////////////
CGet_ADC::CGet_ADC(void)
{
}

CGet_ADC::~CGet_ADC(void)
{
}

SPRESULT CGet_ADC::__PollAction(void)
{
	SPRESULT res = SP_OK;
	
	PC_ADC_T pc_adc_t;
	memset(&pc_adc_t, 0, sizeof(PC_ADC_T));
	pc_adc_t.nChannel = m_nChannel;

	res = SP_GetADC(m_hDUT, &pc_adc_t);
	if (res != SP_OK)
	{
		NOTIFY("SendADCCmd", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Channel = %d", pc_adc_t.nChannel);
		return res;
	}
	else
    {	
		NOTIFY("GetADC", LEVEL_ITEM, m_nDownSpec, pc_adc_t.adc, m_nUpSpec, NULL, -1, NULL, "Channel = %d", pc_adc_t.nChannel);
		if(pc_adc_t.adc < m_nDownSpec || pc_adc_t.adc > m_nUpSpec) //mV
        {
            return SP_E_BBAT_ADC_VALUE_FAIL;
        }
    }
    return res;
}
BOOL CGet_ADC::LoadXMLConfig(void)
{
    m_nChannel = (uint8)GetComboxValue(m_lpwAdcChannel, ADC_CHANNEL_MAX, L"Option:Channel");
	if (MAX_FLASH == m_nChannel)
	{
		return FALSE;
	}
	m_nUpSpec = GetConfigValue(L"Option:UpSpec", 0);
	m_nDownSpec = GetConfigValue(L"Option:DownSpec", 0);
	return TRUE;
}