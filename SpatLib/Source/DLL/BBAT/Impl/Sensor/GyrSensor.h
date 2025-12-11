#pragma once
#include "../ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CGyrSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CGyrSensor)
public:
	CGyrSensor(void);
	virtual ~CGyrSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);

private:

};