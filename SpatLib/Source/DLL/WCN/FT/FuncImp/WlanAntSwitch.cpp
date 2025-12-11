#include "StdAfx.h"
#include "WlanApiAT.h"
#include "WlanAntSwitch.h"
#include "SimpleAop.h"
CWlanAntSwitch::CWlanAntSwitch(CImpBase *pImpBase)
: m_pWlanApi(NULL)
, m_pImpBase(pImpBase)
{
    m_pWlanApi = new CWlanApiAT(pImpBase->m_hDUT);
}

CWlanAntSwitch::~CWlanAntSwitch(void)
{
    if (NULL != m_pWlanApi)
    {
        delete m_pWlanApi;
        m_pWlanApi = NULL;
    }
}

SPRESULT CWlanAntSwitch::Wlan_AntSwitch(ANTENNA_ENUM eAnt)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    SPRESULT Result = SP_E_WCN_BASE_ERROR;
//	ANTENNA_ENUM eCurrAnt;

//	CHKRESULT(m_pWlanApi->DUT_GetRfPath(eCurrAnt));

	

    //if (eCurrAnt == eAnt)
    //{
    //    return SP_OK;
    //}
    //else
    {
        unsigned int          nCount = 0;
        const unsigned int MAX_RETRY = 3;

        do 
        {
			Result = m_pWlanApi->DUT_SetRfPath(eAnt);
			if (SP_OK == Result)
			{
				break;
			}

        } while(++nCount < MAX_RETRY);
    }
	m_pImpBase->_UiSendMsg("Set Ant",LEVEL_ITEM, 0, SP_OK == Result? 0:1, 0, CwcnUtility::WLAN_ANT_NAME[eAnt]);/*"Set Antenna", LEVEL_ITEM, 0	, Result, 0, eAnt, 0, 0*/
    return Result;
}

SPRESULT CWlanAntSwitch::BDWlan_AntSwitch(ANTENNA_ENUM eAnt)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	SPRESULT Result = SP_E_WCN_BASE_ERROR;
	{
		unsigned int          nCount = 0;
		const unsigned int MAX_RETRY = 3;

		do
		{
			Result = m_pWlanApi->BDDUT_SetTxChain(eAnt);
			if (SP_OK != Result)
			{
				continue;
			}
			Result = m_pWlanApi->BDDUT_SetRxChain(eAnt);
			if (SP_OK == Result)
			{
				break;
			}

		} while (++nCount < MAX_RETRY);
	}
	m_pImpBase->_UiSendMsg("Set BDDUT_SetTxChain", LEVEL_ITEM, 0, SP_OK == Result ? 0 : 1, 0, CwcnUtility::WLAN_ANT_NAME[eAnt]);/*"Set Antenna", LEVEL_ITEM, 0	, Result, 0, eAnt, 0, 0*/
	return Result;
}
