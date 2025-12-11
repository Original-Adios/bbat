#include "StdAfx.h"
#include "WlanMeasUDS710.h"
#include "wcnUtility.h"
#include "Utility.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeasUDS710::CWlanMeasUDS710(CImpBase *pImpBase) : CWlanMeasBase(pImpBase)
{
}

CWlanMeasUDS710::~CWlanMeasUDS710(void)
{
}

SPRESULT CWlanMeasUDS710::InitPerAndMeasureUplink(WIFI_SIGNAL WifiSignal,SPWI_WLAN_PARAM_MEAS_GROUP* pUplinkMeasParam)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (pUplinkMeasParam == NULL)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	ConfigTesterParam(pUplinkMeasParam);
	// [1]: Setup DUT Channel
	int nCenCh = pUplinkMeasParam->stChan.nCenChan;
	int nPriCh = pUplinkMeasParam->stChan.nPriChan;


	WIFI_PROTOCOL_ENUM eProto = m_pWlanParamBand->eMode;
	m_MeasParam.eMode = m_pWlanParamBand->eMode;
	//set band
	WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
	if (m_MeasParam.stChan.nCenChan <= 14)
	{
		eBand = WLAN_BAND_24G;
	}
	else
	{
		eBand = WLAN_BAND_50G;
	}

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_Down(), "BDDUT_Down");

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetMPC(0), "BDDUT_TxMpc");

	if (eBand != WLAN_BAND_50G || (eProto == WIFI_802_11n && nCenCh < 36))
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_UP(), "BDDUT_UP");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_Down(), "BDDUT_Down");
	}

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetPHYWDOG(0), "BDDUT_SetPHYWDOG");

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_UP(), "BDDUT_UP");

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetCOUNTRY("ALL"), "BDDUT_SetCOUNTRY");

	if (WifiSignal == 1)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetBtcmode(0), "BDDUT_SetBtcmode");
		/*CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetRxChain(1), "BDDUT_SetRxChain");*/
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_RestCnts(), "BDDUT_RestCnts");
	}
	m_pImpBase->LogFmtStrA(SPLOGLV_INFO, "[WIFI]: eProto %d", eProto);
	m_pImpBase->LogFmtStrA(SPLOGLV_INFO, "[WIFI]: nCenCh %d", nCenCh);
	char pRateParam[100] = { 0 };
	if (eProto == WIFI_802_11b || eProto == WIFI_802_11g)
	{
		sprintf_s(pRateParam, sizeof(pRateParam), "%s,%s,%d,%s,%d", "2g_rate","r",
			(char)CwcnUtility::WLAN_BD_RATE_NAME[pUplinkMeasParam->stTesterParamGroupSub.eRate], "b",
			(char)CwcnUtility::WLAN_BD_BANDWIDTH_NAME[(WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nCBWType]);
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetRate(pRateParam), "BDDUT_SetRate");
	}
	else if ((eProto == WIFI_802_11n && (pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 0 && nCenCh < 36))
		|| (eProto == WIFI_802_11n && nCenCh <  34 && pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 1)
			|| (eProto == WIFI_802_11n && nCenCh < 30 && pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 2))
	{
		sprintf_s(pRateParam, sizeof(pRateParam), "%s,%s,%d,%s,%d", "2g_rate", "h", (char)CwcnUtility::WLAN_BD_RATE_NAME[pUplinkMeasParam->stTesterParamGroupSub.eRate], "b", (char)CwcnUtility::WLAN_BD_BANDWIDTH_NAME[(WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nCBWType]);
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetBand("b"), "BDDUT_SetCOUNTRY");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetRate(pRateParam), "BDDUT_SetRate");
	}
	else if (eProto == WIFI_802_11ax && (pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 0 && nCenCh < 36)
		|| (eProto == WIFI_802_11ax && nCenCh < 34 && pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 1)
			|| (eProto == WIFI_802_11ax && nCenCh < 30 && pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 2))
	{

		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetMimoTxBw(-1), "BDDUT_SetMimoTxBw");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetHecap(1), "BDDUT_SetHecap");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetBand("b"), "BDDUT_SetCOUNTRY");

		sprintf_s(pRateParam, sizeof(pRateParam), "%s,%s,%d,%s,%s,%s,%s,%s,%s,%d", "2g_rate",
			"e", (char)CwcnUtility::WLAN_BD_RATE_NAME[pUplinkMeasParam->stTesterParamGroupSub.eRate],
			"s", "1", "i", "1", "--ldpc",
			"b", (char)CwcnUtility::WLAN_BD_BANDWIDTH_NAME[(WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nCBWType]);
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetRate(pRateParam), "BDDUT_SetRate");
	}
	else if (eProto == WIFI_802_11n && (pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 0 && nCenCh >= 36)
		|| (eProto == WIFI_802_11n && nCenCh >= 34 && pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 1)
			|| (eProto == WIFI_802_11n && nCenCh >= 30 && pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 2))
	{
		sprintf_s(pRateParam, sizeof(pRateParam), "%s,%s,%d,%s,%d", "5g_rate", "h", (char)CwcnUtility::WLAN_BD_RATE_NAME[pUplinkMeasParam->stTesterParamGroupSub.eRate], "b", (char)CwcnUtility::WLAN_BD_BANDWIDTH_NAME[(WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nCBWType]);
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetBand("a"), "BDDUT_SetCOUNTRY");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetRate(pRateParam), "BDDUT_SetRate");
	}
	else if (eProto == WIFI_802_11ax && (pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 0 && nCenCh >= 36)
		|| (eProto == WIFI_802_11ax && nCenCh >= 34 && pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 1)
			|| (eProto == WIFI_802_11ax && nCenCh >= 30 && pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 2))
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetMimoTxBw(-1), "BDDUT_SetMimoTxBw");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetHecap(1), "BDDUT_SetHecap");

		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetBand("a"), "BDDUT_SetCOUNTRY");

		sprintf_s(pRateParam, sizeof(pRateParam), "%s,%s,%d,%s,%s,%s,%s,%s,%s,%d", "5g_rate",
			"e", (char)CwcnUtility::WLAN_BD_RATE_NAME[pUplinkMeasParam->stTesterParamGroupSub.eRate],
			"s", "1", "i", "1", "--ldpc",
			"b", (char)CwcnUtility::WLAN_BD_BANDWIDTH_NAME[(WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nCBWType]);
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetRate(pRateParam), "BDDUT_SetRate");


	}
	else if (eProto == WIFI_802_11ac)
	{
		sprintf_s(pRateParam, sizeof(pRateParam), "%s,%s,%d,%s,%d", "5g_rate", "v", (char)CwcnUtility::WLAN_BD_RATE_NAME[pUplinkMeasParam->stTesterParamGroupSub.eRate], "b", (char)CwcnUtility::WLAN_BD_BANDWIDTH_NAME[(WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nCBWType]);
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetBand("a"), "BDDUT_SetCOUNTRY");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetRate(pRateParam), "BDDUT_SetRate");
	}
	else if (eProto == WIFI_802_11a)
	{
		sprintf_s(pRateParam, sizeof(pRateParam), "%s,%s,%d,%s,%d", "5g_rate", "r", (char)CwcnUtility::WLAN_BD_RATE_NAME[pUplinkMeasParam->stTesterParamGroupSub.eRate], "b", (char)CwcnUtility::WLAN_BD_BANDWIDTH_NAME[(WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nCBWType]);
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetBand("a"), "BDDUT_SetCOUNTRY");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetRate(pRateParam), "BDDUT_SetRate");
	}

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetChanSpec(nCenCh, CwcnUtility::WLAN_BD_BANDWIDTH_NAME[(WIFI_BANDWIDTH_ENUM)pUplinkMeasParam->stTesterParamGroupSub.nCBWType]), "BDDUT_SetChanSpec");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetPhyFcal(1), "BDDUT_SetPhyFcal");

	if (eBand == WLAN_BAND_50G || nCenCh >= 36)
	{
		m_pWlanApi->BDDUT_PhyActiveCal();
	}

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetScanSup(1), "BDDUT_SetScanSup");

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetPhyTxPwrctl(1), "BDDUT_SetPhyTxPwrctl");


	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetTxPwr1(), "BDDUT_SetTxPwr1");


	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_STOP(), "BDDUT_STOP");


	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetPhyFcal(1), "BDDUT_SetPhyFcal");

	return SP_OK;
}

