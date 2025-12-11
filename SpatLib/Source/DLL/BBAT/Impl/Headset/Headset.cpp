#include "StdAfx.h"
#include "Headset.h"

//
IMPLEMENT_RUNTIME_CLASS(CHeadset)

CHeadset::CHeadset(void)
{

}
CHeadset::~CHeadset(void)
{
}

SPRESULT CHeadset::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return HeadsetAction();
}

BOOL CHeadset::LoadXMLConfig(void)
{
    m_dSnrSpec = GetConfigValue(L"Option:SNR", 0.0);
    m_dPeakSpec = GetConfigValue(L"Option:PEAK", 0.0);
    m_bSnr8k = GetConfigValue(L"Option:Snr8K", 0);
    m_dVoltageUp = GetConfigValue(L"Option:Voltage:UpSpec", 0.0);
    m_dVoltageDown = GetConfigValue(L"Option:Voltage:DownSpec", 0.0);
    m_bFP = GetConfigValue(L"Option:FeaturePhone", 0);

    LPCWSTR lpwHeadsetName[MAX_TYPE_HEADSET] =
    {
        L"Dummy",
        L"EUROPE",
        L"AMERICA",
    };

    m_eHeadsetType = (HEADSET_TYPE)GetComboxValue(lpwHeadsetName, MAX_TYPE_HEADSET, L"Option:Headset");

    LPCWSTR lpwMicName[MAX_HEADSET_TYPE] =
    {
        L"NoHeadset",
        L"WithMic",
        L"WithoutMic"
    };
    m_nHeadsetMicSpec = (HeadsetTypeMic)GetComboxValue(lpwMicName, MAX_HEADSET_TYPE, L"Option:HeadsetMic");
    if (m_eHeadsetType == MAX_TYPE_HEADSET || m_nHeadsetMicSpec == MAX_HEADSET_TYPE)
    {
        return FALSE;
    }

    return TRUE;
}

SPRESULT CHeadset::HeadsetAction()
{
    CHKRESULT(AFCB_Headmic(m_eHeadsetType));
    CHKRESULT(Phone_CheckHeadset());
    if (HeadsetWithMic == m_nHeadsetMicSpec)
    {
        CHKRESULT(RunWithoutAdc());
    }
    else
    {
        CHKRESULT(RunWithAdc());
    }

    return SP_OK;
}

SPRESULT CHeadset::AFCB_Headmic(HEADSET_TYPE nType)
{
    LPCSTR lpName[MAX_TYPE_HEADSET] =
    {
        "",
        "G3: Select Europe",
        "G3: Select America",
    };
    RETURNSPRESULT(m_pObjFunBox->FB_HeadMic_Ctrl_Select_X(nType), lpName[nType]);
    return SP_OK;
}

SPRESULT CHeadset::AFCB_AdcEnable(BOOL bStatus)
{
    LPCSTR lpTemp = "G3: ADC Enable";
    if (!bStatus)
    {
        lpTemp = "G3: ADC Disable";
    }
    RETURNSPRESULT(m_pObjFunBox->FB_Common_ADC_En_Set_X(bStatus), lpTemp);
    return SP_OK;
}

SPRESULT CHeadset::Phone_HeadsetMicOpen()
{
    Sleep(500);
    if (m_bFP)
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_FP_MicOpen(m_hDUT, HeadSetMic),
            "Phone: FP Headset Mic Open", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "FP Phone: FP Headset Mic Open", LEVEL_ITEM);
    }
    else
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_MicOpen(m_hDUT, HeadSetMic),
            "Phone: Headset- Mic Open", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: Headset Mic Open", LEVEL_ITEM);
    }

    return SP_OK;
}

SPRESULT CHeadset::Phone_HeadsetOpen()
{
    int arrSpkData[1024] = { 0 };
    GenerateSpeakerData(arrSpkData);
    if (m_bFP)
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_FP_SpeakerPlayByDataSend(m_hDUT, HeadSet, arrSpkData),
            "Phone: FP Headset Open", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: FP Headset Open", LEVEL_ITEM);
    }
    else
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_SpeakerPlayByDataSend(m_hDUT, HeadSet, arrSpkData),
            "Phone: Headset Open", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: Headset Open", LEVEL_ITEM);
    }

    return SP_OK;
}

SPRESULT CHeadset::AFCB_AdcGetVoltage()
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

