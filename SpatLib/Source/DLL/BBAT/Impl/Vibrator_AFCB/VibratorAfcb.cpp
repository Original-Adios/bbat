#include "StdAfx.h"
#include "VibratorAfcb.h"

//
IMPLEMENT_RUNTIME_CLASS(CVibratorAfcb)

//////////////////////////////////////////////////////////////////////////
CVibratorAfcb::CVibratorAfcb(void)
{

}

CVibratorAfcb::~CVibratorAfcb(void)
{

}

SPRESULT CVibratorAfcb::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return VibAction();
}

BOOL CVibratorAfcb::LoadXMLConfig(void)
{
    m_bFp = GetConfigValue(L"Option:FP", 0);
    m_dUpSpec = GetConfigValue(L"Option:UpSpec", 0.0);
    m_dDownSpec = GetConfigValue(L"Option:DownSpec", 0.0);
    return TRUE;
}

SPRESULT CVibratorAfcb::VibAction()
{
    CHKRESULT(VibratorOpen());
    Sleep(500);
    SPRESULT Res = GetCurrent();
    CHKRESULT(VibratorClose());
    return Res;
}

SPRESULT CVibratorAfcb::VibratorOpen()
{
    if (m_bFp)
    {
        CHKRESULT(PrintErrorMsg(SP_BBAT_FP_VibratorOpen(m_hDUT),
            "Phone: FP Vibrator Open", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone: FP Vibrator Open", LEVEL_ITEM);
        return SP_OK;
    }
    else
    {
        CHKRESULT(PrintErrorMsg(SP_BBAT_VibratorOpen(m_hDUT),
            "Phone:Vibrator Open", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Phone:Vibrator Open", LEVEL_ITEM);
        return SP_OK;
    }
}

SPRESULT CVibratorAfcb::VibratorClose()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_VibratorClose(m_hDUT),
        "Phone: Vibrator Close", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: Vibrator Close", LEVEL_ITEM);

    return SP_OK;
}

SPRESULT CVibratorAfcb::GetCurrent()
{
    float fCurrent = 0.0f;
    RETURNSPRESULT(m_pObjFunBox->FB_CURRENT_VIB_CTRL_GetCurrent_X(&fCurrent), "G3: Get Current");

    _UiSendMsg("Vib Current", LEVEL_ITEM,
        m_dDownSpec, fCurrent, m_dUpSpec,
        nullptr, -1, nullptr,
        "Current = %.2f", fCurrent);

    if (!IN_RANGE(m_dDownSpec, fCurrent, m_dUpSpec))
    {
        return SP_E_BBAT_VALUE_FAIL;
    }

    return SP_OK;
}
