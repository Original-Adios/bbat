#pragma once
#include "BTMeasBase.h"

class CBTMeasBDRUDS710 : public CBTMeasBase
{
public:
    virtual ~CBTMeasBDRUDS710(void);
	CBTMeasBDRUDS710(CImpBase *pImpBase, ICBTApi *pBtApi);//

protected:
	//////////////////////////////////////////////////////////////////////////
	// Measurement functions
	virtual BOOL MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult);
	virtual BOOL TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult);
};
