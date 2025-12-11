#include "StdAfx.h"
#include "ApiTxV21.h"
#include "math.h"
#include "assert.h"

CApiTxV21::CApiTxV21( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CApiBase(lpName, pFuncCenter)
{
    AddFunc(lpName, (IApiTxV2*)this);

    m_pPhoneParam = NULL;
    m_pTesterParam = NULL;
    m_pTesterResult = NULL;

    m_nMaxPointNumber = 1000;
    m_bChannelGap = 1;
	//MT8820
	m_bMT8820 = FALSE;
	m_nStartBand = 0;
	m_nStopBand = 0;
	m_nStartChannel = 0;
	m_nStopChannel = 0;
	m_nStartSection = 0;
	m_nStopSection = 0;

	m_nPointNumber = 0;
	m_bChannelGap = FALSE;
	m_pCurrentCa = NULL;
	m_bResult = FALSE;
	m_uRetryTime = 1;
}

CApiTxV21::~CApiTxV21(void)
{
    if (m_pPhoneParam != NULL)
    {
        delete m_pPhoneParam;
        m_pPhoneParam = NULL;
    }

    if (m_pTesterParam != NULL)
    {
        delete m_pTesterParam;
        m_pTesterParam = NULL;
    }

    if (m_pTesterResult != NULL)
    {
        delete m_pTesterResult;
        m_pTesterResult = NULL;
    }
}

SPRESULT CApiTxV21::Run()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    CHKRESULT(SwitchCa());

	BOOL ret;
	m_uRetryTime = 0;
	
	CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->GetProperty(DP_TXRX_SAMEPOINT, 0, &m_bMT8820), "Instrument::GetProperty()!");

    do 
    {
        LogFmtStrA(SPLOGLV_INFO, "%s: Time = %d", __FUNCTION__, m_uRetryTime);

        InitParam(0, 0, 0);
        m_bResult = TRUE;

        for (uint32 i = 0; i < m_parrBandData->size(); i++)
        {   
            BandData* pBandData = &(*m_parrBandData)[i];
			if (m_bMT8820)
			{
				InitParam(i, 0 , 0);
			}

            for (uint32 j = 0; j < pBandData->arrChannel.size(); j++)
            {
                ChannelData* pChannelData = &pBandData->arrChannel[j];
                BOOL bNewChannel = TRUE;
                double dEnp = 0;

                for (uint32 k = 0; k < pChannelData->arrSection.size(); k++)
                {
                    SectionData* pSectionData = &pChannelData->arrSection[k];

                    if (!m_bMT8820 && pSectionData->usDone)
                    {
                        continue;
                    }

                    BOOL bGap = FALSE;

                    if (bNewChannel)
                    {
                        bNewChannel = FALSE;
                        CHKRESULT(InsertChannel(&ret, i, j, k));
                        if (!ret)
                        {
                            CHKRESULT(DoFunc());
                            InitParam(i, j ,k);
                            CHKRESULT(InsertChannel(&ret, i, j, k));
                        }

                        dEnp = pSectionData->dEnp;
                        bGap = TRUE;
                        //if (m_bMT8820)
                        //{
                        //    bGap = FALSE;
                        //}
                    }
                    else if (fabs(dEnp - pSectionData->dEnp) >= 20)
                    {
                        bNewChannel = FALSE;
                        CHKRESULT(InsertChannel(&ret, i, j, k));
                        if (!ret)
                        {
                            CHKRESULT(DoFunc());
                            InitParam(i, j ,k);
                            CHKRESULT(InsertChannel(&ret, i, j, k));
                        }

                        dEnp = pSectionData->dEnp;
                        bGap = TRUE;
                    }

                    CHKRESULT(InsertSection(&ret, i, j, k, bGap));
                    if (!ret)
                    {
                        CHKRESULT(DoFunc());

                        InitParam(i, j ,k);

                        CHKRESULT(InsertChannel(&ret, i, j, k));
                        CHKRESULT(InsertSection(&ret, i, j, k, TRUE));
                    }
  
                    dEnp = pSectionData->dEnp;
                    m_nStopBand = i;
                    m_nStopChannel = j;
                    m_nStopSection = k;
                }
            }
			if (m_bMT8820)
			{
				CHKRESULT(DoFunc());
                InitParam(i, m_nStopChannel, m_nStopSection);
			}
        }
        CHKRESULT(DoFunc());

        m_uRetryTime++;

    } while (!m_bResult);

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CApiTxV21::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pIApi = (IApi*)this;

    try
    {
        if (m_pPhoneParam == NULL)
        {
            m_pPhoneParam = new PC_LTE_FDT_TX_T();
        }

        if (m_pTesterParam == NULL)
        {
            m_pTesterParam = new RF_LTE_CAL_TX_V1_REQ_T();
        }

        if (m_pTesterResult == NULL)
        {
            m_pTesterResult = new RF_LTE_CAL_TX_V1_RLT_T();
        }
    }
    catch (const std::bad_alloc& /*e*/)
    {
        assert(0);
        return SP_E_SPAT_ALLOC_MEMORY;
    }
   
    CHKRESULT(GetMaxPointsNumber());
    m_pCurrentCa = (LTE_CA_E*)GetFunc(CURRENT_CA);

    return SP_OK;
}

