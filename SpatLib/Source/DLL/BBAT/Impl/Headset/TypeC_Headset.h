#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
enum HEADSET_LR
{
    HEADSET_L,
    HEADSET_R,
    MAX_LR
};

enum HEADSET_TYPE
{
    EUROPE = 1,
    AMERICA,
    NC,
    MAX_TYPE_HEADSET
};

class CTypeCHeadset : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CTypeCHeadset)
public:
    CTypeCHeadset(void);
    virtual ~CTypeCHeadset(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    SPRESULT HeadsetAction();
    SPRESULT AFCB_Headmic(HEADSET_TYPE nType);
    SPRESULT AFCB_AdcEnable(BOOL bStatus);
    SPRESULT Phone_HeadsetMicOpen();
    SPRESULT Phone_HeadsetOpen();
    SPRESULT AFCB_AdcGetVoltage();
    SPRESULT Phone_HeadsetMicClose();
    SPRESULT AFCB_HeadSet_LR(HEADSET_LR eHeadset);
    SPRESULT Phone_HeadsetMicCheck();
    SPRESULT Phone_HeadsetMicRead();
    SPRESULT Phone_HeadsetClose();

    SPRESULT RunHeadset_LR();
    SPRESULT RunWithAdc();

    SPRESULT RunMic();


    double m_dVoltageUp = 0.0;
    double m_dVoltageDown = 0.0;
    double m_dPeakSpec = 0.0;
    double m_dSnrSpec = 0.0;
    BOOL m_bSnr8k = FALSE;


};