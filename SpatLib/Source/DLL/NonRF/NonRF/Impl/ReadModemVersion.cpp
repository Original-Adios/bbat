#include "StdAfx.h"
#include "ReadModemVersion.h"
#include <tchar.h>

//
IMPLEMENT_RUNTIME_CLASS(CReadModemVersion)

//////////////////////////////////////////////////////////////////////////
CReadModemVersion::CReadModemVersion(void)
{
}

CReadModemVersion::~CReadModemVersion(void)
{
}

BOOL CReadModemVersion::LoadXMLConfig(void)
{

    // TODO: Load the configuration of XXXX.XML specified by IDR_XML_XXXX defined in <ActionList.h> 
    std::wstring strModemVersion;
    strModemVersion = GetConfigValue(L"Option:ModemVersion", L"");

    if (strModemVersion == L"")
    {
        LogFmtStrW(SPLOGLV_ERROR, L"Init Compare Version is NULL, Check SEQ!");
        return FALSE;
    }

    std::vector<std::wstring> arrModemVersion = ws_split(strModemVersion, L"[.]");

    if (arrModemVersion.size() < 1)
    {
        LogFmtStrW(SPLOGLV_ERROR, L"UI GetVersion Fail!");
        return FALSE;
    }

    uint8 nMajorVersion = _wtoi(arrModemVersion[0].c_str());
    uint8 nMinorVersion = _wtoi(arrModemVersion[1].c_str());
    m_u16ModemVersion = ( nMinorVersion << 8 ) | nMajorVersion;
    LogFmtStrW(SPLOGLV_INFO, L"UI GetVersion:0x%x", m_u16ModemVersion);

    return TRUE;
}


SPRESULT CReadModemVersion::__PollAction(void)
{ 

    READ_NV_PARAM_RLT_MODEM_VERSION nvVersion;

    SPRESULT sPRESULT = SP_ReadModemVersion(m_hDUT, &nvVersion);

    if (sPRESULT != SP_OK) 
    {
        LogFmtStrW(SPLOGLV_INFO, L"ReadModemVersion Fail!");
        return SP_E_SPAT_TEST_FAIL;

    }

    if (nvVersion.ModemVersion != m_u16ModemVersion)
    {
        LogFmtStrW(SPLOGLV_INFO, L"version=%x", nvVersion.ModemVersion);
        NOTIFY("ReadModermVersion", LEVEL_UI | LEVEL_REPORT, 0.0, 1.0, 0.0, NULL, -1, "-", "modemversion=%x", nvVersion.ModemVersion);
        return SP_E_SPAT_TEST_FAIL;
    }
    NOTIFY("ReadModermVersion", LEVEL_UI,1.0,1.0,1.0,NULL, -1, "-" , "modemversion=%x", nvVersion.ModemVersion);
    return SP_OK;
}
