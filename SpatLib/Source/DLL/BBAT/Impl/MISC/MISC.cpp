#include "StdAfx.h"
#include "MISC.h"

//
IMPLEMENT_RUNTIME_CLASS(CMisc)

//////////////////////////////////////////////////////////////////////////
CMisc::CMisc(void)
{
}

CMisc::~CMisc(void)
{
}

SPRESULT CMisc::__PollAction(void)
{
    if (m_nBackFlashCold)
    {
        CHKRESULT(BackFlashCold());
    }
    if (m_nBackFlashWarm)
    {
        CHKRESULT(BackFlashWarm());
    }
    if (m_nFrontFlash)
    {
        CHKRESULT(FrontFlash());
    }
    if (m_nTorch)
    {
        CHKRESULT(Torch());
    }
    if (m_nKeypadBackLight)
    {
        CHKRESULT(KeypadBackLight());
    }
    if (m_nRgbRed)
    {
        CHKRESULT(RgbRed());
    }
    if (m_nRgbBlue)
    {
        CHKRESULT(RgbBlue());
    }
    if (m_nRgbGreen)
    {
        CHKRESULT(RgbGreen());
    }
    if (m_nVibratorOpen)
    {
        CHKRESULT(VibratorOpen());
    }
    if (m_nVibratorClose)
    {
        Sleep(m_nVibratorDelayTime);
        CHKRESULT(VibratorClose());
    }
    if (m_nLcdWhite)
    {
        CHKRESULT(LcdWhite());
    }
    if (m_nLcdRed)
    {
        CHKRESULT(LcdRed());
    }
    if (m_nLcdGreen)
    {
        CHKRESULT(LcdGreen());
    }
    if (m_nLcdBlue)
    {
        CHKRESULT(LcdBlue());
    }
    if (m_nLcdBlack)
    {
        CHKRESULT(LcdBlack());
    }

    if (m_nUartLoop)
    {
        CHKRESULT(UartLoop());
    }
    return SP_OK;
}
BOOL CMisc::LoadXMLConfig(void)
{
    m_nBackFlashCold = GetConfigValue(L"Option:Flash:Back_Flash_Cold", 0);
    m_nBackFlashWarm = GetConfigValue(L"Option:Flash:Back_Flash_Warm", 0);
    m_nFrontFlash = GetConfigValue(L"Option:Flash:Front_Flash", 0);
    m_nTorch = GetConfigValue(L"Option:Flash:Torch", 0);
    m_nKeypadBackLight = GetConfigValue(L"Option:Flash:Keypad", 0);

    m_nRgbRed = GetConfigValue(L"Option:RGB:Red", 0);
    m_nRgbBlue = GetConfigValue(L"Option:RGB:Blue", 0);
    m_nRgbGreen = GetConfigValue(L"Option:RGB:Green", 0);

    m_nVibratorOpen = GetConfigValue(L"Option:Vibrator:Open", 0);
    m_nVibratorClose = GetConfigValue(L"Option:Vibrator:Close", 0);

    m_nLcdWhite = GetConfigValue(L"Option:LCD:White", 0);
    m_nLcdRed = GetConfigValue(L"Option:LCD:Red", 0);
    m_nLcdGreen = GetConfigValue(L"Option:LCD:Green", 0);
    m_nLcdBlue = GetConfigValue(L"Option:LCD:Blue", 0);
    m_nLcdBlack = GetConfigValue(L"Option:LCD:Black", 0);
    m_nUartLoop = GetConfigValue(L"Option:UartLoop", 0);

    m_nFlashDelayTime = GetConfigValue(L"Param:FlashDelayTime", 500);
    m_nRgbDelayTime = GetConfigValue(L"Param:RgbDelayTime", 500);
    m_nVibratorDelayTime = GetConfigValue(L"Param:VibratorDelayTime", 1000);
    m_nLcdDelayTime = GetConfigValue(L"Param:LcdDelayTime", 500);

    return TRUE;
}

