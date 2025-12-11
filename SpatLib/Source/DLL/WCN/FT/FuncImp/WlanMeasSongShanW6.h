#pragma once
#include "WlanMeasBase.h"

class CWlanMeaSongShanW6 : public CWlanMeasBase
{
public:
    virtual ~CWlanMeaSongShanW6(void);
	CWlanMeaSongShanW6(CImpBase *pImpBase);//

protected:
	//////////////////////////////////////////////////////////////////////////
	// Measurement functions
	virtual SPRESULT MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult);
	virtual SPRESULT TestPER( SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult);

    virtual SPRESULT ShowMeasRst(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T* pTestResult, int nChan, double dBSLevel, int nSbwType = 0, int nCbwType = 0, ANTENNA_ENUM eAnt = ANT_PRIMARY);
    virtual SPRESULT JudgeMeasRst(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T* pTestResult, double dBSLevel, ANTENNA_ENUM eAnt = ANT_PRIMARY);
};
