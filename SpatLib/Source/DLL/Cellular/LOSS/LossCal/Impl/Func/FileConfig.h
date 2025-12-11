#pragma once
#include "FuncBase.h"

class CFileConfig :
    public CFuncBase
{
public:
    CFileConfig(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CFileConfig(void);

    virtual SPRESULT PreInit();

    SPRESULT Load();

public:
    BOOL m_arrBandEnable[MAX_LTE_BAND];
    BOOL m_MainAnt;
    BOOL m_DivAnt;
    BOOL m_bCompatibility;
    double m_dMaxCellPower;
    double m_dMinCellPower;
    double m_dRxTarget;
    double m_dRxTolerance;
	double m_dLossValLower;
	double m_dLossValUpper;
	double m_dPreLoss;
    uint8 m_byRxIndex;
    int m_nVoltage;

	string*m_pstrNegativeLossDetected;
private:
    int      GetConfigValue(LPCWSTR lpPath, int     defValue);
    double   GetConfigValue(LPCWSTR lpPath, double  defValue);
    LPCWSTR  GetConfigValue(LPCWSTR lpPath, LPCWSTR defString);

};
