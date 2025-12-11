#include "StdAfx.h"
#include "BTApiAT.h"
#include "BTRFPathSwitch.h"

CBTRFPathSwitch::CBTRFPathSwitch(ICBTApi *pBTApi)
: m_pBTApi(NULL)
{
    m_pBTApi = pBTApi;
}

CBTRFPathSwitch::~CBTRFPathSwitch(void)
{
    if (NULL != m_pBTApi)
    {
        m_pBTApi = NULL;
    }
}

SPRESULT CBTRFPathSwitch::BT_RFPathSet(BT_RFPATH_ENUM eRfPath)
{
    SPRESULT Result = SP_E_WCN_BASE_ERROR;
    
//	RFPATH_ENUM eCurPath = ANT_BT_MAX;

	//CHKRESULT(m_pBTApi->DUT_GetRfPath(eCurPath));

	//if (eCurPath == eRfPath)
	//{
	//	Result = SP_OK;  
	//}
	//else
    {
        unsigned int          nCount = 0;
        const unsigned int MAX_RETRY = 1;

        do 
        {
            if (SP_OK == m_pBTApi->DUT_SetRfPath(eRfPath))
            {
//                Sleep(TIMEOUT_1S);
                Result = SP_OK;
                break;
            }
            else
            {
                Sleep(200);
            }

        } while(++nCount < MAX_RETRY);
    }

    return Result;
}