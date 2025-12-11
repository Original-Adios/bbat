#pragma once
#include "WlanMeasBase.h"

class CWlanMeasBaseMimo : public CWlanMeasBase
{
public:
	virtual ~CWlanMeasBaseMimo(void);
	CWlanMeasBaseMimo(CImpBase *pImpBase);//

public:
	virtual SPRESULT WlanMeasPerform();

private:
	SPRESULT ShowMeasRstMimo(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T *pTestResult, int nChan, BOOL bTxShow = TRUE, int nSbwType = 0, int nCbwType = 0);

private:
	SPWI_RESULT_T       m_TxRstMimo[2];

};
