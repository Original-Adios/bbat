#include "StdAfx.h"
#include "ApiRxV1.h"
#include "math.h"
#include "assert.h"
#include "LteUtility.h"

CApiRxV1::CApiRxV1( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CApiBase(lpName, pFuncCenter)
{
    AddFunc(lpName, (IApiRxV1*)this);

    m_pPhoneParam = NULL;
    m_pPhoneRet = NULL;
    m_pTesterParam = NULL;
	m_bMT8820 = FALSE;
	m_uMaxRetryCout = MAX_AGC_RETRY_COUNT;

	m_nStartBand = 0;
	m_nStopBand = 0;
	m_nStartChannel = 0;
	m_nStopChannel = 0;
	m_nStartGain = 0;
	m_nStopGain = 0;
	m_pCurrentCa = NULL;
}

CApiRxV1::~CApiRxV1(void)
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
}

SPRESULT CApiRxV1::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pIApi = (IApi*)this;

    try
    {
        if (m_pPhoneParam == NULL)
        {
            m_pPhoneParam = new PC_LTE_FDT_RX_T();
        }

        if (m_pPhoneRet == NULL)
        {
            m_pPhoneRet = new PC_LTE_FDT_RX_RESULT_T();
        }

        if (m_pTesterParam == NULL)
        {
            m_pTesterParam = new RF_LTE_CAL_RX_V1_REQ_T();
        }
    }
    catch (const std::bad_alloc& /*e*/)
    {
        assert(0);
        return SP_E_SPAT_ALLOC_MEMORY;
    }
    
    m_pCurrentCa = (LTE_CA_E*)GetFunc(CURRENT_CA);

    return SP_OK;
}

SPRESULT CApiRxV1::Run()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

	CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->GetProperty(DP_TXRX_SAMEPOINT, 0, &m_bMT8820), "Instrument::GetProperty()!");

    CHKRESULT(SwitchCa());

    BOOL ret;

    InitParam(0, 0, 0);

    for (uint32 i = 0; i < m_parrBandData->size(); i++) //band
    { 
		if (m_bMT8820)
		{
			InitParam(i, 0, 0);
		}
        BandData* pBandData = &(*m_parrBandData)[i];

        for (uint32 j = 0; j < pBandData->arrChannel.size(); j++)
        {
            ChannelData* pChannelData = &pBandData->arrChannel[j];
            CHKRESULT(InsertPhoneChannel(&ret, i, j));
            if (!ret)
            {
                CHKRESULT(DoFunc());
                InitParam(i, j, 0);
                CHKRESULT(InsertPhoneChannel(&ret, i, j));
            }
            CHKRESULT(InsertInsChannel(&ret, i, j));

            for (uint32 k = 0; k < pChannelData->arrPoint.size(); k++)
            {
                CHKRESULT(InsertGainIndex(&ret, i, j , k));
                if (!ret)
                {
                    CHKRESULT(DoFunc());
                    InitParam(i, j, k);

                    CHKRESULT(InsertPhoneChannel(&ret, i, j));
                    CHKRESULT(InsertInsChannel(&ret, i, j));
                    CHKRESULT(InsertGainIndex(&ret, i, j , k));
                    if (!ret)
                    {
                        LogFmtStrA(SPLOGLV_ERROR, "%s Exception 1", __FUNCTION__);
						CHKRESULT_WITH_NOTIFY_LTE(SP_E_LTE_CALI_RF_EXCEPTION);
                    }
                }

                m_nStopBand = i;
                m_nStopChannel = j;
                m_nStopGain = k;
            }
			if (m_bMT8820)
			{
				CHKRESULT(ChangeParam(&ret, m_nStopBand, m_nStopChannel, m_nStopGain));
			}
        }
		if (m_bMT8820)
		{
			CHKRESULT(DoFunc());
		}
    }
//	if (!m_bMT8820)
	{
		 CHKRESULT(DoFunc());
	}

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

void CApiRxV1::InitParam( int nBand, int nChannel, int nSection )
{
    ZeroMemory(m_pPhoneParam,sizeof(PC_LTE_FDT_RX_T));
    ZeroMemory(m_pPhoneRet,sizeof(PC_LTE_FDT_RX_RESULT_T));
    ZeroMemory(m_pTesterParam,sizeof(RF_LTE_CAL_RX_V1_REQ_T));

    m_arrInsChannel.clear();
    m_arrPower.clear();

    m_pPhoneParam->eBW = LTE_BW_20M;
    m_pPhoneParam->nAfcVal = 1;
    m_pPhoneParam->afc1 = m_Afc;
    m_pPhoneParam->afc2 = m_Afc;

    m_pTesterParam->RxFilter = 200;
    m_pTesterParam->TxFilter = 200;
    m_pTesterParam->bCw = TRUE;
    m_pTesterParam->TxAnt = RF_ANT_1st;
    m_pTesterParam->RxAnt = m_Ant;
    m_pTesterParam->nStepLength = 4;
    m_pTesterParam->nGenerateOffset = 0;
    m_pTesterParam->nGenerateLength = 4000;
	m_pTesterParam->nTriggerOffset = 1000;
	if (m_bMT8820)
	{
		m_pTesterParam->nTriggerOffset = 2000;
	}
    
    m_pTesterParam->nTriggerPower = 23;
    m_pTesterParam->nTriggerThreshold = -20;
    m_pTesterParam->nTriggerTimeout = 2000;
	m_pTesterParam->bChRevert =FALSE;

    m_nStartBand = nBand;
    m_nStartChannel = nChannel;
    m_nStartGain = nSection;

    m_nStopBand = nBand;
    m_nStopChannel = nChannel;
    m_nStopGain = nSection;
}

