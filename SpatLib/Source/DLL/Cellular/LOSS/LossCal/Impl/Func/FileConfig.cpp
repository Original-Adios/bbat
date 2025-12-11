#include "StdAfx.h"
#include "FileConfig.h"
#include "LteUtility.h"

CFileConfig::CFileConfig( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
	m_MainAnt = LTE_ANT_MAIN;
	m_DivAnt  = LTE_ANT_DIV;
	m_bCompatibility = TRUE;
	m_dMaxCellPower = 0.0;
	m_dMinCellPower = 0.0;
	m_dRxTarget = 0.0;
	m_dRxTolerance = 0.5;
	m_dLossValLower = -0.5;
	m_dLossValUpper = 0.5;
	m_dPreLoss = 3.0;
	m_byRxIndex = 0;
	m_pstrNegativeLossDetected = NULL;

	for (uint32 i = 0; i < MAX_LTE_BAND; i++)
	{
		m_arrBandEnable[i] = FALSE;
	}
}

CFileConfig::~CFileConfig(void)
{
}

SPRESULT CFileConfig::PreInit()
{
    CHKRESULT(__super::PreInit());

    return SP_OK;
}

SPRESULT CFileConfig::Load()
{
    std::wstring BaseKey = L"Option:LTE:LTE:";
    //Band
    for (uint32 i = 0; i < MAX_LTE_BAND; i++)
    {
        m_arrBandEnable[i] = GetConfigValue((BaseKey + CLteUtility::m_BandInfo[i].NameW).c_str(), FALSE);
    }

    BaseKey = L"Option:LTE:";
    m_MainAnt = GetConfigValue((BaseKey + L"ANT:Primary").c_str(), TRUE);
    m_DivAnt  = GetConfigValue((BaseKey + L"ANT:Diversity").c_str(), TRUE);
    m_bCompatibility = GetConfigValue((BaseKey + L"Compatibility").c_str(), TRUE);

    BaseKey = L"Param:LTE:";
    m_dMaxCellPower    = GetConfigValue((BaseKey + L"MaxCellPower").c_str(), -20.0);
    m_dMinCellPower    = GetConfigValue((BaseKey + L"MinCellPower").c_str(), -75.0);
    m_dRxTarget        = GetConfigValue((BaseKey + L"RxTarget")    .c_str(), -10.0);
    m_dRxTolerance     = GetConfigValue((BaseKey + L"RxTolerance") .c_str(),   0.5);
    m_byRxIndex        = (uint8)GetConfigValue((BaseKey + L"RxIndex").c_str(), 20);
    m_nVoltage         = GetConfigValue((BaseKey + L"APT").c_str(), 24);
    
    return SP_OK;
}

int CFileConfig::GetConfigValue( LPCWSTR lpPath, int defValue )
{
    return m_pSpatBase->GetConfigValue(lpPath, defValue);
}

double CFileConfig::GetConfigValue( LPCWSTR lpPath, double defValue )
{
    return m_pSpatBase->GetConfigValue(lpPath, defValue);
}

LPCWSTR CFileConfig::GetConfigValue( LPCWSTR lpPath, LPCWSTR defString )
{
    return m_pSpatBase->GetConfigValue(lpPath, defString);
}