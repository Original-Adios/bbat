#include "StdAfx.h"
#include "AlgoGsRx.h"
#include "LteUtility.h"

CAlgoGsRx::CAlgoGsRx( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
, m_uMaxRetryTime(30)
, m_nMinIndex(14)
, m_nMaxIndex(108)
{
	m_pNal = NULL;
	m_pFileConfig = NULL;
	m_Ant = RF_ANT_INVALID;
	m_Ca = LTE_CA_PCC;
	m_pApi = NULL;
	m_parrConfig = NULL;
	m_parrResult = NULL;
	m_uRetryTime = 0;
}

CAlgoGsRx::~CAlgoGsRx(void)
{


}

void CAlgoGsRx::SetAntCa( RF_ANT_E Ant, LTE_CA_E Ca )
{
    m_Ant = Ant;
    m_Ca = Ca;

    m_parrConfig = &m_pNal->m_arrConfig[Ant];
    m_parrResult = &m_pNal->m_arrResult[Ant];
}

SPRESULT CAlgoGsRx::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pNal = (CNalGs_UIS8910*)GetFunc(FUNC_INAL);
    m_pFileConfig = (CFileConfig_UIS8910*)GetFunc(FUNC_FILE_CONFIG);
    m_pApi = (IApiRx*)GetFunc(API_AGC);

    return SP_OK;
}

SPRESULT CAlgoGsRx::Init()
{
    if (m_parrConfig->size() == 0)
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant);
    LogFmtStrA(SPLOGLV_INFO, "%s: ANT = %s; CA = %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca]);

    m_pApi->m_UeType = m_pFileConfig->m_Uetype;

    CHKRESULT(InitData());

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
    return SP_OK;
}

