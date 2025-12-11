#include "StdAfx.h"
#include "WlanMeasSongShanW6.h"
#include "Utility.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeaSongShanW6::CWlanMeaSongShanW6(CImpBase *pImpBase) : CWlanMeasBase(pImpBase)
{

}

CWlanMeaSongShanW6::~CWlanMeaSongShanW6(void)
{
}

SPRESULT CWlanMeaSongShanW6::MeasureUplink(SPWI_WLAN_PARAM_MEAS_GROUP* pUplinkMeasParam, SPWI_RESULT_T* pTestResult)
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

	if (WIFI_802_11ax == pUplinkMeasParam->eMode)
	{
		//HE_LTF_SIZE
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetHeLtfSize((WIFI_HE_LTF_SIZE)pUplinkMeasParam->stTesterParamGroupSub.nHeLtfSize), "DUT_SetHeLtfSize");

		//RU_SIZE
		//CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetRuSize((WIFI_RU_SIZE)pUplinkMeasParam->stTesterParamGroupSub.RuSize), "DUT_SetRuSize");

		//GUARD INTERVAL
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetGuardInterval((WIFI_GUARDINTERVAL)pUplinkMeasParam->stTesterParamGroupSub.nGuardInterval), "DUT_SetGuardInterval");
	}
	////Set Channel
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(nPriCh, nCenCh), "DUT_SetCH");
	}
	// [2]: Setup DUT data rate
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate(pUplinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate");



	//set channel coding
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetDecoMode((WIFI_DECOMODE)pUplinkMeasParam->stTesterParamGroupSub.nChCode), "DUT_SetDecoMode");

	//Set packet length
	if (WIFI_802_11b == pUplinkMeasParam->eMode
		|| WIFI_802_11g == pUplinkMeasParam->eMode
		|| WIFI_802_11a == pUplinkMeasParam->eMode
		)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPacktLen(1024), "DUT_SetPacktLen");
	}
	else
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPacktLen(4096), "DUT_SetPacktLen");
	}
	/*	//Set tx mode
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetTxMode(WIFI_TXMODE_CARRIER_SUPPRESSION), "DUT_SetTxMode");
	*/

	if (WIFI_802_11ax == pUplinkMeasParam->eMode || WIFI_802_11ac == pUplinkMeasParam->eMode)
	{
		//Set payload
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPayLoad(WIFI_PAYLOAD_0000), "DUT_SetPayLoad");
	}
	else
	{
		//Set preamble
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPreamble(0), "DUT_SetPreamble");
	}


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
    //    // TX off while failure
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_TxOn(false), "DUT_TxOn(false)");
    return SP_OK;
}

SPRESULT CWlanMeaSongShanW6::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
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
    ConfigTesterParam( pDownlinkMeasParam);

	int nCenCh = pDownlinkMeasParam->stChan.nCenChan;
	int nPriCh = pDownlinkMeasParam->stChan.nPriChan;


	//Set Band
	//Set Net mode
//	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetRfPath(pDownlinkMeasParam->stTesterParamGroupSub.eAnt), "DUT_SetRfPath");
	//Set channel band width
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetBandWidth((WIFI_BANDWIDTH_ENUM)pDownlinkMeasParam->stTesterParamGroupSub.nCBWType), "DUT_SetBandWidth");
	//Set signal band width
//	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetSigBandWidth((WIFI_BANDWIDTH_ENUM)pDownlinkMeasParam->stTesterParamGroupSub.nSBWType), "DUT_SetSigBandWidth");
	//Set Channel
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(nPriCh, nCenCh), "DUT_SetCH"); 
	
/*	// [2]: Setup DUT data rate
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate( pDownlinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate");
	//set channel coding
	//CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetDecoMode((WIFI_DECOMODE)pDownlinkMeasParam->stTesterParamGroupSub.nChCode), "DUT_SetDecoMode");
	//Set preamble
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPreamble(0), "DUT_SetPreamble");
*/
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_RxOn(true), "DUT_RxOn");
	Sleep(100);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	//   Sleep(200);
	const int MAXCOUNTRETRY = 20;
	if (IS_BIT_SET(dwMask, WIFI_PER))
	{
		int nTotalPackets = m_stTester.nTotalPackets;
		int nCountTry = MAXCOUNTRETRY;
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

			if (MAXCOUNTRETRY / 2 == nCountTry)
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
	/*CHKRESULT(m_pWlanApi->DUT_RxOn(false));*/
	// Turn off VSG
	//MessageBoxA(NULL,"debug output port", "debug", MB_OK);
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE), "SetGen");

    return SP_OK;
}


