#pragma once
#include "../ImpBase.h"


struct TGroupGpioParam
{
    AFCB_GPIO_NUMBER m_byGPIOX = AFCB_GPIO_C;
    BYTE byAfcbGpioNo = 0;
    BYTE byPhoneGpioNo = 0;
    bool bIsGPIOEx = false;
    BYTE bySetLevel = 0;
};

class CGpioBase
{
public:
    CGpioBase(TGroupGpioParam tGroup, CImpBase* pImp);
    virtual ~CGpioBase(void) = default;

    virtual SPRESULT Run();

    SPRESULT Phone_GpioRead0();
    SPRESULT Phone_GpioRead1();

    SPRESULT Phone_GpioWrite0();
    SPRESULT Phone_GpioWrite1();

    SPRESULT AFCB_GpioSetData0();
    SPRESULT AFCB_GpioSetData1();

    SPRESULT AFCB_GpioGetData0();
    SPRESULT AFCB_GpioGetData1();


    CImpBase* m_pImp = nullptr;
    LPCSTR m_arrAfcbGpioName[5] =
    {
        "GPIO_A",
        "GPIO_B",
        "GPIO_C",
        "GPIO_D",
        "IO"
    };
    AFCB_GPIO_NUMBER m_byGPIOX = AFCB_GPIO_C;
    BYTE m_byAfcbGpioNo = 0;
    BYTE m_byPhoneGpioNo = 0;
    bool m_bIsGPIOEx = false;
    BYTE m_bySetLevel = 0;
};