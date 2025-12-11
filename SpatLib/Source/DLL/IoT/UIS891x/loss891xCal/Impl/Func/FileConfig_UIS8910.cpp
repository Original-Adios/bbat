#include "StdAfx.h"
#include "FileConfig_UIS8910.h"
#include "LteUtility.h"
#include "../ImpBaseUIS8910.h"

CFileConfig_UIS8910::CFileConfig_UIS8910( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
    m_bAutoBand = FALSE;
    m_MainAnt = FALSE;
    m_DivAnt = FALSE;
    m_bCompatibility = FALSE;
    m_PriorTxFreqMeasLoss = FALSE;
    m_OnlyTxFreqMeasLoss = FALSE;
    m_OnlyRxFreqMeasLoss = FALSE;
    m_dMaxCellPower = 0;
    m_dMinCellPower = 0;
    m_dRxTarget = 0;
    m_dRxTolerance = 0;
    m_dLossValLower = 0;
    m_dLossValUpper = 0;
    m_byRxIndex = 0;
    m_nTestNum = 0;
    m_dTxTarget = 0;
    m_pstrNegativeLossDetected = NULL;
    m_Uetype = 0;
}

CFileConfig_UIS8910::~CFileConfig_UIS8910(void)
{
}

SPRESULT CFileConfig_UIS8910::PreInit()
{
    CHKRESULT(__super::PreInit());

    return SP_OK;
}

SPRESULT CFileConfig_UIS8910::Load()
{
    //DUTType
    m_Uetype = GetConfigValue(L"Option:DUTType:UEType", 0x8910);
    //Band
    std::wstring BaseKey = L"Option:LTE:LTE:";
    wstring sValue;
    sValue = GetConfigValue(L"Option:LTE:LTE:Mode", L"Manually");
    if (0 == sValue.compare(L"Automatically"))
    {
        m_bAutoBand = TRUE;
    }
    else
    {
        m_bAutoBand = FALSE;
    }

    for (uint32 i = 0; i < MAX_LTE_BAND; i++)
    {
        m_arrBandEnable[i] = GetConfigValue((BaseKey + CLteUtility::m_BandInfo[i].NameW).c_str(), FALSE);
    }

    BaseKey = L"Option:LTE:";
    m_MainAnt = GetConfigValue((BaseKey + L"ANT:Primary").c_str(), TRUE);
    m_DivAnt  = GetConfigValue((BaseKey + L"ANT:Diversity").c_str(), TRUE);
    m_bCompatibility = GetConfigValue((BaseKey + L"Compatibility").c_str(), TRUE);

    BaseKey = L"Param:LTE:";
    m_PriorTxFreqMeasLoss = GetConfigValue((BaseKey + L"PriorTxFreqMeasLoss").c_str(), FALSE);
    m_OnlyTxFreqMeasLoss = GetConfigValue((BaseKey + L"OnlyTxFreqMeasLoss").c_str(), FALSE);
    m_OnlyRxFreqMeasLoss = GetConfigValue((BaseKey + L"OnlyRxFreqMeasLoss").c_str(), FALSE);
    m_dMaxCellPower    = GetConfigValue((BaseKey + L"MaxCellPower").c_str(), -20.0);
    m_dMinCellPower    = GetConfigValue((BaseKey + L"MinCellPower").c_str(), -75.0);
    m_dRxTarget        = GetConfigValue((BaseKey + L"RxTarget")    .c_str(), -12.0);
    m_dRxTolerance     = GetConfigValue((BaseKey + L"RxTolerance") .c_str(),   0.5);
    m_byRxIndex        = (uint8)GetConfigValue((BaseKey + L"RxIndex").c_str(), 60);
    m_nTestNum         = GetConfigValue((BaseKey + L"TestNum")    .c_str(),    1);  
    m_dTxTarget        = GetConfigValue((BaseKey + L"TxTarget")    .c_str(),  -5.0);
    return SP_OK;
}

int CFileConfig_UIS8910::GetConfigValue( LPCWSTR lpPath, int defValue )
{
    return m_pSpatBase->GetConfigValue(lpPath, defValue);
}

double CFileConfig_UIS8910::GetConfigValue( LPCWSTR lpPath, double defValue )
{
    return m_pSpatBase->GetConfigValue(lpPath, defValue);
}

LPCWSTR CFileConfig_UIS8910::GetConfigValue( LPCWSTR lpPath, LPCWSTR defString )
{
    return m_pSpatBase->GetConfigValue(lpPath, defString);
}

void CFileConfig_UIS8910::GetLossLimit(const double dFreq, double& lowerLimit, double& upperLimit)
{
    ((CImpBaseUIS8910*)m_pSpatBase)->GetLossLimit(dFreq, lowerLimit, upperLimit);
}