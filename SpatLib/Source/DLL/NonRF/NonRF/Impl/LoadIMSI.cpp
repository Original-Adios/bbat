#include "StdAfx.h"
#include "LoadIMSI.h"
#include "ShareMemoryDefine.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CLoadIMSI)

///
CLoadIMSI::CLoadIMSI(void)
{
    // IMSI只能在BBAT模式下
    SetupDUTRunMode(RM_AUTOTEST_MODE);
}

CLoadIMSI::~CLoadIMSI(void)
{
    
}

SPRESULT CLoadIMSI::__PollAction()
{
    std::string strBuf = "";
    BOOL bOK = FALSE;
    for (INT i=0; i<5; i++)
    {
        CHAR   szBuf[4096] = {0};
        uint32 u32recvSize =  0;
        SPRESULT res = SP_SendATCommand(m_hDUT, "AT+CIMI", TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_3S);
        if (SP_OK != res)
        {
            NOTIFY("IMSI", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load IMSI fail");
            return res;
        }

        // AT+CIMI
        // 460006963106758
        strBuf = szBuf;
        replace_all(strBuf, "\r", "");
        replace_all(strBuf, "\n", "");
        replace_all(strBuf, "OK", "");

        if (NULL != strstr(strBuf.c_str(), "+CME ERROR"))
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
        NOTIFY("IMSI", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid response: %s", strBuf.c_str());
        return SP_E_SPAT_IMSI_LOAD_ERROR;
    }

    NOTIFY("IMSI", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strBuf.c_str());
    NOTIFY(DUT_INFO_IMSI, strBuf.c_str());

    // Save into ShareMemory
    CHAR szIMSI[ShareMemory_IMSI_SIZE] = {0}; // ICCID is 20 digits.
    strncpy_s(szIMSI, strBuf.c_str(), ShareMemory_IMSI_SIZE - 1);
    CHKRESULT(SetShareMemory(ShareMemory_IMSI, (const void* )&szIMSI[0], sizeof(szIMSI)));

    return SP_OK;
}