SPRESULT CAlgoGsRx::Run()
{
    if (m_arrBandData.size() == 0)
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant);
    LogFmtStrA(SPLOGLV_INFO, "%s, Ant: %s, Ca: %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca]);

    SPRESULT Result = SP_OK;
    BOOL ret = FALSE;

    m_pApi->m_Afc = 0;
    m_pApi->m_Ant = m_Ant; 
    m_pApi->m_Ca = m_Ca; 
    m_pApi->m_parrBandData = &m_arrBandData;

    m_pApi->m_pCurrentCaller = NULL;
    m_pApi->DealwithData = NULL;

    m_pApi->m_pIApi->ClearData();
    m_uRetryTime = 0;
    do 
    {
        CHKRESULT(m_pApi->m_pIApi->Run());
        CHKRESULT(AdjustCellPower(ret));
		m_uRetryTime++;
    } while (m_uRetryTime < m_uMaxRetryTime && !ret);

    if (!ret)
    {
        Result = 1; 
    }
    CHKRESULT(Result);

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CAlgoGsRx::InitData()
{
    m_arrBandData.resize(0);
    uint32 iArrConfigSize = m_parrConfig->size();
    for (uint32 i = 0; i < iArrConfigSize; i++)
    {
        Config* pConfig = &(*m_parrConfig)[i];

        if (pConfig->byPath == RX)
        {
            BandData BandData;

            BandData.nBand = pConfig->byBand;
            BandData.arrChannel.resize(1);

            ChannelData* pChannelData = &BandData.arrChannel[0];
            pChannelData->nIndicator = CLteUtility::m_BandInfo[pConfig->byBand].nIndicator;
            pChannelData->usArfcn = pConfig->usChannel;
            pChannelData->arrPoint.resize(1);

            PointData* pPointData = &pChannelData->arrPoint[0];

            pPointData->usGainIndex = m_pFileConfig->m_byRxIndex;
            pPointData->dPower = -50;

            m_arrBandData.push_back(BandData);
        }
    }

    return SP_OK;
}

SPRESULT CAlgoGsRx::AdjustCellPower(BOOL &ret)
{
    LogFmtStrA(SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, m_uRetryTime);
    IsValidIndex(m_Ant);
    if(m_uRetryTime < 5) //用于等待UE/仪表工作状态稳定。
	{
		ret = FALSE;
		return SP_OK;
	}

    double dTarget = m_pFileConfig->m_dRxTarget;
    double dMaxTarget = dTarget + m_pFileConfig->m_dRxTolerance;
    double dMinTarget = dTarget - m_pFileConfig->m_dRxTolerance;
    double dMaxCellPower = m_pFileConfig->m_dMaxCellPower;
    double dMinCellPower = m_pFileConfig->m_dMinCellPower;

    ret = TRUE;

    int nIndex= 0;
    uint32 iArrConfigSize = m_parrConfig->size();
    for (uint32 i = 0; i < iArrConfigSize; i++)
    {
        Config* pConfig = &(*m_parrConfig)[i];
        Result* pResult = &(*m_parrResult)[i];

        if (pConfig->byPath != RX)
        {
            continue;
        }

        if (pResult->bDone)
        {
            nIndex++;
            continue;
        }

        ChannelData* pChannelData = &m_arrBandData[nIndex++].arrChannel[0];
        PointData* pPointData = &pChannelData->arrPoint[0];
        double dRssi = pPointData->dRssi;
        double dPower = pPointData->dPower;

        if (IN_RANGE(dMinTarget, dRssi, dMaxTarget))
        {
            UiSendMsg("LTE RX Loss"
                , LEVEL_ITEM
                , dMinTarget
                , dRssi
                , dMaxTarget
                , NULL
                , pChannelData->usArfcn
                , "dBm"
                , "ANT = %s; CA = %s; Freq = %.1lf; CellPower = %.1lf; Index = %d"
                , g_lpAntNameA[m_Ant]
                , g_lpCaNameA[m_Ca]
                , CLteUtility::GetFreq(pChannelData->usArfcn)
                , dPower
                , pPointData->usGainIndex
                );

            pResult->RxResult.dRssi = dRssi;
            pResult->RxResult.dCellPower = dPower;
            pResult->RxResult.byIndex = (uint8)pPointData->usGainIndex;
            pResult->bDone = TRUE;
        }
        else if (dRssi > dMaxTarget)
        {
            if (dPower == dMinCellPower)
            {
                if (pPointData->usGainIndex == m_nMinIndex)
                {
                    UiSendMsg("LTE RX Loss"
                        , LEVEL_ITEM
                        , dMinTarget
                        , dRssi
                        , dMaxTarget
                        , NULL
                        , pChannelData->usArfcn
                        , "dBm"
                        , "ANT = %s; CA = %s; Freq = %.1lf; CellPower = %.1lf;"
                        , g_lpAntNameA[m_Ant]
                        , g_lpCaNameA[m_Ca]
                        , CLteUtility::GetFreq(pChannelData->usArfcn)
                        , dPower);

                    return SP_E_SPAT_LOSS_LTE_RX_GAIN_TOO_HIGH;
                } 
                else
                {
                    pPointData->usGainIndex -= 10;
                    pPointData->usGainIndex = (uint16)min(pPointData->usGainIndex, m_nMaxIndex);
                    pPointData->usGainIndex = (uint16)max(pPointData->usGainIndex, m_nMinIndex);
                    pPointData->dPower = (dMinCellPower + dMaxCellPower) / 2;

                    ret = FALSE;
                }
            }
            else
            {
                dPower = dPower - (dRssi - dTarget);
                dPower = max(dMinCellPower, dPower);

                ret = FALSE;
            }
        }
        else if (dRssi < dMinTarget)
        {
            if (dPower == dMaxCellPower)
            {
                if (pPointData->usGainIndex == m_nMaxIndex)
                {
                    UiSendMsg("LTE RX Loss"
                        , LEVEL_ITEM
                        , dMinTarget
                        , dRssi
                        , dMaxTarget
                        , NULL
                        , pChannelData->usArfcn
                        , "dBm"
                        , "ANT = %s; CA = %s; Freq = %.1lf; CellPower = %.1lf;"
                        , g_lpAntNameA[m_Ant]
                    , g_lpCaNameA[m_Ca]
                    , CLteUtility::GetFreq(pChannelData->usArfcn)
                        , dPower);

                    return SP_E_SPAT_LOSS_LTE_RX_GAIN_TOO_LOW;
                } 
                else
                {
                    pPointData->usGainIndex += 10;
                    pPointData->usGainIndex = (uint16)min(pPointData->usGainIndex, m_nMaxIndex);
                    pPointData->usGainIndex = (uint16)max(pPointData->usGainIndex, m_nMinIndex);
                    pPointData->dPower = (dMinCellPower + dMaxCellPower) / 2;

                    ret = FALSE;
                }
            }
            else
            {
                dPower = dPower - (dRssi - dTarget);
                dPower = min(dMaxCellPower, dPower);

                ret = FALSE;
            }
        }

        pPointData->dPower = dPower;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s: End", __FUNCTION__);

    return SP_OK;
}
