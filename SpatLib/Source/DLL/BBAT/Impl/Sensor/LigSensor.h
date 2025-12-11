#pragma once
#include "../ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CLigSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CLigSensor)
public:
	CLigSensor(void);
	virtual ~CLigSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);

private:

	SPRESULT RunLig();

	SPAT_VALUE m_LigValue;
	
};
