#include "StdAfx.h"
#include "GpioAfcbGet.h"


CGpioGet::CGpioGet(TGroupGpioParam tGroup, CImpBase* pImp)
    :CGpioBase(tGroup, pImp)
{
}

CGpioGet::~CGpioGet(void)
{
}

SPRESULT CGpioGet::Run(void)
{
    CHKRESULT(Phone_GpioWrite0());
    CHKRESULT(AFCB_GpioGetData0());
    CHKRESULT(Phone_GpioWrite1());
    CHKRESULT(AFCB_GpioGetData1());
    return SP_OK;
}