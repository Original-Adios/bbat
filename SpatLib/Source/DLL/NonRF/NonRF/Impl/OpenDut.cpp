#include "StdAfx.h"
#include "OpenDut.h"
#include "ModeSwitch.h"
#include "Winsock2.h"
#pragma comment(lib, "ws2_32.lib")

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(COpenDut)

///
COpenDut::COpenDut(void)
: m_eMode(RM_INVALID_MODE)
, m_eType(DUT_CON_AUTO)
{
}

COpenDut::~COpenDut(void)
{
}

BOOL COpenDut::LoadXMLConfig(void)
{
    LPCWSTR lpszMode = GetConfigValue(L"Option:TestMode", L"GSM Calibration");
    if (NULL == lpszMode)
    {
        return FALSE;
    }

    m_eMode = RM_INVALID_MODE;
    for (INT i=0; i<MAX_DUT_MODE; i++)
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
		if(0 == _wcsicmp(lpszAuto, L"Auto"))
		{
			m_eType = DUT_CON_AUTO;
		}
		else if(0 == _wcsicmp(lpszAuto, L"Uart"))
		{
			m_eType = DUT_CON_UART;
		}
		else
		{
			m_eType = DUT_CON_SOCKET;
		}

    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

SPRESULT COpenDut::__PollAction(void)
{
    CHANNEL_ATTRIBUTE   ca;
    ZeroMemory((void* )&ca, sizeof(ca));

    if (DUT_CON_AUTO == m_eType)
    {
        ca = GetAdjParam().ca;
        if (ca.ChannelType != CHANNEL_TYPE_COM && ca.ChannelType != CHANNEL_TYPE_SOCKET && ca.ChannelType != CHANNEL_TYPE_USBMON)
        {
            NOTIFY("INVALID CON TYPE", LEVEL_UI, 1, 0, 1);
            return SP_E_SPAT_INVALID_PARAMETER;
        }
        if (ca.ChannelType == CHANNEL_TYPE_USBMON)
        {
            ca.ChannelType = CHANNEL_TYPE_COM;
        }
    }
    else if(DUT_CON_UART == m_eType)
    {
        ca.ChannelType    = CHANNEL_TYPE_COM;
        ca.Com.dwPortNum  = GetConfigValue(L"Option:Port::Uart:Port", 1);
        ca.Com.dwBaudRate = GetConfigValue(L"Option:Port:Uart:Baudrate", 115200);
        if (0 == ca.Com.dwPortNum)
        {
            NOTIFY("InvalidPort", LEVEL_UI, 1, 0, 1);
            return SP_E_SPAT_INVALID_PARAMETER;
        }
    }
	else
	{
		ca.ChannelType    = CHANNEL_TYPE_SOCKET;
		ca.Socket.dwPort  = GetConfigValue(L"Option:Port:Socket:Port", 1);
		string strIP = _W2CA(GetConfigValue(L"Option:Port:Socket:IP", L"127.0.0.1"));
		ca.Socket.dwIP =  ntohl(inet_addr(strIP.c_str()));
		ca.Socket.dwFlag = SC_TYPE_CLIENT;
		if (0 == ca.Socket.dwIP || 0 == ca.Socket.dwPort)
		{
			NOTIFY("InvalidSocketSet", LEVEL_UI, 1, 0, 1);
			return SP_E_SPAT_INVALID_PARAMETER;
		}
	}

    SPRESULT res = SP_BeginPhoneTest(m_hDUT, &ca);
    if (SP_OK == res)
    {
        CHKRESULT_WITH_NOTIFY(m_pModeSwitch->SetCurrentMode(m_eMode), "SetCurrentMode");
        CHKRESULT_WITH_NOTIFY(m_pModeSwitch->ActiveLogels(), "ActiveLogel");
    }
	if (ca.ChannelType == CHANNEL_TYPE_COM)
	{
		NOTIFY("OpenUART", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Port %d, %d bps", ca.Com.dwPortNum, ca.Com.dwBaudRate);
	}
	else if(ca.ChannelType == CHANNEL_TYPE_SOCKET)
	{
		NOTIFY("OpenSocket", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "IP %d.%d.%d.%d, Port %d", 
			(ca.Socket.dwIP >> 24) & 0xFF, (ca.Socket.dwIP >> 16) & 0xFF, (ca.Socket.dwIP >> 8) & 0xFF, (ca.Socket.dwIP) & 0xFF,ca.Socket.dwPort);
	}
    return res;
}

