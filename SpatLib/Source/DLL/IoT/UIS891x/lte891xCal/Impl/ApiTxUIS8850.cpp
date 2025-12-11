#include "StdAfx.h"
#include "ApiTxUIS8850.h"
#include "LteUtility.h"
#include "math.h"

CApiTxUIS8850::CApiTxUIS8850( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CApiBase(lpName, pFuncCenter)
{
    AddFunc(lpName, (IApiTx*)this);

    m_pPhoneParam = NULL;
    m_pTesterParam = NULL;
    m_pTesterResult = NULL;

    m_uMaxSize = 3400;
    m_nMaxPointNumber = 1000;
    m_bChannelGap = 1;
	m_bLinteger = FALSE;
	//MT8820
	m_bMT8820 = FALSE;

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
    m_UeType = 0;
}

CApiTxUIS8850::~CApiTxUIS8850(void)
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

SPRESULT CApiTxUIS8850::Run()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    BOOL ret;
    m_uRetryTime = 0;
	//CHKRESULT(m_pRFTester->GetProperty(DP_TXRX_SAMEPOINT, 0, &m_bMT8820));

    do 
    {
        LogFmtStrA(SPLOGLV_INFO, "%s: Time = %d", __FUNCTION__, m_uRetryTime);

        InitParam(0, 0, 0);
        m_bResult = TRUE;
        uint32 iArrBandDataSize = m_parrBandData->size();
        for (uint32 i = 0; i < iArrBandDataSize; i++)
        {   
            LogFmtStrA(SPLOGLV_INFO, "%s: Band", __FUNCTION__);
            BandData* pBandData = &(*m_parrBandData)[i];
			if (m_bMT8820)
			{
				InitParam(i, 0 , 0);
			}
            uint32 iArrChannelSize = pBandData->arrChannel.size();
            for (uint32 j = 0; j < iArrChannelSize; j++)
            {
                LogFmtStrA(SPLOGLV_INFO, "%s: Chan", __FUNCTION__);
                ChannelData* pChannelData = &pBandData->arrChannel[j];
                BOOL bNewChannel = TRUE;
                double dEnp = 0;
                uint32 iArrSectionSize = pChannelData->arrSection.size();
                for (uint32 k = 0; k < iArrSectionSize; k++)
                {
                    LogFmtStrA(SPLOGLV_INFO, "%s: Section", __FUNCTION__);
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

SPRESULT CApiTxUIS8850::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pIApi = (IApi*)this;

    if (m_pPhoneParam == NULL)
    {
        m_pPhoneParam = new PC_LTE_APC_UIS8910_T();
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

SPRESULT CApiTxUIS8850::GetMaxPointsNumber()
{
    return SP_OK;
}

SPRESULT CApiTxUIS8850::InsertSection( BOOL* ret, int nBand, int nChannel, int nSection, BOOL bGap )
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];
    SectionData* pSectionData = &pChannelData->arrSection[nSection];

    uint16 usAPT = pSectionData->usApt;
    uint16 usPaMode = pSectionData->usPaMode;
    uint16 usBeginWord = pSectionData->usBeginWord + pSectionData->sWordOffset;
    double dEnp = pSectionData->dEnp;
    uint16 usStepNum = (uint16)pSectionData->arrPower.size();
    uint16 usStepLength = pSectionData->usWordStep;
    uint16 usGapNum = 0;

    if (TRUE == bGap)
    {
        if(0x8850 == m_UeType)
        {
            usGapNum = 2;
        }
        else
        {
            usGapNum = 1;
        }
    }
    else
    {
        usGapNum = 0;
    }

    if (m_nPointNumber + usStepNum + usGapNum > m_nMaxPointNumber)
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
    PhoneApt.StepLength = usStepLength;

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

    m_nPointNumber += usStepNum + usGapNum;
    //LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
    return SP_OK;
}

SPRESULT CApiTxUIS8850::InsertChannel( BOOL* ret, int nBand, int nChannel, int nSection )
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];
    SectionData* pSectionData = &pChannelData->arrSection[nSection];

	if (pChannelData->usArfcn > 65535)
	{
		m_bLinteger = TRUE;
	}
	
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

    if (MAX_PC_LTE_AGC_CHANNEL_NUM-1 == m_pTesterParam->nChannelCount)
	{
	//	LogFmtStrA(SPLOGLV_ERROR, "%s Exception 1, channel count %d", __FUNCTION__, m_pTesterParam->nChannelCount);
		*ret = FALSE;
		return SP_OK;
	}

    //phone
    PC_LTE_APC_V3_CHANNEL_T PhoneChannel;
    //PhoneChannel.usArfcn = pChannelData->usArfcn;
	PhoneChannel.usArfcn = (uint32)CLteUtility::GetDlCHannel(pChannelData->usArfcn);
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
    //LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
    return SP_OK;
}

SPRESULT CApiTxUIS8850::DoFunc()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
    IS_USER_STOP_FUNC;

    if (m_nPointNumber == 0)
    {
        return SP_OK;
    }

	//Sleep(1000);  //chengui

    BandData* pBandData = &(*m_parrBandData)[m_nStartBand];
    SPRESULT Result;
	PC_LTE_APC_UIS8910_T* pParam = NULL;

	CHKRESULT(SetApcParam());

	pParam = (PC_LTE_APC_UIS8910_T*)m_pPhoneParam;

	int nStartIndex = m_bMT8820? 1:0;
	m_pTesterParam->arrChannel = &m_arrInsChannel[nStartIndex];
	m_pTesterParam->arrSection = &m_arrInsSection[nStartIndex];
	m_pTesterParam->nChannelCount -= nStartIndex;
	m_pTesterParam->nSectionCount -= nStartIndex;

    RF_LTE_PARAM param;
    param.SubItem = STI_LTE_TX_V1;
    param.pParam = m_pTesterParam;

    Result =  m_pRFTester->InitTest(TI_FDT, &param);
    if (Result != SP_OK)
    {
        LogRawStrA(SPLOGLV_ERROR, "Lte Cali CApiTxV4 RF InitTest failed");
        CHKRESULT_WITH_NOTIFY_LTE_ITEM(Result, "Instrument::InitTest()!");
    }

    //Result = SP_lteApc_UIS8910(m_hDUT, pParam, m_bLinteger);
	Result = SP_lteApc_UIS8910(m_hDUT, pParam);
    if (Result != SP_OK)
    {
        LogRawStrA(SPLOGLV_ERROR, "SP_lteApc_UIS8910 failed");
        CHKRESULT_WITH_NOTIFY_LTE_ITEM(Result, "SP_lteApc_UIS8910()!");
    }

    param.pParam = m_pTesterResult;
    Result = m_pRFTester->FetchResult(TI_FDT, &param);
    if (Result == SP_E_RF_TIME_OUT)
    {
        LogRawStrA(SPLOGLV_ERROR, "Lte Apc Line Cali Timeout");
        SetRepairBand(CLteUtility::m_BandInfo[pBandData->nBand].pRepairBand);
        CHKRESULT_WITH_NOTIFY_LTE_ITEM_BAND(Result, "TX APC Instrument::FetchResult()!", CLteUtility::m_BandInfo[pBandData->nBand].NameA);
    }

    GetData();

    if (m_pCurrentCaller != NULL && DealwithData != NULL)
    {   
        TxSetPara Para;
        Para.nStartBand = m_nStartBand;
        Para.nStartChannel = m_nStartChannel;
        Para.nStartSection = m_nStartSection;
        Para.nStopBand = m_nStopBand;
        Para.nStopChannel = m_nStopChannel;
        Para.nStopSection = m_nStopSection;

        CHKRESULT(DealwithData(
            m_pCurrentCaller
            , &m_bResult
            , Para
            , m_uRetryTime 
            ));
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

void CApiTxUIS8850::GetData()
{
    int nPowerIndex = 1;

    for (uint32 i = m_nStartBand; i <= m_nStopBand; i++)
    {   
        BandData* pBandData = &(*m_parrBandData)[i];

        uint32 nTempStartChannel = 0;
        uint32 nTempStopChannel = pBandData->arrChannel.size();

        if (i == m_nStartBand) { nTempStartChannel = m_nStartChannel; }
        if (i == m_nStopBand) { nTempStopChannel = m_nStopChannel + 1; }

        for (uint32 j = nTempStartChannel; j < nTempStopChannel; j++)
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
                uint32 iArrPowerSize = pSectionData->arrPower.size();
                for (uint32 l = 0; l < iArrPowerSize; l++)
                {
                    pSectionData->arrPower[l] = m_pTesterResult->arrPower[nPowerIndex++];
                }
            }
        }
    }
}

void CApiTxUIS8850::InitParam( int nBand, int nChannel, int nSection )
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
    ZeroMemory(m_pPhoneParam, sizeof(PC_LTE_APC_UIS8910_T));
    ZeroMemory(m_pTesterParam, sizeof(RF_LTE_CAL_TX_V1_REQ_T));
    ZeroMemory(m_pTesterResult, sizeof(RF_LTE_CAL_TX_V1_RLT_T));

    m_arrPhoneChannel.clear();
    m_arrPhoneApt.clear();

    m_arrInsChannel.clear();
    m_arrInsSection.clear();

    m_pTesterResult->pReq = m_pTesterParam;

	//V3
    if (0x8850 == m_UeType)//10MHz
    {
        m_pPhoneParam->Header.Bw = LTE_BW_10M;
        //m_pPhoneParam->Header.Bw = LTE_BW_1_4M;
    }
    else//1.4MHz
    {
        m_pPhoneParam->Header.Bw = LTE_BW_1_4M;  //EMTC
    }
    m_pPhoneParam->Header.Ant = (uint8)(m_Ant % 2);
    m_pPhoneParam->Header.Ca = (uint8)m_Ca;

    m_pPhoneParam->Header.HeaderSize = sizeof(PC_LTE_APC_UIS8910_HEADER_T);
    m_pPhoneParam->Header.AptSize = sizeof(PC_LTE_APC_V2_APT_T);
    m_pPhoneParam->Header.ChannelSize = sizeof(PC_LTE_APC_V3_CHANNEL_T);
    m_pPhoneParam->Header.ChannelOffset = 16 + sizeof(PC_LTE_APC_UIS8910_HEADER_T);
	
    m_pTesterParam->TxAnt = m_Ant;
    if (0x8850 == m_UeType)//10MHz
    {
        m_pTesterParam->Filter = 100; 
        //m_pTesterParam->Filter = 14;
    }
    else//1.4MHz
    {
        m_pTesterParam->Filter = 14; //50;  //EMTC
    }

    m_pTesterParam->nStepLength = 1;
    m_pTesterParam->nMeasureLength = 400;
    m_pTesterParam->nMeasureOffset = 400;
    m_pTesterParam->nTriggerOffset = 0;
	if (m_bMT8820)
	{
		m_pTesterParam->nTriggerOffset = 2000;
	}
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

    m_uTxSize = 16 + sizeof(PC_LTE_APC_UIS8910_HEADER_T);
	m_bLinteger = FALSE;
    //LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
}

