#include "StdAfx.h"
#include "ImpGpioAfcb.h"
IMPLEMENT_RUNTIME_CLASS(CImpGpioAfcb)

CImpGpioAfcb::CImpGpioAfcb(void)
{
}
CImpGpioAfcb::~CImpGpioAfcb(void)
{
}

SPRESULT CImpGpioAfcb::__PollAction(void)
{
    SPRESULT spRlt = SP_OK;
    FUNBOX_INIT_CHECK();
    spRlt = GpioAction();
    spRlt |= GpioReset();
    return spRlt;
}

BOOL CImpGpioAfcb::LoadXMLConfig(void)
{
    m_vecTestItems.clear();
    m_bFailStop = GetConfigValue(L"Param:GPIO:FailStop", 0);
    m_nGroupSize = GetConfigValue(L"Param:GPIO:GroupCount", 0);

    LPCWSTR lpGpioName[AFCB_MAX_GPIO_NUMBER] =
    {
        L"GPIO_A",
        L"GPIO_B",
        L"GPIO_C",
        L"GPIO_D",
        L"IO"
    };
    m_eType = (GpioTestType)GetComboxValue(lpTestName, 2, L"Param:GPIO:TestType");
    int nTemp = GetConfigValue(L"Param:GPIO:IsGPIOEx", 0);
    wstring sBase = L"Param:GPIO:";
    for (int i = 0; i < m_nGroupSize; i++)
    {
        TGroupGpioParam tGroup;
        ZeroMemory(&tGroup, sizeof(tGroup));
        wchar_t    szGroup[20] = { 0 };
        swprintf_s(szGroup, L"Group%d:", i + 1);
        wstring sTemp = sBase + szGroup;
        wstring sPath = sTemp + L"Phone_GPIO";
        tGroup.byPhoneGpioNo = (BYTE)GetConfigValue(sPath.c_str(), 0);
        sPath = sTemp + L"G3_PORT";
        tGroup.m_byGPIOX = (AFCB_GPIO_NUMBER)GetComboxValue(lpGpioName, AFCB_MAX_GPIO_NUMBER, sPath.c_str());
        sPath = sTemp + L"G3_GPIO";
        tGroup.byAfcbGpioNo = (BYTE)GetConfigValue(sPath.c_str(), 0);
        tGroup.bIsGPIOEx = nTemp;
        sPath = sTemp + L"SET_LEVEL";
        tGroup.bySetLevel = (BYTE)GetConfigValue(sPath.c_str(), 0);
        m_vecTestItems.push_back(tGroup);
    }
    BuildSteps();
    return TRUE;
}

void CImpGpioAfcb::BuildSteps()
{
    size_t nVecSize = m_vecTestItems.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        CGpioBase* Step = nullptr;
        switch (m_eType)
        {
        case PhoneToG3:
        {
            Step = new CGpioGet(m_vecTestItems[i], this);
            break;
        }
        case G3ToPhone:
        {
            Step = new CGpioSet(m_vecTestItems[i], this);
            break;
        }
        default:
        break;
        }
        if (nullptr != Step)
        {
            m_vecSteps.push_back(Step);
        }
    }
}

SPRESULT CImpGpioAfcb::GpioAction()
{
    SPRESULT res = SP_OK;
    if (2 != m_eType)
    {
        LogFmtStrW(SPLOGLV_INFO, L"Test Type: %s", lpTestName[m_eType]);
    }
    CHKRESULT(GpioInit());
    size_t nVecSize = m_vecSteps.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        if (m_bFailStop)
        {
            CHKRESULT(m_vecSteps[i]->Run());
        }
        else
        {
            res |= m_vecSteps[i]->Run();
        }
    }

    return res;
}

SPRESULT CImpGpioAfcb::__FinalAction()
{
    size_t nVecSize = m_vecSteps.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        if (NULL != m_vecSteps[i])
        {
            delete m_vecSteps[i];
            m_vecSteps[i] = NULL;
        }
    }
    m_vecSteps.clear();
    return SP_OK;
}

SPRESULT CImpGpioAfcb::GpioInit()
{
    LPCSTR lpTemp = "G3: GPIOA_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOA, true), lpTemp);
    lpTemp = "G3: GPIOB_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOB, true), lpTemp);
    lpTemp = "G3: GPIOC_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOC, true), lpTemp);
    lpTemp = "G3: GPIOD_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOD, true), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 17 0";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(17, 0), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 19 0";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(19, 0), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 21 0";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(21, 0), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 23 0";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(23, 0), lpTemp);

    lpTemp = "G3: FUNCTION_IO_ENABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Function_IO_SetStatus_X(true), lpTemp);
    if (m_eType == G3ToPhone)
    {
        lpTemp = "G3: GPIO_DIRECTION_SET 17 1";
        RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(17, 1), lpTemp);
        lpTemp = "G3: GPIO_DIRECTION_SET 19 1";
        RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(19, 1), lpTemp);
        lpTemp = "G3: GPIO_DIRECTION_SET 21 1";
        RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(21, 1), lpTemp);
        lpTemp = "G3: GPIO_DIRECTION_SET 23 1";
        RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(23, 1), lpTemp);
    }
    return SP_OK;
}

SPRESULT CImpGpioAfcb::GpioReset()
{
    LPCSTR lpTemp = "G3: GPIO_DIRECTION_SET 17 0";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(17, 0), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 19 0";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(19, 0), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 21 0";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(21, 0), lpTemp);
    lpTemp = "G3: GPIO_DIRECTION_SET 23 0";
    RETURNSPRESULT(m_pObjFunBox->FB_GPIO_SetDirection_X(23, 0), lpTemp);
    lpTemp = "G3: GPIOA_DISABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOA, false), lpTemp);
    lpTemp = "G3: GPIOB_DISABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOB, false), lpTemp);
    lpTemp = "G3: GPIOC_DISABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOC, false), lpTemp);
    lpTemp = "G3: GPIOD_DISABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Gpio_OE_Set_X(FB_GPIOD, false), lpTemp);
    lpTemp = "G3: FUNCTION_IO_DISABLE";
    RETURNSPRESULT(m_pObjFunBox->FB_Function_IO_SetStatus_X(false), lpTemp);
    return SP_OK;
}