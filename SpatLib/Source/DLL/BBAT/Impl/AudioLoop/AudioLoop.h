#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////


class CAudioLoop : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAudioLoop)
public:
    CAudioLoop(void);
    virtual ~CAudioLoop(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL    LoadXMLConfig(void);


private:

    int m_nMic8k = 0;
    double m_fSnrSpec = 0.0;
    double m_fPeakSpec = 0.0;
    int m_nRetry = 0;
    int m_nLoopCount = 0;

    LPCSTR m_pMicMsg = nullptr;
    LPCSTR m_pSpeakerMsg = nullptr;


    BBAT_MIC_CHANNEL_BYTE m_byMicId = MainMic;
    BBAT_SPK_CHANNEL_BYTE m_bySpkId = MainSpeaker;

    SPRESULT phMicOpen();
    SPRESULT phMicReadStatus();
    SPRESULT phMicReadData();
    SPRESULT phMicClose();

    SPRESULT phSpkPlayByDataSend();
    SPRESULT phSpkClose();
    SPRESULT CheckHeadset();

    SPRESULT MicAction();
    SPRESULT MicReadAction();
};