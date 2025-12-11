#pragma once
#include "../ImpBase.h"
#include "WlanLoadXMLOS80.h"

using namespace std;

class CWlanTestSysMimo : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CWlanTestSysMimo)
protected:
	CWlanTestSysMimo(void);//
    virtual ~CWlanTestSysMimo(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
private:
	WLAN_PARAM_CONF m_stWlanParamImp;
	CWlanMeasBaseMimo* m_pWlanMeas;
	CWlanEnterMode* m_pEnterMode;
	CWlanAntSwitch* m_pWlanAnt;
};
