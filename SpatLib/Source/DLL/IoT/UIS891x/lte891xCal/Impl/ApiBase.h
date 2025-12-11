#pragma once
#include "IApi.h"
#include "FuncBase.h"
#include "IRFDevice.h"

class CApiBase :
    public CFuncBase,
    public IApi
{
public:
    CApiBase(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    virtual ~CApiBase(void);

    virtual SPRESULT PreInit();

protected:
    SP_HANDLE  m_hDUT;
    IRFDevice* m_pRFTester; 
};
