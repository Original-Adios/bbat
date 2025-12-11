#include "StdAfx.h"
#include "LED_AFCB.h"



//////////////////////////////////////////////////////////////////////////
CLedAfcb::CLedAfcb(TGroupLed tGroup, CImpBase* pImp)
{
    m_pImp = pImp;
    m_eFlashType = tGroup.eFlashType;
    m_eLedType = tGroup.eLedType;
    m_dVoltageUpSpec = tGroup.dVoltageUp;
    m_dVoltageDownSpec = tGroup.dVoltageDown;
    m_nGpioNo = tGroup.nGpioNo;
}

CLedAfcb::~CLedAfcb(void)
{

}

SPRESULT CLedAfcb::Run(void)
{

    CHKRESULT(LightLedSet(TRUE));

    CHKRESULT(LedOpen());
    Sleep(500);
    CHKRESULT(GetVoltage(FirstTime));

    CHKRESULT(LedClose());
    Sleep(500);
    CHKRESULT(GetVoltage(SecondTime));
    CHKRESULT(DeltaVoltage());
    CHKRESULT(LightLedSet(FALSE));
    return SP_OK;
}

SPRESULT CLedAfcb::LightLedSet(BOOL bStatus)
{
    LPCSTR lpTemp[2] = 
    {
        "AFCB: LED_A Set Disable",
        "AFCB: LED_A Set Enable"
    };

    RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Light_LED_A_En_Set(bStatus), lpTemp[bStatus]);
    return SP_OK;
}

SPRESULT CLedAfcb::LedOpen()
{
    if (m_eFlashType == FlashCold)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(SP_BBAT_FlashLedOpenCold(m_pImp->m_hDUT),
            "Phone: Flash Cold Open", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Flash Cold Open", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == FlashWarm)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(SP_BBAT_FlashLedOpenWarm(m_pImp->m_hDUT),
            "Phone: Flash Warm Open", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Flash Warm Open", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == FP_Flash)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(SP_BBAT_FP_FlashLightOpen(m_pImp->m_hDUT),
            "Phone: FP Flash Warm Open", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: FP Flash Warm Open", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == FrontFlash)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(SP_BBAT_FrontFlashOpen(m_pImp->m_hDUT),
            "Phone: Front Flash Open", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Front Flash Open", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == KeypadLight)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_KeypadBackLightOpen(m_pImp->m_hDUT),
            "Phone: Keypad Back Light Open", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Keypad Back Light Open", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == LedRed)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_RgbOpen(m_pImp->m_hDUT, RED),
            "Phone: Led Red Open", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Led Red Open", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == LedGreen)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_RgbOpen(m_pImp->m_hDUT, GREEN),
            "Phone: Led Green Open", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Led Green Open", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == LedBlue)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_RgbOpen(m_pImp->m_hDUT, BLUE),
            "Phone: Led Blue Open", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Led Blue Open", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == Torch)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_TorchOpen(m_pImp->m_hDUT),
            "Phone: Torch Open", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Torch Open", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == GPIO)
    {
        CHKRESULT(GpioWriteUp());
//        Sleep(500);
        return SP_OK;
    }
    else
    {
        m_pImp->PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "Wrong Led Type", LEVEL_ITEM);
        return SP_E_PHONE_BBAT_COMMAND_FAIL;
    }
}