void CApiTxUIS8850::ClearData()
{
    uint32 iArrBandDataSize = m_parrBandData->size();
    for (uint32 i = 0; i < iArrBandDataSize; i++)
    {   
        BandData* pBandData = &(*m_parrBandData)[i];
        uint32 iArrChannelSize = pBandData->arrChannel.size();
        for (uint32 j = 0; j < iArrChannelSize; j++)
        {
            ChannelData* pChannelData = &pBandData->arrChannel[j];
            uint32 iArrSectionSize = pChannelData->arrSection.size();
            for (uint32 k = 0; k < iArrSectionSize; k++)
            {
                SectionData* pSectionData = &pChannelData->arrSection[k];

                pSectionData->usDone = FALSE;
                pSectionData->sWordOffset = 0;
                uint32 iArrPowerSize = pSectionData->arrPower.size();
                for (uint32 l = 0; l < iArrPowerSize; l++)
                {
                    pSectionData->arrPower[l] = 0;
                }
            }
        }
    }
}

SPRESULT CApiTxUIS8850::InsertTrigger( int nBand, int nChannel )
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];

    //phone
    PC_LTE_APC_V3_CHANNEL_T PhoneChannel;
    //PhoneChannel.usArfcn = pChannelData->usArfcn;
	PhoneChannel.usArfcn = CLteUtility::GetDlCHannel( pChannelData->usArfcn);
    PhoneChannel.Indicator = (uint8)pBandData->nIndicator;
    PhoneChannel.AptCount = 1;
    PhoneChannel.Reserved[0] = 0;
    PhoneChannel.Reserved[1] = 0;
    m_arrPhoneChannel.push_back(PhoneChannel);
    m_pPhoneParam->Header.ChannelCount++;
	 
    m_uTxSize += sizeof(PC_LTE_APC_V3_CHANNEL_T);

    PC_LTE_APC_V2_APT_T PhoneApt;
    PhoneApt.BeginWord = 0;
    PhoneApt.PaMode = 0;
    PhoneApt.Voltage = 0;
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

SPRESULT CApiTxUIS8850::SetApcParam()
{
	//V3
	m_pPhoneParam->Header.AptOffset = (uint16)(m_pPhoneParam->Header.ChannelOffset + (m_pPhoneParam->Header.ChannelSize * m_pPhoneParam->Header.ChannelCount));

	m_pPhoneParam->pChannel = &m_arrPhoneChannel[0];
	m_pPhoneParam->pApt = &m_arrPhoneApt[0];

	return SP_OK;
}
