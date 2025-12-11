#pragma once
#include "SpatBase.h"
#include "WlanMeasBase.h"
#include <map>
#include <vector>
#include <string>

using namespace std;

class CWlanLoadXMLCW
{
public:
    CWlanLoadXMLCW(CSpatBase *pSpatBase);
    ~CWlanLoadXMLCW(void);

public:
	SPRESULT WlanLoadXmlFile(WLAN_PARAM_CONF &stWlanParamImp);
private:
	SPRESULT LoadWlanCWConfig( vector<WlanMeasParamBand> &VecWlanParamBandImp);
	SPRESULT ChanRangeCheck(int nBand, int nCh, int nChBandType);

private:
    CSpatBase *m_pSpatBase;
	wstring m_strBandSel;
	wstring m_strParamChan;

	WlanMeasParamBand m_stWlanParamBand;
	SPWI_WLAN_PARAM_GROUP m_stGroupParam;
};
