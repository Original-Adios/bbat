#pragma once
#include "../ImpBase.h"
#include "WlanProbeLoadXML.h"

using namespace std;

class CWlanBerProbe : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CWlanBerProbe)
protected:
	CWlanBerProbe(void);//
    virtual ~CWlanBerProbe(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
private:
	WLAN_PARAM_CONF m_stWlanParamImp;
	CWlanMeasBaseProbe* m_pWlanMeas;
	CWlanEnterMode* m_pEnterMode;
	CWlanAntSwitch* m_pWlanAnt;
};
