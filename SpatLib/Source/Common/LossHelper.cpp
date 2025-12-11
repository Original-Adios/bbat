#include "StdAfx.h"
#include "LossHelper.h"
#include "SpatLibExport.h"

CLossHelper::CLossHelper(void)
{
    m_dFreq = 0;
    m_Ant = RF_ANT_1st;
    m_dTolernece = 0.1;
	m_Loss = 0;
    m_arrAnt.clear();
}


CLossHelper::~CLossHelper(void)
{
}

void CLossHelper::ClearLoss()
{
    m_pSpatbase->LogFmtStrA(SPLOGLV_INFO ,"%s", __FUNCTION__);
    m_arrLoss.clear();
}

RF_CABLE_LOSS_POINT_EX* CLossHelper::FindLoss(int nBand, uint32 uArfcn, double dFreq, RF_IO_E trx)
{
    m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "%s: %f", __FUNCTION__, dFreq);

    for (uint32 i = 0; i < m_arrLoss.size(); i++)
    {
        if (m_arrLoss[i].nBand == nBand)
        {
            if (m_arrLoss[i].dFreq[trx] == dFreq &&
                m_arrLoss[i].uArfcn[trx] == uArfcn)
            {
                return &m_arrLoss[i];
            }
            if (m_arrLoss[i].dFreq[trx] == 0.0 &&
                m_arrLoss[i].uArfcn[trx] == 0)
            {
                m_arrLoss[i].dFreq[trx] = dFreq;
                m_arrLoss[i].uArfcn[trx] = uArfcn;
                return &m_arrLoss[i];
            }
        }
    }

    RF_CABLE_LOSS_POINT_EX Loss;
    memset(&Loss, 0, sizeof Loss);
    Loss.dFreq[trx] = dFreq;
    Loss.nBand = nBand;
    Loss.uArfcn[trx] = uArfcn;

    m_arrLoss.push_back(Loss);

    return &m_arrLoss[m_arrLoss.size() - 1];
}


SPRESULT CLossHelper::ApplyLoss(RF_IO_E trx)
{
    m_pSpatbase->LogFmtStrA(SPLOGLV_INFO ,"%s", __FUNCTION__);

    //RF_CABLE_LOSS_EX线损结构体太大，会有可能导致栈溢出，不能使用局部对象
    std::unique_ptr<RF_CABLE_LOSS_EX> Loss = std::make_unique<RF_CABLE_LOSS_EX>();

    RF_CABLE_LOSS_UNIT_EX* pUint = NULL;

    switch(m_NetMode)
    {
    case NM_GSM:
		pUint = &Loss.get()->gsmLoss;
        break;
	case NM_WCDMA:
		pUint = &Loss.get()->wcdmaLoss;
		break;
    case NM_LTE:
        pUint = &Loss.get()->lteLoss;
        break;
    case NM_NR:
        pUint = &Loss.get()->nrLoss;
        break;
	case NM_GPS:
		pUint = &Loss.get()->gpsLoss;
		break;
	case NM_WIFI:
		pUint = &Loss.get()->wlanLoss;
		break;
	case NM_BT:
		pUint = &Loss.get()->btLoss;
		break;
    default:
        break;
    }

    RF_CABLE_LOSS_POINT_EX* pLoss = FindLoss(m_nBand, m_uArfcn, m_dFreq, trx);
    double dLoss = pLoss->dLoss[m_Ant][trx];

    pUint->nCount = 1;
    pUint->arrPoint[0].nBand = m_nBand;
    for (int i = 0; i < MAX_RF_IO; i++)
    {
        pUint->arrPoint[0].uArfcn[i] = m_uArfcn;
        pUint->arrPoint[0].dFreq[i] = m_dFreq;
        for (int j = 0; j < MAX_RF_ANT; j++)
        {
            pUint->arrPoint[0].dLoss[j][i] = dLoss;
            pUint->arrPoint[0].dLoss[j][i] = dLoss;
        }
    }

	m_Loss = dLoss;
    CHKRESULT(m_pSpatbase->m_pRFTester->SetProperty(DP_CABLE_LOSS_EX, 0, (LPCVOID)Loss.get()));

    return SP_OK;
}


