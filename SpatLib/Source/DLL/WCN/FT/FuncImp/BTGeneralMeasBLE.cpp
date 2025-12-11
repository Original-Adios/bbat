#include "StdAfx.h"
#include "BTGeneralMeasBLE.h"
#include <math.h>

#define BLE_RX_MAX_RECEIVE_COUNTS 1500

//////////////////////////////////////////////////////////////////////////
//
CBTGeneralMeasBLE::CBTGeneralMeasBLE(CImpBase *pImpBase, ICBTApi *pBtApi) : CBTMeasBase(pImpBase, pBtApi)
{

}

CBTGeneralMeasBLE::~CBTGeneralMeasBLE(void)
{

}

BOOL CBTGeneralMeasBLE::MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult)
{
	if (NULL == pUplinkMeasParam)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
    if (   !IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_POWER)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_ACP)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_MC)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_CFD)
        )
    {
        return SP_OK;
    }

    m_pImpBase->SetRepairBand($REPAIR_BAND_B_BLE);
    m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
    
    DWORD dwTestMask = 0x00;

    ConfigTesterParam( pUplinkMeasParam);
    //BT test process
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_TX, pUplinkMeasParam->nCh), "DUT_SetCH()!");
    //Set packet type
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_TX, pUplinkMeasParam->ePacketType), "DUT_SetPacketType()!");
    //Set packet length
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketLength(pUplinkMeasParam->nPacketLen), "DUT_SetPacketLength()!");
    //BLE_TX_PBRS9 performance perform
    if (   IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_POWER)
        || IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_ACP)
        )
    {
        dwTestMask = BLE_POWER | BLE_ACP;
        m_stTester.ePattern = BLE_TX_PBRS9;
        CHKRESULT(TxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));
    }

    if (   IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_MC)
        || IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_CFD)
        )
    {
        dwTestMask = 0x00;
        dwTestMask = BLE_MC | BLE_CFD;
        m_stTester.ePattern = BLE_TX_0xF0;

        CHKRESULT(TxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));
        m_stTester.ePattern = BLE_TX_0xAA;
        CHKRESULT(TxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));

        if (fabs(pTestResult->Modulation.f1avg) > 0.0001)
        {
            pTestResult->Modulation.f2f1avg_rate = pTestResult->Modulation.f2avg / pTestResult->Modulation.f1avg;
        }
    }

    return SP_OK;
}

BOOL CBTGeneralMeasBLE::TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult)
{
	if (pDownlinkMeasParam == NULL)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth BLE PER measurement start!");

    if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BLE_PER) && !IS_BIT_SET(pDownlinkMeasParam->dwMask, BLE_RSSI))
    {
        return SP_OK;
    }
    m_pImpBase->SetRepairBand($REPAIR_BAND_B_BLE);
   

    ConfigTesterParam( pDownlinkMeasParam);

    //BT test process
     //Initial equipment
    m_stTester.nTotalPackets = pDownlinkMeasParam->nTotalPackets;
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitTest(BLE_PER, &m_stTester), "Instrument::InitTest()!");
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_RX, pDownlinkMeasParam->nCh), "DUT_SetCH()!");

	//Rx ON
    CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(TRUE)!");
    CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "Instrument::SetGen()!");
    
    Sleep(200);
    int error_bits = 0;
    int total_bits = 0;
    int error_pkts = 0;
    int total_pkts = 0;
    int rssi       = INVALID_NEGATIVE_INTEGER_VALUE;
	int nPreCount = -1*pDownlinkMeasParam->nTotalPackets;
	int nTotalCount = pDownlinkMeasParam->nTotalPackets;
    CSPTimer timer;
    do 
    {
        if (SP_OK != m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi))
        {
			// RX OFF
            CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, FALSE), "DUT_RFOn(false)!");
			// Turn off VSG
            CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE), "Instrument::SetGen()!");


			Sleep(100);

			//Rx ON
            CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(TRUE)!");
            CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "Instrument::SetGen()!");



			if (SP_OK != m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi))
			{
				m_pImpBase->_UiSendMsg("DUT_GetRxData", LEVEL_ITEM, 1, 0, 1);
				m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE DUT_GetRxData failed");
                m_pRfTesterBT->SetGen(MM_MODULATION, FALSE);
				m_pBTApi->DUT_RFOn(BT_RX, false);
				return SP_E_WCN_BT_RX_ERROR;
			}
        }
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER error_pkts=%d, total_pkts=%d, rssi = %d", error_pkts, total_pkts, rssi);
        if (total_pkts >= nTotalCount || total_pkts - nPreCount < 10)
        {
            break;
        }
		nPreCount = total_pkts;
        Sleep(300);

    } while (!timer.IsTimeOut(10000));
	if(total_pkts - error_pkts > nTotalCount)
	{
		nTotalCount = total_pkts - error_pkts;
	}

	pRxReult->BER.dAvgValue = 1.0 * (nTotalCount - total_pkts + error_pkts) / nTotalCount * 100.0 ;
	pRxReult->Rssi.dAvgValue = (double)rssi;
    m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER = %d/%d (%.2f) RSSI = %.2f", nTotalCount - total_pkts + error_pkts, nTotalCount, pRxReult->BER.dAvgValue, pRxReult->Rssi.dAvgValue);
    // RX OFF
    CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, FALSE), "DUT_RFOn(false)!");
    // Turn off VSG
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE), "Instrument::SetGen()!");
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth BLE PER measurement end!");

    return SP_OK;
}