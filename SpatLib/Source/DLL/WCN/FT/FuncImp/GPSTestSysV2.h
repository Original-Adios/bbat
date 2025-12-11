#pragma once
#include "GPSTestSys.h"
#include <vector>

class CGpsTestSysV2 : public CGpsTestSys
{
	 DECLARE_RUNTIME_CLASS(CGpsTestSysV2)

public:
	CGpsTestSysV2(void);
	virtual ~CGpsTestSysV2(void);
	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);
	virtual SPRESULT GPS_MeasureEUT(GPSMeasParamChan* pGPSMeasParam);
	virtual SPRESULT GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam);
};
