#include "StdAfx.h"
#include "DeActiveArmLog.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CDeActiveArmLog)

///
CDeActiveArmLog::CDeActiveArmLog(void)
{
}

CDeActiveArmLog::~CDeActiveArmLog(void)
{
}

SPRESULT CDeActiveArmLog::__PollAction(void)
{
	//Add load SystemConfig.ini  2021.12.20    
	extern CActionApp myApp;
	std::wstring strIniPath = myApp.GetSysPath();
	strIniPath += L"\\..\\Setting\\SystemConfig.ini";

	//Armlog TimeOut  2021.12.20
	m_nArmlogTimeOut = GetPrivateProfileIntW(L"ArmLog", L"TIMEOUT", TIMEOUT_30S, strIniPath.c_str());

	// Wait some time for logel dump out of DUT buffer  
	CONST DWORD LOGEL_DUMP_TIME = GetConfigValue(L"GLOBAL:LOGEL_DUMP_TIME", m_nArmlogTimeOut, TRUE);

	NOTIFY("LogelDumping", LEVEL_UI, 1, 1, 1, NULL, -1, NULL, "Waiting %d secs", LOGEL_DUMP_TIME / 1000);

	if (NULL != GetAdjParam().hStopEvent)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(GetAdjParam().hStopEvent, LOGEL_DUMP_TIME))
		{
			return SP_E_USER_ABORT;
		}
	}
	else
	{
		Sleep(LOGEL_DUMP_TIME);
	}

	uint8 recvbuf[64] = { 0 };
	SP_SendATCommand(m_hDUT, "AT+SPDSPOP=0", TRUE, recvbuf, sizeof(recvbuf), NULL, TIMEOUT_2S);
	SP_SendATCommand(m_hDUT, "AT+ARMLOG=0", TRUE, recvbuf, sizeof(recvbuf), NULL, TIMEOUT_2S);

	NOTIFY("DeActiveLogel", LEVEL_UI, 1, 1, 1);

    return SP_OK;
}
