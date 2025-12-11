#pragma once
#include "../ImpBase.h"
#include "AudioLoopAfcb.h"
#include "AudioLoopPA.h"
#include <vector>

class CImpAudioLoopBase :public CImpBase
{
    DECLARE_RUNTIME_CLASS(CImpAudioLoopBase)

public:
    CImpAudioLoopBase(void);
    virtual ~CImpAudioLoopBase(void);

protected:
    virtual void __EnterAction(void);
    virtual SPRESULT __PollAction(void);
    virtual void __LeaveAction(void);
    virtual SPRESULT __FinalAction(void);
    virtual BOOL LoadXMLConfig(void);
    SPRESULT AudioLoopAction();
    void BuildSteps();
    //   void Print();
    LPCWSTR m_lpwTestType[2] =
    {
        L"AudioLoop_PA",
        L"AudioLoop_G3"
    };
    std::vector<CAudioLoopBase*> m_vecSteps;
    std::vector<TGroupAudioLoop> m_vecTestItems;
    int m_nGroupSize = 0;
    LPCWSTR m_lpwMicName[BASE_MAX_MIC] =
    {
        L"MainMic",
        L"SubMic",
        L"FP_MainMic"
    };
    LPCWSTR m_lpwSpkName[BASE_MAX_SPEAKER] =
    {
        L"MainSpeaker",
        L"MainReceiver",
        L"HeadSet",
        L"SubSpeaker",
        L"FP_Speaker",
        L"FP_Receiver"
    };
};
