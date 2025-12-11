#include "StdAfx.h"
#include "CheckSimCard.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckSimCard)

///
CCheckSimCard::CCheckSimCard(void)
{
	m_bCheckSimCard1 = TRUE;
	m_bCheckSimCard2 = FALSE;
	m_strSimCard1CheckType = L"Existence";
	m_strSimCard2CheckType = L"Existence";
}

CCheckSimCard::~CCheckSimCard(void)
{

}

BOOL CCheckSimCard::LoadXMLConfig(void)
{
	m_bCheckSimCard1 = (BOOL)GetConfigValue(L"Option:SimCard1", 1);
	m_bCheckSimCard2 = (BOOL)GetConfigValue(L"Option:SimCard2", 0);

	//bug 1076200
	m_strSimCard1CheckType = GetConfigValue(L"Option:SimCard1CheckType", L"Existence");
	m_strSimCard2CheckType = GetConfigValue(L"Option:SimCard2CheckType", L"Existence");

     return TRUE;
}

SPRESULT CCheckSimCard::__PollAction()
{
    if (!m_bCheckSimCard1 && !m_bCheckSimCard2)
    {
        LogRawStrA(SPLOGLV_WARN, "No need to check SIM1 and SIM2");
        return SP_OK;
    }

    std::string strRsp = "";
    LPCSTR AT_CPIN   = "AT+CPIN?";
    LPCSTR SIM_READY = "+CPIN: READY";

	if (m_bCheckSimCard1)
    {
        LPCSTR AT_CARD = "AT+SPACTCARD=0";
        CHKRESULT_WITH_NOTIFY(SendAT(AT_CARD, strRsp, 3, TIMEOUT_1S, TIMEOUT_3S), AT_CARD);
		if(m_strSimCard1CheckType == L"Existence")
		{
			CHKRESULT_WITH_NOTIFY(SendAT(AT_CPIN, strRsp, 5, TIMEOUT_1S, TIMEOUT_3S), AT_CPIN);
			if (NULL != strstr(strRsp.c_str(), SIM_READY))
			{
				NOTIFY("CheckSimCard1", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
			}
			else
			{
				NOTIFY("CheckSimCard1", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
				return SP_E_SPAT_CHECK_SIM_CARD;
			}
		}
		else
		{
			CHKRESULT_WITH_NOTIFY(CheckSimNoInserted(strRsp, 3, TIMEOUT_1S, TIMEOUT_3S), "CheckSimCard1");
			NOTIFY("CheckSimCard1", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Sim1 No Inserted");
		}
    }

    if (m_bCheckSimCard2)
    {
        LPCSTR AT_CARD = "AT+SPACTCARD=1";
        CHKRESULT_WITH_NOTIFY(SendAT(AT_CARD, strRsp, 3, TIMEOUT_1S, TIMEOUT_3S), AT_CARD);
		if(m_strSimCard2CheckType == L"Existence")
		{
			CHKRESULT_WITH_NOTIFY(SendAT(AT_CPIN, strRsp, 5, TIMEOUT_1S, TIMEOUT_3S), AT_CPIN);
			if (NULL != strstr(strRsp.c_str(), SIM_READY))
			{
				NOTIFY("CheckSimCard2", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
			}
			else
			{
				NOTIFY("CheckSimCard2", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
				return SP_E_SPAT_CHECK_SIM_CARD;
			}
		}
		else
		{
			CHKRESULT_WITH_NOTIFY(CheckSimNoInserted(strRsp, 3, TIMEOUT_1S, TIMEOUT_3S), "CheckSimCard2");
			NOTIFY("CheckSimCard2", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Sim2 No Inserted");
		}
    }
    return SP_OK;
}
SPRESULT CCheckSimCard::CheckSimNoInserted(
    std::string& strRsp, 
    uint32 u32RetryCount /* = 3 */, 
    uint32 u32Interval /* = 200 */,
    uint32 u32TimeOut /* = TIMEOUT_3S */
    )
{
    UNREFERENCED_PARAMETER(strRsp);
    SPRESULT res = SP_OK;
    CHAR   recvBuf[4096] = {0};
    uint32 recvSize = 0;
    for (uint32 i=0; i<u32RetryCount; i++)
    {
        res = SP_SendATCommand(m_hDUT, "AT+CPIN?", TRUE, recvBuf, sizeof(recvBuf), &recvSize, u32TimeOut);
        if (SP_OK == res)
        {
			if(NULL != strstr(recvBuf, "+CME ERROR: 10"))
			{
				return SP_OK;
			}
			if(NULL != strstr(recvBuf, "+CPIN: READY"))
			{
			    return SP_E_PHONE_AT_EXECUTE_FAIL;
			}
        }
        Sleep(u32Interval);
    }
    return SP_E_PHONE_AT_EXECUTE_FAIL;
}

