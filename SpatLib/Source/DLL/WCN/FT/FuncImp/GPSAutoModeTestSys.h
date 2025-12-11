#pragma once
#include "../ImpBase.h"
//#include "CustomizedBase.h"
#include "GPSTestSys.h"

#include "GPSTestSysV3.h"
#include "GpsTestSysBeiDou.h"
#include "GpsTestSysGlonass.h"

class CGpsAutoModeTestSys : public CGpsTestSys
{
	 DECLARE_RUNTIME_CLASS(CGpsAutoModeTestSys)

protected:
	CGpsAutoModeTestSys(void);
	virtual ~CGpsAutoModeTestSys(void);

	virtual SPRESULT __InitAction(void);
	virtual SPRESULT __PollAction(void);
	virtual BOOL       LoadXMLConfig(void);
};
