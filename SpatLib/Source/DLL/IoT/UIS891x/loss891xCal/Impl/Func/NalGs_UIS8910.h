#pragma once
#include "FuncBase.h"
#include "Nv_UIS8910.h"
#include "FileConfig_UIS8910.h"
#include "ImpBaseUIS8910.h"
#include <vector>

class CNalGs_UIS8910 :
    public CFuncBase
{
public:
#define TX 1
#define RX 2
#define NotDetermined 3

    struct TxConfig
    {
        double dTxTarget;
    };

    struct RxConfig
    {
        double dRxTarget;
    };

    struct Config
    {
        double dFreq;
        uint32 usChannel;
        uint8  byBand;
        uint8  byPath;
    };

    struct TxResult
    {
        uint16 usWord;
        double dPower;
        double dTargetPower;
    };

    struct RxResult
    {
        uint8 byIndex;
        double dRssi;
        double dCellPower;
    };

    struct Result
    {
        BOOL bDone;
        RxResult RxResult;
        TxResult TxResult;
    };

    struct BandInfo
    {
        LTE_BAND_E Band;
        BOOL Enable[MAX_LTE_ANT];
        BOOL Tx[MAX_LTE_ANT];
        LTE_CA_E TxCa[MAX_LTE_ANT];
    };

    class cmp    
    {    
    public:
        bool operator () (const double& a,const double& b)    
        {    
            return a < b;    
        }  
    }; 
private:
    typedef CNv_UIS8910::NvData_BandInfo NvData_BandInfo;

public:
    CNalGs_UIS8910(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CNalGs_UIS8910(void);

    virtual SPRESULT PreInit();

    SPRESULT GetConfig();
    SPRESULT InitResult();
    SPRESULT ClearResult();
    SPRESULT InitResultAcc();
    SPRESULT ClearResultAcc();
    SPRESULT AccResultPro(int ave_calc);
    void Serialization(std::vector<uint8>* parrData);

public:
    std::vector<Config> m_arrConfig[MAX_LTE_ANT];
    std::vector<Result> m_arrResult[MAX_LTE_ANT];
    std::vector<Result> m_arrResultAcc[MAX_LTE_ANT];

    CNv_UIS8910* m_pNv;
    CFileConfig_UIS8910* m_pFileConfig;

    NvData_BandInfo** m_ppNvBandInfo;

private:
    void InitBandInfo();

    SPRESULT GetFreqTable(LTE_ANT_E Ant);
    SPRESULT GetConfigRx(LTE_ANT_E Ant);
    SPRESULT GetConfigTx(LTE_ANT_E Ant);
    SPRESULT InitResult(LTE_ANT_E Ant);
    SPRESULT ClearResult(LTE_ANT_E Ant);
    SPRESULT InitResultAcc(LTE_ANT_E Ant);
    SPRESULT ClearResultAcc(LTE_ANT_E Ant);
    int Serialization(std::vector<uint8>* parrData, LTE_ANT_E Ant);

private:
    RF_CABLE_LOSS_EX* m_plossVal;
    std::vector<BandInfo> m_arrBandInfo;

    std::vector<double> m_arrFreq;
};
