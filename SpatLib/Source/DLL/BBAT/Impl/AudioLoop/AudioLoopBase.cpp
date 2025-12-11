#include "StdAfx.h"
#include "AudioLoopBase.h"

CAudioLoopBase::CAudioLoopBase(TGroupAudioLoop tGroup, CImpBase* pImp)
{
    m_pImp = pImp;
    m_eTestType = tGroup.eTestType;
    m_nLoopIn = tGroup.nLoopIn;
    m_nLoopOut = tGroup.nLoopOut;
    m_dSnrSpec = tGroup.dSnrSpec;
    m_dPeakSpec = tGroup.dPeakSpec;
    m_nMic8k = tGroup.nMic8K;
    m_eBaseMic = tGroup.eMicId;
    m_eBaseSpk = tGroup.eSpkId;
    m_dVoltageUpN = tGroup.dVoltageUpN;
    m_dVoltageDownP = tGroup.dVoltageDownP;
    m_ePaId = tGroup.ePaId;

}

SPRESULT CAudioLoopBase::Run()
{
    m_pImp->_UiSendMsg("From", "CAudioLoopBase::Run");
    m_pImp->_UiSendMsg(m_pMicMsg, m_pSpeakerMsg);
    return SP_OK;
}

SPRESULT CAudioLoopBase::Phone_MicOpen()
{
    Sleep(500);
    SPRESULT Res = SP_OK;
    if (m_bFP)
    {
        Res = SP_BBAT_FP_MicOpen(m_pImp->m_hDUT, m_byMicId);
    }
    else
    {
        Res = SP_BBAT_MicOpen(m_pImp->m_hDUT, m_byMicId);
    }

    if (Res != SP_OK)
    {
        m_pImp->_UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Open");
        return Res;
    }
    m_pImp->_UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Open");

    return SP_OK;
}

SPRESULT CAudioLoopBase::Phone_MicReadStatus()
{
    Sleep(1500);
    SPRESULT Res = SP_OK;
    if (m_bFP)
    {
        Res = SP_BBAT_FP_MicReadStatus(m_pImp->m_hDUT, m_byMicId);
    }
    else
    {
        Res = SP_BBAT_MicReadStatus(m_pImp->m_hDUT, m_byMicId);
    }

    if (Res != SP_OK)
    {
        m_pImp->_UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Read Status");
        return Res;
    }
    m_pImp->_UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Read Status");

    return SP_OK;
}

SPRESULT CAudioLoopBase::Phone_MicReadData()
{
    BYTE arrDataRecv[1600] = { 0 };

    SPRESULT Res = SP_OK;
    if (m_bFP)
    {
        Res = SP_BBAT_FP_MicReadData(m_pImp->m_hDUT, m_byMicId, arrDataRecv);
    }
    else
    {
        Res = SP_BBAT_MicReadData(m_pImp->m_hDUT, m_byMicId, arrDataRecv);
    }
    if (Res != SP_OK)
    {
        m_pImp->_UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Read Data");
        return Res;
    }
    m_pImp->_UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Read Data");

    m_pImp->CalcMicData(arrDataRecv, m_nMic8k, m_tResult);
    CHKRESULT(CheckResult());
    return SP_OK;
}

