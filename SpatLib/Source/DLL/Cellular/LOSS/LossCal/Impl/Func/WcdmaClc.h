#pragma once

#include "WcdmaBase.h"

class CWcdmaClc : public CWcdmaBase
{
public:
    CWcdmaClc(CImpBase *pSpat);
    virtual ~CWcdmaClc(void);

    virtual SPRESULT Run();
    SPRESULT Run_V2();
    SPRESULT Run_V3();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
private:
    SPRESULT TunePwr(double dPwr, SPRESULT spRlt, PWR_RLT &pwrRlt);
    SPRESULT InitSeq(PC_CALI_WCDMA_TX_REQ_T *pSeq);
    SPRESULT LoadNV();
    SPRESULT InitData();

    BOOL m_bApt;
};
