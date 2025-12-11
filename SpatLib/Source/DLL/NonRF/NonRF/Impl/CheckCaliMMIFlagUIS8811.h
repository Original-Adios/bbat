#pragma once
#include "ImpBase.h"
#include "SpatBase.h"
#include "global_def.h"
using namespace std;
//////////////////////////////////////////////////////////////////////////
class CCheckCaliMMIFlagUIS8811 : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckCaliMMIFlagUIS8811)
public:
    CCheckCaliMMIFlagUIS8811(void);
    virtual ~CCheckCaliMMIFlagUIS8811(void);
    typedef enum
    {
        WHOLE_FLAG,
        PCBA_FLAG,
        BBAT_FLAG,
        APK_FLAG
    }MMI_FLAG_CHECK_E;

    typedef enum
    {
        FLAGMODE_NBIOT,
    }FLAG_MODE_E;

    typedef enum
    {
        NBIOT_CALIB_AFC = 0,
        NBIOT_CALIB_AGC,
        NBIOT_CALIB_APC,
        NBIOT_FT,
        NBIOT_CALIB_RXFAT,
        NBIOT_CALIB_TXFAT, 	
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

protected:
    virtual SPRESULT __InitAction();
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

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
    int  m_nNBIOTflag;
};
