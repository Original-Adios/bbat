#include "StdAfx.h"
#include "ApiRxV3.h"
#include "math.h"
#include "assert.h"
#include "LteUtility.h"

CApiRxV3::CApiRxV3( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CApiBase(lpName, pFuncCenter)
{
    AddFunc(lpName, (IApiRxV3*)this);

    m_pPhoneParam = NULL;
	m_pPhoneParamV2 = NULL;
    m_pTesterParam = NULL;
	m_bMT8820 = FALSE;
	m_bLinteger = FALSE;

    DealwithData = NULL;
    m_pCurrentCaller = NULL;

	m_nStartBand = 0;
	m_nStopBand = 0;
	m_nStartChannel = 0;
	m_nStopChannel = 0;
	m_nStartGain = 0;
	m_nStopGain = 0;

	m_CurrentBand = 0;
	m_CurrentChannel = 0;
	m_pCurrentCa = NULL;
}

CApiRxV3::~CApiRxV3()
{
    if (m_pPhoneParam != NULL)
    {
        delete m_pPhoneParam;
        m_pPhoneParam = NULL;
    }

	if (m_pPhoneParamV2 != NULL)
	{
		delete m_pPhoneParamV2;
		m_pPhoneParamV2 = NULL;
	}
    
    if (m_pTesterParam != NULL)
    {
        delete m_pTesterParam;
        m_pTesterParam = NULL;
    }
}

SPRESULT CApiRxV3::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pIApi = (IApi*)this;

	try
	{
		if (m_pPhoneParam == NULL)
		{
			m_pPhoneParam = new PC_LTE_AGC_V3_T();
		}

		if (m_pPhoneParamV2 == NULL)
		{
			m_pPhoneParamV2 = new PC_LTE_AGC_V2_T();
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

SPRESULT CApiRxV3::Run()
{
	LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

	CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->GetProperty(DP_TXRX_SAMEPOINT, 0, &m_bMT8820), "Instrument::GetProperty()!");

	CHKRESULT(SwitchCa());

	m_nStartBand = 0;
	m_nStartChannel = 0;
	m_nStartGain = 0;
	m_nStopBand = 0;
	m_nStopChannel = 0;
	m_nStopGain = 0;

	InitParam();

	for (uint32 i = 0; i <  m_parrBandData->size(); i++) //band
	{   
		BandData* pBandData = &(*m_parrBandData)[i];
		
		if (m_bMT8820)
		{
			CHKRESULT(DoFunc());
			InitParam();

			m_nStartBand = i;
			m_nStartChannel = 0;
			m_nStartGain = 0;
		}

		for (uint32 j = 0; j < pBandData->arrChannel.size(); j++)
		{
			ChannelData* pChannelData = &pBandData->arrChannel[j];
			
			for (uint32 k = 0; k < pChannelData->arrPoint.size(); k++)
			{
				if (InsertGainIndex(i, j , k))
				{
					CHKRESULT(DoFunc());
					InitParam();

					m_nStartBand = i;
					m_nStartChannel = j;
					m_nStartGain = k;

					if (InsertGainIndex(i, j , k))
					{
						LogFmtStrA(SPLOGLV_ERROR, "%s Exception 1, InsertGainIndex(band:%d, chan:%d, point:%d)", __FUNCTION__, i, j , k);
						CHKRESULT_WITH_NOTIFY_LTE(SP_E_LTE_CALI_RF_EXCEPTION);
					}
				}

				m_nStopBand = i;
				m_nStopChannel = j;
				m_nStopGain = k;
			}
		}
	}
	if (DoFunc())
	{
		LogFmtStrA(SPLOGLV_ERROR, "%s Exception 1, last function DoFunc()", __FUNCTION__);
		return SP_E_LTE_CALI_RF_EXCEPTION;
	}

	LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

	return SP_OK;
}

SPRESULT CApiRxV3::DoFunc()
{
	if (m_pPhoneParam->Header.PointNumber == 0)
	{
		return SP_OK;
	}

	SPRESULT Result = SP_OK;

	if (m_ApiRetRssi.size() < m_pPhoneParam->Header.PointNumber)
	{
		m_ApiRetRssi.resize(m_pPhoneParam->Header.PointNumber);
	}
	//Instrument
	RF_LTE_PARAM param;
	m_pTesterParam->arrChannel = &m_arrInsChannel[0];
	m_pTesterParam->arrPower = &m_arrPower[0];
	param.pParam = m_pTesterParam;
	param.SubItem = STI_LTE_RX_V1;
	//Dut
	CHKRESULT(SetAgcParam());

	void* pAgcParam = NULL;

	pAgcParam = m_bLinteger ? (void*)m_pPhoneParam : (void*)m_pPhoneParamV2;

	CHKRESULT(SetRfSwitch(CLteUtility::GetBand(m_pTesterParam->usTriggerChannel), m_pTesterParam->TxAnt, m_pTesterParam->RxAnt));

	for(uint32 i = 0; i <= m_uMaxRetryCout; i++)
	{
		LTE_BAND_E Band = CLteUtility::GetBand(m_pTesterParam->arrChannel[0].usArfcn);
		IsValidIndex(Band);
        SetRepairBand(CLteUtility::m_BandInfo[Band].pRepairBand);
		CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->InitTest(TI_FDT ,&param), "Instrument::InitTest()!");

		CHKRESULT_WITH_NOTIFY_LTE_ITEM(SP_LteAgc_V4( m_hDUT, pAgcParam, &m_ApiRetRssi[0], m_bLinteger), "SP_LteAgc_V4()!");

		CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->FetchResult(TI_FDT, &param, m_pTesterParam->nTriggerTimeout + 1000), "Instrument::FetchResult()!");

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
				,m_nStartBand
				, m_nStartChannel
				, m_nStartGain
				, m_nStopBand
				, m_nStopChannel
				, m_nStopGain
				, i
				);
		}
		if (Result == SP_OK)
		{
			break;
		}
	}
	
	CHKRESULT(ResetRfSwitch());

	return Result;
}

