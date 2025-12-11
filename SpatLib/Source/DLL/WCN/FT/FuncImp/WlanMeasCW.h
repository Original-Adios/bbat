#pragma once
#include "WlanMeasBase.h"

class CWlanMeasCW : public CWlanMeasBase
{
public:
    virtual ~CWlanMeasCW(void);
	CWlanMeasCW(CImpBase *pImpBase);//

protected:
	//////////////////////////////////////////////////////////////////////////
	// Measurement functions
	virtual SPRESULT MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult);
	virtual SPRESULT TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult);
};
