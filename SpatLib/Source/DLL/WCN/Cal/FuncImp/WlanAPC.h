#pragma once
#include "impbase.h"
#include "IWlanApi.h"


class CWlanAPC :
	public CImpBase
{
	DECLARE_RUNTIME_CLASS(CWlanAPC)
public:
	CWlanAPC(void);
	~CWlanAPC(void);
protected:
	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __FinalAction(void);

	SPRESULT DoAPc();
	SPRESULT ReadApcChan();
	ICWlanApi* m_pWlanApi;
	std::vector<SPWI_WLAN_PARAM_TESTER> m_TesterParam[2][2];
	SPWI_RESULT_T m_TesterRlt;
	double m_dPowerLimit[2][2][2];
	BOOL m_bEnableCal[2][2];
	int m_nTSSI;
	CAL_TPC_RLT_T m_TxCalRlt[20];
	int m_nEfuseLimit;
	BOOL m_bAutoChan;
};

