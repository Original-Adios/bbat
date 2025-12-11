#pragma once
#include "FuncBase.h"
#include "FileConfig.h"
#include <vector>

#define MAX_NV_BAND_NUMBER_MV3 23

class CModV3Nv :
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
		uint16 Reserved6;
		uint16 ulTriggerArfcn;
		uint16 Reserved[9];
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
public:
    CModV3Nv(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CModV3Nv(void);

	virtual SPRESULT PreInit();
	SPRESULT InsertBand( uint8 nBand );
	SPRESULT Load();
	SPRESULT LoadClcPorComp();
	SPRESULT ReadBandList( void );
	SPRESULT LoadSupportBandInfo( void );
	SPRESULT ReadNv( PC_MODEM_RF_V3_DATA_PARAM_T *pNvRsp, int eNvType, int nNvPos = 0, int nRfChain = 0, int nBwId = 0, int nAnt = LTE_RF_ANT_MAIN, int nDataOffset = 0, int nDataSize = 0 );
	SPRESULT WriteNv(  PC_MODEM_RF_V3_DATA_PARAM_T *pNvRsp, int eNvType, int BandId = 0, int nDataSize = 0, int nRfChain = 0, int nAnt = LTE_RF_ANT_MAIN, int nDataOffset = 0, int nBwId = 0 );
	
	SPRESULT LoadAndCheckFeatureSupport();

	NvData_LTE_BAND_LIST_INFOR		m_BandListEnFlag;
	NvData_LTE_BAND_SUPPORT_INFO	m_BandSupportInfo[MAX_NV_BAND_NUMBER_MV3];
	std::vector<NvData_LTE_BAND_SUPPORT_INFO>        m_arrpDownload_BandInfo;
	std::vector<Band_Info> m_arrBandInfo;
	int m_nMaxBandCount;
	uint32 m_nBandCnt;
	uint8 m_nBand[MAX_NV_BAND_NUMBER_MV3];
	CFileConfig* m_pFileConfig;
	uint16 m_nCaliParamVersion;

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

	BOOL m_bBandAdaptation;
};
