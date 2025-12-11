#pragma once
#include "../ImpBase.h"
#include "BTGeneralLoadXML.h"
#include "BTEnterMode.h"

using namespace std;

class CBTMeasSysV1 : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CBTMeasSysV1)
protected:
	CBTMeasSysV1(void);
    virtual ~CBTMeasSysV1(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
public:
	vector<BTMeasParamBand>   m_VecBTParamBandImp;

private:
	vector<CBTMeasBase*> m_pvecBTMeas;
    CBTGeneralLoadXML   *m_pLoadXml;
    CBTEnterMode        *m_pEnterMode;
    ICBTApi             *m_pBtApi;
};
