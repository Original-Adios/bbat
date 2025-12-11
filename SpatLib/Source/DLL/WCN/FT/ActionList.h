
#include "resource.h"

#ifdef ACTION
/*
			FnTag,      SubFnName,  ClassName,          ActionName,     rcID,            rcName,                Description
	----------------------------------------------------------------------------------------------------------------------------------
	Example:
	==
	1. Action with RC
	ACTION(L"Dummy",    L"",        L"CDummyAction",    L"Dummy",       IDR_XML_DUMMY,   TOWSTR(IDR_XML_DUMMY),  L"Action with    RC"),

	2. Action without RC
	ACTION(L"Dummy",    L"",        L"CDummyAction",    L"Dummy",       0,               NULL,                   L"Action without RC"),

 */
	ACTION(L"SC265X(Marlin3)",		L"BT FT",		L"CBTTestSysV2",		L"BT FT",			IDR_XML_BT,					TOWSTR(IDR_XML_BT),				L"BlueTooth performance measurement, support BT BDR,EDR,BLE and BLE 5.0. Marlin3 project please use this action"),
	ACTION(L"SC265X(Marlin3)",	    L"Wlan FT",		L"CWlanTestSysOS80",	L"Wlan FT",			IDR_XML_WCN_OS_80,			TOWSTR(IDR_XML_WCN_OS_80),		L"Wlan performance Test, support SPRD 11ac proto measurement"),
	ACTION(L"SC2342(Marlin2)",		L"BT FT",		L"CBTMeasSysV1",		L"BT FT",			IDR_XML_BT_GENERAL,			TOWSTR(IDR_XML_BT_GENERAL),		L"BlueTooth performance measurement, supoort BT BDR,EDR and BLE. support all projects except Marlin3"),
	ACTION(L"SC2342(Marlin2)",      L"Wlan FT",		L"CWlanTestSys",		L"Wlan FT",			IDR_XML_WCN,				TOWSTR(IDR_XML_WCN),			L"Wlan performance measurement"),
	//ACTION(L"Wear Device",			L"Wcn FT(Wear)",L"CWcnTestSysWear",		L"Wcn FT(Wear)",	IDR_XML_WCN_WEAR,			TOWSTR(IDR_XML_WCN_WEAR),		L"Wlan and BlueTooth performance measurement for wear devices(by script)"),
	ACTION(L"Wear Device",			L"Wcn FT(Marlin2)",L"CWcnTestSysWearAT",	L"Wcn FT(WearAT)",	IDR_XML_WCN_WEAR_AT,		TOWSTR(IDR_XML_WCN_WEAR_AT),	L"Wlan and BlueTooth performance measurement for wear devices (by AT commands)"),
	ACTION(L"Wear Device",			L"Wcn FT(Marlin3)", L"CWcnTestSysWearATEx", L"Wcn FT Ex(WearAT)", IDR_XML_WCN_WEAR_AT_EX, TOWSTR(IDR_XML_WCN_WEAR_AT_EX), L"Add 5G Band,Wlan and BlueTooth performance measurement for wear devices (by AT commands)"),
	ACTION(L"Wear Device",			L"Wcn FT(CW)",  L"CWcnTestSysWearCW",   L"Wcn FT CW",		IDR_XML_WCN_WEAR_CW,		TOWSTR(IDR_XML_WCN_WEAR_CW),    L"Wlan and BlueTooth performance CW measurement for wear devices (Only TX)"),
	ACTION(L"SC265X(Marlin3)",		L"Wlan FT(CW)",	L"CWlanTestSysCW",		L"Wlan FT(CW)",		IDR_XML_WCN_CW,				TOWSTR(IDR_XML_WCN_CW),			L"Wlan performance Test, support CW measurement"),
	ACTION(L"SC265X(Marlin3)",		L"BT FT(CW)",   L"CBTTestSysCW",        L"BT FT(CW)",		IDR_XML_BT_CW,				TOWSTR(IDR_XML_BT_CW),			L"BT performance Test, support CW measurement"),
	ACTION(L"General",				L"GPS FT",		L"CGpsTestSys",			L"GPS FT",			IDR_XML_GPSFM,				TOWSTR(IDR_XML_GPSFM),			L"GPS performance Test"),
	ACTION(L"General",				L"GPS FT V2",	L"CGpsTestSysV2",		L"GPS FT V2",		IDR_XML_GPSFM_V2,			TOWSTR(IDR_XML_GPSFM_V2),		L"GPS performance Test, Optional Band"),
	ACTION(L"General",				L"GPS FT V3",	L"CGpsTestSysV3",		L"GPS FT V3",		IDR_XML_GPSFM_V3,			TOWSTR(IDR_XML_GPSFM_V3),		L"GPS performance Test, Optional Band, Set CW Freq ID"),
	ACTION(L"General",				L"GPS FT BeiDou", L"CGpsTestSysBeiDou", L"GPS FT BeiDou",	IDR_XML_GPSFM_BeiDou,		TOWSTR(IDR_XML_GPSFM_BeiDou),   L"GPS performance Test, Optional BeiDou"),
	ACTION(L"General",				L"GPS FT Glonass",  L"CGpsTestSysGlonass",		L"GPS FT Glonass",	    IDR_XML_GPSFM_GLO,			TOWSTR(IDR_XML_GPSFM_GLO),		L"GPS performance Test, Optional Glonass"),
	ACTION(L"General",				L"GPS FT MSLT", L"CGpsTestSysMslt",		L"GPS FT MSLT",		IDR_XML_GPSFM_MSLT,			TOWSTR(IDR_XML_GPSFM_MSLT),		L"GPS performance Test, MSLT mode, Optional Band"),
	ACTION(L"General",				L"GPS FT Auto", L"CGpsAutoModeTestSys", L"GPS FT Auto",		IDR_XML_GPSFM_AUTO,			TOWSTR(IDR_XML_GPSFM_AUTO),		L"GPS performance Test, Auto Optional Band"),
	ACTION(L"Development",          L"LogEnable",	L"CLogEnable",			L"LogEnable",		IDR_XML_LOG,				TOWSTR(IDR_XML_LOG),			L"Enable arm or cp2 log"),
	ACTION(L"Development",          L"Wlan PerProbe",	L"CWlanBerProbe",   L"Wlan PerSearch",	IDR_XML_WCN_OS_80_PROBE,    TOWSTR(IDR_XML_WCN_OS_80_PROBE),L"Wlan per probe meas, support SPRD chip marlin3 proto measurement"),
	ACTION(L"Development",	        L"Wlan FT",		L"CWlanTestSysMimo",	L"Wlan FT(Mimo)",	IDR_XML_WCN_OS_80_MIMO,		TOWSTR(IDR_XML_WCN_OS_80_MIMO), L"Wlan performance Test, support SPRD 11ac proto measurement which is only avaible for mimo ant"),
	ACTION(L"Development",          L"BT PerProbe",	L"CBTTestSysProbe",	L"BT PerSearch",	IDR_XML_BT_PROBE,			TOWSTR(IDR_XML_BT_PROBE),		L"BT per probe meas, support SPRD chip marlin3 proto measurement"),
	ACTION(L"Development",			L"Wlan FT",		L"CWlanTestSysOS80",	L"Wlan FT",			IDR_XML_WCN_DEV,			TOWSTR(IDR_XML_WCN_DEV),		L"Wlan performance Test, support SPRD 11ac proto measurement"),
	ACTION(L"WCN Loss",				L"GPS Loss FT",		L"CGPSTestLoss",			L"GPSLoss FT",			IDR_XML_WCN_FT_GPS_LOSS,				TOWSTR(IDR_XML_WCN_FT_GPS_LOSS),			L"GPSLoss performance Test"),
	ACTION(L"WCN Loss",				L"Wlan Loss FT",		L"CWlanTestSysAntLoss",		L"Wlan Loss FT",	IDR_XML_WCN_LOSS,				TOWSTR(IDR_XML_WCN_LOSS),			L"Wlan Loss performance measurement"),
	ACTION(L"WCN Loss",				L"Wlan Loss FT_V2", L"CWlanTestSysAntLossV2", L"Wlan Loss FT_V2", IDR_XML_WCN_LOSS_V2, TOWSTR(IDR_XML_WCN_LOSS_V2), L"Wlan Loss V2 performance measurement"),
	ACTION(L"WCN Loss",				L"Wlan Loss FT_SC266X", L"CWlanTestSysAntLossSC266X", L"Wlan Loss FT_SC266X", IDR_XML_WCN_LOSS_W6, TOWSTR(IDR_XML_WCN_LOSS_W6), L"Wlan Loss SC266X(SongShanW6) performance measurement"),
	ACTION(L"WCN Loss",				L"BT Loss FT", L"CBTMeasSysV1Loss", L"BT Loss FT", IDR_XML_BT_GENERAL_Loss, TOWSTR(IDR_XML_BT_GENERAL_Loss), L"bt Loss performance measurement"),
	ACTION(L"UDS710 WIFI6&BT5.1",   L"Wlan FT UDS710", L"CWlanTestSysUDS710", L"Wlan FT UDS710", IDR_XML_WCN_UDS710, TOWSTR(IDR_XML_WCN_UDS710), L"Wlan performance Test, support UDS710"),
	ACTION(L"UDS710 WIFI6&BT5.1",   L"BT FT UDS710", L"CBTTestSysUDS710", L"BT FT UDS710", IDR_XML_BT_UDS710, TOWSTR(IDR_XML_BT_UDS710), L"BT performance Test, support UDS710"),
	ACTION(L"SC266X(SongShanW6)",	L"Wlan6 FT", L"CWlanTestSysSongShanW6", L"Wlan6 FT", IDR_XML_WCN_SONGSHAN_W6, TOWSTR(IDR_XML_WCN_SONGSHAN_W6), L"Wlan6 performance Test, support SPRD 11b,11g,11n,11a,11ac,11ax  proto measurement"),
	ACTION(L"SC266X(SongShanW6)", 	L"Wlan6DL FT", L"CWlanTestSysSongShanW6DL", L"Wlan6DL FT", IDR_XML_WCN_SONGSHAN_W6, TOWSTR(IDR_XML_WCN_SONGSHAN_W6), L"Wlan6DL performance Test, support SPRD 11b,11g,11n,11a,11ac,11ax  proto measurement"),
	ACTION(L"SC266X(SongShanW6)",		L"BT FT", L"CBTTestSysV2", L"BT FT", IDR_XML_BT_V2_EX, TOWSTR(IDR_XML_BT_V2_EX), L"BlueTooth performance measurement, support BT BDR,EDR,BLE and BLE 5.0. Marlin3 project please use this action"),
	//ACTION(L"SongShan", L"BT5.3 FT", L"CBTTestSysSongShan", L"BT5.3 FT", IDR_XML_BT_BLE53, TOWSTR(IDR_XML_BT_BLE53), L"BlueTooth performance measurement, support BT BDR,EDR,BLE,BLE 5.0,BLE 5.3. SongShan project please use this action"),
	ACTION(L"HCI",					L"HCI BT FT",	L"CBTTestSysHCI",	L"HCI BT FT", IDR_XML_HCI_BT, TOWSTR(IDR_XML_HCI_BT), L"HCI BT Command performance Test"),
	//ACTION(L"HCI",					L"Write Bt Addr",  L"CHciBtWriteAddr", L"Write Bt Addr", IDR_XML_HCI_BT_WADDR, TOWSTR(IDR_XML_HCI_BT_WADDR), L"HCI Write BT Address"),
	ACTION(L"HCI",					L"Check Bt Addr", L"CHciBtCheckAddr", L"Check Bt Addr", IDR_XML_HCI_BT_RADDR, TOWSTR(IDR_XML_HCI_BT_RADDR), L"HCI Check BT Address"),
	ACTION(L"MSLT",					L"Check Wcn Efuse Uid", L"CCheckWcnUid", L"Check Wcn Efuse Uid", IDR_XML_WCN_UID, TOWSTR(IDR_XML_WCN_UID), L"Check Wcn Efuse Uid"),
	ACTION(L"MSLT",					L"Bt Tx Impact Wifi Rx Per", L"CWlanTestSysByBtTxOn", L"Bt Tx Impact Wifi Rx Per", IDR_XML_BT_INF_WCN, TOWSTR(IDR_XML_BT_INF_WCN), L"Check Bt Tx Impact Wifi Rx Per"),
#endif
