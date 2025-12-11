#include "StdAfx.h"
#include "UMP5865_MIC.h"

//
IMPLEMENT_RUNTIME_CLASS(CUMP5865_MIC)

//////////////////////////////////////////////////////////////////////////
CUMP5865_MIC::CUMP5865_MIC(void)
{
	m_iPeakUpSpec = 0;
	m_iPeakDownSpec = 0;
}

CUMP5865_MIC::~CUMP5865_MIC(void)
{
}

SPRESULT CUMP5865_MIC::__PollAction(void)
{
	SPRESULT res = SP_OK;
    BYTE bySend[1024] = {0x01};
    BYTE byRecv[1024] = {0x00};
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_RESERVE_7;
	bySend[0]=1;  
    res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 1, byRecv, 2, Timeout_PHONECOMMAND_BBAT);
    NOTIFY("UMP5865_MIC", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Cmd = %d", eDeviceCmd);
    if(res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
	
    NOTIFY("UMP5865_MIC", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Receive Data:%d,%d", byRecv[0],byRecv[1]);
	return res;
}
BOOL CUMP5865_MIC::LoadXMLConfig(void)
{
	m_iPeakUpSpec = GetConfigValue(L"Option:Mic_Peak_UpSepc", 1);

	if (m_iPeakUpSpec < 0)
    {
        return FALSE;
    }
	m_iPeakDownSpec = GetConfigValue(L"Option:Mic_Peak_DownSepc", 1);

	if (m_iPeakDownSpec < 0)
    {
        return FALSE;
    }
	return TRUE;
}