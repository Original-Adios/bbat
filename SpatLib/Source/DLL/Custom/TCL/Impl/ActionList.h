
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
    ACTION(L"JIG",     L"OpenJig",        L"COpenJig",       L"OpenJig",        IDR_XML_JIG,   TOWSTR(IDR_XML_JIG),   L"Open JIG"),
    
    ACTION(L"CU",      L"WriteCU",        L"CWriteCU",       L"WriteCU",        IDR_XML_WRITECU,   TOWSTR(IDR_XML_WRITECU),   L"Write CU"),
    ACTION(L"CU",      L"CheckCU",        L"CCheckCU",       L"CheckCU",        IDR_XML_CHECKCU,   TOWSTR(IDR_XML_CHECKCU),   L"Load and check CU"),

#endif 
