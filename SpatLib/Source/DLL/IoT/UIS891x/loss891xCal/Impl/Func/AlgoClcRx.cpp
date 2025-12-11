#include "StdAfx.h"
#include "AlgoClcRx.h"
#include "LteUtility.h"

CAlgoClcRx::CAlgoClcRx( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
    , m_uMaxRetryTime(30)
{
	m_pNal = NULL;
	m_pFileConfig = NULL;
	m_Ant = RF_ANT_INVALID;
	m_Ca = LTE_CA_PCC;
	m_pApi = NULL;
	m_parrConfig = NULL;
	m_parrResult = NULL;
	m_uRetryTime = 0;
	ZeroMemory((void *)(&m_Loss),sizeof(RF_CABLE_LOSS)) ;
	m_pRFTester = NULL; 
}

CAlgoClcRx::~CAlgoClcRx(void)
{


}

void CAlgoClcRx::SetAntCa( RF_ANT_E Ant, LTE_CA_E Ca )
{
    m_Ant = Ant;
    m_Ca = Ca;

    m_parrConfig = &m_pNal->m_arrConfig[Ant];
    m_parrResult = &m_pNal->m_arrResult[Ant];
}

SPRESULT CAlgoClcRx::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pNal = (CNalClc_UIS8910*)GetFunc(FUNC_INAL);
    m_pFileConfig = (CFileConfig_UIS8910*)GetFunc(FUNC_FILE_CONFIG);
    m_pRFTester = m_pSpatBase->m_pRFTester;
    m_pApi = (IApiRx*)GetFunc(API_AGC);

    return SP_OK;
}

SPRESULT CAlgoClcRx::Init()
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

