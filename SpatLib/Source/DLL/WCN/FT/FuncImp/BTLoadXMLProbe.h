#pragma once
#include "SpatBase.h"
#include "BTMeasBase.h"
#include <map>
#include <string>

using namespace std;

class CBTLoadXMLProbe
{
public:
    CBTLoadXMLProbe(CSpatBase *pSpatBase);
    ~CBTLoadXMLProbe(void);

public:
    SPRESULT BTLoadXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp);
private:
    CSpatBase *m_pSpatBase;
};
