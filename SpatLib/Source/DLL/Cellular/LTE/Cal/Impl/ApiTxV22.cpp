#include "StdAfx.h"
#include "ApiTxV22.h"
#include "math.h"

CApiTxV22::CApiTxV22( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CApiBase(lpName, pFuncCenter)
{
    AddFunc(lpName, (IApiTxV2*)this);

    m_pPhoneParam = NULL;
    m_pTesterParam = NULL;
    m_pTesterResult = NULL;

    m_uMaxSize = 3400;
    m_nMaxPointNumber = 1000;
    m_bChannelGap = 1;

	m_nStartBand = 0;
	m_nStopBand = 0;
	m_nStartChannel = 0;
	m_nStopChannel = 0;
	m_nStartSection = 0;
	m_nStopSection = 0;

	m_nPointNumber = 0;
	m_bResult = FALSE;
	m_uRetryTime = 0;
	m_uTxSize = 0;

	m_arrPhoneChannel.clear();
}

CApiTxV22::~CApiTxV22(void)
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

SPRESULT CApiTxV22::Run()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    BOOL ret;
    m_uRetryTime = 0;

    do 
    {
        LogFmtStrA(SPLOGLV_INFO, "%s: Time = %d", __FUNCTION__, m_uRetryTime);

        InitParam(0, 0, 0);
        m_bResult = TRUE;

        for (uint32 i = 0; i < m_parrBandData->size(); i++)
        {   
            BandData* pBandData = &(*m_parrBandData)[i];

            for (uint32 j = 0; j < pBandData->arrChannel.size(); j++)
            {
                ChannelData* pChannelData = &pBandData->arrChannel[j];
                BOOL bNewChannel = TRUE;
                double dEnp = 0;

                for (uint32 k = 0; k < pChannelData->arrSection.size(); k++)
                {
                    SectionData* pSectionData = &pChannelData->arrSection[k];

                    if (pSectionData->usDone)
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
        }
        CHKRESULT(DoFunc());

        m_uRetryTime++;

    } while (!m_bResult);

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CApiTxV22::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pIApi = (IApi*)this;

    if (m_pPhoneParam == NULL)
    {
        m_pPhoneParam = new PC_LTE_APC_V3_T();
    }

    if (m_pTesterParam == NULL)
    {
        m_pTesterParam = new RF_LTE_CAL_TX_V1_REQ_T();
    }

    if (m_pTesterResult == NULL)
    {
        m_pTesterResult = new RF_LTE_CAL_TX_V1_RLT_T();
    }

    CHKRESULT(GetMaxPointsNumber());

    return SP_OK;
}

SPRESULT CApiTxV22::GetMaxPointsNumber()
{
    return SP_OK;
}

SPRESULT CApiTxV22::InsertSection( BOOL* ret, int nBand, int nChannel, int nSection, BOOL bGap )
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

    if (m_uTxSize + sizeof(PC_LTE_APC_V2_APT_T) > m_uMaxSize)
    {
        *ret = FALSE;
        return SP_OK;
    }

    int nChannelIndex = m_pPhoneParam->Header.ChannelCount - 1;
    
    //phone
    PC_LTE_APC_V2_APT_T PhoneApt;
    PhoneApt.BeginWord = usBeginWord;
    PhoneApt.PaMode = usPaMode;
    PhoneApt.Voltage = usAPT;
    PhoneApt.StepCount = (uint8)usStepNum;
    PhoneApt.StepLength = 8;

    m_arrPhoneApt.push_back(PhoneApt);
    m_arrPhoneChannel[nChannelIndex].AptCount++;
    m_pPhoneParam->Header.AptCount++;

    m_uTxSize += sizeof(PC_LTE_APC_V2_APT_T);

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

SPRESULT CApiTxV22::InsertChannel( BOOL* ret, int nBand, int nChannel, int nSection )
{
    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];
    SectionData* pSectionData = &pChannelData->arrSection[nSection];

    if (m_pPhoneParam->Header.ChannelCount == 0)
    {
        InsertTrigger(nBand, nChannel);
    }

    uint16 usStepNum = (uint16)pSectionData->arrPower.size();

    if (m_nPointNumber + usStepNum + m_bChannelGap > m_nMaxPointNumber)
    {
        *ret = FALSE;
        return SP_OK;
    }

    if (m_uTxSize + sizeof(PC_LTE_APC_V3_CHANNEL_T) + sizeof(PC_LTE_APC_V2_APT_T) > m_uMaxSize)
    {
        *ret = FALSE;
        return SP_OK;
    }

    //phone
    PC_LTE_APC_V3_CHANNEL_T PhoneChannel;
	ZeroMemory(&PhoneChannel, sizeof(PC_LTE_APC_V3_CHANNEL_T));
    PhoneChannel.usArfcn = (uint32)pChannelData->usArfcn;
    PhoneChannel.Indicator = (uint8)pBandData->nIndicator;
    PhoneChannel.AptCount = 0;
    m_arrPhoneChannel.push_back(PhoneChannel);
    m_pPhoneParam->Header.ChannelCount++;

    m_uTxSize += sizeof(PC_LTE_APC_V3_CHANNEL_T);

    //instrument
    RF_LTE_CAL_TX_V1_REQ_CHANNEL_T InsChannel;
    InsChannel.usArfcn = pChannelData->usArfcn;
    InsChannel.usSectionCount = 0;
    m_arrInsChannel.push_back(InsChannel);
    m_pTesterParam->nChannelCount++;

    *ret = TRUE;
    return SP_OK;
}

SPRESULT CApiTxV22::DoFunc()
{
    IS_USER_STOP_FUNC;

    if (m_nPointNumber == 0)
    {
        return SP_OK;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_pPhoneParam->Header.AptOffset = (uint16)(m_pPhoneParam->Header.ChannelOffset + (m_pPhoneParam->Header.ChannelSize * m_pPhoneParam->Header.ChannelCount));

    m_pPhoneParam->pChannel = &m_arrPhoneChannel[0];
    m_pPhoneParam->pApt = &m_arrPhoneApt[0];

    m_pTesterParam->arrChannel = &m_arrInsChannel[0];
    m_pTesterParam->arrSection = &m_arrInsSection[0];

    RF_LTE_PARAM param;
    param.SubItem = STI_LTE_TX_V1;
    param.pParam = m_pTesterParam;

    CHKRESULT(SetRfSwitch(CLteUtility::GetBand(m_pTesterParam->arrChannel[0].usArfcn), m_pTesterParam->TxAnt, RF_ANT_INVALID));

	CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->InitTest(TI_FDT, &param), "Instrument::InitTest()!");

	CHKRESULT_WITH_NOTIFY_LTE_ITEM(SP_lteApc_V4(m_hDUT, m_pPhoneParam, TRUE), "SP_lteApc_V2()!");

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

void CApiTxV22::GetData()
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

void CApiTxV22::InitParam( int nBand, int nChannel, int nSection )
{
    ZeroMemory(m_pPhoneParam, sizeof(PC_LTE_APC_V2_T));
    ZeroMemory(m_pTesterParam, sizeof(RF_LTE_CAL_TX_V1_REQ_T));
    ZeroMemory(m_pTesterResult, sizeof(RF_LTE_CAL_TX_V1_RLT_T));

    m_arrPhoneChannel.clear();
    m_arrPhoneApt.clear();

    m_arrInsChannel.clear();
    m_arrInsSection.clear();

    m_pTesterResult->pReq = m_pTesterParam;

    m_pPhoneParam->Header.Bw = LTE_BW_5M;
    m_pPhoneParam->Header.Ant = (uint8)m_Ant;
    m_pPhoneParam->Header.Ca = (uint8)m_Ca;

    m_pPhoneParam->Header.HeaderSize = sizeof(PC_LTE_APC_V2_HEADER_T);
    m_pPhoneParam->Header.AptSize = sizeof(PC_LTE_APC_V2_APT_T);
    m_pPhoneParam->Header.ChannelSize = sizeof(PC_LTE_APC_V3_CHANNEL_T);
    m_pPhoneParam->Header.ChannelOffset = 16 + sizeof(PC_LTE_APC_V2_HEADER_T);

    m_pTesterParam->TxAnt = m_Ant;
    m_pTesterParam->Filter = 50;
    m_pTesterParam->nStepLength = 1;
    m_pTesterParam->nMeasureLength = 400;
    m_pTesterParam->nMeasureOffset = 400;
    m_pTesterParam->nTriggerOffset = 0;
    m_pTesterParam->nTriggerThreshold = -30;
    m_pTesterParam->nTriggerTimeout = 1000;
	m_pTesterParam->bChRevert = FALSE;

    m_nPointNumber = 0;

    m_nStartBand = nBand;
    m_nStartChannel = nChannel;
    m_nStartSection = nSection;

    m_nStopBand = nBand;
    m_nStopChannel = nChannel;
    m_nStopSection = nSection;

    m_uTxSize = 16 + sizeof(PC_LTE_APC_V2_HEADER_T);
}

void CApiTxV22::ClearData()
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

SPRESULT CApiTxV22::InsertTrigger( int nBand, int nChannel )
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];

    //phone
    PC_LTE_APC_V3_CHANNEL_T PhoneChannel;
    PhoneChannel.usArfcn = (uint32)pChannelData->usArfcn;
    PhoneChannel.Indicator = (uint8)pBandData->nIndicator;
    PhoneChannel.AptCount = 1;
    m_arrPhoneChannel.push_back(PhoneChannel);
    m_pPhoneParam->Header.ChannelCount++;

    m_uTxSize += sizeof(PC_LTE_APC_V3_CHANNEL_T);

    PC_LTE_APC_V2_APT_T PhoneApt;
    PhoneApt.BeginWord = 0;
    PhoneApt.PaMode = 0;
    PhoneApt.Voltage = 35;
    PhoneApt.StepCount = (uint8)1;
    PhoneApt.StepLength = 8;

    m_arrPhoneApt.push_back(PhoneApt);
   // m_arrPhoneChannel[0].AptCount++;
    m_pPhoneParam->Header.AptCount++;

    m_uTxSize += sizeof(PC_LTE_APC_V2_APT_T);

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
