#pragma once
#include "FuncBase.h"
#include "NvHelper.h"
#include "FileConfig.h"
#include "ImpBase.h"
#include <vector>

class CNalGs :
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
        uint16 usTriggerArfcn;
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

	struct FreqInfo
	{
		double m_arrFreq;
		LTE_BAND_E Band;
        int nPath;
	};

    struct NvData_LTE_BAND_SUPPORT_INFO_GS
    {
        uint16 band_num;
        uint16 bandwidth_type;
        uint16 duplicate_band;
        uint16 lna_type_pri;
        uint16 lna_type_div;
        uint16 agc_rssi_comp_enable;
        uint16 dl_gain_tbl_band;
        int16 pd_start_pwr;
        int16 pd_end_pwr;
        uint16 div_disable;
        uint16 carrier_leakage_cal;
        uint16 dlca_pcc_scc;
        uint16 ulca_pcc_scc;
        uint16 TRP_pwr_backoff_enable;
        uint16 ca_apc_flag;
        uint16 PowerClass[4];
        uint16 PDET_path_select;
        uint16 ant_mapping_tx;
        uint16 ant_mapping_rx;
        uint16 cal_freq_step;
        uint16 band_start_freq;
        uint16 band_end_freq;
        uint16 Reserved6;
        uint16 ulTriggerArfcn;
        uint16 Reserved[9];
        NvData_LTE_BAND_SUPPORT_INFO_GS()
        {
            ZeroMemory(this, sizeof(*this));
        }
    };

    typedef struct NVM_MODEMV3
    {
        PC_MODEM_RF_V3_DATA_REQ_CMD_T stReqNv;
        PC_MODEM_RF_V3_DATA_PARAM_T stRspNv;

        void NVM_Uninitialize()
        {
            ZeroMemory(&stReqNv, sizeof(PC_MODEM_RF_V3_DATA_REQ_CMD_T));
            ZeroMemory(&stRspNv, sizeof(PC_MODEM_RF_V3_DATA_PARAM_T));
        }
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
    typedef CNvHelper::NvData_HelperInfo NvData_HelperInfo;

public:
    CNalGs(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CNalGs(void);

    virtual SPRESULT PreInit();

    SPRESULT GetConfig();
    SPRESULT InitResult();
    SPRESULT ClearResult();
    void Serialization(std::vector<uint8>* parrData);

public:
    std::vector<Config> m_arrConfig[MAX_LTE_ANT];
    std::vector<Result> m_arrResult[MAX_LTE_ANT];

    CNvHelper* m_pNvHelper;
    CFileConfig* m_pFileConfig;

    NvData_HelperInfo* m_ppNvBandInfo;

private:
    void InitBandInfo();

    SPRESULT GetFreqTable(LTE_ANT_E Ant);
    SPRESULT GetConfig(LTE_ANT_E Ant);
    SPRESULT InitResult(LTE_ANT_E Ant);
    SPRESULT ClearResult(LTE_ANT_E Ant);
    int Serialization(std::vector<uint8>* parrData, LTE_ANT_E Ant);

	SPRESULT GetFreqLowUp(LTE_BAND_E Band,  int nNvPos, BOOL bV2, int nTRX, double &dLow, double &dUp);
    SPRESULT ReadNv(PC_MODEM_RF_V3_DATA_PARAM_T* pNvRsp, int eNvType, int nNvPos = 0, int nRfChain = 0, int nBwId = 0, int nAnt = LTE_RF_ANT_MAIN, int nDataOffset = 0, int nDataSize = 0);

private:
    RF_CABLE_LOSS_EX* m_plossVal;
    std::vector<BandInfo> m_arrBandInfo;

    std::vector<FreqInfo> m_arrFreq;

    NVM_MODEMV3 m_stNvmParam;

    typedef struct NVM_MODEMV3_RSP_T
    {
        PC_MODEM_RF_V3_DATA_PARAM_T stRspNv;

        void NVM_Uninitialize()
        {
            ZeroMemory(&stRspNv, sizeof(PC_MODEM_RF_V3_DATA_PARAM_T));
        }
    };
    NVM_MODEMV3_RSP_T   m_stNvRsp;
    NvData_LTE_BAND_SUPPORT_INFO_GS    m_BandSupportInfo[MAX_NV_BAND_NUMBER_MV3];
};
