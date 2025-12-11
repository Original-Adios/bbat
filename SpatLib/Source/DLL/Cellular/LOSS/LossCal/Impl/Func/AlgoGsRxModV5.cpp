#include "StdAfx.h"
#include "AlgoGsRxModV5.h"
#include "LteUtility.h"

CAlgoGsRxModV5::CAlgoGsRxModV5( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CAlgoBase(lpName, pFuncCenter)
, m_uMaxRetryTime(30)
, m_nMinIndex(14)
, m_nMaxIndex(108)
{
	m_pNal = NULL;
	m_pFileConfig = NULL;

	m_Ant = RF_ANT_1st;

	m_pApi = NULL;
	m_parrConfig = NULL;
	m_parrResult = NULL;

	m_uRetryTime = 0;

	m_pNv = NULL;
}

CAlgoGsRxModV5::~CAlgoGsRxModV5(void)
{


}

void CAlgoGsRxModV5::__SetAntCa()
{
	__super::__SetAntCa();

	m_parrConfig = &m_pNal->m_arrConfig[m_Ant];
	m_parrResult = &m_pNal->m_arrResult[m_Ant];
}

SPRESULT CAlgoGsRxModV5::PreInit()
{
    CHKRESULT(__super::PreInit());

	m_pNv = (CNvHelperV5*)GetFunc(FUNC_INVHelper);
    m_pNal = (CNalGsV5*)GetFunc(FUNC_INAL);
    m_pFileConfig = (CFileConfig*)GetFunc(FUNC_FILE_CONFIG);
    m_pApi = (IApiRxModV3*)GetFunc(API_AGC);

    return SP_OK;
}

SPRESULT CAlgoGsRxModV5::Init()
{
	CHKRESULT(__super::Init());
    if (m_parrConfig->size() == 0)
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant)
    LogFmtStrA(SPLOGLV_INFO, "%s: ANT = %s", __FUNCTION__, g_lpAntNameAV4[m_Ant]);

    CHKRESULT(InitData());

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
    return SP_OK;
}

