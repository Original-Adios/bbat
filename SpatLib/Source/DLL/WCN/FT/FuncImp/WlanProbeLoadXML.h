#pragma once
#include "SpatBase.h"
#include "WlanMeasBase.h"
#include <map>
#include <vector>
#include <string>

using namespace std;

class CWlanProbeLoadXML
{
public:
    CWlanProbeLoadXML(CSpatBase *pSpatBase);
    ~CWlanProbeLoadXML(void);

public:
	SPRESULT WlanLoadXmlFile(WLAN_PARAM_CONF &stWlanParamImp);
private:
	SPRESULT LoadWlanBand24GConfig( LPCWSTR lpBandParamPath, vector<WlanMeasParamBand> &VecWlanParamBandImp, int nBand);
	SPRESULT LoadWlanBand50GConfig( LPCWSTR lpBandParamPath, vector<WlanMeasParamBand> &VecWlanParamBandImp, int nBand);
	SPRESULT ChanRangeCheck(int nBand, int nCh, int nChBandType);
	SPRESULT InitChMap();
private:
    CSpatBase *m_pSpatBase;
	wstring m_strBandSel;
	wstring m_strParamChan;

	WlanMeasParamBand m_stWlanParamBand;
	SPWI_WLAN_PARAM_GROUP m_stGroupParam;

	vector<wstring> m_vectProtco;
	vector<wstring> m_vectBandWidth;
};
