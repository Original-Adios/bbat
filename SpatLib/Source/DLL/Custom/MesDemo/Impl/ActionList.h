
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

    ACTION(L"显式调用", L"MES_Connect",  L"CMES_Connect",    L"MES_Connect",    IDR_XML_MES_CONFIG,     TOWSTR(IDR_XML_MES_CONFIG),    L"MES Connect"),
    ACTION(L"显式调用", L"MES_Disconnect",  L"CMES_Disconnect",    L"MES_Disconnect",    0,     NULL,    L"MES Disconnect"),
    ACTION(L"显式调用", L"MES_CheckFlow",  L"CMES_CheckFlow",    L"MES_CheckFlow",    0,     NULL,    L"MES CheckFlow"),
    ACTION(L"显式调用", L"MES_SendTestResult",  L"CMES_SendTestResult",    L"MES_SendTestResult",    0,     NULL,    L"MES SendTestResult"),
    ACTION(L"显式调用", L"MES_UploadDataLog",  L"CMES_UploadDataLog",    L"MES_UploadDataLog",    0,     NULL,    L"MES UploadDataLog"),
    ACTION(L"隐式调用", L"ImplicitCallMesDll",  L"CImplicitCallMesDll",    L"ImplicitCallMesDll",    IDR_XML_MES_CONFIG,     TOWSTR(IDR_XML_MES_CONFIG),    L"Implicit Call MesD ll"),
    
#endif CMES_Connect