SPRESULT CHeadset::Phone_HeadsetMicClose()
{
    if (m_bFP)
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_FP_MicClose(m_hDUT, HeadSetMic),
            "Phone: FP Headset Mic Close", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: FP Headset Mic Close", LEVEL_ITEM);
    }
    else
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_MicClose(m_hDUT, HeadSetMic),
            "Phone: HeadsetMic Close", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: HeadsetMic Close", LEVEL_ITEM);
    }

    return SP_OK;
}

SPRESULT CHeadset::AFCB_HeadSet_LR(HEADSET_LR eHeadset)
{
    LPCSTR lpTemp[MAX_LR] = 
    {
        "G3: Headset L",
        "G3: Headset R"
    };

    RETURNSPRESULT(m_pObjFunBox->FB_HeadSet_LR_Select_X(eHeadset), lpTemp[eHeadset]);
    return SP_OK;
}

SPRESULT CHeadset::Phone_HeadsetMicCheck()
{
    Sleep(1500);
    if (m_bFP)
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_FP_MicReadStatus(m_hDUT, HeadSetMic),
            "Phone: FP Headset Mic Check", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: FP Headset Mic Check", LEVEL_ITEM);
    }
    else
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_MicReadStatus(m_hDUT, HeadSetMic),
            "Phone: Headset Mic Check", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: Headset Mic Check", LEVEL_ITEM);
    }
    

    return SP_OK;
}

SPRESULT CHeadset::Phone_HeadsetMicRead()
{
    BYTE arrDataRecv[1600] = { 0 };
    if(m_bFP)
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_FP_MicReadData(m_hDUT, HeadSetMic, arrDataRecv),
            "Phone: FP Headset Mic Read", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: FP Headset Mic Read", LEVEL_ITEM);
    }
    else
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_MicReadData(m_hDUT, HeadSetMic, arrDataRecv),
            "Phone: Headset Mic Read", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: Headset Mic Read", LEVEL_ITEM);
    }
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

SPRESULT CHeadset::Phone_HeadsetClose()
{
    if (m_bFP)
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_FP_SpeakerClose(m_hDUT, HeadSet),
            "Phone: FP Headset Close", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: FP Headset Close", LEVEL_ITEM);
    }
    else
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_SpeakerClose(m_hDUT, HeadSet),
            "Phone: Headset Close", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: Headset Close", LEVEL_ITEM);
    }

    return SP_OK;
}

SPRESULT CHeadset::Phone_CheckHeadset()
{
    LPCSTR lpTemp[MAX_HEADSET_TYPE] =
    {
        "No Headset",
        "Headset With Mic",
        "Headset Without Mic"
    };

    if (m_nHeadsetMicSpec > MAX_TYPE_HEADSET)
    {
        PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "Wrong HeadSet Id", LEVEL_ITEM);
        return SP_E_BBAT_VALUE_FAIL;
    }
    HeadsetTypeMic RecvType = NoHeadset;
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_CheckHeadset(m_hDUT, &RecvType), "Phone: Check Headset", LEVEL_ITEM));

    _UiSendMsg("Phone: Headset Type", LEVEL_ITEM,
        m_nHeadsetMicSpec, RecvType, m_nHeadsetMicSpec,
        0, -1, 0, "Type Detected = %s, Type Setting = %s", lpTemp[RecvType], lpTemp[m_nHeadsetMicSpec]);

    if (RecvType != (HeadsetTypeMic)m_nHeadsetMicSpec)
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CHeadset::RunHeadset_LR()
{
    CHKRESULT(AFCB_HeadSet_LR(HEADSET_L));
    CHKRESULT(RunMic());
    CHKRESULT(AFCB_HeadSet_LR(HEADSET_R));
    CHKRESULT(RunMic());
    return SP_OK;
}

SPRESULT CHeadset::RunWithAdc()
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
        RETRY_PRINT(i,"Retry Mic Read")
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

SPRESULT CHeadset::RunWithoutAdc()
{
    CHKRESULT(Phone_HeadsetOpen());
    Sleep(100);
    CHKRESULT(RunHeadset_LR());
    CHKRESULT(Phone_HeadsetClose());
    return SP_OK;
}

SPRESULT CHeadset::RunMic()
{
    Sleep(500);
    CHKRESULT(Phone_HeadsetMicOpen());
    Sleep(500);
    CHKRESULT(Phone_HeadsetMicCheck());
    SPRESULT Res = SP_OK;
    for (int i = 0; i < 3; i++)
    {
        if (i != 0)
        {
            PrintSuccessMsg(SP_OK, "Retry Mic Read", LEVEL_ITEM);
        }
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
