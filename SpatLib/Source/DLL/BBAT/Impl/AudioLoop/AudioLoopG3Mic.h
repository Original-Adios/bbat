#pragma once
#include "../ImpBase.h"
#include "../Headset/Headset.h"

//////////////////////////////////////////////////////////////////////////

class CAudioLoopG3 : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAudioLoopG3)
public:
    CAudioLoopG3(void);
    virtual ~CAudioLoopG3(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL    LoadXMLConfig(void);

private:
    SPRESULT AudioLoopAction();
    enum AFCB_MIC_TYPE: BYTE 
    {
        AFCB_MIC_TYPE_COMADC,
        AFCB_MIC_TYPE_SPK,
        AFCB_MIC_TYPE_REC = 3,
        AFCB_MIC_TYPE_HEADSET = 5
    };

    double m_dSnrSpec = 0.0;
    double m_dPeakSpec = 0.0;
    AFCB_ADC_Channel_TYPE m_eAdcCh = Main_Mic_P;
    //BYTE m_byAfcbSpeaker = 0;
    LPCSTR m_pSpeakerMsg = nullptr;

    BBAT_SPK_CHANNEL_BYTE m_bySpkId = MainSpeaker;

    int m_nLoopIn = 1;
    int m_nLoopOut = 1;

    SPRESULT Phone_SpkPlayByDataSend();
    SPRESULT Phone_SpkClose();

    SPRESULT AFCB_WaveGet();
    SPRESULT AFCB_WaveSet(bool bStatus);

    SPRESULT AFCB_AudioLoopSelect(bool bStatus);

    SPRESULT AFCB_AmpEnable(bool bStatus);

    SPRESULT AFCB_Headmic(HEADSET_TYPE nType);
    SPRESULT AFCB_HeadSet_LR(HEADSET_LR eHeadset);
#define BYSEND_BUFF_SIZE  (0x400)

};