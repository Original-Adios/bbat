#pragma once
#include "NrAlgoClc.h"
#include "NrApiV5.h"


class CNrAlgoClcV5 :
    public CNrAlgoClc
{
public:
    CNrAlgoClcV5(CImpBase* pImp);
    virtual ~CNrAlgoClcV5(void);

protected:
    CNrApiV5 m_Api;
};

