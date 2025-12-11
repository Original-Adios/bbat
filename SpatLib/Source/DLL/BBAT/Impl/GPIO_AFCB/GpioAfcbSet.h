#pragma once
#include "GpioAfcbBase.h"

//////////////////////////////////////////////////////////////////////////
class CGpioSet : public CGpioBase
{

public:
    CGpioSet(TGroupGpioParam tGroup, CImpBase* pImp);
    virtual ~CGpioSet(void);

protected:
    virtual SPRESULT Run();

private:
};


