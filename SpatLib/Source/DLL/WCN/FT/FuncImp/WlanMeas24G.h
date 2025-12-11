#pragma once
#include "WlanMeasBase.h"

class CWlanMeas24G : public CWlanMeasBase
{
public:
    virtual ~CWlanMeas24G(void);
	CWlanMeas24G(CImpBase *pImpBase);//

protected:
	//////////////////////////////////////////////////////////////////////////
	// Measurement functions
	virtual SPRESULT MeasureUplink(SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult);
	virtual SPRESULT TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult);
};
