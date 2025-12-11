#include "stdafx.h"
#include "CheckTP.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckTP)

//
CCheckTP::CCheckTP()
{
}

CCheckTP::~CCheckTP()
{

}

BOOL CCheckTP::LoadXMLConfig(void)
{
    m_strTP = _W2CA(GetConfigValue(L"Option:TP", L""));
    return TRUE;
}

SPRESULT CCheckTP::__PollAction(void)
{
    LPCSTR TP_CMD = "AT+TPID";
    SPRESULT res = SendATCommand(TP_CMD);
    if (SP_OK != res)
    {
        NOTIFY("TP", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, TP_CMD);
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }
    else
    {
        if (NULL == strstr(m_strRevBuf.c_str(), m_strTP.c_str()))
        {
            NOTIFY("TP", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, m_strRevBuf.c_str());
            return SP_E_CHECK_TP_TYPE_ERROR;
        }
        else
        {
            NOTIFY("TP", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
        }
    }

    return SP_OK;
}
