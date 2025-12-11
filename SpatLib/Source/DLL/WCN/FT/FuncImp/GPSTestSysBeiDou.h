#pragma once
#include "GPSTestSys.h"

class CGpsTestSysBeiDou : public CGpsTestSys
{
	 DECLARE_RUNTIME_CLASS(CGpsTestSysBeiDou)
public:
	CGpsTestSysBeiDou(void);
	virtual ~CGpsTestSysBeiDou(void);

	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);
	virtual SPRESULT GPS_MeasureEUT(GPSMeasParamChan* pGPSMeasParam);
	virtual SPRESULT GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam);

};
