#pragma once
#include <WINSOCK2.H>
#include <string>

#pragma comment(lib, "ws2_32.lib")
//////////////////////////////////////////////////////////////////////////
class CSocketCon
{
public:
    CSocketCon(void);
    virtual ~CSocketCon(void);

	int Connect(const std::string strIP="127.0.0.1", UINT dwPort=8300);
	void Disconnect();

	int Send(const std::string strData);
	int Receive(std::string &strData);

private:
	//WSADATA mwsaData;
	SOCKET m_sClient;
	sockaddr_in m_serAddr;
};
