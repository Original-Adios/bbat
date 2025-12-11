#include "StdAfx.h"
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{
}

CImpBase::~CImpBase(void)
{
}

SPRESULT CImpBase::SendAT(
    LPCSTR lpszAT, 
    std::string& strRsp, 
    uint32 u32RetryCount /* = 3 */, 
    uint32 u32Interval /* = 200 */,
    uint32 u32TimeOut /* = TIMEOUT_3S */
    )
{
    SPRESULT res = SP_OK;
    CHAR   recvBuf[4096] = {0};
    uint32 recvSize = 0;
    for (uint32 i=0; i<u32RetryCount; i++)
    {
        res = SP_SendATCommand(m_hDUT, lpszAT, TRUE, recvBuf, sizeof(recvBuf), &recvSize, u32TimeOut);
        if (SP_OK == res)
        {
            if (NULL != strstr(recvBuf, "+CME ERROR"))
            {
                res = SP_E_PHONE_AT_EXECUTE_FAIL;
            }
            else
            {
                strRsp = recvBuf;
                replace_all(strRsp, "\r", "");
                replace_all(strRsp, "\n", "");
                replace_all(strRsp, "OK", "");
                trimA(strRsp);

                return SP_OK;
            }
        }
        
        Sleep(u32Interval);
    }

    return res;
}