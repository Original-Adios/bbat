#pragma once
#include "WlanMeasBase.h"

class CWlanMeasOS80 : public CWlanMeasBase
{
public:
    virtual ~CWlanMeasOS80(void);
	CWlanMeasOS80(CImpBase *pImpBase);//

protected:
	//////////////////////////////////////////////////////////////////////////
	// Measurement functions
	virtual SPRESULT MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult);
	virtual SPRESULT TestPER( SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult);
};
