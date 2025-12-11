#pragma once
#include "BTMeasBase.h"

using namespace std;

class CBTGeneralMeasBLE : public CBTMeasBase
{
public:
	virtual ~CBTGeneralMeasBLE(void);
	CBTGeneralMeasBLE(CImpBase *pImpBase, ICBTApi *pBtApi);//

protected:
	// Measurement functions
	virtual BOOL MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult);
	virtual BOOL TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult);
};
