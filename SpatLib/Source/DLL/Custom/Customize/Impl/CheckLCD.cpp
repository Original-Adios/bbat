#include "stdafx.h"
#include "CheckLCD.h"

IMPLEMENT_RUNTIME_CLASS(CCheckLCD)

CCheckLCD::CCheckLCD()
{
}

CCheckLCD::~CCheckLCD()
{

}

BOOL CCheckLCD::LoadXMLConfig(void)
{
    m_strLCD = _W2CA(GetConfigValue(L"Option:LCD", L""));

    return TRUE;
}

SPRESULT CCheckLCD::__PollAction(void)
{
    LPCSTR LCD_CMD = "AT+LCDID";
    SPRESULT res = SendATCommand(LCD_CMD);
    if (SP_OK != res)
    {
        NOTIFY("LCD", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, LCD_CMD);
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }
    else
    {
        if (NULL == strstr(m_strRevBuf.c_str(), m_strLCD.c_str()))
        {
            NOTIFY("LCD", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, m_strRevBuf.c_str());
            return SP_E_CHECK_LCD_TYPE_ERROR;
        }
        else
        {
            NOTIFY("LCD", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
        }
    }

    return SP_OK;
}
