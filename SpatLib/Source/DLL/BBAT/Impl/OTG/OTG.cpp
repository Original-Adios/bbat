#include "stdafx.h"
#include "OTG.h"

IMPLEMENT_RUNTIME_CLASS(COtg)
COtg::COtg(void)
{
}
COtg::~COtg(void)
{
}

SPRESULT COtg::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return OtgAction();
}

BOOL COtg::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT COtg::OtgAction()
{
    CHKRESULT(Phone_OtgDisable());
    CHKRESULT(AFCB_GpioInit(TRUE));
    CHKRESULT(ReadAction(FALSE));
    CHKRESULT(ReadAction(TRUE));
    CHKRESULT(AFCB_GpioInit(FALSE));
    return SP_OK;
}

SPRESULT COtg::Phone_OtgDisable()
{
    CHKRESULT(PrintErrorMsg
    (
        SP_BBAT_OtgDisable(m_hDUT),
        "OTG Disable",
        LEVEL_ITEM
    )
    );
    PrintSuccessMsg(SP_OK, "OTG Disable", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT COtg::Phone_OtgRead()
{
    int8 byReadValue = 0;
    CHKRESULT(PrintErrorMsg
    (
        SP_BBAT_OtgRead(m_hDUT, byReadValue),
        "OTG Read",
        LEVEL_ITEM
    )
    );
    m_nReadValue = (int)byReadValue;
    return SP_OK;
}

SPRESULT COtg::CheckReadValue(int nSpec)
{
    _UiSendMsg("OTG Read", LEVEL_ITEM,
        nSpec, m_nReadValue, nSpec,
        nullptr, -1, nullptr,
        "Read Value = %d", m_nReadValue);
    if (nSpec != m_nReadValue)
    {
        return SP_E_PHONE_BBAT_COMMAND_FAIL;
    }
    return SP_OK;
}

SPRESULT COtg::AFCB_GpioInit(BOOL bStatus)
{
    LPCSTR arrGpioB[2] =
    {
        "GPIO B Disable",
        "GPIO B Enable"
    };
    LPCSTR arrDirection[2] =
    {
        "Direction False",
        "Direction True"
    };
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOB,bStatus), arrGpioB[bStatus]);
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(GPIO_B_DIR_REG, bStatus), arrDirection[bStatus]);
    return SP_OK;
}

SPRESULT COtg::AFCB_GpioSet(BOOL bStatus)
{
    LPCSTR lpGpio[2] =
    {
        "Gpio B4 False",
        "Gpio B4 True"
    };
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetData_X(GPIO_B, GPIO_Data4, (BYTE)bStatus), lpGpio[bStatus]);
    return SP_OK;
}

SPRESULT COtg::ReadAction(BOOL bStatus)
{
    CHKRESULT(AFCB_GpioSet(bStatus));
    CHKRESULT(Phone_OtgRead());
    CHKRESULT(CheckReadValue(bStatus));
    return SP_OK;
}
