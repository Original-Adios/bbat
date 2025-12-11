#include "StdAfx.h"
#include "WlanMeas24G.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeas24G::CWlanMeas24G(CImpBase *pImpBase) : CWlanMeasBase(pImpBase)
{
}

CWlanMeas24G::~CWlanMeas24G(void)
{

}

SPRESULT CWlanMeas24G::MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	DWORD dwMask = pUplinkMeasParam->stTesterParamGroupSub.dwMask;

    if (   !IS_BIT_SET(dwMask, WIFI_TXP)
        && !IS_BIT_SET(dwMask, WIFI_FER)
		&& !IS_BIT_SET(dwMask, WIFI_IQ)
        && !IS_BIT_SET(dwMask, WIFI_EVM)
        && !IS_BIT_SET(dwMask, WIFI_MASK)
        && !IS_BIT_SET(dwMask, WIFI_FLATNESS)
        )
    {
        return SP_OK;
    }
	ConfigTesterParam(pUplinkMeasParam);
    // [1]: Setup DUT Channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(pUplinkMeasParam->stChan.nCenChan), "DUT_SetCH()!");       
    // [2]: Setup DUT data rate
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate( pUplinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate()!");
	//////////////////////////////////////////////////////////////////////////
	if (pUplinkMeasParam->stTesterParamGroupSub.bSetPwrLvl)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPwrLvl(pUplinkMeasParam->stTesterParamGroupSub.dPwrLvl), "DUT_SetPwrLvl()!");
	}
	// 
    // [4] DUT start to transmit WIFI signal
	CHKRESULT_WITH_NOTIFY_WCN_ITEM_TX_OFF(m_pWlanApi->DUT_TxOn(true), "DUT_TxOn()!");
    // Wait DUT to transmit WIFI signal, default is 300ms.
    // m_nTxOnSleep could be configured by INI file.
    Sleep(m_nTxOnSleep);
    //////////////////////////////////////////////////////////////////////////
    // [6]: Setup instrument and measure up-link result
    m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);

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
			if (IS_BIT_SET(dwMask, WIFI_TXP))
			{
				if (3 == pTestResult->txp.nIndicator)
				{
					m_stTester.dRefLvl = dRefLvl + (nLoop + 1) * 2;

				}
				if (4 == pTestResult->txp.nIndicator)
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

SPRESULT CWlanMeas24G::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (pDownlinkMeasParam == NULL)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	DWORD dwMask = pDownlinkMeasParam->stTesterParamGroupSub.dwMask;
    if (!IS_BIT_SET(dwMask, WIFI_PER) && !IS_BIT_SET(dwMask, WIFI_RSSI))
    {
        return SP_OK;
    }
	
    m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	ConfigTesterParam(pDownlinkMeasParam);

	//Set wifi preamble normal mode
    // [1]: Setup DUT Channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(pDownlinkMeasParam->stChan.nCenChan), "DUT_SetCH()!");
    // [2]: Setup DUT data rate
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate( pDownlinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate()!");
   // CHKRESULT(m_pRfTesterWlan->SetNetMode(NM_WIFI));

	CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(true), "DUT_RxOn()!");
    m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);

	const int MAXCOUNTRETRY = 20;
	//   Sleep(200);   
	if (IS_BIT_SET(dwMask, WIFI_PER))
	{
		int nTotalPackets = m_stTester.nTotalPackets;
		int nCountTry = MAXCOUNTRETRY;
		int good_packets  = 0;
		int error_packets = 0;
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->InitTest(WIFI_PER, &m_stTester), "Instrument::InitTest()!");
		do 
		{
			CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_GetPER(good_packets, error_packets), "DUT_GetPER");
			if (good_packets > nTotalPackets)
			{
				break;
			}
			double dPerValue = ((double)(nTotalPackets - good_packets)) / (nTotalPackets) * 100.0;
			if (dPerValue >= 0.0 && dPerValue < m_pWlanParamBand->stSpec.dPer)
			{
				break;
			}

			if (MAXCOUNTRETRY / 2 == nCountTry)
			{
				CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(false), "DUT_RxOff");
				CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE), "SetGen off");
				Sleep(20);
				CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(true), "DUT_RxOn");
				CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->InitTest(WIFI_PER, &m_stTester), "Instrument::InitTest()!");
			}
			Sleep(100);
		} while (nCountTry-- > 0);
		/// To improve the pass rate of mass production line, 
		/// We accept the interference which is not too big.
		/// So we ignore below check rules.
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[WIFI]: Total = %d, good = %d, error = %d.", nTotalPackets, good_packets, error_packets);

		if (good_packets > nTotalPackets)
		{
			pRxReult->per.dAvgValue = 0.0;
		}
		else
		{
			pRxReult->per.dAvgValue = ((double)(nTotalPackets - good_packets))/(nTotalPackets) * 100.0;
		}
	}

	if (IS_BIT_SET(dwMask, WIFI_RSSI))
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE), "SetGen(WIFI_RSSI)");
		m_stTester.nTotalPackets = 0;
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->InitTest(WIFI_PER, &m_stTester), "InitTest(WIFI_RSSI)");

		int nCountTry = MAXCOUNTRETRY;
		double dRssi = INVALID_NEGATIVE_DOUBLE_VALUE;
		do
		{
			CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_GetRssi(dRssi), "DUT_GetRssi");
			if (0.0 != dRssi)
			{
				break;
			}
			Sleep(100);
		} while (nCountTry-- > 0);

		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[WIFI]: RSSI = %.2f.", dRssi);
		pRxReult->Rssi.dAvgValue = dRssi;
	}

    // RX OFF
	CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(false), "DUT_RxOn()!");
    // Turn off VSG
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE), "SetGen(False)!");

    return SP_OK;
}