
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
    ACTION(L"AoiCheck", L"AoiCheck",  L"CAoiCheck",    L"AoiCheck",	0,      NULL,     L"Aoi Check"),
    ACTION(L"AddTextTestInfo", L"AddTextTestInfo",  L"CAddTextTestInfo",    L"AddTextTestInfo",	IDR_XML_ADDTESTINFO, TOWSTR(IDR_XML_ADDTESTINFO), L"Add Text Test Info"),
    ACTION(L"AddTextTestInfoCheck", L"AddTextTestInfoCheck",  L"CAddTextTestInfoCheck",    L"AddTextTestInfoCheck",	0,      NULL,     L"Add Text Test Info Check"),
    ACTION(L"GetChipInfo", L"GetChipInfo",  L"CGetChipInfo",    L"GetChipInfo",	0,      NULL,     L"Get Chip Info"),

#endif 
