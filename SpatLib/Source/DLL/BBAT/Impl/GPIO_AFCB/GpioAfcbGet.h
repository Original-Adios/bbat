#pragma once
#include "GpioAfcbBase.h"

//////////////////////////////////////////////////////////////////////////
class CGpioGet : public CGpioBase
{

public:
    CGpioGet(TGroupGpioParam tGroup, CImpBase* pImp);
    virtual ~CGpioGet(void);

protected:
    virtual SPRESULT Run();

private:
};
