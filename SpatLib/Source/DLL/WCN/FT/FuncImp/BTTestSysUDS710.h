#pragma once
#include "../ImpBase.h"
#include "BTEnterMode.h"
#include "BTApiAT.h"
#include "BTRFPathSwitch.h"
#include "BTMeasBDRUDS710.h"
#include "BTMeasEDRUDS710.h"
#include "BTMeasBLE_EXUDS710.h"

using namespace std;

class CBTTestSysUDS710 : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CBTTestSysUDS710)
protected:
	CBTTestSysUDS710(void);
    virtual ~CBTTestSysUDS710(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
public:
	BTMeasParam   m_VecBTParamBandImp;

private:
    CBTEnterMode        *m_pEnterMode;
	CBTRFPathSwitch		*m_pRfPathSwitch;
    ICBTApi             *m_pBtApi;

	CBTMeasBDRUDS710* m_pBdrMeas;
	CBTMeasEDRUDS710* m_pEdrMeas;
	CBTMeasBLEExUDS710* m_pBleExMeas;
};
