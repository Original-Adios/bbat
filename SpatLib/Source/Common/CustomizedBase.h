#pragma once
#include "global_def.h"
//#include "global_err.h"
#include "cellular_def.h"
#include "SpatBase.h"

class CCustomized
{
public:
    CCustomized(void);
    virtual ~CCustomized(void);

public:
    void InitRfSwitch(INT nDutId, SP_MODE_INFO Mode, CSpatBase* pSpatbase, LPCWSTR lpProjectName);
    SPRESULT SetRfSwitch(int nBand, int TxAnt, int RxAnt);
    SPRESULT ResetRfSwitch();

private:
    CSpatBase* m_pSpatbase;
    LPCWSTR m_lpProjectName;
    SP_MODE_INFO m_Mode;
    INT m_nDutId;
};