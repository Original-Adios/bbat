#pragma once
#include "Define.h"


#define     GS19        0x47533140
#define     GS20        0x47533230
#define     GS21        0x47533231
#define     GS_VERSION  GS21
#define     GS30        0x47533330

#define     DUL_ANT     2
#define     MUL_ANT     4
#define     WCN_ANT     4
#define     MAX_GOLDEN_SAMPLE_SIZE 4096
#define     MAX_GOLDEN_SAMPLE_CUSTOMER_SIZE 1024 * 64

/// GSM
typedef struct _tagGSM_DATA
{
	uint8   nLoss;      /// real loss * 10;
	uint8   nPCL;       /// pcl index
	uint16  nExpPwr;    /// real power * 100;
	uint16  reserved;
} GSM_DATA;

typedef struct _tagGSM_ITEM
{
	uint16     nBand;
	uint16     nArfcn;
	GSM_DATA   data[DUL_ANT];
} GSM_ITEM;

/// WCDMA
typedef struct _tagWCDMA_DATA
{
	uint8   nLoss;      /// real loss * 10;
	uint8   nBand;
	uint16  nIndex;     /// gain index
	uint16  nExpPwr;    /// real power * 100;
} WCDMA_DATA;

typedef struct _tagWCDMA_ITEM
{
	uint16          nFreq;       /// real frequency * 10
	WCDMA_DATA      data[DUL_ANT];
} WCDMA_ITEM;

/// C2K
typedef struct _tagC2K_DATA
{
	uint8   nLoss;      /// real loss * 10;
	uint8   nBand;
	uint16  nIndex;     /// gain index
	uint16  nExpPwr;    /// real power * 100;
} C2K_DATA;

typedef struct _tagC2K_ITEM
{
	uint16          nFreq;       /// real frequency * 10
	C2K_DATA        data[DUL_ANT];
} C2K_ITEM;

/// TDSCDMA
typedef struct _tagTD_DATA
{
	uint8   nLoss;      /// real loss * 10;
	uint8   nBand;
	uint16  nIndex;     /// gain index
	uint16  nExpPwr;    /// real power * 100;
} TD_DATA;
typedef struct _tagTD_ITEM
{
	uint16          nFreq;       /// real frequency * 10
	TD_DATA         data[DUL_ANT];
} TD_ITEMS;

//DMR

typedef struct _tagDMR_DATA
{
	uint8   nPcl;
	uint16  nExpPwr;    /// real power * 100;
} DMR_DATA;

typedef struct _tagDMR_ITEM
{
	uint16          nFreq;       /// real frequency * 10
	DMR_DATA        data[DUL_ANT];
} DMR_ITEMS;

/// LTE
#pragma region LTE
typedef struct
{
	int16  nMeaTxp;
	uint16 nTxFactor;
} LTE_TX_RESULT;

typedef struct
{
	int16 nRxRssi;
	int8  nRxCellPower;
	uint8 nRxGainIndex;
} LTE_RX_RESULT;

typedef union
{
	LTE_TX_RESULT tx;
	LTE_RX_RESULT rx;
} LTE_RESULT;

typedef struct _tagLTE_DATA
{
	uint8 nBandIndicator;
	uint8 nRx;
	LTE_RESULT  lteResult;
} LTE_DATA;

typedef struct _tagLTE_ITEM
{
	uint16          nFreq;       /// real frequency * 10
	LTE_DATA        data[MUL_ANT];
} LTE_ITEM;


//WCN
typedef struct _tagWCN_DATA
{
	//  int8   nModeType;
	int8   nProtocalType;
	int8   nPath[WCN_ANT];
	int16  nPower[WCN_ANT]; /// real power * 100;
} WCN_DATA;

typedef struct _tagWCN_ITEM
{
	int16          nCh;       /// channel
	WCN_DATA        data;
} WCN_ITEM;

#pragma endregion

