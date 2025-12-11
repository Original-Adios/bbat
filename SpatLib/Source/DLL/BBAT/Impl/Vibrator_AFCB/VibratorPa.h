#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////


class CVibratorPa : public CVibratorAfcb
{
    DECLARE_RUNTIME_CLASS(CVibratorPa)
public:
    CVibratorPa(void);
    virtual ~CVibratorPa(void);

private:
    virtual SPRESULT VibAction();
    virtual SPRESULT AFCB_PowerCurrentGet(float* pfCurrentf);
    virtual SPRESULT GetResult();
    float m_fOpenCurrent = 0.0f;
    float m_fCloseCurrent = 0.0f;
};