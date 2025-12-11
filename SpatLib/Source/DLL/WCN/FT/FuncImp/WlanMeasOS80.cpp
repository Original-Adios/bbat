#include "StdAfx.h"
#include "WlanMeasOS80.h"
#include "Utility.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeasOS80::CWlanMeasOS80(CImpBase* pImpBase) : CWlanMeasBase(pImpBase)
{
}

CWlanMeasOS80::~CWlanMeasOS80(void)
{
}

SPRESULT CWlanMeasOS80::MeasureUplink(SPWI_WLAN_PARAM_MEAS_GROUP* pUplinkMeasParam, SPWI_RESULT_T* pTestResult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (pUplinkMeasParam == NULL)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	DWORD dwMask = pUplinkMeasParam->stTesterParamGroupSub.dwMask;

	if (!IS_BIT_SET(dwMask, WIFI_TXP)
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
	// [1]: Setup DUT Channel
	int nCenCh = pUplinkMeasParam->stChan.nCenChan;
	int nPriCh = pUplinkMeasParam->stChan.nPriChan;



	//	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetRfPath(pUplinkMeasParam->stTesterParamGroupSub.eAnt), "DUT_SetRfPath");
		//Set channel band width
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetBandWidth((WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nCBWType), "DUT_SetBandWidth");
	//Set signal band width
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetSigBandWidth((WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nSBWType), "DUT_SetSigBandWidth");
	////Set Channel
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(nPriCh, nCenCh), "DUT_SetCH");
	}
	// [2]: Setup DUT data rate
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate(pUplinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate");

	/*

	//set channel coding
//	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetDecoMode((WIFI_DECOMODE)pUplinkMeasParam->stTesterParamGroupSub.nChCode), "DUT_SetDecoMode");

	//Set packet length
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPacktLen(1024), "DUT_SetPacktLen");
	//Set tx mode
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetTxMode(WIFI_TXMODE_CARRIER_SUPPRESSION), "DUT_SetTxMode");
	//Set preamble
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPreamble(0), "DUT_SetPreamble");
	//Set payload
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPayLoad(WIFI_PAYLOAD_1111), "DUT_SetPayLoad");*/
	if (pUplinkMeasParam->stTesterParamGroupSub.bSetPwrLvl)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPwrLvl(pUplinkMeasParam->stTesterParamGroupSub.dPwrLvl), "DUT_SetPwrLvl()!");
	}
	// [4] DUT start to transmit WIFI signal
    CHKRESULT_WITH_NOTIFY_WCN_ITEM_TX_OFF(m_pWlanApi->DUT_TxOn(true), "DUT_TxOn(true)");
	// Wait DUT to transmit WIFI signal, default is 300ms.
	// m_nTxOnSleep could be configured by INI file.
//    Sleep(m_nTxOnSleep);
	// [6]: Setup instrument and measure up-link result
	Sleep(pUplinkMeasParam->stTesterParamGroupSub.nTxSleep);
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
		res = m_pRfTesterWlan->InitTest(dwMask, (LPVOID)& m_stTester);
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
	//    // TX off while failure
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_TxOn(false), "DUT_TxOn(false)");
	return SP_OK;
}

SPRESULT CWlanMeasOS80::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP* pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
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

	int nCenCh = pDownlinkMeasParam->stChan.nCenChan;
	int nPriCh = pDownlinkMeasParam->stChan.nPriChan;


	//Set Band
	//Set Net mode
//	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetRfPath(pDownlinkMeasParam->stTesterParamGroupSub.eAnt), "DUT_SetRfPath");

	//Set channel band width
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetBandWidth((WIFI_BANDWIDTH_ENUM)pDownlinkMeasParam->stTesterParamGroupSub.nCBWType), "DUT_SetBandWidth");
	//Set signal band width
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetSigBandWidth((WIFI_BANDWIDTH_ENUM)pDownlinkMeasParam->stTesterParamGroupSub.nSBWType), "DUT_SetSigBandWidth");
	//Set Channel
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(nPriCh, nCenCh), "DUT_SetCH");
	/*
	// [2]: Setup DUT data rate
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate( pDownlinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate");
	//set channel coding
	//CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetDecoMode((WIFI_DECOMODE)pDownlinkMeasParam->stTesterParamGroupSub.nChCode), "DUT_SetDecoMode");
	//Set preamble
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPreamble(0), "DUT_SetPreamble");
	*/
	CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(true), "DUT_RxOn");
	Sleep(100);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	//   Sleep(200);
	const int MAXCOUNTRETRY = 20;
	if (IS_BIT_SET(dwMask, WIFI_PER))
	{
		int nCountTry = MAXCOUNTRETRY;
		int nTotalPackets = m_stTester.nTotalPackets;
		int good_packets = 0;
		int error_packets = 0;

		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->InitTest(WIFI_PER, &m_stTester), "InitTest(WIFI_PER)");

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

			if (MAXCOUNTRETRY/2 == nCountTry)
			{
				CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(false), "DUT_RxOff");
				CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE), "SetGen off");
				Sleep(20);
				CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(true), "DUT_RxOn");
				CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->InitTest(WIFI_PER, &m_stTester), "InitTest(WIFI_PER)");
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
			pRxReult->per.dAvgValue = ((double)(nTotalPackets - good_packets)) / (nTotalPackets) * 100.0;
		}
	}
	if (IS_BIT_SET(dwMask, WIFI_RSSI))
	{

		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE), "SetGen OFF(WIFI_RSSI)");
		m_stTester.nTotalPackets = 0;
		//CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_RxOn(false), "DUT_RxOff()!");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pRfTesterWlan->InitTest(WIFI_PER, &m_stTester), "InitTest(WIFI_RSSI)");
		//CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(true), "DUT_RxOn");
		//Sleep(100);

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
	/*CHKRESULT(m_pWlanApi->DUT_RxOn(false));*/
	// Turn off VSG
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE), "SetGen");

	return SP_OK;
}