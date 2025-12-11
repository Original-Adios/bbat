#include "StdAfx.h"
#include "Mcu_LCD_AFCB.h"

//
IMPLEMENT_RUNTIME_CLASS(CMcuLcdAfcb)

//////////////////////////////////////////////////////////////////////////
CMcuLcdAfcb::CMcuLcdAfcb(void)
{
    ZeroMemory(m_TValue, sizeof(m_TValue));
    ZeroMemory(m_TSpec, sizeof(m_TSpec));
}

CMcuLcdAfcb::~CMcuLcdAfcb(void)
{
}

SPRESULT CMcuLcdAfcb::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    CHKRESULT(LcdAction());
    return SP_OK;
}

BOOL CMcuLcdAfcb::LoadXMLConfig(void)
{
    m_TSpec[RED].nUpLimit = GetConfigValue(L"Option:RED:SpecUp", 0);
    m_TSpec[RED].nDownLimit = GetConfigValue(L"Option:RED:SpecDown", 0);
    m_TSpec[RED].dPercent = GetConfigValue(L"Option:RED:PercentDown", 0.0);

    m_TSpec[BLUE].nUpLimit = GetConfigValue(L"Option:BLUE:SpecUp", 0);
    m_TSpec[BLUE].nDownLimit = GetConfigValue(L"Option:BLUE:SpecDown", 0);
    m_TSpec[BLUE].dPercent = GetConfigValue(L"Option:BLUE:PercentDown", 0.0);

    m_nSleepTime = GetConfigValue(L"Param:SleepTime", 2000);
    return TRUE;
}

SPRESULT CMcuLcdAfcb::LcdAction()
{
    CHKRESULT(RgbSet());
    Sleep(200);

    CHKRESULT(LcdRgbRed());
    Sleep(m_nSleepTime);
    CHKRESULT(RgbGet(LCD_RED));
    CHKRESULT(CheckResult(LCD_RED));

    CHKRESULT(LcdRgbBlue());
    Sleep(m_nSleepTime);
    CHKRESULT(RgbGet(LCD_BLUE));
    CHKRESULT(CheckResult(LCD_BLUE));

    CHKRESULT(LcdRgbBlack());
    CHKRESULT(LcdRgbSwitchCtrl());

    return SP_OK;
}

SPRESULT CMcuLcdAfcb::RgbGet(ColorType Color)
{

    RETURNSPRESULT(m_pObjFunBox->FB_RGB_Get_Color(
        &m_TValue[Color].wValue[RED], &m_TValue[Color].wValue[GREEN], &m_TValue[Color].wValue[BLUE], 
        &m_TValue[Color].wColorC), "AFCB: RGB GET");

    m_TValue[Color].dPercent = ((double)m_TValue[Color].wValue[Color] / m_TValue[Color].wColorC) * 100;

    return SP_OK;
}

SPRESULT CMcuLcdAfcb::CheckResult(ColorType Color)
{
    _UiSendMsg("SPEC", LEVEL_ITEM,
        m_TSpec[Color].nDownLimit, m_TValue[Color].wValue[Color], m_TSpec[Color].nUpLimit,
        nullptr, -1, nullptr,
        "%s Value = %d", m_arrColorName[Color].c_str(), m_TValue[Color].wValue[Color]);
    if (!IN_RANGE(m_TSpec[Color].nDownLimit, m_TValue[Color].wValue[Color], m_TSpec[Color].nUpLimit))
    {
        return SP_E_BBAT_VALUE_FAIL;
    }

    _UiSendMsg("SPEC", LEVEL_ITEM,
        m_TSpec[Color].dPercent, m_TValue[Color].dPercent, 100,
        nullptr, -1, nullptr,
        "%s Percent = %.2f", m_arrColorName[Color].c_str(), m_TValue[Color].dPercent);

    if (!IN_RANGE(m_TSpec[Color].dPercent, m_TValue[Color].dPercent, 100))
    {
        return SP_E_BBAT_VALUE_FAIL;
    }

    return SP_OK;
}

SPRESULT CMcuLcdAfcb::RgbSet()
{
    RETURNSPRESULT(m_pObjFunBox->FB_RGB_Set_Time_AMP((BYTE)nTime, (BYTE)nAmp),"AFCB: RGB SET");
    return SP_OK;
}

SPRESULT CMcuLcdAfcb::LcdRgbRed()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_Mcu_LcdColor(m_hDUT, MIPILCD_RED),
        "PHONE: RGB RED", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "PHONE: RGB RED", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CMcuLcdAfcb::LcdRgbBlue()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_Mcu_LcdColor(m_hDUT, MIPILCD_BLUE),
        "PHONE: RGB BLUE", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "PHONE: RGB BLUE", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CMcuLcdAfcb::LcdRgbBlack()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_Mcu_LcdColor(m_hDUT, MIPILCD_BLACK),
        "PHONE: RGB BLACK", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "PHONE: RGB BLACK", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CMcuLcdAfcb::LcdRgbSwitchCtrl()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_Mcu_LcdColor(m_hDUT, MIPILCD_SWTICH_CTRL),
        "PHONE: RGB Switch Ctrl", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "PHONE: RGB Switch Ctrl", LEVEL_ITEM);
    return SP_OK;
}
