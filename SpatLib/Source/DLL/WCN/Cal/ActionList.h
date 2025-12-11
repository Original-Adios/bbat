
#include "resource.h"

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
  ACTION(L"WlanAFCTx",    L"WlanAFCTx",		    L"CWlanAFCTx",		L"WlanAFC",		  IDR_XML_WLANAFCTX,	TOWSTR(IDR_XML_WLANAFCTX),	L"Wlan Tx AFC calibration"),
  ACTION(L"WriteEfuse",  L"WriteEfuse",       L"CWriteEfuse",	L"WriteEfuse",	  IDR_XML_WRITEEFUSE,		TOWSTR(IDR_XML_WRITEEFUSE),		L"Write wcn mac addres & AFC and APC efuse"),
  ACTION(L"Verify MAC",  L"Verify MAC",       L"CVerifyMac",	L"Verify MAC",	  IDR_XML_DUMMY,		TOWSTR(IDR_XML_DUMMY),		L"Verify Mac address"),
  ACTION(L"TSX",          L"TsxStart",    L"CBtAFCRx",		L"TsxStart",		   IDR_XML_TSX_BT,       TOWSTR(IDR_XML_TSX_BT),      L"BT TSX the 1st temperature measurement"),
  ACTION(L"TSX",          L"TsxCal",      L"CBtAFCRx",			L"TsxCal",		   IDR_XML_DUMMY,       TOWSTR(IDR_XML_DUMMY),      L"BT TSX the 2nd temperature measurement"),
  ACTION(L"TSX",          L"TsxFinal",    L"CBtAFCRx",		L"TsxFinal",		   IDR_XML_TSX_BT_FINAL,	   TOWSTR(IDR_XML_TSX_BT_FINAL),      L"BT TSX End"),
  ACTION(L"WlanAPC",      L"WlanAPC",    L"CWlanApc",		L"WlanAPC",			   IDR_XML_WLAN_APC,       TOWSTR(IDR_XML_WLAN_APC),      L"Wlan TPC calibration"),
  ACTION(L"BTAFCRx",      L"BTAFCRx",    L"CBtAFCRx",		L"BTAFCRx",		   IDR_XML_AFC_BT,	   TOWSTR(IDR_XML_AFC_BT),      L"BT Rx AFC calibration"),
#endif 
