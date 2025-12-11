#pragma once
#include "../ImpBase.h"
 
//////////////////////////////////////////////////////////////////////////
class CUART0_Test : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUART0_Test)
public:
    CUART0_Test(void);
    virtual ~CUART0_Test(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    DWORD m_dwUartNo;
    DWORD m_dwBaudrate;
    DWORD m_iSleepTime;
};

