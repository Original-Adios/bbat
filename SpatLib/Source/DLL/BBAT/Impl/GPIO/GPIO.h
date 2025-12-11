#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CGPIO : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CGPIO)
public:
    CGPIO(void);
    virtual ~CGPIO(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    int m_nReadGPIONo = 0;
    int m_nWriteGPIONo = 0;
    int m_nRead = 0;
    int m_nWrite = 0;
    int m_nCompare = 0;
    int m_nCompareValue = 0;
    int m_nLevel = 0;

    SPRESULT phGpioWrite();
    SPRESULT phGpioRead();
    SPRESULT phGpioWritePullUp();
    SPRESULT phGpioWritePullDown();
};


