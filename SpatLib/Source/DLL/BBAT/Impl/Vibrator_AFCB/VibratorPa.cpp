#include "StdAfx.h"
#include "VibratorAfcb.h"
#include "VibratorPa.h"




//
IMPLEMENT_RUNTIME_CLASS(CVibratorPa)

//////////////////////////////////////////////////////////////////////////
CVibratorPa::CVibratorPa(void)
{

}

CVibratorPa::~CVibratorPa(void)
{

}

SPRESULT CVibratorPa::VibAction()
{
    SPRESULT Res = SP_OK;
    CHKRESULT(VibratorOpen());
    Sleep(1000);
    Res = AFCB_PowerCurrentGet(&m_fOpenCurrent);
    CHKRESULT(VibratorClose());
    Sleep(1000);
    Res = AFCB_PowerCurrentGet(&m_fCloseCurrent);
    CHKRESULT(GetResult());

    return Res;
}

SPRESULT CVibratorPa::AFCB_PowerCurrentGet(float* pfCurrentf)
{
    RETURNSPRESULT(m_pObjFunBox->FB_Power_Supply_GetCurrent_X(pfCurrentf), "G3: 4V Current Get");
    _UiSendMsg("Power Current", LEVEL_REPORT, NOLOWLMT, *pfCurrentf, NOUPPLMT, nullptr, -1, "mA");

    return SP_OK;
}

SPRESULT CVibratorPa::GetResult()
{
    float fDeltaCurrent = m_fOpenCurrent - m_fCloseCurrent;
    _UiSendMsg("Power Current", LEVEL_ITEM,
        m_dDownSpec, (double)fDeltaCurrent, m_dUpSpec,
        nullptr, -1, nullptr,
        "Delta Current = %.2f", fDeltaCurrent);

    if (!IN_RANGE(m_dDownSpec, (double)fDeltaCurrent, m_dUpSpec))
    {
        return SP_E_BBAT_CURRENT_VALUE_FAIL;
    }

    return SP_OK;
}
