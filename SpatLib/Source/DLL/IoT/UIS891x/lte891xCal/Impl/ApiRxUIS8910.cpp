#include "StdAfx.h"
#include "ApiRxUIS8910.h"
#include "math.h"
#include "assert.h"
#include "LteUtility.h"

CApiRxUIS8910::CApiRxUIS8910( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CApiBase(lpName, pFuncCenter)
{
    AddFunc(lpName, (IApiRx*)this);

    m_pPhoneParam = NULL;
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

	m_nMaxChanNumber = MAX_PC_LTE_AGC_CHANNEL_NUM;
	m_nMaxPointNumber = MAX_PC_LTE_AGC_TOTAL_NUM;

	m_pCurrentCa = NULL;
	m_DeltaTriggerLevl = 0;
    m_UeType = 0;
}

CApiRxUIS8910::~CApiRxUIS8910()
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
}

SPRESULT CApiRxUIS8910::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pIApi = (IApi*)this;

	try
	{
		if (m_pPhoneParam == NULL)
		{
			m_pPhoneParam = new PC_LTE_AGC_UIS8910_T();
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

SPRESULT CApiRxUIS8910::Run()
{
	LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

	//CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->GetProperty(DP_TXRX_SAMEPOINT, 0, &m_bMT8820), "Instrument::GetProperty()!");

	CHKRESULT(SwitchCa());

	m_nStartBand = 0;
	InitParam(0, 0, 0);
    uint32 iArrBandDataSize = m_parrBandData->size();
	for (uint32 i = 0; i < iArrBandDataSize; i++) //band
	{   
		BandData* pBandData = &(*m_parrBandData)[i];
		
		if (m_bMT8820)
		{
			CHKRESULT(DoFunc());
			m_nStartBand = i;
			InitParam(i, 0, 0);
		}
        uint32 iArrChannelSize = pBandData->arrChannel.size();
		for (uint32 j = 0; j < iArrChannelSize; j++)
		{
			ChannelData* pChannelData = &pBandData->arrChannel[j];
			uint16 usPointNum = (uint16)pChannelData->arrPoint.size();
			if ((m_pPhoneParam->Header.ChannelNumber + 1 > m_nMaxChanNumber)|| (m_pPhoneParam->Header.PointNumber + usPointNum > m_nMaxPointNumber))
			{
				LogFmtStrA(SPLOGLV_INFO, "%s: ChanNumber(%d) Exceeds MaxChanNumber or PointNumber(%d) Exceeds MaxPointNumber,Start This Group Channel Test Firstly!"
				, __FUNCTION__, m_pPhoneParam->Header.ChannelNumber + 1, m_pPhoneParam->Header.PointNumber + usPointNum);
				CHKRESULT(DoFunc());
				m_nStartBand = i;
				InitParam(i, j, 0);
			}

			for (uint32 k = 0; k < usPointNum; k++)
			{
				CHKRESULT(InsertGainIndex(i, j , k))
				m_nStopBand = i;
				m_nStopChannel = j;
				m_nStopGain = k;
			}
		}
	}
	CHKRESULT(DoFunc());

	LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

	return SP_OK;
}

SPRESULT CApiRxUIS8910::DoFunc()
{
	if (m_pPhoneParam->Header.PointNumber == 0)
	{
		return SP_OK;
	}

	SPRESULT Result = SP_OK;
	BandData* pBandData = &(*m_parrBandData)[m_nStartBand];
	int nTriggerThreshold = m_pTesterParam->nTriggerThreshold;
	m_pTesterParam->nTriggerThreshold = nTriggerThreshold + m_DeltaTriggerLevl;

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

	PC_LTE_AGC_UIS8910_T* pAgcParam = NULL;

	pAgcParam = (PC_LTE_AGC_UIS8910_T*)m_pPhoneParam;

	for(uint32 i = 0; i <= m_uMaxRetryCout; i++)
	{
        LogFmtStrA(SPLOGLV_INFO, "%s :Get AGC-RSSI Retry Count = %d", __FUNCTION__, i);
		CHKRESULT_WITH_NOTIFY_LTE_ITEM(m_pRFTester->InitTest(TI_FDT ,&param), "Instrument::InitTest()!");

		//Result = SP_LteAgc_V4( m_hDUT, pAgcParam, &m_ApiRetRssi[0], m_bLinteger);
		Result = SP_LteAgc_UIS8910( m_hDUT, pAgcParam, &m_ApiRetRssi[0]);
		
		if (Result != SP_OK)
		{
			LogRawStrA(SPLOGLV_ERROR, "SP_lteAgcFDT failed");
			CHKRESULT_WITH_NOTIFY_LTE_ITEM(Result, "SP_LteAgc_UIS8910()!");
		}


		Result = m_pRFTester->FetchResult(TI_FDT, &param, m_pTesterParam->nPowerCount * 4 + 1000);
		if (Result != SP_OK)
		{
			LogRawStrA(SPLOGLV_ERROR, "FetchResult failed");
			if (0x1930 == m_UeType)
			{
				if (i < m_uMaxRetryCout)
				{
					if (i % 2)
					{
						m_DeltaTriggerLevl = 3 * ((i + 2) / 2);
					}
					else
					{
						m_DeltaTriggerLevl = 0 - 3 * ((i + 2) / 2);
					}
					m_pTesterParam->nTriggerThreshold = nTriggerThreshold + m_DeltaTriggerLevl;
					continue;
				}
			}
			SetRepairBand(CLteUtility::m_BandInfo[pBandData->nBand].pRepairBand);
			CHKRESULT_WITH_NOTIFY_LTE_ITEM_BAND(Result, "RX AGC Instrument::FetchResult()!", CLteUtility::m_BandInfo[pBandData->nBand].NameA);
		}

		Result = GetData(i);
		if (Result != SP_OK)
		{
			LogRawStrA(SPLOGLV_ERROR, "GetData return error!");
			continue;
		}
		if (m_pCurrentCaller != NULL && DealwithData != NULL)
		{   
			RxSetPara Para;
			Para.nStartBand = m_nStartBand;
			Para.nStartChannel = m_nStartChannel;
			Para.nStartGain = m_nStartGain;
			Para.nStopBand = m_nStopBand;
			Para.nStopChannel = m_nStopChannel;
			Para.nStopGain = m_nStopGain;

			Result = DealwithData(
				m_pCurrentCaller
				, Para
				, i
				);
		    if (Result != SP_OK)
			{
			    LogRawStrA(SPLOGLV_ERROR, "DealwithData return error!");
                continue;
			}
		}
		if (Result == SP_OK)
		{
			break;
		}
	}
	
	return Result;
}

SPRESULT CApiRxUIS8910::InsertGainIndex( int nBand, int nChannel, int nGain )
{
	BandData* pBandData = &(*m_parrBandData)[nBand];
	ChannelData* pChannelData = &pBandData->arrChannel[nChannel];
	PointData* pPointData = &pChannelData->arrPoint[nGain];

    if (m_CurrentBand != nBand || m_CurrentChannel != nChannel)
    {
        CHKRESULT(InsertChannel(nBand, nChannel))
    }

    int nChannelIndex = m_pPhoneParam->Header.ChannelNumber - 1;

    //Dut
    PC_LTE_AGC_UIS8910_POINT_T Point;
    ZeroMemory(&Point, sizeof(PC_LTE_AGC_UIS8910_POINT_T));
    //Point.RxIndex = pPointData->usGainIndex + GainIndexOffset; //chengui
	Point.RxIndex = pPointData->usGainIndex;
    m_ApiPoints.push_back(Point);
    m_pPhoneParam->Header.PointNumber++;
    m_vecApiChannels[nChannelIndex].PointNumber++;

    //Tester
	nChannelIndex = m_pTesterParam->nChannelCount - 1;
	m_arrInsChannel[nChannelIndex].PowerCount++;

	m_arrPower.push_back(pPointData->dPower);
	m_pTesterParam->nPowerCount++;

    return SP_OK;
}


SPRESULT CApiRxUIS8910::InsertChannel( int nBand, int nChannel )
{
	BandData* pBandData = &(*m_parrBandData)[nBand];
	ChannelData* pChannelData = &pBandData->arrChannel[nChannel];

	m_CurrentBand = nBand;
	m_CurrentChannel = nChannel;

	unsigned int Arfcn = pChannelData->usArfcn;
	unsigned char Indicator = (unsigned char)pChannelData->nIndicator;

	if (Arfcn > 65535)
	{
		m_bLinteger = TRUE;
	}

	//phone
	PC_LTE_AGC_UIS8910_CHANNEL_T Channel;
	Channel.Arfcn = (uint32)Arfcn;
	Channel.Indicator = Indicator;
	Channel.PointNumber = 0;
	Channel.Reserved[0] = 0;
	Channel.Reserved[1] = 0;
	m_vecApiChannels.push_back(Channel);
	m_pPhoneParam->Header.ChannelNumber++;

	//instrument
	RF_LTE_CAL_RX_V1_REQ_CHANNEL_T InsChannel;
	InsChannel.usArfcn = pChannelData->usArfcn;
	//if (0x8850 == m_UeType)//1.4MHz
	InsChannel.FreqOffset = 2;

	InsChannel.bGap = TRUE;
	InsChannel.PowerCount = 0;

	m_arrInsChannel.push_back(InsChannel);
	m_pTesterParam->nChannelCount++;
	//config for Band66 downlink cal
	if (1 == m_pTesterParam->nChannelCount )
	{
		uint32 unTrigChan = (uint32)pBandData->arrChannel[0].usArfcn;

		if (m_bLinteger && ( CLteUtility::GetUlCHannel(unTrigChan) > CLteUtility::m_BandInfo[LTE_BAND66].UlChannel.usEnd))
		{
			unTrigChan = CLteUtility::m_BandInfo[LTE_BAND66].UlChannel.usEnd;
		}
		//m_pPhoneParam->Trigger.Arfcn = (uint32)CLteUtility::GetUlCHannel(unTrigChan);
		m_pPhoneParam->Trigger.Arfcn = unTrigChan;
		m_pPhoneParam->Trigger.Indicator = Indicator;
		m_pTesterParam->usTriggerChannel = unTrigChan;
	}

	return SP_OK;
}

void CApiRxUIS8910::InitParam(int nBand, int nChannel, int nGain)
{
    ZeroMemory(m_pPhoneParam,sizeof(PC_LTE_AGC_UIS8910_T));
    ZeroMemory(m_pTesterParam,sizeof(RF_LTE_CAL_RX_V1_REQ_T));
	m_ApiRetRssi.clear();

	m_vecApiChannels.clear();
	m_ApiPoints.clear();

	m_arrPower.clear();
	m_arrInsChannel.clear();
    //DUT V3
    //m_pPhoneParam->Header.Bw = LTE_BW_20M;
	//if (0x8850 == m_UeType)//1.4MHz
    m_pPhoneParam->Header.Bw = LTE_BW_1_4M;

    m_pPhoneParam->Header.CicleTime = 1;
    m_pPhoneParam->Header.MeasurementTime = m_bMT8820 ? 0:10;
    m_pPhoneParam->Header.OffsetTime = m_bMT8820?2:1;

    m_pPhoneParam->Header.HeaderSize = sizeof(PC_LTE_AGC_UIS8910_HEADER_T);

    m_pPhoneParam->Header.TriggerSize = sizeof(PC_LTE_AGC_UIS8910_TRIGGER_T);
    m_pPhoneParam->Header.TriggerOffset = 16 + sizeof(PC_LTE_AGC_UIS8910_HEADER_T);
    m_pPhoneParam->Header.ChannelSize = sizeof(PC_LTE_AGC_UIS8910_CHANNEL_T);
    m_pPhoneParam->Header.ChannelOffset = 16 + sizeof(PC_LTE_AGC_UIS8910_HEADER_T) + sizeof(PC_LTE_AGC_UIS8910_TRIGGER_T);

    m_pPhoneParam->Header.PointSize = sizeof(PC_LTE_AGC_UIS8910_POINT_T);
	
    m_pPhoneParam->Header.Ant = (unsigned char)(m_Ant % 2);
    m_pPhoneParam->Header.Cc = (unsigned char)m_Ca;

    m_pPhoneParam->Trigger.Arfcn = 18000;
    m_pPhoneParam->Trigger.Indicator = 0;
    m_pPhoneParam->Trigger.Voltage = 0;
    m_pPhoneParam->Trigger.PaMode = 0;
//     if(0x1930 == m_UeType)
//     {
//         m_pPhoneParam->Trigger.Word = 69;//max power
//     }
//     else
//     {
//         m_pPhoneParam->Trigger.Word = 16; //0x20;
//     }
	//TriggerWord was been config by Seq
	m_pPhoneParam->Trigger.Word = (*m_parrBandData)[m_nStartBand].nTriggerWord;

    m_pPhoneParam->Trigger.Cc = (unsigned char)m_Ca;
    m_pPhoneParam->Trigger.Ant = (m_Ant % 2);

	//Tester
	if (0x8850 == m_UeType)
	{
		m_pTesterParam->RxFilter = 14;//50; //200;  //EMTC
		m_pTesterParam->TxFilter = 14;//50; //200;  //EMTC
	}
	else
	{
		m_pTesterParam->RxFilter = 14; //200;  //EMTC
		m_pTesterParam->TxFilter = 14; //200;  //EMTC
	}
	m_pTesterParam->bCw = TRUE;
	m_pTesterParam->TxAnt = m_Ant <= RF_ANT_2nd ? RF_ANT_1st : RF_ANT_3rd;
	m_pTesterParam->RxAnt = m_Ant;
	m_pTesterParam->nStepLength = m_pPhoneParam->Header.CicleTime;
	m_pTesterParam->nGenerateOffset = 0;
	m_pTesterParam->nGenerateLength = 2000;//2000;
	m_pTesterParam->nTriggerOffset = 2000;
	if(0x1930 == m_UeType)
	{
//		m_pTesterParam->nTriggerPower = 23;
		m_pTesterParam->nTriggerThreshold = -27;
	}
	else
	{
//        m_pTesterParam->nTriggerPower = 23;
		m_pTesterParam->nTriggerThreshold = -20;
	}

	//TargetPower was been config by Seq
	m_pTesterParam->nTriggerPower = (*m_parrBandData)[m_nStartBand].nEnp;

	m_pTesterParam->nTriggerTimeout = 2000;//2000;
	m_pTesterParam->bChRevert =FALSE;

    m_CurrentBand = -1;
    m_CurrentChannel = - 1;

//    m_nStartBand = nBand;
	m_nStartChannel = nChannel;
	m_nStartGain = nGain;

	m_nStopBand = nBand;
	m_nStopChannel = nChannel;
	m_nStopGain = nGain;

	m_bLinteger = FALSE;
}


SPRESULT CApiRxUIS8910::GetData(uint32 nRetryCount)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int nGainIndex = 0;
    
    int nTempStartChannel = 0;
    int nTempStopChannel = 0;
    int nTempStartGain = 0;
    int nTempStopGain = 0;

	//UINT16 ofdm[6] = {128,256,512,1024,2048,2048};
    
    for (i = m_nStartBand; i <= m_nStopBand; i++)
    {   
		BandData* pBandData = &(*m_parrBandData)[i];
        nTempStartChannel = 0;
        nTempStopChannel = pBandData->arrChannel.size();
        
        if (i == m_nStartBand)
        {
            nTempStartChannel = m_nStartChannel;
        }
        if (i == m_nStopBand)
        {
            nTempStopChannel = m_nStopChannel + 1;
        }
        
        for (j = nTempStartChannel; j < nTempStopChannel; j++)
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
                    IsValidIndex(CLteUtility::GetBand(pChannelData->usArfcn));
                    LogFmtStrA(SPLOGLV_WARN, "%s arfcn(%d) Agc(%d) calibration response RSSI value is zero which is invalid!",CLteUtility::m_BandInfo[CLteUtility::GetBand(pChannelData->usArfcn)].NameA,pChannelData->usArfcn,nGainIndex);
                    if(nRetryCount < m_uMaxRetryCout)
                    {
                        //return SP_E_SPAT_INVALID_DATA;
                    }
                    else
                    {
                        //CHKRESULT_WITH_NOTIFY_LTE(SP_E_SPAT_INVALID_DATA);
                    }
                }

                PointData* pPointData = &pChannelData->arrPoint[k];
                //double dRssi = 10 * log10(m_ApiRetRssi[nGainIndex]/pow((double)2, 30)) + 13;  //chengui

                //double dRssi = 10 * log10((double)m_ApiRetRssi[nGainIndex] * 2048) - 10*log10(pow((double)2, 22)) -30;
                double dRssi = 10 * log10((double)m_ApiRetRssi[nGainIndex] * 128) - 10*log10(pow((double)2, 22)) -30 - 1;
                pPointData->dRssi = dRssi;
                pPointData->dGain = dRssi - pPointData->dPower;
                nGainIndex++;
            }
        }
    }
	return SP_OK;
}

