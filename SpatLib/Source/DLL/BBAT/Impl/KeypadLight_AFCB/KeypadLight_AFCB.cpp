#include "StdAfx.h"
#include "KeypadLight_AFCB.h"

//
IMPLEMENT_RUNTIME_CLASS(CKeypadLightAfcb)

//////////////////////////////////////////////////////////////////////////
CKeypadLightAfcb::CKeypadLightAfcb(void)
{
    
}

CKeypadLightAfcb::~CKeypadLightAfcb(void)
{
}

SPRESULT CKeypadLightAfcb:: __PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return KeypadLightAction();
}

BOOL CKeypadLightAfcb::LoadXMLConfig(void)
{
    m_nKeypadLightNo = (KeypadLightName)GetConfigValue(L"Option:LightNumber", 0);
    m_dUpSpec = GetConfigValue(L"Option:UpSpec", 0.0);
    m_dDownSpec = GetConfigValue(L"Option:DownSpec", 0.0);
    m_nSingleDelay = GetConfigValue(L"Param:SingleDelay", 500);
    m_nMultiDelay = GetConfigValue(L"Param:MultiDelay", 500);

    LPCWSTR lpwTestType[2] =
    {
        L"Single",
        L"Multi"
    };
    m_eTestType = (TestType)GetComboxValue(lpwTestType, 2, L"Option:TestType");

    if (MAX_TEST_TYPE == m_eTestType)
    {
        return FALSE;
    }
    return TRUE;
}

SPRESULT CKeypadLightAfcb::KeypadLightAction()
{
    SPRESULT Res = SP_OK;
    CHKRESULT(LightEnable(TRUE));
    for (int i = 0; i < 3; i++)
    {
        RETRY_PRINT(i, "Retry")
            Res = RunKeypadLightTest();
        if (Res == SP_OK)
        {
            break;
        }
    }
    CHKRESULT(LightEnable(FALSE));
    return Res;
}

SPRESULT CKeypadLightAfcb::LightEnable(BOOL bStatus)
{
    LPCSTR lpTemp[2] =
    {
        "AFCB: LED_B Set Disable",
        "AFCB: LED_B Set Enable"
    };

    RETURNSPRESULT(m_pObjFunBox->FB_Light_LED_B_En_Set(bStatus), lpTemp[bStatus]);
    return SP_OK;
}

SPRESULT CKeypadLightAfcb::KeypadLightOpen()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_KeypadBackLightOpen(m_hDUT),
        "Phone: Keypad Light OPEN", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: Keypad Light OPEN", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CKeypadLightAfcb::KeypadLightClose()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_KeypadBackLightClose(m_hDUT),
        "Phone: Keypad Light Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: Keypad Light Close", LEVEL_ITEM);
    return SP_OK;
}


SPRESULT CKeypadLightAfcb::GetVoltageMulti(Times nTimes)
{
    for (KeypadLightName i = LED8; i < m_nKeypadLightNo; i = (KeypadLightName)(i + 1))
    {
        float fVoltage = 0.0;
        CHKRESULT(GetVoltage(&fVoltage, i));
        CHKRESULT(SaveVoltage(nTimes, fVoltage, i));
    }
    return SP_OK;
}

SPRESULT CKeypadLightAfcb::GetVoltage(float* pfVoltage, KeypadLightName nKeypadLightNo)
{
    Sleep(500);
    int nLedIndex = FB_LED8 + nKeypadLightNo;
    char szInfo[128] = { 0 };
    sprintf_s(szInfo, sizeof(szInfo), ("G3: LED%i Get Voltage"), nLedIndex);
    if (nLedIndex >= FB_LED8 && nLedIndex <= FB_LED15)
    {
        RETURNSPRESULT(m_pObjFunBox->FB_PhotoRes_LED_GetVoltage_X((FUNBOX_LED_INDEX)nLedIndex, pfVoltage), szInfo);
    }
    else
    {
        PrintErrorMsg(SP_E_BBAT_CMD_FAIL, "Wrong G3 LED Type", LEVEL_ITEM);
        return SP_E_BBAT_CMD_FAIL;
    }
    _UiSendMsg("Voltage", LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Voltage = %.2f", *pfVoltage);
    return SP_OK;
}

SPRESULT CKeypadLightAfcb::SaveVoltage(Times nTimes, float fVoltage, KeypadLightName nKeypadLightNo)
{
    if (nTimes == FirstTime)
    {
        m_arrVoltage_1[nKeypadLightNo] = (double)fVoltage;
    }
    else
    {
        m_arrVoltage_2[nKeypadLightNo] = (double)fVoltage;
        m_arrDeltaVoltage[nKeypadLightNo] = (abs)(m_arrVoltage_2[nKeypadLightNo] - m_arrVoltage_1[nKeypadLightNo]);
    }
    return SP_OK;
}

SPRESULT CKeypadLightAfcb::GetVoltageSingle(Times nTimes)
{
    float fVoltage = 0.0;
    CHKRESULT(GetVoltage(&fVoltage, m_nKeypadLightNo));
    
    CHKRESULT(SaveVoltage(nTimes, fVoltage, m_nKeypadLightNo));
    return SP_OK;
}

SPRESULT CKeypadLightAfcb::GetDelta(KeypadLightName nKeypadLightNo)
{
    _UiSendMsg(m_lpKeyName[nKeypadLightNo], LEVEL_ITEM,
        m_dDownSpec, m_arrDeltaVoltage[nKeypadLightNo], m_dUpSpec,
        nullptr, -1, nullptr,
        "Delta Voltage = %.2f", m_arrDeltaVoltage[nKeypadLightNo]);

    if (!IN_RANGE(m_dDownSpec, m_arrDeltaVoltage[nKeypadLightNo], m_dUpSpec))
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CKeypadLightAfcb::GetDeltaSingle()
{
    CHKRESULT(GetDelta(m_nKeypadLightNo));
    return SP_OK;
}

SPRESULT CKeypadLightAfcb::GetDeltaMulti()
{
    for (KeypadLightName i = LED8; i < m_nKeypadLightNo; i = (KeypadLightName)(i + 1))
    {
        CHKRESULT(GetDelta(i));
    }
    return SP_OK;
}

SPRESULT CKeypadLightAfcb::RunKeypadLightTest()
{
    CHKRESULT(KeypadLightOpen());
    if (m_eTestType == Single)
    {
        Sleep(m_nSingleDelay);
        CHKRESULT(GetVoltageSingle(FirstTime));
        CHKRESULT(KeypadLightClose());
        Sleep(m_nSingleDelay);
        CHKRESULT(GetVoltageSingle(SecondTime));
        CHKRESULT(GetDeltaSingle());
    }
    else
    {
        Sleep(m_nMultiDelay);
        CHKRESULT(GetVoltageMulti(FirstTime));
        CHKRESULT(KeypadLightClose());
        Sleep(m_nMultiDelay);
        CHKRESULT(GetVoltageMulti(SecondTime));
        CHKRESULT(GetDeltaMulti());
    }
    return SP_OK;
}