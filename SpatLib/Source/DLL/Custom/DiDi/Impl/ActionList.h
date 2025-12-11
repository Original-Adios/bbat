
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
    ACTION(L"BarCode",      L"InputBarCodes",   L"CInputBarCodes",  L"InputBarCodes",   IDR_XML_BARCODES,   TOWSTR(IDR_XML_BARCODES),       L"Input/Scan BarCodes"),
    ACTION(L"BarCode",      L"WriteCodes",      L"CWriteCodes",     L"WriteCodes",      IDR_XML_WRITECODES, TOWSTR(IDR_XML_WRITECODES),     L"Write Codes"),
    ACTION(L"TriAdData",    L"WriteTriAdData",  L"CWriteTriAdData", L"WriteTriAdData",  IDR_XML_TRIAD,      TOWSTR(IDR_XML_TRIAD),          L"Write Tri-Ad Data"),
#endif 
