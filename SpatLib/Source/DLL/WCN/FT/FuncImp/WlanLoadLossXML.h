#pragma once
#include "SpatBase.h"
#include "WlanMeasBase.h"
#include <map>
#include <string>

using namespace std;

class CWlanLoadLossXML
{
public:
    CWlanLoadLossXML(CSpatBase *pSpatBase);
    ~CWlanLoadLossXML(void);

public:
    SPRESULT WlanLoadLossXmlFile(vector<WlanMeasParamBand> &VecWlanParamBandImp, WLAN_LOSS_PARAM &wlanLossParam);
private:
    CSpatBase *m_pSpatBase;
	//WLAN_LOSS_PARAM wlanLossParam;
};
