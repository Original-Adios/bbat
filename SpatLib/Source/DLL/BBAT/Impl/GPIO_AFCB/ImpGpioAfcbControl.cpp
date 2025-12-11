#include "StdAfx.h"
#include "ImpGpioAfcbControl.h"
IMPLEMENT_RUNTIME_CLASS(CImpGpioAfcbControl)

CImpGpioAfcbControl::CImpGpioAfcbControl(void)
{
}
CImpGpioAfcbControl::~CImpGpioAfcbControl(void)
{
}

void CImpGpioAfcbControl::BuildSteps()
{
    size_t nVecSize = m_vecTestItems.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        CGpioBase* Step = nullptr;
        Step = new CGpioControl(m_vecTestItems[i], this);
        if (nullptr != Step)
        {
            m_vecSteps.push_back(Step);
        }
    }
}

SPRESULT CImpGpioAfcbControl::GpioInit()
{
    LPCSTR lpTemp = "G3: GPIOA_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOA, true), lpTemp);
    lpTemp = "G3: GPIOB_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOB, true), lpTemp);
    lpTemp = "G3: GPIOC_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOC, true), lpTemp);
    lpTemp = "G3: GPIOD_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOD, true), lpTemp);

    lpTemp = "G3: GPIO_DIRECTION_SET 17 1";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(17, 1), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 19 1";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(19, 1), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 21 1";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(21, 1), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 23 1";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(23, 1), lpTemp);

    lpTemp = "G3: FUNCTION_IO_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Function_IO_SetStatus_X(true), lpTemp);

    return SP_OK;
}

SPRESULT CImpGpioAfcbControl::GpioReset()
{
    return SP_OK;
}