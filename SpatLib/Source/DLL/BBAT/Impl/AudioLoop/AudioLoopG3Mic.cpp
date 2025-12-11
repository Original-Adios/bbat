#include "Stdafx.h"
#include "AudioLoopG3Mic.h"

IMPLEMENT_RUNTIME_CLASS(CAudioLoopG3)

CAudioLoopG3::CAudioLoopG3(void)
{
}

CAudioLoopG3::~CAudioLoopG3(void)
{
}
SPRESULT CAudioLoopG3::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return AudioLoopAction();
}
BOOL CAudioLoopG3::LoadXMLConfig(void)
{
    std::string Speaker_id = _W2CA(GetConfigValue(L"Option:Speaker", L""));
    m_nLoopIn = GetConfigValue(L"Option:LoopSelect:LoopIn", 0);
    m_nLoopOut = GetConfigValue(L"Option:LoopSelect:LoopOut", 0);
    m_dSnrSpec = GetConfigValue(L"Option:SNR", 0.0);
    m_dPeakSpec = GetConfigValue(L"Option:Peak", 0.0);

    if ("MainSpeaker" == Speaker_id)
    {
        m_pSpeakerMsg = BBAT_SPK_CHANNEL_NAME[Main_Speaker];
        m_bySpkId = MainSpeaker;
        //m_byAfcbSpeaker = AFCB_MIC_TYPE_SPK;
    }
    else if ("MainReceiver" == Speaker_id)
    {
        m_pSpeakerMsg = BBAT_SPK_CHANNEL_NAME[Main_Receiver];
        m_bySpkId = MainReceiver;
        //m_byAfcbSpeaker = AFCB_MIC_TYPE_REC;
    }
    else if ("Headset" == Speaker_id)
    {
        m_pSpeakerMsg = BBAT_SPK_CHANNEL_NAME[Head_Set];
        m_bySpkId = HeadSet;
        m_eAdcCh = Head_Mic;
        //m_byAfcbSpeaker = AFCB_MIC_TYPE_HEADSET;
    }
    if ("Headset" != Speaker_id)
    {
        if (1 == m_nLoopOut)
        {
            m_eAdcCh = Main_Mic_P;
        }
        else if (2 == m_nLoopOut)
        {
            m_eAdcCh = Sub_Mic_P;
        }
    }
    

    return TRUE;
}

SPRESULT CAudioLoopG3::AudioLoopAction()
{
    //speaker 发送成功则一定能跑到 speaker close
    SPRESULT Res = SP_OK;
    CHKRESULT(Phone_SpkPlayByDataSend());
    Sleep(500);
    CHKRESULT(AFCB_AmpEnable(false));
    CHKRESULT(AFCB_AudioLoopSelect(true));
    if (Head_Mic == m_eAdcCh)
    {
        CHKRESULT(AFCB_Headmic(EUROPE));
        CHKRESULT(AFCB_HeadSet_LR(HEADSET_L));
    }

    for (int i = 0; i < 3; i++)
    {
        RETRY_PRINT(i, "Retry");
        Res = AFCB_WaveGet();
        Sleep(500);
    }
    CHKRESULT(Phone_SpkClose());
    CHKRESULT(AFCB_WaveSet(false));
    CHKRESULT(AFCB_AudioLoopSelect(false));
    return Res;
}

SPRESULT CAudioLoopG3::Phone_SpkPlayByDataSend()
{
    SPRESULT Res = SP_OK;
    int arrSpkData[1024] = { 0 };
    GenerateSpeakerData(arrSpkData);
    Res = SP_BBAT_SpeakerPlayByDataSend(m_hDUT, m_bySpkId, arrSpkData);

    if (SP_OK != Res)
    {
        _UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Send Data");
        return Res; //
    }
    _UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Send Data");
    return SP_OK;
}

SPRESULT CAudioLoopG3::Phone_SpkClose()
{
    SPRESULT Res = SP_OK;
    Res = SP_BBAT_SpeakerClose(m_hDUT, m_bySpkId);

    if (Res != SP_OK)
    {
        _UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Close");
        return Res; //
    }
    _UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Close");

    return SP_OK;
}

