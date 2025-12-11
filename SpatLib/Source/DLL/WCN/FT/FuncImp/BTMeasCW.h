#pragma once
//#include <vector>
#include "BTMeasBase.h"

using namespace std;

class CBTMeasCW : public CBTMeasBase
{
public:
    virtual ~CBTMeasCW(void);
    CBTMeasCW(CImpBase* pImpBase, ICBTApi* pBtApi);//

protected:
    // Measurement functions
    virtual BOOL MeasureUplink(BTMeasParamChan* pUplinkMeasParam, SPBT_RESULT_T* pTestResult);
    virtual BOOL TestPER(BTMeasParamChan* pDownlinkMeasParam, SPBT_RESULT_T* pRxReult);
};
