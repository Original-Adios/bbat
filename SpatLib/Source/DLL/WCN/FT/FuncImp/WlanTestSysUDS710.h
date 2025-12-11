#pragma once
#include "../ImpBase.h"
using namespace std;
#include "WlanAntSwitch.h"

class CWlanTestSysUDS710 : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CWlanTestSysUDS710)
protected:
	CWlanTestSysUDS710(void);//
    virtual ~CWlanTestSysUDS710(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
private:
	WLAN_PARAM_CONF m_stWlanParamImp;
	CWlanMeasBase* m_pWlanMeas;
    CWlanEnterMode* m_pEnterMode;
	CWlanAntSwitch* m_pWlanAnt;
	
};
