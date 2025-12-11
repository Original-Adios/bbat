#include "StdAfx.h"
#include "GPIO.h"

//
IMPLEMENT_RUNTIME_CLASS(CGPIO)

//////////////////////////////////////////////////////////////////////////
CGPIO::CGPIO(void)
{

}

CGPIO::~CGPIO(void)
{
}

SPRESULT CGPIO::__PollAction(void)
{
    if (m_nWrite)
    {
        CHKRESULT(phGpioWrite());
    }
    if (m_nRead)
    {
        CHKRESULT(phGpioRead());
    }
    return SP_OK;
}
BOOL CGPIO::LoadXMLConfig(void)
{
    m_nReadGPIONo = GetConfigValue(L"Option:Read:GPIO_Number", 0);
    m_nWriteGPIONo = GetConfigValue(L"Option:Write:GPIO_Number", 0);
    m_nRead = GetConfigValue(L"Option:Read:GPIO_Read", 0);
    m_nCompare = GetConfigValue(L"Option:Read:Compare", 0);
    m_nCompareValue = GetConfigValue(L"Option:Read:Compare_Value", 0);
    m_nWrite = GetConfigValue(L"Option:Write:GPIO_Write", 0);
    m_nLevel = GetConfigValue(L"Option:Write:Set_Level", 0);
    return TRUE;
}

SPRESULT CGPIO::phGpioWrite()
{
    if (m_nLevel)
    {
        CHKRESULT(phGpioWritePullUp());
    }
    else
    {
        CHKRESULT(phGpioWritePullDown());
    }
    return SP_OK;
}

SPRESULT CGPIO::phGpioRead()
{
    int nNumberRead = 0;
    CHKRESULT(PrintErrorMsg(SP_BBAT_GpioRead(m_hDUT, (int8)m_nReadGPIONo, &nNumberRead), "GPIO Read Fail", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "GPIO Read Pass", LEVEL_ITEM);
    if (m_nCompare)
    {
        _UiSendMsg("GPIO_Compare", LEVEL_ITEM,
            m_nCompareValue, nNumberRead, m_nCompareValue,
            nullptr, -1, nullptr);
        if (nNumberRead != m_nCompareValue)
        {
            return SP_E_BBAT_GPIO_VALUE_FAIL;
        }
    }

    return SP_OK;
}

SPRESULT CGPIO::phGpioWritePullUp()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_GpioWritePullUp(m_hDUT, (int8)m_nWriteGPIONo), "GPIO Write Pull Up Fail", LEVEL_ITEM));
    _UiSendMsg("GPIO Write Pull Up Pass", LEVEL_ITEM,
        0, 0, 0,
        nullptr, -1, nullptr,
        "GPIO Number = %d", m_nWriteGPIONo);
    return SP_OK;
}

SPRESULT CGPIO::phGpioWritePullDown()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_GpioWritePullDown(m_hDUT, (int8)m_nWriteGPIONo), "GPIO Write Pull Down Fail", LEVEL_ITEM));
    _UiSendMsg("GPIO Write Pull Down Pass", LEVEL_ITEM,
        0, 0, 0,
        nullptr, -1, nullptr,
        "GPIO Number = %d", m_nWriteGPIONo);
    return SP_OK;
}
