#include "StdAfx.h"
#include "CheckWcnUid.h"
#include "SimpleAop.h"

IMPLEMENT_RUNTIME_CLASS(CCheckWcnUid)
//////////////////////////////////////////////////////////////////////////
CCheckWcnUid::CCheckWcnUid(void)
	: m_strWcnUid("")
{
}

CCheckWcnUid::~CCheckWcnUid(void)
{
}

BOOL CCheckWcnUid::LoadXMLConfig(void)
{
	m_strWcnUid = _W2CA(GetConfigValue(L"Option:UID", L""));

	return TRUE;
}

SPRESULT CCheckWcnUid::LeaveBTMode(void)
{
	CHAR   szBuf[4096] = { 0 };
	uint32 u32recvSize = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+SPBTTEST=TESTMODE,0", TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_3S);
	if (SP_OK != res)
	{
		NOTIFY("TestMode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Leave TestMode fail");
		return res;
	}

	return SP_OK;
}

/*AT + SPBTTEST = WCNUID ?
-- >h.AT + SPBTTEST = WCNUID ? ..~
<<0M9H588_12_31_54
<<OK
*/

SPRESULT CCheckWcnUid::__PollAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	std::string strBuf = "";
	BOOL bOK = FALSE;
	CHAR   szBuf[4096] = { 0 };
	uint32 u32recvSize = 0;

	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+SPBTTEST=TESTMODE,2", TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_3S);
	if (SP_OK != res)
	{
		NOTIFY("TESTMODE", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Set TESTMODE fail");
		return res;
	}


	for (INT i = 0; i < 5; i++)
	{

		SPRESULT res = SP_SendATCommand(m_hDUT, "AT+SPBTTEST=WCNUID?", TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_3S);
		if (SP_OK != res)
		{
			NOTIFY("WCN EFUSE ID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load WCN EFUSE ID fail");
			CHKRESULT(LeaveBTMode());
			return res;
		}

		strBuf = szBuf;
		replace_all(strBuf, "\r", "");
		replace_all(strBuf, "\n", "");

		if (NULL != strstr(strBuf.c_str(), "+CME ERROR") || NULL == strstr(strBuf.c_str(), "OK"))
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
		NOTIFY("WCN EFUSE ID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid response: %s", strBuf.c_str());
	}
	else
	{
		replace_all(strBuf, "OK", "");
		NOTIFY("WCN EFUSE ID", LEVEL_ITEM | LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, strBuf.c_str());
	}

	CHKRESULT(LeaveBTMode());

	return TRUE == bOK ? SP_OK : SP_E_SPAT_BBID_LOAD_ERROR;
}