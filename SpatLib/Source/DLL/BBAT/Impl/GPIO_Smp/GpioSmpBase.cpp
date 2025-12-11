#include "StdAfx.h"
#include "GpioSmpBase.h"

CGpioSmpBase::CGpioSmpBase(TGroupGpioSmpParam tGroup, CImpBase* pImp)
{
    m_pImp = pImp;
    m_byWriteGpioNo = tGroup.byWriteGpioNo;
    m_byReadGpioNo = tGroup.byReadGpioNo;
}

SPRESULT CGpioSmpBase::Run()
{
    CHKRESULT(Phone_GpioWrite0());
    CHKRESULT(Phone_GpioRead0());
    CHKRESULT(Phone_GpioWrite1());
    CHKRESULT(Phone_GpioRead1());

    return SP_OK;
}

SPRESULT CGpioSmpBase::Phone_GpioRead0()
{
    int nValue = 0;
    CHKRESULT(m_pImp->PrintErrorMsg(
        SP_BBAT_GpioRead(m_pImp->m_hDUT, m_byReadGpioNo, &nValue),
        "Phone: Gpio Read 0", LEVEL_ITEM));

    m_pImp->_UiSendMsg("Phone: Gpio Read 0", LEVEL_ITEM,
        0, nValue, 0,
        nullptr, -1, nullptr,
        "Read %d Value = %d, Spec = 0", m_byReadGpioNo, nValue);
    if (nValue != 0)
    {
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }

    return SP_OK;
}

SPRESULT CGpioSmpBase::Phone_GpioRead1()
{
    int nValue = 0;
    CHKRESULT(m_pImp->PrintErrorMsg(
        SP_BBAT_GpioRead(m_pImp->m_hDUT, m_byReadGpioNo, &nValue),
        "Phone: Gpio Read 1", LEVEL_ITEM));

    m_pImp->_UiSendMsg("Phone: Gpio Read 1", LEVEL_ITEM,
        1, nValue, 1,
        nullptr, -1, nullptr,
        "Read %d Value = %d, Spec = 1", m_byReadGpioNo, nValue);
    if (nValue != 1)
    {
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }

    return SP_OK;
}

SPRESULT CGpioSmpBase::Phone_GpioWrite0()
{
    SPRESULT Res = SP_BBAT_GpioWritePullDown(m_pImp->m_hDUT, m_byWriteGpioNo);
    m_pImp->_UiSendMsg("Phone: Gpio Write 0", LEVEL_ITEM,
        0, 0, 0, nullptr, -1, nullptr,
        "Write %d", m_byWriteGpioNo);
    return Res;
}

SPRESULT CGpioSmpBase::Phone_GpioWrite1()
{
    SPRESULT Res = SP_BBAT_GpioWritePullUp(m_pImp->m_hDUT, m_byWriteGpioNo);
    m_pImp->_UiSendMsg("Phone: Gpio Write 1", LEVEL_ITEM,
        0, 0, 0, nullptr, -1, nullptr,
        "Write %d", m_byWriteGpioNo);
    return Res;
}
