#pragma once
#include "ImpBaseUIS8910.h"
#include "ILossFunc_UIS8910.h"

class CCalcLSUIS8910 :
    public CImpBaseUIS8910
{
    DECLARE_RUNTIME_CLASS(CCalcLSUIS8910)
public:
    CCalcLSUIS8910(void);
    virtual ~CCalcLSUIS8910(void);
    virtual SPRESULT   __InitAction(void);
    virtual SPRESULT   __PollAction(void);
    virtual BOOL       LoadXMLConfig(void);
    SPRESULT LoadWifiUeInfo(void);
    int m_wifi_UeInfo[20];

    std::vector<double> m_vecFreq;
    std::vector<double> m_vecLoss;
private:
    SPRESULT CheckResult(SPRESULT spRlt);

};
