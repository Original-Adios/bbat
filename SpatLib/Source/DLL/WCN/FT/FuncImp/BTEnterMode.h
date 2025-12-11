#pragma once
#include "IBTApi.h"
#include "../ImpBase.h"

class CBTEnterMode
{
public:
    virtual ~CBTEnterMode(void);
    CBTEnterMode(ICBTApi *pBTApi, BT_TYPE eType);//

public:
    SPRESULT BT_ModeEnable(BT_TESTMODE eMode,BOOL bEnterOrLeave);
    SPRESULT BT_SetBtType(BT_TYPE eType);

private:
    ICBTApi     *m_pBTApi;
    char          m_szRecvBuf[2048];
    char          m_szCmd[2048];
    BT_TYPE       m_eType;
};
