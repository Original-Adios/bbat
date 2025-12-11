#pragma once
#include "global_def.h"
#include "LogFile.h"

//
class CSpatBase;
//////////////////////////////////////////////////////////////////////////
class CExtraLog
{
public:
    CExtraLog(void);
    virtual ~CExtraLog(void);

    BOOL    Open(CSpatBase* pParent, LPCTSTR lpszLogName, DWORD dwCreateFlags = CREATE_ALWAYS, DWORD dwMaxCacheSize = 20*KBytes);
    void    Close(void);

    BOOL    Read (LPVOID  lpBuffer, DWORD dwNbrOfBytesToRead, LPDWORD lpdwNbrOfBytesRead);
    BOOL    Write(LPCVOID lpBuffer, DWORD dwNbrOfBytesToWrite);

    BOOL    WriteStringA(LPCSTR  lpszFmt, ...);
    BOOL    WriteStringW(LPCWSTR lpszFmt, ...);

    LPCTSTR GetFilePath(void) { return m_File.GetFilePath(); };
    LPCTSTR GetFileName(void) { return m_File.GetFileName(); };

private:
    CLogFile  m_File;

#if defined (UNICODE) ||  defined (_UNICODE)
#define WriteString     WriteStringW
#else
#define WriteString     WriteStringA
#endif
};


