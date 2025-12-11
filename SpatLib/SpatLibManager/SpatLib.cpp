#include "StdAfx.h"
#include "SpatLib.h"
#include "MemoryCounter.h"

///
extern LPCSTR g_sczModule;
#define SAFE_LOG_PTR    if (NULL != m_pLogUtil)  m_pLogUtil
//////////////////////////////////////////////////////////////////////////
CSpatLib::CSpatLib(LPCWSTR lpFilePath, LPCWSTR lpFileName, ISpLog* pLogUtil)
    : m_hDLL(NULL)
    , m_pLogUtil(pLogUtil)
{
    if (NULL == lpFilePath || NULL == lpFileName)
    {
        assert(0);
    }
    else
    {
        wcsncpy_s(m_LibInfo.szLibaryPath, lpFilePath, MAX_PATH - 1);
        wcsncpy_s(m_LibInfo.szLibaryName, lpFileName, MAX_PATH - 1);
    }
}

CSpatLib::~CSpatLib(void)
{
    Free();
}

BOOL CSpatLib::Load(void)
{
    LPCWSTR lpFilePath = m_LibInfo.szLibaryPath;
    LPCWSTR lpFileName = m_LibInfo.szLibaryName;

    COUNT_MEMORY1(m_pLogUtil, lpFileName);

    m_hDLL = LoadLibraryExW(lpFilePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == m_hDLL)
    {
        SAFE_LOG_PTR->LogFmtStrW(g_sczModule, SPLOGLV_ERROR, L"Load %s fail! WinErr = 0x%X", lpFilePath, ::GetLastError());
        return FALSE;
    }

    LPSPAT_EXPORT_FUNCS lpFn = &m_LibInfo.Fn;
    BOOL bLoadres = FALSE;
    for ( ; ; )
    {
        lpFn->Startup       = (LPSpat_Startup)      GetProcAddress(m_hDLL, "Spat_Startup"     );
        lpFn->Cleanup       = (LPSpat_Cleanup)      GetProcAddress(m_hDLL, "Spat_Cleanup"     );
        lpFn->CreateAction  = (LPSpat_CreateAction) GetProcAddress(m_hDLL, "Spat_CreateAction");
        lpFn->FreeAction    = (LPSpat_FreeAction)   GetProcAddress(m_hDLL, "Spat_FreeAction"  );
        lpFn->SetDriver     = (LPSpat_SetDriver)    GetProcAddress(m_hDLL, "Spat_SetDriver"   );
        lpFn->InitAction    = (LPSpat_InitAction)   GetProcAddress(m_hDLL, "Spat_InitAction"  );
        lpFn->EnterAction   = (LPSpat_EnterAction)  GetProcAddress(m_hDLL, "Spat_EnterAction" );
        lpFn->PollAction    = (LPSpat_PollAction)   GetProcAddress(m_hDLL, "Spat_PollAction"  );
        lpFn->FinalAction   = (LPSpat_FinalAction)  GetProcAddress(m_hDLL, "Spat_FinalAction" );
        lpFn->LeaveAction   = (LPSpat_LeaveAction)  GetProcAddress(m_hDLL, "Spat_LeaveAction" );
        lpFn->SetProperty   = (LPSpat_SetProperty)  GetProcAddress(m_hDLL, "Spat_SetProperty" );
        lpFn->GetProperty   = (LPSpat_GetProperty)  GetProcAddress(m_hDLL, "Spat_GetProperty" );

        if (!lpFn->operator bool())
        {
            lpFn->Initialize();
            SAFE_LOG_PTR->LogFmtStrW(g_sczModule, SPLOGLV_ERROR, L"Load %s failed!", lpFilePath);
        }
        else
        {
            bLoadres = (SP_OK == lpFn->Startup(&m_LibInfo.lpData)) ?  TRUE : FALSE;
            if (    bLoadres 
                && (NULL != m_LibInfo.lpData && NULL != m_LibInfo.lpData->lpFileVersion)
                )
            {
                SAFE_LOG_PTR->LogFmtStrW(g_sczModule, SPLOGLV_INFO, L"%s,\t Version:%s, Vendor: %s", \
                    lpFileName, m_LibInfo.lpData->lpFileVersion, 
                    (NULL != m_LibInfo.lpData->lpVendorInfo) ? m_LibInfo.lpData->lpVendorInfo : L"Unknown"
                    );
            }
            else
            {
                SAFE_LOG_PTR->LogFmtStrW(g_sczModule, SPLOGLV_WARN, L"Invalid library : %s!", lpFilePath);
            }
        }

        break;
    }

    if (!bLoadres)
    {
        Free();
    }

    return bLoadres;
}

void CSpatLib::Free(void)
{ 
    if (IsLoaded())
    {
        if (m_LibInfo.Fn.Cleanup != NULL)
        {
            m_LibInfo.Fn.Cleanup();
            m_LibInfo.lpData = NULL;
            m_LibInfo.Fn.Initialize();
        }

        FreeLibrary(m_hDLL);
        m_hDLL = NULL;

        SAFE_LOG_PTR->LogFmtStrW(g_sczModule, SPLOGLV_INFO, L"%s is free.", m_LibInfo.szLibaryName);
    }
}

LPCSPATLIB_INFO CSpatLib::GetLibInfo(void) 
{
    if (!IsLoaded() && !Load())
    {
        return NULL;
    }

    return &m_LibInfo;
}