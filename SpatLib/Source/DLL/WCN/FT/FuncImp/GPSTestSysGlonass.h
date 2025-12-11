#pragma once
#include "GPSTestSys.h"

class CGpsTestSysGlonass : public CGpsTestSys
{
	 DECLARE_RUNTIME_CLASS(CGpsTestSysGlonass)
public:
	CGpsTestSysGlonass(void);
	virtual ~CGpsTestSysGlonass(void);

	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);
	virtual SPRESULT GPS_MeasureEUT(GPSMeasParamChan* pGPSMeasParam);
	virtual SPRESULT GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam);

};
