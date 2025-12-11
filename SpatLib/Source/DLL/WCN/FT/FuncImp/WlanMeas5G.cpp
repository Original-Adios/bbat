#include "StdAfx.h"
#include "WlanMeas5G.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeas5G::CWlanMeas5G(CImpBase *pImpBase) : CWlanMeasBase(pImpBase)
{
}

CWlanMeas5G::~CWlanMeas5G(void)
{

}

SPRESULT CWlanMeas5G::MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult)
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

    m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	ConfigTesterParam(pUplinkMeasParam);

	int nCenCh = pUplinkMeasParam->stChan.nCenChan;
    // [1]: Setup DUT Channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(nCenCh), "DUT_SetCH()!");       
    // [2]: Setup DUT data rate
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate( pUplinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate()!");
    //////////////////////////////////////////////////////////////////////////
    // 
	if (pUplinkMeasParam->stTesterParamGroupSub.bSetPwrLvl)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPwrLvl(pUplinkMeasParam->stTesterParamGroupSub.dPwrLvl), "DUT_SetPwrLvl()!");
	}
    // [4] DUT start to transmit WIFI signal
	CHKRESULT_WITH_NOTIFY_WCN_ITEM_TX_OFF(m_pWlanApi->DUT_TxOn(true), "DUT_TxOn()!");
    // Wait DUT to transmit WIFI signal, default is 300ms.
    // m_nTxOnSleep could be configured by INI file.
    Sleep(m_nTxOnSleep);
    //////////////////////////////////////////////////////////////////////////
    // [6]: Setup instrument and measure up-link result
    m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
  //  CHKRESULT(m_pRfTesterWlan->SetNetMode(NM_WIFI));

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

SPRESULT CWlanMeas5G::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    UNREFERENCED_PARAMETER(pDownlinkMeasParam);
    UNREFERENCED_PARAMETER(pRxReult);

    return SP_OK;
//	int nBandWidth = 0;
//	//Set wifi preamble normal mode
//	// [1]: Setup DUT Channel
//	CHKRESULT(m_pWlanApi->DUT_SetCH(pDownlinkMeasParam->pstMeasParamChan->nCh));
//	// [2]: Setup DUT data rate
//	CHKRESULT(m_pWlanApi->DUT_SetModRate((WIFI_PROTOCOL_ENUM)pDownlinkMeasParam->nProto
//                                        , pDownlinkMeasParam->pstMeasParamChan->szRate));
//
//	//CHKRESULT(m_pRfTesterWlan->SetParameter(PT_UPLINK_CHAN, &nCH));
//	//CHKRESULT(m_pRfTesterWlan->SetParameter(PT_WIFI_DATARATE, &dRate));
//
//	CHKRESULT(m_pWlanApi->DUT_SetPreamble(pDownlinkMeasParam->nProto));
//	CHKRESULT(m_pWlanApi->DUT_SetBandWidth(!nBandWidth));
//
//	//CHKRESULT(m_pRfTesterWlan->SetGen(MM_MODULATION, TRUE));
//	//CHKRESULT(m_pRfTesterWlan->SetParameter(PT_OPT_PWR, &dVsgLv));
//
//	CHKRESULT(m_pWlanApi->DUT_RxOn(true));
//	///
////	CHKRESULT(m_pRfTesterWlan->SetParameter(PT_DOWNLINK_TOTAL_PACKETS, &pDownlinkMeasParam->nTotalPackets));
//
//	Sleep(200);
//	int good_packets  = 0;
//	int error_packets = 0;
//	CHKRESULT(m_pWlanApi->DUT_GetPER(good_packets, error_packets));
//
//	/// To improve the pass rate of mass production line, 
//	/// We accept the interference which is not too big.
//	/// So we ignore below check rules.
//
//	//   Tr(NormalTr, "[WIFI]: Total = %d, good = %d, error = %d.", nTotalPackets, good_packets, error_packets);
//	int total_error_packets = pDownlinkMeasParam->nTotalPackets - good_packets;
//	if (total_error_packets <= 0)
//	{
//		pPER->dAvgValue = 0.0;
//	}
//	else
//	{
//		pPER->dAvgValue = ((double)total_error_packets)/pDownlinkMeasParam->nTotalPackets * 100.0;
//	}
//	//  [12/30/2014 xiaoping.jing] Bug 388519 ]]]
//
//	// RX OFF
//	CHKRESULT(m_pWlanApi->DUT_RxOn(false));
//	// Turn off VSG
//	CHKRESULT(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE));
//
//	return SP_OK;
}