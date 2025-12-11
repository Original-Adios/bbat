#pragma once
#include "BTMeasBase.h"

class CBTGeneralMeasBDR : public CBTMeasBase
{
public:
    virtual ~CBTGeneralMeasBDR(void);
	CBTGeneralMeasBDR(CImpBase *pImpBase, ICBTApi *pBtApi);//

protected:
	//////////////////////////////////////////////////////////////////////////
	// Measurement functions
	virtual BOOL MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult);
	virtual BOOL TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult);
};
