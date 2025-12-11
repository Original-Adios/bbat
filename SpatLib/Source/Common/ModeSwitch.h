#pragma once
#include "SpatBase.h"
#include <string>
#include "ActionApp.h"
//
#define DIAG_MODE(x)    (0x80|(x))
//#define MAX_AT_CMDS     (5)

#ifdef __SUPPORT_DUAL_GSM__
    #define IS_SIGNAL_CELLULAR_MODE(x)       ( \
           RM_CALIBR_POST_MODE       == (x) \
        || RM_TD_CALIBR_POST_MODE    == (x) \
        || RM_WCDMA_CALIBR_POST_MODE == (x) \
        || RM_WCDMA_GSM_CALIBR_POST_MODE == (x) \
        || RM_LTE_CALIBR_POST_MODE   == (x) \
        )

    #define IS_CALIBRATION_CELLULAR_MODE(x) ( \
           RM_CALIBRATION_MODE       == (x) \
        || RM_TD_CALIBRATION_MODE    == (x) \
        || RM_WCDMA_CALIBRATION_MODE == (x) \
        || RM_WCDMA_GSM_CALIBR_CAL_MODE == (x) \
        || RM_LTE_CALIBRATION_MODE   == (x) \
	    || RM_DMR_CALIBRATION_MOD   == (x) \
	    || RM_DMR_SIG_TEST_MOD		== (x) \
        )
#else
    #define IS_SIGNAL_CELLULAR_MODE(x)       ( \
           RM_CALIBR_POST_MODE       == (x) \
        || RM_TD_CALIBR_POST_MODE    == (x) \
        || RM_WCDMA_CALIBR_POST_MODE == (x) \
		|| RM_C2K_CALIBR_POST_MODE	 == (x) \
        || RM_LTE_CALIBR_POST_MODE   == (x) \
        || RM_NR_CALIBR_POST_MODE   == (x) \
        )

    #define IS_CALIBRATION_CELLULAR_MODE(x) ( \
           RM_CALIBRATION_MODE       == (x) \
        || RM_TD_CALIBRATION_MODE    == (x) \
        || RM_WCDMA_CALIBRATION_MODE == (x) \
        || RM_LTE_CALIBRATION_MODE   == (x) \
        || RM_DMR_CALIBRATION_MOD   == (x) \
        || RM_DMR_SIG_TEST_MOD		== (x) \
		|| RM_C2K_CALIBRATION_MODE	== (x) \
        )
#endif

#define IS_VALID_MODE(x)        ((x) > RM_INVALID_MODE && (x) < RM_MAX_MODE)

//////////////////////////////////////////////////////////////////////////
class CModeOptions sealed
{
public:
    enum RESTART_OPTION
    {
        AUTO       = 0,
        COMMAND    = 1,
        CUSTOMIZED = 2,
        NPIDEVICE = 3
    };

    CModeOptions(void) 
      : m_eMode(RM_INVALID_MODE)
      , m_dwTimeOut(TIMEOUT_60S)
      , m_dwSleepTime(0)
      , m_bSearchFixedBCCH(TRUE)
      , m_eRestartOption(AUTO)
  //  , m_bLoadModemSwVer(FALSE)
      , m_fVBUSVoltage(5.0f)
      , m_bChargeOff(FALSE)
	  , m_bAuth(0)
      , m_eUsbSpeedType(USB_SPEED_NOTSET)
      , m_u32DelayTime(350)
    {
    };
    ~CModeOptions(void) { };

    CModeOptions(const CModeOptions& rhs)
    {
        _Copy(rhs);   
    }

    const CModeOptions& operator=(const CModeOptions& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }
        
        return _Copy(rhs);
    }

    RM_MODE_ENUM    m_eMode;
    DWORD           m_dwTimeOut;
    DWORD           m_dwSleepTime;
    BOOL            m_bSearchFixedBCCH;
    RESTART_OPTION  m_eRestartOption;
    /*
      在NonRF.dll --> AT 中单独把AT作为一个测试项目，故在此删除
      另外，Container内禁止使用STL容器
    std::string     m_arrATCmd[MAX_AT_CMDS];
    */
    // Bug925444: AP only product is no need to load modem version
 // BOOL            m_bLoadModemSwVer;      // Bug 1162044
    float           m_fVBUSVoltage;
    BOOL            m_bChargeOff;  // Bug1020677
	int             m_bAuth;
    uint32          m_u32DelayTime;
    USB_SPEED_TYPE_ENUM m_eUsbSpeedType;

