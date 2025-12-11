#include "StdAfx.h"
#include "WaitOpenPort.h"
#include "ModeSwitch.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CWaitOpenPort)

///
CWaitOpenPort::CWaitOpenPort(void)
	: m_eMode(RM_INVALID_MODE)
	, m_bAutoSetup(TRUE)
{
}

CWaitOpenPort::~CWaitOpenPort(void)
{
}

BOOL CWaitOpenPort::LoadXMLConfig(void)
{
	LPCWSTR lpszMode = GetConfigValue(L"Option:TestMode", L"GSM Calibration");
	if (NULL == lpszMode)
	{
		return FALSE;
	}

	m_eMode = RM_INVALID_MODE;
	for (INT i = 0; i < MAX_DUT_MODE; i++)
	{
		if (0 == _wcsicmp(lpszMode, M_DUT_MODE[i].name))
		{
			m_eMode = M_DUT_MODE[i].mode;
			break;
		}
	}

	if (RM_INVALID_MODE == m_eMode)
	{
		return FALSE;
	}

	LPCWSTR lpszAuto = GetConfigValue(L"Option:Port:Setting", L"Auto");
	if (NULL != lpszAuto)
	{
		m_bAutoSetup = (0 == _wcsicmp(lpszAuto, L"Auto")) ? TRUE : FALSE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

SPRESULT CWaitOpenPort::__PollAction(void)
{
	CHANNEL_ATTRIBUTE   ca;
	ZeroMemory((void*)&ca, sizeof(ca));

	if (m_bAutoSetup)
	{
		ca = GetAdjParam().ca;
		if (ca.ChannelType != CHANNEL_TYPE_COM && ca.ChannelType != CHANNEL_TYPE_USBMON)
		{
			NOTIFY("NOT UART", LEVEL_UI, 1, 0, 1);
			return SP_E_SPAT_INVALID_PARAMETER;
		}
		ca.ChannelType = CHANNEL_TYPE_COM;
	}
	else
	{
		ca.ChannelType = CHANNEL_TYPE_COM;
		ca.Com.dwPortNum = GetConfigValue(L"Option:Port:Port", 1);
		ca.Com.dwBaudRate = GetConfigValue(L"Option:Port:Baudrate", 115200);
		if (0 == ca.Com.dwPortNum)
		{
			NOTIFY("InvalidPort", LEVEL_UI, 1, 0, 1);
			return SP_E_SPAT_INVALID_PARAMETER;
		}
	}

	for (;;)
	{
		HANDLE  hCom = NULL;
		char szComPort[16] = { 0 };
		sprintf_s(szComPort, sizeof(szComPort), "\\\\.\\COM%d", ca.Com.dwPortNum);
		hCom = CreateFileA(
			szComPort,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			NULL
		);

		if (INVALID_HANDLE_VALUE != hCom)
		{
			if (CloseHandle(hCom))
			{
				break;
			}
		}
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}

		Sleep(1000);
	}

	SendWorkStatus();
	SPRESULT res = SP_BeginPhoneTest(m_hDUT, &ca);
	if (SP_OK == res)
	{
		CHKRESULT_WITH_NOTIFY(m_pModeSwitch->SetCurrentMode(m_eMode), "SetCurrentMode");
		CHKRESULT_WITH_NOTIFY(m_pModeSwitch->ActiveLogels(), "ActiveLogel");
	}

	NOTIFY("OpenUART", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Port %d, %d bps", ca.Com.dwPortNum, ca.Com.dwBaudRate);
	return res;
}

