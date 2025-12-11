#pragma once
#include "ImpBase.h"

class CNrFileConfig
{
public:
    CNrFileConfig(void);
    virtual ~CNrFileConfig(void);

    SPRESULT Load();
    CImpBase *m_pSpatBase;
public:
    BOOL m_arrBand[MAX_NR_BAND];
    double m_dMaxCellPower;
    double m_dMinCellPower;
    double m_dRxTarget;
    double m_dRxTolerance;
    int m_nRxIndex;
    int m_nVoltage;

    double m_dTxTarget;

    double m_dLossValLower;
    double m_dLossValUpper;
    double m_dPreLoss;
    double m_dSpecLower;
    double m_dSpecUpper;

private:
    int      GetConfigValue(LPCWSTR lpPath, int     defValue);
    double   GetConfigValue(LPCWSTR lpPath, double  defValue);
    LPCWSTR  GetConfigValue(LPCWSTR lpPath, LPCWSTR defString);
};