SPRESULT CApiRxV3::InsertGainIndex( int nBand, int nChannel, int nGain )
{
    int i = nBand;
    int j = nChannel;
    int k = nGain;

	BandData* pBandData = &(*m_parrBandData)[i];
	ChannelData* pChannelData = &pBandData->arrChannel[j];
	PointData* pPointData = &pChannelData->arrPoint[k];

    if (m_CurrentBand != i || m_CurrentChannel != j)
    {
        if (InsertChannel(i, j))
        {
            return SP_E_LTE_CALI_UNEXPECTED;
        }
    }

    int nChannelIndex = m_pPhoneParam->Header.ChannelNumber - 1;
    int nPointIndex = m_pPhoneParam->Header.PointNumber;

    //Dut
    int nSize = m_ApiPoints.size();
    if (nSize == m_pPhoneParam->Header.PointNumber)
    {
        nSize += 100;
        m_ApiPoints.resize(nSize);
    }
    PC_LTE_AGC_V2_POINT_T Point;
    ZeroMemory(&Point, sizeof(PC_LTE_AGC_V2_POINT_T));
    
    Point.RxIndex = pPointData->usGainIndex + GainIndexOffset;

    m_ApiPoints[nPointIndex] = Point;
    m_pPhoneParam->Header.PointNumber++;
    m_vecApiChannels[nChannelIndex].PointNumber++;

    //Tester
	nChannelIndex = m_pTesterParam->nChannelCount - 1;
	m_arrInsChannel[nChannelIndex].PowerCount++;

	m_arrPower.push_back(pPointData->dPower);
	m_pTesterParam->nPowerCount++;

    return SP_OK;
}


