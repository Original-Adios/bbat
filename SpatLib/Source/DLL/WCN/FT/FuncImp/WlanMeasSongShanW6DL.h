#pragma once
#include "WlanMeasSongShanW6.h"

class CWlanMeaSongShanW6DL : public CWlanMeaSongShanW6
{
public:
    virtual ~CWlanMeaSongShanW6DL(void);
	CWlanMeaSongShanW6DL(CImpBase *pImpBase);//

protected:
	//////////////////////////////////////////////////////////////////////////
	virtual SPRESULT TestPER( SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T* pRxReult);
};
