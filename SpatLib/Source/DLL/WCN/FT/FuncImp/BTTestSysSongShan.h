#pragma once
#include "../ImpBase.h"
#include "BTEnterMode.h"
#include "BTApiAT.h"
#include "BTRFPathSwitch.h"
#include "BTMeasBDR.h"
#include "BTMeasEDR.h"
#include "BTMeasBLE_BLE53.h"

using namespace std;

class CBTTestSysSongShan : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CBTTestSysSongShan)
protected:
	CBTTestSysSongShan(void);
    virtual ~CBTTestSysSongShan(void);

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

	CBTMeasBDR* m_pBdrMeas;
	CBTMeasEDR* m_pEdrMeas;
	CBTMeasBLE_BLE53* m_pBle53Meas;
};
