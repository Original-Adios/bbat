#include "StdAfx.h"
#include "LoadWidevineDeviceID.h"
#include "ActionApp.h"
#include "LogFile.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

IMPLEMENT_RUNTIME_CLASS(CLoadWidevineDeviceID)

CLoadWidevineDeviceID::CLoadWidevineDeviceID(void)
{
}

CLoadWidevineDeviceID::~CLoadWidevineDeviceID(void)
{    
}

BOOL CLoadWidevineDeviceID::LoadXMLConfig(void)
{
    m_strFilePath = _T("");

    if (1 == GetConfigValue(L"Option:SaveFile", 1))
    {
        LPCWSTR lpszPath = GetConfigValue(L"Option:FilePath", L"");
        if (NULL != lpszPath && wcslen(lpszPath) > 0)
        {
            if (PathIsRelativeW(lpszPath))
            {
                extern CActionApp myApp;
                m_strFilePath = (string_t)myApp.GetAppPath() + _T("\\") + _W2CT(lpszPath);
            }
            else
            {
                m_strFilePath = _W2CT(lpszPath);
            }

#if defined (UNICODE) ||  defined (_UNICODE)
            replace_all_w(m_strFilePath, L"/", L"\\");
#else
            replace_all(m_strFilePath, "/", "\\");
#endif
            CONST INT SIZE = m_strFilePath.length() + 1;
            LPTSTR pszFile = new TCHAR[SIZE];
            _tcscpy_s(pszFile,  SIZE, m_strFilePath.c_str());
            PathCanonicalize(pszFile, m_strFilePath.c_str());  // Replace .  or ... in path
            m_strFilePath = pszFile;
            delete []pszFile;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

SPRESULT CLoadWidevineDeviceID::__PollAction(void)
{
    CHAR szID[32] = {0};
    SPRESULT res = SP_GetWidevineKeyboxDeviceID(m_hDUT, szID, sizeof(szID));
    if (SP_OK != res)
    {
        NOTIFY("GetWidevineKeyboxDeviceID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return res;
    }

    CONST INT SIZE = m_strFilePath.length() + 1;
    LPTSTR  pszDir = new TCHAR[SIZE];
    memset( pszDir, 0x00, SIZE );
    _tcscpy_s(pszDir, SIZE, m_strFilePath.c_str());
    PathRemoveFileSpec(pszDir);
    CreateMultiDirectory(pszDir, NULL);
    delete []pszDir;

    CLogFile file;
    if (!file.Open(m_strFilePath.c_str(), OPEN_ALWAYS))
    {
        NOTIFY("OpenFile", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "File: %s", _T2CA(m_strFilePath.c_str()));
        return SP_E_OPEN_FILE;
    }

    LARGE_INTEGER li;
    li.QuadPart = 0;
    if (!file.GetFileSize(li))
    {
        file.Close();
        return SP_E_FILE_IO;
    }

    // SN1, Widevine Keybox Device ID
    // 123456789012345,73e38eb4f313e4fce8a5ab547cc7e2c0
    if (0 == li.QuadPart)
    {
        LPCSTR HEADER = "SN1,Widevine Keybox Device ID\r\n";
        file.Write(HEADER, strlen(HEADER));
    }

    std::string strLine = "";
    CHAR SN[ShareMemory_SN_SIZE] = {0};
    if (SP_OK == GetShareMemory(ShareMemory_SN1, (LPVOID)&SN[0], sizeof(SN)))
    {
        strLine = (std::string)SN + "," + szID + "\r\n";
    }
    else
    {
        strLine = (std::string)" ," + szID + "\r\n";
    }

    file.Write(strLine.c_str(), strLine.length());
    file.Close();

    NOTIFY("GetWidevineKeyboxDeviceID", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, szID);
    return SP_OK;
}