SPRESULT CApiTxV21::GetMaxPointsNumber()
{
    return SP_OK;
}

SPRESULT CApiTxV21::InsertSection( BOOL* ret, int nBand, int nChannel, int nSection, BOOL bGap )
{
    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];
    SectionData* pSectionData = &pChannelData->arrSection[nSection];

    uint16 usAPT = pSectionData->usApt;
    uint16 usPaMode = pSectionData->usPaMode;
    uint16 usBeginWord = pSectionData->usBeginWord + pSectionData->sWordOffset;
    double dEnp = pSectionData->dEnp;
    uint16 usStepNum = (uint16)pSectionData->arrPower.size();

    if (m_nPointNumber + usStepNum + bGap > m_nMaxPointNumber)
    {
        *ret = FALSE;
        return SP_OK;
    }

    int nChannelIndex = m_pPhoneParam->nChanNum - 1;
    int nSectionIndex = m_pPhoneParam->ArfcnGroup[nChannelIndex].nTxArryNum;

    //phone
    m_pPhoneParam->ArfcnGroup[nChannelIndex].TX_config[nSectionIndex].nApt_value     = usAPT;
    m_pPhoneParam->ArfcnGroup[nChannelIndex].TX_config[nSectionIndex].nPa_mode       = usPaMode;
    m_pPhoneParam->ArfcnGroup[nChannelIndex].TX_config[nSectionIndex].nStartFactor   = usBeginWord;
    m_pPhoneParam->ArfcnGroup[nChannelIndex].TX_config[nSectionIndex].nTxNum         = usStepNum;
    m_pPhoneParam->ArfcnGroup[nChannelIndex].nTxArryNum++;

    //instrument
    RF_LTE_CAL_TX_V1_REQ_SECTION_T InsSection;
    InsSection.bGap = bGap;
    InsSection.dEnp = dEnp;
    InsSection.nStepCount = usStepNum;
    m_arrInsChannel[nChannelIndex].usSectionCount++;
    m_arrInsSection.push_back(InsSection);
    m_pTesterParam->nSectionCount++;

    m_nPointNumber += usStepNum + bGap;

    return SP_OK;
}

SPRESULT CApiTxV21::InsertChannel( BOOL* ret, int nBand, int nChannel, int nSection )
{
    if (m_pPhoneParam->nChanNum >= MAX_PC_LTE_APC_CHANNEL_NUM)
    {
        *ret = FALSE;
        return SP_OK;
    }

    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];
    SectionData* pSectionData = &pChannelData->arrSection[nSection];

    if (m_pPhoneParam->nChanNum == 0)
    {
        InsertTrigger(nBand, nChannel);
    }

    uint16 usStepNum = (uint16)pSectionData->arrPower.size();

    if (m_nPointNumber + usStepNum + m_bChannelGap > m_nMaxPointNumber)
    {
        *ret = FALSE;
        return SP_OK;
    }

    uint32 Arfcn = pChannelData->usArfcn;
    uint8 Indicator = (uint8)pBandData->nIndicator;

    int nChannelIndex = m_pPhoneParam->nChanNum;

    //phone
    m_pPhoneParam->ArfcnGroup[nChannelIndex].nArfcn = (uint16)Arfcn;
    m_pPhoneParam->BandIndicator[nChannelIndex] = Indicator;
    m_pPhoneParam->nChanNum++;

    //instrument
    RF_LTE_CAL_TX_V1_REQ_CHANNEL_T InsChannel;
    InsChannel.usArfcn = Arfcn;
    InsChannel.usSectionCount = 0;
    m_arrInsChannel.push_back(InsChannel);
    m_pTesterParam->nChannelCount++;

    *ret = TRUE;
    return SP_OK;
}

