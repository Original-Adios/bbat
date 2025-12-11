#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CKeypad : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CKeypad)
public:
	CKeypad(void);
	virtual ~CKeypad(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);
	

private:
	SPRESULT KeypadAction();
	SPRESULT KeypadOpen();
	SPRESULT KeypadClose();
	SPRESULT RunPowerKey();
	SPRESULT RunPowerKey2();
	SPRESULT RunSideKey();
	SPRESULT KeypadRead(int nSpec, BOOL bFp);

	BOOL m_bPowerKey = FALSE;
	BOOL m_bPowerKey2 = FALSE;
	BOOL m_bFpPowerKey = FALSE;
	BOOL m_bSideKey = FALSE;
	int m_nPowerKeySpec = 0;
	int m_nPowerKey2Spec = 0;
	int m_nSideKeySpec = 0;
};