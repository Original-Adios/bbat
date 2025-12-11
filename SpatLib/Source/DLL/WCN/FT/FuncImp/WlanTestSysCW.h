#pragma once
#include "../ImpBase.h"
#include "WlanAntSwitch.h"
using namespace std;

class CWlanTestSysCW : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CWlanTestSysCW)
protected:
	CWlanTestSysCW(void);//
    virtual ~CWlanTestSysCW(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
private:
	WLAN_PARAM_CONF m_stWlanParamImp;
private:
	CWlanMeasBase* m_pWlanMeas;
    CWlanEnterMode* m_pEnterMode;
	CWlanAntSwitch* m_pWlanAnt;
};
