
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
    ACTION(L"UIS8910", L"GSEditor", L"CMakeGSUIS8910",   L"MakeGolden",   IDR_XML_MAKE_UIS8910,   TOWSTR(IDR_XML_MAKE_UIS8910),    L"Make Golden Sample for UIS8910"),
    ACTION(L"UIS8910", L"GSCLC",    L"CCalcLSUIS8910",   L"LossCalc",     IDR_XML_CALC_UIS8910,   TOWSTR(IDR_XML_CALC_UIS8910),    L"Calculate Loss for UIS8910"),
    ACTION(L"UIS1930", L"GSEditor", L"CMakeGSUIS8910",   L"MakeGolden",   IDR_XML_MAKE_UIS1930,   TOWSTR(IDR_XML_MAKE_UIS1930),    L"Make Golden Sample for UIS1930"),
    ACTION(L"UIS1930", L"GSCLC",    L"CCalcLSUIS8910",   L"LossCalc",     IDR_XML_CALC_UIS1930,   TOWSTR(IDR_XML_CALC_UIS1930),    L"Calculate Loss for UIS1930"),
    ACTION(L"UIS8850", L"GSEditor", L"CMakeGSUIS8910",   L"MakeGolden",   IDR_XML_MAKE_UIS8850,   TOWSTR(IDR_XML_MAKE_UIS8850),    L"Make Golden Sample for UIS8850"),
    ACTION(L"UIS8850", L"GSCLC",    L"CCalcLSUIS8910",   L"LossCalc",     IDR_XML_CALC_UIS8850,   TOWSTR(IDR_XML_CALC_UIS8850),    L"Calculate Loss for UIS8850"),
    ACTION(L"W217",    L"GSEditor", L"CMakeGSUIS8910",   L"MakeGolden",   IDR_XML_MAKE_UIS8850,   TOWSTR(IDR_XML_MAKE_UIS8850),    L"Make Golden Sample for W217"),
    ACTION(L"W217",    L"GSCLC",    L"CCalcLSUIS8910",   L"LossCalc",     IDR_XML_CALC_UIS8850,   TOWSTR(IDR_XML_CALC_UIS8850),    L"Calculate Loss for W217"),
    ACTION(L"UIS8910C2&C3", L"GSEditor", L"CMakeGSUIS8910", L"MakeGolden", IDR_XML_MAKE_UIS891C, TOWSTR(IDR_XML_MAKE_UIS891C),     L"Make Golden Sample for UIS8910C2_C3"),
    ACTION(L"UIS8910C2&C3", L"GSCLC",    L"CCalcLSUIS8910", L"LossCalc",   IDR_XML_CALC_UIS891C, TOWSTR(IDR_XML_CALC_UIS891C),     L"Calculate Loss for UIS8910C2_C3"),
    ACTION(L"UIS8910FF-L", L"GSEditor", L"CMakeGSUIS8910", L"MakeGolden", IDR_XML_MAKE_UIS8910FF_L, TOWSTR(IDR_XML_MAKE_UIS8910FF_L),    L"Make Golden Sample for UIS8910FF-L"),
    ACTION(L"UIS8910FF-L", L"GSCLC",    L"CCalcLSUIS8910", L"LossCalc",   IDR_XML_CALC_UIS8910FF_L, TOWSTR(IDR_XML_CALC_UIS8910FF_L),     L"Calculate Loss for UIS8910FF-L")

#endif 
