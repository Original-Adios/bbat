#pragma once
#include "../ImpBase.h"
#include "LED_AFCB.h"
#include <vector>

class CImpLedAfcb :public CImpBase
{
    DECLARE_RUNTIME_CLASS(CImpLedAfcb)

public:
    CImpLedAfcb(void);
    virtual ~CImpLedAfcb(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual SPRESULT __FinalAction(void);
    virtual BOOL LoadXMLConfig(void);

    SPRESULT CImpLedAfcb::LedAction();

    void BuildSteps();
 //   void Print();

    std::vector<CLedAfcb*> m_vecSteps;
    std::vector<TGroupLed> m_vecTestItems;
    int m_nGroupSize = 0;
    LPCWSTR m_lpwFlashName[MAX_FLASH] =
    {
        L"FlashCold",
        L"FlashWarm",
        L"FP_Flash",
        L"FrontFlash",
        L"KeypadLight",
        L"LedRed",
        L"LedGreen",
        L"LedBlue",
        L"Torch",
        L"GPIO"
    };

    LPCSTR m_lptFlashName[MAX_FLASH] =
    {
        "Flash Cold",
        "Flash Warm",
        "FP Falsh",
        "Front Flash",
        "Keypad Light",
        "Led Red",
        "Led Green",
        "Led Blue",
        "Torch"
    };
};
