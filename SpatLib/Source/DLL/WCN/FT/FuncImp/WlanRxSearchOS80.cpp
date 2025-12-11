#include "StdAfx.h"
#include "WlanRxSearchOS80.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanRxSearchOS80::CWlanRxSearchOS80(CImpBase *pImpBase) : CWlanMeasBaseProbe(pImpBase)
{
}

CWlanRxSearchOS80::~CWlanRxSearchOS80(void)
{
}

SPRESULT CWlanRxSearchOS80::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
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

	m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	ConfigTesterParam(pDownlinkMeasParam);

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
	//CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetDecoMode((WIFI_DECOMODE)pDownlinkMeasParam->stTesterParamGroupSub.nChCode), "DUT_SetDecoMode");
	//set channel coding	¡ä¨°?a??11ax
	//CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetDecoMode((WIFI_DECOMODE)pDownlinkMeasParam->stTesterParamGroupSub.nChCode), "DUT_SetDecoMode");
	//Set preamble
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pWlanApi->DUT_SetPreamble(0), "DUT_SetPreamble");
	//Set payload

    CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_RxOn(true), "DUT_RxOn");
	Sleep(200);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->InitTest(WIFI_PER, &m_stTester), "InitTest"); 

	//   Sleep(200);
	int nCountTry = 3;
	int good_packets  = 0;
	int error_packets = 0;
	int nTotalPackets = m_stTester.nTotalPackets;
	do
	{
        CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(m_pWlanApi->DUT_GetPER(good_packets, error_packets), "DUT_GetPER");
		Sleep(100);
		nCountTry--;
	} while (nCountTry > 0 && (good_packets+error_packets)  < nTotalPackets );
	/// To improve the pass rate of mass production line, 
	/// We accept the interference which is not too big.
	/// So we ignore below check rules.
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[WIFI]: Total = %d, good = %d, error = %d.", good_packets+error_packets, good_packets, error_packets);

	if (good_packets + error_packets < 800)
	{
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[WIFI]: %s.", "good_packets + error_packets < 800");
		pRxReult->per.dAvgValue = 100.0;
	}
	else if (good_packets > nTotalPackets)
	{
		pRxReult->per.dAvgValue = 0.0;
	}
	else
	{
		pRxReult->per.dAvgValue = ((double)(nTotalPackets - good_packets))/(nTotalPackets) * 100.0;
	}
	// RX OFF
	/*CHKRESULT(m_pWlanApi->DUT_RxOn(false));*/
	// Turn off VSG
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterWlan->SetGen(MM_MODULATION, FALSE), "SetGen");

	return SP_OK;
}