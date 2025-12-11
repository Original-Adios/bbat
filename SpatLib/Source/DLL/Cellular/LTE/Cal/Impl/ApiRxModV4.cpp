#include "StdAfx.h"
#include "ApiRxModV4.h"
#include "math.h"
#include "assert.h"
#include "LteUtility.h"

CApiRxModV4::CApiRxModV4( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CApiBase(lpName, pFuncCenter)
{
    AddFunc(lpName, (IApiRxModV3*)this);

	m_pPhoneParamV4 = NULL;
    m_pTesterParam = NULL;
	m_bMT8820 = FALSE;

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

	m_TxIns = 0;
	m_RxIns = 0;
	m_bBandAdaptation = FALSE;

	m_nChannelId = 0;
}

CApiRxModV4::~CApiRxModV4()
{
	if (m_pPhoneParamV4 != NULL)
	{
		delete m_pPhoneParamV4;
		m_pPhoneParamV4 = NULL;
	}

    if (m_pTesterParam != NULL)
    {
        delete m_pTesterParam;
        m_pTesterParam = NULL;
    }
}

SPRESULT CApiRxModV4::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pIApi = (IApi*)this;

	try
	{
		if (m_pPhoneParamV4 == NULL)
		{
			m_pPhoneParamV4 = new PC_MODEM_RF_V4_LTE_AGC_PARAM();
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

	return SP_OK;
}

SPRESULT CApiRxModV4::Run()
{
	LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

	CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->GetProperty(DP_TXRX_SAMEPOINT, 0, &m_bMT8820), "Instrument::GetProperty()!");

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

		uint32 iChannelsize = pBandData->arrChannel.size();

		for (uint32 j = 0; j < iChannelsize; j++)
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
                        CHKRESULT_WITH_NOTIFY_LTE(SP_E_LTE_CALI_RF_EXCEPTION)
                    }

					if (InsertGainIndex(i, j , k))
					{
						LogFmtStrA(SPLOGLV_ERROR, "%s Exception 1, InsertGainIndex(band:%d, chan:%d, point:%d)", __FUNCTION__, i, j , k);
						CHKRESULT_WITH_NOTIFY_LTE(SP_E_LTE_CALI_RF_EXCEPTION)
					}
				}
                InsertGainIndex(i, j , k);

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

SPRESULT CApiRxModV4::DoFunc()
{
	if (m_pPhoneParamV4->Header.AgcRfChain[0].RxGainNum == 0)
	{
		return SP_OK;
	}

	SPRESULT Result = SP_OK;

	if (m_ApiRetRssi.size() < m_pPhoneParamV4->Header.AgcRfChain[0].RxGainNum)
	{
		m_ApiRetRssi.resize(m_pPhoneParamV4->Header.AgcRfChain[0].RxGainNum);
	}
	//Instrument
	RF_LTE_PARAM param;
	m_pTesterParam->arrChannel = &m_arrInsChannel[0];
	m_pTesterParam->arrPower = &m_arrPower[0];
	param.pParam = m_pTesterParam;
	param.SubItem = STI_LTE_RX_V1;

	//DUT
	m_pPhoneParamV4->ChannelsChian0 = &m_vecApiChannels[0];
	m_pPhoneParamV4->PointsChain0 = &m_ApiPoints[0];
	m_pPhoneParamV4->Header.AgcRfChain[0].RxGainOffset = 4
		+  sizeof(PC_MODEM_RF_V3_LTE_AGC_HEADER_T)
		+ sizeof(PC_MODEM_RF_V3_LTE_AGC_TRIGGER_T)
		+ m_pPhoneParamV4->Header.AgcRfChain[0].ArfcnNum*sizeof(PC_MODEM_RF_V4_LTE_AGC_CHANNEL_T);

	//Dut
	for(uint32 i = 0; i <= m_uMaxRetryCout; i++)
	{
		CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->InitTest(TI_FDT ,&param), "Instrument::InitTest()!");

		Result = SP_ModemV4_LTE_AgcCal(m_hDUT, m_pPhoneParamV4, (uint16*)&m_ApiRetRssi[0]);
	
		if (Result != SP_OK)
		{
			LogRawStrA(SPLOGLV_ERROR, "SP_ModemV3_LTE_AgcCal failed");
			CHKRESULT_WITH_NOTIFY_LTE_ITEM(Result, "SP_ModemV3_LTE_AgcCal()!")
		}

		Result = m_pRFTester->FetchResult(TI_FDT, &param, m_pTesterParam->nTriggerTimeout + 5000);
		if (Result != SP_OK)
		{
			LogRawStrA(SPLOGLV_ERROR, "FetchResult failed");
			CHKRESULT_WITH_NOTIFY_LTE_ITEM(Result, "Instrument::FetchResult()!")
		}

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
	
	return Result;
}