SPRESULT CAudioLoopBase::Phone_MicClose()
{
    SPRESULT Res = SP_OK;
    if (m_bFP)
    {
        Res = SP_BBAT_FP_MicClose(m_pImp->m_hDUT, m_byMicId);
    }
    else
    {
        Res = SP_BBAT_MicClose(m_pImp->m_hDUT, m_byMicId);
    }
    if (Res != SP_OK)
    {
        m_pImp->_UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Close");
        return Res;
    }
    m_pImp->_UiSendMsg(m_pMicMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Close");
    return SP_OK;
}

SPRESULT CAudioLoopBase::Phone_SpkPlayByDataSend()
{
    SPRESULT Res = SP_OK;
    int arrSpkData[1024] = { 0 };
    m_pImp->GenerateSpeakerData(arrSpkData);
    if (m_bFP)
    {
        Res = SP_BBAT_FP_SpeakerPlayByDataSend(m_pImp->m_hDUT, m_bySpkId, arrSpkData);
    }
    else
    {
        Res = SP_BBAT_SpeakerPlayByDataSend(m_pImp->m_hDUT, m_bySpkId, arrSpkData);
    }

    if (SP_OK != Res)
    {
        m_pImp->_UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Send Data");
        return Res; //
    }
    m_pImp->_UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Send Data");
    return SP_OK;
}

SPRESULT CAudioLoopBase::Phone_SpkClose()
{
    SPRESULT Res = SP_OK;
    if (m_bFP)
    {
        Res = SP_BBAT_FP_SpeakerClose(m_pImp->m_hDUT, m_bySpkId);
    }
    else
    {
        Res = SP_BBAT_SpeakerClose(m_pImp->m_hDUT, m_bySpkId);
    }

    if (Res != SP_OK)
    {
        m_pImp->_UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Close");
        return Res; //
    }
    m_pImp->_UiSendMsg(m_pSpeakerMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Close");

    return SP_OK;
}

SPRESULT CAudioLoopBase::AFCB_AudioLoopSelect(bool bStatus)
{
    int nLoopIn = m_nLoopIn;
    int nLoopOut = m_nLoopOut;
    if (!bStatus)
    {
        nLoopIn = 0;
        nLoopOut = 0;
    }
    if (m_pImp->m_pObjFunBox->FB_Audio_Loop_Select(nLoopIn, nLoopOut) == -1)
    {
        m_pImp->_UiSendMsg("G3: Loop Select", LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr,
            "Loop Select: %d, %d", nLoopIn, nLoopOut);
        return SP_E_BBAT_VALUE_FAIL;
    }
    m_pImp->_UiSendMsg("G3: Loop Select", LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr,
        "Loop Select: %d, %d", nLoopIn, nLoopOut);
    return SP_OK;
}

SPRESULT CAudioLoopBase::AFCB_AmpEnable(bool bStatus)
{
    LPCSTR lpMainMic[2] =
    {
        "G3: Main Mic Amp Close",
        "G3: Main Mic Amp Open"
    };
    LPCSTR lpSubMic[2] =
    {
        "G3: Sub Mic Amp Close",
        "G3: Sub Mic Amp Open"
    };
    if (m_byMicId == MainMic)
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Main_Mic_Amp_EN(bStatus), lpMainMic[bStatus]);
    }
    else if (m_byMicId == SubMic)
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Sub_Mic_Amp_EN(bStatus), lpSubMic[bStatus]);
    }
    return SP_OK;
}

SPRESULT CAudioLoopBase::AFCB_PA0_Enable(bool bStatus)
{
    LPCSTR lpName[2] =
    {
        "G3: PA0 Close",
        "G3: PA0 Open"
    };
    if (bStatus)
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Inter_Audio0_OPEN_X(), lpName[bStatus]);
    }
    else
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Inter_Audio0_CLOSE_X(), lpName[bStatus]);
    }
    return SP_OK;
}

SPRESULT CAudioLoopBase::AFCB_PA1_Enable(bool bStatus)
{
    LPCSTR lpName[2] =
    {
        "G3: PA1 Close",
        "G3: PA1 Open"
    };
    if (bStatus)
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Inter_Audio1_OPEN_X(), lpName[bStatus]);
    }
    else
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Inter_Audio1_CLOSE_X(), lpName[bStatus]);
    }
    return SP_OK;
}

