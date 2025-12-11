
#include "../resource.h"

#ifdef ACTION
/*     
            FnTag,            Version,       ClassName,          ActionName,        rcID,            rcName,                Description     
    ----------------------------------------------------------------------------------------------------------------------------------------
    Example:
    ==
    1. Action with RC 
    ACTION(L"Dummy",       MAKEWORD(0, 1),  L"CDummyAction",    L"Dummy",      IDR_XML_DUMMY,   TOWSTR(IDR_XML_DUMMY),  L"Action with    RC"),
    
    2. Action without RC
    ACTION(L"Dummy",       MAKEWORD(0, 1),  L"CDummyAction",    L"Dummy",      0,               NULL,                   L"Action without RC"),

 */
    ACTION(L"XUNRUI", L"Camera",    L"CCheckCamera", L"CheckCamera", IDR_XML_CAMERA,     TOWSTR(IDR_XML_CAMERA),        L"Check Camera"),
    ACTION(L"XUNRUI", L"TP",        L"CCheckTP",     L"CheckTP",     IDR_XML_CHECK_TP,   TOWSTR(IDR_XML_CHECK_TP),      L"Check TP"),
    ACTION(L"XUNRUI", L"LCD",       L"CCheckLCD",    L"CheckLCD",    IDR_XML_CHECK_LCD,  TOWSTR(IDR_XML_CHECK_LCD),     L"Check LCD"),
    ACTION(L"XUNRUI", L"NFC",       L"CCheckCPLC",   L"CheckCPLC",   IDR_XML_CHECK_CPLC, TOWSTR(IDR_XML_CHECK_CPLC),    L"Check NFC CPLC"),
	ACTION(L"Common", L"Camera",    L"CCheckCommonCamera", L"CheckComonCamera", IDR_XML_COMMONCAMERA,     TOWSTR(IDR_XML_COMMONCAMERA),        L"Check Common Camera"),
    ACTION(L"ZC",  L"CompareVER",			  L"CLoadZCPropVersion",  L"LoadZCPropVersion",   IDR_XML_CHECK_ZCVER, TOWSTR(IDR_XML_CHECK_ZCVER),    L"LoadZCPropVersion"),
#endif 
