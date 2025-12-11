#pragma once
#include "../ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CChipConnect : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CChipConnect)
public:
    CChipConnect(void);
    virtual ~CChipConnect(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL    LoadXMLConfig(void);


private:
    BOOL    m_bAccelerometer;
    BOOL    m_bMagnetic;
    BOOL    m_bOrientation;
    BOOL    m_bGyroscope;
    BOOL    m_bLight;
    BOOL    m_bPressure;
    BOOL    m_bTemperature;
    BOOL    m_bProximity;
    BOOL    m_bGravity;
    BOOL    m_bLinear;
    BOOL    m_bRotation;
    BOOL    m_bFinger;
    BOOL    m_bHeartRate;
    BOOL    m_Blood;
    BOOL    m_Sar;
    BOOL    m_Step;

};