SPRESULT CApiRxModV4::InsertGainIndex( int nBand, int nChannel, int nGain )
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

    int nChannelIndex = m_pPhoneParamV4->Header.AgcRfChain[0].ArfcnNum - 1;
    int nPointIndex = m_pPhoneParamV4->Header.AgcRfChain[0].RxGainNum;
    //Dut
    int nSize = m_ApiPoints.size();
    if (nSize == m_pPhoneParamV4->Header.AgcRfChain[0].RxGainNum)
    {
        nSize += 100;
        m_ApiPoints.resize(nSize);
    }

    PC_MODEM_RF_V3_LTE_AGC_GAIN_T Point;
    ZeroMemory(&Point, sizeof(PC_MODEM_RF_V3_LTE_AGC_GAIN_T));
    
    Point.AgcIndex = pPointData->usGainIndex;

    m_ApiPoints[nPointIndex] = Point;
	m_pPhoneParamV4->Header.AgcRfChain[0].RxGainNum++;
    m_vecApiChannels[nChannelIndex].RxGainNum++;
    //Tester
	nChannelIndex = m_pTesterParam->nChannelCount - 1;
	m_arrInsChannel[nChannelIndex].PowerCount++;

	m_arrPower.push_back(pPointData->dPower);
	m_pTesterParam->nPowerCount++;

    return SP_OK;
}

SPRESULT CApiRxModV4::InsertChannel( int nBand, int nChannel )
{
	BandData* pBandData = &(*m_parrBandData)[nBand];
	ChannelData* pChannelData = &pBandData->arrChannel[nChannel];

	m_nChannelId = pBandData->nChannelId;

	if (MAX_PC_LTE_AGC_CHANNEL_NUM == m_pTesterParam->nChannelCount)
	{
	//	LogFmtStrA(SPLOGLV_ERROR, "%s Exception 1, channel count %d", __FUNCTION__, m_pTesterParam->nChannelCount);
		return SP_E_FAIL;
	}

	m_CurrentBand = nBand;
	m_CurrentChannel = nChannel;

	unsigned int Arfcn = pChannelData->usArfcn;
	//phone
	int nSize = m_vecApiChannels.size();
	if (nSize == m_pPhoneParamV4->Header.AgcRfChain[0].ArfcnNum)
	{
		nSize += 100;
		m_vecApiChannels.resize(nSize);
	}

	m_pPhoneParamV4->Header.AgcRfChain[0].ArfcnNum++;
	int nChannelIndex = m_pPhoneParamV4->Header.AgcRfChain[0].ArfcnNum - 1;

	PC_MODEM_RF_V4_LTE_AGC_CHANNEL_T Channel;
	ZeroMemory(&Channel, sizeof(PC_MODEM_RF_V4_LTE_AGC_CHANNEL_T));
	Channel.Arfcn = (uint32)Arfcn;
	if(m_bBandAdaptation)
	{
		Channel.Band = CLteUtility::m_BandInfo[pBandData->nBand].BandIdent;
	}
	Channel.RxGainNum = 0;
	Channel.RF_CH = (uint16)pBandData->nChannelId;

	m_vecApiChannels[nChannelIndex] = Channel;
	//instrument
	RF_LTE_CAL_RX_V1_REQ_CHANNEL_T InsChannel;
	InsChannel.usArfcn = pChannelData->usArfcn;

	if(!m_bVsBW_Enable)
	{
		InsChannel.FreqOffset = 10;
	}
	else
	{
		if(LTE_RF_BW0 == m_Bw)
		{
			InsChannel.FreqOffset = 7;
		}
		else
		{
			InsChannel.FreqOffset = 10;
		}
	}

	InsChannel.bGap = TRUE;
	InsChannel.PowerCount = 0;

	m_arrInsChannel.push_back(InsChannel);
	m_pTesterParam->nChannelCount++;
	//config for Band66 downlink cal
	if (1 == m_pTesterParam->nChannelCount )
	{
		uint32 unTrigChan = (uint32)pBandData->arrChannel[0].usArfcn;

		if (IN_RANGE(CLteUtility::m_BandInfo[LTE_BAND66].DlChannel.usBegin, CLteUtility::GetDlCHannel(unTrigChan), CLteUtility::m_BandInfo[LTE_BAND66].DlChannel.usEnd))
		{
			if ((CLteUtility::GetUlCHannel(unTrigChan) > CLteUtility::m_BandInfo[LTE_BAND66].UlChannel.usEnd))
			{
				unTrigChan = CLteUtility::m_BandInfo[LTE_BAND66].UlChannel.usEnd;
			}
		}

		if (0 != pBandData->nTriggerArfcn)
		{
			m_pPhoneParamV4->Trigger.Arfcn = (uint32)CLteUtility::GetUlCHannel(pBandData->nTriggerArfcn);
			if (m_bBandAdaptation)
			{
				LTE_BAND_E nband = CLteUtility::GetBand(pBandData->nTriggerArfcn);
				if (!IN_RANGE(LTE_BAND1, nband, LTE_BAND62))
				{
					LogFmtStrA(SPLOGLV_ERROR, "%s Invalid band:: %d", __FUNCTION__, nband);
					return SP_E_INVALID_PARAMETER;;
				}
				m_pPhoneParamV4->Trigger.Band = CLteUtility::m_BandInfo[nband].BandIdent;
			}
			m_pTesterParam->usTriggerChannel = pBandData->nTriggerArfcn;
		}
		else
		{
			m_pPhoneParamV4->Trigger.Arfcn = (uint32)CLteUtility::GetUlCHannel(unTrigChan);
			if (m_bBandAdaptation)
			{
				m_pPhoneParamV4->Trigger.Band = CLteUtility::m_BandInfo[pBandData->nBand].BandIdent;
			}
			m_pTesterParam->usTriggerChannel = unTrigChan;
		}
	}

	return SP_OK;
}