SPRESULT CApiRxV3::InsertChannel( int nBand, int nChannel )
{
	BandData* pBandData = &(*m_parrBandData)[nBand];
	ChannelData* pChannelData = &pBandData->arrChannel[nChannel];

	if (MAX_PC_LTE_AGC_CHANNEL_NUM == m_pTesterParam->nChannelCount)
	{
	//	LogFmtStrA(SPLOGLV_ERROR, "%s Exception 1, channel count %d", __FUNCTION__, m_pTesterParam->nChannelCount);
		return SP_E_FAIL;
	}

	m_CurrentBand = nBand;
	m_CurrentChannel = nChannel;

	unsigned int Arfcn = pChannelData->usArfcn;
	unsigned char Indicator = (unsigned char)pChannelData->nIndicator;

	if (Arfcn > 65535)
	{
		m_bLinteger = TRUE;
	}

	//phone
	int nSize = m_vecApiChannels.size();
	if (nSize == m_pPhoneParam->Header.ChannelNumber)
	{
		nSize += 100;
		m_vecApiChannels.resize(nSize);
	}

	m_pPhoneParam->Header.ChannelNumber++;
	int nChannelIndex = m_pPhoneParam->Header.ChannelNumber - 1;

	PC_LTE_AGC_V3_CHANNEL_T Channel;
	Channel.Arfcn = (uint32)Arfcn;
	Channel.Indicator = Indicator;
	Channel.PointNumber = 0;
	m_vecApiChannels[nChannelIndex] = Channel;

	//instrument
	RF_LTE_CAL_RX_V1_REQ_CHANNEL_T InsChannel;
	InsChannel.usArfcn = pChannelData->usArfcn;
	InsChannel.FreqOffset = 10;
	InsChannel.bGap = TRUE;
	InsChannel.PowerCount = 0;

	m_arrInsChannel.push_back(InsChannel);
	m_pTesterParam->nChannelCount++;
	//config for Band66 downlink cal
	if (1 == m_pTesterParam->nChannelCount )
	{
		uint32 unTrigChan = (uint32)pBandData->arrChannel[0].usArfcn;

		if (m_bLinteger &&
			IN_RANGE(CLteUtility::m_BandInfo[LTE_BAND66].DlChannel.usBegin, CLteUtility::GetDlCHannel(unTrigChan), CLteUtility::m_BandInfo[LTE_BAND66].DlChannel.usEnd))
		{
			if ((CLteUtility::GetUlCHannel(unTrigChan) > CLteUtility::m_BandInfo[LTE_BAND66].UlChannel.usEnd))
			{
				unTrigChan = CLteUtility::m_BandInfo[LTE_BAND66].UlChannel.usEnd;
			}
		}
		m_pPhoneParam->Trigger.Arfcn = (uint32)CLteUtility::GetUlCHannel(unTrigChan);
		m_pPhoneParam->Trigger.Indicator = Indicator;
		m_pTesterParam->usTriggerChannel = unTrigChan;
	}

	return SP_OK;
}

