#include "StdAfx.h"
#include "CommonApiBase.h"

CCommonApiBase::CCommonApiBase(void)
{
    m_pLossData = NULL;

    m_hDUT = NULL;
    m_pRFTester = NULL;
}

CCommonApiBase::~CCommonApiBase(void)
{
}

SPRESULT CCommonApiBase::Run()
{
    CHKRESULT(m_pRFTester->SetNetMode(NM_NR));
    CHKRESULT(m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_COMMON, NULL));

    if (m_pLossData->bTx)
    {
        CHKRESULT(RunTxRfTester());
        CHKRESULT(RunTxPhoneCommand(TRUE));
        CHKRESULT(FetchTxRfTester());
        CHKRESULT(RunTxPhoneCommand(FALSE));
    }
    else
    {
        CHKRESULT(RunRxRfTester());
        CHKRESULT(RunRxPhoneCommand());
    }

    return SP_OK;
}

SPRESULT CCommonApiBase::RunRxRfTester()
{
    RF_NR_RXS_REQ_T req;
    req.dFreq = GetDlFreq(m_pLossData->nBand, m_pLossData->uArfcn);
    req.Ant = m_pLossData->RfAnt;

    req.dCellpower = m_pLossData->dPower + m_pLossData->dPowerOffset;

    CHKRESULT(m_pRFTester->InitTest(TI_RSCP, &req));

    return SP_OK;
}

SPRESULT CCommonApiBase::RunTxRfTester()
{
    RF_NR_TXS_REQ_T req;

    req.dFreq = GetUlFreq(m_pLossData->nBand, m_pLossData->uArfcn);
    req.Ant = m_pLossData->RfAnt;

    req.dEnp = m_pLossData->dPower;

    CHKRESULT(m_pRFTester->InitTest(TI_PWR2, &req));

    return SP_OK;
}

SPRESULT CCommonApiBase::FetchTxRfTester()
{
    PWR_RLT rlt;
    CHKRESULT(m_pRFTester->FetchResult(TI_PWR2, &rlt));
    m_pLossData->dResult = rlt.dAvgPwr;

    return SP_OK;
}
