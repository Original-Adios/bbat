#pragma once
#include <vector>
#include <map>
#include <list>
#include "IWlanApi.h"
#include "../ImpBase.h"
#include "WlanMeasBase.h"

using namespace std;

#define CHKRESULT_WITH_NOTIFY_WCN_ITEM(statement, Itemname)  \
{										                    \
	SPRESULT __sprslt = (statement);                        \
	if (SP_OK != __sprslt)                                  \
{                                                       \
	m_pImpBase->_UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1);                \
	return __sprslt;                                    \
}                                                       \
}	

class CWlanMeasBaseProbe : public CWlanMeasBase
{
public:
	virtual ~CWlanMeasBaseProbe(void);
	CWlanMeasBaseProbe(CImpBase *pImpBase);//

public:
	virtual SPRESULT WlanParamSet(WlanMeasParamBand *pWlanParamBand);
	virtual SPRESULT WlanMeasPerform();
protected:
    // Measurement functions
    virtual SPRESULT TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult) = 0;
private:
    SPRESULT ShowMeasRst(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T *pTestResult, BOOL bTxShow = TRUE, double dBsLvl = -60.0);

public:
	HANDLE m_hcsvHandle;
};
