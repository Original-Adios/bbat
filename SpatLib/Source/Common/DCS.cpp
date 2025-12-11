#include "StdAfx.h"
#include "DCS.h"
#include "Utility.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
#define CheckDCObject()     { \
    if (NULL == m_pDC) { \
        assert(0); \
        return SP_E_INVALID_PARAMETER; \
    } \
}

//////////////////////////////////////////////////////////////////////////
const float CDCS::INVALID_CURRENT = (float)-99999.99;

CDCS::CDCS(IDCS* pDC, INT nChannel)
    : m_pDC(pDC)
    , m_nChannel(nChannel)
{
}

CDCS::~CDCS(void)
{
}

void CDCS::BindingDC(IDCS* pDC, INT nChannel)
{
    m_pDC = pDC;
    m_nChannel = nChannel;
}

SPRESULT CDCS::GetTimeSpanCurrent(float& fmA, uint32 u32TimeSpan/* = 1000*/, BOOL bExcludeNagtive/* = TRUE */)
{
    fmA = INVALID_CURRENT;
    CheckDCObject()

    ULONG uCount = 0;
    float fTotal = 0;
    CSPTimer timer;
    do
    {
        float  fCurr = 0.0f;
        SPRESULT res = m_pDC->GetCurrent(fCurr, m_nChannel);
        if (SP_OK != res)
        {
            if (SP_E_DC_OVERFLOW == res)
            {
                continue;
            }
            else
            {
                return res;
            }
        }
        else
        {
            if (bExcludeNagtive && fCurr < 0)
            {
                continue;
            }
            else
            {
                fTotal += fCurr;
                uCount++;
            }
        }

    } while (!timer.IsTimeOut(u32TimeSpan));

    if (uCount > 0)
    {
        fmA = fTotal / uCount;
        return SP_OK;
    }
    else
    {
        return SP_E_DC_OVERFLOW;
    }
}


SPRESULT CDCS::GetAccurateCurrent(float& fmA, uint32 u32TimeSpan/* = 1000*/)
{
    fmA = INVALID_CURRENT;

    CheckDCObject()

    CHKRESULT(m_pDC->GetCurrent(fmA, m_nChannel))
 
    const float LOW_CURRENT_RANGE = (float)A2mA(0.02);
    const float MID_CURRENT_RANGE = (float)A2mA(1.00);
    const float HIG_CURRENT_RANGE = (float)A2mA(3.00);

    if (fmA < LOW_CURRENT_RANGE)
    {
        CHKRESULT(m_pDC->SetCurrentRange(LOW_CURRENT_RANGE, m_nChannel));
        CHKRESULT(GetTimeSpanCurrent(fmA, u32TimeSpan, TRUE));
        CHKRESULT(m_pDC->SetCurrentRange(HIG_CURRENT_RANGE, m_nChannel));
    }
    else if (IN_RANGE(LOW_CURRENT_RANGE, fmA, MID_CURRENT_RANGE))
    {
        CHKRESULT(m_pDC->SetCurrentRange(MID_CURRENT_RANGE, m_nChannel));
        CHKRESULT(GetTimeSpanCurrent(fmA, u32TimeSpan, TRUE));
        CHKRESULT(m_pDC->SetCurrentRange(HIG_CURRENT_RANGE, m_nChannel));
    }
    else
    {
        CHKRESULT(GetTimeSpanCurrent(fmA, u32TimeSpan, TRUE));
    }

    return SP_OK;
}

SPRESULT CDCS::SwitchOff(void)
{
    return SetVoltage((float)-1.0);
}

SPRESULT CDCS::SetVoltage(float fV)
{
    CheckDCObject()
    return m_pDC->SetVoltage(fV, m_nChannel);
}

SPRESULT CDCS::GetVoltage(float& fV)
{
    CheckDCObject()
    return m_pDC->GetVotlage(fV, m_nChannel);
}

SPRESULT CDCS::SetCurrentRange(float mA)
{
    CheckDCObject()
    return m_pDC->SetCurrentRange(mA, m_nChannel);
}

SPRESULT CDCS::GetCurrent(float& fmA)
{
    CheckDCObject()
    return m_pDC->GetCurrent(fmA, m_nChannel);
}

SPRESULT CDCS::GetCurrent(uint32 u32TimeSpan, float& fmA)
{
    CheckDCObject()
    return m_pDC->GetCurrent(u32TimeSpan, fmA, m_nChannel);
}