#pragma once
#include "NrAlgoGs.h"
#include "NrApiV5.h"

class CNrAlgoGsV5 :
    public CNrAlgoGs
{
private:

public:
    CNrAlgoGsV5(CImpBase* pImp);
    virtual ~CNrAlgoGsV5(void);

    virtual SPRESULT MakeGs();

protected:
    CNrApiV5 m_Api;

private:
    SPRESULT Load_PathInfo(NR_BAND_E Band, NR_CALI_RF_V5_PATH_INFO_T& PathInfo);

private:

};

