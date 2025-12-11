#include "StdAfx.h"
#include "WlanMeasCW.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeasCW::CWlanMeasCW(CImpBase *pImpBase) : CWlanMeasBase(pImpBase)
{

}

CWlanMeasCW::~CWlanMeasCW(void)
{

}

SPRESULT CWlanMeasCW::MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	DWORD dwMask = pUplinkMeasParam->stTesterParamGroupSub.dwMask;
    /// 
    if (!IS_BIT_SET(dwMask, WIFI_CW))
    {
        return SP_E_INVALID_PARAMETER;
    }
	ConfigTesterParam(pUplinkMeasParam);
    // [1]: Setup DUT Channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(pUplinkMeasParam->stChan.nCenChan), "DUT_SetCH()!");

	if (pUplinkMeasParam->stTesterParamGroupSub.bSetPwrLvl)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPwrLvl(pUplinkMeasParam->stTesterParamGroupSub.dPwrLvl), "DUT_SetPwrLvl()!");
	}

    // [4] DUT start to transmit WIFI signal
	CHKRESULT_WITH_NOTIFY_WCN_ITEM_TX_OFF(m_pWlanApi->DUT_TxOn_CW(true), "DUT_TxOn_CW()!");
    Sleep(m_nTxOnSleep);

	//Auto ADJ Instrument Enp to match overload
	SPRESULT res = SP_OK;
	int nLoop = 0;
	double dRefLvl = m_stTester.dRefLvl;
	do
	{
		if (m_pImpBase->_IsUserStop())
		{
			m_pWlanApi->DUT_TxOn(false);
			return SP_E_USER_ABORT;
		}
		m_pImpBase->LogRawStrA(SPLOGLV_INFO, "InitTest(TX)");
		res = m_pRfTesterWlan->InitTest(dwMask, (LPVOID)&m_stTester);
		if (SP_OK != res)
		{
			m_pImpBase->LogRawStrA(SPLOGLV_INFO, "InitTest(TX) Fail");
			m_pWlanApi->DUT_TxOn(false);
			return res;
		}
		m_pImpBase->LogRawStrA(SPLOGLV_INFO, "FetchResult");
		res = m_pRfTesterWlan->FetchResult(dwMask, pTestResult);
		if (SP_OK != res)
		{
			m_pImpBase->LogFmtStrA(SPLOGLV_INFO, "%s", "FetchResult Fail, Then Adj Instrument ENP");
			if (IS_BIT_SET(dwMask, WIFI_CW))
			{
				if (3 == pTestResult->CWPwr.nIndicator)
				{
					m_stTester.dRefLvl = dRefLvl + (nLoop + 1) * 2;

				}
				if (4 == pTestResult->CWPwr.nIndicator)
				{
					m_stTester.dRefLvl = dRefLvl - (nLoop + 1) * 2;
				}

			}
		}

	} while (++nLoop < 3 && SP_OK != res);

	pUplinkMeasParam->stTesterParamGroupSub.dRefLvl = m_stTester.dRefLvl;
    // TX off while failure
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_TxOn(false), "DUT_TxOn(false)!");
	return SP_OK;
}


SPRESULT CWlanMeasCW::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	UNREFERENCED_PARAMETER(pDownlinkMeasParam);
	UNREFERENCED_PARAMETER(pRxReult);

	return SP_OK;
}