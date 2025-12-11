#pragma once
#include "SpatBase.h"
#include "BTMeasBase.h"
#include <map>
#include <string>

using namespace std;

class CBTLoadXMLUDS710
{
public:
	CBTLoadXMLUDS710(CSpatBase *pSpatBase);
    ~CBTLoadXMLUDS710(void);

public:
    SPRESULT BTLoadXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp);
private:
    CSpatBase *m_pSpatBase;
};
