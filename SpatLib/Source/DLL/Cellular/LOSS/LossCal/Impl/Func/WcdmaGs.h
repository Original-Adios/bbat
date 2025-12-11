#pragma once

#include "WcdmaBase.h"
#include <list>

class CWcdmaGs : public CWcdmaBase
{
public:
    CWcdmaGs(CImpBase *pSpat);
    virtual ~CWcdmaGs(void);

    virtual SPRESULT Run();
    SPRESULT Run_V2();
    SPRESULT Run_V3();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void ConfigFreq();
private:
    SPRESULT TunePwr(double dPwr, SPRESULT spRlt, PWR_RLT &pwrRlt);
    SPRESULT InitSeq(PC_CALI_WCDMA_TX_REQ_T *pSeq);
    SPRESULT LoadNV();
    SPRESULT InitData();

	std::list<double> m_listFreq;

    BOOL m_bApt;
};