SPRESULT CApiRxUIS8910::Init()
{
    return TRUE;
}

void CApiRxUIS8910::ClearData()
{
    uint32 iArrBandDataSize = m_parrBandData->size();
    for (uint32 i = 0; i < iArrBandDataSize; i++)
    {
        BandData* pBandData = &(*m_parrBandData)[i];
        uint32 iArrChannelSize = pBandData->arrChannel.size();
        for (uint32 j = 0; j < iArrChannelSize; j++)
        {
            ChannelData* pChannelData = &pBandData->arrChannel[j];
            uint32 iArrPointSize = pChannelData->arrPoint.size();
            for (uint32 k = 0; k < iArrPointSize; k++)
            {
                pChannelData->arrPoint[k].dRssi = 0;
            }
        }
    }
}

SPRESULT CApiRxUIS8910::SwitchCa()
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

SPRESULT CApiRxUIS8910::SetAgcParam()
{
	m_pPhoneParam->Channels = &m_vecApiChannels[0];
	m_pPhoneParam->Points = &m_ApiPoints[0];
	m_pPhoneParam->Header.PointOffset = (unsigned short)(m_pPhoneParam->Header.ChannelOffset + m_pPhoneParam->Header.ChannelNumber * m_pPhoneParam->Header.ChannelSize);

	return SP_OK;
}
