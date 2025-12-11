#pragma once
#include "impbase.h"
#include "IWlanApi.h"

class CWlanAFCTx :
	public CImpBase
{
	DECLARE_RUNTIME_CLASS(CWlanAFCTx)
public:
	CWlanAFCTx(void);
	~CWlanAFCTx(void);
protected:
	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __FinalAction(void);

	void ConfigInstrumentParam();
	SPRESULT GetFer(int nDac, double& dFer);
	
	ICWlanApi* m_pWlanApi;
	SPWI_WLAN_PARAM_TESTER  m_TesterParam;
	SPWI_RESULT_T m_TesterRlt;
	int m_nCalChan;
	double m_dRefLevel;
	E_WLAN_RATE m_eRate;
	int m_nStartAFCDac1;
	int m_nStartAFCDac2;
	double m_dFerLimit;
	int m_nEfuseLimit;
	ANTENNA_ENUM m_ePath;
};

