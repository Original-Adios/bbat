#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CAccSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CAccSensor)
public:
	CAccSensor(void);
	virtual ~CAccSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:

	double m_dAccuracy = 1e3;  //¾«¶È
	SPAT_VALUE m_AccValueX;
	SPAT_VALUE m_AccValueY;
	SPAT_VALUE m_AccValueZ;

	SPRESULT RunAccTest();
	SPRESULT CheckAcc(AccReturn DataRecv);

};