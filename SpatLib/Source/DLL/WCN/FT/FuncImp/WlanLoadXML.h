#pragma once
#include "SpatBase.h"
#include "WlanMeasBase.h"
#include <map>
#include <string>

using namespace std;

class CWlanLoadXML
{
public:
    CWlanLoadXML(CSpatBase *pSpatBase);
    ~CWlanLoadXML(void);

public:
    SPRESULT WlanLoadXmlFile(vector<WlanMeasParamBand> &VecWlanParamBandImp);
private:
    CSpatBase *m_pSpatBase;
};
