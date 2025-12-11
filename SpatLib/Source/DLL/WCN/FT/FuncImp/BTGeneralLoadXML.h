#pragma once
#include "SpatBase.h"
#include "BTMeasBase.h"
#include <map>
#include <string>

using namespace std;

class CBTGeneralLoadXML
{
public:
    CBTGeneralLoadXML(CSpatBase *pSpatBase);
    ~CBTGeneralLoadXML(void);

public:
    SPRESULT BTLoadXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp);
private:
    CSpatBase *m_pSpatBase;

    map<string, int>       m_mapPacketTypeBDR;
    map<string, int>       m_mapPacketTypeEDR;
    map<string, int>       m_mapPacketTypeBLE;
};
