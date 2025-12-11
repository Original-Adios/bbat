#include "StdAfx.h"
#include "AlgoGsTxModV5.h"
#include "LteUtility.h"
#include "assert.h"

CAlgoGsTxModV5::CAlgoGsTxModV5( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CAlgoBase(lpName, pFuncCenter)
, m_uMaxRetryTime(30)
{
	m_pNal = NULL;
	m_pFileConfig = NULL;
	m_Ant = RF_ANT_1st;
	m_pApi = NULL;
	m_parrConfig = NULL;
	m_parrResult = NULL;

	m_pNv = NULL;
}

CAlgoGsTxModV5::~CAlgoGsTxModV5(void)
{


}

void CAlgoGsTxModV5::__SetAntCa()
{
	__super::__SetAntCa();

	m_parrConfig = &m_pNal->m_arrConfig[m_Ant];
	m_parrResult = &m_pNal->m_arrResult[m_Ant];
}

SPRESULT CAlgoGsTxModV5::PreInit()
{
    CHKRESULT(__super::PreInit());

	m_pNv = (CNvHelperV5*)GetFunc(FUNC_INVHelper);
    m_pNal = (CNalGsV5*)GetFunc(FUNC_INAL);
    m_pFileConfig = (CFileConfig*)GetFunc(FUNC_FILE_CONFIG);
    m_pApi = (IApiTxModV3*)GetFunc(API_APC);

    return SP_OK;
}

SPRESULT CAlgoGsTxModV5::Init()
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

SPRESULT CAlgoGsTxModV5::Run()
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
    LogFmtStrA(SPLOGLV_INFO, "%s, Ant: %s", __FUNCTION__, g_lpAntNameAV4[m_Ant]);

    SPRESULT Result;

    m_pApi->m_Ant = m_UiAnt; 
    m_pApi->m_RfChain = RF_CHAIN_CC0; 

    m_pApi->m_TxIns = m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Tx[m_UiAnt];
    m_pApi->m_RxIns = m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[m_UiAnt];
    m_pApi->m_bBandAdaptation = m_pNv->m_pNvV5->m_bBandAdaptation;

    m_pApi->m_pCurrentCaller = this;
    m_pApi->DealwithData = this->GetResultStatic;


    for (int i = 0; i < m_arrBandData.size(); i++)
    {
        m_BandData.clear();
        m_BandData.push_back(m_arrBandData[i]);
        m_pApi->m_parrBandData = &m_BandData;
        m_uIndex = i;

        m_pApi->m_pIApi->ClearData();

        Result = m_pApi->m_pIApi->Run();
        m_arrBandData[i] = m_BandData[0];
        CHKRESULT(Result);
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CAlgoGsTxModV5::InitData()
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

		if( (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Tx[0] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Tx[0])
			||(m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[0] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[0])
			||(m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[1] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[1]) 
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[2] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[2])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[3] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[3]))
		{
			continue;
		}
        
        if (pConfig->byPath == TX)
        {
            BandData BandData;

			BandData.nBand = pConfig->byBand;
            BandData.arrChannel.resize(1);
            BandData.nIndicator = CLteUtility::m_BandInfo[pConfig->byBand].nIndicator;

            BandData.nChannelId = pConfig->channelId;
			BandData.nPathId = pConfig->nPathId;
			BandData.nPathGroupIndex = pConfig->nPathGroupIndex;
            
            ChannelData* pChannelData = &BandData.arrChannel[0];
            pChannelData->usArfcn = pConfig->usChannel;
            pChannelData->arrSection.resize(1);
			pChannelData->usPointNumber = 1;

            SectionData* pSectionData = &pChannelData->arrSection[0];
            pSectionData->arrPower.resize(1);
            pSectionData->dEnp = 0;
            pSectionData->usApt = m_pFileConfig->m_nVoltage;
            pSectionData->usPaMode = 1;
            pSectionData->usWordStep = 8;
            pSectionData->usBeginWord = 8 * 25;
			pSectionData->sWordOffset = 0;
			pSectionData->usPdet = 0;
			pSectionData->usDone = 0;

            m_arrBandData.push_back(BandData);
        }
    }

    return SP_OK;
}

SPRESULT CAlgoGsTxModV5::GetResultStatic( void* pCaller
                                    , BOOL* ret
                                    , uint32 uStartBand
                                    , uint32 uStartChannel
                                    , uint32 uStartSection
                                    , uint32 uStopBand
                                    , uint32 uStopChannel
                                    , uint32 uStopSection
                                    , uint32 uRetryTime )
{
    CAlgoGsTxModV5* pObject = (CAlgoGsTxModV5*)pCaller;

    return pObject->GetResult(ret
        , uStartBand
        , uStartChannel
        , uStartSection
        , uStopBand
        , uStopChannel
        , uStopSection
        , uRetryTime);
}

