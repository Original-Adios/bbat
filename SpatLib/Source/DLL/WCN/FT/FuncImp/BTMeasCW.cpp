#include "StdAfx.h"
#include "BTMeasCW.h"

//////////////////////////////////////////////////////////////////////////
//
CBTMeasCW::CBTMeasCW(CImpBase* pImpBase, ICBTApi* pBtApi)
    : CBTMeasBase(pImpBase, pBtApi)
{

}

CBTMeasCW::~CBTMeasCW(void)
{

}

BOOL CBTMeasCW::MeasureUplink(BTMeasParamChan* pUplinkMeasParam, SPBT_RESULT_T* pTestResult)
{
    if (NULL == pUplinkMeasParam)
    {
        return SP_E_SPAT_INVALID_PARAMETER;
    }

    if (!IS_BIT_SET(pUplinkMeasParam->dwMask, BT_CW))
    {
        return SP_OK;
    }

    m_pImpBase->SetRepairBand($REPAIR_BAND_B_EDR);
    m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);


    ConfigTesterParam(pUplinkMeasParam);

    //BT test process
    //set channel
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_TX, pUplinkMeasParam->nCh), "DUT_SetCH");
    //Set packet type
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_TX, (int)pUplinkMeasParam->ePacketType), "DUT_SetPacketType");
    //Set packet length
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketLength(pUplinkMeasParam->nPacketLen), "DUT_SetPacketLength");
    //BDR_TX_PBRS9 performance perform
    //m_stTester.ePattern = BDR_TX_PBRS9;
    CHKRESULT(TxCWMeas(pUplinkMeasParam->dwMask, &m_stTester, pTestResult));

    return SP_OK;
}

BOOL CBTMeasCW::TestPER(BTMeasParamChan* pDownlinkMeasParam, SPBT_RESULT_T* pRxReult)
{
    UNREFERENCED_PARAMETER(pDownlinkMeasParam);
    UNREFERENCED_PARAMETER(pRxReult);
    return SP_OK;
}