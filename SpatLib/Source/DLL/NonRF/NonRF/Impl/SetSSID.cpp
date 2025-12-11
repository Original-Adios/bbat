#include "StdAfx.h"
#include "SetSSID.h"
#include "ShareMemoryDefine.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSetSSID)

///
CSetSSID::CSetSSID(void)
{
}

CSetSSID::~CSetSSID(void)
{
    
}

BOOL CSetSSID::LoadXMLConfig(void)
{
    m_strSSID = _W2CA(GetConfigValue(L"Option:SSID", L""));
    if (m_strSSID.length() > MAX_AP_SSID_LEN)
    {
        return FALSE;
    }
    m_strPSK = _W2CA(GetConfigValue(L"Option:PSK", L""));
    if (m_strPSK.length() > MAX_AP_PSK_LEN)
    {
        return FALSE;
    }

    return TRUE;
}

SPRESULT CSetSSID::__PollAction(void)
{
    CHAR szSSID[MAX_AP_SSID_LEN + 1] = {0};
    if (SP_OK != GetShareMemory(ShareMemory_SSID, szSSID, MAX_AP_SSID_LEN))
    {
        memcpy(szSSID, m_strSSID.c_str(), m_strSSID.length());
    }
    if (0 == strlen(szSSID))
    {
        NOTIFY("InvalidSSID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return SP_E_SPAT_INVALID_AP_SSID_NAME;
    }

    CHAR szPSK[MAX_AP_PSK_LEN + 1] = {0};
    if (SP_OK != GetShareMemory(ShareMemory_PSK, szPSK, MAX_AP_PSK_LEN))
    {
        memcpy(szPSK, m_strPSK.c_str(), m_strPSK.length());
    }
    size_t nPskLen = strlen(szPSK);
    if (nPskLen > 0 && nPskLen < 8)  // PSK maybe null.
    {
        NOTIFY("InvalidPSK", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return SP_E_SPAT_INVALID_AP_PSK;
    }

	SPRESULT res = SP_apSetupSSID(m_hDUT, szSSID, szPSK);
    NOTIFY("SetSSID", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "SSID: %s; PSK: %s", szSSID, szPSK);
    return res;
}
