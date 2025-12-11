#include "StdAfx.h"
#include "ApiTxModV3.h"
#include "math.h"
#include "LteUtility.h"

CApiTxModV3::CApiTxModV3( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CApiBase(lpName, pFuncCenter)
{
    AddFunc(lpName, (IApiTxModV3*)this);

    m_pPhoneParam = NULL;
    m_pTesterParam = NULL;
    m_pTesterResult = NULL;
	m_pPhoneRet = NULL;
    m_uMaxSize = 3400;
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
	m_bResult = FALSE;
	m_uRetryTime = 0;
	m_uTxSize = 0;
	m_unRangeCount = 3;

	m_TxIns = 0;
	m_RxIns = 0;
	m_bBandAdaptation = FALSE;
}

CApiTxModV3::~CApiTxModV3(void)
{
    if (m_pPhoneParam != NULL)
    {
        delete m_pPhoneParam;
        m_pPhoneParam = NULL;
    }

	if (m_pPhoneRet != NULL)
	{
		delete m_pPhoneRet;
		m_pPhoneRet = NULL;
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

SPRESULT CApiTxModV3::Run()
{
	LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

	BOOL ret;
	m_uRetryTime = 0;
	CHKRESULT(m_pRFTester->GetProperty(DP_TXRX_SAMEPOINT, 0, &m_bMT8820));

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
			uint32 iChannelsize = pBandData->arrChannel.size();
			//if(( 39 == iChannelsize) || (38 == iChannelsize))
			//{
			//	iChannelsize = iChannelsize - 12;
			//}
			for (uint32 j = 0; j < iChannelsize; j++)
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

SPRESULT CApiTxModV3::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pIApi = (IApi*)this;

    if (m_pPhoneParam == NULL)
    {
        m_pPhoneParam = new PC_MODEM_RF_V3_LTE_APC_REQ_CMD_T();
    }

	if (m_pPhoneRet == NULL)
	{
		m_pPhoneRet = new PC_MODEM_RF_V3_LTE_APC_RSP_T();
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

SPRESULT CApiTxModV3::GetMaxPointsNumber()
{
    return SP_OK;
}

SPRESULT CApiTxModV3::InsertSection( BOOL* ret, int nBand, int nChannel, int nSection, BOOL bGap )
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

    if (m_uTxSize + sizeof(CALI_APC_APT_CONFIG_T) > m_uMaxSize)
    {
        *ret = FALSE;
        return SP_OK;
    }

    int nChannelIndex = m_pPhoneParam->stReqHeader.ApcRfChainCtrl->ArfcnNum - 1;
    
    //phone
    CALI_APC_APT_CONFIG_T PhoneApt;
    PhoneApt.BeginWord = usBeginWord;
    PhoneApt.PaMode = usPaMode;
    PhoneApt.APT = usAPT;
    PhoneApt.StepNum = (uint8)usStepNum;
    PhoneApt.StepLength = 8;
	PhoneApt.RbNum = (uint16)CLteUtility::nRbNumber[LTE_BW_5M][LTE_FULL_RB];
	PhoneApt.RbPosition = LTE_RB_POSITION_LOW;
	PhoneApt.PdetFlag = pSectionData->usPdet;

	m_arrPhoneApt.push_back(PhoneApt);
    m_arrPhoneChannel[nChannelIndex].AptNum++;
    m_pPhoneParam->nAptNum[m_RfChain]++;

	if(PhoneApt.PdetFlag)
	{
		m_pPhoneParam->stReqHeader.ApcRfChainCtrl[0].PdetNum += PhoneApt.StepNum*3;
	}
	

    m_uTxSize += sizeof(CALI_APC_APT_CONFIG_T);

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

SPRESULT CApiTxModV3::InsertChannel( BOOL* ret, int nBand, int nChannel, int nSection )
{
    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];
    SectionData* pSectionData = &pChannelData->arrSection[nSection];

	
    if (m_pPhoneParam->stReqHeader.ApcRfChainCtrl[m_RfChain].ArfcnNum == 0)
    {
        InsertTrigger(nBand, nChannel);
    }

    uint16 usStepNum = (uint16)pSectionData->arrPower.size();

    if (m_nPointNumber + usStepNum + m_bChannelGap > m_nMaxPointNumber)
    {
        *ret = FALSE;
        return SP_OK;
    }

    if (m_uTxSize + sizeof(CALI_APC_ARFCN_T) + sizeof(CALI_APC_APT_CONFIG_T) > m_uMaxSize)
    {
        *ret = FALSE;
        return SP_OK;
    }

    //phone
    CALI_APC_ARFCN_T PhoneChannel;
	ZeroMemory(&PhoneChannel, sizeof(CALI_APC_ARFCN_T));
    PhoneChannel.Arfcn = pChannelData->usArfcn;
	if(m_bBandAdaptation)
	{
		PhoneChannel.Band = CLteUtility::m_BandInfo[pBandData->nBand].BandIdent;
	}
    PhoneChannel.AptNum = 0;
    m_arrPhoneChannel.push_back(PhoneChannel);
	m_pPhoneParam->stReqHeader.ApcRfChainCtrl[0].ArfcnNum++;
    m_uTxSize += sizeof(CALI_APC_ARFCN_T);

    //instrument
    RF_LTE_CAL_TX_V1_REQ_CHANNEL_T InsChannel;
    InsChannel.usArfcn = pChannelData->usArfcn;
    InsChannel.usSectionCount = 0;
    m_arrInsChannel.push_back(InsChannel);
    m_pTesterParam->nChannelCount++;

    *ret = TRUE;
    return SP_OK;
}

SPRESULT CApiTxModV3::DoFunc()
{
    IS_USER_STOP_FUNC;

    if (m_nPointNumber == 0)
    {
        return SP_OK;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    SPRESULT Result;

	int nStartIndex = m_bMT8820? 1:0;
	m_pTesterParam->arrChannel = &m_arrInsChannel[nStartIndex];
	m_pTesterParam->arrSection = &m_arrInsSection[nStartIndex];
	m_pTesterParam->nChannelCount -= nStartIndex;
	m_pTesterParam->nSectionCount -= nStartIndex;

	uint32 nPdetRlstSize = 0;
	BOOL bPdet = (0 != m_pPhoneParam->stReqHeader.ApcRfChainCtrl[0].PdetNum);
	if(bPdet)
	{
		m_vecPdetRslt.resize(m_pPhoneParam->stReqHeader.ApcRfChainCtrl[0].PdetNum);
		m_pPhoneRet->pPdetRlst = &m_vecPdetRslt[0];
		nPdetRlstSize = m_vecPdetRslt.size() * sizeof(uint16);
	}

    RF_LTE_PARAM param;
    param.SubItem = STI_LTE_TX_V1;
    param.pParam = m_pTesterParam;

	m_pPhoneParam->pArfcnConfChian0 = &m_arrPhoneChannel[0];
	m_pPhoneParam->pAptConfChain0 = &m_arrPhoneApt[0];
	m_pPhoneParam->stReqHeader.ApcRfChainCtrl[0].AptOffset = 4 + sizeof(CALI_APC_CTRL_HEADER_T) 
		+ m_pPhoneParam->stReqHeader.ApcRfChainCtrl[0].ArfcnNum*sizeof(CALI_APC_ARFCN_T);
	m_pPhoneParam->stReqHeader.ApcRfChainCtrl[0].ArfcnOffset = 4 + sizeof(CALI_APC_CTRL_HEADER_T);
	Result =  m_pRFTester->InitTest(TI_FDT, &param);
	if (Result != SP_OK)
	{
		LogRawStrA(SPLOGLV_ERROR, "Lte Cali CApiTxV1 RF InitTest failed");
		CHKRESULT_WITH_NOTIFY_LTE_ITEM(Result, "Instrument::InitTest()!")
	}


    Result = SP_ModemV3_LTE_ApcCal(m_hDUT, m_pPhoneParam, bPdet, m_pPhoneRet, nPdetRlstSize);

    if (Result != SP_OK)
    {
        LogRawStrA(SPLOGLV_ERROR, "SP_ModemV3_LTE_ApcCal failed");
        CHKRESULT_WITH_NOTIFY_LTE_ITEM(Result, "SP_ModemV3_LTE_ApcCal()!")
    }

    param.pParam = m_pTesterResult;
    Result = m_pRFTester->FetchResult(TI_FDT, &param,4000);
    if (Result == SP_E_RF_TIME_OUT)
    {
        LogRawStrA(SPLOGLV_ERROR, "Lte Apc Line Cali Timeout");
        CHKRESULT_WITH_NOTIFY_LTE_ITEM(Result, "Instrument::FetchResult()!")
    }
	if (m_pPhoneRet->status != SP_OK)
	{
		LogRawStrA(SPLOGLV_ERROR, "Lte power detect cal Invalid Data");
		CHKRESULT_WITH_NOTIFY_LTE(Result);
	}

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

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

void CApiTxModV3::GetData()
{
    int nPowerIndex = 1;
	int nValueIndex = 0;

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
					PointData* pPointData = &pSectionData->arrPower[l];
					
					pPointData->dPower = m_pTesterResult->arrPower[nPowerIndex++];
        
					if((BOOL)pSectionData->usPdet)
					{
						for (uint32 l = 0; l < m_unRangeCount; l++)
						{
							pPointData->usValue[l] = m_vecPdetRslt[nValueIndex];
							pPointData->dLogValue[l] = 10 * log10((double)pPointData->usValue[l]);

							nValueIndex++;
						}
					}
                }
            }
        }
    }
}

void CApiTxModV3::InitParam( int nBand, int nChannel, int nSection )
{
    ZeroMemory(m_pPhoneParam, sizeof(PC_MODEM_RF_V3_LTE_APC_REQ_CMD_T));
    ZeroMemory(m_pTesterParam, sizeof(RF_LTE_CAL_TX_V1_REQ_T));
    ZeroMemory(m_pTesterResult, sizeof(RF_LTE_CAL_TX_V1_RLT_T));

    m_arrPhoneChannel.clear();
    m_arrPhoneApt.clear();

    m_arrInsChannel.clear();
    m_arrInsSection.clear();

    m_pTesterResult->pReq = m_pTesterParam;

	//V3
    m_pPhoneParam->stReqHeader.ApcRfChainCtrl[m_RfChain].BW = LTE_BW_5M;
    m_pPhoneParam->stReqHeader.ApcRfChainCtrl[m_RfChain].Ant = (uint8)(m_Ant % 2);
    m_pPhoneParam->stReqHeader.ApcRfChainCtrl[m_RfChain].RfChain = (uint8)m_RfChain;

    m_pPhoneParam->stReqHeader.HeaderSize = sizeof(CALI_APC_CTRL_HEADER_T);
    m_pPhoneParam->stReqHeader.AptSize = sizeof(CALI_APC_APT_CONFIG_T);
    m_pPhoneParam->stReqHeader.ArfcnSize = sizeof(CALI_APC_ARFCN_T);
	m_pPhoneParam->stReqHeader.CaFlag = NON_CA;

	m_pTesterParam->TxAnt = (RF_ANT_E)m_TxIns;
    m_pTesterParam->Filter = 50;
    m_pTesterParam->nStepLength = 1;
    m_pTesterParam->nMeasureLength = 400;
    m_pTesterParam->nMeasureOffset = 400;
    m_pTesterParam->nTriggerOffset = 0;
	if (m_bMT8820)
	{
		m_pTesterParam->nTriggerOffset = 2000;
	}
    m_pTesterParam->nTriggerThreshold = -30;
    m_pTesterParam->nTriggerTimeout = 5000;
	m_pTesterParam->bChRevert = FALSE;

    m_nPointNumber = 0;

    m_nStartBand = nBand;
    m_nStartChannel = nChannel;
    m_nStartSection = nSection;

    m_nStopBand = nBand;
    m_nStopChannel = nChannel;
    m_nStopSection = nSection;

    m_uTxSize = 16 + sizeof(CALI_APC_CTRL_HEADER_T);
}

void CApiTxModV3::ClearData()
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
                    pSectionData->arrPower[l].dPower = 0;
					for (uint32 nRangeindex = 0; nRangeindex < m_unRangeCount; nRangeindex++)
					{
						pSectionData->arrPower[l].dLogValue[nRangeindex] = 0;
						pSectionData->arrPower[l].usValue[nRangeindex] = 0;
					}
                }
            }
        }
    }
}

