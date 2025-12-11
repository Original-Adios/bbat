#pragma once
#include "../ImpBase.h"
#include "WlanLoadXML.h"

using namespace std;

class CWlanTestSys : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CWlanTestSys)
protected:
	CWlanTestSys(void);//
    virtual ~CWlanTestSys(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
public:
	vector<WlanMeasParamBand>   m_VecWlanParamBandImp;


protected:
	vector<CWlanMeasBase*> m_pvecWlanMeas;
    CWlanLoadXML* m_pLoadXml;
    CWlanEnterMode* m_pEnterMode;

private:

};
