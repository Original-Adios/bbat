
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
    ACTION(L"InputCode",    L"WriteX",              L"CInputCodes",         L"InputCodes",          IDR_XML_INPUTCODES_WRITEX,      TOWSTR(IDR_XML_INPUTCODES_WRITEX),     L"InputCodes for WriteX"),
    ACTION(L"InputCode",    L"CheckX",              L"CInputCodes",         L"InputCodes",          IDR_XML_INPUTCODES_CHECKX,      TOWSTR(IDR_XML_INPUTCODES_CHECKX),     L"InputCodes for CheckX"),


    ACTION(L"Provision",    L"WriteCodes",          L"CWriteCodes",         L"WriteCodes",          IDR_XML_WRITEX,                 TOWSTR(IDR_XML_WRITEX),                L"WriteCodes"),
    ACTION(L"Provision",    L"CheckCodes",          L"CCheckCodes",         L"CheckCodes",          IDR_XML_CHECKCODES1,            TOWSTR(IDR_XML_CHECKCODES1),           L"CheckCodes"),
    ACTION(L"Provision",    L"CheckNetCode",        L"CCheckNetCode",       L"CheckNetCode",        0,                              NULL, L"CheckNetCode"),
    ACTION(L"Provision",    L"ManualInputCompare",  L"CCheckManualCode",    L"ManualInputCompare",  0,                              NULL,                                   L"Compare scan and manual codes"),

	ACTION(L"APPID", L"WriteAppid",  L"CWriteAppid",  L"WriteAppid",IDR_XML_APPID,     TOWSTR(IDR_XML_APPID),    L"WriteAppid"),
	ACTION(L"APPID", L"CheckAppid",  L"CCheckAppid",  L"CheckAppid",IDR_XML_APPID,     TOWSTR(IDR_XML_APPID),    L"CheckAppid"),
	ACTION(L"APPID768", L"WriteAppid",  L"CWriteAppid",  L"WriteAppid", IDR_XML_APPID_768K,     TOWSTR(IDR_XML_APPID_768K),    L"WriteAppid"),
	ACTION(L"APPID768", L"CheckAppid",  L"CCheckAppid",  L"CheckAppid", IDR_XML_APPID_768K,     TOWSTR(IDR_XML_APPID_768K),    L"CheckAppid"),

    ACTION(L"SSN(Sougou)", L"WriteSSN",  L"CWriteSSN",  L"WriteSSN", 0,     NULL,    L"WriteSSN"),
    ACTION(L"SSN(Sougou)", L"CheckSSN",  L"CCheckSSN",  L"CheckSSN", 0,     NULL,    L"CheckSSN"),

	ACTION(L"SSNS1", L"WriteSSNS1",  L"CWriteSSNSN1",  L"WriteSSNS1", 0,     NULL,    L"WriteSSNS1"),

	ACTION(L"Kuda", L"WriteKudaCodes",  L"CWriteKudaCodes",  L"WriteKudaCodes", 0,     NULL,    L"WriteKudaCodes"),

#endif 
