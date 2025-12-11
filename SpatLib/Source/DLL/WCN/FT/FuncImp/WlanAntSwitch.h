#pragma once
#include <map>
#include "IWlanApi.h"
#include "../ImpBase.h"

using namespace std;

class CWlanAntSwitch
{
public:
    virtual ~CWlanAntSwitch(void);
    CWlanAntSwitch(CImpBase *pImpBase);//
	CWlanAntSwitch() = default;
public:
    SPRESULT Wlan_AntSwitch(ANTENNA_ENUM eAnt);
	SPRESULT BDWlan_AntSwitch(ANTENNA_ENUM eAnt);

private:
    ICWlanApi	*m_pWlanApi;
	CImpBase	*m_pImpBase;
};
