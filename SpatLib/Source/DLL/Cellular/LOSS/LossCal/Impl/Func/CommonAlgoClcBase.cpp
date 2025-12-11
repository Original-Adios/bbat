#include "StdAfx.h"
#include "CommonAlgoClcBase.h"

CCommonAlgoClcBase::CCommonAlgoClcBase(CImpBase *pImp)
{
    m_pApi = nullptr;
    m_pImp = pImp;
    m_pLoss = nullptr;
    m_dPreLoss = pImp->m_dPreLoss;
    m_dSpecLower = pImp->m_dLossValLower/*pImp->m_dPreLoss - pImp->m_dLossCheckSpec*/;
    m_dSpecUpper = pImp->m_dLossValUpper/*pImp->m_dPreLoss + pImp->m_dLossCheckSpec*/;
    m_bLossCheck = pImp->m_bLossCheck;

    m_nMaxRetryTime = 20;
    m_nRetryTime = 0;
}

CCommonAlgoClcBase::~CCommonAlgoClcBase(void)
{
}

SPRESULT CCommonAlgoClcBase::CustomizeLossData()
{
    return SP_OK;
}

SPRESULT CCommonAlgoClcBase::ClcLoss()
{
    CHKRESULT(GetGsData());

    BOOL bClcRlst = TRUE;

    for (int i = 0; i < MAX_RF_ANT; i++)
    {
        for (uint32 j = 0; j < m_arrPoint[i].size(); j++)
        {
			BOOL bPntState = TRUE;

            CHKRESULT(SetLoss(m_pImp->m_dPreLoss));
            m_pApi->m_pLossData = &m_arrPoint[i][j];

            m_nRetryTime = 0;

            do
            {
                CHKRESULT(m_pApi->Run());
                m_nRetryTime++;
                bPntState = CheckLoss(m_pApi->m_pLossData);
            } while (!bPntState && m_nRetryTime < m_nMaxRetryTime);
			
			bClcRlst &= bPntState;
        }
    }

    CHKRESULT(CustomizeLossData());
    CHKRESULT(GenLossTable());

    return bClcRlst? SP_OK: SP_E_FAIL;
}

SPRESULT CCommonAlgoClcBase::GetGsData()
{
    for (int i = 0; i < MAX_RF_ANT; i++)
    {
        m_arrPoint[i].clear();
    }

    for (uint32 i = 0; i < m_arrGsData.size(); i++)
    {
        GsData *pGsData = &m_arrGsData[i];

        RefData *prefData = new RefData();
        prefData->dLoss = m_pImp->m_dPreLoss;
        prefData->dGsData = pGsData->dResult;

        PointData data;
        data.nBand = pGsData->nBand;
        data.uArfcn = pGsData->uArfcn;
        data.dFreq = pGsData->dFreq;
        data.bTx = pGsData->bTx;
        data.nParam = pGsData->nParam;
        data.dPower = pGsData->dPower;
        data.RfAnt = pGsData->RfAnt;
        data.nAnt = pGsData->nAnt;
        data.nChannel = pGsData->nChannel;
        data.PathGroup = pGsData->PathGroup;
        data.pTag = prefData;
        data.nVoltage = pGsData->nVoltage;

        m_arrPoint[pGsData->RfAnt].push_back(data);
    }

    return SP_OK;
}

BOOL CCommonAlgoClcBase::CheckLoss(PointData *pLossData)
{
    if (pLossData->bTx)
    {
        return CheckLossTx(pLossData);
    }
    else
    {
        return CheckLossRx(pLossData);
    }
}

