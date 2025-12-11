#pragma once
//#include <vector>
#include "BTMeasBase.h"

using namespace std;

class CBTMeasBLE_BLE53 : public CBTMeasBase
{
public:
	virtual ~CBTMeasBLE_BLE53(void);
	CBTMeasBLE_BLE53(CImpBase *pImpBase, ICBTApi *pBtApi);//

protected:
	// Measurement functions
	virtual BOOL MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult);
	virtual BOOL TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult);

};
