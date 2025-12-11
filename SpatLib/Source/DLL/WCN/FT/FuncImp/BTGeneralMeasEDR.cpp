#include "StdAfx.h"
#include "BTGeneralMeasEDR.h"

//////////////////////////////////////////////////////////////////////////
//
CBTGeneralMeasEDR::CBTGeneralMeasEDR(CImpBase *pImpBase, ICBTApi *pBtApi) 
: CBTMeasBase(pImpBase, pBtApi)
{
    
}

CBTGeneralMeasEDR::~CBTGeneralMeasEDR(void)
{
   
}

BOOL CBTGeneralMeasEDR::MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult)
{
	if (NULL == pUplinkMeasParam)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
    if (   !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_POWER)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, EDR_ETP)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, EDR_DEVM)
        && !IS_BIT_SET(pUplinkMeasParam->dwMask, EDR_ACP)
        )
    {
        return SP_OK;
    }

    m_pImpBase->SetRepairBand($REPAIR_BAND_B_EDR);
    m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
        
     ConfigTesterParam(pUplinkMeasParam);

    //BT test process
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_TX, pUplinkMeasParam->nCh), "DUT_SetCH(BT_TX)");
    //Set packet type
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_TX, (int)pUplinkMeasParam->ePacketType), "DUT_SetPacketType(BT_TX)");
    //Set packet length
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketLength(pUplinkMeasParam->nPacketLen), "DUT_SetPacketLength");
    //BDR_TX_PBRS9 performance perform
    m_stTester.ePattern = BDR_TX_PBRS9;
    CHKRESULT(TxSpecialPatternMeas(pUplinkMeasParam->dwMask, &m_stTester, pTestResult));

    return SP_OK;
}

BOOL CBTGeneralMeasEDR::TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult)
{
	if (NULL == pDownlinkMeasParam)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth EDR PER measurement start!");

//    SPRESULT bOperRes = SP_E_WCN_BT_RX_ERROR;

    if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BDR_BER) && !IS_BIT_SET(pDownlinkMeasParam->dwMask, BLE_RSSI))
    {
        return SP_OK;
    }

    m_pImpBase->SetRepairBand($REPAIR_BAND_B_EDR);
  
    ConfigTesterParam( pDownlinkMeasParam);

    //BT test process
    //Initial equipment
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitTest(BDR_BER, &m_stTester), "InitTest(BER)");
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_RX, pDownlinkMeasParam->nCh), "DUT_SetCH(RX)");
    //Set packet type
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_RX, pDownlinkMeasParam->ePacketType), "DUT_SetPacketType");
    //Set address
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetupAddr("00:00:88:C0:FF:EE"), "DUT_SetupAddr");

    //Set packet length
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPattern(BT_RX, BDR_RX_RECV), "DUT_SetPattern");
    //Rx ON
    CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(true)");
    Sleep(200);
    CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "SetGen(On)");

    int error_bits = 0;
    int total_bits = 0;
    int error_pkts = 0;
    int total_pkts = 0;
    int rssi       = INVALID_NEGATIVE_INTEGER_VALUE;

    CSPTimer timer;
    do 
    {
        if (SP_OK != m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi))
        {
			// RX OFF
            CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, FALSE), "DUT_RFOn(false)");
			Sleep(100);
            CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(true)");
			Sleep(200);
			if (SP_OK != m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi))
			{
				m_pImpBase->_UiSendMsg("DUT_GetRxData", LEVEL_ITEM, 1, 0, 1);
				m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR DUT_GetRxData failed");
                m_pRfTesterBT->SetGen(MM_MODULATION, FALSE);
				m_pBTApi->DUT_RFOn(BT_RX, false);
				return SP_E_WCN_BT_RX_ERROR;
			}
        }
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR PER error_bits=%d, total_bits=%d, rssi = %d", error_bits, total_bits, rssi);
        /* if (total_bits >= TOTAL_PAYLOAD_BITS*10)
        {
            bOperRes = SP_OK;
            break;
        }

        if (total_bits >= pDownlinkMeasParam->nTotalPackets)
        {
            double dValue = 1.0 * error_bits / total_bits;
            if (dValue <= 7*1e-5)
            {
                bOperRes = SP_OK;
                break;
            }
        }*/
		if (total_bits >= pDownlinkMeasParam->nTotalPackets)
		{
			break;
		}
        Sleep(300);
    } while (!timer.IsTimeOut(20000));

	if(total_bits < pDownlinkMeasParam->nTotalPackets)
	{
		pRxReult->BER.dAvgValue = 100.0;
	}
	else
	{
		pRxReult->BER.dAvgValue = 1.0 * error_bits / total_bits * 100.0;
	}

	pRxReult->Rssi.dAvgValue = (double)rssi;
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR PER = %d/%d (%.2f) RSSI = %.2f", error_bits, total_bits, pRxReult->BER.dAvgValue, pRxReult->Rssi.dAvgValue);
    // RX OFF
    CHKRESULT_WITH_NOTIFY_BT_RX_OFF(m_pBTApi->DUT_RFOn(BT_RX, FALSE), "DUT_RFOn(OFF)");
    // Turn off VSG
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE), "SetGen(OFF)");

	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth EDR PER measurement end!");

	return SP_OK;
}