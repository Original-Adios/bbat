#include "StdAfx.h"
#include "BTMeasEDRUDS710.h"

//////////////////////////////////////////////////////////////////////////
//
CBTMeasEDRUDS710::CBTMeasEDRUDS710(CImpBase *pImpBase, ICBTApi *pBtApi) 
: CBTMeasBase(pImpBase, pBtApi)
{

}

CBTMeasEDRUDS710::~CBTMeasEDRUDS710(void)
{
  
}

BOOL CBTMeasEDRUDS710::MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult)
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
   
    
    ConfigTesterParam( pUplinkMeasParam);

    //BT test process
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_TX, pUplinkMeasParam->nCh), "DUT_SetCH");
	//Set logical channel
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->BDDUT_SetTxLogicCh(pUplinkMeasParam->nTxLogicCh), "DUT_SetPacketType(BT_TX)");
    //Set packet type
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_TX, (int)pUplinkMeasParam->ePacketType), "DUT_SetPacketType");
    //Set packet length
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketLength(pUplinkMeasParam->nPacketLen), "DUT_SetPacketLength");
    //BDR_TX_PBRS9 performance perform
    m_stTester.ePattern = BDR_TX_PBRS9;
    CHKRESULT(BDTxSpecialPatternMeas(pUplinkMeasParam->dwMask, &m_stTester, pTestResult));

    return SP_OK;
}

BOOL CBTMeasEDRUDS710::TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult)
{
	if (pDownlinkMeasParam == NULL)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BDR_BER))
	{
		return SP_OK;
	}
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth EDR PER measurement start!");

    if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BDR_BER))
    {
        return SP_OK;
    }

    m_pImpBase->SetRepairBand($REPAIR_BAND_B_EDR);
    

	ConfigTesterParam( pDownlinkMeasParam);

    
	//Set address
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetupAddr("00:00:88:C0:FF:EE"), "DUT_SetupAddr()!");
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


	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->BDDUT_SetPacketRxLength(BDRxLength), "DUT_SetPacketLength()!");

	//Rx ON
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn");
	//BT test process
	//Initial equipment
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitTest(BDR_BER, &m_stTester), "InitTest");

    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "SetGen on"); 

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

			//// RX OFF
			CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, FALSE), "DUT_RFOn");

			Sleep(100);

			CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn");

			if (SP_OK != m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi))
			{
				m_pImpBase->_UiSendMsg("DUT_GetRxData", LEVEL_ITEM, 1, 0, 1);
				m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR DUT_GetRxData failed");
				CHKRESULT(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE));
				CHKRESULT(m_pBTApi->DUT_RFOn(BT_RX, false));
				return SP_E_WCN_BT_RX_ERROR;
			}	
        }
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR PER error_bits=%d, total_bits=%d, rssi = %d", error_bits, total_bits, rssi);
     
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
    //// RX OFF
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, FALSE), "DUT_RFOn");
    // Turn off VSG
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE), "SetGen off");

	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth EDR PER measurement end!");

	return SP_OK;
}