SPRESULT CWlanMeaSongShanW6::ShowMeasRst(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T* pTestResult, int nChan, double dBSLevel, int nSbwType, int nCbwType, ANTENNA_ENUM eAnt)
{
    auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    if (pTestResult == NULL)
    {
        CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_INVALID_PARAMETER, "Wlan::Invalid result!");
    }
    BOOL bFailStop = m_pImpBase->m_bFailStop;

    double dFreq = CwcnUtility::WIFI_Ch2MHz(nChan);
    LPCSTR strCBW = CwcnUtility::WLAN_BW_NAME[nCbwType];
    LPCSTR strSBW = CwcnUtility::WLAN_BW_NAME[nSbwType];
    LPCSTR lpRate = CwcnUtility::WlanGetRateString(eRate);
    if (IS_BIT_SET(dwItemMask, WIFI_TXP))
    {
        double dTxpLow = 0.0;
        double dTxpUpp = 0.0;
        if (ANT_PRIMARY == eAnt)
        {
            dTxpLow = m_pWlanParamBand->stSpec.dTxpRate[eRate].low;
            dTxpUpp = m_pWlanParamBand->stSpec.dTxpRate[eRate].upp;
        }
        else
        {
            dTxpLow = m_pWlanParamBand->stSpec.dTXPant2ndRate[eRate].low;
            dTxpUpp = m_pWlanParamBand->stSpec.dTXPant2ndRate[eRate].upp;
        }

        m_pImpBase->_UiSendMsg("Transmit Power"
            , LEVEL_ITEM | LEVEL_FT
            , dTxpLow
            , pTestResult->txp.dAvgValue
            , dTxpUpp
            , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
            , nChan
            , "dBm"
            , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
            , dFreq
            , CwcnUtility::WLAN_ANT_NAME[eAnt]
            , lpRate
            , strCBW
            , strSBW);


        if (bFailStop && (pTestResult->txp.dAvgValue < dTxpLow
            || pTestResult->txp.dAvgValue > dTxpUpp))
        {
            return SP_E_WCN_WLAN_TXP_FAIL;
        }
    }

    if (IS_BIT_SET(dwItemMask, WIFI_CW))
    {
        m_pImpBase->_UiSendMsg("Transmit CW Power"
            , LEVEL_ITEM | LEVEL_FT
            , m_pWlanParamBand->stSpec.dTxp.low
            , pTestResult->CWPwr.dAvgValue
            , m_pWlanParamBand->stSpec.dTxp.upp
            , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
            , nChan
            , "dBm"
            , "Frequency:%0.2f;%s"
            , dFreq
            , CwcnUtility::WLAN_ANT_NAME[eAnt]
        );
    }

    if (IS_BIT_SET(dwItemMask, WIFI_FER))
    {
        m_pImpBase->_UiSendMsg("Freq Error"
            , LEVEL_ITEM | LEVEL_FT
            , m_pWlanParamBand->stSpec.dFer.low
            , pTestResult->fer.dAvgValue
            , m_pWlanParamBand->stSpec.dFer.upp
            , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
            , nChan
            , "PPM"
            , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
            , dFreq
            , CwcnUtility::WLAN_ANT_NAME[eAnt]
            , lpRate
            , strCBW
            , strSBW);
    }

    if (IS_BIT_SET(dwItemMask, WIFI_IQ))
    {
        m_pImpBase->_UiSendMsg("IQ Offset Error"
            , LEVEL_ITEM | LEVEL_FT
            , m_pWlanParamBand->stSpec.dIQ.low
            , pTestResult->iqOffset.dAvgValue
            , m_pWlanParamBand->stSpec.dIQ.upp
            , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
            , nChan
            , "dB"
            , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
            , dFreq
            , CwcnUtility::WLAN_ANT_NAME[eAnt]
            , lpRate
            , strCBW
            , strSBW);
    }

    if (IS_BIT_SET(dwItemMask, WIFI_EVM))
    {
        m_pImpBase->_UiSendMsg("EVM"
            , LEVEL_ITEM | LEVEL_FT
            , m_pWlanParamBand->stSpec.dEvm[eRate].low
            , pTestResult->evm.dAvgValue
            , m_pWlanParamBand->stSpec.dEvm[eRate].upp
            , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
            , nChan
            , "dB"
            , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
            , dFreq
            , CwcnUtility::WLAN_ANT_NAME[eAnt]
            , lpRate
            , strCBW
            , strSBW);
    }

    if (IS_BIT_SET(dwItemMask, WIFI_MASK))
    {
        LPCSTR lpcMask11b[] = { "AB", "CD", "DC", "BA", "NA", "NA", "NA", "NA" };
        LPCSTR lpcMask[] = { "AB", "BC", "CD", "DE", "ED", "DC", "CB", "BA" };
        char szItemName[64] = { 0 };
        int nCount = 8;
        if (eRate <= CCK_11)
        {
            nCount = 4;
        }
        for (int i = 0; i < nCount; i++)
        {
            if (eRate <= CCK_11)
            {
                sprintf_s(szItemName, "Spectrum_Mask(%s)_Margin", lpcMask11b[i]);
            }
            else
            {
                sprintf_s(szItemName, "Spectrum_Mask(%s)_Margin", lpcMask[i]);
            }

            m_pImpBase->_UiSendMsg(szItemName
                , LEVEL_ITEM | LEVEL_FT
                , NOLOWLMT
                , pTestResult->mask.dMagin[i]
                , m_pWlanParamBand->stSpec.dSpecMaskMargin[i]
                , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
                , dFreq
                , CwcnUtility::WLAN_ANT_NAME[eAnt]
                , lpRate
                , strCBW
                , strSBW);
        }
    }
    if (eRate > CCK_11)
    {
        if (IS_BIT_SET(dwItemMask, WIFI_FLATNESS))
        {
            m_pImpBase->_UiSendMsg("FLATNESS"
                , LEVEL_ITEM | LEVEL_FT
                , TRUE
                , pTestResult->flatness.bPass
                , TRUE
                , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
                , dFreq
                , CwcnUtility::WLAN_ANT_NAME[eAnt]
                , lpRate
                , strCBW
                , strSBW);
        }
    }

    if (IS_BIT_SET(dwItemMask, WIFI_PER))
    {
        m_pImpBase->_UiSendMsg("SensitivityTest"
            , LEVEL_ITEM | LEVEL_FT
            , 0
            , pTestResult->per.dAvgValue
            , m_pWlanParamBand->stSpec.dPer
            , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s;BSLevel:%.2f"
            , dFreq
            , CwcnUtility::WLAN_ANT_NAME[eAnt]
            , lpRate
            , strCBW
            , strSBW
            , dBSLevel);

        if (bFailStop && pTestResult->per.dAvgValue > m_pWlanParamBand->stSpec.dPer)
        {
            return SP_E_WCN_WLAN_PER_FAIL;
        }
    }

    if (IS_BIT_SET(dwItemMask, WIFI_RSSI))
    {
        m_pImpBase->_UiSendMsg("RSSITest"
            , LEVEL_ITEM | LEVEL_FT
            , dBSLevel + m_pWlanParamBand->stSpec.dRssi.low
            , pTestResult->Rssi.dAvgValue
            , dBSLevel + m_pWlanParamBand->stSpec.dRssi.upp
            , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s;BSLevel:%.2f"
            , dFreq
            , CwcnUtility::WLAN_ANT_NAME[eAnt]
            , lpRate
            , strCBW
            , strSBW
            , dBSLevel);

        if (bFailStop
            && (pTestResult->Rssi.dAvgValue < dBSLevel + m_pWlanParamBand->stSpec.dRssi.low
                || pTestResult->Rssi.dAvgValue > dBSLevel + m_pWlanParamBand->stSpec.dRssi.upp)
            )
        {
            return SP_E_WCN_WLAN_RSSI_FAIL;
        }
    }

    return SP_OK;
}