SPRESULT CLedAfcb::LedClose()
{
    if (m_eFlashType == FP_Flash)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(SP_BBAT_FP_FlashLightClose(m_pImp->m_hDUT),
            "Phone: FP Flash Close", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: FP Flash Close", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == FlashCold || m_eFlashType == FlashWarm)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(SP_BBAT_FlashLedClose(m_pImp->m_hDUT),
            "Phone: Flash Close", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Flash Close", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == FrontFlash)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(SP_BBAT_FrontFlashClose(m_pImp->m_hDUT),
            "Phone: Front Flash Close", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Front Flash Close", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == KeypadLight)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_KeypadBackLightClose(m_pImp->m_hDUT),
            "Phone: Keypad Back Light Close", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Keypad Back Light Close", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == LedRed || m_eFlashType == LedGreen || m_eFlashType == LedBlue)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_RgbClose(m_pImp->m_hDUT),
            "Phone: Led Close", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Led Close", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == Torch)
    {
        CHKRESULT(m_pImp->PrintErrorMsg(
            SP_BBAT_TorchClose(m_pImp->m_hDUT),
            "Phone: Torch Close", LEVEL_ITEM));
        m_pImp->PrintSuccessMsg(SP_OK, "Phone: Torch Close", LEVEL_ITEM);
        return SP_OK;
    }
    else if (m_eFlashType == GPIO)
    {
        CHKRESULT(GpioWriteDown());
//        Sleep(500);
        return SP_OK;
    }
    else
    {
        m_pImp->PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "Wrong LED Type", LEVEL_ITEM);
        return SP_E_PHONE_BBAT_COMMAND_FAIL;
    }
}

SPRESULT CLedAfcb::AfcbLed(float* pfVoltage)
{
    Sleep(500);
    char szInfo[128] = { 0 };
    sprintf_s(szInfo, sizeof(szInfo), ("G3: LED%i Get Voltage"), (int)m_eLedType);
    if (m_eLedType >= FB_LED0 && m_eLedType <= FB_LED7)
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_PhotoRes_LED_GetVoltage_X((FUNBOX_LED_INDEX)m_eLedType, pfVoltage), szInfo);
    }
    else
    {
        m_pImp->PrintErrorMsg(SP_E_BBAT_CMD_FAIL, "Wrong G3 LED Type", LEVEL_ITEM);
        return SP_E_BBAT_CMD_FAIL;
    }
    m_pImp->_UiSendMsg("Voltage", LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Voltage = %.2f", *pfVoltage);
    return SP_OK;
}

SPRESULT CLedAfcb::GetVoltage(Times eTimes)
{
    float fVoltage = 0.0;
    CHKRESULT(AfcbLed(&fVoltage));
    if (eTimes == FirstTime)
    {
        m_dVoltage1 = (double)fVoltage;
    }
    else if (eTimes == SecondTime)
    {
        m_dVoltage2 = (double)fVoltage;
        m_dVoltageDelta = (abs)(m_dVoltage2 - m_dVoltage1);
    }
    return SP_OK;
}

SPRESULT CLedAfcb::DeltaVoltage()
{
    m_pImp->_UiSendMsg(m_lpFlashName[m_eFlashType], LEVEL_ITEM,
        m_dVoltageDownSpec, m_dVoltageDelta, m_dVoltageUpSpec,
        nullptr, -1, nullptr,
        "Delta Voltage = %.2f", m_dVoltageDelta);
    if (!IN_RANGE(m_dVoltageDownSpec, m_dVoltageDelta, m_dVoltageUpSpec))
    {
        return SP_E_PHONE_BBAT_COMMAND_FAIL;
    }
    return SP_OK;
}

SPRESULT CLedAfcb::GpioWriteUp()
{
    SPRESULT Res = SP_BBAT_GpioWritePullUp(m_pImp->m_hDUT, (int8)m_nGpioNo);
    m_pImp->_UiSendMsg("Phone: GPIO Write Up", LEVEL_ITEM,
        SP_OK, Res, SP_OK,
        nullptr, -1, nullptr,
        "Gpio Number = %d", m_nGpioNo);
    if (Res != SP_OK)
    {
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CLedAfcb::GpioWriteDown()
{
    SPRESULT Res = SP_BBAT_GpioWritePullDown(m_pImp->m_hDUT, (int8)m_nGpioNo);
    m_pImp->_UiSendMsg("Phone: GPIO Write Down", LEVEL_ITEM,
        SP_OK, Res, SP_OK,
        nullptr, -1, nullptr,
        "Gpio Number = %d", m_nGpioNo);
    if (Res != SP_OK)
    {
        return SP_E_BBAT_GPIO_VALUE_FAIL;
    }
    return SP_OK;
}