typedef struct _tagGS_COMMON
{
	int nMagicNum;
	uint32 u32TotalBytes;  // 2018/03/22 @JXP Move to backward to Magic Number: total size
	SYSTEMTIME ts;
	uint8 nGsmCnt;
	uint8 nWcdmaCnt;
	uint8 nC2KCnt;
	uint8 nTdCnt;
	uint16 usLteCnt;
	uint16 usNrCnt;
	uint8 nDmrCnt;
	uint8 nAntCount;
	uint8 nWlanCnt;
	uint8 nBtCnt;
	uint8 nGpsCnt;
	_tagGS_COMMON()
	{
		memset(this, 0, sizeof(_tagGS_COMMON));
	}
} GS_COMMON;

typedef struct _tagGS_COMMON_R6
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
	_tagGS_COMMON_R6()
	{
		memset(this, 0, sizeof(_tagGS_COMMON_R6));
	}
} GS_COMMON_R6;

#define MAX_GSM_LOSS_NUMBER         (20)
#define MAX_WCDMA_LOSS_NUMBER       (40)
#define MAX_LTE_LOSS_NUMBER         (100)
#define MAX_TD_LOSS_NUMBER          (13)
#define MAX_DMR_LOSS_NUMBER         (20)
#define MAX_WLAN_LOSS_NUMBER        (20)
#define MAX_BT_LOSS_NUMBER          (10)
#define MAX_GPS_LOSS_NUMBER         (4)
#define MAX_C2K_LOSS_NUMBER         (20)

typedef struct _tagGS_MODULE_GS10
{
	GS_COMMON   common;
	GSM_ITEM    gsm[MAX_GSM_LOSS_NUMBER];
	WCDMA_ITEM  wcdma[MAX_WCDMA_LOSS_NUMBER];
	C2K_ITEM    c2k[MAX_WCDMA_LOSS_NUMBER];
	LTE_Loss_Data  lte[MAX_LTE_LOSS_NUMBER];
	TD_ITEMS    td[MAX_TD_LOSS_NUMBER];
	DMR_ITEMS   dmr[MAX_DMR_LOSS_NUMBER];
	WCN_ITEM    wlan[MAX_WLAN_LOSS_NUMBER];
	WCN_ITEM    bt[MAX_BT_LOSS_NUMBER];
	WCN_ITEM    gps[MAX_GPS_LOSS_NUMBER];
} GS_MODULE_GS10;

/// The order of the enumeration must not be changed,
/// otherwise compatibility cannot be achieved.
typedef enum
{
	GS_GSM = 0,
	GS_TD,
	GS_WCDMA,
	GS_C2K,
	GS_LTE,
	GS_DMR,
	GS_BT,
	GS_WLAN,
	GS_GPS,
	GS_NR,
	GS_MAX = 0xFF
}GS_RAT;

typedef struct _GS_DATA_HEADER
{
	int nMagicNum;
	uint32 u32TotalBytes;
	SYSTEMTIME ts;
	_GS_DATA_HEADER()
	{
		memset(this, 0, sizeof(_GS_DATA_HEADER));
	}
}GS_DATA_HEADER;

typedef struct GS_RAT_HEAD
{
	uint16 rat;
	uint16 size;
	GS_RAT_HEAD()
	{
		memset(this, 0, sizeof(GS_RAT_HEAD));
	}
}GS_RAT_HEAD;

enum DSP_STATE
{
	DSP_G = 0,
	DSP_T = 1
};


//imp&exp
typedef struct _tag_IMPEXP_ITEM_DATA
{
	int nLoss;
	int nPath;     //WCN
	int nProtocol; //WCN
	int nIndex;   //TX or RX Index PCL
	int nTxRx; //TX or RX
	double dCellPower;
	double dPower;
	_tag_IMPEXP_ITEM_DATA()
	{
		memset(this, 0, sizeof(_tag_IMPEXP_ITEM_DATA));
	}
}IMPEXP_ITEM_DATA;