BOOL CCommonAlgoClcBase::CheckLossTx(PointData *pLossData)
{
    RefData *pRefData = (RefData *)(pLossData->pTag);

    double dTarget = pRefData->dGsData;
    double dSpec = 0.15;
    double dMaxTarget = dTarget + dSpec;
    double dMinTarget = dTarget - dSpec;

    m_pImp->LogFmtStrA(SPLOGLV_INFO, "m_nRetryTime:%d, Power:%.2f, Target:%.2f", m_nRetryTime, pLossData->dResult, dTarget);

    if (IN_RANGE(dMinTarget, pLossData->dResult, dMaxTarget))
    {
        m_pImp->_UiSendMsg("TX Power",
            LEVEL_ITEM,
            dMinTarget,
            pLossData->dResult,
            dMaxTarget,
            GetBandName(pLossData->nBand),
            pLossData->uArfcn,
            "dBm",
            "RfAnt = %d; Freq = %.1lf",
            pLossData->RfAnt,
            pLossData->dFreq);

        pRefData->dLoss += (pRefData->dGsData - pLossData->dResult);

        if (!IN_RANGE(m_dSpecLower, pRefData->dLoss, m_dSpecUpper))
        {
            if (m_nRetryTime == m_nMaxRetryTime)
            {
                m_pImp->_UiSendMsg("TX Loss m_nRetryTime == m_nMaxRetryTime",
                    LEVEL_ITEM,
                    m_dSpecLower,
                    pRefData->dLoss,
                    m_dSpecUpper,
                    GetBandName(pLossData->nBand),
                    pLossData->uArfcn,
                    "dBm",
                    "RfAnt = %d; Freq = %.1lf",
                    pLossData->RfAnt,
                    pLossData->dFreq);
            }
            return FALSE;
        }
        else
        {
            m_pImp->_UiSendMsg("TX Loss",
                LEVEL_ITEM,
                m_dSpecLower,
                pRefData->dLoss,
                m_dSpecUpper,
                GetBandName(pLossData->nBand),
                pLossData->uArfcn,
                "dBm",
                "RfAnt = %d; Freq = %.1lf",
                pLossData->RfAnt,
                pLossData->dFreq);
        }
    }
    else
    {
        if (m_nRetryTime == m_nMaxRetryTime)
        {
            m_pImp->_UiSendMsg("TX Power",
                                LEVEL_ITEM,
                                dMinTarget,
                                pLossData->dResult,
                                dMaxTarget,
                                GetBandName(pLossData->nBand),
                                pLossData->uArfcn,
                                "dBm",
                                "RfAnt = %d; Freq = %.1lf",
                                pLossData->RfAnt,
                                pLossData->dFreq);

            m_pImp->_UiSendMsg("TX Loss m_nRetryTime == m_nMaxRetryTime",
                                LEVEL_ITEM,
                                m_dSpecLower,
                                pRefData->dLoss,
                                m_dSpecUpper,
                                GetBandName(pLossData->nBand),
                                pLossData->uArfcn,
                                "dBm",
                                "RfAnt = %d; Freq = %.1lf",
                                pLossData->RfAnt,
                                pLossData->dFreq);

            return FALSE;
        }
        pRefData->dLoss += (pRefData->dGsData - pLossData->dResult);
        SetLoss(pRefData->dLoss);
        return FALSE;
    }
    return TRUE;
}

