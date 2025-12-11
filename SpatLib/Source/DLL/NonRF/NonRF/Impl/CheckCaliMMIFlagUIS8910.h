#pragma once
#include "ImpBase.h"
#include "SpatBase.h"
#include "global_def.h"
using namespace std;
//////////////////////////////////////////////////////////////////////////
class CCheckCaliMMIFlagUIS8910 : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckCaliMMIFlagUIS8910)
public:
    CCheckCaliMMIFlagUIS8910(void);
    virtual ~CCheckCaliMMIFlagUIS8910(void);
    typedef enum
    {
        WHOLE_FLAG,
        PCBA_FLAG,
        BBAT_FLAG,
        APK_FLAG
    }MMI_FLAG_CHECK_E;

    typedef enum
    {
        FLAGMODE_GSM,
        FLAGMODE_LTE,
        FLAGMODE_WIFI,
    }FLAG_MODE_E;

    typedef enum
    {
        GSM_CALI_AFC = 0,
        GSM850_CALI_AGC,
        GSM850_CALI_APC,
        EGSM_CALI_AGC,
        EGSM_CALI_APC,
        DCS_CALI_AGC,
        DCS_CALI_APC,
        PCS_CALI_AGC,
        PCS_CALI_APC,
        GSM_FINAL,
        GSM_ANTENNA,
        GSM_FINAL2,
        LTE_CALI_AFC,
        LTE_CALI_AGC_TDD,
        LTE_CALI_APC_TDD,
        LTE_CALI_TX_PDT,
        LTE_CALI_AGC_FDD,
        LTE_CALI_APC_FDD,
        LTE_CALI_TX_DC,
        LTE_CALI_TX_DROOP,
        LTE_FINAL,
        LTE_ANTENNA,
        LTE_FINAL2,
        LTE_CALI_RX_IRR,
        WIFI_CALI_LTE,
        WIFI_CALI_WIFI,
        WIFI_CALI_NST,
        WIFI_CALI_ANT,
        WIFI_CALI_NST2,
        FLAG_INDEX_MAX
    }FLAG_Index;

    typedef struct 
    {
        MMI_FLAG_CHECK_E  type;
        PC_MMI_CIT_T nativeMMI;
    }MMI_TestItem;

    struct FlagInfo{
        FLAG_Index  flagIndex;
        FLAG_MODE_E flagMode;   // GSM/L
        LPCWSTR     NameW;
        LPCSTR      NameA;
        int         FlagMask;
        int         nBit;   // MMI标志位中指定地某位 [5/26/2017 jian.zhong]
    };

    typedef struct {
        char type_id; //----wholephonetest：0 /PCBA function test：1/BBAT Auto test；2
        char funtion_id; //   ----63种测试功能：0-63
        char support; //-----------是否支持,eng 初始化时读取cmdline 中的相应bitmask，来设置支持支持
        char status; //   --------测试结果，eng初始化时统一设置为00: no  test
    }TEST_NEW_RESULT_INFO;

    int m_Uetype;
protected:
    virtual SPRESULT __InitAction();
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
    SPRESULT LoadWifiUeInfo(void);

private:
    SPRESULT CheckMMI();
    SPRESULT CheckCaliFlag();
    SPRESULT CheckUID();
    int* GetFlag(FLAG_MODE_E flagmode);
private:
    BOOL  m_bCheckWholeMMI;
    BOOL  m_bCheckBBATMMI;
    BOOL  m_bCheckPCBAMMI;
    BOOL  m_bCheckUID;
    vector<MMI_TestItem> m_arrTestItem;
    static FlagInfo m_FlagInfo[FLAG_INDEX_MAX];
    int*  m_pCalFlag;
    int  m_nGSMFlag;
    int  m_nLTEflag;
    int  m_nWIFIflag;
    int m_wifi_UeInfo[20];
};
