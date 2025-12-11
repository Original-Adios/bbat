
#include "../resource.h"

#ifdef ACTION
/*     
            FnTag,      SubFnName,          ClassName,          ActionName,         rcID,               rcName,                     Description    
    ----------------------------------------------------------------------------------------------------------------------------------
    Example:
    ==
    1. Action with RC 
    ACTION(L"Dummy",    L"",                L"CDummyAction",    L"Dummy",           IDR_XML_DUMMY,      TOWSTR(IDR_XML_DUMMY),      L"Action with    RC"),
    
    2. Action without RC
    ACTION(L"Dummy",    L"",                L"CDummyAction",    L"Dummy",           0,                  NULL,                       L"Action without RC"),

 */
    ACTION(L"JIG",      L"StartJig",        L"CStartJig",       L"StartJig",        IDR_XML_STARTJIG,   TOWSTR(IDR_XML_STARTJIG),   L"Start JIG"),
    ACTION(L"JIG",      L"CloseJig",        L"CCloseJig",       L"CloseJig",        0,                  NULL,                       L"Close JIG"),
    ACTION(L"JIG",      L"CheckUsageCount", L"CCheckUsageCount",L"CheckUsageCount", IDR_XML_USAGECOUNT, TOWSTR(IDR_XML_USAGECOUNT), L"Check fogo pins and cable usage count"),
    ACTION(L"QRCode",   L"ScanQRcode",      L"CInputQRCode",    L"ScanQRcode",      IDR_XML_QRCODE,     TOWSTR(IDR_XML_QRCODE),     L"Scan QR code"),
    ACTION(L"Provision",L"WriteUniqueID",   L"CWriteUniqueID",  L"WriteUniqueID",   0,                  NULL,                       L"Write a unique ID(AT*IMEISET) to UE"),
    ACTION(L"Provision",L"WriteFID",        L"CWriteFID",       L"WriteFID",        0,                  NULL,                       L"Write SN(AT*FID) to UE"),
    ACTION(L"LGIT MES", L"MES Start",       L"CLgitMesStart",       L"LGIT MES Start",  IDR_XML_MES_START,  TOWSTR(IDR_XML_MES_START),  L"LGIT MES Start Work"),
    ACTION(L"LGIT MES", L"MES End",         L"CLgitMesEnd",         L"LGIT MES End",    0,                  NULL,                       L"LGIT MES Complete Work"),
    ACTION(L"UpdateCalFlag", L"UpdateCalFlag",       L"CUpdateCalFlag",   L"UpdateCalFlag",   IDR_XML_DUMMY, TOWSTR(IDR_XML_DUMMY),L"UpdateCalFlag AT*CALVERIFY=1/0"),
	ACTION(L"ReadUniqueID", L"ReadUniqueID",        L"CReadUniqueID",    L"ReadUniqueID",   IDR_XML_DUMMY, TOWSTR(IDR_XML_DUMMY), L"ReadUniqueID AT+SPIMEI?"),
#endif 
