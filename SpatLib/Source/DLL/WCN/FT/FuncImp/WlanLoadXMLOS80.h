#pragma once
#include "SpatBase.h"
#include "WlanMeasBase.h"
#include <map>
#include <vector>
#include <string>

using namespace std;

class CWlanLoadXMLOS80
{
public:
    CWlanLoadXMLOS80(CSpatBase *pSpatBase);
    ~CWlanLoadXMLOS80(void);

public:
	SPRESULT WlanLoadXmlFile(WLAN_PARAM_CONF &stWlanParamImp);
	SPRESULT WlanLoadLossXmlFile(WLAN_LOSS_PARAM& wlanLossParam);
private:;
	SPRESULT ChanRangeCheck(int nBand, int nCh, int nChBandType);

private:
    CSpatBase *m_pSpatBase;
	wstring m_strBandSel;
	wstring m_strParamChan;

	WlanMeasParamBand m_stWlanParamBand;
	SPWI_WLAN_PARAM_GROUP m_stGroupParam;

	vector<wstring> m_vectProtco;
	vector<wstring> m_vectBandWidth;
};
