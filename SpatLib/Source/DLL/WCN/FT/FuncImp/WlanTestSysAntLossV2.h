#pragma once
#include "../ImpBase.h"
using namespace std;
#include "WlanAntSwitch.h"

class CWlanTestSysAntLossV2 : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CWlanTestSysAntLossV2)
protected:
	CWlanTestSysAntLossV2(void);//
    virtual ~CWlanTestSysAntLossV2(void);

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
