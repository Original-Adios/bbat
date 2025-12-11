#include "StdAfx.h"
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{
}

CImpBase::~CImpBase(void)
{
}

SPRESULT CImpBase::SendATCommand(LPCSTR lpszCmd, uint32 u32TimeOut /* = TIMEOUT_3S */)
{
    CHAR   szRevBuf[4096] = {0};
    uint32 u32RevSize = 0;
    SPRESULT res = SP_SendATCommand(m_hDUT, lpszCmd, TRUE, szRevBuf, sizeof(szRevBuf), &u32RevSize, u32TimeOut);
    if (SP_OK != res)
    {
        return res;
    }

    if (NULL != strstr(szRevBuf, "CME ERROR"))
    {
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }

    m_strRevBuf = szRevBuf;
    replace_all(m_strRevBuf, "\r", "");
    replace_all(m_strRevBuf, "\n", "");
    replace_all(m_strRevBuf, "OK", "");

    return SP_OK;
}