SPRESULT CApiTxV21::DoFunc()
{
    IS_USER_STOP_FUNC;

    if (m_nPointNumber == 0)
    {
        return SP_OK;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    int nStartIndex = m_bMT8820? 1:0;
    m_pTesterParam->arrChannel = &m_arrInsChannel[nStartIndex];
    m_pTesterParam->arrSection = &m_arrInsSection[nStartIndex];
    m_pTesterParam->nChannelCount -= nStartIndex;
    m_pTesterParam->nSectionCount -= nStartIndex;

    RF_LTE_PARAM param;
    param.SubItem = STI_LTE_TX_V1;
    param.pParam = m_pTesterParam;

    CHKRESULT(SetRfSwitch(CLteUtility::GetBand(m_pTesterParam->arrChannel[0].usArfcn), m_pTesterParam->TxAnt, RF_ANT_INVALID));

	CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->InitTest(TI_FDT, &param), "Instrument::InitTest()!");

	CHKRESULT_WITH_NOTIFY_LTE_ITEM(SP_lteApc(m_hDUT, m_pPhoneParam), "SP_lteApc()!");

    param.pParam = m_pTesterResult;
	CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->FetchResult(TI_FDT, &param), "Instrument::FetchResult()!");

    GetData();

    if (m_pCurrentCaller != NULL && DealwithData != NULL)
    {   
        CHKRESULT(DealwithData(
            m_pCurrentCaller
            , &m_bResult
            , m_nStartBand
            , m_nStartChannel
            , m_nStartSection
            , m_nStopBand
            , m_nStopChannel
            , m_nStopSection
            , m_uRetryTime 
            ));
    }

    CHKRESULT(ResetRfSwitch());

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

void CApiTxV21::GetData()
{
    int nPowerIndex = 1;

    for (uint32 i = m_nStartBand; i <= m_nStopBand; i++)
    {   
        BandData* pBandData = &(*m_parrBandData)[i];

        uint32 nTempStartChannel = 0;
        uint32 nTempStopChannel = pBandData->arrChannel.size() - 1;

        if (i == m_nStartBand) { nTempStartChannel = m_nStartChannel; }
        if (i == m_nStopBand) { nTempStopChannel = m_nStopChannel; }

        for (uint32 j = nTempStartChannel; j <= nTempStopChannel; j++)
        {
            ChannelData* pChannelData = &pBandData->arrChannel[j];

            uint32 nTempStartSection = 0;
            uint32 nTempStopSection = pChannelData->arrSection.size() - 1;

            if (i == m_nStartBand && j == m_nStartChannel){ nTempStartSection = m_nStartSection; }
            if (i == m_nStopBand && j == m_nStopChannel) { nTempStopSection = m_nStopSection; }
            for (uint32 k = nTempStartSection; k <= nTempStopSection; k++)
            {
                SectionData* pSectionData = &pChannelData->arrSection[k];

                if (pSectionData->usDone)
                {
                    if (m_bMT8820)
                    {
                        nPowerIndex += pSectionData->arrPower.size();
                    }
                    continue;
                }

                for (uint32 l = 0; l < pSectionData->arrPower.size(); l++)
                {
                    pSectionData->arrPower[l] = m_pTesterResult->arrPower[nPowerIndex++];
                }
            }
        }
    }
}

