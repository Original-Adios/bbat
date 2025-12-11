#include "StdAfx.h"
#include "GpioAfcbSet.h"


CGpioSet::CGpioSet(TGroupGpioParam tGroup, CImpBase* pImp)
    :CGpioBase(tGroup, pImp)
{
}

CGpioSet::~CGpioSet(void)
{
}

SPRESULT CGpioSet::Run(void)
{
    CHKRESULT(AFCB_GpioSetData0());
    CHKRESULT(Phone_GpioRead0());
    CHKRESULT(AFCB_GpioSetData1());
    CHKRESULT(Phone_GpioRead1());
    return SP_OK;
}