SPRESULT CAlgoClcRx::Run()
{
    if (m_arrarrBandData.size() == 0)
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant);
    LogFmtStrA(SPLOGLV_INFO, "%s, Ant: %s, Ca: %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca]);

    SPRESULT Result;

    m_pApi->m_Afc = 0;
    m_pApi->m_Ant = m_Ant; 
    m_pApi->m_Ca = m_Ca; 

    m_pApi->m_pCurrentCaller = NULL;
    m_pApi->DealwithData = NULL;
    uint32 iArrBandDataSize = m_arrarrBandData.size();
    for (uint32 i = 0; i < iArrBandDataSize; i++)
    {
        m_pApi->m_parrBandData = &m_arrarrBandData[i];
        m_pApi->m_pIApi->ClearData();
        SetLoss(0);

        BOOL ret = TRUE;
        m_uRetryTime = 0;

        do 
        {
            Result = m_pApi->m_pIApi->Run();
            CHKRESULT(Result);
            CHKRESULT(AdjustLoss(ret, i));
            m_uRetryTime++;
        } while (!ret && m_uRetryTime != m_uMaxRetryTime);

        if (m_uRetryTime == m_uMaxRetryTime)
        {
            return SP_E_SPAT_LOSS_LTE_RX_RETRY_FAIL;
        }
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CAlgoClcRx::InitData()
{
    m_arrarrBandData.resize(0);
    uint32 iArrConfigSize = m_parrConfig->size();
    for (uint32 i = 0; i < iArrConfigSize; i++)
    {
        Config* pConfig = &(*m_parrConfig)[i];

        if (pConfig->byPath == RX)
        {
            BandData data;

            data.nBand = pConfig->byBand;
            data.arrChannel.resize(1);

            ChannelData* pChannelData = &data.arrChannel[0];
            pChannelData->nIndicator = pConfig->byIndicator;
            pChannelData->usArfcn = pConfig->usChannel;
            pChannelData->arrPoint.resize(1);

            PointData* pPointData = &pChannelData->arrPoint[0];

            pPointData->usGainIndex = pConfig->Rx.byIndex;
            pPointData->dPower = pConfig->Rx.dCellPower;

            vector<BandData> arrBandData;
            arrBandData.push_back(data);
            m_arrarrBandData.push_back(arrBandData);
        }
    }

    return SP_OK;
}


SPRESULT CAlgoClcRx::CheckLoss( uint32 uIndex )
{
    LogFmtStrA(SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, m_uRetryTime);
    IsValidIndex(m_Ant);
    BandData* pBandData = &m_arrarrBandData[uIndex][0];
    ChannelData* pChannelData = &pBandData->arrChannel[0];
    PointData* pPointData = &pChannelData->arrPoint[0];
    uint32 iArrConfigSize = m_parrConfig->size();
    uint32 i = 0;
    for (; i < iArrConfigSize; i++)
    {
        if ((*m_parrConfig)[i].byPath == RX )
        {
            if (0 == uIndex)
            {
                break;
            }
            else
            {
                uIndex--;
            }
        }
    }

    Config* pConfig = &(*m_parrConfig)[i];

    double dTarget = pConfig->Rx.dRssi;
    double dSpec = 0.15;
    double dRssi = pPointData->dRssi;

    double dMaxTarget = dTarget + dSpec;
    double dMinTarget = dTarget - dSpec;

    if (!IN_RANGE(dMinTarget, dRssi, dMaxTarget))
    {
        UiSendMsg("LTE RX Loss Check"
            , LEVEL_ITEM
            , dMinTarget
            , dRssi
            , dMaxTarget
            , NULL
            , pChannelData->usArfcn
            , "dBm"
            , "ANT = %s; CA = %s; Freq = %.1lf"
            , g_lpAntNameA[m_Ant]
            , g_lpCaNameA[m_Ca]
            , CLteUtility::GetFreq(pChannelData->usArfcn));

        return SP_E_SPAT_LOSS_LTE_CHECK_FAIL;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s: End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CAlgoClcRx::AdjustLoss( BOOL &ret, uint32 uIndex )
{
    LogFmtStrA(SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, m_uRetryTime);
    IsValidIndex(m_Ant);
    if(m_uRetryTime < 5) //用于等待UE/仪表工作状态稳定。
	{
		ret = FALSE;
		return SP_OK;
	}

    BandData* pBandData = &m_arrarrBandData[uIndex][0];
    ChannelData* pChannelData = &pBandData->arrChannel[0];
    PointData* pPointData = &pChannelData->arrPoint[0];

    ret = TRUE;
    uint32 iArrConfigSize = m_parrConfig->size();
    uint32 i = 0;
    for (; i < iArrConfigSize; i++)
    {
        if ((*m_parrConfig)[i].byPath == RX )
        {
            if (0 == uIndex)
            {
                break;
            }
            else
            {
                uIndex--;
            }
        }
    }

    Result* pResult = &(*m_parrResult)[i];
    Config* pConfig = &(*m_parrConfig)[i];

    double dTarget = pConfig->Rx.dRssi;
    double dSpec = 0.15;
    double dRssi = pPointData->dRssi;
    CONST double dDelta = 0.0f;//根据实际测试补偿loss结果

    double dMaxTarget = dTarget + dSpec;
    double dMinTarget = dTarget - dSpec;

    if (IN_RANGE(dMinTarget, dRssi, dMaxTarget))
    {
        pResult->dLoss += (dTarget - dRssi) + dDelta;
        m_pFileConfig->GetLossLimit(CLteUtility::GetFreq(pChannelData->usArfcn), m_pFileConfig->m_dLossValLower, m_pFileConfig->m_dLossValUpper);
        UiSendMsg("LTE RX Loss"
            , LEVEL_ITEM
            , m_pFileConfig->m_dLossValLower
            , pResult->dLoss
            , m_pFileConfig->m_dLossValUpper
            , NULL
            , pChannelData->usArfcn
            , "dBm"
            , "ANT = %s; CA = %s; Freq = %.1lf; CellPower = %.1lf; Index = %d; Rssi = %.1lf"
            , g_lpAntNameA[m_Ant]
            , g_lpCaNameA[m_Ca]
            , CLteUtility::GetFreq(pChannelData->usArfcn)
            , pConfig->Rx.dCellPower
            , pConfig->Rx.byIndex
            , dRssi);

        if (pResult->dLoss < 0.0)
        {
            if (NULL != m_pFileConfig->m_pstrNegativeLossDetected)
            {
                if ( string::npos == (*m_pFileConfig->m_pstrNegativeLossDetected).find("LTE"))
                {
                    (*m_pFileConfig->m_pstrNegativeLossDetected) += "LTE";
                    (*m_pFileConfig->m_pstrNegativeLossDetected) += ",";
                }
            }
        }

        if (pResult->dLoss <= m_pFileConfig->m_dLossValLower || pResult->dLoss > m_pFileConfig->m_dLossValUpper)
        {
            LogFmtStrA(SPLOGLV_ERROR, "Loss value(%.2f) is out of range(%.2f, %.2f)", pResult->dLoss, m_pFileConfig->m_dLossValLower, m_pFileConfig->m_dLossValUpper);
            return SP_E_SPAT_LOSS_OUT_OF_RANGE;
        }
    }
    else
    {
        if (m_uRetryTime == m_uMaxRetryTime)
        {
            UiSendMsg("LTE RX Loss"
                , LEVEL_ITEM
                , dMinTarget
                , dRssi
                , dMaxTarget
                , NULL
                , pChannelData->usArfcn
                , "dBm"
                , "ANT = %s; CA = %s; Freq = %.1lf; CellPower = %.1lf; Index = %d; Rssi = %.1lf"
                , g_lpAntNameA[m_Ant]
                , g_lpCaNameA[m_Ca]
                , CLteUtility::GetFreq(pChannelData->usArfcn)
                , pConfig->Rx.dCellPower
                , pConfig->Rx.byIndex
                , dRssi);

            return SP_E_SPAT_LOSS_LTE_RX_RETRY_FAIL;
        }
        else
        {
            LogFmtStrA(SPLOGLV_INFO, "Freq = % .1lf; Loss = % .1lf; CellPower = %.1lf; Index = %d; dTargetRssi = % .1lf; dRssi = % .1lf", CLteUtility::GetFreq(pChannelData->usArfcn), pResult->dLoss, pConfig->Rx.dCellPower, pConfig->Rx.byIndex, dTarget, dRssi);
        }
        pResult->dLoss += (dTarget - dRssi);
        SetLoss(pResult->dLoss);
        ret = FALSE;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s: End", __FUNCTION__);

    return SP_OK;
}

void CAlgoClcRx::SetLoss( double dLoss )
{
    uint32 retrun_val = 0;

    m_Loss.lteLoss.nCount = 1;
    m_Loss.lteLoss.arrPoint[0].dFreq = 1800;
    m_Loss.lteLoss.arrPoint[0].dLoss[0][RF_IO_TX] = dLoss;
    m_Loss.lteLoss.arrPoint[0].dLoss[0][RF_IO_RX] = dLoss;
    m_Loss.lteLoss.arrPoint[0].dLoss[1][RF_IO_TX] = dLoss;
    m_Loss.lteLoss.arrPoint[0].dLoss[1][RF_IO_RX] = dLoss;

    U_RF_PARAM param;
    param.pLoss = &m_Loss;
    retrun_val = m_pRFTester->SetParameter(PT_CABLE_LOSS,param);
}

SPRESULT CAlgoClcRx::Check()
{
    if (m_arrarrBandData.size() == 0)
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant);
    LogFmtStrA(SPLOGLV_INFO, "%s, Ant: %s, Ca: %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca]);

    SPRESULT Result;

    m_pApi->m_Afc = 0;
    m_pApi->m_Ant = m_Ant; 
    m_pApi->m_Ca = m_Ca; 

    m_pApi->m_pCurrentCaller = NULL;
    m_pApi->DealwithData = NULL;
    uint32 iArrBandDataSize = m_arrarrBandData.size();
    for (uint32 i = 0; i < iArrBandDataSize; i++)
    {
        m_pApi->m_parrBandData = &m_arrarrBandData[i];
        SetLoss((*m_parrResult)[i].dLoss);
        
        Result = m_pApi->m_pIApi->Run();
        CHKRESULT(Result);
        CHKRESULT(CheckLoss(i));
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

