#pragma once
#include "SpatBase.h"
#include "BTMeasBase.h"
#include <map>
#include <string>

using namespace std;

class CBTLoadXML_BLE53
{
public:
    CBTLoadXML_BLE53(CSpatBase *pSpatBase);
    ~CBTLoadXML_BLE53(void);

public:
    SPRESULT BTLoadXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp);
private:
    CSpatBase *m_pSpatBase;
};
