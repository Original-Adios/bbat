#pragma once
#include "FuncBase.h"
#include "FileConfig.h"
#include <vector>
#include "Phonecommand_LTE.h"
#include "Phonecommand.h"

#define MAX_NV_BAND_NUMBER_MV3 40

class CModV5Nv :
    public CFuncBase
{
public:
	//Band infor
	struct NvData_LTE_BAND_LIST_INFOR_UINT
	{
		uint8 Band[MAX_LTE_BAND];
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
		LTE_CALI_RF_PATH_GROUP_T_V5 Groups;
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
		uint32 rf_channel_id;
		uint8 AntIndex;
		uint8 Group;
		uint8 DlStream;
		uint8 priordiv;
		BOOL  bSaveNvEn;
		BOOL  b2rx;
		_Band_RF_CHAIN_PATH_T()
		{
			ZeroMemory(this, sizeof(_Band_RF_CHAIN_PATH_T));
		}
	}Band_RF_CHAIN_PATH_T;

	typedef struct
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
	} Band_RF_CHAINNEL_RX_T;

	typedef struct
	{
		BOOL bChannelEn[MAX_LTE_RF_CHAINNEL - 2];
		BOOL bChannelPriEn[MAX_LTE_RF_CHAINNEL - 2];
		BOOL bChannelPriEnSave[MAX_LTE_RF_CHAINNEL - 2];
		uint8 ChannelPriPathNum[MAX_LTE_RF_CHAINNEL - 2];
		std::vector <Band_RF_CHAIN_PATH_T> priPathinfo[MAX_LTE_RF_CHAINNEL - 2];
	} Band_RF_CHAINNEL_TX_T;

	typedef struct
	{
		uint32 antRxPathNum[4] = {0};
		std::vector <Band_RF_CHAIN_PATH_T> rxPathinfo[4];
	} Band_RF_ANT_RX_T;

	typedef struct
	{
		uint32 antTxPathNum;
		std::vector <Band_RF_CHAIN_PATH_T> txPathinfo;
	} Band_RF_ANT_TX_T;

	typedef struct
	{
		uint8   Band;
		uint8   NvPos;
		Band_RF_CHAINNEL_RX_T RxChain;
		Band_RF_CHAINNEL_TX_T TxChain;
		Band_RF_ANT_RX_T RxAnt;
		Band_RF_ANT_TX_T TxAnt;
		vector <Band_RF_CHAIN_PATH_T> triggerPath;
	} Band_RF_CHAINNEL_T;
public:
    CModV5Nv(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CModV5Nv(void);

	virtual SPRESULT PreInit();
	SPRESULT InsertBand( uint8 nBand );
	SPRESULT Load();
	SPRESULT LoadClcPorComp();
	SPRESULT ReadBandList( void );
	//SPRESULT LoadSupportBandInfo( void );
	SPRESULT ReadNv( PC_MODEM_RF_V3_DATA_PARAM_T *pNvRsp, int eNvType, int nNvPos = 0, int nRfChain = 0, int nBwId = 0, int nAnt = LTE_RF_ANT_MAIN, int nDataOffset = 0, int nDataSize = 0 );
	SPRESULT WriteNv(  PC_MODEM_RF_V3_DATA_PARAM_T *pNvRsp, int eNvType, int BandId = 0, int nDataSize = 0, int nRfChain = 0, int nAnt = LTE_RF_ANT_MAIN, int nDataOffset = 0, int nBwId = 0 );
	
	SPRESULT LoadAndCheckFeatureSupport();

	SPRESULT ReadBandRfChainV5(int nIndex, LTE_BAND_E Band);
	SPRESULT ConfigBandRfChainEnV4(const unsigned int nBandIndex, int nNvPos);

    NvData_LTE_BAND_LIST_FLAG		m_BandListEnFlag;
    //NvData_LTE_BAND_SUPPORT_INFO_BAND_V5	m_BandSupportInfo[MAX_NV_BAND_NUMBER_MV3];
	NvData_LTE_BAND_SUPPORT_INFO_BAND_V5	m_BandSupportInfoV5[MAX_LTE_BAND];
	LTE_ModemV5_Download_Band_CaliSetting_V5 m_Download_CalSettingV5[MAX_LTE_BAND];

	std::vector<NvData_LTE_BAND_SUPPORT_INFO_BAND_V5>        m_arrpDownload_BandInfo;
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

	BOOL m_bBandAdaptation;
};
