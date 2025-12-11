
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
    ACTION(L"BYD", L"MES_Start_New",  L"CMES_Start_New",  L"[BYD]MES_Start_New", 0,     NULL,    L"Start_New[对指定位置的SFC执行Start操作]"),
    ACTION(L"BYD", L"MES_Complete_New",  L"CMES_Complete_New",  L"[BYD]MES_Complete_New", 0,               NULL,     L"Complete_New[对指定位置的SFC执行Complete操作]"),
	ACTION(L"BYD", L"MES_NcComplete_New",  L"CMES_NcComplete_New",  L"[BYD]MES_NcComplete_New", 0,               NULL,     L"Complete_New[对指定位置的SFC执行NC_Complete操作]"),
	ACTION(L"BYD", L"MES_SfcKeyCollect_New",  L"CMES_SfcKeyCollect_New",  L"[BYD]MES_SfcKeyCollect_New", 0,               NULL,     L"SfcKeyCollect_New[关键物料收集(绑定)]"),
	ACTION(L"BYD", L"MES_GetNumberBySfc_New",  L"CMES_GetNumberBySfc_New",  L"[BYD]MES_GetNumberBySfc_New", 0,               NULL,     L"GetNumberBySfc_New[通过SFC分配号码]"),
#endif 
