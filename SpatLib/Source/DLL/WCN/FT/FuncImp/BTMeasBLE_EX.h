#pragma once
//#include <vector>
#include "BTMeasBase.h"

using namespace std;

class CBTMeasBLEEx : public CBTMeasBase
{
public:
	virtual ~CBTMeasBLEEx(void);
	CBTMeasBLEEx(CImpBase *pImpBase, ICBTApi *pBtApi);//

protected:
	// Measurement functions
	virtual BOOL MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult);
	virtual BOOL TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult);

};
