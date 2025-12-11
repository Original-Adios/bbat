#pragma once
#include "IWlanApi.h"
#include "../ImpBase.h"

class CWlanEnterMode
{
public:
    virtual ~CWlanEnterMode(void);
    CWlanEnterMode(CImpBase *pImpBase);//
	CWlanEnterMode() = default;

public:
    SPRESULT Wlan_SetBand(WLAN_BAND_ENUM eBand);
    SPRESULT Wlan_ModeEnable(BOOL bEnterOrLeave);
	SPRESULT BDWlan_ModeEnable(BOOL bEnterOrLeave);

private:
    ICWlanApi     *m_pWlanApi;
    char          m_szRecvBuf[2048];
    char          m_szCmd[2048];

};
