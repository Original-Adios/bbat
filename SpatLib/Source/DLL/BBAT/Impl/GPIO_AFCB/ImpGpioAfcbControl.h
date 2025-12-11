#pragma once
#include "../ImpBase.h"
#include "ImpGpioAfcb.h"
#include "GpioAfcbControl.h"
#include <vector>

class CImpGpioAfcbControl :public CImpGpioAfcb
{
    DECLARE_RUNTIME_CLASS(CImpGpioAfcbControl)

public:
    CImpGpioAfcbControl(void);
    virtual ~CImpGpioAfcbControl(void);

protected:
    void BuildSteps();
    SPRESULT GpioInit();
    SPRESULT GpioReset();
};
