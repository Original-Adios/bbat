#include "StdAfx.h"
#include "GPIO.h"

/// 
IMPLEMENT_RUNTIME_CLASS(CGPIO)
//////////////////////////////////////////////////////////////////////////
CGPIO::CGPIO(void)
{
}

CGPIO::~CGPIO(void)
{
}

BOOL CGPIO::LoadXMLConfig(void)
{
  /* 版本中请删除
    INT y = GetConfigValue(L"Option:P1", 1);
    m_x = GetConfigValue(L"Option:P1", 0);
    LPCWSTR lpParam = GetConfigValue(L"Param:P2", L"");
    */
    return TRUE;
}

SPRESULT CGPIO::__PollAction(void)
{
/*
    版本中请删除
    NOTIFY("测试项目1", LEVEL_UI|LEVEL_REPORT, NOLOWLMT, 100.2, NOUPPLMT, NULL, -1, NULL);
    NOTIFY("关键信息", "dddddd");
    LogFmtStrA(SPLOGLV_VERBOSE, "This is trace, %s %d", "hello", 123);

    uint8 buff[10] = {0x01, 0x02, 0x03};
    LogBufData(SPLOGLV_INFO, buff, sizeof(buff));


    //SPRESULT res = SP_AutoBBTest(m_hDUT, ...)
*/
    return SP_OK;
}