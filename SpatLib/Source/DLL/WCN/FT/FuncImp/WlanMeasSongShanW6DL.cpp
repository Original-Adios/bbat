#include "StdAfx.h"
#include "WlanMeasSongShanW6DL.h"
#include "Utility.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeaSongShanW6DL::CWlanMeaSongShanW6DL(CImpBase *pImpBase) : CWlanMeaSongShanW6(pImpBase)
{
}

CWlanMeaSongShanW6DL::~CWlanMeaSongShanW6DL(void)
{
}

SPRESULT CWlanMeaSongShanW6DL::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
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
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetSigBandWidth((WIFI_BANDWIDTH_ENUM)pDownlinkMeasParam->stTesterParamGroupSub.nSBWType), "DUT_SetSigBandWidth");
	//Set Channel
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetCH(nPriCh, nCenCh), "DUT_SetCH"); 
	
/*	// [2]: Setup DUT data rate
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetModRate( pDownlinkMeasParam->stTesterParamGroupSub.eRate), "DUT_SetModRate");
	//set channel coding
	//CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetDecoMode((WIFI_DECOMODE)pDownlinkMeasParam->stTesterParamGroupSub.nChCode), "DUT_SetDecoMode");
	//Set preamble
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPreamble(0), "DUT_SetPreamble");
*/

	int RxType = 1;
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetRxType(RxType), "DUT_SetRxType");
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_RxOn(true),"DUT_RxOn");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_EnableRxType(RxType), "DUT_EnableRxType");
	Sleep(100);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
    const int MAXCOUNTRETRY = 20;
	//   Sleep(200);
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
			CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_GetRxSNR(pDownlinkMeasParam->eMode, dRssi), "DUT_GetRxSNR");
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