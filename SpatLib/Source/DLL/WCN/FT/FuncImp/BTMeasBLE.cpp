#include "StdAfx.h"
#include "BTMeasBLE.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////
//
CBTMeasBLE::CBTMeasBLE(CImpBase *pImpBase, BT_TYPE eType, ICBTApi *pBtApi) : CBTMeasBase(pImpBase, eType, pBtApi)
{
    try{

        m_pstTester = new SPWI_BT_PARAM_TESTER;
        ZeroMemory(m_pstTester, sizeof(SPWI_BT_PARAM_TESTER));
        m_pRfConf = new SPWI_BT_PARAM;
        ZeroMemory(m_pRfConf, sizeof(SPWI_BT_PARAM));
    }
    catch (const std::bad_alloc& /*e*/)
    {
        assert(0);
    }
}

CBTMeasBLE::~CBTMeasBLE(void)
{
    if (NULL != m_pRfConf)
    {
        delete m_pRfConf;
        m_pRfConf = NULL;
    }

    if (NULL != m_pstTester)
    {
        delete m_pstTester;
        m_pstTester = NULL;
    }
}

BOOL CBTMeasBLE::MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult, RFPATH_ENUM eRfPath)
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

    m_pstTester->pstMeasParamChan = pUplinkMeasParam;

    m_pstTester->eSubItem = BT_TX;
    m_pRfConf->nProto = BLE;
    m_pRfConf->nAvgCount = 2;
    m_pRfConf->nTotalPackets = 1000;
    m_pRfConf->bVsgOn = FALSE;
    m_pRfConf->eRfPort = (eRfPath == ANT_SINGLE? RF_ANT_WCN_1st:RF_ANT_WCN_2nd);;

    m_pstTester->pstBTParamTester = m_pRfConf;
    //BT test process
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_TX, pUplinkMeasParam->nCh), "DUT_SetCH()!");
    //Set packet type
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_TX, pUplinkMeasParam->ePacketType), "DUT_SetPacketType()!");
    //Set packet length
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketLength(CwcnUtility::BT_MAX_PKTLEN[BLE][pUplinkMeasParam->ePacketType]), "DUT_SetPacketLength()!");
    //BLE_TX_PBRS9 performance perform
    if (   IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_POWER)
        || IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_ACP)
        )
    {
        dwTestMask = BLE_POWER | BLE_ACP;
        pUplinkMeasParam->ePattern = BLE_TX_PBRS9;
        CHKRESULT(TxSpecialPatternMeas(dwTestMask, m_pstTester, pTestResult));
    }

    if (   IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_MC)
        || IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_CFD)
        )
    {
        dwTestMask = 0x00;
        dwTestMask = BLE_MC | BLE_CFD;
        pUplinkMeasParam->ePattern = BLE_TX_0xF0;

        CHKRESULT(TxSpecialPatternMeas(dwTestMask, m_pstTester, pTestResult));
        pUplinkMeasParam->ePattern = BLE_TX_0xAA;
        CHKRESULT(TxSpecialPatternMeas(dwTestMask, m_pstTester, pTestResult));

        if (fabs(pTestResult->Modulation.f1avg) > 0.0001)
        {
            pTestResult->Modulation.f2f1avg_rate = pTestResult->Modulation.f2avg / pTestResult->Modulation.f1avg;
        }
    }

    return SP_OK;
}

BOOL CBTMeasBLE::MeasureUplink_CW(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult, RFPATH_ENUM /*eRfPath*/)
{
    UNREFERENCED_PARAMETER(pUplinkMeasParam);
    UNREFERENCED_PARAMETER(pTestResult);
	return SP_OK;
}

BOOL CBTMeasBLE::TestPER(BTMeasParamChan *pDownlinkMeasParam, SPWI_VALUES_T *pPER, RFPATH_ENUM eRfPath)
{
    if (pDownlinkMeasParam == NULL)
    {
        return SP_E_SPAT_INVALID_PARAMETER;
    }
	
	if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BDR_BER))
	{
		return SP_OK;
	}
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth BLE PER measurement start!");

    SPRESULT bOperRes = SP_E_WCN_BT_RX_ERROR;

    if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BLE_PER))
    {
        return SP_OK;
    }
    m_pImpBase->SetRepairBand($REPAIR_BAND_B_BLE);


    m_pstTester->eSubItem = BT_RX;
    m_pRfConf->nProto = BLE;
    m_pRfConf->nAvgCount = 2;
    m_pRfConf->nTotalPackets = 1500;
    m_pRfConf->bVsgOn = FALSE;
    m_pRfConf->eRfPort = (eRfPath == ANT_SINGLE? RF_ANT_WCN_1st:RF_ANT_WCN_2nd);

    m_pstTester->pstBTParamTester = m_pRfConf;
    m_pstTester->pstMeasParamChan = pDownlinkMeasParam;

    //BT test process
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_RX, pDownlinkMeasParam->nCh), "DUT_SetCH()!");
    //Initial equipment
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitTest(BDR_BER, m_pstTester), "Instrument::InitTest()!");
    //Rx ON
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, true), "DUT_RFOn(TRUE)!");  
    Sleep(200);
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, TRUE), "Instrument::SetGen()!"); 

    int error_bits = 0;
    int total_bits = 0;
    int error_pkts = 0;
    int total_pkts = 0;
    int rssi       = 0;

    CSPTimer timer;
    do 
    {
        if (SP_OK != m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi))
        {
			m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE DUT_GetRxData failed");
            m_pRfTesterBT->SetGen(MM_MODULATION, FALSE);
            CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, false), "DUT_RFOn(FALSE)!");
			return SP_E_WCN_BT_RX_ERROR;
        }
		m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER error_pkts=%d, total_pkts=%d", error_pkts, total_pkts);
        if (total_pkts >= m_pRfConf->nTotalPackets - 10)
        {
            bOperRes = SP_OK;
            break;
        }

        Sleep(300);

    } while (!timer.IsTimeOut(10000));

    pPER->dAvgValue = 1.0 * error_pkts / total_pkts * 100.0;
    m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER = %d/%d (%.2f)", error_pkts, total_pkts, pPER->dAvgValue);
    // RX OFF
 //   CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_RX, FALSE), "DUT_RFOn()!");
    // Turn off VSG
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetGen(MM_MODULATION, FALSE), "Instrument::SetGen()!");
	m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth BLE PER measurement end!");

    return bOperRes;
}