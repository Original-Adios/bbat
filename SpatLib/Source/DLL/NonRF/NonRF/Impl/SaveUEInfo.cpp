#include "StdAfx.h"
#include "SaveUEInfo.h"
#include "ShareMemoryDefine.h"
#include "ActionApp.h"
#include "LogFile.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSaveUEInfo)

///
CSaveUEInfo::CSaveUEInfo(void)
{
}

CSaveUEInfo::~CSaveUEInfo(void)
{    
}

BOOL CSaveUEInfo::LoadXMLConfig(void)
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

SPRESULT CSaveUEInfo::__PollAction()
{
    if (0 == m_strFilePath.length())
    {
        return SP_OK;
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

    if (0 == li.QuadPart)
    {
        LPCSTR HEADER = "SN1,IMEI1,ICCID,IMSI,EID,UID,DATE\r\n";
        file.Write(HEADER, strlen(HEADER));
    }

    std::string strLine = "";
    CHAR szSN[ShareMemory_SN_SIZE] = {0};
    if (SP_OK == GetShareMemory(ShareMemory_SN1, (LPVOID)&szSN[0], sizeof(szSN)))
    {
        strLine = strLine + "'" + szSN + ",";
    }
    else
    {
        strLine = strLine + ",";
    }

    CHAR szIMEI[ShareMemory_IMEI_SIZE] = {0};
    if (SP_OK == GetShareMemory(ShareMemory_IMEI1, (LPVOID)&szIMEI[0], sizeof(szIMEI)))
    {
        strLine = strLine + "'" + szIMEI + ",";
    }
    else
    {
        strLine = strLine + ",";
    }

    CHAR szICCID[ShareMemory_ICCID_SIZE] = {0};
    if (SP_OK == GetShareMemory(ShareMemory_ICCID, (LPVOID)&szICCID[0], sizeof(szICCID)))
    {
        strLine = strLine + "'" + szICCID + ",";
    }
    else
    {
        strLine = strLine + ",";
    }

    CHAR szIMSI[ShareMemory_IMSI_SIZE] = {0};
    if (SP_OK == GetShareMemory(ShareMemory_IMSI, (LPVOID)&szIMSI[0], sizeof(szIMSI)))
    {
        strLine = strLine + "'" + szIMSI + ",";
    }
    else
    {
        strLine = strLine + "'" + ",";
    }

    CHAR szEID[ShareMemory_EID_SIZE] = {0};
    if (SP_OK == GetShareMemory(ShareMemory_EID, (LPVOID)&szEID[0], sizeof(szEID)))
    {
        strLine = strLine + "'" + szEID + ",";
    }
    else
    {
        strLine = strLine + ",";
    }

    CHAR szUID[ShareMemory_UID_SIZE] = {0};
    if (SP_OK == GetShareMemory(ShareMemory_UID, (LPVOID)&szUID[0], sizeof(szUID)))
    {
        strLine = strLine + szUID + ",";
    }
    else
    {
        strLine = strLine + ",";
    }

    CHAR szDate[64] = {0};
    SYSTEMTIME    t;
    GetLocalTime(&t);
    sprintf_s(szDate, "'%04d-%02d-%02d %02d:%02d:%02d:%03d\r\n", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
    strLine = strLine + szDate;
    file.Write(strLine.c_str(), strLine.length());
    file.Close();

    return SP_OK;
}
