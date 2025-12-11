#include "StdAfx.h"
#include "NormalModeToCalMode.h"
#include "SharedDefine.h"
#include <atlconv.h>

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CNormalModeToCalMode)

///
CNormalModeToCalMode::CNormalModeToCalMode(void)
: m_bNormalToCalMode(TRUE)
{
}

CNormalModeToCalMode::~CNormalModeToCalMode(void)
{
}

BOOL CNormalModeToCalMode::LoadXMLConfig(void)
{
	LPCWSTR lpszMode = GetConfigValue(L"Option:ModeSwitch", L"NormalToCalMode");
	if (NULL == lpszMode)
	{
		return FALSE;
	}

	if (0 == _wcsicmp(lpszMode, L"NormalToCalMode"))
	{
		m_bNormalToCalMode = TRUE;
	}
	else
	{
		m_bNormalToCalMode = FALSE;
	}

	lpszMode = GetConfigValue(L"Option:TestMode", L"GSM Calibration");
	if (NULL == lpszMode)
	{
		return FALSE;
	}

	BOOL bValid = FALSE;
	for (INT i=0; i< MAX_DUT_MODE; i++)
	{
		if (0 == _wcsicmp(lpszMode, M_DUT_MODE[i].name))
		{
			bValid  = TRUE;
			m_Options.m_eMode = M_DUT_MODE[i].mode;
			break;
		}
	}

	if (!bValid)
	{
		return FALSE;
	}

	USES_CONVERSION;
	std::wstring wstrtmp = GetConfigValue(L"Option:AtCmd:NormalModeToCalMode", L"AT+MODECHAN=0,0");
	m_strNormalToCal = _W2CA(wstrtmp.c_str());

	wstrtmp = GetConfigValue(L"Option:AtCmd:CalModeToNormalMode", L"AT+MODECHAN=0,2");
	m_strCalToNormal = W2A(wstrtmp.c_str());

    return TRUE;
}

SPRESULT CNormalModeToCalMode::__PollAction(void)
{
#define BUFFSIZE  1024
	SPRESULT resEnterMode = SP_E_SPAT_ENTER_MODE;

	char szRecvBuf[BUFFSIZE] = {0};
	unsigned long recvLen = 4000;
	uint32 realRecv = 0;
	char szATCmd[BUFFSIZE] = {0};

	if (m_bNormalToCalMode)
	{
		BOOL bNormal = FALSE;
		for (int i = 0; i < 5; i++)
		{
			//不管怎样，第一次都先进入正常模式，再进校准模式。
			if (!bNormal)
			{
				sprintf_s(szATCmd, "%s\r\n", m_strCalToNormal.c_str());
				SPRESULT res = SP_SendATCommand(m_hDUT, szATCmd, 1, szRecvBuf, recvLen, &realRecv, 3000);

				if ((res == SP_OK) && strstr(szRecvBuf, "OK"))
				{
					bNormal = TRUE;
				}
				else
				{
					Sleep(1000);
					continue;
				}
			}

			if (bNormal)
			{
				sprintf_s(szATCmd, "%s\r\n", m_strNormalToCal.c_str());
				SPRESULT res = SP_SendATCommand(m_hDUT, szATCmd, 1, szRecvBuf, recvLen, &realRecv, 3000);

				if ((res == SP_OK) && strstr(szRecvBuf, "OK"))
				{
					bNormal = FALSE;
					resEnterMode = SP_OK;
					break;
				}
				else
				{
					Sleep(1000);
					continue;
				}
			}
		}
		if (resEnterMode == SP_OK)
		{
			if (m_Options.m_eMode == RM_CALIBRATION_MODE )
			{
				resEnterMode = SP_tdCalSwitchTogsmCal(m_hDUT); 
			}
			else if (m_Options.m_eMode == RM_TD_CALIBRATION_MODE)
			{
				resEnterMode = SP_gsmCalSwitchToTDCal(m_hDUT);
			}

			if (SP_OK == resEnterMode)
			{
				CHKRESULT_WITH_NOTIFY(m_pModeSwitch->SetCurrentMode(m_Options.m_eMode), "SetCurrentMode");
			}
		}
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			sprintf_s(szATCmd, "%s\r\n", m_strCalToNormal.c_str());
			SPRESULT res = SP_SendATCommand(m_hDUT, szATCmd, 1, szRecvBuf, recvLen, &realRecv, 3000);

			if ((res == SP_OK) && strstr(szRecvBuf, "OK"))
			{
				resEnterMode = SP_OK;
				break;
			}
			else
			{
				Sleep(1000);

				continue;
			}
		}

	}

	if (m_bNormalToCalMode)
	{
		NOTIFY("NormalToCalMode", LEVEL_UI, 1, (SP_OK == resEnterMode) ? 1 : 0, 1, NULL, -1, NULL, NULL);
	}
	else
	{
		NOTIFY("CalToNormalMode", LEVEL_UI, 1, (SP_OK == resEnterMode) ? 1 : 0, 1, NULL, -1, NULL, NULL);
	}


    return resEnterMode;
}