void CApiRxModV4::InitParam()
{
	ZeroMemory(m_pPhoneParamV4, sizeof(PC_MODEM_RF_V4_LTE_AGC_PARAM));
    ZeroMemory(m_pTesterParam,sizeof(RF_LTE_CAL_RX_V1_REQ_T));

	m_arrPower.clear();
	m_arrInsChannel.clear();
	//////////////////////////////////////////////////////////////////////////
	//Modem V3
	m_pPhoneParamV4->Header.HeaderSize = sizeof(PC_MODEM_RF_V3_LTE_AGC_HEADER_T);
	m_pPhoneParamV4->Header.ArfcnSize = sizeof(PC_MODEM_RF_V4_LTE_AGC_CHANNEL_T);
	m_pPhoneParamV4->Header.TrigSize = sizeof(PC_MODEM_RF_V3_LTE_AGC_TRIGGER_T);
	m_pPhoneParamV4->Header.RxGainSize = sizeof(PC_MODEM_RF_V3_LTE_AGC_GAIN_T);
	m_pPhoneParamV4->Header.CaFlag = NON_CA;
	//Rf chain config
	m_pPhoneParamV4->Header.AgcRfChain[0].Ant = (uint8)m_Ant;
	m_pPhoneParamV4->Header.AgcRfChain[0].RfChain = (uint8)m_RfChain;

	if(!m_bVsBW_Enable)
	{
		m_pPhoneParamV4->Header.AgcRfChain[0].Bw = LTE_BW_5M;
	}
	else
	{
		if(LTE_RF_BW0 == m_Bw)
		{
			m_pPhoneParamV4->Header.AgcRfChain[0].Bw = LTE_BW_3M;
		}
		else
		{
			m_pPhoneParamV4->Header.AgcRfChain[0].Bw = LTE_BW_5M;
		}
	}

	m_pPhoneParamV4->Header.AgcRfChain[0].ArfcnOffset = 4 +  sizeof(PC_MODEM_RF_V3_LTE_AGC_HEADER_T)+ sizeof(PC_MODEM_RF_V3_LTE_AGC_TRIGGER_T);
	m_pPhoneParamV4->Header.AgcRfChain[0].TrigOffset = 4 +  sizeof(PC_MODEM_RF_V3_LTE_AGC_HEADER_T);
	m_pPhoneParamV4->Header.NopTime = 1;
	//Trigger config
	m_pPhoneParamV4->Trigger.Ant = (uint8)m_Ant;
	m_pPhoneParamV4->Trigger.Apt = 35;
	m_pPhoneParamV4->Trigger.PaMode = 0;
	m_pPhoneParamV4->Trigger.RfChain = (uint8)0;

	//Tester
	m_pTesterParam->RxFilter = 200;
	m_pTesterParam->TxFilter = 200;
	m_pTesterParam->bCw = TRUE;
	m_pTesterParam->TxAnt = (RF_ANT_E)m_TxIns;
	m_pTesterParam->RxAnt = (RF_ANT_E)m_RxIns;
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
}

SPRESULT CApiRxModV4::GetData()
{
    int i = 0;
    int j = 0;
    int k = 0;
    int nGainIndex = 1;
    
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
				/*if (m_ApiRetRssi[nGainIndex].rssi == 0)
				{
					LogRawStrA(SPLOGLV_ERROR, "Agc calibration response RSSI value is zero which is invalid!");
					CHKRESULT_WITH_NOTIFY_LTE(SP_E_SPAT_INVALID_DATA);
				}*/
				PointData* pPointData = &pChannelData->arrPoint[k];

				double dRssi = m_ApiRetRssi[nGainIndex].rssi*3/32.0 -10 * log10(pow((double)2, 23)) + 17.6;	

				pPointData->dRssi = dRssi;
				pPointData->dGain = dRssi - pPointData->dPower;
                nGainIndex++;
                nGainIndex++;
            }
        }
    }
	return SP_OK;
}

SPRESULT CApiRxModV4::Init()
{
    return TRUE;
}

void CApiRxModV4::ClearData()
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