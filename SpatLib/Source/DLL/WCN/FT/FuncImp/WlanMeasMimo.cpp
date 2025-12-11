#include "StdAfx.h"
#include "WlanMeasMimo.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeasMimo::CWlanMeasMimo(CImpBase *pImpBase) : CWlanMeasBaseMimo(pImpBase)
{
}

CWlanMeasMimo::~CWlanMeasMimo(void)
{
}

SPRESULT CWlanMeasMimo::MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	DWORD dwMask = pUplinkMeasParam->stTesterParamGroupSub.dwMask;

    if (   !IS_BIT_SET(dwMask, WIFI_TXP)
        && !IS_BIT_SET(dwMask, WIFI_FER)
        && !IS_BIT_SET(dwMask, WIFI_EVM)
        && !IS_BIT_SET(dwMask, WIFI_MASK)
        && !IS_BIT_SET(dwMask, WIFI_FLATNESS)
        )
    {
        return SP_OK;
    }
    m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	ConfigTesterParam( pUplinkMeasParam);
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
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate( pUplinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate");
	//Set packet length
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPacktLen(1024), "DUT_SetPacktLen");
	//set channel coding
//	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetDecoMode((WIFI_DECOMODE)pUplinkMeasParam->stTesterParamGroupSub.nChCode), "DUT_SetDecoMode");
	//Set tx mode
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetTxMode(WIFI_TXMODE_CARRIER_SUPPRESSION), "DUT_SetTxMode");
	//Set preamble
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPreamble(0), "DUT_SetPreamble");
	//Set payload
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPayLoad(WIFI_PAYLOAD_1111), "DUT_SetPayLoad");
    // [4] DUT start to transmit WIFI signal

	if (pUplinkMeasParam->stTesterParamGroupSub.bSetPwrLvl)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPwrLvl(pUplinkMeasParam->stTesterParamGroupSub.dPwrLvl), "DUT_SetPwrLvl()!");
	}

    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_TxOn(true), "DUT_TxOn");
    // Wait DUT to transmit WIFI signal, default is 300ms.
    // m_nTxOnSleep could be configured by INI file.
    Sleep(m_nTxOnSleep);
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
			//if (!m_pImpBase->m_bFailStop)
			{
				m_pWlanApi->DUT_TxOn(false);
			}
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
    CHKRESULT(m_pWlanApi->DUT_TxOn(false));
    return SP_OK;
}

SPRESULT CWlanMeasMimo::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (pDownlinkMeasParam == NULL)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	DWORD dwMask = pDownlinkMeasParam->stTesterParamGroupSub.dwMask;

    if (!IS_BIT_SET(dwMask, WIFI_PER) && IS_BIT_SET(dwMask, WIFI_RSSI))
    {
        return SP_OK;
    }
	    
    m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	m_stTester.Init();
    ConfigTesterParam( pDownlinkMeasParam);

	int nTotalPackets = m_stTester.nTotalPackets ;

	int nCenCh = pDownlinkMeasParam->stChan.nCenChan;
	int nPriCh = pDownlinkMeasParam->stChan.nPriChan;
	//Set Net mode
//	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetRfPath(pDownlinkMeasParam->stTesterParamGroupSub.eAnt), "DUT_SetRfPath");
	//Set channel band width
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetBandWidth((WIFI_BANDWIDTH_ENUM)pDownlinkMeasParam->stTesterParamGroupSub.nCBWType), "DUT_SetBandWidth");
	//Set signal band width
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetSigBandWidth((WIFI_BANDWIDTH_ENUM)pDownlinkMeasParam->stTesterParamGroupSub.nSBWType), "DUT_SetSigBandWidth");
	//Set Channel
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(nPriCh, nCenCh), "DUT_SetCH");  
	// [2]: Setup DUT data rate
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate( pDownlinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate");
	//set channel coding
	//CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetDecoMode((WIFI_DECOMODE)pDownlinkMeasParam->pstTesterParamGroupSub->nChCode), "DUT_SetDecoMode");
	//Set preamble
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPreamble(0), "DUT_SetPreamble");
	//Set payload

    CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(true), "DUT_RxOn");
	Sleep(100);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->InitTest(WIFI_PER, &m_stTester), "InitTest"); 
	//   Sleep(200);


	if (IS_BIT_SET(dwMask, WIFI_PER))
	{
		int nCountTry = 30;
		int good_packets  = 0;
		int error_packets = 0;
		int nPreCount = -nTotalPackets;
		do 
		{
            CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_GetPER(good_packets, error_packets), "DUT_GetPER");
			if(good_packets > nTotalPackets || good_packets+error_packets - nPreCount < 100)
			{
				break;
			}
			Sleep(100);
			nPreCount = good_packets+error_packets;
		} while (nCountTry-- > 0 );
		/// To improve the pass rate of mass production line, 
		/// We accept the interference which is not too big.
		/// So we ignore below check rules.
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[WIFI]: Total = %d, good = %d, error = %d.", nTotalPackets, good_packets, error_packets);

		if (good_packets >= nTotalPackets)
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
		int nCountTry = 30;
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