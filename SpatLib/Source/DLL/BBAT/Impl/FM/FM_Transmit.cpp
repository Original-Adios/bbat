#include "StdAfx.h"
#include "FM_Transmit.h"

//
IMPLEMENT_RUNTIME_CLASS(CFm_Transmit)

//////////////////////////////////////////////////////////////////////////
CFm_Transmit::CFm_Transmit(void)
{
}

CFm_Transmit::~CFm_Transmit(void)
{
}

SPRESULT CFm_Transmit::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return FM_TransmitAction();
}

void CFm_Transmit::__LeaveAction(void)
{
    Phone_FMTransClose();
}

SPRESULT CFm_Transmit::FM_TransmitAction()
{
    SPRESULT Res = SP_OK;
    CHKRESULT(AFCB_HeadMicModeSelect());
    CHKRESULT(Phone_FMTransOpen());
    Sleep(m_nSleepTime);
    CHKRESULT(AFCB_MainMicAmpEnable(false));
    CHKRESULT(AFCB_HeadSetLRSelect());
    Sleep(500);
    int i = 0;
    for (i; i < 3; i++)
    {
        Res = AFCB_WaveGetData();
        if (SP_OK == Res)
        {
            break;
        }
    }
    if (3 <= i)
    {
        return Res;
    }

    Sleep(100);
    CHKRESULT(AFCB_WaveSetStatus(false));

    return Res;
}

BOOL CFm_Transmit::LoadXMLConfig(void)
{

    m_u16Freq1 = (uint16)(GetConfigValue(L"Option:Freq1", 0.0) * 10);
    m_u16Freq2 = (uint16)(GetConfigValue(L"Option:Freq2", 0.0) * 10);
    m_u16Freq3 = (uint16)(GetConfigValue(L"Option:Freq3", 0.0) * 10);
    m_u16Freq4 = (uint16)(GetConfigValue(L"Option:Freq4", 0.0) * 10);
    m_dSNRSpec = GetConfigValue(L"Option:SNR", 0.0);
    m_dPeakSpec = GetConfigValue(L"Option:Peak", 0.0);
    m_nSleepTime = GetConfigValue(L"Option:SleepTime_ms", 0);
	return TRUE;
}

SPRESULT CFm_Transmit::AFCB_HeadMicModeSelect()
{
    RETURNSPRESULT(m_pObjFunBox->FB_HeadMic_Ctrl_Select_X((int)HeadSet_Mic_NC), "G3:HEADMIC_NC");//0:HeadSet_Mic_Off; 1:HeadSet_Mic_Europe; 2:HeadSet_Mic_American; 3:HeadSet_Mic_NC;
    return SP_OK;
}

SPRESULT CFm_Transmit::Phone_FMTransOpen()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_FmTransmitOpen(m_hDUT, m_u16Freq1), "FM Transmit Open Failed", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "FM Transmit Open", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CFm_Transmit::AFCB_HeadSetLRSelect()
{
    RETURNSPRESULT(m_pObjFunBox->FB_HeadSet_LR_Select_X((int)HeadSet_Left_Channel), "G3:Left Channel To Headset Mic");//0:HeadSet_Left_Channel; 1:HeadSet_Right_Channel
    return SP_OK;
}


SPRESULT CFm_Transmit::Phone_FMTransClose()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_FmTransmitClose(m_hDUT), "FM Transmit Close", LEVEL_ITEM));
	return SP_OK;
}

SPRESULT CFm_Transmit::AFCB_MainMicAmpEnable(bool bStatus)
{
    LPCSTR lpMainMic[2] =
    {
        "G3: Main Mic Amp Close",
        "G3: Main Mic Amp Open"
    };
    RETURNSPRESULT(m_pObjFunBox->FB_Main_Mic_Amp_EN(bStatus), lpMainMic[bStatus]);
    return SP_OK;
}

SPRESULT CFm_Transmit::AFCB_WaveGetData()
{
    SPRESULT Res = SP_OK;
    double dSNRValue = 0.0;
    double dPeakValue = 0.0;
    RETURNSPRESULT(m_pObjFunBox->FB_Common_ADC_En_Set_X(1), "G3: ADC Enable");
    unsigned short arrRecv_Value[1024] = { 0 };
    RETURNSPRESULT(m_pObjFunBox->FB_ADC_Wave_Get_Data((int)Head_Mic, arrRecv_Value), "G3: Wave Get Data");

    Res = AnalysisG3Audio(arrRecv_Value, Head_Mic, dSNRValue, dPeakValue);
    _UiSendMsg("G3: ADC Wave", LEVEL_ITEM, m_dSNRSpec, dSNRValue, NOUPPLMT, 0, -1, 0, "Ch%d: SNR:%0.2lf", Head_Mic, dSNRValue);
    _UiSendMsg("G3: ADC Wave", LEVEL_ITEM, m_dPeakSpec, dPeakValue, NOUPPLMT, 0, -1, 0, "Ch%d: Peak:%0.2lf(%0.2lfmV)", Head_Mic, dPeakValue, dPeakValue*3300/4096);
    if ((dSNRValue < m_dSNRSpec) || (dPeakValue < m_dPeakSpec))
    {
        Res = SP_E_BBAT_VALUE_FAIL;
    }

    RETURNSPRESULT(m_pObjFunBox->FB_Common_ADC_En_Set_X(0), "G3: ADC Disable");
    return Res;
}

SPRESULT CFm_Transmit::AFCB_WaveSetStatus(bool bstatus)
{
    LPCSTR lpName[2] =
    {
        "G3:ADC Wave Status Set Flase",
        "G3:ADC Wave Status Set True"
    };
    RETURNSPRESULT(m_pObjFunBox->FB_ADC_Wave_Set_Status(bstatus, Head_Mic), lpName[bstatus]);
    return SP_OK;

}
