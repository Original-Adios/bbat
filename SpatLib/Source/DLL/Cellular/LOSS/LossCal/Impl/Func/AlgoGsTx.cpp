#include "StdAfx.h"
#include "AlgoGsTx.h"
#include "LteUtility.h"

CAlgoGsTx::CAlgoGsTx( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CAlgoBase(lpName, pFuncCenter)
, m_uMaxRetryTime(30)
{
	m_pNal = NULL;
	m_pFileConfig = NULL;

	m_Ant = RF_ANT_1st;
	m_Ca = LTE_CA_PCC;

	m_pApi = NULL;
	m_parrConfig = NULL;
	m_parrResult = NULL;
}

CAlgoGsTx::~CAlgoGsTx(void)
{


}

void CAlgoGsTx::__SetAntCa()
{
	__super::__SetAntCa();

	m_parrConfig = &m_pNal->m_arrConfig[m_Ant];
	m_parrResult = &m_pNal->m_arrResult[m_Ant];


}

SPRESULT CAlgoGsTx::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pNal = (CNalGs*)GetFunc(FUNC_INAL);
    m_pFileConfig = (CFileConfig*)GetFunc(FUNC_FILE_CONFIG);
    m_pApi = (IApiTxV2*)GetFunc(API_APC);

    return SP_OK;
}

SPRESULT CAlgoGsTx::Init()
{
	CHKRESULT(__super::Init());
    if (m_parrConfig->size() == 0)
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant)
    LogFmtStrA(SPLOGLV_INFO, "%s: ANT = %s; CA = %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca]);

    CHKRESULT(InitData());

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
    return SP_OK;
}

SPRESULT CAlgoGsTx::Run()
{
    if (m_arrBandData.size() == 0)
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant)
    LogFmtStrA(SPLOGLV_INFO, "%s, Ant: %s, Ca: %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca]);

    SPRESULT Result;

    m_pApi->m_Afc = 0;
    m_pApi->m_Ant = m_Ant; 
    m_pApi->m_Ca = m_Ca; 
    m_pApi->m_parrBandData = &m_arrBandData;

    m_pApi->m_pCurrentCaller = this;
    m_pApi->DealwithData = this->GetResultStatic;

    m_pApi->m_pIApi->ClearData();
    Result = m_pApi->m_pIApi->Run();

    CHKRESULT(Result);

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CAlgoGsTx::InitData()
{
    m_arrBandData.resize(0);

    for (uint32 i = 0; i < m_parrConfig->size(); i++)
    {
        Config* pConfig = &(*m_parrConfig)[i];
        
        if (pConfig->byPath == TX)
        {
            BandData BandData;

            BandData.arrChannel.resize(1);
            BandData.nIndicator = CLteUtility::m_BandInfo[pConfig->byBand].nIndicator;
            
            ChannelData* pChannelData = &BandData.arrChannel[0];
            pChannelData->usArfcn = pConfig->usChannel;
            pChannelData->arrSection.resize(1);
			pChannelData->usPointNumber = 1;
			pChannelData->reserved = 0;

            SectionData* pSectionData = &pChannelData->arrSection[0];
            pSectionData->arrPower.resize(1);
            pSectionData->dEnp = 0;
            pSectionData->usApt = 24;
            pSectionData->usPaMode = 1;
            pSectionData->usWordStep = 8;
            pSectionData->usBeginWord = 8 * 25;

            m_arrBandData.push_back(BandData);
        }
    }

    return SP_OK;
}

SPRESULT CAlgoGsTx::GetResultStatic( void* pCaller
                                    , BOOL* ret
                                    , uint32 uStartBand
                                    , uint32 uStartChannel
                                    , uint32 uStartSection
                                    , uint32 uStopBand
                                    , uint32 uStopChannel
                                    , uint32 uStopSection
                                    , uint32 uRetryTime )
{
    CAlgoGsTx* pObject = (CAlgoGsTx*)pCaller;

    return pObject->GetResult(ret
        , uStartBand
        , uStartChannel
        , uStartSection
        , uStopBand
        , uStopChannel
        , uStopSection
        , uRetryTime);
}

SPRESULT CAlgoGsTx::GetResult( BOOL* ret
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

        if (uIndex < uStartBand || uIndex > uStopBand)
        {
            uIndex++;
            continue;
        }
        
        BandData* pBandData = &m_arrBandData[uIndex++];

        ChannelData* pChannelData = &pBandData->arrChannel[0];
        SectionData* pSectionData = &pChannelData->arrSection[0];

        if (pSectionData->usDone)
        {
            continue;
        }

        double dTargetPower = pBandResult->TxResult.dTargetPower;
        double dPower = pSectionData->arrPower[0];
        uint16 usWord = (uint16)(pSectionData->usBeginWord + pSectionData->sWordOffset);

        //need move
        double dSpec = 0.5;
        double dSlope = -8;

        double dMaxTarget = dTargetPower + dSpec;
        double dMinTarget = dTargetPower - dSpec;

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
                    , "ANT = %s; CA = %s; Freq = %.1lf; Word = %d;"
                    , g_lpAntNameA[m_Ant]
                    , g_lpCaNameA[m_Ca]
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
                continue;
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
                    , "ANT = %s; CA = %s; Freq = %.1lf; Word = %d;"
                    , g_lpAntNameA[m_Ant]
                    , g_lpCaNameA[m_Ca]
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
                continue;
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
                , "ANT = %s; CA = %s; Freq = %.1lf; Word = %d;"
                , g_lpAntNameA[m_Ant]
                , g_lpCaNameA[m_Ca]
                , CLteUtility::GetFreq(pChannelData->usArfcn)
                , usWord);

            pBandResult->TxResult.dPower = dPower;
            pBandResult->TxResult.usWord = usWord;

            pBandResult->bDone = TRUE;
            pSectionData->usDone = TRUE;
        }
    }

    LogFmtStrA(SPLOGLV_INFO, "%s: End", __FUNCTION__);

    return SP_OK;
}
