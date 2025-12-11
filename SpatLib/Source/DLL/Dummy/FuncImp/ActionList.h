
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
    ACTION(L"Dummy",       L"Dummy",  L"CDummyAction",    L"Demo",       IDR_XML_DUMMY,   TOWSTR(IDR_XML_DUMMY),  L"Dummy Action"),
    ACTION(L"GSM",         L"GSM FT",  L"CDummyGSM",    L"GSM FT",       0,  NULL,  L"GSM DUMMY"),





#endif 