SPRESULT CAudioLoopG3::AFCB_AudioLoopSelect(bool bStatus)
{
    int nLoopIn = m_nLoopIn;
    int nLoopOut = m_nLoopOut;
    if (!bStatus)
    {
        nLoopIn = 0;
        nLoopOut = 0;
    }
    if (m_pObjFunBox->FB_Audio_Loop_Select(m_nLoopIn, m_nLoopOut) == -1)
    {
        _UiSendMsg("G3: Loop Select", LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr,
            "Loop Select: %d, %d", m_nLoopIn, m_nLoopOut);
        return SP_E_BBAT_VALUE_FAIL;
    }
    _UiSendMsg("G3: Loop Select", LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr,
        "Loop Select: %d, %d", m_nLoopIn, m_nLoopOut);
    return SP_OK;
}



SPRESULT CAudioLoopG3::AFCB_WaveGet()
{
    SPRESULT Res = SP_OK;
    double dSNRValue = 0.0;
    double dPeakValue = 0.0;
    RETURNSPRESULT(m_pObjFunBox->FB_Common_ADC_En_Set_X(1),"G3: Adc Enable");
    unsigned short arrRecv_Value[1024] = { 0 };
    ZeroMemory(arrRecv_Value, sizeof(arrRecv_Value));
    RETURNSPRESULT(m_pObjFunBox->FB_ADC_Wave_Get_Data(m_eAdcCh, arrRecv_Value),"G3: Wave Get Data");

    Res = AnalysisG3Audio(arrRecv_Value, m_eAdcCh, dSNRValue, dPeakValue);
    _UiSendMsg("G3 ADC Wave", LEVEL_ITEM, m_dSnrSpec, dSNRValue, NOUPPLMT, 0, -1, 0, "Ch%d: SNR:%0.2lf", m_eAdcCh, dSNRValue);
    _UiSendMsg("G3 ADC Wave", LEVEL_ITEM, m_dPeakSpec, dPeakValue, NOUPPLMT, 0, -1, 0, "Ch%d: Peak:%0.2lf(%0.2lfmV)", m_eAdcCh, dPeakValue, dPeakValue * 3300 / 4096);
    if ((dSNRValue < m_dSnrSpec) || (dPeakValue < m_dPeakSpec))
    {
        Res = SP_E_BBAT_VALUE_FAIL;
    }
    RETURNSPRESULT(m_pObjFunBox->FB_Common_ADC_En_Set_X(0),"G3: Adc Disable");
    return Res;
}

SPRESULT CAudioLoopG3::AFCB_WaveSet(bool bStatus)
{
    LPCSTR lpName[2] =
    {
        "G3: Adc Wave Set False",
        "G3: Adc Wave Set True"
    };
    RETURNSPRESULT(m_pObjFunBox->FB_ADC_Wave_Set_Status(bStatus, (BYTE)m_eAdcCh), lpName[bStatus]);
    return SP_OK;
}

SPRESULT CAudioLoopG3::AFCB_AmpEnable(bool bStatus)
{
    LPCSTR lpMainMic[2] =
    {
        "G3: Main Mic Amp Close",
        "G3: Main Mic Amp Open"
    };
    RETURNSPRESULT(m_pObjFunBox->FB_Main_Mic_Amp_EN(bStatus), lpMainMic[bStatus]);
    return SP_OK;
}

SPRESULT CAudioLoopG3::AFCB_Headmic(HEADSET_TYPE nType)
{
    LPCSTR lpName[MAX_TYPE_HEADSET] =
    {
        "",
        "G3: Select Europe",
        "G3: Select America",
    };
    RETURNSPRESULT(m_pObjFunBox->FB_HeadMic_Ctrl_Select_X(nType), lpName[nType]);
    return SP_OK;
}

SPRESULT CAudioLoopG3::AFCB_HeadSet_LR(HEADSET_LR eHeadset)
{    
    LPCSTR lpTemp[MAX_LR] =
    {
        "G3: Headset L",
        "G3: Headset R"
    };
    RETURNSPRESULT(m_pObjFunBox->FB_HeadSet_LR_Select_X(eHeadset), lpTemp[eHeadset]);
    return SP_OK;
}