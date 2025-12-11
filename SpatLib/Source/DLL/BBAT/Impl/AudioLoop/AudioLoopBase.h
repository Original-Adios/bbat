#pragma once
#include "../ImpBase.h"

enum BASE_MIC_CHANNEL
{
    BASE_MAIN_MIC,
    BASE_SUB_MIC,
    BASE_FP_MAIN_MIC,
    BASE_MAX_MIC
};

enum BASE_SPEAKER_CHANNEL
{
    BASE_MAIN_SPEAKER,
    BASE_MAIN_RECEIVER,
    BASE_HEADSET,
    BASE_SUB_SPEAKER,
    BASE_FP_SPEAKER,
    BASE_FP_RECEIVER,
    BASE_MAX_SPEAKER
};

#pragma pack(4)
struct TGroupAudioLoop
{
    alTestType eTestType;
    BASE_MIC_CHANNEL eMicId;
    BASE_SPEAKER_CHANNEL eSpkId;
    BBAT_PA_MIC_CHANNEL ePaId;
    int nLoopIn;
    int nLoopOut;
    int nMic8K;
    int nFP;
    double dVoltageUpN;
    double dVoltageDownP;
    double dSnrSpec;
    double dPeakSpec;
};
#pragma pack()

class CAudioLoopBase
{
public:
    CAudioLoopBase(TGroupAudioLoop tGroup, CImpBase* pImp);
    virtual ~CAudioLoopBase(void) = default;

    virtual SPRESULT Run();
    SPRESULT Phone_MicOpen();
    SPRESULT Phone_MicReadStatus();
    SPRESULT Phone_MicReadData();
    SPRESULT Phone_MicClose();
    SPRESULT Phone_SpkPlayByDataSend();
    SPRESULT Phone_SpkClose();

    SPRESULT AFCB_AudioLoopSelect(bool bStatus);
    SPRESULT AFCB_AmpEnable(bool bStatus);
    SPRESULT AFCB_PA0_Enable(bool bStatus);
    SPRESULT AFCB_PA1_Enable(bool bStatus);
    void SetParameter();
    SPRESULT CheckResult();
    CImpBase* m_pImp;
    int m_nMic8k = 0;
    double m_dSnrSpec = 0.0;
    double m_dPeakSpec = 0.0;

    double m_dVoltageUpN = 0.0;
    double m_dVoltageDownP = 0.0;

    LPCSTR m_pMicMsg = nullptr;
    LPCSTR m_pSpeakerMsg = nullptr;
    BOOL m_bFP = FALSE;

    alTestType m_eTestType;
    BASE_MIC_CHANNEL m_eBaseMic = BASE_MAIN_MIC;
    BASE_SPEAKER_CHANNEL m_eBaseSpk = BASE_MAIN_SPEAKER;
    BBAT_MIC_CHANNEL_BYTE m_byMicId = MainMic;
    BBAT_SPK_CHANNEL_BYTE m_bySpkId = MainSpeaker;
    BBAT_PA_MIC_CHANNEL m_ePaId = PA0;
    int m_nLoopIn = 1;
    int m_nLoopOut = 1;
    AudioResult m_tResult;

};