void CApiRxV3::InitParam()
{
    ZeroMemory(m_pPhoneParam,sizeof(PC_LTE_AGC_V3_T));
	ZeroMemory(m_pPhoneParamV2,sizeof(PC_LTE_AGC_V2_T));
    ZeroMemory(m_pTesterParam,sizeof(RF_LTE_CAL_RX_V1_REQ_T));

	m_arrPower.clear();
	m_arrInsChannel.clear();
    //DUT V3
    m_pPhoneParam->Header.Bw = LTE_BW_20M;
    m_pPhoneParam->Header.CicleTime = 2;
    m_pPhoneParam->Header.MeasurementTime = m_bMT8820 ? 0:10;
    m_pPhoneParam->Header.OffsetTime = m_bMT8820?2:1;

    m_pPhoneParam->Header.HeaderSize = sizeof(PC_LTE_AGC_V2_HEADER_T);

    m_pPhoneParam->Header.TriggerSize = sizeof(PC_LTE_AGC_V3_TRIGGER_T);
    m_pPhoneParam->Header.TriggerOffset = 16 + sizeof(PC_LTE_AGC_V2_HEADER_T);
    m_pPhoneParam->Header.ChannelSize = sizeof(PC_LTE_AGC_V3_CHANNEL_T);
    m_pPhoneParam->Header.ChannelOffset = 16 + sizeof(PC_LTE_AGC_V2_HEADER_T) + sizeof(PC_LTE_AGC_V3_TRIGGER_T);

    m_pPhoneParam->Header.PointSize = sizeof(PC_LTE_AGC_V2_POINT_T);
	
    m_pPhoneParam->Header.Ant = (unsigned char)(m_Ant % 2);
    m_pPhoneParam->Header.Cc = (unsigned char)m_Ca;

    m_pPhoneParam->Trigger.Arfcn = 18000;
    m_pPhoneParam->Trigger.Indicator = 0;
    m_pPhoneParam->Trigger.Voltage = 35;
    m_pPhoneParam->Trigger.PaMode = 0;
    m_pPhoneParam->Trigger.Word = (unsigned short)m_nTriggerWord;
    m_pPhoneParam->Trigger.Cc = (unsigned char)m_Ca;
    m_pPhoneParam->Trigger.Ant = (m_Ant % 2);

	//DUT V2
	m_pPhoneParamV2->Header.Bw = LTE_BW_20M;
	m_pPhoneParamV2->Header.CicleTime = 2;
	m_pPhoneParamV2->Header.MeasurementTime = m_bMT8820 ? 0:10;
	m_pPhoneParamV2->Header.OffsetTime = m_bMT8820?2:1;

	m_pPhoneParamV2->Header.HeaderSize = sizeof(PC_LTE_AGC_V2_HEADER_T);

	m_pPhoneParamV2->Header.TriggerSize = sizeof(PC_LTE_AGC_V2_TRIGGER_T);
	m_pPhoneParamV2->Header.TriggerOffset = 16 + sizeof(PC_LTE_AGC_V2_HEADER_T);

	m_pPhoneParamV2->Header.ChannelSize = sizeof(PC_LTE_AGC_V2_CHANNEL_T);
	m_pPhoneParamV2->Header.ChannelOffset = 16 + sizeof(PC_LTE_AGC_V2_HEADER_T) + sizeof(PC_LTE_AGC_V2_TRIGGER_T);

	m_pPhoneParamV2->Header.PointSize = sizeof(PC_LTE_AGC_V2_POINT_T);

	m_pPhoneParamV2->Header.Ant = (unsigned char)(m_Ant % 2);
	m_pPhoneParamV2->Header.Cc = (unsigned char)m_Ca;

	m_pPhoneParamV2->Trigger.Arfcn = 18000;
	m_pPhoneParamV2->Trigger.Indicator = 0;
	m_pPhoneParamV2->Trigger.Voltage = 35;
	m_pPhoneParamV2->Trigger.PaMode = 0;
	m_pPhoneParamV2->Trigger.Word = (unsigned short)m_nTriggerWord;
	m_pPhoneParamV2->Trigger.Cc = (unsigned char)m_Ca;
	m_pPhoneParamV2->Trigger.Ant = (m_Ant % 2);
	//Tester
	m_pTesterParam->RxFilter = 200;
	m_pTesterParam->TxFilter = 200;
	m_pTesterParam->bCw = TRUE;
	m_pTesterParam->TxAnt = m_Ant <= RF_ANT_2nd ? RF_ANT_1st : RF_ANT_3rd;
	m_pTesterParam->RxAnt = m_Ant;
	m_pTesterParam->nStepLength = 2;
	m_pTesterParam->nGenerateOffset = 0;
	m_pTesterParam->nGenerateLength = 2000;
	m_pTesterParam->nTriggerOffset = 2000;
	m_pTesterParam->nTriggerPower = 23;
	m_pTesterParam->nTriggerThreshold = -20;
	m_pTesterParam->nTriggerTimeout = 2000;
	m_pTesterParam->bChRevert =FALSE;

    m_CurrentBand = -1;
    m_CurrentChannel = - 1;
	m_bLinteger = FALSE;
}

