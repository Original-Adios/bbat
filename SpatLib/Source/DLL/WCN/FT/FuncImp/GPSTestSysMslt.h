#pragma once
#include "GPSTestSys.h"

class CGpsTestSysMslt : public CGpsTestSys
{
	 DECLARE_RUNTIME_CLASS(CGpsTestSysMslt)
protected:
	CGpsTestSysMslt(void);
	virtual ~CGpsTestSysMslt(void);

	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);
	virtual SPRESULT GPS_MeasureEUT(GPSMeasParamChan* pGPSMeasParam);
};
