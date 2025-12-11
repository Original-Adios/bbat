#pragma once
#include "SpatBase.h"
#include "BTMeasBase.h"
#include <map>
#include <string>

using namespace std;

class CBTGeneralLoadLossXML
{
public:
	CBTGeneralLoadLossXML(CSpatBase *pSpatBase);
    ~CBTGeneralLoadLossXML(void);

public:
    SPRESULT BTLoadLossXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp, BT_LOSS_PARAM &BtLossParm);
private:
    CSpatBase *m_pSpatBase;

    map<string, int>       m_mapPacketTypeBDR;
    map<string, int>       m_mapPacketTypeEDR;
    map<string, int>       m_mapPacketTypeBLE;
};
