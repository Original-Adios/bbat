
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
    ACTION(L"L313", L"WriteCU",  L"CL313_WriteCU",    L"WriteCU",    IDR_XML_L313_WRITE_CU,     TOWSTR(IDR_XML_L313_WRITE_CU),    L"TCL写CU功能, CU可以从Unisoc MES Code字段获取"),
	ACTION(L"L313", L"CheckCU",  L"CL313_CheckCU",    L"CheckCU",    IDR_XML_L313_CHECK_CU,     TOWSTR(IDR_XML_L313_CHECK_CU),    L"TCL检查CU功能, CU可以从Unisoc MES Code字段获取"),
    
#endif 
