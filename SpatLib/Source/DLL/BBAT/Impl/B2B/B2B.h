#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CB2B : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CB2B)
public:
	CB2B(void);
	virtual ~CB2B(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);
		
private:
	int m_LCD_ID;
	int m_TP_ID;
	int m_RF_ID;
	int m_TP_VALUE;
	int m_FINGER_ID;

	SPRESULT Run_LCD_ID();
	SPRESULT Run_TP_ID();
	SPRESULT Run_TP_VALUE();
	SPRESULT Run_RF_ID();
	SPRESULT Run_FINGER_ID();
	
};