BOOL CCommonAlgoClcBase::CheckLossRx(PointData *pLossData)
{
    RefData *pRefData = (RefData *)(pLossData->pTag);

    double dTarget = pRefData->dGsData;
    double dSpec = 0.15;
    double dMaxTarget = dTarget + dSpec;
    double dMinTarget = dTarget - dSpec;

	m_pImp->LogFmtStrA(SPLOGLV_INFO, "m_nRetryTime:%d, Rssi:%.2f, Target:%.2f", m_nRetryTime, pLossData->dResult, dTarget);

    if (IN_RANGE(dMinTarget, pLossData->dResult, dMaxTarget))
    {
        m_pImp->_UiSendMsg("RX RSSI",
            LEVEL_ITEM,
            dMinTarget,
            pLossData->dResult,
            dMaxTarget,
            GetBandName(pLossData->nBand),
            pLossData->uArfcn,
            "dBm",
            "RfAnt = %d; Freq = %.1lf",
            pLossData->RfAnt,
            pLossData->dFreq);

        pRefData->dLoss += (dTarget - pLossData->dResult);

        if (!IN_RANGE(m_dSpecLower, pRefData->dLoss, m_dSpecUpper))
        {
            if (m_nRetryTime == m_nMaxRetryTime)
            {
                m_pImp->_UiSendMsg("RX Loss m_nRetryTime == m_nMaxRetryTime",
                    LEVEL_ITEM,
                    m_dSpecLower,
                    pRefData->dLoss,
                    m_dSpecUpper,
                    GetBandName(pLossData->nBand),
                    pLossData->uArfcn,
                    "dBm",
                    "RfAnt = %d; Freq = %.1lf",
                    pLossData->RfAnt,
                    pLossData->dFreq);
            }
            return FALSE;
        }
        else
        {
            m_pImp->_UiSendMsg("RX Loss",
                LEVEL_ITEM,
                m_dSpecLower,
                pRefData->dLoss,
                m_dSpecUpper,
                GetBandName(pLossData->nBand),
                pLossData->uArfcn,
                "dBm",
                "RfAnt = %d; Freq = %.1lf",
                pLossData->RfAnt,
                pLossData->dFreq);
        }
    }
    else
    {
        if (m_nRetryTime == m_nMaxRetryTime)
        {
            m_pImp->_UiSendMsg("RX RSSI",
                                LEVEL_ITEM,
                                dMinTarget,
                                pLossData->dResult,
                                dMaxTarget,
                                GetBandName(pLossData->nBand),
                                pLossData->uArfcn,
                                "dBm",
                                "RfAnt = %d; Freq = %.1lf",
                                pLossData->RfAnt,
                                pLossData->dFreq);

            m_pImp->_UiSendMsg("RX Loss m_nRetryTime == m_nMaxRetryTime",
                                LEVEL_ITEM,
                                m_dSpecLower,
                                pRefData->dLoss,
                                m_dSpecUpper,
                                GetBandName(pLossData->nBand),
                                pLossData->uArfcn,
                                "dBm",
                                "RfAnt = %d; Freq = %.1lf",
                                pLossData->RfAnt,
                                pLossData->dFreq);

            return FALSE;
        }
        pRefData->dLoss += ( dTarget - pLossData->dResult );
        SetLoss(pRefData->dLoss);
        return FALSE;
    }
    return TRUE;
}

class vector_finder
{
public:
    vector_finder(const int nBand, const uint32 uArfcn, const double dFreq, RF_IO_E TRX) :
        m_nBand(nBand),
        m_trx(TRX),
        m_dFreq(dFreq),
        m_uArfcn(uArfcn) {}
    bool operator ()(const std::vector<RF_CABLE_LOSS_POINT_EX>::value_type& value)
    {

        return ((value.nBand == m_nBand && value.dFreq[m_trx] == m_dFreq && value.uArfcn[m_trx] == m_uArfcn)
            ||
            (value.nBand == m_nBand && value.dFreq[m_trx] == 0.0 && value.uArfcn[m_trx] == 0)
            );
    }
private:
    double m_dFreq;
    int m_nBand;
    uint32 m_uArfcn;
    RF_IO_E m_trx;
};

class cmp    
{    
public:
    bool operator () (const RF_CABLE_LOSS_POINT& a,const RF_CABLE_LOSS_POINT& b)    
    {    
        return a.dFreq < b.dFreq;    
    }  
}; 

