#include "StdAfx.h"
#include "LoadICCID.h"
#include "ShareMemoryDefine.h"
#include "MesHelper.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CLoadICCID)

///
CLoadICCID::CLoadICCID(void)
{
    // ICCID只能在BBAT模式下
    SetupDUTRunMode(RM_AUTOTEST_MODE);
}

CLoadICCID::~CLoadICCID(void)
{
    
}

BOOL CLoadICCID::LoadXMLConfig(void)
{
    std::wstring strCode = GetConfigValue(L"Option:ICCID", L"");
    m_strCode = _W2CA(strCode.c_str());

    return TRUE;
}

SPRESULT CLoadICCID::__PollAction()
{
    std::string strBuf = "";
    BOOL bOK = FALSE;
    for (INT i=0; i<5; i++) 
    {
        CHAR   szBuf[4096] = {0};
        uint32 u32recvSize =  0;
        SPRESULT res = SP_SendATCommand(m_hDUT, "AT+CCID", TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_3S);
        if (SP_OK != res)
        {
            NOTIFY("ICCID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load ICCID fail");
            return res;
        }

        strBuf = szBuf;
        replace_all(strBuf, "\r", "");
        replace_all(strBuf, "\n", "");

        if (NULL != strstr(strBuf.c_str(), "+CME ERROR") || NULL == strstr(strBuf.c_str(), "+CCID:"))
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
        NOTIFY("ICCID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid response: %s", strBuf.c_str());
        return SP_E_SPAT_ICCID_LOAD_ERROR;
    }

    // +CCID:"898600650915F8009933"\r\nOK\r\n
    size_t nStart = strBuf.find('\"');
    size_t nStop  = strBuf.rfind('\"');
    if (std::string::npos == nStart || std::string::npos == nStop)
    {
        NOTIFY("ICCID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "No double quotation marks: %s", strBuf.c_str());
        return SP_E_SPAT_ICCID_LOAD_ERROR;
    }

    strBuf[nStop] = '\0';
    std::string strICCID = &strBuf[nStart + 1];

    if (!m_strCode.empty())
    {
        if (NULL == strstr(strICCID.c_str(), m_strCode.c_str()))
        {
            NOTIFY("ICCID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unmatched ICCID: %s", strICCID.c_str());
            return SP_E_SPAT_ICCID_UNMATCHED;
        }
    }

    NOTIFY("ICCID", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, strICCID.c_str());
    NOTIFY(DUT_INFO_ICCID, strICCID.c_str());

    // Save into ShareMemory
    CHAR szICCID[ShareMemory_ICCID_SIZE] = {0}; // ICCID is 20 digits.
    strncpy_s(szICCID, strICCID.c_str(), ShareMemory_ICCID_SIZE - 1);
    CHKRESULT(SetShareMemory(ShareMemory_ICCID, (const void* )&szICCID[0], sizeof(szICCID)));

    CMesHelper mes(this);
	if (MES_SUCCESS == mes.UnisocMesActive())
	{
		_UNISOC_MES_CMN_CODES mes_codes;
		if (SP_OK == GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes)))
		{
			strncpy_s(mes_codes.sz_cmn_sim_iccid, strICCID.data(), CopySize(mes_codes.sz_cmn_sim_iccid));
			CHKRESULT(SetShareMemory(ShareMemory_MES_Assigned_Codes, (const void*)&mes_codes, sizeof(mes_codes)));
		}
	}

    return SP_OK;
}
