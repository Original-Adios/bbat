#include "StdAfx.h"
#include "SocketCon.h"
//#include "Jig.h"

//
//////////////////////////////////////////////////////////////////////////
CSocketCon::CSocketCon(void)
{
    m_sClient = NULL;
    memset(&m_serAddr, 0, sizeof(m_serAddr));
}

CSocketCon::~CSocketCon(void)
{
}

int CSocketCon::Connect(const std::string strIP/* ="127.0.0.1" */, UINT dwPort/* =8300 */)
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
    memset(&data, 0, sizeof(data));

	if(WSAStartup(sockVersion, &data)!=0)
	{
		return -1;
	}
	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sClient == INVALID_SOCKET)
	{
		return -999999;
	}

	sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons((u_short)dwPort);
	serAddr.sin_addr.S_un.S_addr = inet_addr(strIP.c_str());
	
	if(connect(m_sClient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{  
		closesocket(m_sClient);
		return -1;
	}

	return 0;
}

void CSocketCon::Disconnect()
{
	closesocket(m_sClient);
	WSACleanup();
}

int CSocketCon::Send(const std::string strData)
{
	if (m_sClient == NULL)
	{
		return -999999;
	}

	//char szBuff[1024] = {0};
	int ret = send(m_sClient, strData.c_str(), strData.length(), 0);
	if (ret == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	return 0;
}

int CSocketCon::Receive(std::string &strData)
{
	strData = "";
	if (m_sClient == NULL)
	{
		return -999999;;
	}

	char szBuff[1024] = {0};
	int ret = recv(m_sClient, szBuff, sizeof(szBuff), 0);
	if (ret == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

    szBuff[1023] = '\0';
	strData = szBuff;
	return 0;
}