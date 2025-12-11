#include "StdAfx.h"
#include "LoadEID.h"
#include "ShareMemoryDefine.h"
#include "ActionApp.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CLoadEID)

///
CLoadEID::CLoadEID(void)
{
    // IMSI只能在BBAT模式下
    SetupDUTRunMode(RM_AUTOTEST_MODE);
}

CLoadEID::~CLoadEID(void)
{
    
}

SPRESULT CLoadEID::__PollAction()
{
    std::string strBuf = "";
    BOOL bOK = FALSE;
    for (INT i=0; i<5; i++)
    {
        CHAR   szBuf[4096] = {0};
        uint32 u32recvSize =  0;
        SPRESULT res = SP_SendATCommand(m_hDUT, "AT+CEID", TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_10S);
        if (SP_OK != res)
        {
            NOTIFY("EID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load EID fail");
            return res;
        }

        strBuf = szBuf;
        replace_all(strBuf, "\r", "");
        replace_all(strBuf, "\n", "");
        if (NULL != strstr(strBuf.c_str(), "+CME ERROR") || NULL == strstr(strBuf.c_str(), "+CEID:"))
        {
            // 重发一次
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
        NOTIFY("EID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid response: %s", strBuf.c_str());
        return SP_E_SPAT_EID_LOAD_ERROR;
    }
   
    replace_all(strBuf, "+CEID:", "");
    replace_all(strBuf, "OK", "");
    trimA(strBuf);
    NOTIFY("EID", LEVEL_UI, 1, 1, 1, NULL, -1, NULL, strBuf.c_str());
    NOTIFY("EID", LEVEL_REPORT | LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, ("'" + strBuf).c_str());
    NOTIFY(DUT_INFO_EID, strBuf.c_str());

    // Save into ShareMemory
    CHAR szEID[ShareMemory_EID_SIZE] = {0}; 
    strncpy_s(szEID, strBuf.c_str(), ShareMemory_EID_SIZE - 1);
    CHKRESULT(SetShareMemory(ShareMemory_EID, (const void* )&szEID[0], sizeof(szEID)));

    return SP_OK;
}