void CApiRxV1::ClearData()
{
    for (uint32 i = 0; i < m_parrBandData->size(); i++)
    {
        BandData* pBandData = &(*m_parrBandData)[i];

        for (uint32 j = 0; j < pBandData->arrChannel.size(); j++)
        {
            ChannelData* pChannelData = &pBandData->arrChannel[j];

            for (uint32 k = 0; k < pChannelData->arrPoint.size(); k++)
            {
                pChannelData->arrPoint[k].dRssi = 0;
                pChannelData->arrPoint[k].dGain = 0;
            }
        }
    }
}

SPRESULT CApiRxV1::GetData()
{
    int nGainIndex = 0;

    for (uint32 i = m_nStartBand; i <= m_nStopBand; i++)
    {   
        BandData* pBandData = &(*m_parrBandData)[i];

        uint32 nTempStartChannel = 0;
        uint32 nTempStopChannel = pBandData->arrChannel.size() - 1;

        if (i == m_nStartBand){ nTempStartChannel = m_nStartChannel; }
        if (i == m_nStopBand){ nTempStopChannel = m_nStopChannel; }

        for (uint32 j = nTempStartChannel; j <= nTempStopChannel; j++)
        {         
            ChannelData* pChannelData = &pBandData->arrChannel[j];

            uint32 nTempStartGain = 0;
            uint32 nTempStopGain = pChannelData->arrPoint.size() - 1;

            if (i == m_nStartBand && j == m_nStartChannel){ nTempStartGain = m_nStartGain; }
            if (i == m_nStopBand && j == m_nStopChannel){ nTempStopGain = m_nStopGain; }

            for (uint32 k = nTempStartGain; k <= nTempStopGain; k++)
            {
                PointData* pPointData = &pChannelData->arrPoint[k];

                if (m_Ant == LTE_ANT_DIV)
                {
                    nGainIndex++;
                }
				
				if (m_pPhoneRet->RSSI[nGainIndex] == 0)
				{
					LogRawStrA(SPLOGLV_ERROR, "Agc calibration response RSSI value is zero which is invalid!");
					return SP_E_SPAT_INVALID_DATA;
					//CHKRESULT_WITH_NOTIFY_LTE(SP_E_SPAT_INVALID_DATA);
				}

                pPointData->dRssi = 10 * log10(m_pPhoneRet->RSSI[nGainIndex] / pow((double)2, 30)) + 13;
                pPointData->dGain = pPointData->dRssi - pPointData->dPower;
                nGainIndex++;

                if (m_Ant == LTE_ANT_MAIN)
                {
                    nGainIndex++;
                }

				if (m_bMT8820 && k == nTempStopGain)
				{
					nGainIndex += 2;
				}
            }
        }
    }
	return SP_OK;
}

SPRESULT CApiRxV1::DoFunc()
{
    IS_USER_STOP_FUNC;

    if (m_pPhoneParam->nChannelNum == 0)
    {
        return SP_OK;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    SPRESULT Result = SP_OK;

    RF_LTE_PARAM param;
    m_pTesterParam->arrChannel = &m_arrInsChannel[0];
    m_pTesterParam->arrPower = &m_arrPower[0];
    m_pTesterParam->usTriggerChannel = m_arrInsChannel[0].usArfcn;
    param.pParam = m_pTesterParam;
    param.SubItem = STI_LTE_RX_V1;
    
    CHKRESULT(SetRfSwitch(CLteUtility::GetBand(m_pTesterParam->usTriggerChannel), m_pTesterParam->TxAnt, m_pTesterParam->RxAnt));
    LTE_BAND_E Band = CLteUtility::GetBand(m_pTesterParam->arrChannel[0].usArfcn);
    IsValidIndex(Band);

    SetRepairBand(CLteUtility::m_BandInfo[Band].pRepairBand);
    for (uint32 i = 0; i <= m_uMaxRetryCout; i++)
    {
		CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->InitTest(TI_FDT ,&param), "Instrument::InitTest()!");

		CHKRESULT_WITH_NOTIFY_LTE_ITEM(SP_lteAgc(m_hDUT, m_pPhoneParam, m_pPhoneRet), "SP_lteAgc()!");

        if(m_pPhoneParam->nChannelNum != m_pPhoneRet->nChannelNum)
        {
            LogRawStrA(SPLOGLV_ERROR, "SP_lteAgcFDT: m_pPhoneParam->nChannelNum != m_pPhoneRet->nChanelNum");
			CHKRESULT_WITH_NOTIFY_LTE_ITEM(SP_E_LTE_CALI_AGC_PHONE_API_INVALID_RET, "Chan number is not correct!");
        }

		CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->FetchResult(TI_FDT, &param, m_pTesterParam->nPowerCount * 4 + 1000), "Instrument::FetchResult()!");

        Result = GetData();
		if (Result != SP_OK)
		{
			LogRawStrA(SPLOGLV_ERROR, "GetData return error!");
			continue;
		}

        if (m_pCurrentCaller != NULL && DealwithData != NULL)
        {   
            Result = DealwithData(
                m_pCurrentCaller
                , m_nStartBand
                , m_nStartChannel
                , m_nStartGain
                , m_nStopBand
                , m_nStopChannel
                , m_nStopGain
				, i);
        }

        if (Result == SP_OK)
        {
            break;
        }
    }

    CHKRESULT(ResetRfSwitch());

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return Result;
}

