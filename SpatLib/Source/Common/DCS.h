#pragma once
#include "IDCS.h"

#define V2mV(x)       ((x)*1000)
#define A2mA(x)       ((x)*1000)   
//////////////////////////////////////////////////////////////////////////
class CDCS
{
public:
    CDCS(IDCS* pDC = NULL, INT nChannel = 1);
    virtual ~CDCS(void);
    static const float INVALID_CURRENT; 

    void     BindingDC(IDCS* pDC, INT nChannel = 1);
    BOOL     IsEmptyDC(void) { return NULL == m_pDC; };

    SPRESULT SwitchOff(void);

    SPRESULT GetVoltage(float& fV);
    SPRESULT SetVoltage(float  fV);
    
    SPRESULT SetCurrentRange(float mA);
    SPRESULT GetCurrent(float& fmA);
    SPRESULT GetCurrent(uint32 u32TimeSpan, float& fmA);
    SPRESULT GetAccurateCurrent(float& fmA, uint32 u32TimeSpan = 1000);

protected:
    SPRESULT GetTimeSpanCurrent(float& fmA, uint32 u32TimeSpan = 1000, BOOL bExcludeNagtive = TRUE);
    
private:
    IDCS*    m_pDC;
    INT m_nChannel;
};
