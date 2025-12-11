#include "StdAfx.h"
#include "GpioAfcbBase.h"

CGpioBase::CGpioBase(TGroupGpioParam tGroup, CImpBase* pImp)
{
    m_pImp = pImp;
    m_byGPIOX = tGroup.m_byGPIOX;
    m_byAfcbGpioNo = tGroup.byAfcbGpioNo;
    m_byPhoneGpioNo = tGroup.byPhoneGpioNo;
    m_bIsGPIOEx = tGroup.bIsGPIOEx;
    m_bySetLevel = tGroup.bySetLevel;
}

SPRESULT CGpioBase::Run()
{
    return SP_OK;
}

SPRESULT CGpioBase::Phone_GpioRead0()
{
    int nValue = 0;
    if (m_bIsGPIOEx)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_GpioReadEx(m_pImp->m_hDUT, m_byPhoneGpioNo, &nValue),
            "Phone: Gpio Read 0", LEVEL_ITEM));
    }
    else
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_GpioRead(m_pImp->m_hDUT, m_byPhoneGpioNo, &nValue),
            "Phone: Gpio Read 0", LEVEL_ITEM));
    }
    m_pImp->_UiSendMsg("Phone: Gpio Read 0", LEVEL_ITEM,
        0, nValue, 0,
        nullptr, -1, nullptr,
        "Read Value = %d, Spec = 0", nValue);
    if (nValue != 0)
    {
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }

    return SP_OK;
}

SPRESULT CGpioBase::Phone_GpioRead1()
{
    int nValue = 0;
    if (m_bIsGPIOEx)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_GpioReadEx(m_pImp->m_hDUT, m_byPhoneGpioNo, &nValue),
            "Phone: Gpio Read 1", LEVEL_ITEM));
    }
    else
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_GpioRead(m_pImp->m_hDUT, m_byPhoneGpioNo, &nValue),
            "Phone: Gpio Read 1", LEVEL_ITEM));
    }    
    m_pImp->_UiSendMsg("Phone: Gpio Read 1", LEVEL_ITEM,
        1, nValue, 1,
        nullptr, -1, nullptr,
        "Read Value = %d, Spec = 1", nValue);
    if (nValue != 1)
    {
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }

    return SP_OK;
}

SPRESULT CGpioBase::Phone_GpioWrite0()
{
    SPRESULT Res = SP_OK;
    if (m_bIsGPIOEx)
    {
        Res = SP_BBAT_GpioWritePullDownEx(m_pImp->m_hDUT, m_byPhoneGpioNo);
    }
    else
    {
        Res = SP_BBAT_GpioWritePullDown(m_pImp->m_hDUT, m_byPhoneGpioNo);
    }    
    m_pImp->_UiSendMsg("Phone: Gpio Write 0", LEVEL_ITEM,
        0, 0, 0, nullptr, -1, nullptr,
        "Write %d", m_byPhoneGpioNo);
    return Res;
}

SPRESULT CGpioBase::Phone_GpioWrite1()
{
    SPRESULT Res = SP_OK;
    if (m_bIsGPIOEx)
    {
        Res = SP_BBAT_GpioWritePullUpEx(m_pImp->m_hDUT, m_byPhoneGpioNo);
    }
    else
    {
        Res = SP_BBAT_GpioWritePullUp(m_pImp->m_hDUT, m_byPhoneGpioNo);
    }    
    m_pImp->_UiSendMsg("Phone: Gpio Write 1", LEVEL_ITEM,
        0, 0, 0, nullptr, -1, nullptr,
        "Write %d", m_byPhoneGpioNo);
    return Res;
}

SPRESULT CGpioBase::AFCB_GpioSetData0()
{
    int nRes = m_pImp->m_pObjFunBox->FB_GPIO_SetData_X(m_byGPIOX, m_byAfcbGpioNo, 0);
    m_pImp->_UiSendMsg("G3: GPIO Set Data 0", LEVEL_ITEM,
        0, nRes, 0,
        nullptr, -1, nullptr,
        "G3 Port = %s%d", m_arrAfcbGpioName[m_byGPIOX], m_byAfcbGpioNo);
    if (nRes == -1)
    {
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CGpioBase::AFCB_GpioSetData1()
{
    int nRes = m_pImp->m_pObjFunBox->FB_GPIO_SetData_X(m_byGPIOX, m_byAfcbGpioNo, 1);
    m_pImp->_UiSendMsg("G3: GPIO Set Data 1", LEVEL_ITEM,
        0, nRes, 0,
        nullptr, -1, nullptr,
        "G3 Port = %s%d", m_arrAfcbGpioName[m_byGPIOX], m_byAfcbGpioNo);
    if (nRes == -1)
    {
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CGpioBase::AFCB_GpioGetData0()
{
    BYTE byReturn = 0;
    int nRes = m_pImp->m_pObjFunBox->FB_GPIO_GetData_X(m_byGPIOX, m_byAfcbGpioNo, &byReturn);
    m_pImp->_UiSendMsg("G3: GPIO Get Data 0", LEVEL_ITEM,
        0, nRes, 0,
        nullptr, -1, nullptr,
        "G3 Port = %s%d", m_arrAfcbGpioName[m_byGPIOX], m_byAfcbGpioNo);
    if (nRes != 0 || byReturn != 0 )
    {
        m_pImp->PrintErrorMsg(SP_E_BBAT_GPIO_VALUE_FAIL, "G3: GPIO Get Data 0", LEVEL_ITEM);
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CGpioBase::AFCB_GpioGetData1()
{
    BYTE byReturn = 0;
    int nRes = m_pImp->m_pObjFunBox->FB_GPIO_GetData_X(m_byGPIOX, m_byAfcbGpioNo, &byReturn);
    m_pImp->_UiSendMsg("G3: GPIO Get Data 1", LEVEL_ITEM,
        0, nRes, 0,
        nullptr, -1, nullptr,
        "G3 Port = %s%d", m_arrAfcbGpioName[m_byGPIOX], m_byAfcbGpioNo);
    if (nRes != 0 || byReturn != 1)
    {
        m_pImp->PrintErrorMsg(SP_E_BBAT_GPIO_VALUE_FAIL, "G3: GPIO Get Data 1", LEVEL_ITEM);
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }
    return SP_OK;
}
