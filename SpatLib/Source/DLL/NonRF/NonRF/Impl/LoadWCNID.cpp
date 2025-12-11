#include "StdAfx.h"
#include "LoadWCNID.h"

IMPLEMENT_RUNTIME_CLASS( CLoadWCNID )
CLoadWCNID::CLoadWCNID( void )
{

}

CLoadWCNID::~CLoadWCNID( void )
{

}

BOOL CLoadWCNID::LoadXMLConfig( void )
{
	std::wstring strCode = GetConfigValue(L"Option:WCNID", L"");
	m_strCode = _W2CA(strCode.c_str());
    return TRUE;
}

SPRESULT CLoadWCNID::__PollAction( void )
{
	std::string strBuf = "";
	BOOL bOK = FALSE;
	for (INT i=0; i<5; i++) 
	{
		CHAR   szBuf[4096] = {0};
		uint32 u32recvSize =  0;
		SPRESULT res = SP_SendATCommand(m_hDUT, "AT+SPBTTEST=CHIP?", TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_3S);
		if (SP_OK != res)
		{
			NOTIFY("WCNID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load WCNID fail");
			return res;
		}

		strBuf = szBuf;
		replace_all(strBuf, "\r", "");
		replace_all(strBuf, "\n", "");

		if (NULL != strstr(strBuf.c_str(), "+CME ERROR") || NULL == strstr(strBuf.c_str(), "SPBTTEST:"))
		{
			Sleep(TIMEOUT_3S);
			continue;
		}
		else
		{
			bOK = TRUE;
			break;
		}
	}

	if (!bOK)
	{
		NOTIFY("WCNID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid response: %s", strBuf.c_str());
		return SP_E_SPAT_BBID_LOAD_ERROR;
	}

	// +SPBTTEST:CHIP=Marlin3Lite_AB_0x2355B001OK\r\n
	size_t nStart = strBuf.find('=');
	size_t nStop  = strBuf.rfind('O');
	if (std::string::npos == nStart || std::string::npos == nStop)
	{
		NOTIFY("WCNID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "No double quotation marks: %s", strBuf.c_str());
		return SP_E_SPAT_BBID_LOAD_ERROR;
	}

	strBuf[nStop] = '\0';
	std::string strWCNID = &strBuf[nStart + 1];

	if (!m_strCode.empty())
	{
		if (strWCNID != m_strCode)
		{
			NOTIFY("WCNID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unmatched WCNID: %s", strWCNID.c_str());
			return SP_E_SPAT_BBID_UNMATCHED;
		}
	}

	NOTIFY("WCNID", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, strWCNID.c_str());

    return SP_OK;
}
