#pragma once
#include "ImpBase.h"
#include "SpatBase.h"
#include "global_def.h"
using namespace std;
//////////////////////////////////////////////////////////////////////////
class CCheckCaliMMIFlag : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckCaliMMIFlag)
public:
    CCheckCaliMMIFlag(void);
    virtual ~CCheckCaliMMIFlag(void);
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
        FLAGMODE_WCDMA,
        FLAGMODE_LTE,
		FLAGMODE_CDMA,
    }FLAG_MODE_E;

    typedef enum
    {
        PCS_CALI_AGC = 0, 	
        PCS_CALI_APC, 	
        DCS_CALI_AGC, 	
        DCS_CALI_APC, 	
        EGSM_CALI_AGC, 	
        EGSM_CALI_APC, 	
        GSM850_CALI_AGC, 
        GSM850_CALI_APC, 
        GSM_CALI_AFC, 	
        MISC_ADC, 		
        PCS_FINAL, 	
        DCS_FINAL, 	
        EGSM_FINAL, 
        GSM850_FINAL,
        TD_CALI_AFC,		
        TD_CALI_APC,		
        TD_CALI_AGC,		
        TD_FINAL,	
        GSM_ANTENNA,
        EDGE_CALI, 	
        EDGE_FINAL, 
        MISC_MMI_CIT, 	
        W_CALI_BANDI, 
        W_CALI_BANDII,
        W_CALI_BANDV, 
        W_CALI_BANDVIII, 	
        W_CALI_BANDIV,
        W_FINAL_TEST, 
        LTE_CALI_AFC_TDD,
        LTE_CALI_AGC_TDD,
        LTE_CALI_APC_TDD,
        LTE_CALI_AFC_FDD,
        LTE_CALI_AGC_FDD,
        LTE_CALI_APC_FDD,
        LTE_FINAL_TDD, 	
        LTE_ANTENNA_TDD,	
        LTE_FINAL_FDD, 	
        LTE_ANTENNA_FDD,
		CDMA_CALI_BAND0,
		CDMA_FT_BAND0,
        FLAG_INDEX_MAX
    }FLAG_Index;

    typedef struct 
    {
        MMI_FLAG_CHECK_E  type;
        PC_MMI_CIT_T nativeMMI;
    }MMI_TestItem;

    struct FlagInfo{
        FLAG_Index  flagIndex;
        FLAG_MODE_E flagMode;   // GSM/W/L
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
    BOOL  m_bCheckAPKMMI;
    BOOL  m_bCheckUID;
    vector<MMI_TestItem> m_arrTestItem;
    static FlagInfo m_FlagInfo[FLAG_INDEX_MAX];
    int*  m_pCalFlag;
    int  m_nGSMFlag;
    int  m_nWCDMAflag;
    int  m_nLTEflag;
	int	 m_nCDMAflag;
};
