#include "StdAfx.h"
#include "ReadSwVer.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CReadSwVer)

///
CReadSwVer::CReadSwVer(void)
{
}

CReadSwVer::~CReadSwVer(void)
{
}

BOOL CReadSwVer::LoadXMLConfig(void)
{
    m_CP.bLoad   = GetConfigValue(L"Option:CP:Load", 1);
    m_CP.bVerify = GetConfigValue(L"Option:CP:Verify", 0);
    LPCWSTR lpCP = GetConfigValue(L"Option:CP:ExpectedVersion", L"");   
    if (NULL != lpCP && wcslen(lpCP) > 0)
    {
        if (wcslen(lpCP) < 5)
        {
            SendCommonCallback(L"when Check Software Version ,Option:CP:ExpectedVersion length should be longer than 5!");
            return FALSE;
        }
        m_CP.strExpVer = _W2CA(lpCP);
    }

    m_AP.bLoad   = GetConfigValue(L"Option:AP:Load", 0);
    m_AP.bVerify = GetConfigValue(L"Option:AP:Verify", 0);
    LPCWSTR lpAP = GetConfigValue(L"Option:AP:ExpectedVersion", L"");   
    if (NULL != lpAP && wcslen(lpAP) > 0)
    {
        if (wcslen(lpAP) < 5)
        {
            SendCommonCallback(L"when Check Software Version ,Option:AP:ExpectedVersion length should be longer than 5!");
            return FALSE;
        }
        m_AP.strExpVer = _W2CA(lpAP);
    }

    return TRUE;
}

SPRESULT CReadSwVer::__PollAction(void)
{ 
    CHAR szSwVer[2500] = {0};
    if (m_CP.bLoad)
    {
        CHKRESULT_WITH_NOTIFY(SP_GetSWVer(m_hDUT, CP, szSwVer, sizeof(szSwVer)), "SP_GetSWVer(CP)");
        NOTIFY(DUT_INFO_CP, szSwVer);
        
        if (m_CP.bVerify && m_CP.strExpVer.length() > 0)
        {
            if (NULL == strstr(szSwVer, m_CP.strExpVer.c_str()))
            {
                NOTIFY("SW[CP] VERIFY", LEVEL_ITEM|LEVEL_INFO, 1, 0, 1, NULL, -1, NULL,  m_CP.strExpVer.c_str());
                return SP_E_SPAT_INVALID_DATA;
            }
            else
            {
                NOTIFY("SW[CP] VERIFY", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  m_CP.strExpVer.c_str());
            }
        }
        else
        {
            NOTIFY("SW[CP]", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, szSwVer);
        }
        SetShareMemory(ShareMemory_SW_CP_VERSION, (const void*)&szSwVer[0], sizeof(char)*strlen(szSwVer));
    }

    if (m_AP.bLoad)
    {
        ZeroMemory(szSwVer, sizeof(szSwVer));
        CHKRESULT_WITH_NOTIFY(SP_GetSWVer(m_hDUT, AP, szSwVer, sizeof(szSwVer)), "SP_GetSWVer(AP)");
        NOTIFY(DUT_INFO_AP, szSwVer);
        
        if (m_AP.bVerify && m_AP.strExpVer.length() > 0)
        {
            if (NULL == strstr(szSwVer, m_AP.strExpVer.c_str()))
            {
                NOTIFY("SW[AP] VERIFY", LEVEL_ITEM|LEVEL_INFO, 1, 0, 1, NULL, -1, NULL,  m_AP.strExpVer.c_str());
                return SP_E_SPAT_INVALID_DATA;
            }
            else
            {
                NOTIFY("SW[AP] VERIFY", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  m_AP.strExpVer.c_str());
            }
        }
        else
        {
            NOTIFY("SW[AP]", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, szSwVer);
        }
        SetShareMemory(ShareMemory_SW_AP_VERSION, (const void*)&szSwVer[0], sizeof(char) * strlen(szSwVer));
    }

    return SP_OK;
}
