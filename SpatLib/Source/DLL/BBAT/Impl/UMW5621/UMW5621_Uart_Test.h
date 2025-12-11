#pragma once
#include "../ImpBase.h"
 
//////////////////////////////////////////////////////////////////////////
class CUMW5621_Uart_Test : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUMW5621_Uart_Test)
public:
    CUMW5621_Uart_Test(void);
    virtual ~CUMW5621_Uart_Test(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    DWORD m_dwUartNo;
    DWORD m_dwBaudrate;
};