SPRESULT CAlgoGsTxModV5::GetResult( BOOL* ret
                              , uint32 uStartBand
                              , uint32 /*uStartChannel*/
                              , uint32 /*uStartSection*/
                              , uint32 uStopBand
                              , uint32 /*uStopChannel*/
                              , uint32 /*uStopSection*/
                              , uint32 uRetryTime )
{
    LogFmtStrA(SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, uRetryTime);

    IsValidIndex(m_Ant)
    uint32 uIndex = 0;
    for (uint32 i = 0; i < m_parrConfig->size(); i++)
    {
        Config* pConfig = &(*m_parrConfig)[i];
        Result* pBandResult = &(*m_parrResult)[i];

        if (pConfig->byPath != TX)
        {
            continue;
        }

        if ((m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Tx[0] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Tx[0])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[0] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[0])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[1] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[1])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[2] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[2])
            || (m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Rx[3] != m_pNv->m_pNvV5->m_arrGS_PortComp[m_ePortComp].Rx[3]))
		{
			continue;
		}

        if (pBandResult->bDone)
        {
            continue;
        }
        
        BandData* pBandData = &m_BandData[0];

        ChannelData* pChannelData = &pBandData->arrChannel[0];
        SectionData* pSectionData = &pChannelData->arrSection[0];

        if (pSectionData->usDone)
        {
            continue;
        }

        double dTargetPower = pBandResult->TxResult.dTargetPower;
        double dPower = pSectionData->arrPower[0].dPower;
        uint16 usWord = (uint16)(pSectionData->usBeginWord + pSectionData->sWordOffset);

        //need move
        double dSpec = 0.5;
        double dSlope = -8;

        double dMaxTarget = dTargetPower + dSpec;
        double dMinTarget = dTargetPower - dSpec;

        LogFmtStrA(SPLOGLV_INFO, "dTargetPower = %.2f, dPower = %.2f, dMaxTarget = %.2f, dMinTarget = %.2f, usWord = %d"
            , dTargetPower
            , dPower
            , dMaxTarget
            , dMinTarget
            , usWord);

        if (dPower < dMinTarget)
        {
            if (uRetryTime == m_uMaxRetryTime || pSectionData->usBeginWord + pSectionData->sWordOffset == 0)
            {
                UiSendMsg("LTE TX GS"
                    , LEVEL_ITEM
                    , dMinTarget
                    , dPower
                    , dMaxTarget
                    , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                    , pChannelData->usArfcn
                    , "dBm"
                    , "ANT = %s; RfAnt = %d; Path = %d; Freq = %.1lf; Word = %d"
                    , g_lpAntNameAV4[m_Ant]
                    , m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Tx[0]
                    , pConfig->nPathId
                    , CLteUtility::GetFreq(pChannelData->usArfcn)
                    , usWord);

                return SP_E_SPAT_LOSS_LTE_TX_TOO_LOW;
            }
            else
            {
                usWord = (uint16)((dTargetPower - dPower) * dSlope + usWord + 0.5);
                usWord = (uint16)max(0, (short)usWord);
                usWord = (uint16)min(0x270, usWord);

                pSectionData->sWordOffset = (short)(usWord - pSectionData->usBeginWord);
                *ret = FALSE;
                break;
            }
        }
        else if (dPower > dMaxTarget)
        {
            if (uRetryTime == m_uMaxRetryTime || (pSectionData->usBeginWord + pSectionData->sWordOffset == 0x270))
            {
                UiSendMsg("LTE TX GS"
                    , LEVEL_ITEM
                    , dMinTarget
                    , dPower
                    , dMaxTarget
                    , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                    , pChannelData->usArfcn
                    , "dBm"
                    , "ANT = %s; RfAnt = %d; Path = %d; Freq = %.1lf; Word = %d"
                    , g_lpAntNameAV4[m_Ant]
                    , m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Tx[0]
                    , pConfig->nPathId
                    , CLteUtility::GetFreq(pChannelData->usArfcn)
                    , usWord);
                

                return SP_E_SPAT_LOSS_LTE_TX_TOO_HIGH;
            }
            else 
            {
                usWord = (uint16)((dTargetPower - dPower) * dSlope + usWord + 0.5);
                usWord = (uint16)max(0, (short)usWord);
                usWord = (uint16)min(0x270, usWord);

                pSectionData->sWordOffset = (short)(usWord - pSectionData->usBeginWord);
                *ret = FALSE;
                break;
            }
        }
        else
        {
            UiSendMsg("LTE TX GS"
                , LEVEL_ITEM
                , dMinTarget
                , dPower
                , dMaxTarget
                , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                , pChannelData->usArfcn
                , "dBm"
                , "ANT = %s; RfAnt = %d; Path = %d; Freq = %.1lf; Word = %d"
                , g_lpAntNameAV4[m_Ant]
                , m_pNv->m_pNvV5->m_arrDownload_AntMap[pConfig->byBand].Tx[0]
                , pConfig->nPathId
                , CLteUtility::GetFreq(pChannelData->usArfcn)
                , usWord);

            pBandResult->TxResult.dPower = dPower;
            pBandResult->TxResult.usWord = usWord;
            pBandResult->TxResult.nVoltage = m_pFileConfig->m_nVoltage;

            pBandResult->bDone = TRUE;
            pSectionData->usDone = TRUE;
            break;
        }
    }

    LogFmtStrA(SPLOGLV_INFO, "%s: End", __FUNCTION__);

    return SP_OK;
}
