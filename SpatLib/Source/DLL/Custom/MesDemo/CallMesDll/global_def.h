/************************************************************************
*                                                                       *
*   global_def.h -  global definitions                                  *
*                                                                       *
*   COPYRIGHT (C) 2019 UNISOC TECHNOLOGIES INC.                         *
*                                                                       *
************************************************************************/
#if !defined(AFX_GLOBAL_DEF_H__3C71714E_9156_4801_B01D_E4079A65C17A__INCLUDED_)
#define AFX_GLOBAL_DEF_H__3C71714E_9156_4801_B01D_E4079A65C17A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

// Counter memory distribution
// #define __MEMORY_COUNTER    1

//////////////////////////////////////////////////////////////////////////
/// 
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned __int64    uint64;

typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef __int64             int64;

//#define Internal_Debug
///
/// UNICDOE string
///
#include <string>
#if defined (UNICODE) ||  defined (_UNICODE)
    typedef std::wstring    string_t;
#else
    typedef std::string     string_t;
#endif 

//////////////////////////////////////////////////////////////////////////
///
/// Handle 
///
typedef void* SP_HANDLE;
#define INVALID_NPI_HANDLE      (NULL)

///
/// Calling convention, if VARARG functions, replaced with __cdecl.
///
#ifdef  SP_API
#undef  SP_API
#endif 
#define SP_API  //__stdcall



#ifdef __cplusplus
    #define STRUCT_INITIALIZE(_tag)         \
        _tag(void)  { \
        ZeroMemory(this, sizeof(*this));    \
    }
#else
    #define STRUCT_INITIALIZE(_tag) 
#endif

/// 
#define SAFE_RELEASE_OBJECT(_pObj)  \
    {  \
        if (NULL != (_pObj)) { \
            (_pObj)->Release(); \
            (_pObj) = NULL; \
        } \
    } \

///
/// NPI modules
///
typedef enum
{
    MODULE_DUMMY            =  0,          
    MODULE_BASE             =  1,      
    MODULE_FRAMEWORK        =  2,          
    MODULE_PHONE            =  3,            
    MODULE_GPIB             =  4,             
    MODULE_RFTESTER         =  5,            
    MODULE_DCSOURCE         =  6,         
    MODULE_SPATLIB          =  7,           
    MODULE_ENGINE           =  8,             
    MODULE_CONTAINER        =  9,
    MODULE_SEQPARSE         = 10,
    MODULE_LTE_CALI         = 11,
    MODULE_LTE_FT           = 12,
    MODULE_PHASECHECK       = 13,
    MODULE_SPDB             = 14,
	MODULE_WCDMA_CALI		= 15,
	MODULE_WCN_FT		    = 16,
	MODULE_GSM				= 17,
	MODULE_DMR				= 18,
	MODULE_WCDMA_FT			= 19,
	MODULE_TD_CAL			= 20,
	MODULE_TD_FT			= 21,
	MODULE_REPEATER			= 22,
    MODULE_CHECK_WIRTE_X    = 23,
    MODULE_SECURITY         = 24,
    MODULE_DEPLOY           = 25,
	MODULE_CDMA				= 26,
    MODULE_NV_TEST,
    MODULE_CUSTOMIZE,
    MODULE_NR,
    MODULE_BBAT,
    MODULE_NR_FT,
    MODULE_MAX_TYPE
} SP_MODULE_TYPE;

///
/// Magic word 
///
const DWORD MAGIC_SP14 = 0x53503134; // "SP14"

/// 
/// IP: xxx.xxx.xxx.xxx
#define MAX_IP_ADDR_LEN             ( 16 )

///
///	Calculate array size.
///
#define ARRAY_SIZE(a)	            ( sizeof((a))/sizeof((a)[0]) )


///
/// Limit check 
///
#define IN_RANGE(low, x, high)      ( (x) >= (low) && (x) <= (high) )


/// 
/// Integer to string
/// Example: 
///      NUM2STR(Hello, __LINE__)   ==> Hello129
///
#define NUM2STR_2(x,y)      x##y
#define NUM2STR(x,y)        NUM2STR_2(x,y)

#define TOSTR(str)          #str
#define TOWSTR(str)        L#str

#define CopySize(x)         (sizeof(x) - 1)


#ifdef __cplusplus
#define CLASS_UNCOPYABLE(class_name) \
    private: \
    class_name##(const class_name##&); \
    class_name##& operator=(const class_name##&);
#endif 
///
/// Timeout, unit: ms
/// 
#define TIMEOUT_1S      ( 1000)
#define TIMEOUT_2S      ( 2000)
#define TIMEOUT_3S      ( 3000)
#define TIMEOUT_5S      ( 5000)
#define TIMEOUT_10S     (10000)
#define TIMEOUT_20S     (20000)
#define TIMEOUT_30S     (30000)
#define TIMEOUT_60S     (60000)
#define TIMEOUT_100S    (100000)
#define TIMEOUT_180S    (180000)

//////////////////////////////////////////////////////////////////////////
/// Cellular Definition
/// 
typedef enum
{
    SP_INVALID  =-1,

	SP_CELLUAR_START	= 0,
    SP_GSM				= SP_CELLUAR_START,
    SP_TDSCDMA,
    SP_WCDMA,
	SP_C2K,
    SP_LTE,
    SP_NR,
	SP_CELLUAR_END,
	MAX_CELLUAR_TYPE	=(SP_CELLUAR_END - SP_CELLUAR_START),

	SP_CONNECITITY_START= 10,
    SP_WIFI				= SP_CONNECITITY_START,
    SP_BT,
    SP_GPS,
	SP_CONNECITITY_END,
    MAX_CONNECITITY_TYPE=(SP_CONNECITITY_END - SP_CONNECITITY_START),

	SP_MODE_MAX			=(MAX_CELLUAR_TYPE + MAX_CONNECITITY_TYPE),

} SP_MODE_INFO;


