#include "StdAfx.h"
#include "BTMeasBLE_EXUDS710.h"
#include <math.h>
#define BLE50_RX_MAX_RECEIVE_COUNTS 1500
//////////////////////////////////////////////////////////////////////////
//
CBTMeasBLEExUDS710::CBTMeasBLEExUDS710(CImpBase *pImpBase, ICBTApi *pBtApi) : CBTMeasBase(pImpBase, pBtApi)
{
}

CBTMeasBLEExUDS710::~CBTMeasBLEExUDS710(void)
{

}

BOOL CBTMeasBLEExUDS710::MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult)
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

BOOL CBTMeasBLEExUDS710::TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult)
{
	
		if (pDownlinkMeasParam == NULL)
		{
			return SP_E_SPAT_INVALID_PARAMETER;
		}
		if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BLE_PER))
		{
			return SP_OK;
		}
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth BLE PER measurement start!");

		if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BLE_PER))
		{
			return SP_OK;
		}
		m_pImpBase->SetRepairBand($REPAIR_BAND_B_BLE);

		ConfigTesterParam(pDownlinkMeasParam);

		//set channel
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_RX, pDownlinkMeasParam->nCh), "DUT_SetCH(BT_RX)");
		//Set packet type
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_RX, pDownlinkMeasParam->ePacketType), "DUT_SetPacketType(BT_RX)");
		//Set packet length
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->BDDUT_SetPacketRxLength(65535), "DUT_SetPacketType(BT_RX)");

		//Rx ON
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(BT_RX, true)");
		m_stTester.nTotalPackets = pDownlinkMeasParam->nTotalPackets;
		//Initial equipment
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitTest(BLE_PER, &m_stTester), "InitTest(BLE_PER)");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "SetGen(MM_MODULATION, TRUE)");


		/*int error_bits = 0;
		int total_bits = 0;
		int error_pkts = 0;*/
		int total_pkts = 0;
		int rssi = INVALID_NEGATIVE_INTEGER_VALUE;
		CSPTimer timer;
		SPRESULT res = SP_OK;
		int nTotalCount = pDownlinkMeasParam->nTotalPackets;
		do
		{
			Sleep(1000);
			CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, false), "DUT_RFOn(BT_RX, false)");
			Sleep(300);
			res = m_pBTApi->BDDUT_BleGetRxData(total_pkts);
			if (SP_OK != res)
			{
				// RX OFF
				CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, false), "DUT_RFOn(BT_RX, false)");
				// Turn off VSG
				CHKRESULT(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE));
				Sleep(100);

				//Rx ON
				CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(BT_RX, true)");

				CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "SetGen(MM_MODULATION, TRUE)");
				Sleep(1000);
				CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, false), "DUT_RFOn(BT_RX, false)");
				Sleep(300);
				
				res = m_pBTApi->BDDUT_BleGetRxData(total_pkts);

				if (SP_OK != res)
				{
					m_pImpBase->_UiSendMsg("DUT_GetRxData", LEVEL_ITEM, 1, 0, 1);
					m_pImpBase->LogFmtStrA(SPLOGLV_ERROR, "[BT]: BLE DUT_GetRxData failed");
					CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE), "Instrument:SetGen(Off)");
					m_pBTApi->DUT_RFOn(BT_RX, false);
					return SP_E_WCN_BT_RX_ERROR;
				}
			}
			m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER nTotalCount=%d, total_pkts=%d, res = %d, rssi = %d", nTotalCount, total_pkts, res, rssi);

			if (total_pkts <= nTotalCount && total_pkts > 0 )
			{
				break;
			}
		} while (!timer.IsTimeOut(20000));

		// Turn off VSG
		CHKRESULT(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE));

		pRxReult->BER.dAvgValue = 1.0 * (nTotalCount - total_pkts) / nTotalCount * 100.0;
		pRxReult->Rssi.dAvgValue = (double)rssi;
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER = %d/%d (%.2f) RSSI = %.2f", nTotalCount - total_pkts, nTotalCount, pRxReult->BER.dAvgValue, pRxReult->Rssi.dAvgValue);
		
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth BLE PER measurement end!");

		return SP_OK;

}