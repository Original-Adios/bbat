#include "StdAfx.h"
#include "CheckUsageCount.h"
#include "CLocks.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//
IMPLEMENT_RUNTIME_CLASS(CCheckUsageCount)
//////////////////////////////////////////////////////////////////////////

// If rename these texts, remember to update UI as well.
LPCSTR CCheckUsageCount::USAGE_INFO[MAX_USAGE] = {"CablePinCount", "FakraCount", "JigCount3"};

///
CCheckUsageCount::CCheckUsageCount(void)
{
}

CCheckUsageCount::~CCheckUsageCount(void)
{
}

BOOL CCheckUsageCount::LoadXMLConfig(void)
{
    m_strFilePath = _T("");
    LPCWSTR lpszPath = GetConfigValue(L"Option:FilePath", L"../../sys/UsageCount.bin");
    if (NULL != lpszPath && wcslen(lpszPath) > 0)
    {
        // Get absolute path
        if (PathIsRelativeW(lpszPath))
        {
            m_strFilePath = (string_t)myApp.GetAppPath() + _T("\\") + _W2CT(lpszPath);
        }
        else
        {
            m_strFilePath = _W2CT(lpszPath);
        }

        // Replace '/' to '\'
#if defined (UNICODE) ||  defined (_UNICODE)
        replace_all_w(m_strFilePath, L"/", L"\\");
#else
        replace_all(m_strFilePath, "/", "\\");
#endif

        // Replace .  or .. with the real path
        CONST INT SIZE = m_strFilePath.length() + 1;
        LPTSTR pszFile = new TCHAR[SIZE];
        memset(pszFile, 0, sizeof(TCHAR)*SIZE);
        _tcscpy_s(pszFile,  SIZE, m_strFilePath.c_str());
        PathCanonicalize(pszFile, m_strFilePath.c_str());  
        m_strFilePath = pszFile;
        delete []pszFile;
    }

    return TRUE;
}

SPRESULT CCheckUsageCount::__PollAction(void)
{
    // Lock for multi-thread, 
    // Only allow one thread access the usage count file at one time.
    CLocks Lock(myApp.GetLock());

    
    CONST LPCTSTR SECT[MAX_USAGE] = {_T("CablePin"), _T("Fakra"), _T("JigCount")};
    CONST LPCSTR  NAME[MAX_USAGE] = {"CheckCablePinCount", "CheckFakraCount", "CheckJigCount"};
    struct USAGE_COUNT 
    {
        BOOL   used;
        INT32  left;
        INT32   max;
    } arrUsages[MAX_USAGE];
    for (INT i=0; i<MAX_USAGE; i++)
    {
        arrUsages[i].used = GetPrivateProfileInt(SECT[i], _T("used"), 0, m_strFilePath.c_str());
        arrUsages[i].left = GetPrivateProfileInt(SECT[i], _T("left"), 0, m_strFilePath.c_str());
        arrUsages[i].max  = GetPrivateProfileInt(SECT[i], _T("max" ), 0, m_strFilePath.c_str());
    }
    /*
        - Check the usage count
        ¡ú If usage count < 0, Stop the test and Alarm
        ¡ú If usage count > 0, decrease usage count
    */
    for (INT i=0; i<MAX_USAGE; i++)
    {
        if (!arrUsages[i].used)
        {
            continue;
        }

        if (arrUsages[i].left <= 0)
        {
            NOTIFY(NAME[i], LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Count = %d", arrUsages[i].left);
            return SP_E_NO_USAGE_COUNT;
        }
        else
        {
            CHAR szCnt[32] = {0};
            sprintf_s(szCnt, "%d", --arrUsages[i].left);
            NOTIFY(USAGE_INFO[i], szCnt);
            NOTIFY(NAME[i], LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Count = %d", arrUsages[i].left);
            
            WritePrivateProfileString(SECT[i], _T("left"), _A2CT(szCnt), m_strFilePath.c_str());
        }
    }
    
    return SP_OK;
}

