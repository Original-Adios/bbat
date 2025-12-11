#pragma once
#include "../ImpBase.h"
#include "BTGeneralLoadLossXML.h"
#include "BTEnterMode.h"

using namespace std;

class CBTMeasSysV1Loss : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CBTMeasSysV1Loss)
protected:
	CBTMeasSysV1Loss(void);
    virtual ~CBTMeasSysV1Loss(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
public:
	vector<BTMeasParamBand>   m_VecBTParamBandImp;
	BT_LOSS_PARAM m_BtLossParm;

private:
	vector<CBTMeasBase*> m_pvecBTMeas;
	CBTGeneralLoadLossXML *m_pLoadLossXml;
    CBTEnterMode        *m_pEnterMode;
    ICBTApi             *m_pBtApi;
	
};
