
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
    ACTION(L"TinnoMes", L"MES_Connect",  L"CMES_Connect",  L"[Tinno]MES_Connect", IDR_XML_MES_CONNECT,     TOWSTR(IDR_XML_MES_CONNECT),    L"MES_Connect[MES连接]"),
    ACTION(L"TinnoMes", L"MES_Disconnect",  L"CMES_Disconnect",  L"[Tinno]MES_Disconnect", 0,               NULL,     L"MES_Disconnect[MES断开连接]"),
	ACTION(L"TinnoMes", L"MES_GetinfoSNorIMEI",  L"CMES_GetinfoSN",  L"[Tinno]MES_GetinfoSNorIMEI", 0,               NULL,     L"MES_GetinfoSNorIMEI[MES检查过站]"),
	ACTION(L"TinnoMes", L"MES_SaveTestRecord",  L"CMES_SaveTestRecord",  L"[Tinno]MES_SaveTestRecord", 0,               NULL,     L"MES_SaveTestRecord[MES上传测试结果]"),
	ACTION(L"TinnoMes", L"MES_Upload",  L"CMES_Upload",  L"[Tinno]MES_Upload", 0,               NULL,     L"MES_Upload[MES上传txt Log]"),
#endif 
