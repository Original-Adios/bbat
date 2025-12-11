#pragma once
#include "../ImpBase.h"
using namespace std;
#include "WlanAntSwitch.h"

class CWlanTestSysAntLossSC266X : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CWlanTestSysAntLossSC266X)
protected:
	CWlanTestSysAntLossSC266X(void);//
    virtual ~CWlanTestSysAntLossSC266X(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
private:
	WLAN_PARAM_CONF m_stWlanParamImp;
	WLAN_LOSS_PARAM m_wlanLossParam;
	CWlanMeasBase* m_pWlanMeas;
    CWlanEnterMode* m_pEnterMode;
	CWlanAntSwitch* m_pWlanAnt;
	
};
