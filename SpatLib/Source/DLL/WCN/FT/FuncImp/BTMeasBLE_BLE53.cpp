#include "StdAfx.h"
#include "BTMeasBLE_BLE53.h"
#include <math.h>
#define BLE50_RX_MAX_RECEIVE_COUNTS 1500
//////////////////////////////////////////////////////////////////////////
//
CBTMeasBLE_BLE53::CBTMeasBLE_BLE53(CImpBase *pImpBase, ICBTApi *pBtApi) : CBTMeasBase(pImpBase, pBtApi)
{
}

CBTMeasBLE_BLE53::~CBTMeasBLE_BLE53(void)
{

}

BOOL CBTMeasBLE_BLE53::MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult)
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
	//set modulation index
	//CHKRESULT(m_pBTApi->DUT_SetModIndex(BT_TX, pUplinkMeasParam->eModIndex));

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCteType(BT_TX, (BT_CTE_TYPE)pUplinkMeasParam->nCteType), "DUT_SetCteType");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCteLength(BT_TX, (BT_CTE_LEN)pUplinkMeasParam->nCteLength), "DUT_SetCteLength");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCteSwitchPatternLen(BT_TX, (BT_CTE_ANTID)pUplinkMeasParam->nNumAntID), "DUT_SetCteSwitchPatternLen");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCteAntID(BT_TX, (BT_CTE_ANTID)pUplinkMeasParam->nNumAntID), "DUT_SetCteAntID");
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_TX, pUplinkMeasParam->nCh), "DUT_SetCH(BT_TX)");
    //Set packet type
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_TX, pUplinkMeasParam->ePacketType), "DUT_SetPacketType(BT_TX)");
    //Set packet length
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketLength(pUplinkMeasParam->nPacketLen), "DUT_SetPacketLength");
    //BLE_TX_PBRS9 performance perform
    if (   IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_POWER)
        || IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_ACP)
        )
    {
        dwTestMask = BLE_POWER | BLE_ACP;
		if(RF_PHY_S2 == pUplinkMeasParam->ePacketType)
		{
			dwTestMask = BLE_POWER;
		}
        m_stTester.ePattern =( pUplinkMeasParam->ePacketType == RF_PHY_2M ? BLE_TX_0xAA:BLE_TX_PBRS9);
        CHKRESULT(TxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));
    }

    if (   IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_MC)
        || IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_CFD)
        )
    {
        dwTestMask = 0x00;
        dwTestMask = BLE_MC | BLE_CFD;
		switch(pUplinkMeasParam->ePacketType)
		{
		case RF_PHY_1M:
		case RF_PHY_2M:
			m_stTester.ePattern = BLE_TX_0xF0;
			CHKRESULT(TxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));
			m_stTester.ePattern = BLE_TX_0xAA;
			CHKRESULT(TxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));
			if (fabs(pTestResult->Modulation.f1avg) > 0.0001)
			{
				pTestResult->Modulation.f2f1avg_rate = pTestResult->Modulation.f2avg / pTestResult->Modulation.f1avg;
			}
			break;
		case RF_PHY_S8:
			m_stTester.ePattern = BLE_TX_0xFF;
			CHKRESULT(TxSpecialPatternMeas(dwTestMask, &m_stTester, pTestResult));
			break;
			//S2 not do anything
		case RF_PHY_S2:
			break;
		default:
			m_pImpBase->LogFmtStrA(SPLOGLV_ERROR, "Invalid Packet type %d", pUplinkMeasParam->ePacketType);
			return SP_E_SPAT_INVALID_PARAMETER;
		}
        
    }

    return SP_OK;
}

BOOL CBTMeasBLE_BLE53::TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult)
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
  
    ConfigTesterParam(pDownlinkMeasParam);
	//Initial equipment
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitTest(BLE_PER, &m_stTester), "InitTest(BLE_PER)");
	//CTE
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCteType(BT_RX, (BT_CTE_TYPE)pDownlinkMeasParam->nCteType), "DUT_SetCteType");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCteLength(BT_RX, (BT_CTE_LEN)pDownlinkMeasParam->nCteLength), "DUT_SetCteLength");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCteSwitchPatternLen(BT_RX, (BT_CTE_ANTID)pDownlinkMeasParam->nNumAntID), "DUT_SetCteSwitchPatternLen");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCteAntID(BT_RX, (BT_CTE_ANTID)pDownlinkMeasParam->nNumAntID), "DUT_SetCteAntID");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCteSlotDurations((BT_CTE_SLOT_DURATIONS)pDownlinkMeasParam->nSlotDurations), "DUT_SetCteSlotDurations");
    //
	//set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_RX, pDownlinkMeasParam->nCh), "DUT_SetCH(BT_RX)");
	//Set packet type
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_RX, pDownlinkMeasParam->ePacketType), "DUT_SetPacketType(BT_RX)");
	//Set address
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetupAddr("00:00:88:C0:FF:EE"), "DUT_SetupAddr");

	//Rx ON
    CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(BT_RX, true)");
	m_stTester.nTotalPackets = pDownlinkMeasParam->nTotalPackets;

    CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "SetGen(MM_MODULATION, TRUE)");
	

    int error_bits = 0;
    int total_bits = 0;
    int error_pkts = 0;
    int total_pkts = 0;
    int rssi       = INVALID_NEGATIVE_INTEGER_VALUE;
    CSPTimer timer;
	SPRESULT res = SP_OK;
	int nPreCount = -1*pDownlinkMeasParam->nTotalPackets;
	int nTotalCount = pDownlinkMeasParam->nTotalPackets;
    do 
    {
		Sleep(300);
		res = m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi);
		if (SP_OK != res)
		{
			// RX OFF
            CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, false), "DUT_RFOn(BT_RX, false)");
			// Turn off VSG
			CHKRESULT(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE));
			Sleep(100);

			//Rx ON
            CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(BT_RX, true)");
			CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "SetGen(MM_MODULATION, TRUE)");
			Sleep(300);
			res = m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi);

			if (SP_OK != res)
			{
				m_pImpBase->_UiSendMsg("DUT_GetRxData", LEVEL_ITEM, 1, 0, 1);
				m_pImpBase->LogFmtStrA(SPLOGLV_ERROR, "[BT]: BLE DUT_GetRxData failed");
				m_pRfTesterBT->SetGen(MM_MODULATION, FALSE);
				m_pBTApi->DUT_RFOn(BT_RX, false);
				return SP_E_WCN_BT_RX_ERROR;
			}
		}
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER error_pkts=%d, total_pkts=%d, res = %d, rssi = %d", error_pkts, total_pkts, res, rssi);
		if (total_pkts >= nTotalCount || total_pkts - nPreCount < 10)
		{
			break;
		}
		nPreCount = total_pkts;
    } while (!timer.IsTimeOut(20000));

	// RX OFF
    CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, false), "DUT_RFOn(BT_RX, false)");
	// Turn off VSG
	CHKRESULT(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE));

	if(total_pkts - error_pkts > nTotalCount)
	{
		nTotalCount = total_pkts - error_pkts;
	}

	pRxReult->BER.dAvgValue = 1.0 * (nTotalCount - total_pkts + error_pkts) / nTotalCount * 100.0 ;
	pRxReult->Rssi.dAvgValue = (double)rssi;
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER = %d/%d (%.2f) RSSI = %.2f", nTotalCount - total_pkts + error_pkts, nTotalCount, pRxReult->BER.dAvgValue, pRxReult->Rssi.dAvgValue);

   	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth BLE PER measurement end!");

    return SP_OK;
}