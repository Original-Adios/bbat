#pragma once
#include "BTMeasBase.h"

class CBTMeasBDR : public CBTMeasBase
{
public:
    virtual ~CBTMeasBDR(void);
	CBTMeasBDR(CImpBase *pImpBase, ICBTApi *pBtApi);//

protected:
	//////////////////////////////////////////////////////////////////////////
	// Measurement functions
	virtual BOOL MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult);
	virtual BOOL TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult);
};
