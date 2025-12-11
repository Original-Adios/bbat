#pragma once
#include "..//ImpBase.h"

///////////////////////////////////////
class CPowerOff :public CImpBase
{
	DECLARE_RUNTIME_CLASS(CPowerOff)

public:
	CPowerOff(void);
	virtual ~CPowerOff(void);
protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);
public:

	BOOL m_bPowerOff = TRUE;
	ChargeVoltage m_eVoltage = Charge_4V;
};