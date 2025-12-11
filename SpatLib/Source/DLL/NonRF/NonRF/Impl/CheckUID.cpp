#include "StdAfx.h"
#include "CheckUID.h"
#include "ShareMemoryDefine.h"
#include "ExtraLogFile.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckUID)

///
CCheckUID::CCheckUID(void)
{
}

CCheckUID::~CCheckUID(void)
{
}

BOOL CCheckUID::LoadXMLConfig(void)
{
    m_strUIDFile = GetConfigValue(L"Option:SaveUID", L".\\Log\\UID.txt");

    return TRUE;
}

SPRESULT CCheckUID::__PollAction(void)
{
    CHAR szUID[256] = {0};
    SPRESULT res = SP_GetUID_V2(m_hDUT, szUID, sizeof(szUID));
    if (SP_OK != res)
    {
        NOTIFY("UID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load fail");
        return res;
    }
    
    /*
    <<- 41(0x00000029) Bytes
    00000000h: 01 00 00 00 29 00 05 32 54 30 59 42 39 39 5F 32 : ....)..2T0YB99_2
    00000010h: 35 5F 32 33 5F 34 33 0A 0A 00 00 00 00 00 00 00 : 5_23_43.........
    00000020h: 00 00 00 00 00 00 00 00 00                      : .........    
    */
    std::string strUID = szUID;
    replace_all(strUID, "\r", "");
    replace_all(strUID, "\n", "");

    // Bug875168
    if (!m_strUIDFile.empty())
    {
        SaveToFile(strUID.c_str());
    }

    // Save into ShareMemory
    // LotID(6)_waferID(2)_×ø±êX(2)_×ø±ê(2)
    CHKRESULT(SetShareMemory(ShareMemory_UID,  (const void* )strUID.c_str(), ShareMemory_UID_SIZE)); 

    NOTIFY("UID", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, "%s", strUID.c_str());
    NOTIFY(DUT_INFO_UID, szUID);
    return SP_OK;
}

BOOL CCheckUID::SaveToFile(const char* lpszUID)
{
    if (NULL == lpszUID)
    {
      //  assert(0);
        return FALSE;
    }

    if (::PathIsRelativeW(m_strUIDFile.c_str()))
    {
        WCHAR szAppPath[MAX_PATH] = {0};
        GetModuleFileNameW(NULL, szAppPath, MAX_PATH);
        LPWSTR lpChar = wcsrchr(szAppPath, L'\\');
        if (NULL != lpChar)
        {
            *(lpChar + 1) = L'\0';
        }
        wcscat_s(szAppPath, m_strUIDFile.c_str());
        m_strUIDFile = szAppPath;
    }
    
    CExtraLog txt;
    txt.Open(this, _W2CT(m_strUIDFile.c_str()));
    txt.WriteStringA(lpszUID);
    txt.Close();

    return TRUE;
}