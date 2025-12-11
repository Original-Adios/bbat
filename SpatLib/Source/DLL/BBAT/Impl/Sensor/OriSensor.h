#pragma once
#include "../ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class COriSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(COriSensor)
public:
	COriSensor(void);
	virtual ~COriSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);

private:

};