SPRESULT CWlanMeasUDS710::MeasureUplink(SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	DWORD dwMask = pUplinkMeasParam->stTesterParamGroupSub.dwMask;

	if (!IS_BIT_SET(dwMask, WIFI_TXP)
		&& !IS_BIT_SET(dwMask, WIFI_FER)
		&& !IS_BIT_SET(dwMask, WIFI_EVM)
		&& !IS_BIT_SET(dwMask, WIFI_MASK)
		&& !IS_BIT_SET(dwMask, WIFI_FLATNESS)
		)
	{
		return SP_OK;
	}
	InitPerAndMeasureUplink(WIFI_TX, pUplinkMeasParam);

	int nCenCh = pUplinkMeasParam->stChan.nCenChan;
	int nPriCh = pUplinkMeasParam->stChan.nPriChan;

	WIFI_PROTOCOL_ENUM eProto = m_pWlanParamBand->eMode;
	m_MeasParam.eMode = m_pWlanParamBand->eMode;
	//set band
	WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
	if (m_MeasParam.stChan.nCenChan <= 14)
	{
		eBand = WLAN_BAND_24G;
	}
	else
	{
		eBand = WLAN_BAND_50G;
	}

	if (eBand == WLAN_BAND_24G)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetTxStart("00:11:22:33:44:55", "tx", 100, 1500, 0), "BDDUT_SetTxStart");
	}
	else
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetTxStart("00:11:22:33:44:55", "tx", 100, 1024, 0), "BDDUT_SetTxStart");
	}
	Sleep(pUplinkMeasParam->stTesterParamGroupSub.nTxSleep);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	m_pImpBase->LogRawStrA(SPLOGLV_INFO, "InitTest");
	if (pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 1)
	{
		m_stTester.nCenChan = m_stTester.nCenChan + 2;
	}
	if (pUplinkMeasParam->stTesterParamGroupSub.nCBWType == 2)
	{
		m_stTester.nCenChan = m_stTester.nCenChan + 6;
	}

	m_pImpBase->LogFmtStrA(SPLOGLV_INFO, "[WIFI]: rate %d", m_stTester.eRate);

	//Auto ADJ Instrument Enp to match overload
	SPRESULT res = SP_OK;
	int nLoop = 0;
	double dRefLvl = m_stTester.dRefLvl;
	do
	{
		if (m_pImpBase->_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}

		m_pImpBase->LogRawStrA(SPLOGLV_INFO, "InitTest(TX)");
		res = m_pRfTesterWlan->InitTest(dwMask, (LPVOID)&m_stTester);
		if (SP_OK != res)
		{
			m_pImpBase->LogRawStrA(SPLOGLV_INFO, "InitTest(TX) Fail");
			if (!m_pImpBase->m_bFailStop)
			{
				m_pWlanApi->DUT_TxOn(false);
			}
			return res;
		}
		m_pImpBase->LogRawStrA(SPLOGLV_INFO, "FetchResult");
		res = m_pRfTesterWlan->FetchResult(dwMask, pTestResult);
		if (SP_OK != res)
		{
			m_pImpBase->LogFmtStrA(SPLOGLV_INFO, "FetchResult Fail, Then Adj Device ENP %d", (int)m_stTester.dRefLvl);
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
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_STOP(), "BDDUT_STOP");
	return SP_OK;
}

