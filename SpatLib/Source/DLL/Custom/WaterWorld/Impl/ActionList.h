
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
	IDR_XML_SQLCONNECT IDR_XML_INPUTBARCODES
 */
    ACTION(L"WW", L"SaveCountryCode",  L"CSaveCountryCode",   L"SaveCountryCode",   IDR_XML_COUNTRYCODE,   TOWSTR(IDR_XML_COUNTRYCODE),    L"Save country code"),
    ACTION(L"WW", L"DeleteCountryCode",  L"CDeleteCountryCode",   L"DeleteCountryCode",   IDR_XML_DELETECOUNTRYCODE,   TOWSTR(IDR_XML_DELETECOUNTRYCODE),    L"Delete Country Code"),
    ACTION(L"WW", L"InputBarCodes",  L"CInputBarCodes",   L"InputBarCodes",   IDR_XML_INPUTBARCODES,   TOWSTR(IDR_XML_INPUTBARCODES),    L"Input BarCodes"),
    ACTION(L"WW", L"SqlConnect",  L"CSqlConnect",   L"SqlConnect",   IDR_XML_SQLCONNECT,   TOWSTR(IDR_XML_SQLCONNECT),    L"Sql Connect"),
    ACTION(L"WW", L"SqlDisConnect",  L"CSqlDisConnect",   L"SqlDisConnect",   0,   TOWSTR(NULL),    L"Sql DisConnect"),
	ACTION(L"WW", L"GetInputInfo",       L"CGetInputInfo",   L"GetInputInfo",   IDR_XML_GET_INPUT_INFO, TOWSTR(IDR_XML_GET_INPUT_INFO),    L"GetInputInfo"),
	ACTION(L"WW", L"SaveCodesInfo",       L"CSaveCodesInfo",   L"SaveCodesInfo",   IDR_XML_SAVE_CODES_INFO, TOWSTR(IDR_XML_SAVE_CODES_INFO),    L"SaveCodesInfo"),
#endif 
