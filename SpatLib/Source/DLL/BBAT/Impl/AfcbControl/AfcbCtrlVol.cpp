#include "StdAfx.h"
#include "AfcbCtrlVol.h"

//
IMPLEMENT_RUNTIME_CLASS(CAfcbCtrlVol)

//////////////////////////////////////////////////////////////////////////
CAfcbCtrlVol::CAfcbCtrlVol(void)
{

}

CAfcbCtrlVol::~CAfcbCtrlVol(void)
{

}

SPRESULT CAfcbCtrlVol::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    if (m_bExtPower1Enable)
    {
        CHKRESULT(AFCB_Ext1PowerOn(m_bExtPower1Open));
    }
    
    if (m_bExtPower2Enable)
    {
        CHKRESULT(AFCB_Ext2PowerOn(m_bExtPower2Open));
    }

    return SP_OK;
}

BOOL CAfcbCtrlVol::LoadXMLConfig(void)
{
    m_bExtPower1Enable = GetConfigValue(L"Option:ExtPower1:Enable", 0);
    m_bExtPower1Open = GetConfigValue(L"Option:ExtPower1:Open", 0);
    m_bExtPower2Enable = GetConfigValue(L"Option:ExtPower2:Enable", 0);
    m_bExtPower2Open = GetConfigValue(L"Option:ExtPower2:Open", 0);
    return TRUE;
}

SPRESULT CAfcbCtrlVol::AFCB_Ext1PowerOn(bool bStatus)
{
    LPCSTR lpName[2] =
    {
        "G3: EXT1 Power Off",
        "G3: EXT1 Power On",
    };
    RETURNSPRESULT(m_pObjFunBox->FB_EXT1_Power_Ctrl(bStatus), lpName[bStatus]);

    return SP_OK;
}

SPRESULT CAfcbCtrlVol::AFCB_Ext2PowerOn(bool bStatus)
{
    LPCSTR lpName[2] =
    {
        "G3: EXT2 Power Off",
        "G3: EXT2 Power On",
    };
    RETURNSPRESULT(m_pObjFunBox->FB_EXT2_Power_Ctrl(bStatus), lpName[bStatus]);

    return SP_OK;
}