void CApiTxV21::InitParam( int nBand, int nChannel, int nSection )
{
    ZeroMemory(m_pPhoneParam, sizeof(PC_LTE_FDT_TX_T));
    ZeroMemory(m_pTesterParam, sizeof(RF_LTE_CAL_TX_V1_REQ_T));
    ZeroMemory(m_pTesterResult, sizeof(RF_LTE_CAL_TX_V1_RLT_T));

    m_arrInsChannel.clear();
    m_arrInsSection.clear();

    m_pTesterResult->pReq = m_pTesterParam;

    m_pPhoneParam->eBW  = LTE_BW_5M;
    m_pPhoneParam->nFactorStep = 8;
    m_pPhoneParam->nAfcVal = m_Afc;

    m_pTesterParam->TxAnt = m_Ant;
    m_pTesterParam->Filter = 50;
    m_pTesterParam->nStepLength = 1;
    m_pTesterParam->nMeasureLength = 400;
    m_pTesterParam->nMeasureOffset = 400;
    m_pTesterParam->nTriggerOffset = 0;
    m_pTesterParam->nTriggerThreshold = -30;
    m_pTesterParam->nTriggerTimeout = 1000;
	m_pTesterParam->bChRevert = FALSE;

    if (m_bMT8820)
    {
        m_pTesterParam->nTriggerOffset = 2000;
    }

    m_nPointNumber = 0;

    m_nStartBand = nBand;
    m_nStartChannel = nChannel;
    m_nStartSection = nSection;

    m_nStopBand = nBand;
    m_nStopChannel = nChannel;
    m_nStopSection = nSection;
}

void CApiTxV21::ClearData()
{
    for (uint32 i = 0; i < m_parrBandData->size(); i++)
    {   
        BandData* pBandData = &(*m_parrBandData)[i];

        for (uint32 j = 0; j < pBandData->arrChannel.size(); j++)
        {
            ChannelData* pChannelData = &pBandData->arrChannel[j];

            for (uint32 k = 0; k < pChannelData->arrSection.size(); k++)
            {
                SectionData* pSectionData = &pChannelData->arrSection[k];

                pSectionData->usDone = FALSE;
                pSectionData->sWordOffset = 0;
                
                for (uint32 l = 0; l < pSectionData->arrPower.size(); l++)
                {
                    pSectionData->arrPower[l] = 0;
                }
            }
        }
    }
}

SPRESULT CApiTxV21::InsertTrigger( int nBand, int nChannel )
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];

    //phone
    m_pPhoneParam->ArfcnGroup[0].nArfcn = (uint16)pChannelData->usArfcn;
    m_pPhoneParam->BandIndicator[0] = (uint8)pBandData->nIndicator;
    m_pPhoneParam->nChanNum = 1;

    m_pPhoneParam->ArfcnGroup[0].TX_config[0].nApt_value     = 35;
    m_pPhoneParam->ArfcnGroup[0].TX_config[0].nPa_mode       = 0;
    m_pPhoneParam->ArfcnGroup[0].TX_config[0].nStartFactor   = 0;
    m_pPhoneParam->ArfcnGroup[0].TX_config[0].nTxNum         = 1;
    m_pPhoneParam->ArfcnGroup[0].nTxArryNum = 1;

    //instrument
    RF_LTE_CAL_TX_V1_REQ_CHANNEL_T InsChannel;
    InsChannel.usArfcn = pChannelData->usArfcn;
    InsChannel.usSectionCount = 1;
    m_arrInsChannel.push_back(InsChannel);
    m_pTesterParam->nChannelCount = 1;

    RF_LTE_CAL_TX_V1_REQ_SECTION_T InsSection;
    InsSection.bGap = FALSE;
    InsSection.dEnp = 25;
    InsSection.nStepCount = 1;
    m_arrInsSection.push_back(InsSection);
    m_pTesterParam->nSectionCount = 1;

    m_nPointNumber = 1;

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CApiTxV21::SwitchCa()
{
    if (*m_pCurrentCa == m_Ca)
    {
        return SP_OK;
    }

    *m_pCurrentCa = m_Ca;

    PC_LTE_SWITCH_REQ_T LteSwitch;
    PC_LTE_SWITCH_CA_T LteSwitchCa;

    LteSwitch.type = LTE_SWITCH_CA;
    LteSwitch.size = sizeof(PC_LTE_SWITCH_CA_T);
    LteSwitchCa.Ca = m_Ca;

    SPRESULT Result = SP_lteSwitch(m_hDUT, &LteSwitch, &LteSwitchCa);
    if (Result != SP_OK)
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s: SP_lteSwitch fail.", __FUNCTION__);
		CHKRESULT_WITH_NOTIFY_LTE(Result);
    }

    return Result;
}
