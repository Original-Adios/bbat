#include "Stdafx.h"
#include "AudioLoopPa.h"

CAudioLoopPa::CAudioLoopPa(TGroupAudioLoop tGroup, CImpBase* pImp)
    :CAudioLoopBase(tGroup, pImp)
{

}

CAudioLoopPa::~CAudioLoopPa(void)
{
}

SPRESULT CAudioLoopPa::Run(void)
{
    SetParameter();
    //speaker 发送成功则一定能跑到 speaker close
    SPRESULT Res = SP_OK;
    CHKRESULT(AFCB_PA_Enable(true));
    CHKRESULT(AFCB_AudioLoopSelect(true));
    CHKRESULT(Phone_SpkPlayByDataSend());
    Sleep(500);
    Res = MicAction();

    CHKRESULT(Phone_SpkClose());
    CHKRESULT(AFCB_PA_Enable(false));
    CHKRESULT(AFCB_AudioLoopSelect(false));

    return Res;
}

SPRESULT CAudioLoopPa::AFCB_PA_Enable(bool bStatus)
{
    if (m_ePaId == PA0)
    {
        CHKRESULT(AFCB_PA0_Enable(bStatus));
    }
    else if(m_ePaId == PA1)
    {
        CHKRESULT(AFCB_PA1_Enable(bStatus));
    }
    else
    {
        return SP_E_BBAT_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CAudioLoopPa::MicAction()
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

SPRESULT CAudioLoopPa::MicReadAction()
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

    return SP_OK;
}


