#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CHallSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CHallSensor)
public:
	CHallSensor(void);
	virtual ~CHallSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:
	BYTE m_byGpioNo = 0;

	SPRESULT AFCB_PowerOn(bool bStatus);
	SPRESULT Phone_GpioRead();

};