#pragma once
#include "IBTApi.h"
#include "../ImpBase.h"

class CBTRFPathSwitch
{
public:
    virtual ~CBTRFPathSwitch(void);
    CBTRFPathSwitch(ICBTApi *pBTApi);//

public:
    SPRESULT BT_RFPathSet(BT_RFPATH_ENUM eRfPath);

private:
    ICBTApi     *m_pBTApi;
};
