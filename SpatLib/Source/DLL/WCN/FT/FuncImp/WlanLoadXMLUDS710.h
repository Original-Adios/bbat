#pragma once
#include "SpatBase.h"
#include "WlanMeasBase.h"
#include <map>
#include <vector>
#include <string>

using namespace std;

class CWlanLoadXMLUDS710
{
public:
	CWlanLoadXMLUDS710(CSpatBase *pSpatBase);
    ~CWlanLoadXMLUDS710(void);

public:
	SPRESULT WlanLoadXmlFile(WLAN_PARAM_CONF &stWlanParamImp);
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
