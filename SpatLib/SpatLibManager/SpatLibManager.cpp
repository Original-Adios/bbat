#include "StdAfx.h"
#include "SpatLibManager.h"
#include "AppVer.h"
#include "MemoryCounter.h"
#include <string>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//
extern LPCSTR g_sczModule;
extern TCHAR  g_szIniPath[MAX_PATH];

//////////////////////////////////////////////////////////////////////////
CSpatLibManager::CSpatLibManager(void)
: m_pLogUtil(NULL)
{
}

CSpatLibManager::~CSpatLibManager(void)
{
}

void CSpatLibManager::InitLog(LPVOID pLogUtil)
{
    UINT nLv = GetPrivateProfileInt(_T("Log"), _T("Level"), SPLOGLV_ERROR, g_szIniPath);
    trInit((ISpLog* )pLogUtil, g_sczModule, nLv);
    m_pLogUtil = GetISpLogObject();
    extern CAppVer theVer;
    LogFmtStrW(SPLOGLV_INFO, L"%s,\t Version:%s, Vendor: %s", theVer.GetFileName(), theVer.GetVersion(), theVer.GetVendor());
}

void CSpatLibManager::Release(void)
{
    Free();
    trFree();
    m_pLogUtil = NULL;

    delete this;
}

void CSpatLibManager::Free(void)
{
    COUNT_MEMORY0(m_pLogUtil);

    for (size_t i=0; i<m_vecLibs.size(); i++)
    {
        CSpatLib* pDLL = m_vecLibs[i];
        if (NULL != pDLL)
        {
            pDLL->Free();
            delete pDLL;
        }
        else
        {
            assert(0);
        }
    }

    m_vecLibs.clear();
}

BOOL CSpatLibManager::Load(LPCWSTR lpszLibDirPath)
{
    COUNT_MEMORY0(m_pLogUtil);

    Free();

    if (NULL == lpszLibDirPath)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid SpatLib directory path (NULL)!");
        assert(0);
        return FALSE;
    }

    return SearchLoad(lpszLibDirPath);
}

CSpatLib* CSpatLibManager::Validate(LPCWSTR lpszLibPath, LPCWSTR lpszLibName)
{
    if (NULL == lpszLibPath || NULL == lpszLibName)
    {
        assert(0);
        return NULL;
    } 

    CSpatLib* pDLL = NULL;

    try
    {
        pDLL = new CSpatLib(lpszLibPath, lpszLibName, m_pLogUtil);
    }
    catch (const std::bad_alloc& /*e*/)
    {
        pDLL = NULL;
    }
    
    if (NULL == pDLL)
    {
        LogFmtStrW(SPLOGLV_ERROR, L"Create SpatLib (%s) object failed!", lpszLibName);
        assert(0);
        return NULL;
    }
    
    if (!pDLL->Load())
    {
        delete pDLL;
        return NULL;
    }
    else
    {
        pDLL->Free();  // Free to decrease memory consumption
        return pDLL;
    }
}

BOOL CSpatLibManager::SearchLoad(LPCWSTR lpszDirPath)
{
    if (NULL == lpszDirPath)
    {
        assert(0);
        return FALSE;
    }

    WIN32_FIND_DATA wfd;  
    std::wstring dir_path = (std::wstring)lpszDirPath + L"\\*.*";
    HANDLE hFind = ::FindFirstFileW(dir_path.c_str(), &wfd);  
    if (INVALID_HANDLE_VALUE == hFind)  
    {  
        LogFmtStrA(SPLOGLV_ERROR, "FindFirstFileW(%s) fails, WinErr = %d", dir_path.c_str(), ::GetLastError());
        return FALSE;
    }  
    
    do
    {  
        if (L'.' == wfd.cFileName[0])  
        {  
            continue;  
        }  

        std::wstring file_path = (std::wstring)lpszDirPath + L"\\" + (std::wstring)wfd.cFileName;
        if (FILE_ATTRIBUTE_DIRECTORY == (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  
        {  
            SearchLoad(file_path.c_str()); 
        }  
        else  
        {  
            LPCWSTR lpFileExt = ::PathFindExtensionW(&wfd.cFileName[0]);
            if (0 == _wcsicmp(lpFileExt, L".dll"))
            {
                CSpatLib* pDLL = Validate(file_path.c_str(), &wfd.cFileName[0]);
                if (NULL != pDLL)
                {
                    m_vecLibs.push_back(pDLL);
                }
            }
        }  

    } while (::FindNextFileW(hFind, &wfd));

    ::FindClose(hFind);  
    return TRUE; 
}

LPCSPATLIB_INFO CSpatLibManager::GetLibInfo(LPCWSTR lpszLibName)const
{
    if (NULL == lpszLibName)
    {
        assert(0);
        return NULL;
    }

    for (size_t i=0; i<m_vecLibs.size(); i++)
    {
        CSpatLib* pDLL = m_vecLibs[i];
        if (NULL != pDLL)
        {
            if (0 == _wcsicmp(lpszLibName, pDLL->GetLibName()))
            {
                return pDLL->GetLibInfo();
            }
        }
        else
        {
            assert(0);
        }  
    }

    if (NULL != m_pLogUtil)
    {
        m_pLogUtil->LogFmtStrW(g_sczModule, SPLOGLV_ERROR, L"%s is not exist or invalid library!", lpszLibName);
    }

    return NULL;
}

LPCSPATLIB_INFO CSpatLibManager::GetLibInfo(UINT32 u32Index)const
{
    if (u32Index < m_vecLibs.size())
    {
        return m_vecLibs[u32Index]->GetLibInfo();
    }
    else
    {
        if (NULL != m_pLogUtil)
        {
            m_pLogUtil->LogFmtStrW(g_sczModule, SPLOGLV_ERROR, L"Invalid library index: %d!", u32Index);
        }

        return NULL;
    }
}