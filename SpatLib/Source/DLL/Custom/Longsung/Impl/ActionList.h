
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
    ACTION(L"MES", L"ConnectSL",     L"CConnect",     L"ConnectSL",    IDR_XML_CONNECT, TOWSTR(IDR_XML_CONNECT), L"Connect Server"),
    ACTION(L"MES", L"DisconnectSL",  L"CDisConnect",  L"DisconnectSL", 0,               NULL,                    L"Disconnect Server"),
    ACTION(L"MES", L"Upload",        L"CUpload",      L"Upload",       0,               NULL,                    L"Upload"),

#endif 
