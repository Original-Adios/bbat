#pragma once
//#include <vector>
#include "BTMeasBase.h"

using namespace std;

class CBTMeasBLEExUDS710 : public CBTMeasBase
{
public:
	virtual ~CBTMeasBLEExUDS710(void);
	CBTMeasBLEExUDS710(CImpBase *pImpBase, ICBTApi *pBtApi);//

protected:
	// Measurement functions
	virtual BOOL MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult);
	virtual BOOL TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult);

};
