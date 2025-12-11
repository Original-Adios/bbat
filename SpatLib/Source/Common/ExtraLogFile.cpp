#include "StdAfx.h"
#include "ExtraLogFile.h"
#include "SpatBase.h"
#include "ActionApp.h"
#include "Utility.h"
#include <assert.h>
#include <vector>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

extern CActionApp myApp;
//////////////////////////////////////////////////////////////////////////
CExtraLog::CExtraLog(void)
{
}

CExtraLog::~CExtraLog(void) 
{
}

BOOL CExtraLog::Open(CSpatBase* pParent, LPCTSTR lpszLogName, DWORD dwCreateFlags /* = CREATE_ALWAYS */, DWORD dwMaxCacheSize /* = 1MBytes */)
{
    if (NULL == lpszLogName || NULL == pParent)
    {
        assert(0);
        return FALSE;
    }

    string_t strLogPath = _T("");
    if (!::PathIsRelative(lpszLogName))
    {
        strLogPath = lpszLogName;
    }
    else
    {
        TCHAR szLogPath[MAX_PATH] = {0};
        ISpLog* pLog = pParent->GetISpLogObject();
        if (NULL != pLog)
        {
            WCHAR szPath[MAX_PATH] = {0};
            pLog->GetProperty(LogProp_LogFilePath, MAX_PATH, (LPVOID)szPath);
            if (PathFileExistsW(szPath))
            {
                LPWSTR lpChar = wcsrchr(szPath, L'\\');
                if (NULL != lpChar)
                {
                    *lpChar = L'\0';
                }
#if defined (UNICODE) ||  defined (_UNICODE)
                wcscpy_s(szLogPath, szPath);
#else
                WideCharToMultiByte(CP_ACP, 0, szPath, -1, szLogPath, MAX_PATH, 0, 0);
#endif 
            }
            else
            {
                _stprintf_s(szLogPath, _T("%s\\Log"), myApp.GetAppPath());
            }
        }
        else
        {
            _stprintf_s(szLogPath, _T("%s\\Log"), myApp.GetAppPath());
        }

        strLogPath = (string_t)szLogPath + _T("\\") + lpszLogName;
    }

    TCHAR szPath[MAX_PATH] = {0};
    _tcsncpy_s(szPath, strLogPath.c_str(), MAX_PATH-1);
    ::PathRemoveFileSpec(szPath);
    pParent->CreateMultiDirectory(szPath, NULL);

    return m_File.Open(strLogPath.c_str(), dwCreateFlags, 0, dwMaxCacheSize);
}

void CExtraLog::Close(void)
{
    m_File.Close();
}

BOOL CExtraLog::Read(LPVOID lpBuffer, DWORD dwNbrOfBytesToRead, LPDWORD lpdwNbrOfBytesRead)
{
    return m_File.Read(lpBuffer, dwNbrOfBytesToRead, lpdwNbrOfBytesRead);
}

BOOL CExtraLog::Write(LPCVOID lpBuffer, DWORD dwNbrOfBytesToWrite)
{
    return m_File.Write(lpBuffer, dwNbrOfBytesToWrite);
}

BOOL CExtraLog::WriteStringA(LPCSTR lpszFmt, ...)
{
    va_list  args = NULL;  
    va_start(args, lpszFmt); 
    const size_t nLength = _vscprintf(lpszFmt, args) + 3/*\r\n\0*/;   
    std::vector<CHAR> vBuf(nLength);
    if (_vsnprintf_s(&vBuf[0], vBuf.size(), nLength, lpszFmt, args) <= 0) 
    {
        va_end(args);
        return FALSE;
    }
    
    va_end(args);

    vBuf[nLength - 3] = '\r';
    vBuf[nLength - 2] = '\n';
    vBuf[nLength - 1] = '\0';

    return Write(&vBuf[0], strlen(&vBuf[0]));
}

BOOL CExtraLog::WriteStringW(LPCWSTR lpszFmt, ...)
{
    va_list  args = NULL;  
    va_start(args, lpszFmt); 
    const size_t nLength = _vscwprintf(lpszFmt, args) + 3/*\r\n\0*/;   
    std::vector<WCHAR> vBuf(nLength);
    if (_vsnwprintf_s(&vBuf[0], vBuf.size(), nLength, lpszFmt, args) <= 0) 
    {
        va_end(args);
        return FALSE;
    }

    va_end(args);

    vBuf[nLength - 3] = L'\r';
    vBuf[nLength - 2] = L'\n';
    vBuf[nLength - 1] = L'\0';

    CUtility util;
    LPCSTR lpString = util._W2CA(&vBuf[0]);
    if (NULL == lpString)
    {
        return FALSE;
    }
    else
    {
        return Write(lpString, strlen(lpString));
    }
}