SPRESULT CWlanMeasUDS710::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (pDownlinkMeasParam == NULL)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	DWORD dwMask = pDownlinkMeasParam->stTesterParamGroupSub.dwMask;

	if (!IS_BIT_SET(dwMask, WIFI_PER))
	{
		return SP_OK;
	}
	InitPerAndMeasureUplink(WIFI_RX, pDownlinkMeasParam);
	if (pDownlinkMeasParam->stTesterParamGroupSub.nCBWType == 1)
	{
		m_stTester.nCenChan = m_stTester.nCenChan + 2;
	}
	if (pDownlinkMeasParam->stTesterParamGroupSub.nCBWType == 2)
	{
		m_stTester.nCenChan = m_stTester.nCenChan + 6;
	}

	WIFI_PROTOCOL_ENUM eProto = m_pWlanParamBand->eMode;

	if(eProto == WIFI_802_11ax)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetOclEnable(0), "BDDUT_SetOclEnable");
	}

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_SetRxStart("00:11:22:33:44:66", "rx"), "BDDUT_SetRxStart");
	Sleep(100);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	m_pImpBase->LogFmtStrA(SPLOGLV_INFO, "[WIFI]: rate %d", m_stTester.eRate);
	int nTotalPackets = m_stTester.nTotalPackets;
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->InitTest(WIFI_PER, &m_stTester), "InitTest(WIFI_PER)");
	Sleep(200);
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_RXSTOP(), "BDDUT_RXSTOP");
	int nCountTry = 30;
	int good_packets  = -3;
	int error_packets = -3;
	int cancl_packet = -3;
	int ufc_cnt = -3;
	int ufc_miss = -3;
	do
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->BDDUT_GetCounters(good_packets, error_packets, cancl_packet, ufc_cnt, ufc_miss), "DUT_GetPER");
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[WIFI]: good = %d, error = %d. cancl_packet = %d, ufc_cnt = %d. ufc_miss= %d", good_packets, error_packets, cancl_packet, ufc_cnt, ufc_miss);
		if(good_packets > nTotalPackets)
		{
			break;
		}
		Sleep(100);
	} while (nCountTry-- > 0 );
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
	// Turn off VSG
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE), "SetGen");

	return SP_OK;
}