SPRESULT CAlgoGsRxModV5::Run()
{
    if (m_arrBandData.size() == 0)
    {
        return SP_OK;
    }

	if( (uint32)m_ePortComp >= m_pNv->m_pNvV5->m_arrGS_PortComp.size())
	{
		LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
		return SP_OK;
	}
    IsValidIndex(m_Ant)
    LogFmtStrA(SPLOGLV_INFO, "%s: ANT = %s", __FUNCTION__, g_lpAntNameA[m_Ant]);

    SPRESULT Result = SP_OK;
    BOOL ret = FALSE;

	m_pApi->m_Afc = 0;
	m_pApi->m_Ant = m_UiAnt;
	m_pApi->m_Bw = LTE_RF_BW0;
	m_pApi->m_bVsBW_Enable = FALSE;
	m_pApi->m_RfChain = RF_CHAIN_CC0;

	m_pApi->m_TxIns = m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Tx[0];
    if (m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Tx[0] > MAX_RF_ANT)
    {
        m_pApi->m_TxIns = 0;
    }
	m_pApi->m_RxIns = m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[m_UiAnt];
	m_pApi->m_bBandAdaptation = m_pNv->m_pNvV5->m_bBandAdaptation;

    m_pApi->m_pCurrentCaller = NULL;
    m_pApi->DealwithData = NULL;


    for (int i = 0; i < m_arrBandData.size(); i++)
    {
        vector<BandData> arrBandData;
        arrBandData.push_back(m_arrBandData[i]);
        m_pApi->m_parrBandData = &arrBandData;
        m_uIndex = i;


        m_pApi->m_pIApi->ClearData();
        m_uRetryTime = 0;
        do
        {
            CHKRESULT(m_pApi->m_pIApi->Run());
            m_arrBandData[i] = arrBandData[0];
            CHKRESULT(AdjustCellPower(ret));
            arrBandData[0].arrChannel[0].arrPoint[0].dPower = m_arrBandData[i].arrChannel[0].arrPoint[0].dPower;
            arrBandData[0].arrChannel[0].arrPoint[0].usGainIndex = m_arrBandData[i].arrChannel[0].arrPoint[0].usGainIndex;
            m_uRetryTime++;
        } while (m_uRetryTime < 10 && !ret);

        if (!ret)
        {
            Result = 1;
        }
        CHKRESULT(Result);
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CAlgoGsRxModV5::InitData()
{
    m_arrBandData.resize(0);

    for (uint32 i = 0; i < m_parrConfig->size(); i++)
    {
        Config* pConfig = &(*m_parrConfig)[i];

		if ((uint32)m_ePortComp >= m_pNv->m_pNvV5->m_arrGS_PortComp.size())
		{
			LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
			return SP_OK;
		}

		if ((m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Tx[0] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Tx[0])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[0] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[0])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[1] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[1])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[2] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[2])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[3] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[3]))
		{
			continue;
		}

        if (pConfig->byPath == RX)
        {
            BandData BandData;

			BandData.nBand = pConfig->byBand;
            BandData.arrChannel.resize(1);

            BandData.nChannelId = pConfig->channelId;
            BandData.nTriggerArfcn = pConfig->nTriggerArfcn;
			BandData.nPathGroupIndex = pConfig->nPathGroupIndex;
			BandData.nPathId = pConfig->nPathId;
			BandData.nTriggerPathId = pConfig->nTriggerPathId;
			BandData.nTriggerGroupIndex = pConfig->nTriggerPathGroupIndex;
			BandData.nTriggerUpLinkStream = pConfig->nTriggerUpLinkStream;

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

SPRESULT CAlgoGsRxModV5::AdjustCellPower(BOOL &ret)
{
    LogFmtStrA(SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, m_uRetryTime);

    if (m_Ant == LTE_RF_ANT_FOURTH)
    {
        int a = 0;
    }

    double dTarget = m_pFileConfig->m_dRxTarget;
    double dMaxTarget = dTarget + m_pFileConfig->m_dRxTolerance;
    double dMinTarget = dTarget - m_pFileConfig->m_dRxTolerance;
    double dMaxCellPower = m_pFileConfig->m_dMaxCellPower;
    double dMinCellPower = m_pFileConfig->m_dMinCellPower;

    ret = TRUE;

    int nIndex= 0;

    for (uint32 i = 0; i < m_parrConfig->size(); i++)
    {
        Config* pConfig = &(*m_parrConfig)[i];
        Result* pResult = &(*m_parrResult)[i];

        if (pConfig->byPath != RX)
        {
            continue;
        }

        if((m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Tx[0] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Tx[0])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[0] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[0])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[1] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[1])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[2] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[2])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[3] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[3]))
		{
			continue;
		}

        if (pResult->bDone)
        {
            nIndex++;
            continue;
        }

        ChannelData* pChannelData = &m_arrBandData[m_uIndex].arrChannel[0];
        PointData* pPointData = &pChannelData->arrPoint[0];
        double dRssi = pPointData->dRssi;
        double dPower = pPointData->dPower;
        IsValidIndex(m_Ant)
        if (IN_RANGE(dMinTarget, dRssi, dMaxTarget))
        {
            UiSendMsg("LTE RX GS"
                , LEVEL_ITEM
                , dMinTarget
                , dRssi
                , dMaxTarget
                , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                , pChannelData->usArfcn
                , "dBm"
                , "ANT = %s; PathId = %d; RfAnt = %d; Freq = %.1lf; CellPower = %.1lf; Index = %d"
                , g_lpAntNameAV4[m_Ant]
				, pConfig->nPathId
				, m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[m_Ant]
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
                    UiSendMsg("LTE RX GS"
                        , LEVEL_ITEM
                        , dMinTarget
                        , dRssi
                        , dMaxTarget
                        , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                        , pChannelData->usArfcn
                        , "dBm"
                        , "ANT = %s;Freq = %.1lf; CellPower = %.1lf;"
                        , g_lpAntNameAV4[m_Ant]
                        , CLteUtility::GetFreq(pChannelData->usArfcn)
                        , dPower);

                    return SP_E_SPAT_LOSS_LTE_RX_GAIN_TOO_HIGH;
                } 
                else
                {
                    pPointData->usGainIndex -= 5;
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
                    UiSendMsg("LTE RX GS"
                        , LEVEL_ITEM
                        , dMinTarget
                        , dRssi
                        , dMaxTarget
                        , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                        , pChannelData->usArfcn
                        , "dBm"
                        , "ANT = %s; Freq = %.1lf; CellPower = %.1lf;"
                        , g_lpAntNameAV4[m_Ant]
                    , CLteUtility::GetFreq(pChannelData->usArfcn)
                        , dPower);

                    return SP_E_SPAT_LOSS_LTE_RX_GAIN_TOO_LOW;
                } 
                else
                {
                    pPointData->usGainIndex += 5;
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
        break;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s: End", __FUNCTION__);

    return SP_OK;
}