SPRESULT CApiTxModV3::InsertTrigger( int nBand, int nChannel )
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    BandData* pBandData = &(*m_parrBandData)[nBand];
    ChannelData* pChannelData = &pBandData->arrChannel[nChannel];

    //phone
    CALI_APC_ARFCN_T PhoneChannel;
	ZeroMemory(&PhoneChannel, sizeof(CALI_APC_ARFCN_T));
    PhoneChannel.Arfcn = pChannelData->usArfcn;
	if(m_bBandAdaptation)
	{
		PhoneChannel.Band = CLteUtility::m_BandInfo[pBandData->nBand].BandIdent;
	}
    PhoneChannel.AptNum = 1;
    m_arrPhoneChannel.push_back(PhoneChannel);
    m_pPhoneParam->stReqHeader.ApcRfChainCtrl[m_RfChain].ArfcnNum++;
	 
    m_uTxSize += sizeof(CALI_APC_ARFCN_T);

    CALI_APC_APT_CONFIG_T PhoneApt;
    PhoneApt.BeginWord = 0;
    PhoneApt.PaMode = 0;
    PhoneApt.APT = 35;
    PhoneApt.StepNum = (uint8)1;
    PhoneApt.StepLength = 8;
	PhoneApt.RbNum = QPSK_FULLRB; //QPSK_FULLRB
	PhoneApt.RbPosition = LTE_FULL_RB;
	PhoneApt.PdetFlag = 0;
    m_arrPhoneApt.push_back(PhoneApt);
    m_pPhoneParam->nAptNum[m_RfChain]++;

    m_uTxSize += sizeof(CALI_APC_APT_CONFIG_T);

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