SPRESULT CApiRxV1::InsertGainIndex( BOOL* ret, int nBand, int nChannel, int nGain )
{
    int i = nBand;
    int j = nChannel;
    int k = nGain;

    BandData* pBandData = &(*m_parrBandData)[i];
    ChannelData* pChannelData = &pBandData->arrChannel[j];
    PointData* pPointData = &pChannelData->arrPoint[k];

    int nChannelIndex = 0;
    int nGainIndex = 0;
    //uint16 usArfcn;

    nChannelIndex = m_pPhoneParam->nChannelNum - 1;
    //usArfcn = pChannelData->usArfcn;

    //Dut
    nChannelIndex = m_pPhoneParam->nChannelNum - 1;
    if (10 == m_pPhoneParam->AFRCN[nChannelIndex].nAgcArrNum)
    {
        CHKRESULT(InsertPhoneChannel(ret, i, j));
        if (!*ret)
        {
            return SP_OK;
        }
        nChannelIndex++;
    }
    m_pPhoneParam->AFRCN[nChannelIndex].nAgcArrNum++;
    nGainIndex = m_pPhoneParam->AFRCN[nChannelIndex].nAgcArrNum - 1;
    m_pPhoneParam->AFRCN[nChannelIndex].nAgcRxArr[nGainIndex] = pPointData->usGainIndex + GainIndexOffset;

    //Tester
    nChannelIndex = m_pTesterParam->nChannelCount - 1;
    m_arrInsChannel[nChannelIndex].PowerCount++;
    m_arrPower.push_back(pPointData->dPower);
    m_pTesterParam->nPowerCount++;

    *ret = TRUE;
    return SP_OK;
}

SPRESULT CApiRxV1::InsertPhoneChannel( BOOL* ret, int nBand, int nChannel )
{
    int i = nBand;
    int j = nChannel;

    BandData* pBandData = &(*m_parrBandData)[i];
    ChannelData* pChannelData = &pBandData->arrChannel[j];

    if (MAX_PC_LTE_AGC_CHANNEL_NUM == m_pPhoneParam->nChannelNum)
    {
        *ret = FALSE;
        return SP_OK;
    }

    m_pPhoneParam->nChannelNum++;
    int nChannelIndex = m_pPhoneParam->nChannelNum - 1;

    m_pPhoneParam->AFRCN[nChannelIndex].nArfcn = (uint16)pChannelData->usArfcn;
    m_pPhoneParam->BandIndicator[nChannelIndex] = (uint8)pChannelData->nIndicator;

    *ret = TRUE;
    return SP_OK;
}

SPRESULT CApiRxV1::InsertInsChannel( BOOL* ret, int nBand, int nChannel )
{
    int i = nBand;
    int j = nChannel;

    BandData* pBandData = &(*m_parrBandData)[i];
    ChannelData* pChannelData = &pBandData->arrChannel[j];

    RF_LTE_CAL_RX_V1_REQ_CHANNEL_T InsChannel;
    InsChannel.usArfcn = pChannelData->usArfcn;
    InsChannel.FreqOffset = 10;
    InsChannel.bGap = FALSE;
    InsChannel.PowerCount = 0;

    m_arrInsChannel.push_back(InsChannel);
    m_pTesterParam->nChannelCount++;

    *ret = TRUE;
    return SP_OK;
}

SPRESULT CApiRxV1::SwitchCa()
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

SPRESULT CApiRxV1::ChangeParam(BOOL* ret, int nBand, int nChannel, int nGain)
{
	int nDutChannelIndex = m_pPhoneParam->nChannelNum - 1;

	CHKRESULT(InsertGainIndex(ret, nBand, nChannel, nGain));

	if (!(*ret))
	{
		m_pPhoneParam->nChannelNum++;
		m_pPhoneParam->AFRCN[nDutChannelIndex + 1].nAgcArrNum = 0;
		m_pPhoneParam->AFRCN[nDutChannelIndex + 1].nArfcn = m_pPhoneParam->AFRCN[nDutChannelIndex].nArfcn;
		m_pPhoneParam->BandIndicator[nDutChannelIndex + 1] = m_pPhoneParam->BandIndicator[nDutChannelIndex];
		InsertGainIndex(ret, nBand, nChannel, nGain);
	}
	return SP_OK;
}