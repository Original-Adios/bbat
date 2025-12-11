#pragma once
#include "../ImpBase.h"
#include "WlanLoadLossXML.h"
#include "LossHelper.h"
#include "WlanTestSys.h"

using namespace std;

class CWlanTestSysAntLoss : public CWlanTestSys
{
	 DECLARE_RUNTIME_CLASS(CWlanTestSysAntLoss)
protected:
	CWlanTestSysAntLoss(void);//
    virtual ~CWlanTestSysAntLoss(void);
	virtual SPRESULT __PollAction (void);
	virtual BOOL       LoadXMLConfig(void);
	SPRESULT __FinalAction(void);
public:
	WLAN_LOSS_PARAM m_wlanLossParam;
private:
	CWlanLoadLossXML* m_pLoadLossXml;

};