SPRESULT CApiRxV3::GetData()
{
    int i = 0;
    int j = 0;
    int k = 0;
    int nGainIndex = 0;
    
    int nTempStartChannel = 0;
    int nTempStopChannel = 0;
    int nTempStartGain = 0;
    int nTempStopGain = 0;
    
    for (i = m_nStartBand; i <= m_nStopBand; i++)
    {   
		BandData* pBandData = &(*m_parrBandData)[i];
        nTempStartChannel = 0;
        nTempStopChannel = pBandData->arrChannel.size() - 1;
        
        if (i == m_nStartBand)
        {
            nTempStartChannel = m_nStartChannel;
        }
        if (i == m_nStopBand)
        {
            nTempStopChannel = m_nStopChannel;
        }
        
        for (j = nTempStartChannel; j <= nTempStopChannel; j++)
        {       
			ChannelData* pChannelData = &pBandData->arrChannel[j];
            nTempStartGain = 0;
            nTempStopGain = pChannelData->arrPoint.size() - 1;
            
            if (i == m_nStartBand && j == m_nStartChannel)
            {
                nTempStartGain = m_nStartGain;
            }
            if (i == m_nStopBand && j == m_nStopChannel)
            {
                nTempStopGain = m_nStopGain;
            }
            for (k = nTempStartGain; k <= nTempStopGain; k++)
            {
				if (m_ApiRetRssi[nGainIndex] == 0)
				{
					LogRawStrA(SPLOGLV_ERROR, "Agc calibration response RSSI value is zero which is invalid!");
					return SP_E_SPAT_INVALID_DATA;
					//CHKRESULT_WITH_NOTIFY_LTE(SP_E_SPAT_INVALID_DATA);
				}
				PointData* pPointData = &pChannelData->arrPoint[k];
                double dRssi = 10 * log10(m_ApiRetRssi[nGainIndex]/pow((double)2, 30)) + 13;
				pPointData->dRssi = dRssi;
				pPointData->dGain = dRssi - pPointData->dPower;
                nGainIndex++;
            }
        }
    }
	return SP_OK;
}

SPRESULT CApiRxV3::Init()
{
    return TRUE;
}

void CApiRxV3::ClearData()
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
            }
        }
    }
}

SPRESULT CApiRxV3::SwitchCa()
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

	return SP_OK;
}

SPRESULT CApiRxV3::SetAgcParam()
{
	m_pPhoneParam->Channels = &m_vecApiChannels[0];
	m_pPhoneParam->Points = &m_ApiPoints[0];
	m_pPhoneParam->Header.PointOffset = (unsigned short)(m_pPhoneParam->Header.ChannelOffset + m_pPhoneParam->Header.ChannelNumber * m_pPhoneParam->Header.ChannelSize);

	m_vecApiChannelsV2.resize(m_vecApiChannels.size());
	for (uint32 nChIndx = 0; nChIndx < m_vecApiChannels.size(); nChIndx++)
	{
		m_vecApiChannelsV2[nChIndx].Arfcn = (uint16)m_vecApiChannels[nChIndx].Arfcn;
		m_vecApiChannelsV2[nChIndx].Indicator = m_vecApiChannels[nChIndx].Indicator;
		m_vecApiChannelsV2[nChIndx].PointNumber = m_vecApiChannels[nChIndx].PointNumber;
	}
	m_pPhoneParamV2->Channels = &m_vecApiChannelsV2[0];
	m_pPhoneParamV2->Points = &m_ApiPoints[0];
	m_pPhoneParamV2->Header.PointNumber = m_pPhoneParam->Header.PointNumber;
	m_pPhoneParamV2->Header.ChannelNumber = m_pPhoneParam->Header.ChannelNumber;

	m_pPhoneParamV2->Header.PointOffset = (unsigned short)(m_pPhoneParamV2->Header.ChannelOffset + m_pPhoneParamV2->Header.ChannelNumber * m_pPhoneParamV2->Header.ChannelSize);
	m_pPhoneParamV2->Trigger.Arfcn = (uint16)m_pPhoneParam->Trigger.Arfcn;
	m_pPhoneParamV2->Trigger.Indicator = m_pPhoneParam->Trigger.Indicator;

	return SP_OK;
}
