#pragma once
#include "GPSTestSysV2.h"

class CGpsTestSysV3 : public CGpsTestSysV2
{
	 DECLARE_RUNTIME_CLASS(CGpsTestSysV3)
public:
	CGpsTestSysV3(void);
	virtual ~CGpsTestSysV3(void);

	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);
	virtual SPRESULT GPS_MeasureEUT(GPSMeasParamChan* pGPSMeasParam);
	virtual SPRESULT GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam);
};
