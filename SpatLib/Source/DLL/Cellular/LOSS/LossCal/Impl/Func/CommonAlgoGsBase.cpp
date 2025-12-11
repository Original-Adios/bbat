#include "StdAfx.h"
#include "CommonAlgoGsBase.h"
#include <algorithm>

CCommonAlgoGsBase::CCommonAlgoGsBase(CImpBase *pImp)
{
    m_pApi = nullptr;
    m_pImp = pImp;
    m_nTxAnt = RF_ANT_1st;
    m_nRxAnt = RF_ANT_1st;

    m_nTxParamStart = 0;
    m_nTxParamEnd = 0;
    m_dTxParamSlope = 0.0;
    m_dTxTarget = 0.0;

    m_dRxCellpowerStart = 0.0;
    m_dRxCellpowerEnd = 0.0;
    m_nRxParam = 0;
    m_dRxTarget = 0.0;

    m_dTolerance = 0;
    m_bAdjust = FALSE;

    m_pLoss = NULL;
}

CCommonAlgoGsBase::~CCommonAlgoGsBase(void)
{
}

SPRESULT CCommonAlgoGsBase::RunBandInfo()
{
    if (m_arrBandInfo.size() == 0)
    {
        return SP_E_INVALID_PARAMETER;
    }

    for (int i = 0; i < MAX_RF_ANT; i++)
    {
        //convert band info to point data
        CHKRESULT(GetTestData(i));
        int nRetryCnt = 10;
        //get result of point data
        do
        {
            CHKRESULT(RunTestData(i));
            nRetryCnt--;
        } while ( nRetryCnt > 0 && m_bAdjust && AdjustResult(i));

        CHKRESULT(ShowResult(i));
    }

    GetGsData();

    return SP_OK;
}

class cmp
{
  public:
    bool operator()(const CCommonAlgoGsBase::PointData &a, const CCommonAlgoGsBase::PointData &b)
    {
        return a.dFreq < b.dFreq;
    }
};

SPRESULT CCommonAlgoGsBase::GetTestData(int nIndex)
{
    vector<PointData> *parrPoint = &m_arrPoint[nIndex];

    parrPoint->clear();

    for (uint32 i = 0; i < m_arrBandInfo.size(); i++)
    {
        if (m_arrBandInfo[i].RfAnt == (RF_ANT_E)nIndex)
        {
            PointData data;
            data.nBand = m_arrBandInfo[i].Band;
            data.uArfcn = m_arrBandInfo[i].uArfcn;
            data.dFreq = m_arrBandInfo[i].dFreq;
            data.bTx = m_arrBandInfo[i].bTx;
            data.nAnt = m_arrBandInfo[i].nAnt;
            data.nChannel = m_arrBandInfo[i].nChannel;
            data.PathGroup = m_arrBandInfo[i].PathGroup;
            data.RfAnt = m_arrBandInfo[i].RfAnt;
            data.nVoltage = m_arrBandInfo[i].nVoltage;
            if (data.bTx)
            {
                data.nParam = (m_nTxParamStart + m_nTxParamEnd) / 2;
                data.dPower = m_dTxTarget;
            }
            else
            {
                data.nParam = m_nRxParam;
                data.dPower = (m_dRxCellpowerStart + m_dRxCellpowerEnd) / 2;
            }

            parrPoint->push_back(data);
        }
    }

    //sort
    std::sort(parrPoint->begin(), parrPoint->end(), cmp());

    //remove repeated
    BOOL bOK;
    do
    {
        bOK = TRUE;

        for (std::vector<PointData>::iterator i0 = parrPoint->begin(); i0 != parrPoint->end(); i0++)
        {
            for (std::vector<PointData>::iterator i1 = parrPoint->begin(); i1 != parrPoint->end(); i1++)
            {
                if (i1 == i0)
                {
                    continue;
                }
                if (((*i0).nBand == (*i1).nBand) &&
                    ((*i0).uArfcn == (*i1).uArfcn) &&
                    ((*i0).RfAnt == (*i1).RfAnt) &&
                    ((*i0).bTx == (*i1).bTx))
                {
                    parrPoint->erase(i1);
                    bOK = FALSE;
                    goto Loop;
                }
            }
        }

    Loop:
        Sleep(0);
    } while (!bOK);

    return SP_OK;
}

SPRESULT CCommonAlgoGsBase::RunTestData(int nIndex)
{
    for (uint32 i = 0; i < m_arrPoint[nIndex].size(); i++)
    {
        m_pApi->m_pLossData = &m_arrPoint[nIndex][i];
        CHKRESULT(m_pApi->Run());
    }

    return SP_OK;
}

