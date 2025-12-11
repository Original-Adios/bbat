
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
    ACTION(L"Xiaoxun", L"GetOTP",  L"CGetOTP",  L"[Xiaoxun]GetOTP", IDR_XML_GETOTP,     TOWSTR(IDR_XML_GETOTP),    L"Xiaoxun Get OTP"),
    ACTION(L"Xiaoxun", L"WriteOTP",  L"CWriteOTP",  L"[Xiaoxun]WriteOTP", 0,            NULL,    L"Xiaoxun Write OTP"),
	ACTION(L"Xiaoxun", L"CheckOTP",L"CCheckOTP",L"[Xiaoxun]CheckOTP",IDR_XML_CHECKOTP,	TOWSTR(IDR_XML_CHECKOTP),    L"Xiaoxun Check OTP"),
#endif 
