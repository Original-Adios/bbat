#include "StdAfx.h"
#include "OpenSocket.h"
#include "ModeSwitch.h"
#include "Winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#include "ShareMemoryDefine.h"
#include "NetUtility.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(COpenSocket)

	///
COpenSocket::COpenSocket(void)
: m_dwPort(8080)
{
    m_strIP = "127.0.0.1";
	m_bPing = FALSE;
	m_nPingTimeOut = 30000;
	m_nPingCount = 3;
}

COpenSocket::~COpenSocket(void)
{
}

BOOL COpenSocket::LoadXMLConfig(void)
{
	m_dwPort = GetConfigValue(L"Option:Port", 8080);
	m_strIP = _W2CA(GetConfigValue(L"Option:IP", L"127.0.0.1"));
	m_bPing = GetConfigValue(L"Param:Ping:Enable", FALSE);
	m_nPingTimeOut = GetConfigValue(L"Param:Ping:TimeOut", 30000);
	m_nPingCount = GetConfigValue(L"Param:Ping:Count", 3);
	return TRUE;
}

SPRESULT COpenSocket::__PollAction(void)
{
    std::string strIP = m_strIP;
    DWORD dwPort = m_dwPort;

    IP_T ip;
    if (SP_OK == GetShareMemory(ShareMemory_IP, &ip, sizeof(ip)))
    {
        strIP  = ip.szIP;
        dwPort = ip.dwPort;
    }
	if(m_bPing)
	{
		CSPTimer timer;
		int nCount = 0;
		do
		{
			if(_IsUserStop())
			{
				return SP_E_USER_ABORT;
			}
			if(!CNetUtility::ICMPPing(strIP.c_str()))
			{
				LogFmtStrA(SPLOGLV_ERROR, "Ping Fail");
				nCount = 0;
				continue;
			}
			else
			{
				nCount++;
				if(nCount > m_nPingCount)
				{
					break;
				}
			}
		}while(!timer.IsTimeOut(m_nPingTimeOut));
	}

	CHANNEL_ATTRIBUTE ca;
	memset(&ca, 0, sizeof(CHANNEL_ATTRIBUTE));
	ca.ChannelType = CHANNEL_TYPE_SOCKET;
	ca.Socket.dwPort = dwPort;
	ca.Socket.dwIP = ntohl(inet_addr(strIP.c_str()));
	ca.Socket.dwFlag = SC_TYPE_CLIENT;

	SPRESULT res = SP_BeginPhoneTest(m_hDUT, (CHANNEL_ATTRIBUTE* )&ca);
	if (SP_OK == res)
	{
        // Force to GSM CAL mode
        CHKRESULT_WITH_NOTIFY(m_pModeSwitch->SetCurrentMode(RM_CALIBRATION_MODE), "SetCurrentMode");
		NOTIFY("OpenSocket", LEVEL_UI, 1, 1,1, NULL, -1, NULL, "IP: %s, Port: %d", strIP.c_str(), m_dwPort);
	}
	else
	{
		NOTIFY("OpenSocket", LEVEL_UI, 1, 0,1, NULL, -1, NULL, "IP: %s, Port: %d", strIP.c_str(), m_dwPort);
	}

	return res;
}