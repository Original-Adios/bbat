#include "Stdafx.h"
#include "AudioLoopAfcb.h"



CAudioLoopAfcb::CAudioLoopAfcb(TGroupAudioLoop tGroup, CImpBase* pImp)
    :CAudioLoopBase(tGroup, pImp)
{

}

CAudioLoopAfcb::~CAudioLoopAfcb(void)
{
}
SPRESULT CAudioLoopAfcb::Run()
{
    SetParameter();

    //speaker 发送成功则一定能跑到 speaker close
    SPRESULT Res = SP_OK;
    if (HeadSet == m_bySpkId)
    {
        CHKRESULT(AFCB_AmpEnable(false));
    }
    else
    {
        CHKRESULT(AFCB_AmpEnable(true));
    }
    CHKRESULT(AFCB_AudioLoopSelect(true));
    CHKRESULT(Phone_SpkPlayByDataSend());
    Sleep(500);
    Res = MicAction();

    CHKRESULT(Phone_SpkClose());
    CHKRESULT(AFCB_AmpEnable(false));
    CHKRESULT(AFCB_AudioLoopSelect(false));

    return Res;

}

SPRESULT CAudioLoopAfcb::AFCB_VoltageGetN()
{
    float fVoltage = 0.0;
    if (m_byMicId == MainMic)
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Main_Mic_N_GetVoltage_X(&fVoltage), "G3: Main Mic N Voltage Get");
    }
    else
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Sub_Mic_N_GetVoltage_X(&fVoltage), "G3: Sub Mic N Voltage Get");
    }

    m_pImp->_UiSendMsg("Voltage", LEVEL_ITEM,
        0, fVoltage, m_dVoltageUpN,
        nullptr, -1, nullptr);

    if (!IN_RANGE(0, fVoltage, m_dVoltageUpN))
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CAudioLoopAfcb::AFCB_VoltageGetP()
{
    float fVoltage = 0.0;
    if (m_byMicId == MainMic)
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Main_Mic_P_GetVoltage_X(&fVoltage), "G3: Main Mic P Voltage Get");
    }
    else
    {
        RETURNSPRESULT_IMP(m_pImp, m_pImp->m_pObjFunBox->FB_Sub_Mic_P_GetVoltage_X(&fVoltage), "G3: Sub Mic P Voltage Get");
    }

    m_pImp->_UiSendMsg("Voltage", LEVEL_ITEM,
        m_dVoltageDownP, fVoltage, 3300,
        nullptr, -1, nullptr);

    if (!IN_RANGE(m_dVoltageDownP, fVoltage, 3300))
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}


SPRESULT CAudioLoopAfcb::MicAction()
{
    //mic open成功则一定能跑到mic close
    SPRESULT Res = SP_OK;
    CHKRESULT(Phone_MicOpen());
    Sleep(1500);
    CHKRESULT(Phone_MicReadStatus());
    Res = MicReadAction();
    Sleep(1500);
    CHKRESULT(Phone_MicClose());
    return Res;
}

SPRESULT CAudioLoopAfcb::MicReadAction()
{
    SPRESULT Res = SP_OK;
    for (int i = 0; i < 3; i++)
    {
        if (i != 0)
        {
            m_pImp->PrintSuccessMsg(SP_OK, "Retry Mic Read", LEVEL_ITEM);
        }
        Res = Phone_MicReadData();
        Sleep(100);
        if (Res == SP_OK)
        {
            break;
        }
        Sleep(1000);
    }
    if (Res != SP_OK)
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    CHKRESULT(AFCB_VoltageGetN());
    CHKRESULT(AFCB_VoltageGetP());
    return SP_OK;
}


