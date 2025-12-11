#pragma once
#include "IRFDevice.h"
#include "XException.h"

//////////////////////////////////////////////////////////////////////////
class CInstrumentLock sealed
{
public:
    CInstrumentLock(IRFDevice* pRF, DWORD dwTimeOut = 2000000, BOOL bThrowException = TRUE )  //部分Action测试时间较长超过1000s，需要调整超时时间到2000s
        : m_pRF(pRF)
        , m_bLocked(FALSE)
    {
        uint8 nDeviceNumber = 0;
        pRF->GetProperty(DP_DEVICE_NUMBER, 0, &nDeviceNumber);

        if (NULL != m_pRF)
        {
            if (1 == nDeviceNumber)
            {
                SPRESULT res = m_pRF->Lock(dwTimeOut);
                if (SP_OK != res && bThrowException)
                {
                    m_bLocked = FALSE;
                    THROW_X("Instrument locked timeout, %d", dwTimeOut);
                }
                else
                {
                    m_bLocked = TRUE;
                }
            }
            else
            {
                m_bLocked = FALSE;
            }
        }
    }

    BOOL IsLocked(void)const { return m_bLocked; };

    ~CInstrumentLock(void)
    {
        // If exception happens in constructor, destructor will not be invoked
        if (NULL != m_pRF && m_bLocked)
        {
            // Here ignore the return value of UnLock to avoid exception throw in destructor
            m_pRF->UnLock();
            m_bLocked = FALSE;
        }
    }

private:
    CInstrumentLock(CONST CInstrumentLock &other);
    CInstrumentLock &operator =(CONST CInstrumentLock &other);

    IRFDevice*  m_pRF;
    BOOL    m_bLocked;
};

