#include "StdAfx.h"
#include "LoadPMICID.h"

IMPLEMENT_RUNTIME_CLASS(CLoadPMICID)
CLoadPMICID::CLoadPMICID( void )
{

}

CLoadPMICID::~CLoadPMICID( void )
{

}

BOOL CLoadPMICID::LoadXMLConfig( void )
{
	std::wstring strCode = GetConfigValue(L"Option:PMICID", L"");
	m_strCode = _W2CA(strCode.c_str());
	return TRUE;
}

SPRESULT CLoadPMICID::__PollAction( void )
{
	std::string strBuf = "";
	BOOL bOK = FALSE;
	for (INT i=0; i<5; i++) 
	{
		CHAR   szBuf[4096] = {0};
		uint32 u32recvSize =  0;
		SPRESULT res = SP_SendATCommand(m_hDUT, "AT+CHIPID=PMIC?", TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_3S);
		if (SP_OK != res)
		{
			NOTIFY("PMICID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load PMICID fail");
			return res;
		}

		strBuf = szBuf;
		replace_all(strBuf, "\r", "");
		replace_all(strBuf, "\n", "");

		if (NULL != strstr(strBuf.c_str(), "+CME ERROR") || NULL == strstr(strBuf.c_str(), "PMIC:"))
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
		NOTIFY("PMICID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid response: %s", strBuf.c_str());
		return SP_E_SPAT_PMICID_LOAD_ERROR;
	}

	// PMIC:2730OK\r\n
	size_t nStart = strBuf.find(':');
	size_t nStop  = strBuf.rfind('O');
	if (std::string::npos == nStart || std::string::npos == nStop)
	{
		NOTIFY("PMICID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "No double quotation marks: %s", strBuf.c_str());
		return SP_E_SPAT_PMICID_LOAD_ERROR;
	}

	strBuf[nStop] = '\0';
	std::string strPMICID = &strBuf[nStart + 1];

	if (!m_strCode.empty())
	{
		if (strPMICID != m_strCode)
		{
			NOTIFY("PMICID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unmatched PMICID: %s", strPMICID.c_str());
			return SP_E_SPAT_PMICID_UNMATCHED;
		}
	}

	NOTIFY("PMICID", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, strPMICID.c_str());

	return SP_OK;
}
