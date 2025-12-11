#pragma once
#include "../ImpBase.h"

#define MaxFMRssiNumber -20
//////////////////////////////////////////////////////////////////////////
class CFm : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CFm)
public:
	CFm(void);
	virtual ~CFm(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:    
	enum FM_TYPE
	{
		LANT,
		SANT,
		FP,
		MAX_FM_TYPE
	};
	int m_nType = LANT;
	int m_nClose = 0;
	int m_nRssi = 0;
	SPRESULT RunLantOpen();
	SPRESULT RunSantOpen();
	SPRESULT RunClose();
	SPRESULT RunOpenAction();
	SPRESULT RunFpOpen();
	
	uint16 m_sFreq = 0;

};