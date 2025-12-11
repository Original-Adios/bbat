#pragma once
#include "SpatBase.h"
#include "BTMeasBase.h"
#include <map>
#include <string>

using namespace std;

class CBTLoadXMLCW
{
public:
    CBTLoadXMLCW(CSpatBase* pSpatBase);
    ~CBTLoadXMLCW(void);

public:
    SPRESULT BTLoadXmlFile(vector<BTMeasParamBand>& VecBTParamBandImp);
private:
    CSpatBase* m_pSpatBase;
};
