
#include "../resource.h"

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
    ACTION(L"InputCode",    L"WriteX",              L"CInputCodes",         L"InputCodes",          IDR_XML_INPUTCODES_WRITEX,      TOWSTR(IDR_XML_INPUTCODES_WRITEX),     L"InputCodes for WriteX"),
	ACTION(L"InputCode",    L"WriteXV2",            L"CInputCodesV2",		L"InputCodesV2",		IDR_XML_INPUTCODES_WRITEX2,		TOWSTR(IDR_XML_INPUTCODES_WRITEX2),	   L"InputCodes for WriteX2"),

	ACTION(L"InputCode",    L"CheckX",              L"CInputCodes",         L"InputCodes",          IDR_XML_INPUTCODES_CHECKX,      TOWSTR(IDR_XML_INPUTCODES_CHECKX),     L"InputCodes for CheckX"),
    ACTION(L"InputCode",    L"QRCode",              L"CInputQRCode",        L"ScanQRCode",          IDR_XML_INPUTQRCODE,            TOWSTR(IDR_XML_INPUTQRCODE),           L"Scan QR code"),
    ACTION(L"InputCode",    L"WCN",                 L"CWCNInputCodes",      L"InputCodes",          IDR_XML_INPUTCODES_WCN,         TOWSTR(IDR_XML_INPUTCODES_WCN),        L"InputCodes for WCN"),
    ACTION(L"InputCode",    L"CustomizeQRCode",     L"CCustomizeQRCode",    L"CustomizeQRCode",     IDR_XML_CUSTOMIZEQRCODE,        TOWSTR(IDR_XML_CUSTOMIZEQRCODE),       L"Scan Customize QR code"),

    ACTION(L"Provision",    L"WriteCodes",          L"CWriteCodes",         L"WriteCodes",          IDR_XML_WRITEX,                 TOWSTR(IDR_XML_WRITEX),                L"WriteCodes"),
    //ACTION(L"Provision",    L"WriteCodes(GEID)",    L"CWriteCodesEx",       L"WriteCodes(GEID)",    IDR_XML_WRITEX_EX,              TOWSTR(IDR_XML_WRITEX_EX),             L"WriteCodes IMEI and MEID"),
    ACTION(L"Provision",    L"CheckCodes",          L"CCheckCodes",         L"CheckCodes",          IDR_XML_CHECKCODES,            TOWSTR(IDR_XML_CHECKCODES1),           L"CheckCodes"),
	//ACTION(L"Provision",    L"CheckCodes(GEID)",    L"CCheckCodesEx",       L"CheckCodes(GEID)",    IDR_XML_CHECKCODES,            TOWSTR(IDR_XML_CHECKCODES1),           L"CheckCodes with GEID"),
    ACTION(L"Provision",    L"WriteSNFromRobot",    L"CWriteSNFromRobot",   L"WriteSNFromRobot",    IDR_XML_DUMMY,                  TOWSTR(IDR_XML_DUMMY),                 L"CheckCodes"),
	ACTION(L"Provision",    L"WriteSN_MGB_GSM",     L"CWriteSN_MGB_GSM",    L"WriteSN_MGB_GSM",     IDR_XML_DUMMY,                  TOWSTR(IDR_XML_DUMMY),                 L"WriteSN MGB GSM"),

	ACTION(L"Ethernet", L"Ethernet_InputCodes",       L"CInputCodes",   L"Ethernet_InputCodes",   IDR_XML_ETHERNET_INPUTCODES, TOWSTR(IDR_XML_ETHERNET_INPUTCODES),    L"Ethernet InputCodes"),
	ACTION(L"Ethernet", L"Ethernet_Write",       L"CEthernet_WriteCodes",   L"Ethernet_WriteCodes",   IDR_XML_ETHERNET_WRITECODES, TOWSTR(IDR_XML_ETHERNET_WRITECODES),    L"Ethernet Write Ethernet"),
	ACTION(L"Ethernet", L"Ethernet_Check",       L"CEthernet_CheckCodes",   L"Ethernet_CheckCodes",   0, NULL,    L"Ethernet Check Ethernet"),
	ACTION(L"WriteRtc", L"WriteRtc",       L"CWriteRtc",   L"WriteRtc",   0, NULL,    L"Write Rtc"),
    ACTION(L"HciBtCode", L"HciBtCode", L"CHciBtInputCode", L"InputBtAddr", IDR_XML_INPUTCODES_HCIBT, TOWSTR(IDR_XML_INPUTCODES_HCIBT), L"InputCodes for HciBt Address"),
#endif 
