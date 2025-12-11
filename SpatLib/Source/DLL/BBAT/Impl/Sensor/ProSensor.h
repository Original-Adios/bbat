#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CProSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CProSensor)
public:
	CProSensor(void);
	virtual ~CProSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:

	SPAT_VALUE m_ProValue;

	SPRESULT RunProTest();
    
};