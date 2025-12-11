#include "StdAfx.h"
#include "BTApiAT.h"
#include "BTEnterMode.h"

CBTEnterMode::CBTEnterMode(ICBTApi *pBTApi, BT_TYPE eType)
: m_pBTApi(NULL)
, m_eType(eType)
{
    m_pBTApi = pBTApi;
    ZeroMemory((void *)m_szRecvBuf, sizeof(m_szRecvBuf));
    ZeroMemory((void *)m_szCmd, sizeof(m_szCmd));
}

CBTEnterMode::~CBTEnterMode(void)
{
    if (NULL != m_pBTApi)
    {
        m_pBTApi = NULL;
    }
}

SPRESULT CBTEnterMode::BT_ModeEnable(BT_TESTMODE eMode, BOOL bEnterOrLeave)
{
    SPRESULT Result = SP_E_WCN_BASE_ERROR;
    
    int nCurMode = -1;

    CHKRESULT(m_pBTApi->SetType(m_eType));

    CHKRESULT(m_pBTApi->DUT_GetTestMode(nCurMode));

    if (nCurMode == eMode)
    {
        Result = SP_OK;  
    }
    else
    {
        unsigned long      ulTimeOut = bEnterOrLeave ? 10000 : 30000;
        unsigned int          nCount = 0;
        const unsigned int MAX_RETRY = 3;

        do 
        {
            if (SP_OK == m_pBTApi->DUT_SetTestMode(eMode, ulTimeOut))
            {
//                Sleep(TIMEOUT_1S);
                Result = SP_OK;
                break;
            }
            else
            {
                Sleep(300);
            }

        } while(++nCount < MAX_RETRY);
    }

    return Result;
}

SPRESULT CBTEnterMode::BT_SetBtType(BT_TYPE eType)
{
    m_eType = eType; 
    return SP_OK;
}