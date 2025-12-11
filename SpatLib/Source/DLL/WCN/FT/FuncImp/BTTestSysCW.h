#pragma once
#include "../ImpBase.h"
#include "BTEnterMode.h"
#include "BTApiAT.h"
#include "BTRFPathSwitch.h"
#include "BTMeasCW.h"

using namespace std;

class CBTTestSysCW : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CBTTestSysCW)
protected:
    CBTTestSysCW(void);
    virtual ~CBTTestSysCW(void);

    virtual SPRESULT __InitAction(void);
    virtual SPRESULT __PollAction(void);
    virtual void     __LeaveAction(void);
    virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
public:
    BTMeasParam   m_VecBTParamBandImp;

private:
    CBTEnterMode* m_pEnterMode;
    CBTRFPathSwitch* m_pRfPathSwitch;
    ICBTApi* m_pBtApi;

    CBTMeasCW* m_pCWMeas;

};
