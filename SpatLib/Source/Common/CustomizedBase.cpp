#include "StdAfx.h"
#include "CustomizedBase.h"
#include "CustomRF.h"
#pragma comment(lib, "Customized.lib")

CCustomized::CCustomized(void)
{
}


CCustomized::~CCustomized(void)
{
}

void CCustomized::InitRfSwitch(INT nDutId, SP_MODE_INFO Mode, CSpatBase* pSpatbase, LPCWSTR lpProjectName)
{
    m_pSpatbase = pSpatbase;
    m_lpProjectName = lpProjectName;
    m_Mode = Mode;
    m_nDutId = nDutId;
    m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "%s Mode = %d, DutId = %d", __FUNCTION__, m_Mode, m_nDutId);
    
}

SPRESULT CCustomized::SetRfSwitch(int nBand, int TxAnt, int RxAnt)
{
    ResetRfSwitch();
    m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "%s, TxAnt:RxAnt  %d:%d", __FUNCTION__, TxAnt, RxAnt);
    //band > 0, 天线不同时为-1，至少一个在0 ~ 7范围内
    if (nBand < 0)
    {
        m_pSpatbase->LogFmtStrA(SPLOGLV_ERROR, "Wrong Band Number: %d", nBand);
        return SP_E_SPAT_RF_SWITCH_ERROR;
    }
    if (!IN_RANGE(RF_ANT_1st, TxAnt, MAX_RF_ANT - 1) && !IN_RANGE(RF_ANT_1st, RxAnt, MAX_RF_ANT - 1))
    {
        m_pSpatbase->LogFmtStrA(SPLOGLV_ERROR, "Wrong TxAnt:RxAnt  %d:%d", TxAnt, RxAnt);
        return SP_E_SPAT_RF_SWITCH_ERROR;
    }
    if (!Custom_RF_GetEnable(m_lpProjectName, RF_SWITCH))
    {
        return SP_OK;
    }
    if (!Custom_RF_SetRfSwitch(m_nDutId, m_Mode, nBand, TxAnt, RxAnt))
    {
        m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "Set Rf Switch Fail");
        return SP_E_SPAT_RF_SWITCH_ERROR;
    }
    return SP_OK;
}

SPRESULT CCustomized::ResetRfSwitch()
{
    m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
    if (!Custom_RF_GetEnable(m_lpProjectName, RF_SWITCH))
    {
        return SP_OK;
    }
    if (!Custom_RF_ResetRfSwitch(m_nDutId))
    {
        m_pSpatbase->LogFmtStrA(SPLOGLV_INFO, "Reset Rf Switch Fail");
        return SP_E_SPAT_RF_SWITCH_ERROR;
    }
    return SP_OK;
}