private:
    const CModeOptions& _Copy(const CModeOptions& rhs)
    {
        m_eMode           = rhs.m_eMode;
        m_dwTimeOut       = rhs.m_dwTimeOut;
        m_dwSleepTime     = rhs.m_dwSleepTime;
        m_bSearchFixedBCCH= rhs.m_bSearchFixedBCCH;
        m_eRestartOption  = rhs.m_eRestartOption;
    /*
        for (INT i=0; i<MAX_AT_CMDS; i++)
        {
            m_arrATCmd[i] = rhs.m_arrATCmd[i];
        }
    */
  //    m_bLoadModemSwVer = rhs.m_bLoadModemSwVer;
        m_fVBUSVoltage = rhs.m_fVBUSVoltage;
        m_bChargeOff = rhs.m_bChargeOff;
		m_bAuth = rhs.m_bAuth;
        m_eUsbSpeedType = rhs.m_eUsbSpeedType;
        m_u32DelayTime = rhs.m_u32DelayTime;
        return *this;
    }
};

//////////////////////////////////////////////////////////////////////////
class CModeSwitch
{
public:
    CModeSwitch(CSpatBase* pImp/*Cannot be invalid or NULL*/);
    virtual ~CModeSwitch(void);
    void     Release(void) { delete this; };

    RM_MODE_ENUM GetCurrentMode(void);
    SPRESULT     SetCurrentMode(RM_MODE_ENUM eMode);

    void     SetupBSConfig(const BS_CONFIG_T& bs);
    BOOL	 GetBSConfig(BS_CONFIG_T& bs);
    void     SetupRunMode(RM_MODE_ENUM eMode);
    void     SetupOptions(const CModeOptions& op);

    /*
        RebootMode - Only used for the 1st time boot, usually we called 'Enter Mode'
        ChangeMode - It can be invoked during the test process.
                     For example:
                           RebootMode(Cal mode) --> Calibration --> ChangeMode(Post mode) --> Signal Final Test
                           RebootMode(Cal mode) --> GSM CAL --> ChangeMode(TD CAL) --> TD CAL

                     It supports to change mode by command or power supply depended on the XML configuration:
                     <RestartOption>...</RestartOption>

        @eMode     : Target mode
        @dwTimeOut : Timeout in ms.
        @pBS       : Configuration of base station of equipment, ONLY available while UE post mode.
                     If this parameter is NULL, CModeFunc will get BS_CONFIG_T from Container.
    */
    SPRESULT Reboot(RM_MODE_ENUM eMode, const BS_CONFIG_T* pBS = NULL, BOOL b1stBootUp = FALSE);
    SPRESULT Change(RM_MODE_ENUM eMode, const BS_CONFIG_T* pBS = NULL);

    /*
        Bug 876484, bAuto - TRUE: Auto mode, FALSE: power off by command
        Auto mode: if remote power supply, then power off by power supply, and otherwise by command
    */
    SPRESULT PowerOff(BOOL bAuto = TRUE);
    SPRESULT DeepSleep(DWORD delayTime = 0 /* Add default Vale */);
    SPRESULT RestartPhone(void);
    SPRESULT LDO_VDDWIFIPA(BOOL bOn);

    // Active & DeActive ARM & DSP Logel switch.
    SPRESULT ActiveLogels(void);
    SPRESULT DeActiveLogels(void);

private:
    //   SPRESULT TestCommunicating(void);
    SPRESULT SetCampParam(const BS_CONFIG_T& bs);

    SPRESULT __reboot(RM_MODE_ENUM eMode, const BS_CONFIG_T* pBS, BOOL b1stBootup);
    SPRESULT __switch(RM_MODE_ENUM eNextMode, RM_MODE_ENUM eCurrMode);

    SPRESULT SaveSwitchFlag(RM_MODE_ENUM eMode);

    BOOL GetOptions(CModeOptions& op);
    //BOOL GetBSConfig(BS_CONFIG_T& bs);
    BOOL GetRunMode(RM_MODE_ENUM& eMode);

    void __defaultBSConfig(RM_MODE_ENUM eMode, BS_CONFIG_T& bs);

    // 
    SPRESULT PreSetupSignal(RM_MODE_ENUM eMode, const BS_CONFIG_T* pBS);

private:
    CSpatBase*      m_pImp;
    SP_HANDLE       m_hDUT;
    IRFDevice*      m_pRFTester;
    IDCS*           m_pDCSource;
    CModeOptions    m_Options;
    INT             m_nArmlogTimeOut = 30000;
};