SPRESULT CLossHelper::SetLoss(int nBand, uint32 uArfcn, double dFreq, RF_IO_E trx, double dLoss, RF_ANT_E ANT)
{
    m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "%s: %f, %f, %d", __FUNCTION__, dFreq, dLoss, ANT);

    m_dFreq = dFreq;
    m_Ant = ANT;
    m_nBand = nBand;
    m_uArfcn = uArfcn;
    RF_CABLE_LOSS_POINT_EX* pLoss = FindLoss(nBand, uArfcn, dFreq, trx);
    pLoss->dLoss[ANT][trx] = dLoss;


    ApplyLoss(trx);

    return SP_OK;
}

SPRESULT CLossHelper::UpdateLoss(LOSS_MODE_E Mode)
{
    m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "%s: %d", __FUNCTION__, Mode);

    RF_CABLE_LOSS_COMB_EX lossCom;

    lossCom.nModeID = Mode;
    lossCom.tLossUnit.LossType = BAND_TYPE;
    lossCom.tLossUnit.nCount = m_arrLoss.size();

    int nIndex = 0;

    if (m_arrLoss.size() > MAX_FREQ_LOSS_NUM)
    {
        m_pSpatbase->LogFmtStrA(SPLOGLV_ERROR, "SIZE OVER MAX");
        return SP_E_FAIL;
    }

    for (uint32 i = 0; i < m_arrLoss.size(); i++)
    {
        lossCom.tLossUnit.arrPoint[nIndex].nBand = m_arrLoss[i].nBand;
        lossCom.tLossUnit.arrPoint[nIndex].dFreq[RF_IO_TX] = m_arrLoss[i].dFreq[RF_IO_TX];
        lossCom.tLossUnit.arrPoint[nIndex].dFreq[RF_IO_RX] = m_arrLoss[i].dFreq[RF_IO_RX];
        lossCom.tLossUnit.arrPoint[nIndex].uArfcn[RF_IO_TX] = m_arrLoss[i].uArfcn[RF_IO_TX];
        lossCom.tLossUnit.arrPoint[nIndex].uArfcn[RF_IO_RX] = m_arrLoss[i].uArfcn[RF_IO_RX];
        for (int j = 0; j < MAX_RF_ANT; j++)
        {
            lossCom.tLossUnit.arrPoint[nIndex].dLoss[j][RF_IO_TX] = m_arrLoss[i].dLoss[j][RF_IO_TX];
            lossCom.tLossUnit.arrPoint[nIndex].dLoss[j][RF_IO_RX] = m_arrLoss[i].dLoss[j][RF_IO_RX];

        }
        nIndex++;
    }

    ICallback* pCallBack = m_pSpatbase->GetCallback();
    pCallBack->UiMsgUpdateLoss(&lossCom);
    return SP_OK;
}

SPRESULT CLossHelper::AdjustLoss(double dTarget, double dValue, RF_IO_E trx, BOOL& bNeedRetry, double dTolernece, double m_dLossUpperLimit/* = 200*/)
{
    m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "%s: %f, %f, %d ", __FUNCTION__, dTarget, dValue, trx);

    double dDiff = dTarget - dValue;

    RF_CABLE_LOSS_POINT_EX* pLoss = FindLoss(m_nBand, m_uArfcn, m_dFreq, trx);
    double dLoss = pLoss->dLoss[m_Ant][trx];

    dLoss += fabs(dTarget - dValue) < dTolernece ? (dDiff * 1) : (dDiff * 0.8);

    if (dLoss < 0)
    {
        bNeedRetry = FALSE;
        return SP_OK;
    }
    if (dLoss > m_dLossUpperLimit)
    {
        m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "Loss is out of range! Loss = [0,%f,%f] fail.", dLoss, m_dLossUpperLimit);
        return -1;
    }
    SetLoss(m_nBand, m_uArfcn, m_dFreq, trx, dLoss, m_Ant);
    bNeedRetry = fabs(dTarget - dValue) < dTolernece ? FALSE : TRUE;

    return SP_OK;
}
