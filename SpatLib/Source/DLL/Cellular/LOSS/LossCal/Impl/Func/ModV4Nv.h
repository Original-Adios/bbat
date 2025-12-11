#pragma once
#include "FuncBase.h"
#include "FileConfig.h"
#include <vector>

#define MAX_NV_BAND_NUMBER_MV3 30

class CModV4Nv :
    public CFuncBase
{
public:
	//Band infor
	struct NvData_LTE_BAND_LIST_INFOR_UINT
	{
		uint8 Band[MAX_NV_BAND_NUMBER_MV3];
	};
	struct NvData_LTE_BAND_LIST_INFOR
	{
		uint32 BandCount;
		NvData_LTE_BAND_LIST_INFOR_UINT BandList;
	};

	//LTE_bandinfo
	struct Band_Info
	{
		LTE_BAND_E nBand;
		int nNumeral;
		int nIndicator;
	};

	struct NvData_LTE_BAND_SUPPORT_INFO
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
		uint16 Reserved;
		uint16 ulTriggerArfcn = 0;
		uint16 arrReserved[9];
	};

	typedef struct NVM_MODEMV3
	{
		PC_MODEM_RF_V3_DATA_REQ_CMD_T stReqNv;
		PC_MODEM_RF_V3_DATA_PARAM_T	stRspNv;

		void NVM_Uninitialize()
		{
			ZeroMemory(&stReqNv, sizeof(PC_MODEM_RF_V3_DATA_REQ_CMD_T));
			ZeroMemory(&stRspNv, sizeof(PC_MODEM_RF_V3_DATA_PARAM_T));
		}
	};

	typedef struct
	{
		uint8   RxRfChainNum;
		uint8   RxRfChain[MAX_PC_MODEM_V3_LTE_RF_DL_CHAIN_CNT];
		uint8   TxRfChainNum;
		uint8   TxRfChain[MAX_PC_MODEM_V3_LTE_RF_UL_CHAIN_CNT];
		uint8   Band;
	} NvData_LTE_BAND_RF_CHAIN;

	typedef struct
	{
		//uint8   Band;
		RF_LTE_CALI_RF_CH_T RFChannel;
	}NvData_LTE_BAND_RF_PATH;

	typedef struct
	{
		BOOL bCC0En;
		BOOL bCC1En;
		BOOL bCC2En;
	} Band_RF_CHAIN_RX_T;

	typedef struct
	{
		BOOL bCC0En;
		BOOL bCC1En;
	} Band_RF_CHAIN_TX_T;

	typedef struct
	{
		uint8   Band;
		uint8   NvPos;
		Band_RF_CHAIN_RX_T RxChain;
		Band_RF_CHAIN_TX_T TxChain;
	} Band_RF_CHAIN_T;

	typedef struct _Band_RF_CHAIN_PATH_T
	{
		uint8 rf_channel_id;
		uint8 AntIndex;
		uint8 channel;
		uint8 priordiv;
		BOOL  bSaveNvEn;
		BOOL  b2rx;
		_Band_RF_CHAIN_PATH_T()
		{
			ZeroMemory(this, sizeof(_Band_RF_CHAIN_PATH_T));
		}
	}Band_RF_CHAIN_PATH_T;

	typedef struct _Band_RF_CHAINNEL_RX_T
	{
		BOOL bChannelEn[MAX_LTE_RF_CHAINNEL];
		BOOL bChannelPriEn[MAX_LTE_RF_CHAINNEL];
		BOOL bChannelPriEnSave[MAX_LTE_RF_CHAINNEL];
		BOOL bChannelDivEn[MAX_LTE_RF_CHAINNEL];
		BOOL bChannelDivEnSave[MAX_LTE_RF_CHAINNEL];
		uint8 ChannelPriPathNum[MAX_LTE_RF_CHAINNEL];
		uint8 ChannelDivPathNum[MAX_LTE_RF_CHAINNEL];
		std::vector <Band_RF_CHAIN_PATH_T> priPathinfo[MAX_LTE_RF_CHAINNEL];
		std::vector <Band_RF_CHAIN_PATH_T> divPathinfo[MAX_LTE_RF_CHAINNEL];
		_Band_RF_CHAINNEL_RX_T()
		{
			memset(&bChannelEn, 0x0, sizeof(bChannelEn));
			memset(&bChannelPriEn, 0x0, sizeof(bChannelPriEn));
			memset(&bChannelPriEnSave, 0x0, sizeof(bChannelPriEnSave));
			memset(&bChannelDivEn, 0x0, sizeof(bChannelDivEn));
			memset(&bChannelDivEnSave, 0x0, sizeof(bChannelDivEnSave));
			memset(&ChannelPriPathNum, 0x0, sizeof(ChannelPriPathNum));
			memset(&ChannelDivPathNum, 0x0, sizeof(ChannelDivPathNum));
			for (int i = 0; i < MAX_LTE_RF_CHAINNEL; ++i)
			{
				priPathinfo[i].clear();
				divPathinfo[i].clear();
			}

		}
	} Band_RF_CHAINNEL_RX_T;

	typedef struct _Band_RF_CHAINNEL_TX_T
	{
		BOOL bChannelEn[MAX_LTE_RF_CHAINNEL];
		BOOL bChannelPriEn[MAX_LTE_RF_CHAINNEL];
		BOOL bChannelPriEnSave[MAX_LTE_RF_CHAINNEL];
		uint8 ChannelPriPathNum[MAX_LTE_RF_CHAINNEL];
		std::vector <Band_RF_CHAIN_PATH_T> priPathinfo[MAX_LTE_RF_CHAINNEL];
		_Band_RF_CHAINNEL_TX_T()
		{
			memset(&bChannelEn, 0x0, sizeof(bChannelEn));
			memset(&bChannelPriEn, 0x0, sizeof(bChannelPriEn));
			memset(&bChannelPriEnSave, 0x0, sizeof(bChannelPriEnSave));
			memset(&ChannelPriPathNum, 0x0, sizeof(ChannelPriPathNum));
			for (int i = 0; i < MAX_LTE_RF_CHAINNEL; ++i)
			{
				priPathinfo[i].clear();
			}

		}
	} Band_RF_CHAINNEL_TX_T;

	typedef struct _Band_RF_ANT_RX_T
	{
		uint32 antRxPathNum[4];
		std::vector <Band_RF_CHAIN_PATH_T> rxPathinfo[4];
		_Band_RF_ANT_RX_T()
		{
			memset(&antRxPathNum, 0x0, sizeof(antRxPathNum));
			for (int i = 0; i < MAX_LTE_RF_CHAINNEL; ++i)
			{
				rxPathinfo[i].clear();
			}

		}
	} Band_RF_ANT_RX_T;

	typedef struct _Band_RF_ANT_TX_T
	{
		uint32 antTxPathNum[4];
		std::vector <Band_RF_CHAIN_PATH_T> txPathinfo[4];
		_Band_RF_ANT_TX_T()
		{
			memset(&antTxPathNum, 0x0, sizeof(antTxPathNum));
			for (int i = 0; i < MAX_LTE_RF_CHAINNEL; ++i)
			{
				txPathinfo[i].clear();
			}
		}

	} Band_RF_ANT_TX_T;

	typedef struct _Band_RF_CHAINNEL_T
	{
		uint8   Band;
		uint8   NvPos;
		Band_RF_CHAINNEL_RX_T RxChain;
		Band_RF_CHAINNEL_TX_T TxChain;
		Band_RF_ANT_RX_T RxAnt;
		Band_RF_ANT_TX_T TxAnt;
		vector <Band_RF_CHAIN_PATH_T> triggerPath;
		_Band_RF_CHAINNEL_T()
		{
			Band = 0;
			NvPos = 0;
			triggerPath.clear();
		}
	} Band_RF_CHAINNEL_T;
