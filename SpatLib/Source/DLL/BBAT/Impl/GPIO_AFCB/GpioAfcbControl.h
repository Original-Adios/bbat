#pragma once
#include "GpioAfcbBase.h"

//////////////////////////////////////////////////////////////////////////
class CGpioControl : public CGpioBase
{

public:
    CGpioControl(TGroupGpioParam tGroup, CImpBase* pImp);
    virtual ~CGpioControl(void);

protected:
    virtual SPRESULT Run();

private:
};


