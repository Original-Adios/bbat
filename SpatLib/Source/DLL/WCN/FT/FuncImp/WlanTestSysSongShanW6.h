#pragma once
#include "../ImpBase.h"
using namespace std;
#include "WlanAntSwitch.h"

class CWlanTestSysSongShanW6 : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CWlanTestSysSongShanW6)
protected:
	CWlanTestSysSongShanW6(void);//
	virtual ~CWlanTestSysSongShanW6(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __FinalAction(void);

protected:
	WLAN_PARAM_CONF m_stWlanParamImp;
	CWlanMeasBase* m_pWlanMeas;
	CWlanEnterMode* m_pEnterMode;
	CWlanAntSwitch* m_pWlanAnt;

};
