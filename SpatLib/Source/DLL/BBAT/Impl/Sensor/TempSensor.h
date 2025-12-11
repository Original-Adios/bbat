#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CTempSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CTempSensor)
public:
	CTempSensor(void);
	virtual ~CTempSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:

	int nUpSpec = 0;
	int nDownSpec = 0;

	SPRESULT RunTemp();

};