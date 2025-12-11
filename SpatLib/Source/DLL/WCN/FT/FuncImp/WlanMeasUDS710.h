#pragma once
#include "WlanMeasBase.h"

class CWlanMeasUDS710 : public CWlanMeasBase
{
public:
    virtual ~CWlanMeasUDS710(void);
	CWlanMeasUDS710(CImpBase *pImpBase);//

protected:
	//////////////////////////////////////////////////////////////////////////
	// Measurement functions
	virtual SPRESULT InitPerAndMeasureUplink(WIFI_SIGNAL WifiSignal, SPWI_WLAN_PARAM_MEAS_GROUP* pUplinkMeasParam);

	virtual SPRESULT MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult);
	virtual SPRESULT TestPER( SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult);
};
