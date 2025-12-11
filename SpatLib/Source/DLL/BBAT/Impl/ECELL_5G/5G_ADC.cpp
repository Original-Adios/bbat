#include "StdAfx.h"
#include "5G_ADC.h"

//
IMPLEMENT_RUNTIME_CLASS(C5G_ADC)

//////////////////////////////////////////////////////////////////////////
//Bug 1225316 
C5G_ADC::C5G_ADC(void)
{
	
}

C5G_ADC::~C5G_ADC(void)
{
}

SPRESULT C5G_ADC::__PollAction(void)
{
	SPRESULT res = SP_OK;
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_RESERVE_5;
	BYTE bySend[1024] = {0x01};
	BYTE byRecv[1024] = {0x00};
	bySend[0] = 16;  //
	bySend[1] = 5; //

	res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 2, (LPVOID)byRecv, sizeof(byRecv), TIMEOUT_5S);
	NOTIFY("5G_ADC", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, NULL);
	if (res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
	
    return res;
}
BOOL C5G_ADC::LoadXMLConfig(void)
{
	return TRUE;
}