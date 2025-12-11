#pragma once
#include "WlanMeasBase.h"

using namespace std;

class CWlanMeas5G : public CWlanMeasBase
{
public:
	virtual ~CWlanMeas5G(void);
	CWlanMeas5G(CImpBase *pImpBase);//

protected:
	// Measurement functions
	virtual SPRESULT MeasureUplink(SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult);
	virtual SPRESULT TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult);
};
