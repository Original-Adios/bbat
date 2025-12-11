
#include "../resource.h"

#ifdef ACTION
/*     
            FnTag,                FnSubName,             ClassName,               ActionName,                 rcID,                     rcName,                         Description                
    -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Example:
    ==
    1. Action with RC 
    ACTION(L"Dummy",    L"",        L"CDummyAction",    L"Dummy",       IDR_XML_DUMMY,   TOWSTR(IDR_XML_DUMMY),  L"Action with    RC"),
    
    2. Action without RC
    ACTION(L"Dummy",    L"",        L"CDummyAction",    L"Dummy",       0,               NULL,                   L"Action without RC"),

 */
    ACTION(L"LoadInformation",  L"Software Version",   L"CReadSwVer",            L"LoadSwVer",              IDR_XML_SWVER,           TOWSTR(IDR_XML_SWVER),            L"Load software version [读取软件版本]"),
    ACTION(L"LoadInformation",  L"Hardware Version",   L"CReadHwVer",            L"LoadHwVer",              IDR_XML_HWVER,           TOWSTR(IDR_XML_HWVER),            L"Load hardware version [读取硬件版本]"),
    ACTION(L"LoadInformation",  L"Mcu Version",        L"CReadMcuVer",           L"LoadMcuVer",             IDR_XML_MCUVER,          TOWSTR(IDR_XML_MCUVER),           L"Load MCU version [读取MCU版本]"),
    ACTION(L"LoadInformation",  L"Serial No.",         L"CReadSN",               L"LoadSN",                 IDR_XML_READSN,          TOWSTR(IDR_XML_READSN),           L"Load Info [读取手机信息]"),
    ACTION(L"LoadInformation",  L"IMEI",               L"CReadIMEI",             L"LoadIMEI",               IDR_XML_READIMEI,        TOWSTR(IDR_XML_READIMEI),         L"Load IMEI"),
    ACTION(L"LoadInformation",  L"ICCID",              L"CLoadICCID",            L"LoadICCID",              IDR_XML_ICCID,           TOWSTR(IDR_XML_ICCID),            L"Load ICCID [读取ICCID]，需要插SIM卡且在BBAT模式"),
    ACTION(L"LoadInformation",  L"IMSI",               L"CLoadIMSI",             L"LoadIMSI",               0,                       NULL,                             L"Load IMSI，需要插SIM卡且在BBAT模式"),
    ACTION(L"LoadInformation",  L"EID",                L"CLoadEID",              L"LoadEID",                IDR_XML_EID,             TOWSTR(IDR_XML_EID),              L"Load EID，需要插SIM卡且在BBAT模式"),
    ACTION(L"LoadInformation",  L"Battery Capacity",   L"CCheckBatteryCapacity", L"LoadBatteryCapacity",    IDR_XML_BATTERYCAPACITY, TOWSTR(IDR_XML_BATTERYCAPACITY),  L"Check Battery Capacity"),
    ACTION(L"LoadInformation",  L"MAC Addr.",          L"CLoadMacAddr",          L"LoadMacAddress",         IDR_XML_MACADDR,         TOWSTR(IDR_XML_MACADDR),          L"Load MAC address"),
    ACTION(L"LoadInformation",  L"UID",                L"CCheckUID",             L"LoadUID",                IDR_XML_CheckUID,        TOWSTR(IDR_XML_CheckUID),         L"Load UID [检测 UID]"),
    ACTION(L"LoadInformation",  L"Widevine Device ID", L"CLoadWidevineDeviceID", L"LoadWidevineDeviceID",   IDR_XML_WIDEVINE_DEVICE_ID, TOWSTR(IDR_XML_WIDEVINE_DEVICE_ID),   L"Load Widevine Keybox Device ID"),

    ACTION(L"LoadInformation",  L"GoldenSample",       L"CCheckGoldenSample",    L"CheckGoldenSample",      0,                       NULL,                             L"Check GoldenSample [检查是否是金板]"),
    ACTION(L"LoadInformation",  L"GoldenSampleUIS8910", L"CCheckGoldenSampleUIS8910", L"CheckGoldenSampleUIS8910", IDR_XML_CHECK_GOLDEN_8910, TOWSTR(IDR_XML_CHECK_GOLDEN_8910), L"Check GoldenSample [检查是否是金板]"),
    ACTION(L"LoadInformation",  L"GoldenSampleUIS8850", L"CCheckGoldenSampleUIS8910", L"CheckGoldenSampleUIS8850", IDR_XML_CHECK_GOLDEN_8850, TOWSTR(IDR_XML_CHECK_GOLDEN_8850), L"Check GoldenSample [检查是否是金板]"),
    ACTION(L"LoadInformation",  L"SaveToFile",         L"CSaveUEInfo",           L"SaveToFile",             IDR_XML_SAVEINFO,        TOWSTR(IDR_XML_SAVEINFO),         L"将SN1,IMEI1,ICCID,IMSI,EID等信息保存到本地文件"),
	ACTION(L"LoadInformation",  L"PMICID",			   L"CLoadPMICID",           L"LoadPMICID",             IDR_XML_PMICID,			 TOWSTR(IDR_XML_PMICID),           L"Load PMIC ID"),
	ACTION(L"LoadInformation",  L"BBID",			   L"CLoadBBID",			 L"LoadBBID",               IDR_XML_BBID,			 TOWSTR(IDR_XML_BBID),             L"Load BB ID"),
	ACTION(L"LoadInformation",  L"WCNID",			   L"CLoadWCNID",			 L"LoadWCNID",              IDR_XML_WCNID,			 TOWSTR(IDR_XML_WCNID),            L"Load WCN ID"),
    
    ACTION(L"LoadInformation",  L"PAC Name",	       L"CCheckPacName",		 L"CheckPacName",           IDR_XML_CHK_PACNAME,	 TOWSTR(IDR_XML_CHK_PACNAME),      L"Check PAC name [检查PAC命名]"),
	ACTION(L"LoadInformation",  L"MIPI",			   L"CLoadMIPI",             L"LoadMIPI",				IDR_XML_DUMMY,			 TOWSTR(IDR_XML_DUMMY),			   L"Load MIPI Info,Under Wcdma mode[在WCDMA模式下读取MIPI信息]"),

    ACTION(L"ChangeMode",       L"EnterMode",          L"CEnterMode",            L"EnterMode",              IDR_XML_TESTMODE,        TOWSTR(IDR_XML_TESTMODE),         L"Control UE into specified test mode [测试模式]"),
    ACTION(L"ChangeMode",       L"ModeChange",         L"CModeChange",           L"ChangeMode",             IDR_XML_CHANGEMODE,        TOWSTR(IDR_XML_CHANGEMODE),     L"Change UE test mode [测试模式切换]"),
    ACTION(L"ChangeMode",       L"PowerOff",           L"CSwitchOff",            L"PowerOff",               IDR_XML_SWITCHOFF,       TOWSTR(IDR_XML_SWITCHOFF),        L"Switch off UE [关机]"),
    ACTION(L"ChangeMode",       L"RestartPhone",       L"CRestartPhone",         L"RestartPhone",           0,                       NULL,                             L"RestartPhone [重启]"),
    ACTION(L"ChangeMode",       L"NormalToCalMode",    L"CNormalModeToCalMode",  L"NormalToCalMode",        IDR_XML_NormalToCalMode, TOWSTR(IDR_XML_NormalToCalMode),  L"Normal mode to calibration mode [测试模式]"),
    ACTION(L"ChangeMode",       L"CalToNormalMode",    L"CNormalModeToCalMode",  L"CalToNormalMode",        IDR_XML_CalToNormalMode, TOWSTR(IDR_XML_CalToNormalMode),  L"calibration mode to Normal mode [测试模式]"),
    ACTION(L"ChangeMode",       L"QueryCurrMode",      L"CQueryCurrMode",        L"QueryCurrentMode",       0,                       NULL,                             L"Query current test mode [查询当前DUT的测试模式]"),
    ACTION(L"ChangeMode",       L"SetupRunMode",       L"CSetupNextMode",        L"SetupNextRunMode",       IDR_XML_RUNMODE,         TOWSTR(IDR_XML_RUNMODE),          L"Setup next test mode [设置DUT的启动模式]"),
    ACTION(L"ChangeMode",       L"CheckConnection",    L"CCheckDiagConnect",     L"CheckConnection",        IDR_XML_CHECKDIAGCONN,   TOWSTR(IDR_XML_CHECKDIAGCONN),    L"Check DIAG Connection [检查DIAG通路]"),
	ACTION(L"ChangeMode",       L"SetRunMode",         L"CSetMode",              L"SetRunMode",				IDR_XML_RUNMODE,         TOWSTR(IDR_XML_RUNMODE),          L"Setup current mode unconditionally [无条件设置当前DUT的测试模式]"),

    ACTION(L"Peripheral",       L"SIM Card",           L"CCheckSimCard",         L"CheckSIMCard",           IDR_XML_CHECKSIMCARD,    TOWSTR(IDR_XML_CHECKSIMCARD),     L"Check SimCard [检测Sim卡]"),
	ACTION(L"Peripheral",       L"T Card",             L"CCheckTCard",           L"CheckTCard",             IDR_XML_DUMMY,           TOWSTR(IDR_XML_DUMMY),            L"Check T Card [检测T卡]"),
	ACTION(L"Peripheral",       L"Non T Card",         L"CCheckNonTCard",           L"CheckNonTCard",       IDR_XML_DUMMY,           TOWSTR(IDR_XML_DUMMY),            L"Check Non T Card [检测无T卡]"),
    ACTION(L"Peripheral",       L"DDR+FLASH",          L"CCheckMemory",          L"CheckMemory",            IDR_XML_CHECKMEMORY,     TOWSTR(IDR_XML_CHECKMEMORY),      L"Check Memory [检测内存]"),
	ACTION(L"Peripheral",       L"Check DDR+FLASH Range",          L"CCheckMemoryRange",          L"CheckMemoryRange",            IDR_XML_CHECKMEMORYRANGE,     TOWSTR(IDR_XML_CHECKMEMORYRANGE),      L"Check Memory Range[检测内存范围]"),
    ACTION(L"Peripheral",       L"SIM CATO",           L"CCheckSimCato",         L"CheckSIMCato",           0,                       NULL,                             L"Check SIM Cato [检测SIM卡托]"),
    ACTION(L"Peripheral",       L"ChargeOff",          L"CChargeOff",            L"ChargeOff",              0,                       NULL,                             L"Charge Off [关闭充电]"),
    ACTION(L"Peripheral",       L"ChargeOn",           L"CChargeOn",             L"ChargeOn",               IDR_XML_CHARGER,         TOWSTR(IDR_XML_CHARGER),          L"Charge On  [开启充电]"),
    ACTION(L"Peripheral",       L"SwitchDC",           L"CSwitchDC",             L"SwitchDC",               0,                       NULL,                             L"复位程控电源输出"),
    ACTION(L"Peripheral",       L"BenchMark",          L"CCheckBenchMark",       L"BenchMark",              IDR_XML_BENCHMARK,       TOWSTR(IDR_XML_BENCHMARK),        L"Benchmark [存储跑分测试,如设置 T5D10 测试5秒，分数波动范围10%]"),
	ACTION(L"Peripheral",       L"AudioSaveRecorder",  L"CAudioSaveRecorder",    L"AudioSaveRecorder",      IDR_XML_AUDIOSAVERECORDER,TOWSTR(IDR_XML_AUDIOSAVERECORDER), L"Audio Save Recorder"),
	ACTION(L"Peripheral",       L"Check 32K Crystal",  L"CCheckCrystal32K",    L"Check 32K Crystal",      IDR_XML_CHKCRYSTAL32K,TOWSTR(IDR_XML_CHKCRYSTAL32K), L"Check 32k crystal by wcdma"),

    ACTION(L"AT",               L"AT",                 L"CAT",                   L"SendAT",                 IDR_XML_ATCMD,           TOWSTR(IDR_XML_ATCMD),            L"Send AT command [发送AT命令到手机]"),
	ACTION(L"AT",               L"Wait for AT",        L"CWaitForAT",            L"WaitForAT",              IDR_XMLWAITFORAT,        TOWSTR(IDR_XMLWAITFORAT),         L"Wait for AT response [等待AT返回]"),
    ACTION(L"AT",               L"SyncTime",           L"CSyncTime",             L"SyncTime",               IDR_XML_ATCMD,           TOWSTR(IDR_XML_ATCMD),            L"SyncTime [发送AT命令到手机同步时间]"),

    ACTION(L"PhaseCheck",       L"CheckStation",       L"CCheckStation",         L"CheckStation",           IDR_XML_PHASECHECK,      TOWSTR(IDR_XML_PHASECHECK),       L"Check  station [测试站位检查]"),
    ACTION(L"PhaseCheck",       L"CheckAllStation",    L"CCheckAllStation",      L"CheckAllStation",        IDR_XML_CHECKALLSTATION, TOWSTR(IDR_XML_CHECKALLSTATION),  L"Check all station [测试站位检查]"),
    ACTION(L"PhaseCheck",       L"UpdateStation",      L"CUpdateStation",        L"UpdateStation",          IDR_XML_UPDATEPHASE,     TOWSTR(IDR_XML_UPDATEPHASE),      L"Update station [测试站位更新]"),
    ACTION(L"PhaseCheck",       L"ResetStations",      L"CResetStations",        L"ResetStations",          IDR_XML_RESETSTATIONS,   TOWSTR(IDR_XML_RESETSTATIONS),    L"Reset stations [测试站位重置]"),
    ACTION(L"PhaseCheck",       L"InitPhase",          L"CInitPhase",            L"InitPhase",              0,                       NULL,                             L"Initialize PhaseCheck by PhaseCheck.ini [初始化PHASECHECK]"),
    ACTION(L"PhaseCheck",       L"CheckStationExist",  L"CCheckStationExist",    L"CheckStationExist",      IDR_XML_CHKSTATIONEXIST, TOWSTR(IDR_XML_CHKSTATIONEXIST),  L"Check station exist or onot [检查站位是否存在]"),

    ACTION(L"FactoryReset",     L"FactoryReset",       L"CFactoryReset",         L"FactoryReset",           IDR_XML_DUMMY,           TOWSTR(IDR_XML_DUMMY),            L"Factory reset [恢复出厂设置]"),

    ACTION(L"Debug",            L"AssertUE",           L"CAssertUE",             L"AssertUE",               0,                       NULL,                             L"Assert UE [使手机宕机]"),
    ACTION(L"Debug",            L"miscdata",           L"CCustomerMiscData",     L"CustomerDataExample",    IDR_XML_CUSTOMISCDATA,   TOWSTR(IDR_XML_CUSTOMISCDATA),    L"读写miscdata分区的客户区域例子"),

    ACTION(L"TestFlag",         L"CaliMMIflag",        L"CCheckCaliMMIFlag",     L"CheckCaliMMIFlag",       IDR_XML_ClaiMMIFlag,     TOWSTR(IDR_XML_ClaiMMIFlag),      L"Check Cali&MMI Flag [检测Cali & MMI Flag]"),
    ACTION(L"TestFlag",         L"CaliMMIflag(UIS8910)",  L"CCheckCaliMMIFlagUIS8910", L"CheckCaliMMIFlag",  IDR_XML_ClaiMMIFlagUIS8910,  TOWSTR(IDR_XML_ClaiMMIFlagUIS8910),      L"Check UIS8910 Cali&MMI Flag [检测 UIS8910 Cali & MMI Flag]"),
    ACTION(L"TestFlag",         L"CaliMMIflag(UIS1930)",  L"CCheckCaliMMIFlagUIS8910", L"CheckCaliMMIFlag",  IDR_XML_ClaiMMIFlagUIS1930,  TOWSTR(IDR_XML_ClaiMMIFlagUIS1930),      L"Check UIS1930 Cali&MMI Flag [检测 UIS1930 Cali & MMI Flag]"),
    ACTION(L"TestFlag",         L"CaliMMIflag(UIS8811)",  L"CCheckCaliMMIFlagUIS8811", L"CheckCaliMMIFlag",  IDR_XML_ClaiMMIFlagUIS8811,  TOWSTR(IDR_XML_ClaiMMIFlagUIS8811),      L"Check UIS8811 Cali&MMI Flag [检测 UIS8811 Cali & MMI Flag]"),
    ACTION(L"TestFlag",         L"CaliMMIflag(UIS8850)",  L"CCheckCaliMMIFlagUIS8910", L"CheckCaliMMIFlag",  IDR_XML_ClaiMMIFlagUIS8850,  TOWSTR(IDR_XML_ClaiMMIFlagUIS8850),      L"Check UIS8850 Cali&MMI Flag [检测 UIS8850 Cali & MMI Flag]"),
    ACTION(L"TestFlag",         L"CaliMMIflag(W217)",     L"CCheckCaliMMIFlagUIS8910", L"CheckCaliMMIFlag",  IDR_XML_ClaiMMIFlagUIS8850,  TOWSTR(IDR_XML_ClaiMMIFlagUIS8850),      L"Check W217 Cali&MMI Flag [检测 W217 Cali & MMI Flag]"),
    ACTION(L"TestFlag",         L"CaliMMIflag(UIS8910C2&C3)",  L"CCheckCaliMMIFlagUIS8910", L"CheckCaliMMIFlag",  IDR_XML_ClaiMMIFlagUIS891C,  TOWSTR(IDR_XML_ClaiMMIFlagUIS891C),      L"Check UIS8910C2_C3 Cali&MMI Flag [检测 UIS8910C2_C3 Cali & MMI Flag]"),
    ACTION(L"TestFlag",         L"CaliMMIflag(UIS8910FF-L)",  L"CCheckCaliMMIFlagUIS8910", L"CheckCaliMMIFlag",   IDR_XML_ClaiMMIFlagUIS8910FF_L,  TOWSTR(IDR_XML_ClaiMMIFlagUIS8910FF_L),      L"Check UIS8910FF-L Cali&MMI Flag [检测 UIS8910FF_L Cali & MMI Flag]"),
    ACTION(L"TestFlag",         L"AntennaFlag",        L"CSaveAntennaFlag",      L"SaveAntennaFlag",        0,                       NULL,                             L"Save Antenna flag"),
    ACTION(L"TestFlag",         L"CalFlagV2",          L"CCheckCalFlag",         L"CheckCalFlag(V2)",       IDR_XML_CALFLAG,         TOWSTR(IDR_XML_CALFLAG),          L"Check calibration flag for V2 [UDS710平台]"),
    ACTION(L"TestFlag",         L"CalFlag(DMR)",       L"CCheckCalFlag",         L"CheckCalFlag(DMR)",      IDR_XML_CALFLAG_DMR,     TOWSTR(IDR_XML_CALFLAG_DMR),      L"Check calibration flag for DMR"),
	ACTION(L"TestFlag",         L"CalFlagV3",          L"CCheckCaliFlagV3",      L"CheckCalFlag(V3)",       IDR_XML_CALI_V3,         TOWSTR(IDR_XML_CALI_V3),		   L"Check calibration flag for V3 [其他平台]"),
    ACTION(L"TestFlag",         L"CalFlagV5",          L"CCheckCaliFlagV5",      L"CheckCalFlag(V5)",       IDR_XML_CALI_V3,         TOWSTR(IDR_XML_CALI_V3),          L"Check calibration flag for V5 [其他平台]"),
    ACTION(L"TestFlag",         L"ClearAllCaliCrc",    L"CClearAllCaliCrc",      L"ClearAllCaliCrc",        0,                       NULL,                             L"ClearAllCaliCrc"),

    ACTION(L"NV",               L"LoadNV",             L"CReadNVID",             L"LoadNV",                 IDR_XML_READ_NVID,       TOWSTR(IDR_XML_READ_NVID),        L"Read NV id"),
  
    ACTION(L"PORT",             L"OpenUart",           L"COpenPort",             L"OpenUart",               IDR_XML_OPEN_PORT,       TOWSTR(IDR_XML_OPEN_PORT),        L"Open UART"),
    ACTION(L"PORT",             L"WaitOpenUart",       L"CWaitOpenPort",         L"WaitOpenUart",           IDR_XML_OPEN_PORT,       TOWSTR(IDR_XML_OPEN_PORT),        L"Wait Open UART[等待端口，打开端口进入模式]"),
	ACTION(L"PORT",				L"OpenUartByParity",   L"COpenPortByParity",	 L"OpenUartByParity",		IDR_XML_OPEN_PORT_PARITY,TOWSTR(IDR_XML_OPEN_PORT_PARITY), L"Open UART By Parity"),
    ACTION(L"PORT",             L"OpenSocket",         L"COpenSocket",           L"OpenSocket",             IDR_XML_OPEN_SOCKET,     TOWSTR(IDR_XML_OPEN_SOCKET),      L"Open Socket"),
    ACTION(L"PORT",             L"ClosePort",          L"CClosePort",            L"ClosePort",              0,                       NULL,                             L"Close Port"),
	ACTION(L"PORT",             L"OpenDut",            L"COpenDut",              L"OpenDut",                IDR_XML_OPEN_DUT,        TOWSTR(IDR_XML_OPEN_DUT),         L"Open DUT"),
    
    ACTION(L"SaveToFlash",      L"SaveToFlash",        L"CSaveToFlash",          L"SaveToFlash",            IDR_XML_SAVETOFLASH,     TOWSTR(IDR_XML_SAVETOFLASH),      L"Save NV parameters into flash [将参数写入FLASH]"),
    ACTION(L"TimeSleep",        L"TimeSleep",          L"CSleep",                L"TimeSleep",              IDR_XML_SLEEP,           TOWSTR(IDR_XML_SLEEP),            L"TimeSleep"),
    
    ACTION(L"Misc",             L"CustomControl",      L"CCustomControl",        L"Custom Control",         IDR_XML_CUSTOMCONTROL,   TOWSTR(IDR_XML_CUSTOMCONTROL),    L"Custom control"),
    ACTION(L"Misc",             L"SSID",               L"CSetSSID",              L"SetSSID",                IDR_XML_SSID,            TOWSTR(IDR_XML_SSID),             L"Setup AP SSID"),
    ACTION(L"Misc",             L"SetupConnMode",      L"CSetupConnMode",        L"SetupConnMode",          IDR_XML_SETUPCONNMODE,   TOWSTR(IDR_XML_SETUPCONNMODE),    L"Setup Connect Mode[设置传输模式]"),
    ACTION(L"Misc",             L"Finalize",           L"CFinalize",             L"Finalize",               IDR_XML_FINALIZE,        TOWSTR(IDR_XML_FINALIZE),         L"If pass, factory reset, otherwise power off [测试成功恢复出厂设置，否则关机]"),

    ACTION(L"SeqLock",          L"LockSeq",            L"CLockSeq",              L"LockSeq",                0,                       NULL,                             L"It must match to use with \"UnlockSeq\", and \"RunMode\" must set \"ALWAYS\""),
    ACTION(L"SeqLock",          L"UnlockSeq",          L"CUnlockSeq",            L"UnlockSeq",              0,                       NULL,                             L"It must match to use with \"LockSeq\", and \"RunMode\" must set \"ALWAYS\""),
    ACTION(L"ArmLog",           L"ActiveArmLog",       L"CActiveArmLog",         L"ActiveArmLog",           0,                       NULL,                             L"Active ArmLog Dump."),
    ACTION(L"ArmLog",           L"DeActiveArmLog",     L"CDeActiveArmLog",       L"DeActiveArmLog",         0,                       NULL,                             L"DeActive ArmLog Dump."),
    ACTION(L"ModemVersion",     L"ModemVersion",       L"CReadModemVersion",     L"ModemVersion",           IDR_XML_READMODEMVERSION,TOWSTR(IDR_XML_READMODEMVERSION), L"Read ModemVersion."),

    ACTION(L"Check32kCrystalFreqOffset",         L"Check32kCrystalFreqOffset",           L"CCheck32kCrystalFreqOffset",             L"Check32kCrystalFreqOffset",   IDR_XML_CHECK32kCrystalFreqOffset,        TOWSTR(IDR_XML_CHECK32kCrystalFreqOffset),         L"Check32kCrystalFreqOffset")
#endif 
