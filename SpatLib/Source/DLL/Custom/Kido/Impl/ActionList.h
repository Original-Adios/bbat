
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
    ACTION(L"KidoMes", L"fnMesStart",  L"CfnMesStart",    L"fnMesStart",    IDR_XML_MESCONFIG,     TOWSTR(IDR_XML_MESCONFIG),    L"fnMesStart[过站检查]"),
    ACTION(L"KidoMes", L"fnMesComplete",  L"CfnMesComplete",    L"fnMesComplete",    0,     NULL,    L"fnMesComplete[上传测试结果]"),
	ACTION(L"Kido", L"CheckStationFrMisc",  L"CCheckStationFrMisc",  L"CheckStationFrMisc", IDR_XML_CHECK_STATION,     TOWSTR(IDR_XML_CHECK_STATION),    L"Check Station From MiscData"),
	ACTION(L"Kido", L"UpdateStationToMisc",  L"CUpdateStationToMisc",  L"UpdateStationToMisc", IDR_XML_UPDATE_STATION,     TOWSTR(IDR_XML_UPDATE_STATION),    L"Update Station To MiscData"),
#endif 
