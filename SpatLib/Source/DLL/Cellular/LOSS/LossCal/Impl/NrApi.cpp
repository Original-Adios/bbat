#include "StdAfx.h"
#include "NrApi.h"
#include "NrUtility.h"
#include <math.h>

CNrApi::CNrApi(void)
{
    memset(&m_PhoneRltReq, 0, sizeof(m_PhoneRltReq));
    memset(&m_PhoneReq, 0, sizeof(m_PhoneReq));
    memset(&m_PhoneRltRlt, 0, sizeof(m_PhoneRltRlt));
}

CNrApi::~CNrApi(void)
{
}

SPRESULT CNrApi::RunTxPhoneCommand( BOOL bOn )
{
    m_PhoneReq.PowerMode = 1;
    m_PhoneReq.RxChannelNumber = 0;
    m_PhoneReq.TxChannelNumber = 1;
    m_PhoneReq.TX[0].TxChannel = NR_TX_CHANNEL_PUSCH;
    m_PhoneReq.TX[0].ANT = (NR_ANT_FLAG_E)(1 << m_pLossData->nAnt);
    m_PhoneReq.TX[0].Band = CNrUtility::m_BandInfo[m_pLossData->nBand].BandIdent;
    m_PhoneReq.TX[0].Arfcn = m_pLossData->uArfcn;
    m_PhoneReq.TX[0].BW = NR_BW_5MHz;
    m_PhoneReq.TX[0].Channel = NR_CHANNEL0;
    m_PhoneReq.TX[0].OFDM = NR_OFDM_CP;
    m_PhoneReq.TX[0].SCS = NR_SCS_30K;
    m_PhoneReq.TX[0].Modulation = NR_MODULATION_QPSK;
    m_PhoneReq.TX[0].TxMode = 1;
    m_PhoneReq.TX[0].TxWord = (uint16)(m_pLossData->nParam + m_pLossData->nParamOffset);
    m_PhoneReq.TX[0].Voltage = 20;
    m_PhoneReq.TX[0].RbOffset = 0;
    m_PhoneReq.TX[0].RbNumber = 11;

#ifndef Internal_Debug
    m_PhoneReq.OnOff = (uint16)bOn;
    CHKRESULT(SP_NR_CAL_SetTrx(m_hDUT, &m_PhoneReq));
#endif

    return SP_OK;
}

SPRESULT CNrApi::RunRxPhoneCommand()
{
    m_PhoneReq.PowerMode = 1;
    m_PhoneReq.RxChannelNumber = 1;
    m_PhoneReq.TxChannelNumber = 0;
    m_PhoneReq.RX[0].ANT = (NR_ANT_FLAG_E)(1 << m_pLossData->nAnt);
    m_PhoneReq.RX[0].Band = CNrUtility::m_BandInfo[m_pLossData->nBand].BandIdent;
    m_PhoneReq.RX[0].Arfcn = m_pLossData->uArfcn;
    m_PhoneReq.RX[0].BW = NR_BW_5MHz;
    m_PhoneReq.RX[0].Channel = NR_CHANNEL0;
    m_PhoneReq.RX[0].GainIndex = (uint16)m_pLossData->nParam;
    m_PhoneReq.RX[0].SCS = NR_SCS_30K;

#ifndef Internal_Debug
    m_PhoneReq.OnOff = TRUE;
    CHKRESULT(SP_NR_CAL_SetTrx(m_hDUT, &m_PhoneReq));
#endif

#ifndef Internal_Debug
    NR_CAL_TRX_RLT_REQ_T Setting;
    Setting.Freq = FALSE;
    Setting.Pdet = FALSE;
    Setting.Rssi = TRUE;
    NR_CAL_TRX_RLT_RLT_T Info;

    CHKRESULT(SP_NR_CAL_GetTrxInfo(m_hDUT, &Setting, &Info));
    m_pLossData->dResult = Info.Rssi[0][m_pLossData->nAnt] / 32.0 * 3 - 10 * log10(pow(2.0, 23)) + 15.1;
#else
    m_pLossData->dResult = -5;
#endif

#ifndef Internal_Debug
    m_PhoneReq.OnOff = FALSE;
    CHKRESULT(SP_NR_CAL_SetTrx(m_hDUT, &m_PhoneReq));
#endif

    return SP_OK;
}

double CNrApi::GetUlFreq(int /*nBand*/, uint32 uArfcn)
{
    return CNrUtility::GetFreq(uArfcn);
}

double CNrApi::GetDlFreq(int /*nBand*/, uint32 uArfcn)
{
    return CNrUtility::GetFreq(uArfcn) + 1;
}
