#pragma once
#include "NrAlgoClc.h"
#include "NrApiV4.h"


class CNrAlgoClcV4 :
    public CNrAlgoClc
{
public:
    CNrAlgoClcV4(CImpBase* pImp);
    virtual ~CNrAlgoClcV4(void);

protected:
    CNrApiV4 m_Api;
};

