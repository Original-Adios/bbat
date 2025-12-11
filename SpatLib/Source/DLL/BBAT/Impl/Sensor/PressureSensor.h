#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CPreSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CPreSensor)
public:
	CPreSensor(void);
	virtual ~CPreSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:

	double m_dAccuracy = 1e3;  //¾«¶È
	int m_nSpecUp = 0;
	int m_nSpecDown = 0;

	SPRESULT RunPressureTest();

};