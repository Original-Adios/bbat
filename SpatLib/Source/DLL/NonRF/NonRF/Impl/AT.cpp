#include "StdAfx.h"
#include "AT.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CAT)

///
CAT::CAT(void)
: m_strAT("")
, m_strRsp("")
, m_u32TimeOut(TIMEOUT_3S)
{
}

CAT::~CAT(void)
{
}

BOOL CAT::LoadXMLConfig(void)
{
    m_strAT  = _W2CA(GetConfigValue(L"Option:AT", L""));
    trimA(m_strAT);
    if (0 == m_strAT.length())
    {
        return FALSE;
    }
    m_strRsp = _W2CA(GetConfigValue(L"Option:Response", L""));
    trimA(m_strRsp);
    m_u32TimeOut = GetConfigValue(L"Param:TimeOut", TIMEOUT_3S);

    return TRUE;
}

SPRESULT CAT::__PollAction(void)
{ 
    CHAR   recvBuf[4096] = {0};
    uint32 recvSize = 0;
    SPRESULT res = SP_SendATCommand(m_hDUT, m_strAT.c_str(), TRUE, recvBuf, sizeof(recvBuf), &recvSize, m_u32TimeOut);
    if (SP_OK != res || NULL != strstr(recvBuf, "+CME ERROR"))
    {
        NOTIFY(m_strAT.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "%s", recvBuf);
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }

    std::string strRsp = recvBuf;
    replace_all(strRsp, "\r", "");
    replace_all(strRsp, "\n", "");
    trimA(strRsp);
    if (m_strRsp.length() > 0)
    {
        if (NULL != strstr(strRsp.c_str(), m_strRsp.c_str()))
        {
            NOTIFY(m_strAT.c_str(), LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
            return SP_OK;
        }
        else
        {
            NOTIFY(m_strAT.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
            return SP_E_PHONE_AT_EXECUTE_FAIL;
        }
    }
    else
    {
        NOTIFY(m_strAT.c_str(), LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
        return SP_OK;
    }
}