SPRESULT CWlanMeaSongShanW6::JudgeMeasRst(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T* pTestResult, double dBSLevel, ANTENNA_ENUM eAnt/* = ANT_PRIMARY*/)
{
    auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    if (pTestResult == NULL)
    {
        return SP_E_INVALID_PARAMETER;
    }

    SPRESULT eCmpRlst = SP_OK;
    SPRESULT eItemRlst = SP_OK;

    if (IS_BIT_SET(dwItemMask, WIFI_TXP))
    {
        double dTxpLow = 0.0;
        double dTxpUpp = 0.0;
        if (ANT_PRIMARY == eAnt)
        {
            dTxpLow = m_pWlanParamBand->stSpec.dTxpRate[eRate].low;
            dTxpUpp = m_pWlanParamBand->stSpec.dTxpRate[eRate].upp;
        }
        else
        {
            dTxpLow = m_pWlanParamBand->stSpec.dTXPant2ndRate[eRate].low;
            dTxpUpp = m_pWlanParamBand->stSpec.dTXPant2ndRate[eRate].upp;
        }
        RSLT_RANGE_CHECK(pTestResult->txp.dAvgValue, dTxpLow, dTxpUpp, eItemRlst, SP_E_WCN_WLAN_TXP_FAIL);
        if (SP_OK == eCmpRlst)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            eCmpRlst = eItemRlst;
        }
    }

    if (IS_BIT_SET(dwItemMask, WIFI_CW))
    {
        RSLT_RANGE_CHECK(pTestResult->CWPwr.dAvgValue, m_pWlanParamBand->stSpec.dTxp.low, m_pWlanParamBand->stSpec.dTxp.upp, eItemRlst, SP_E_WCN_WLAN_TXP_FAIL);
        if (SP_OK == eCmpRlst)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            eCmpRlst = eItemRlst;
        }
    }

    if (IS_BIT_SET(dwItemMask, WIFI_FER))
    {
        RSLT_RANGE_CHECK(pTestResult->fer.dAvgValue, m_pWlanParamBand->stSpec.dFer.low, m_pWlanParamBand->stSpec.dFer.upp, eItemRlst, SP_E_WCN_WLAN_FER_FAIL);
        if (SP_OK == eCmpRlst)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_FER);
            eCmpRlst = eItemRlst;
        }
    }

    //IQ Offset
    if (IS_BIT_SET(dwItemMask, WIFI_IQ))
    {
        RSLT_RANGE_CHECK(pTestResult->iqOffset.dAvgValue, m_pWlanParamBand->stSpec.dIQ.low, m_pWlanParamBand->stSpec.dIQ.upp, eItemRlst, SP_E_WCN_WLAN_IQ_FAIL);
        if (SP_OK == eCmpRlst)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_FER);
            eCmpRlst = eItemRlst;
        }
    }

    if (IS_BIT_SET(dwItemMask, WIFI_EVM))
    {
        RSLT_RANGE_CHECK(pTestResult->evm.dAvgValue, m_pWlanParamBand->stSpec.dEvm[eRate].low, m_pWlanParamBand->stSpec.dEvm[eRate].upp, eItemRlst, SP_E_WCN_WLAN_EVM_FAIL);
        if (SP_OK == eCmpRlst)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_EVM);
            eCmpRlst = eItemRlst;
        }
    }

    if (IS_BIT_SET(dwItemMask, WIFI_MASK))
    {
        int nCount = 8;
        if (eRate <= CCK_11)
        {
            nCount = 4;
        }
        for (int i = 0; i < nCount; i++)
        {
            if (pTestResult->mask.dMagin[i] > m_pWlanParamBand->stSpec.dSpecMaskMargin[i])
            {
                if (SP_OK == eCmpRlst)
                {
                    m_pImpBase->SetRepairItem($REPAIR_ITEM_ORFS);
                    eCmpRlst = SP_E_WCN_WLAN_SEM_FAIL;
                }
                break;
            }
        }
    }
    if (eRate > CCK_11)
    {
        if (IS_BIT_SET(dwItemMask, WIFI_FLATNESS) &&
            !pTestResult->flatness.bPass)
        {
            if (SP_OK == eCmpRlst)
            {
                m_pImpBase->SetRepairItem($REPAIR_ITEM_FLATNESS);
                eCmpRlst = SP_E_WCN_WLAN_FLATNESS_FAIL;
            }
        }
    }

    if (IS_BIT_SET(dwItemMask, WIFI_PER))
    {
        RSLT_RANGE_CHECK(pTestResult->per.dAvgValue, 0, m_pWlanParamBand->stSpec.dPer, eItemRlst, SP_E_WCN_WLAN_PER_FAIL);
        if (SP_OK == eCmpRlst)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BLER);
            eCmpRlst = eItemRlst;
        }
    }

    if (IS_BIT_SET(dwItemMask, WIFI_RSSI))
    {
        RSLT_RANGE_CHECK(pTestResult->Rssi.dAvgValue, dBSLevel + m_pWlanParamBand->stSpec.dRssi.low, dBSLevel + m_pWlanParamBand->stSpec.dRssi.upp, eItemRlst, SP_E_WCN_WLAN_PER_FAIL);
        if (SP_OK == eCmpRlst)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BLER);
            eCmpRlst = eItemRlst;
        }
    }
    return eCmpRlst;
}