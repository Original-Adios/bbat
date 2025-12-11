#include "StdAfx.h"
#include "ReadMcuVer.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CReadMcuVer)

///
CReadMcuVer::CReadMcuVer(void)
{
}

CReadMcuVer::~CReadMcuVer(void)
{
}

BOOL CReadMcuVer::LoadXMLConfig(void)
{
    m_McuVer.bLoad   = GetConfigValue(L"Option:Load", 1);
    m_McuVer.bVerify = GetConfigValue(L"Option:Verify", 0);
    LPCWSTR lpMcu = GetConfigValue(L"Option:ExpectedVersion", L"");
    if (NULL != lpMcu && wcslen(lpMcu) > 0)
    {
        if (wcslen(lpMcu) < 5)
        {
            SendCommonCallback(L"when Check Mcu Version ,Option:ExpectedVersion length should be longer than 5!");
            return FALSE;
        }
        m_McuVer.strExpVer = _W2CA(lpMcu);
    }

    return TRUE;
}

SPRESULT CReadMcuVer::__PollAction(void)
{ 
    CHAR szMcuVer[2500] = {0};
    if (m_McuVer.bLoad)
    {
        CHKRESULT(LoadMcuVer(szMcuVer));
        NOTIFY(DUT_INFO_MCU, szMcuVer);
        
        if (m_McuVer.bVerify && m_McuVer.strExpVer.length() > 0)
        {
            if (NULL == strstr(szMcuVer, m_McuVer.strExpVer.c_str()))
            {
                NOTIFY("MCU Version VERIFY", LEVEL_ITEM|LEVEL_INFO, 1, 0, 1, NULL, -1, NULL,  m_McuVer.strExpVer.c_str());
                return SP_E_SPAT_INVALID_DATA;
            }
            else
            {
                NOTIFY("MCU Version VERIFY", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  m_McuVer.strExpVer.c_str());
            }
        }
        else
        {
            NOTIFY("MCU Version", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, szMcuVer);
        }
        SetShareMemory(ShareMemory_MCU_VERSION, (const void*)&szMcuVer[0], sizeof(char)*strlen(szMcuVer));
    }

    return SP_OK;
}

SPRESULT CReadMcuVer::LoadMcuVer(char* szMcuVer)
{
    std::string strBuf = "";
    BOOL bOK = FALSE;
    for (INT i = 0; i < 3; i++)
    {
        CHAR   szBuf[4096] = { 0 };
        uint32 u32recvSize = 0;
        SPRESULT res = SP_SendATCommand(m_hDUT, "AT+MCUVERSION?", TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_10S);
        if (SP_OK != res)
        {
            NOTIFY("Load MCU Version", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, NULL);
            return res;
        }

        strBuf = szBuf;
        replace_all(strBuf, "\r", "");
        replace_all(strBuf, "\n", "");
        if (NULL != strstr(strBuf.c_str(), "+CME ERROR") || NULL == strstr(strBuf.c_str(), "+MCUVERSION:"))
        {
            // 重发一次
            Sleep(TIMEOUT_2S);
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
        NOTIFY("Load MCU Version", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid response: %s", strBuf.c_str());
        return SP_E_SPAT_INVALID_RESPONSE;
    }

    replace_all(strBuf, "+MCUVERSION:", "");
    replace_all(strBuf, "OK", "");
    trimA(strBuf);
    memcpy(szMcuVer, strBuf.c_str(), strBuf.length());

    return SP_OK;
}