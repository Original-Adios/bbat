#include "StdAfx.h"
#include "HallSensor.h"

//
IMPLEMENT_RUNTIME_CLASS(CHallSensor)

CHallSensor::CHallSensor(void)
{
}


CHallSensor::~CHallSensor(void)
{
}

SPRESULT CHallSensor::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    CHKRESULT(AFCB_PowerOn(true));
    Sleep(200);
    CHKRESULT(Phone_GpioRead());
    CHKRESULT(AFCB_PowerOn(false));
    return SP_OK;
}

BOOL CHallSensor::LoadXMLConfig(void)
{
    m_byGpioNo = (BYTE)GetConfigValue(L"Option:GPIO_NO", 0);
    return TRUE;
}

SPRESULT CHallSensor::AFCB_PowerOn(bool bStatus)
{
    LPCSTR lpName[2] =
    {
        "G3: EXT1 Power Off",
        "G3: EXT1 Power On"
    };
    RETURNSPRESULT(m_pObjFunBox->FB_EXT1_Power_Ctrl(bStatus), lpName[bStatus]);
    return SP_OK;
}

SPRESULT CHallSensor::Phone_GpioRead()
{
    int nValue = INVALID_POSITIVE_INTEGER_VALUE;
    SPRESULT Res = SP_BBAT_GpioRead(m_hDUT, m_byGpioNo, &nValue);
    if (SP_OK != Res || 0!= nValue)
    {
        _UiSendMsg("Phone: GPIO Read", LEVEL_ITEM,
            0, nValue, 0,
            nullptr, -1, nullptr,
            "GPIO No. = %d", m_byGpioNo);
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }
    _UiSendMsg("Phone: GPIO Read", LEVEL_ITEM,
        0, nValue, 0,
        nullptr, -1, nullptr,
        "GPIO No. = %d", m_byGpioNo);
    return SP_OK;
}
