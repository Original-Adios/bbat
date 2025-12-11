#pragma once
#include "SpatBase.h"
#include "BTMeasBase.h"
#include <map>
#include <string>

using namespace std;

class CBTLoadXML
{
public:
    CBTLoadXML(CSpatBase *pSpatBase);
    ~CBTLoadXML(void);

public:
    SPRESULT BTLoadXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp);
private:
    CSpatBase *m_pSpatBase;
};
