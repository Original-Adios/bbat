#include "Stdafx.h"
#include "AudioLoop.h"

IMPLEMENT_RUNTIME_CLASS(CAudioLoop)

CAudioLoop::CAudioLoop(void)
{
}

CAudioLoop::~CAudioLoop(void)
{
}
SPRESULT CAudioLoop::__PollAction(void)
{
    //耳机loop时需先进行耳机检测
    if (m_byMicId == HeadSetMic)
    {
        CHKRESULT(CheckHeadset());
    }
    //speaker 发送成功则一定能跑到 speaker close
    SPRESULT Res = SP_OK;

    CHKRESULT(phSpkPlayByDataSend());
    Sleep(500);
    Res = MicAction();

    CHKRESULT(phSpkClose());

    return Res;
}
BOOL CAudioLoop::LoadXMLConfig(void)
{
    std::string Mic_id = _W2CA(GetConfigValue(L"Option:Mic", L""));
    std::string Speaker_id = _W2CA(GetConfigValue(L"Option:Speaker", L""));
    m_nMic8k = GetConfigValue(L"Option:Mic_8K", 0);
    m_nRetry = GetConfigValue(L"Option:MicReadDataRetry:Retry", 0);
    m_nLoopCount = GetConfigValue(L"Option:MicReadDataRetry:LoopCount", 0);
    if (m_nLoopCount < 1)
    {
        m_nLoopCount = 1;
    }


    m_fSnrSpec = GetConfigValue(L"Option:SNR", 0.0);
    m_fPeakSpec = GetConfigValue(L"Option:Peak", 0.0);
    if (Mic_id == "MainMic")
    {
        m_pMicMsg = BBAT_MIC_CHANNEL_NAME[Main_Mic];
        m_byMicId = MainMic;
    }
    else if (Mic_id == "HeadSetMic")
    {
        m_pMicMsg = BBAT_MIC_CHANNEL_NAME[HeadSet_Mic];
        m_byMicId = HeadSetMic;
    }
    else if (Mic_id == "SubMic")
    {
        m_pMicMsg = BBAT_MIC_CHANNEL_NAME[Sub_Mic];
        m_byMicId = SubMic;
    }
    else if (Mic_id == "MidMic")
    {
        m_pMicMsg = BBAT_MIC_CHANNEL_NAME[Mid_Mic];
        m_byMicId = MidMic;
    }

    if (Speaker_id == "MainSpeaker")
    {
        m_pSpeakerMsg = BBAT_SPK_CHANNEL_NAME[Main_Speaker];
        m_bySpkId = MainSpeaker;
    }
    else if (Speaker_id == "MainReceiver")
    {
        m_pSpeakerMsg = BBAT_SPK_CHANNEL_NAME[Main_Receiver];
        m_bySpkId = MainReceiver;
    }
    else if (Speaker_id == "HeadSet")
    {
        m_pSpeakerMsg = BBAT_SPK_CHANNEL_NAME[Head_Set];
        m_bySpkId = HeadSet;
    }
    else
    {
        LogFmtStrA(SPLOGLV_ERROR, "Mic or Speaker Set Wrong.");
        return SP_E_FAIL;
    }
    return TRUE;
}

SPRESULT CAudioLoop::phMicOpen()
{
    SPRESULT Res = SP_BBAT_MicOpen(m_hDUT, m_byMicId);

    if (Res != SP_OK)
    {
        _UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Open Fail");
        return SP_E_BBAT_CMD_FAIL;
    }
    _UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Open Pass");

    return SP_OK;
}

SPRESULT CAudioLoop::phMicReadStatus()
{
    SPRESULT Res = SP_BBAT_MicReadStatus(m_hDUT, m_byMicId);
    if (Res != SP_OK)
    {
        _UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Read Status Fail");
        return SP_E_BBAT_CMD_FAIL;
    }
    _UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Read Status Pass");

    return SP_OK;
}

SPRESULT CAudioLoop::phMicReadData()
{
    BYTE arrDataRecv[1600] = { 0 };
    CHKRESULT(SP_BBAT_MicReadData(m_hDUT, m_byMicId, arrDataRecv));
    AudioResult Result;
    CalcMicData(arrDataRecv, m_nMic8k, Result);

    _UiSendMsg("SNR", LEVEL_ITEM, m_fSnrSpec, Result.dSNR, NOUPPLMT, nullptr, -1, nullptr);
    _UiSendMsg("Peak", LEVEL_ITEM, m_fPeakSpec, Result.dPeak, NOUPPLMT, nullptr, -1, nullptr);
    if (m_fSnrSpec > Result.dSNR || m_fPeakSpec > Result.dPeak)
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CAudioLoop::phMicClose()
{
    SPRESULT Res = SP_BBAT_MicClose(m_hDUT, m_byMicId);
    if (Res != SP_OK)
    {
        _UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Close Fail");
        return SP_E_BBAT_CMD_FAIL;
    }
    _UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Close Pass");
    return SP_OK;
}

SPRESULT CAudioLoop::phSpkPlayByDataSend()
{
    int arrSpkData[1024] = { 0 };
    GenerateSpeakerData(arrSpkData);
    SPRESULT Res = SP_BBAT_SpeakerPlayByDataSend(m_hDUT, m_bySpkId, arrSpkData);
    if (SP_OK != Res)
    {
        _UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Send Data Fail");
        return SP_E_BBAT_CMD_FAIL; //
    }
    _UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Send Data Pass");
    return SP_OK;
}

SPRESULT CAudioLoop::phSpkClose()
{
    SPRESULT Res = SP_BBAT_SpeakerClose(m_hDUT, m_bySpkId);
    if (Res != SP_OK)
    {
        _UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Close Fail");
        return SP_E_BBAT_CMD_FAIL; //
    }
    _UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Close Pass");

    return SP_OK;
}

SPRESULT CAudioLoop::CheckHeadset()
{
    HeadsetTypeMic Type;
    CHKRESULT(PrintErrorMsg(SP_BBAT_CheckHeadset(m_hDUT, &Type), "Check Headset Fail", LEVEL_ITEM));

    if (Type == NoHeadset)
    {
        PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "No Headset", LEVEL_ITEM);
        return SP_E_BBAT_VALUE_FAIL;
    }
    if (Type == HeadsetWithoutMic)
    {
        PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "Headset Without Mic", LEVEL_ITEM);
        return SP_E_BBAT_VALUE_FAIL;
    }
    PrintSuccessMsg(SP_E_BBAT_VALUE_FAIL, "Headset With Mic", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CAudioLoop::MicAction()
{
    //mic open成功则一定能跑到mic close
    SPRESULT Res = SP_OK;
    CHKRESULT(phMicOpen());
    Sleep(1500);
    Res = MicReadAction();
    if (SP_OK != Res && m_nRetry)
    {
        for (int i = 0; i < m_nLoopCount; i++)
        {
            Sleep(500);
            Res = MicReadAction();
            if (Res == SP_OK)
                break;
        }
    }
    Sleep(1500);
    CHKRESULT(phMicClose());
    return Res;
}

SPRESULT CAudioLoop::MicReadAction()
{
    CHKRESULT(phMicReadStatus());
    CHKRESULT(phMicReadData());
    return SP_OK;
}


