#include "stdafx.h"
#include "NrApiV4.h"
#include "NrUtility.h"
#include "NrDefine.h"

CNrApiV4::CNrApiV4(void)
{
    memset(&m_PhoneRltReq, 0, sizeof(m_PhoneRltReq));
    memset(&m_PhoneReq, 0, sizeof(m_PhoneReq));
    memset(&m_PhoneRltRlt, 0, sizeof(m_PhoneRltRlt));
}

CNrApiV4::~CNrApiV4(void)
{
}

SPRESULT CNrApiV4::RunTxPhoneCommand(BOOL bOn)
{
    if (bOn)
    {
        int PathType;
        int PathIndex;
        ConvertFromChannel(m_pLossData->nChannel, PathType, PathIndex);

        m_PhoneReq.Config.OnOff = NR_CAL_TRX_SWITCH::NR_CAL_TRX_SWITCH_TX_ON;
        m_PhoneReq.Config.band = CNrUtility::m_BandInfo[m_pLossData->nBand].BandIdent;
        m_PhoneReq.Config.ul_arfcn = m_pLossData->uArfcn;
        m_PhoneReq.Config.ctrl_path_type = PathType;
        m_PhoneReq.Config.ctrl_path_index = PathIndex;
        m_PhoneReq.Config.scs = NR_SCS_30K;

        m_PhoneReq.TX.PowerMode = 1;
        m_PhoneReq.TX.PhyChType = NR_TX_CHANNEL_PUSCH;
        m_PhoneReq.TX.ANT = (NR_ANT_FLAG_E)(1 << m_pLossData->nAnt);
        m_PhoneReq.TX.BW = NR_BW_5MHz;
        m_PhoneReq.TX.OFDM = NR_OFDM_CP;
        m_PhoneReq.TX.Modulation = NR_MODULATION_QPSK;
        m_PhoneReq.TX.TxMode = 1;
        m_PhoneReq.TX.TxWord = (uint16)(m_pLossData->nParam + m_pLossData->nParamOffset);
        m_PhoneReq.TX.Voltage = m_pLossData->nVoltage;
        m_PhoneReq.TX.RbOffset = 0;
        m_PhoneReq.TX.RbNumber = 11;
    }
    else
    {
        m_PhoneReq.Config.OnOff = NR_CAL_TRX_SWITCH::NR_CAL_TRX_SWITCH_OFF;
    }
    
    CHKRESULT(SP_NR_CAL_ModemV4_SetTrx(m_hDUT, &m_PhoneReq));

    return SP_OK;
}

SPRESULT CNrApiV4::RunRxPhoneCommand()
{
    int PathType;
    int PathIndex;
    ConvertFromChannel(m_pLossData->nChannel, PathType, PathIndex);

    m_PhoneReq.Config.band = CNrUtility::m_BandInfo[m_pLossData->nBand].BandIdent;
    m_PhoneReq.Config.dl_arfcn = m_pLossData->uArfcn;
    m_PhoneReq.Config.OnOff = NR_CAL_TRX_SWITCH::NR_CAL_TRX_SWITCH_RX_ON;
    m_PhoneReq.Config.ctrl_path_type = PathType;
    m_PhoneReq.Config.ctrl_path_index = PathIndex;
    m_PhoneReq.Config.scs = NR_SCS_30K;

    m_PhoneReq.RX.ANT = (NR_ANT_FLAG_E)(1 << m_pLossData->nAnt);
    m_PhoneReq.RX.BW = NR_BW_5MHz;
    m_PhoneReq.RX.GainIndex[m_pLossData->nAnt] = (uint16)m_pLossData->nParam;
    m_PhoneReq.RX.LnaMode[m_pLossData->nAnt] = NR_CAL_TRX_V4_EXT_LNA_TYPE::NR_CAL_TRX_V4_EXT_LNA_HIGH;

    CHKRESULT(SP_NR_CAL_ModemV4_SetTrx(m_hDUT, &m_PhoneReq));

    NR_CAL_TRX_RLT_REQ_T Setting;
    Setting.Freq = FALSE;
    Setting.Pdet = FALSE;
    Setting.Rssi = TRUE;
    NR_CAL_TRX_RLT_RLT_V4_T Info;

    CHKRESULT(SP_NR_CAL_ModemV4_GetTrxInfo(m_hDUT, &Setting, &Info));
    m_pLossData->dResult = ConvertRssi(Info.Rssi[m_pLossData->nAnt]);

    m_PhoneReq.Config.OnOff = NR_CAL_TRX_SWITCH::NR_CAL_TRX_SWITCH_OFF;
    CHKRESULT(SP_NR_CAL_ModemV4_SetTrx(m_hDUT, &m_PhoneReq));

    return SP_OK;
}
