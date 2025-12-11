#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////

enum TestType
{
    Single,
    Multi,
    MAX_TEST_TYPE
};

enum KeypadLightName
{
    LED8,
    LED9,
    LED10,
    LED11,
    LED12,
    LED13,
    LED14,
    LED15,
    MaxKeypadLight
};





class CKeypadLightAfcb : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CKeypadLightAfcb)
public:
    CKeypadLightAfcb(void);
    virtual ~CKeypadLightAfcb(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    SPRESULT KeypadLightAction();
    SPRESULT LightEnable(BOOL bStatus);
    SPRESULT KeypadLightOpen();
    SPRESULT KeypadLightClose();
    SPRESULT GetVoltageSingle(Times nTimes);
    SPRESULT GetVoltageMulti(Times nTimes);
    SPRESULT GetVoltage(float* pfVoltage, KeypadLightName nKeypadLightNo);
    SPRESULT SaveVoltage(Times nTimes, float fVoltage, KeypadLightName nKeypadLightNo);
    SPRESULT GetDelta(KeypadLightName nKeypadLightNo);
    SPRESULT GetDeltaSingle();
    SPRESULT GetDeltaMulti();
    SPRESULT RunKeypadLightTest();

    TestType m_eTestType = Single;
    KeypadLightName m_nKeypadLightNo = LED8;
    double m_arrVoltage_1[MAX_LED] = { 0.0 };
    double m_arrVoltage_2[MAX_LED] = { 0.0 };
    double m_arrDeltaVoltage[MAX_LED] = { 0.0 };
    double m_dUpSpec = 0.0;
    double m_dDownSpec = 0.0;
    int m_nSingleDelay = 500;
    int m_nMultiDelay = 500;
    LPCSTR m_lpKeyName[MAX_LED] =
    {
        "Keypad Light 1",
        "Keypad Light 2",
        "Keypad Light 3",
        "Keypad Light 4",
        "Keypad Light 5",
        "Keypad Light 6",
        "Keypad Light 7",
        "Keypad Light 8",
    };
};