#pragma once
#include "FuncBase.h"

class CFileConfig_UIS8910 :
    public CFuncBase
{
public:
    CFileConfig_UIS8910(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CFileConfig_UIS8910(void);

    virtual SPRESULT PreInit();

    SPRESULT Load();

    void GetLossLimit(const double dFreq, double& lowerLimit, double& upperLimit);
public:
    BOOL m_arrBandEnable[MAX_LTE_BAND];
    BOOL m_bAutoBand;
    BOOL m_MainAnt;
    BOOL m_DivAnt;
    BOOL m_bCompatibility;
    BOOL m_PriorTxFreqMeasLoss;
    BOOL m_OnlyTxFreqMeasLoss;
    BOOL m_OnlyRxFreqMeasLoss;
    double m_dMaxCellPower;
    double m_dMinCellPower;
    double m_dRxTarget;
    double m_dRxTolerance;
    double m_dLossValLower;
    double m_dLossValUpper;
    uint8 m_byRxIndex;
    int m_nTestNum;
    double m_dTxTarget;
    string*m_pstrNegativeLossDetected;
    int m_Uetype;
private:
    int      GetConfigValue(LPCWSTR lpPath, int     defValue);
    double   GetConfigValue(LPCWSTR lpPath, double  defValue);
    LPCWSTR  GetConfigValue(LPCWSTR lpPath, LPCWSTR defString);

};
