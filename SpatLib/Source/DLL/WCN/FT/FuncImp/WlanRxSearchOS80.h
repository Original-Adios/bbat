#pragma once
#include "WlanMeasBaseProbe.h"

class CWlanRxSearchOS80 : public CWlanMeasBaseProbe
{
public:
    virtual ~CWlanRxSearchOS80(void);
	CWlanRxSearchOS80(CImpBase *pImpBase);//

protected:
	//////////////////////////////////////////////////////////////////////////
	// Measurement functions
	virtual SPRESULT TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult);
	virtual SPRESULT MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *, SPWI_RESULT_T *) {return SP_OK;};
};
