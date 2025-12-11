#include "StdAfx.h"
#include "TypeC_Headset.h"

//
IMPLEMENT_RUNTIME_CLASS(CTypeCHeadset)

CTypeCHeadset::CTypeCHeadset(void)
{

}
CTypeCHeadset::~CTypeCHeadset(void)
{
}

SPRESULT CTypeCHeadset::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return HeadsetAction();
}

BOOL CTypeCHeadset::LoadXMLConfig(void)
{
    m_dSnrSpec = GetConfigValue(L"Option:SNR", 0.0);
    m_dPeakSpec = GetConfigValue(L"Option:PEAK", 0.0);
    m_bSnr8k = GetConfigValue(L"Option:Snr8K", 0);
    m_dVoltageUp = GetConfigValue(L"Option:Voltage:UpSpec", 0.0);
    m_dVoltageDown = GetConfigValue(L"Option:Voltage:DownSpec", 0.0);
    return TRUE;
}

SPRESULT CTypeCHeadset::HeadsetAction()
{
    CHKRESULT(AFCB_Headmic(EUROPE));
    CHKRESULT(RunWithAdc());
    return SP_OK;
}

SPRESULT CTypeCHeadset::AFCB_Headmic(HEADSET_TYPE nType)
{
    LPCSTR lpName[MAX_HEADSET_TYPE] =
    {
        "G3: Select Europe",
        "G3: Select America",
        "G3: Select NC"
    };
    RETURNSPRESULT(m_pObjFunBox->FB_HeadMic_Ctrl_Select_X(nType), lpName[nType]);
    return SP_OK;
}

SPRESULT CTypeCHeadset::AFCB_AdcEnable(BOOL bStatus)
{
    LPCSTR lpTemp = "G3: ADC Enable";
    if (!bStatus)
    {
        lpTemp = "G3: ADC Disable";
    }
    RETURNSPRESULT(m_pObjFunBox->FB_Common_ADC_En_Set_X(bStatus), lpTemp);
    return SP_OK;
}

SPRESULT CTypeCHeadset::Phone_HeadsetMicOpen()
{
    Sleep(500);
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_MicOpen(m_hDUT, HeadSetMic),
        "Phone: Headset- Mic Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: Headset Mic Open", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CTypeCHeadset::Phone_HeadsetOpen()
{
    int arrSpkData[1024] = { 0 };
    GenerateSpeakerData(arrSpkData);
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_SpeakerPlayByDataSend(m_hDUT, HeadSet, arrSpkData),
        "Phone: Headset Open", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: Headset Open", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CTypeCHeadset::AFCB_AdcGetVoltage()
{
    float fVoltage = 0.0;
    RETURNSPRESULT(m_pObjFunBox->FB_Common_ADC_GetVoltage_X(FB_ADC7, &fVoltage), "G3: ADC7 Get Voltage");

    _UiSendMsg("Voltage Check", LEVEL_ITEM,
        m_dVoltageDown, fVoltage, m_dVoltageUp,
        nullptr, -1, nullptr,
        "Voltage = %.2f", fVoltage);
    if (!IN_RANGE(m_dVoltageDown, fVoltage, m_dVoltageUp))
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CTypeCHeadset::Phone_HeadsetMicClose()
{

    CHKRESULT(PrintErrorMsg(
        SP_BBAT_MicClose(m_hDUT, HeadSetMic),
        "Phone: HeadsetMic Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: HeadsetMic Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CTypeCHeadset::AFCB_HeadSet_LR(HEADSET_LR eHeadset)
{
    LPCSTR lpTemp[MAX_LR] = 
    {
        "G3: Headset L",
        "G3: Headset R"
    };

    RETURNSPRESULT(m_pObjFunBox->FB_HeadSet_LR_Select_X(eHeadset), lpTemp[eHeadset]);
    return SP_OK;
}

SPRESULT CTypeCHeadset::Phone_HeadsetMicCheck()
{
    Sleep(1500);
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_MicReadStatus(m_hDUT, HeadSetMic),
        "Phone: Headset Mic Check", LEVEL_ITEM));

    return SP_OK;
}

SPRESULT CTypeCHeadset::Phone_HeadsetMicRead()
{
    BYTE arrDataRecv[1600] = { 0 };
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_MicReadData(m_hDUT, HeadSetMic, arrDataRecv),
        "Phone: Headset Mic Read", LEVEL_ITEM));

    AudioResult Result;
    CalcMicData(arrDataRecv, m_bSnr8k, Result);
    _UiSendMsg("Phone: Headset Mic Read", LEVEL_ITEM,
        m_dPeakSpec, Result.dPeak, NOUPPLMT,
        nullptr, -1, nullptr,
        "Peak = %.2f", Result.dPeak);

    _UiSendMsg("Phone: Headset Mic Read", LEVEL_ITEM,
        m_dSnrSpec, Result.dSNR, NOUPPLMT,
        nullptr, -1, nullptr,
        "Snr = %.2f", Result.dSNR);

    if (Result.dSNR < m_dSnrSpec || Result.dPeak < m_dPeakSpec)
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CTypeCHeadset::Phone_HeadsetClose()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_SpeakerClose(m_hDUT, HeadSet),
        "Phone: Headset Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: Headset Close", LEVEL_ITEM);
    return SP_OK;
}


SPRESULT CTypeCHeadset::RunHeadset_LR()
{
    CHKRESULT(AFCB_HeadSet_LR(HEADSET_L));
    CHKRESULT(RunMic());
    CHKRESULT(AFCB_HeadSet_LR(HEADSET_R));
    CHKRESULT(RunMic());
    return SP_OK;
}

SPRESULT CTypeCHeadset::RunWithAdc()
{

    CHKRESULT(AFCB_AdcEnable(TRUE));
    Sleep(100);
    CHKRESULT(Phone_HeadsetMicOpen());
    Sleep(100);
    CHKRESULT(Phone_HeadsetOpen());
    SPRESULT Res = SP_OK;
    for (int i = 0; i < 5; i++)
    {
        Sleep(500);
        Res = AFCB_AdcGetVoltage();
        if (Res == SP_OK)
        {
            break;
        }
    }
    CHKRESULT(AFCB_AdcEnable(FALSE));
    CHKRESULT(Phone_HeadsetMicClose());
    if (Res != SP_OK)
    {
        return Res;
    }
    Sleep(100);
    CHKRESULT(RunHeadset_LR());
    CHKRESULT(Phone_HeadsetClose());

    return Res;
}

SPRESULT CTypeCHeadset::RunMic()
{
    Sleep(500);
    CHKRESULT(Phone_HeadsetMicOpen());
    Sleep(500);
    CHKRESULT(Phone_HeadsetMicCheck());
    SPRESULT Res = SP_OK;
    for (int i = 0; i < 3; i++)
    {
        RETRY_PRINT(i, "Retry Mic Read")
        Res = Phone_HeadsetMicRead();
        if (Res == SP_OK)
        {
            break;
        }
        Sleep(1000);
    }
    CHKRESULT(Phone_HeadsetMicClose());
    return Res;
}
