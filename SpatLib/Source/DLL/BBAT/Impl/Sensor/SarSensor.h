#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CSarSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CSarSensor)
public:
	CSarSensor(void);
	virtual ~CSarSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);

private:
	SPRESULT RunSarTest();
	SPRESULT DelayTask();

	SPAT_VALUE m_SarValue;
	int m_UpDelayTime;
	int m_DownDelayTime;
};