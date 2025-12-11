#pragma once
#include "FuncBase.h"
#include "NvHelperV4.h"
#include "FileConfig.h"
#include "ImpBase.h"
#include <vector>

class CNalGsV4 :
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
        uint8  channelId;
        uint16 nTriggerArfcn;
        uint8  antTxIndex;
        uint8  antRxIndex;
    };

    struct TxResult
    {
        uint16 usWord;
        double dPower;
        double dTargetPower;
        int nVoltage;
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
    };

	struct FreqInfo
	{
		double m_arrFreq;
		LTE_BAND_E Band;
        int nPath;
	};

    class cmp    
    {    
    public:
        bool operator () (const FreqInfo& a,const FreqInfo& b)    
        {    
            return a.m_arrFreq < b.m_arrFreq;    
        }  
    }; 
private:
    typedef CNvHelperV4::NvData_HelperInfo NvData_HelperInfo;

public:
    CNalGsV4(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CNalGsV4(void);

    virtual SPRESULT PreInit();

    SPRESULT GetConfig();
    SPRESULT InitResult();
    SPRESULT ClearResult();
    void Serialization(std::vector<uint8>* parrData);

public:
    std::vector<Config> m_arrConfig[MAX_LTE_RF_ANTENNA];
    std::vector<Result> m_arrResult[MAX_LTE_RF_ANTENNA];

    CNvHelperV4* m_pNvHelper;
    CFileConfig* m_pFileConfig;

    NvData_HelperInfo* m_ppNvBandInfo;

private:
    void InitBandInfo();

    SPRESULT GetFreqTable(LTE_RF_ANTENNA_E Ant);
    SPRESULT GetConfig(LTE_RF_ANTENNA_E Ant);
    SPRESULT InitResult(LTE_RF_ANTENNA_E Ant);
    SPRESULT ClearResult(LTE_RF_ANTENNA_E Ant);
    int Serialization(std::vector<uint8>* parrData, LTE_RF_ANTENNA_E Ant);

	SPRESULT GetFreqLowUp(LTE_BAND_E Band,  int nNvPos, BOOL bV2, int nTRX, double &dLow, double &dUp);

private:
    RF_CABLE_LOSS_EX* m_plossVal;
    std::vector<BandInfo> m_arrBandInfo;

    std::vector<FreqInfo> m_arrFreq;
};
