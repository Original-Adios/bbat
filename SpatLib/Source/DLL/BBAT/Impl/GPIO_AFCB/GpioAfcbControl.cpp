#include "StdAfx.h"
#include "GpioAfcbControl.h"


CGpioControl::CGpioControl(TGroupGpioParam tGroup, CImpBase* pImp)
    :CGpioBase(tGroup, pImp)
{
}

CGpioControl::~CGpioControl(void)
{
}

SPRESULT CGpioControl::Run(void)
{
    if (m_bySetLevel)
    {
        CHKRESULT(AFCB_GpioSetData1());
    }
    else
    {
        
        CHKRESULT(AFCB_GpioSetData0());
    }

    return SP_OK;
}