typedef enum
{
    Signal  =  0,
    NonSignal,
    LMT,
	ANT,      // TRX  强发强收
    SyncANT,  // Sync 非信令同步
    MAX_FT_MODE
} FT_MODE_E;


///
/// BAND
///
typedef enum
{
    BI_INVALID = -1,
    /// GSM
    BI_GSM_850 = 0,
    BI_EGSM,
    BI_DCS,
    BI_PCS,
    BI_GSM_MAX_BAND,

    /// TD
	BI_TD_20G = 0,
    BI_TD_19G,
    BI_TD_MAX_BAND,

    /// WCDMA
    BI_W_B1 = 0,
    BI_W_B2,
	BI_W_B3,
	BI_W_B4,
    BI_W_B5,
	BI_W_B6,
	BI_W_B7,
    BI_W_B8,
	BI_W_B9,
	BI_W_B10,
	BI_W_B11,
	BI_W_B12,
	BI_W_B13,
	BI_W_B14,
	BI_W_B15,
	BI_W_B16,
	BI_W_B17,
	BI_W_B18,
	BI_W_B19,
	BI_W_B20,
	BI_W_B21,
	BI_W_B22,
	BI_W_B23,
	BI_W_B24,
	BI_W_B25,
	BI_W_B26,
    BI_W_MAX_BAND,

	BI_C_BC0 = 0,
	BI_C_BC1,
	BI_C_BC2,
	BI_C_BC3,
	BI_C_BC4,
	BI_C_BC5,
	BI_C_BC6,
	BI_C_BC7,
	BI_C_BC8,
	BI_C_BC9,
	BI_C_BC10,
	BI_C_BC11,
	BI_C_BC12,
	BI_C_BC13,
	BI_C_BC14,
	BI_C_BC15,
	BI_C_BC16,
	BI_C_BC17,
	BI_C_BC18,
	BI_C_BC19,
	BI_C_BC20,
	BI_C_BC21A,
	BI_C_BC21B,
	BI_C_MAX_BAND,
    /// LTE
    BI_LTE_TDD = 0,
	BI_LTE_FDD,
	BI_LTE_MAX,

    ///WCN
    BI_WCN_WIFI_11B = 0,
    BI_WCN_WIFI_11G,
    BI_WCN_WIFI_11N,
    BI_WCN_WIFI_11AC_20M,
    BI_WCN_WIFI_11AC_40M,
    BI_WCN_WIFI_11AC_80M,
    BI_WCN_WIFI_MAX,

    ///BT 
    BI_WCN_BT_BDR = 0,
    BI_WCN_BT_EDR = 1,
    BI_WCN_BT_BLE = 2,
    BI_WCN_BT_MAX, 

    // NBIOT
    BI_NBIOT_FDD = 0,
    BI_NBIOT_MAX,

    /// NR
    BI_NR_TDD = 0,
	BI_NR_FDD,
	BI_NR_MAX,

} SP_BAND_INFO;

typedef enum
{
	CAL_INVALID = -1,
	CAL_FDT,		/// Only FDT mode
	CAL_COMMON,		/// Only Common mode
	CAL_FDT_COMMON,	/// If FDT fails, then run common to adjust.
	CAL_INTERNAL,   /// Internal calibration
	CAL_TYPE_MAX
} E_CAL_ALGO_TYPE;

typedef enum
{
	RF_ANT_INVALID = -1,
    RF_ANT_1st, /// primary 
    RF_ANT_2nd, /// diversity
    RF_ANT_3rd, /// primary 2
    RF_ANT_4th, /// diversity 2
	RF_ANT_5th,
	RF_ANT_6th,
	RF_ANT_7th,
	RF_ANT_8th,
    MAX_RF_ANT
}RF_ANT_E;

typedef enum
{
    RF_ANT_FLAG_1st = 1,
    RF_ANT_FLAG_2nd = 2,
    RF_ANT_FLAG_3rd = 4,
    RF_ANT_FLAG_4th = 8,

}RF_ANT_FLAG_E;

/// 
#define INVALID_POSITIVE_DOUBLE_VALUE       (+999999999999.00)
#define INVALID_NEGATIVE_DOUBLE_VALUE       (-999999999999.00)

#define INVALID_POSITIVE_INTEGER_VALUE      (+99999999)
#define INVALID_NEGATIVE_INTEGER_VALUE      (-99999999)

#define ROUNDTOINTEGER(_x)                  ( ( ((_x) > 0) ? (int((_x) + 0.5)) : (int((_x) - 0.5)) ) )
#define IS_EQUAL(d1,d2)                     (((d1)-(d2))>-0.000001 && ((d1)-(d2))<0.000001)
#define IS_BIT_SET(mask, bit)               ((mask) & (bit))

enum
{
    FAIL = 0,
    PASS = 1
};

// General param definition
typedef struct  _tagDOUBLE_LIMIT
{
	double low;
	double upp;
	void Init()
	{
		low = 0;
		upp = 0;
	}

	_tagDOUBLE_LIMIT()
	{
		Init();
	}
} DOUBLE_LIMIT;

typedef struct  _tagINTEGER_LIMIT
{
	int low;
	int upp;
	void Init()
	{
		low = 0;
		upp = 0;
	}
	_tagINTEGER_LIMIT()
	{
		Init();
	}
} INTEGER_LIMIT;

// 


#endif /* AFX_GLOBAL_DEF_H__3C71714E_9156_4801_B01D_E4079A65C17A__INCLUDED_ */