BOOL CCommonAlgoClcBase::GenLossTable()
{
    m_arrLoss.clear();

    for (int i = 0; i < MAX_RF_ANT; i++)
    {
        if (m_arrPoint[i].size() == 0)
        {
            continue;
        }

        for (uint32 j = 0; j < m_arrPoint[i].size(); j++)
        {
            PointData *pPointData = &m_arrPoint[i][j];
            RefData *pRefData = (RefData *)(pPointData->pTag);
            IsValidIndex(pPointData->RfAnt)

            RF_IO_E TRX = pPointData->bTx ? RF_IO_TX : RF_IO_RX;

            std::vector<RF_CABLE_LOSS_POINT_EX>::iterator itor;
            itor = std::find_if(m_arrLoss.begin(), m_arrLoss.end(), 
                vector_finder(pPointData->nBand, pPointData->uArfcn, pPointData->dFreq, TRX));
            if (itor == m_arrLoss.end())
            {
                RF_CABLE_LOSS_POINT_EX item;
                memset(&item, 0, sizeof(item));
                item.dFreq[TRX] = pPointData->dFreq;
                item.uArfcn[TRX] = pPointData->uArfcn;
                item.nBand = pPointData->nBand;
                item.dLoss[pPointData->RfAnt][TRX] = pRefData->dLoss;
                //item.dLoss[pPointData->RfAnt][RF_IO_RX] = pRefData->dLoss;
                m_arrLoss.push_back(item);
            }
            else
            {
                (*itor).dFreq[TRX] = pPointData->dFreq;
                (*itor).uArfcn[TRX] = pPointData->uArfcn;
                (*itor).dLoss[pPointData->RfAnt][TRX] = pRefData->dLoss;
                //(*itor).dLoss[pPointData->RfAnt][RF_IO_RX] = pRefData->dLoss;
            }
        }
    }

    //    std::sort(m_arrLoss.begin(), m_arrLoss.end(), cmp());

    memset(m_pLoss, 0, sizeof(*m_pLoss));

    m_pLoss->nCount = m_arrLoss.size();
    for (uint32 i = 0; i < m_arrLoss.size(); i++)
    {
        m_pLoss->arrPoint[i].nBand = m_arrLoss[i].nBand;
        m_pLoss->arrPoint[i].dFreq[RF_IO_TX] = m_arrLoss[i].dFreq[RF_IO_TX];
        m_pLoss->arrPoint[i].dFreq[RF_IO_RX] = m_arrLoss[i].dFreq[RF_IO_RX];
        m_pLoss->arrPoint[i].uArfcn[RF_IO_TX] = m_arrLoss[i].uArfcn[RF_IO_TX];
        m_pLoss->arrPoint[i].uArfcn[RF_IO_RX] = m_arrLoss[i].uArfcn[RF_IO_RX];
        for (int j = 0; j < MAX_RF_ANT; j++)
        {
            //nr srs用rx ant做tx，所以没有tx loss的时候需要用rx loss填补
            if (m_arrLoss[i].dLoss[j][RF_IO_TX] == 0)
            {
                m_pLoss->arrPoint[i].dLoss[j][RF_IO_TX] = m_arrLoss[i].dLoss[j][RF_IO_RX];
            }
            else
            {
                m_pLoss->arrPoint[i].dLoss[j][RF_IO_TX] = m_arrLoss[i].dLoss[j][RF_IO_TX];
            }            
            m_pLoss->arrPoint[i].dLoss[j][RF_IO_RX] = m_arrLoss[i].dLoss[j][RF_IO_RX];
        }
    }

    return SP_OK;
}

SPRESULT CCommonAlgoClcBase::SetLoss( double dLoss )
{
    RF_CABLE_LOSS Loss;

    Loss.nrLoss.nCount = 1;
    Loss.nrLoss.arrPoint[0].dFreq = 1800;
    for (int i = 0; i < MAX_RF_ANT; i++)
    {
        Loss.nrLoss.arrPoint[0].dLoss[i][RF_IO_TX] = dLoss;
        Loss.nrLoss.arrPoint[0].dLoss[i][RF_IO_RX] = dLoss;
    }

    U_RF_PARAM param;
    param.pLoss = &Loss;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter( PT_CABLE_LOSS, param ));

    return SP_OK;
}