BOOL CCommonAlgoGsBase::AdjustResult(int nIndex)
{
    BOOL ret = FALSE;

    for (uint32 i = 0; i < m_arrPoint[nIndex].size(); i++)
    {
        PointData *pPointData = &m_arrPoint[nIndex][i];
        if (pPointData->bTx)
        {
            m_pImp->LogFmtStrA(SPLOGLV_INFO, "AdjustResult Band: %s,  Frequency: %f, Physical ant: %d, txWord: %d, Tx: %f", CNrUtility::m_BandInfo[pPointData->nBand].NameA, pPointData->dFreq, pPointData->RfAnt, pPointData->nParam + pPointData->nParamOffset, pPointData->dResult);

            int nParam = pPointData->nParam + pPointData->nParamOffset;
            if (nParam == m_nTxParamStart || nParam == m_nTxParamEnd)
            {
                continue;
            }

            if (!IN_RANGE(m_dTxTarget - m_dTolerance, pPointData->dResult, m_dTxTarget + m_dTolerance))
            {
				m_pImp->LogFmtStrA(SPLOGLV_INFO, "%s,Band %d, Chan %d, MeasPwr %.2f ApcCtrlWord %d", __FUNCTION__, pPointData->nBand, pPointData->uArfcn, pPointData->dResult, nParam);
                nParam = (int)(nParam + (m_dTxTarget - pPointData->dResult) / m_dTxParamSlope);
                nParam = min(nParam, m_nTxParamEnd);
                nParam = max(nParam, m_nTxParamStart);
                pPointData->nParamOffset = nParam - pPointData->nParam;
                ret = TRUE;
            }
        }
        else
        {
            m_pImp->LogFmtStrA(SPLOGLV_INFO, "AdjustResult Band: %s,  Frequency: %f, Physical ant: %d, CellPower: %f, Rx: %f", CNrUtility::m_BandInfo[pPointData->nBand].NameA, pPointData->dFreq, pPointData->RfAnt, pPointData->dPower + pPointData->dPowerOffset, pPointData->dResult);

            double dCellpower = pPointData->dPower + pPointData->dPowerOffset;
            if (dCellpower == m_dRxCellpowerStart || dCellpower == m_dRxCellpowerEnd)
            {
                continue;
            }

            if (!IN_RANGE(m_dRxTarget - m_dTolerance, pPointData->dResult, m_dRxTarget + m_dTolerance))
            {
                dCellpower = dCellpower - (pPointData->dResult - m_dRxTarget);
                dCellpower = max(dCellpower, m_dRxCellpowerEnd);
                dCellpower = min(dCellpower, m_dRxCellpowerStart);
                pPointData->dPowerOffset = dCellpower - pPointData->dPower;
                ret = TRUE;
            }
        }
    }

    return ret;
}

void CCommonAlgoGsBase::GetGsData()
{
    m_arrGsData.clear();
    for (int i = 0; i < MAX_RF_ANT; i++)
    {
        for (uint32 j = 0; j < m_arrPoint[i].size(); j++)
        {
            PointData *pPointData = &m_arrPoint[i][j];

            GsData data;
            memcpy(&data, pPointData, sizeof GsData);
            data.nParam = pPointData->nParam + pPointData->nParamOffset;
            data.dPower = pPointData->dPower + pPointData->dPowerOffset;
            data.nVoltage = pPointData->nVoltage;

            m_arrGsData.push_back(data);
        }
    }
}

SPRESULT CCommonAlgoGsBase::ShowResult( int nIndex )
{
    double dRlst = 0.0;

    for (uint32 i = 0; i < m_arrPoint[nIndex].size(); i++)
    {
        PointData *pPointData = &m_arrPoint[nIndex][i];
        if (pPointData->bTx)
        {
            dRlst = m_dTxTarget - pPointData->dResult;
            m_pImp->_UiSendMsg("Tx GS"
                , LEVEL_ITEM
                , 0 - m_dTolerance
                , dRlst
                , m_dTolerance
                , CNrUtility::m_BandInfo[pPointData->nBand].NameA
                , pPointData->uArfcn
                , "dBm"
                , "Freq = %.2f; RfAnt = %d"
                , pPointData->dFreq
                , pPointData->RfAnt);
        }
        else
        {
            dRlst = pPointData->dResult - m_dRxTarget;
            m_pImp->_UiSendMsg("Rx GS"
                , LEVEL_ITEM
                , 0 - m_dTolerance
                , dRlst
                , m_dTolerance
                , CNrUtility::m_BandInfo[pPointData->nBand].NameA
                , pPointData->uArfcn
                , "dBm"
                , "Freq = %.2f; RfAnt = %d"
                , pPointData->dFreq
                , pPointData->RfAnt);
        }
        if (!IN_RANGE(0 - m_dTolerance, dRlst, m_dTolerance))
        {
            return SP_E_SPAT_LOSS_OUT_OF_RANGE;
        }
    }
    return SP_OK;
}
