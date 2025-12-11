#pragma once

#define     GS19        0x47533140
#define     DUL_ANT     2
#define     MUL_ANT     4
#define     MAX_GOLDEN_SAMPLE_SIZE 4096
#define     MAX_GOLDEN_SAMPLE_SIZE_UIS8910 2048
#define     MAX_GOLDEN_SAMPLE_SIZE_UIS8850 4096

/// GSM
typedef struct _tagGSM_DATA
{
    uint8   nLoss;      /// real loss * 10;
    uint8   nPCL;       /// pcl index
    uint16  nExpPwr;    /// real power * 100;
    uint16  reserved;
}GSM_DATA;

typedef struct _tagGSM_ITEM
{
    uint16     nBand;
    uint16     nArfcn;
    GSM_DATA   data[DUL_ANT];  
}GSM_ITEM;

/// WCDMA
typedef struct _tagWCDMA_DATA
{    
    uint8   nLoss;      /// real loss * 10;
    uint8   nBand;
    uint16  nIndex;     /// gain index
    uint16  nExpPwr;    /// real power * 100;
}WCDMA_DATA;

typedef struct _tagWCDMA_ITEM
{
    uint16          nFreq;       /// real frequency * 10
    WCDMA_DATA      data[DUL_ANT];     
}WCDMA_ITEM;

/// TDSCDMA
typedef struct _tagTD_DATA
{    
    uint8   nLoss;      /// real loss * 10;
    uint8   nBand;
    uint16  nIndex;     /// gain index
    uint16  nExpPwr;    /// real power * 100;
}TD_DATA;
typedef struct _tagTD_ITEM
{
	uint16          nFreq;       /// real frequency * 10
	TD_DATA         data[DUL_ANT];     
}TD_ITEMS;

//DMR

typedef struct _tagDMR_DATA
{    
	uint8	nPcl;
	uint16  nExpPwr;    /// real power * 100;
}DMR_DATA;

typedef struct _tagDMR_ITEM
{
    uint16          nFreq;       /// real frequency * 10
    DMR_DATA        data[DUL_ANT];     
}DMR_ITEMS;

/// LTE
#pragma region LTE
typedef struct 
{
    int16  nMeaTxp;
    uint16 nTxFactor;
}LTE_TX_RESULT;

typedef struct
{
    int16 nRxRssi;
    int8  nRxCellPower;
    uint8 nRxGainIndex;
}LTE_RX_RESULT;

typedef union
{
    LTE_TX_RESULT tx;
    LTE_RX_RESULT rx;
}LTE_RESULT;

typedef struct _tagLTE_DATA
{    
    uint8 nBandIndicator;
    uint8 nRx;
    LTE_RESULT  lteResult;
}LTE_DATA;

typedef struct _tagLTE_ITEM
{
    uint16          nFreq;       /// real frequency * 10
    LTE_DATA        data[MUL_ANT];     
}LTE_ITEM;


typedef struct
{
    int8 Cnt;
    int8 AgcIdx[10];
    int16 Power[10];
}WCN_RLT_T;

//WCN
typedef struct _tagWCN_DATA
{    
//	int8   nModeType;
	int16   nProtocalType; 
    int16   nExpPwr;     /// Cell power * 100;
	int8   nPath[MAX_RF_ANT];
    int16  nPower[MAX_RF_ANT];/// real power * 100;
    WCN_RLT_T  Rlt[MAX_RF_ANT]; 
}WCN_DATA;

typedef struct _tagWCN_ITEM
{
	int16          nCh;       /// channel
	WCN_DATA        data;     
}WCN_ITEM;

#pragma endregion

typedef struct _tagGS_COMMON
{
    int nMagicNum;
    uint32 u32TotalBytes;  // 2018/03/22 @JXP Move to backward to Magic Number: total size
    SYSTEMTIME ts;
    uint8 nGsmCnt;
    uint8 nWcdmaCnt;
    uint16 usLteCnt;
    uint8 nTdCnt;
	uint8 nDmrCnt;
    uint8 nAntCount;
	uint8 nWlanCnt;
	uint8 nBtCnt;
	uint8 nGpsCnt;
}GS_COMMON;

#define MAX_GSM_LOSS_NUMBER         (20)
#define MAX_WCDMA_LOSS_NUMBER       (40)
#define MAX_LTE_LOSS_NUMBER         (60)
#define MAX_TD_LOSS_NUMBER          (13)
#define MAX_DMR_LOSS_NUMBER         (20)
#define MAX_WLAN_LOSS_NUMBER        (20)
#define MAX_BT_LOSS_NUMBER          (10)
#define MAX_GPS_LOSS_NUMBER         (1)

typedef struct _tagGS_MODULE_GS10
{
    GS_COMMON   common;
    GSM_ITEM    gsm[MAX_GSM_LOSS_NUMBER];
    WCDMA_ITEM  wcdma[MAX_WCDMA_LOSS_NUMBER];
    LTE_ITEM    lte[MAX_LTE_LOSS_NUMBER];
    TD_ITEMS    td[MAX_TD_LOSS_NUMBER];
	DMR_ITEMS	dmr[MAX_DMR_LOSS_NUMBER];
	WCN_ITEM    wlan[MAX_WLAN_LOSS_NUMBER];
	WCN_ITEM    bt[MAX_BT_LOSS_NUMBER];
	WCN_ITEM    gps[MAX_GPS_LOSS_NUMBER];
    _tagGS_MODULE_GS10()
    {
        memset(this, 0, sizeof(*this));
    }
}GS_MODULE_GS10;

enum DSP_STATE
{
    DSP_G = 0,
    DSP_T = 1
};
typedef struct _tagLOSS_LIMIT
{
    double dFreq;
    double dLossUpperLimit;
    _tagLOSS_LIMIT()
    {
        dFreq = 0;
        dLossUpperLimit = 0;
    }
}LOSS_LIMIT;