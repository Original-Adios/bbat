#include "StdAfx.h"
#include "NoMicHeadset.h"

//
IMPLEMENT_RUNTIME_CLASS(CNoMicHeadset)

CNoMicHeadset::CNoMicHeadset(void)
{

}
CNoMicHeadset::~CNoMicHeadset(void)
{
}

SPRESULT CNoMicHeadset::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return HeadsetAction();
}

BOOL CNoMicHeadset::LoadXMLConfig(void)
{
    m_dSnrSpec = GetConfigValue(L"Option:SNR", 0.0);
    m_dPeakSpec = GetConfigValue(L"Option:PEAK", 0.0);
    m_bSnr8k = GetConfigValue(L"Option:Snr8K", 0);
    m_bFP = GetConfigValue(L"Option:FeaturePhone", 0);
    return TRUE;
}

SPRESULT CNoMicHeadset::HeadsetAction()
{
    CHKRESULT(AFCB_Headmic(NC));
    CHKRESULT(Phone_CheckHeadset());
    CHKRESULT(Phone_HeadsetOpen());
    CHKRESULT(RunHeadset_LR());
    CHKRESULT(Phone_HeadsetClose());
    return SP_OK;
}

SPRESULT CNoMicHeadset::AFCB_Headmic(HEADSET_TYPE nType)
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


SPRESULT CNoMicHeadset::Phone_HeadsetMicOpen()
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

SPRESULT CNoMicHeadset::Phone_HeadsetOpen()
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


SPRESULT CNoMicHeadset::Phone_HeadsetMicClose()
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

SPRESULT CNoMicHeadset::AFCB_HeadSet_LR(HEADSET_LR eHeadset)
{
    LPCSTR lpTemp[MAX_LR] =
    {
        "G3: Headset L",
        "G3: Headset R"
    };

    RETURNSPRESULT(m_pObjFunBox->FB_HeadSet_LR_Select_X(eHeadset), lpTemp[eHeadset]);
    return SP_OK;
}

SPRESULT CNoMicHeadset::Phone_HeadsetMicCheck()
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

SPRESULT CNoMicHeadset::Phone_HeadsetMicRead()
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

SPRESULT CNoMicHeadset::Phone_HeadsetClose()
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

SPRESULT CNoMicHeadset::Phone_CheckHeadset()
{
    HeadsetTypeMic RecvType = NoHeadset;
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_CheckHeadset(m_hDUT, &RecvType), "Phone: Check Headset", LEVEL_ITEM));

    _UiSendMsg("Phone: Headset Type", LEVEL_ITEM,
        0, RecvType, 0,
        0, -1, 0, "Type = %d, Spec = 0", RecvType);

    if (RecvType != 0)
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}


SPRESULT CNoMicHeadset::RunHeadset_LR()
{
    CHKRESULT(AFCB_HeadSet_LR(HEADSET_L));
    CHKRESULT(RunMic());
    CHKRESULT(AFCB_HeadSet_LR(HEADSET_R));
    CHKRESULT(RunMic());
    return SP_OK;
}


SPRESULT CNoMicHeadset::RunMic()
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
