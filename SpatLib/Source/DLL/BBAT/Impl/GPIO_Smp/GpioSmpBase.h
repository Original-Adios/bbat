#pragma once
#include "../ImpBase.h"


struct TGroupGpioSmpParam
{
    BYTE byWriteGpioNo = 0;
    BYTE byReadGpioNo = 0;
};
class CGpioSmpBase
{
public:
    CGpioSmpBase(TGroupGpioSmpParam tGroup, CImpBase* pImp);

    SPRESULT Run();

    SPRESULT Phone_GpioRead0();
    SPRESULT Phone_GpioRead1();

    SPRESULT Phone_GpioWrite0();
    SPRESULT Phone_GpioWrite1();

    CImpBase* m_pImp = nullptr;

    BYTE m_byWriteGpioNo = 0;
    BYTE m_byReadGpioNo = 0;
};