
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
    ACTION(L"InputCustBarCodes",    L"InputCustBarCodes",              L"CInputCustBarCodes",         L"InputCustBarCodes",          IDR_XML_INPUTCODES1BY1,      TOWSTR(IDR_XML_INPUTCODES1BY1),     L"Input Cust BarCodes One By One"),
    ACTION(L"LoadCodesFromMdb",     L"LoadCodesFromMdb",               L"CLoadCodesFromMdb",          L"LoadCodesFromMdb",          IDR_XML_LOADCODESFRMDB,      TOWSTR(IDR_XML_LOADCODESFRMDB),      L"Load Codes From Mdb File"),
	ACTION(L"Timo", L"WriteSSN",  L"CWriteSSN",  L"WriteSSN", IDR_XML_TIMOENCRYPTMODE,     TOWSTR(IDR_XML_TIMOENCRYPTMODE),    L"Sogou Write SSN"),
	ACTION(L"Timo", L"WriteGuid",  L"CWriteGuid",  L"WriteGuid", 0,     NULL,    L"Sogou Write Guid"),
	ACTION(L"NetCodes", L"WriteNetCodes",  L"CWriteNetCodes",  L"WriteNetCodes", 0,     NULL,    L"Write Net Codes"),

    ACTION(L"WW", L"LoadCodesFromBin",  L"CLoadCodesFromBin",   L"LoadCodesFromBin",   IDR_XML_CODEFRBIN,   TOWSTR(IDR_XML_CODEFRBIN),    L"Load Codes From Bin"),
    ACTION(L"WW", L"WriteCu",  L"CWriteCu",   L"WriteCu",   IDR_XML_CUCONFIG,   TOWSTR(IDR_XML_CUCONFIG),    L"Write Cu"),

	
#endif 
