#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////


#pragma pack(4)
struct TGroupLed
{
    int nGpioNo;
    FlashType eFlashType;
    LedType eLedType;
    double dVoltageDown;
    double dVoltageUp;
};
#pragma pack()

class CLedAfcb
{
//    DECLARE_RUNTIME_CLASS(CLedAfcb)
public:
    CLedAfcb(TGroupLed tGroup, CImpBase* pImp);
    ~CLedAfcb(void);

    SPRESULT Run(void);

private:
    SPRESULT LightLedSet(BOOL bStatus);
    SPRESULT LedOpen();
    SPRESULT LedClose();
    SPRESULT AfcbLed(float* pfVoltage);
    SPRESULT GetVoltage(Times eTimes);
    SPRESULT DeltaVoltage();

    SPRESULT GpioWriteUp();
    SPRESULT GpioWriteDown();
    CImpBase* m_pImp = nullptr;
    FlashType m_eFlashType = FlashCold;
    LedType m_eLedType = LED0;
    double m_dVoltage1 = 0.0;
    double m_dVoltage2 = 0.0;
    double m_dVoltageDelta = 0.0;
    double m_dVoltageUpSpec = 0.0;
    double m_dVoltageDownSpec = 0.0;
    int m_nGpioNo = 0;
    LPCSTR m_lpFlashName[MAX_FLASH] =
    {
        "Flash Cold",
        "Flash Warm",
        "FP Falsh",
        "Front Flash",
        "Keypad Light",
        "Led Red",
        "Led Green",
        "Led Blue",
        "Torch",
        "GPIO"
    };
};