void CAudioLoopBase::SetParameter()
{
    if (m_eTestType == G3)
    {
        if (m_eBaseMic == BASE_MAIN_MIC)
        {
            m_pMicMsg = m_pImp->BBAT_MIC_CHANNEL_NAME[Main_Mic];
            m_byMicId = MainMic;
        }
        else if (m_eBaseMic == BASE_SUB_MIC)
        {
            m_pMicMsg = m_pImp->BBAT_MIC_CHANNEL_NAME[Sub_Mic];
            m_byMicId = SubMic;
        }
        else if (m_eBaseMic == BASE_FP_MAIN_MIC)
        {
            m_pMicMsg = m_pImp->BBAT_FP_MIC_CHANNEL_NAME[Main_Mic];
            m_byMicId = MainMic;
            m_bFP = TRUE;
        }
    }
    else if (m_eTestType == PA_MIC)
    {
        if (m_eBaseMic == BASE_MAIN_MIC)
        {
            m_pMicMsg = m_pImp->BBAT_MIC_CHANNEL_NAME[Main_Mic];
            m_byMicId = MainMic;
        }
        else if (m_eBaseMic == BASE_SUB_MIC)
        {
            m_pMicMsg = m_pImp->BBAT_MIC_CHANNEL_NAME[Sub_Mic];
            m_byMicId = SubMic;
        }
        else if (m_eBaseMic == BASE_FP_MAIN_MIC)
        {
            m_pMicMsg = m_pImp->BBAT_FP_MIC_CHANNEL_NAME[Main_Mic];
            m_byMicId = MainMic;
            m_bFP = TRUE;
        }
    }

    if (m_eBaseSpk == BASE_MAIN_SPEAKER)
    {
        m_pSpeakerMsg = m_pImp->BBAT_SPK_CHANNEL_NAME[Main_Speaker];
        m_bySpkId = MainSpeaker;
    }
    else if (m_eBaseSpk == BASE_MAIN_RECEIVER)
    {
        m_pSpeakerMsg = m_pImp->BBAT_SPK_CHANNEL_NAME[Main_Receiver];
        m_bySpkId = MainReceiver;
    }
    else if (m_eBaseSpk == BASE_HEADSET)
    {
        m_pSpeakerMsg = m_pImp->BBAT_SPK_CHANNEL_NAME[Head_Set];
        m_bySpkId = HeadSet;
    }
    else if (m_eBaseSpk == BASE_SUB_SPEAKER)
    {
        m_pSpeakerMsg = m_pImp->BBAT_SPK_CHANNEL_NAME[Sub_Speaker];
        m_bySpkId = SubSpeaker;
    }
    else if (m_eBaseSpk == BASE_FP_SPEAKER)
    {
        m_pSpeakerMsg = m_pImp->BBAT_FP_SPK_CHANNEL_NAME[MainSpeaker];
        m_bySpkId = MainSpeaker;
        m_bFP = TRUE;
    }
    else if (m_eBaseSpk == BASE_FP_RECEIVER)
    {
        m_pSpeakerMsg = m_pImp->BBAT_FP_SPK_CHANNEL_NAME[Main_Receiver];
        m_bySpkId = MainReceiver;
        m_bFP = TRUE;
    }
}

SPRESULT CAudioLoopBase::CheckResult()
{
    m_pImp->_UiSendMsg("Check Peak", LEVEL_ITEM,
        m_dPeakSpec, m_tResult.dPeak, NOUPPLMT,
        nullptr, -1, nullptr,
        "Peak = %.2f, Spec = %.2f", 
        m_tResult.dPeak, m_dPeakSpec);
    if (m_dPeakSpec > m_tResult.dPeak)
    {
        return SP_E_PHONE_BBAT_COMMAND_FAIL;
    }

    m_pImp->_UiSendMsg("Check SNR", LEVEL_ITEM,
        m_dSnrSpec, m_tResult.dSNR, NOUPPLMT,
        nullptr, -1, nullptr,
        "SNR 1k = %.2f, Spec = %.2f",
        m_tResult.dSNR, m_dSnrSpec);
    if (!IN_RANGE(m_dSnrSpec, m_tResult.dSNR, NOUPPLMT))
    {
        return SP_E_PHONE_BBAT_COMMAND_FAIL;
    }

    return SP_OK;
}