public:
    CModV4Nv(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CModV4Nv(void);

	virtual SPRESULT PreInit();
	SPRESULT InsertBand( uint8 nBand );
	SPRESULT Load();
	SPRESULT LoadClcPorComp();
	SPRESULT ReadBandList( void );
	SPRESULT LoadSupportBandInfo( void );
	SPRESULT ReadNv( PC_MODEM_RF_V3_DATA_PARAM_T *pNvRsp, int eNvType, int nNvPos = 0, int nRfChain = 0, int nBwId = 0, int nAnt = LTE_RF_ANT_MAIN, int nDataOffset = 0, int nDataSize = 0 );
	SPRESULT WriteNv(  PC_MODEM_RF_V3_DATA_PARAM_T *pNvRsp, int eNvType, int BandId = 0, int nDataSize = 0, int nRfChain = 0, int nAnt = LTE_RF_ANT_MAIN, int nDataOffset = 0, int nBwId = 0 );
	
	SPRESULT LoadAndCheckFeatureSupport();

	SPRESULT ReadBandRfChainV4(int nIndex, LTE_BAND_IDENT_E Band);
	SPRESULT ConfigBandRfChainEnV4(const unsigned int nBandIndex, int nNvPos);

	SPRESULT ParsePathInfo(int nIndex,LTE_BAND_E Band);

	NvData_LTE_BAND_LIST_INFOR		m_BandListEnFlag;
	NvData_LTE_BAND_SUPPORT_INFO	m_BandSupportInfo[MAX_NV_BAND_NUMBER_MV3];
	std::vector<NvData_LTE_BAND_SUPPORT_INFO>        m_arrpDownload_BandInfo;
	std::vector<Band_Info> m_arrBandInfo;
	int m_nMaxBandCount;
	uint32 m_nBandCnt;
	uint8 m_nBand[MAX_NV_BAND_NUMBER_MV3];
	CFileConfig* m_pFileConfig;
	uint16 m_nCaliParamVersion;

	NvData_LTE_BAND_RF_PATH            m_BandPathInfo[MAX_NV_BAND_NUMBER_MV3];
	Band_RF_CHAINNEL_T              m_BandPathInfoEn[MAX_NV_BAND_NUMBER_MV3];

protected:
	SP_HANDLE  m_hDUT;  
	NVM_MODEMV3	m_stNvmParam;

private:
	SPRESULT InitBandNv();
	SPRESULT LoadDownloadAntMap( int nBandIndex, int nBandPos );
	SPRESULT GetPortComp( void );

	typedef struct NVM_MODEMV3_RSP_T
	{
		PC_MODEM_RF_V3_DATA_PARAM_T	stRspNv;

		void NVM_Uninitialize()
		{
			ZeroMemory(&stRspNv, sizeof(PC_MODEM_RF_V3_DATA_PARAM_T));
		}
	};
	NVM_MODEMV3_RSP_T	m_stNvRsp;
	NVM_MODEMV3_RSP_T	m_stProgramNvData;

	std::vector<int> m_arrBandSelected;
  
public:
	vector <READ_NV_PARAM_RLT_ANT_MAP> m_arrGS_PortComp;
	READ_NV_PARAM_RLT_ANT_MAP  m_arrDownload_AntMap[MAX_LTE_BAND];

	std::vector<int> m_nClcBand;
	std::vector<int> m_arrNvPos;
	NvData_LTE_COMMON_CONFIG m_Download_CommonConfig;
	NvData_CalReserved m_Calibration_Reserved[MAX_LTE_BAND];
	LTE_BAND_PATH_INFO m_arrPathInfo[MAX_NV_BAND_NUMBER_MV3];
	BOOL m_bBandAdaptation;
	BOOL m_bPathAntEnable;
	BOOL m_bNewPathStructure;
};
