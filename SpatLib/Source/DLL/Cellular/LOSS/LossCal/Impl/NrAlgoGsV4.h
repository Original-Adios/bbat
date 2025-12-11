#pragma once
#include "NrAlgoGs.h"
#include "NrApiV4.h"

class CNrAlgoGsV4 :
    public CNrAlgoGs
{
private:

public:
    CNrAlgoGsV4(CImpBase* pImp);
    virtual ~CNrAlgoGsV4(void);

    virtual SPRESULT MakeGs();

protected:
    CNrApiV4 m_Api;

private:
    SPRESULT Load_PathInfo(NR_BAND_E Band, NR_CALI_RF_CH_V4_T& PathInfo);

private:

};

