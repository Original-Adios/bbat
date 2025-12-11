#include "StdAfx.h"
#include "AlgoGsRxModV4.h"
#include "LteUtility.h"

CAlgoGsRxModV4::CAlgoGsRxModV4( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CAlgoBase(lpName, pFuncCenter)
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

CAlgoGsRxModV4::~CAlgoGsRxModV4(void)
{


}

void CAlgoGsRxModV4::__SetAntCa()
{
	__super::__SetAntCa();

	m_parrConfig = &m_pNal->m_arrConfig[m_Ant];
	m_parrResult = &m_pNal->m_arrResult[m_Ant];
}

SPRESULT CAlgoGsRxModV4::PreInit()
{
    CHKRESULT(__super::PreInit());

	m_pNv = (CNvHelperV4*)GetFunc(FUNC_INVHelper);
    m_pNal = (CNalGsV4*)GetFunc(FUNC_INAL);
    m_pFileConfig = (CFileConfig*)GetFunc(FUNC_FILE_CONFIG);
    m_pApi = (IApiRxModV3*)GetFunc(API_AGC);

    return SP_OK;
}

SPRESULT CAlgoGsRxModV4::Init()
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

SPRESULT CAlgoGsRxModV4::Run()
{
    if (m_arrBandData.size() == 0)
    {
        return SP_OK;
    }

	if( (uint32)m_ePortComp >= m_pNv->m_pNvV4->m_arrGS_PortComp.size())
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

	m_pApi->m_TxIns = m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Tx[0];
	m_pApi->m_RxIns = m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[m_UiAnt];
	m_pApi->m_bBandAdaptation = m_pNv->m_pNvV4->m_bBandAdaptation;

    m_pApi->m_parrBandData = &m_arrBandData;

    m_pApi->m_pCurrentCaller = NULL;
    m_pApi->DealwithData = NULL;

    m_pApi->m_pIApi->ClearData();

    vector<BandData> m_arrBandData_ = m_arrBandData;
    //针对ANT0 ANT1 ANT2 ANT3 区分出2RX和4RX分开测量

    for (int pathtype = 0; pathtype < 2; pathtype++)
    {
        m_arrBandData.clear();
        m_pApi->m_pIApi->ClearData();
        for (int i = 0; i < m_arrBandData_.size(); i++)
        {
            if ((m_arrBandData_[i].nChannelId < 16) && (pathtype == 0))
            {
                m_arrBandData.push_back(m_arrBandData_[i]);
                m_pApi->m_Ant = (RF_ANT_E)m_arrBandAntIndex[i];
            }


            if ((m_arrBandData_[i].nChannelId >= 16) && (pathtype == 1))
            {
                m_arrBandData.push_back(m_arrBandData_[i]);
                m_pApi->m_Ant = (RF_ANT_E)m_arrBandAntIndex[i];
            }
        }
        if (m_arrBandData.size() != 0)
        {
            m_uRetryTime = 0;
            do
            {
                CHKRESULT(m_pApi->m_pIApi->Run());
                CHKRESULT(AdjustCellPower(ret, pathtype));
                m_uRetryTime++;
            } while (m_uRetryTime < m_uMaxRetryTime && !ret);

            if (!ret)
            {
                Result = 1;
            }
            CHKRESULT(Result);
        }
    }
 
    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CAlgoGsRxModV4::InitData()
{
    m_arrBandData.resize(0);
    m_arrBandAntIndex.resize(0);

    for (uint32 i = 0; i < m_parrConfig->size(); i++)
    {
        Config* pConfig = &(*m_parrConfig)[i];

		if ((uint32)m_ePortComp >= m_pNv->m_pNvV4->m_arrGS_PortComp.size())
		{
			LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
			return SP_OK;
		}

		if ((m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Tx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Tx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Rx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Rx[1] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[1])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Rx[2] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[2])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Rx[3] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[3]))
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
            ChannelData* pChannelData = &BandData.arrChannel[0];
            pChannelData->nIndicator = CLteUtility::m_BandInfo[pConfig->byBand].nIndicator;
            pChannelData->usArfcn = pConfig->usChannel;
            pChannelData->arrPoint.resize(1);

            PointData* pPointData = &pChannelData->arrPoint[0];

            pPointData->usGainIndex = m_pFileConfig->m_byRxIndex;
            pPointData->dPower = -50;

            m_arrBandData.push_back(BandData);
            m_arrBandAntIndex.push_back(pConfig->antRxIndex);
        }
    }

    return SP_OK;
}

SPRESULT CAlgoGsRxModV4::AdjustCellPower(BOOL &ret,int pathtype)
{
    LogFmtStrA(SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, m_uRetryTime);

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

        if ((pathtype == 0) && (pConfig->channelId >= 16))
            continue;
        if ((pathtype == 1) && (pConfig->channelId < 16))
            continue;


        if (pConfig->byPath != RX)
        {
            continue;
        }

        if((m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Tx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Tx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Rx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Rx[1] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[1])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Rx[2] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[2])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[pConfig->byBand].Rx[3] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[3]))
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
                , "ANT = %s; Freq = %.1lf; CellPower = %.1lf; Index = %d"
                , g_lpAntNameAV4[m_Ant]
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
