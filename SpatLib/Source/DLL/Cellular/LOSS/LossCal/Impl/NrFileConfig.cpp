#include "StdAfx.h"
#include "NrFileConfig.h"
#include "NrUtility.h"

CNrFileConfig::CNrFileConfig(void)
{
    m_pSpatBase = nullptr;

    memset(m_arrBand, 0, sizeof m_arrBand);

    m_dMaxCellPower = 0.0;
    m_dMinCellPower = 0.0;
    m_dRxTarget = 0.0;
    m_dRxTolerance = 0.0;
    m_nRxIndex = 0;
    m_dTxTarget = 0.0;
    m_dLossValLower = 0.0;
    m_dLossValUpper = 0.0;
    m_dPreLoss = 0.0;
    m_dSpecLower = 0.0;
    m_dSpecUpper = 0.0;
}

CNrFileConfig::~CNrFileConfig(void)
{
}

int CNrFileConfig::GetConfigValue(LPCWSTR lpPath, int defValue)
{
    return m_pSpatBase->GetConfigValue(lpPath, defValue);
}

double CNrFileConfig::GetConfigValue(LPCWSTR lpPath, double defValue)
{
    return m_pSpatBase->GetConfigValue(lpPath, defValue);
}

LPCWSTR CNrFileConfig::GetConfigValue(LPCWSTR lpPath, LPCWSTR defString)
{
    return m_pSpatBase->GetConfigValue(lpPath, defString);
}

SPRESULT CNrFileConfig::Load()
{
    std::wstring BaseKey = L"Option:NR:";
    //Band
    for (uint32 i = 0; i < MAX_NR_BAND; i++)
    {
        m_arrBand[i] = GetConfigValue((BaseKey + CNrUtility::m_BandInfo[i].NameW).c_str(), FALSE);
    }

    m_nRxIndex = GetConfigValue(L"Param:NR:RxIndex", 20);
    m_nVoltage = GetConfigValue(L"Param:NR:APT", 20);
    return SP_OK;
}