SPRESULT CMisc::BackFlashCold()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_FlashLedOpenCold(m_hDUT), "Back Flash Cold Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Back Flash Cold Open", LEVEL_ITEM);
    Sleep(m_nFlashDelayTime);
    CHKRESULT(PrintErrorMsg(SP_BBAT_FlashLedClose(m_hDUT), "Back Flash Cold Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Back Flash Cold Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::BackFlashWarm()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_FlashLedOpenWarm(m_hDUT), "Back Flash Warm Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Back Flash Warm Open", LEVEL_ITEM);
    Sleep(m_nFlashDelayTime);
    CHKRESULT(PrintErrorMsg(SP_BBAT_FlashLedClose(m_hDUT), "Back Flash Warm Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Back Flash Warm Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::FrontFlash()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_FrontFlashOpen(m_hDUT), "Front Flash Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Front Flash Open", LEVEL_ITEM);
    Sleep(m_nFlashDelayTime);
    CHKRESULT(PrintErrorMsg(SP_BBAT_FrontFlashClose(m_hDUT), "Front Flash Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Front Flash Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::Torch()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_TorchOpen(m_hDUT), "Torch Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Torch Open", LEVEL_ITEM);
    Sleep(m_nFlashDelayTime);
    CHKRESULT(PrintErrorMsg(SP_BBAT_TorchClose(m_hDUT), "Torch Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Torch Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::KeypadBackLight()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_KeypadBackLightOpen(m_hDUT), "Keypad Back Light Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Keypad Back Light Open", LEVEL_ITEM);
    Sleep(m_nFlashDelayTime);
    CHKRESULT(PrintErrorMsg(SP_BBAT_KeypadBackLightClose(m_hDUT), "Keypad Back Light Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Keypad Back Light Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::RgbRed()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_RgbOpen(m_hDUT, RED), "RGB Red Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "RGB Red Open", LEVEL_ITEM);
    Sleep(m_nRgbDelayTime);
    CHKRESULT(PrintErrorMsg(SP_BBAT_RgbClose(m_hDUT), "RGB Red Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "RGB Red Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::RgbBlue()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_RgbOpen(m_hDUT, BLUE), "RGB Blue Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "RGB Blue Open", LEVEL_ITEM);
    Sleep(m_nRgbDelayTime);
    CHKRESULT(PrintErrorMsg(SP_BBAT_RgbClose(m_hDUT), "RGB Blue Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "RGB Blue Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::RgbGreen()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_RgbOpen(m_hDUT, GREEN), "RGB Green Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "RGB Green Open", LEVEL_ITEM);
    Sleep(m_nRgbDelayTime);
    CHKRESULT(PrintErrorMsg(SP_BBAT_RgbClose(m_hDUT), "RGB Green Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "RGB Green Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::VibratorOpen()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_VibratorOpen(m_hDUT), "Vibrator Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Vibrator Open", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::VibratorClose()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_VibratorClose(m_hDUT), "Vibrator Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Vibrator Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CMisc::LcdWhite()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_LcdColor(m_hDUT, MIPILCD_WHITE), "LCD White", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "LCD White", LEVEL_ITEM);
    Sleep(m_nLcdDelayTime);
    return SP_OK;
}

SPRESULT CMisc::LcdRed()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_LcdColor(m_hDUT, MIPILCD_RED), "LCD Red", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "LCD Red", LEVEL_ITEM);
    Sleep(m_nLcdDelayTime);
    return SP_OK;
}

SPRESULT CMisc::LcdGreen()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_LcdColor(m_hDUT, MIPILCD_GREEN), "LCD Green", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "LCD Green", LEVEL_ITEM);
    Sleep(m_nLcdDelayTime);
    return SP_OK;
}

SPRESULT CMisc::LcdBlue()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_LcdColor(m_hDUT, MIPILCD_BLUE), "LCD Blue", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "LCD Blue", LEVEL_ITEM);
    Sleep(m_nLcdDelayTime);
    return SP_OK;
}

SPRESULT CMisc::LcdBlack()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_LcdColor(m_hDUT, MIPILCD_BLACK), "LCD Black", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "LCD Black", LEVEL_ITEM);
    Sleep(m_nLcdDelayTime);
    return SP_OK;
}

SPRESULT CMisc::UartLoop()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_UartLoop(m_hDUT), "Uart Loop ", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Uart Loop", LEVEL_ITEM);
    Sleep(500);
    return SP_OK;
}
