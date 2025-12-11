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
    MAX_TYPE_HEADSET
};

class CHeadset : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CHeadset)
public:
    CHeadset(void);
    virtual ~CHeadset(void);

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
    SPRESULT Phone_CheckHeadset();

    SPRESULT RunHeadset_LR();
    SPRESULT RunWithAdc();
    SPRESULT RunWithoutAdc();


    SPRESULT RunMic();


    double m_dVoltageUp = 0.0;
    double m_dVoltageDown = 0.0;
    double m_dPeakSpec = 0.0;
    double m_dSnrSpec = 0.0;
    BOOL m_bSnr8k = FALSE;
    HEADSET_TYPE m_eHeadsetType = EUROPE;
    HeadsetTypeMic m_nHeadsetMicSpec = NoHeadset;
    BOOL m_bFP = FALSE;

};