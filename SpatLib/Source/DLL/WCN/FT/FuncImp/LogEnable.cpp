#include "StdAfx.h"
#include <cassert>
#include<stdio.h>
#include "LogEnable.h"

IMPLEMENT_RUNTIME_CLASS(CLogEnable)

CLogEnable::CLogEnable(void)
: m_bCp2En(FALSE)
, m_bArmEn(FALSE)
{
}

CLogEnable::~CLogEnable(void)
{
}

SPRESULT CLogEnable::__InitAction(void)
{
	CHKRESULT(__super::__InitAction());

	return SP_OK;
}

BOOL CLogEnable::LoadXMLConfig(void)
{
    /*CHKRESULT*/(__super::LoadXMLConfig());

	wstring strLogSwitcher = L"Option:";

	//Log
	m_bArmEn = (BOOL)GetConfigValue((strLogSwitcher + L"ArmLog:Arm").c_str(), FALSE);
	m_bCp2En = (BOOL)GetConfigValue((strLogSwitcher + L"CP2Log:CP2").c_str(), FALSE);

	return TRUE;
}


SPRESULT CLogEnable::__PollAction( void )
{
    SetRepairMode(RepairMode_Bluetooth);
 // CHKRESULT(ChangeDUTRunMode());// 2018/03/22 @JXP This has been done by CSpatBase::PollAction

	if (m_bArmEn)
	{
		_UiSendMsg("EnableArmLog", LEVEL_ITEM, 1, 1, 1, "", -1, "-");
		if (SP_EnableArmLog(m_hDUT, true))
		{
			return SP_E_FAIL;
		}
	}

	if (m_bCp2En)
	{
		_UiSendMsg("EnableCp2Log", LEVEL_ITEM, 1, 1, 1, "", -1, "-");
		if (!EnableWcnLogCmd(m_bCp2En))
		{
			return SP_E_FAIL;
		}
	}
	
	return SP_OK;
}

void CLogEnable::__LeaveAction( void )
{
}

SPRESULT CLogEnable::__FinalAction(void)
{
    return SP_OK;
}

BOOL CLogEnable::EnableWcnLogCmd(BOOL bOn)
{
	ZeroMemory(m_szRecvBuf, sizeof(m_szRecvBuf));
	ZeroMemory(m_szCmd, sizeof(m_szCmd));

	sprintf_s(m_szCmd, sizeof(m_szCmd), "AT+LOGCTL=WCN,%d", bOn);

	if (SendCommandAndRecvResponse(m_szCmd, m_szRecvBuf, sizeof(m_szRecvBuf), 3000))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CLogEnable::SendCommandAndRecvResponse( 
	const char *lpszCmd, 
	char *lpszBuf /* OUT */, 
	unsigned long nBufSize,
	unsigned long ulTimeOut /* = TIMEOUT_3S */, 
	unsigned int nRepeatCount /* = 3 */ 
	)
{

	char    szRecvBuf[2048] = {0};
	unsigned int nOperCount =  0;
	BOOL bOperRes = FALSE;
	do 
	{
		ZeroMemory((void *)lpszBuf,   nBufSize);
		ZeroMemory((void *)szRecvBuf, sizeof(szRecvBuf));
		if ((SP_SendATCommand(m_hDUT, lpszCmd, true, szRecvBuf, sizeof(szRecvBuf), NULL, ulTimeOut)))
		{
			continue ;
		}

		//char szHeader[100] = {0};
		//char szTailer[100] = {0}; 
	
		//sscanf_s(szRecvBuf, "+%[^:]:%s", szHeader, lpszBuf);
		//strcpy_s(szTailer, sizeof(szTailer), "OK");

		if (NULL != strstr(szRecvBuf, "LOGCTL") && NULL != strstr(szRecvBuf, "OK"))
		{
			bOperRes = TRUE;
			break;
		}
		else
		{
			continue;
		}

	} while(++nOperCount <= nRepeatCount);

	return bOperRes;
}