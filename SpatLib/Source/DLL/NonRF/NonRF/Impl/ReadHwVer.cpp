#include "StdAfx.h"
#include "ReadHwVer.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CReadHwVer)

///
CReadHwVer::CReadHwVer(void)
{
}

CReadHwVer::~CReadHwVer(void)
{
}

BOOL CReadHwVer::LoadXMLConfig(void)
{
    m_HwVer.bLoad   = GetConfigValue(L"Option:Load", 1);
    m_HwVer.bVerify = GetConfigValue(L"Option:Verify", 1);
    m_HwVer.uExpVer = GetConfigValue(L"Option:ExpectedVersion", 0);

    return TRUE;
}

SPRESULT CReadHwVer::__PollAction(void)
{ 
    READ_NV_PARAM_RLT_HW_VERSION HwVersion;
    if (m_HwVer.bLoad)
    {
        CHKRESULT(SP_ReadHWVersion(m_hDUT, &HwVersion));

        NOTIFY(DUT_INFO_HW, HwVersion.HWVersion);
        
        if (m_HwVer.bVerify)
        {
            if (m_HwVer.uExpVer != HwVersion.HWVersion)
            {
                NOTIFY("HW Version VERIFY", LEVEL_ITEM|LEVEL_INFO, 1, 0, 1, NULL, -1, NULL, to_string(m_HwVer.uExpVer).c_str());
                return SP_E_SPAT_INVALID_DATA;
            }
            else
            {
                NOTIFY("HW Version VERIFY", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, to_string(m_HwVer.uExpVer).c_str());
            }
        }
        else
        {
            NOTIFY("HW Version", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, to_string(m_HwVer.uExpVer).c_str());
        }
    }

    return SP_OK;
}
