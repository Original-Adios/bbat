#include "StdAfx.h"
#include "BTMeasBDRUDS710.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////
//
CBTMeasBDRUDS710::CBTMeasBDRUDS710(CImpBase *pImpBase, ICBTApi *pBtApi) : CBTMeasBase(pImpBase, pBtApi)
{
  
}

CBTMeasBDRUDS710::~CBTMeasBDRUDS710(void)
{ 

}

BOOL CBTMeasBDRUDS710::MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult)
{
    if (NULL == pUplinkMeasParam)
    {
        return SP_E_SPAT_INVALID_PARAMETER;
    }

    if (   !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_POWER)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_20BW)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_ACP)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_ICFR)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_CFD)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_MC)
        )
    {
        return SP_OK;
    }

	/*SPBT_RESULT_T pTestSelfResult;
	pTestSelfResult.BER.dAvgValue = 100.0;
	pTestSelfResult.Init();*/
    m_pImpBase->SetRepairBand($REPAIR_BAND_B_BDR);
    m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);

    DWORD dwTestMask = 0x00;

	ConfigTesterParam(pUplinkMeasParam);

    //BT test process
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_TX, pUplinkMeasParam->nCh), "DUT_SetCH(BT_TX)");
	//Set logical channel
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->BDDUT_SetTxLogicCh(pUplinkMeasParam->nTxLogicCh), "DUT_SetPacketType(BT_TX)");
    //Set packet type
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_TX, pUplinkMeasParam->ePacketType), "DUT_SetPacketType(BT_TX)");
    //Set packet length
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketLength(pUplinkMeasParam->nPacketLen), "DUT_SetPacketLength");

	
    //BDR_TX_PBRS9 performance perform
    

    if (   IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_CFD)
        || IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_POWER)
		|| IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_ACP)
		|| IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_MC)
        )
    {
		//CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitDev(TM_NON_SIGNAL, SUB_FT_NST, NULL), "Instrument::InitDev()");
        dwTestMask = 0x00;
        dwTestMask = BDR_CFD |BDR_POWER | BDR_ACP | BDR_MC;
        m_stTester.ePattern = BDR_TX_0xF0;
        CHKRESULT(BDTxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));
        m_stTester.ePattern = BDR_TX_0xAA;
        CHKRESULT(BDTxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));

        if (fabs(pTestResult->Modulation.f1avg) > 0.0001)
        {
            pTestResult->Modulation.f2f1avg_rate = pTestResult->Modulation.f2avg / pTestResult->Modulation.f1avg;
        }
    }

	if (
		IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_ICFR)
		|| IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_20BW)
		)
	{
		dwTestMask = BDR_ICFR | BDR_20BW;
		m_stTester.ePattern = BDR_TX_PBRS9;
		CHKRESULT(BDTxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));
	}
    
    return SP_OK;
}

BOOL CBTMeasBDRUDS710::TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult)
{
	if (NULL == pDownlinkMeasParam)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth BDR PER measurement start!");

    if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BDR_BER))
    {
        return SP_OK;
    }

    m_pImpBase->SetRepairBand($REPAIR_BAND_B_BDR);

    ConfigTesterParam(pDownlinkMeasParam);

    //BT test process
	//Set address
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetupAddr("43:25:00:00:01:43"), "DUT_SetupAddr()!");
	//set channel
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->BDDUT_SetRxRptperd(1000), "DUT_SetCH()!");
    //AT+SPBTTEST=RXRPTPERD,1000
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_RX, pDownlinkMeasParam->nCh), "DUT_SetCH()!");
	//Set packet length
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPattern(BT_RX, BDBDR_RX_PBRS9), "DUT_SetPattern()!");
	//Set packet length
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->BDDUT_SetRxLogicCh(pDownlinkMeasParam->nTxLogicCh), "DUT_SetPattern()!");
    //Set packet type
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_RX, pDownlinkMeasParam->ePacketType), "DUT_SetPacketType()!");
	
	//Initial equipment
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitTest(BDR_BER, &m_stTester), "Instrument::InitTest()!");

    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "Instrument::SetGen() on!"); 
	//Rx ON
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(TRUE)!");  
//	Sleep(100);

    int error_bits = 0;
    int total_bits = 0;
    int error_pkts = 0;
    int total_pkts = 0;
    int rssi       = INVALID_NEGATIVE_INTEGER_VALUE;

    CSPTimer timer;
    do 
    {
		SPRESULT res = m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi);
        if (SP_OK != res)
        {
			// RX OFF
			CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, FALSE), "DUT_RFOn(false)!");
			Sleep(100);
			//Rx ON
			CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(TRUE)!");

			res = m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi);

			if (SP_OK != res)
			{
				m_pImpBase->_UiSendMsg("DUT_GetRxData", LEVEL_ITEM, 1, 0, 1);
				m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Perform DUT_GetRxData function failed!");
				CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE), "Instrument:SetGen(Off)");
				CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, false), "DUT_RFOn()!");
				return res;
			}
        }

		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "error_bits = %d, total_bits = %d, rssi = %d!", error_bits, total_bits,rssi);
        if (total_bits >= pDownlinkMeasParam->nTotalPackets)
        {
            break;
        }

    } while (!timer.IsTimeOut(15000));

	if(total_bits < pDownlinkMeasParam->nTotalPackets)
	{
		pRxReult->BER.dAvgValue = 100.0;
	}
	else
	{
		pRxReult->BER.dAvgValue = 1.0 * error_bits / total_bits * 100.0;
	}

	pRxReult->Rssi.dAvgValue = (double)rssi;
    m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR PER = %d/%d (%.2f) RSSI = %.2f", error_bits, total_bits, pRxReult->BER.dAvgValue, pRxReult->Rssi.dAvgValue);
    // RX OFF
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, FALSE), "DUT_RFOn(false)!");
    // Turn off VSG
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE), "Instrument::SetGen()!");

	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth BDR PER measurement end!");
    return SP_OK;
}