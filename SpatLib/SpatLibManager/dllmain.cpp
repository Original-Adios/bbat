// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "SpatLibManager.h"
#include "AppVer.h"

#ifdef _VLD_
/// http://vld.codeplex.com/
    #include "vld.h"
#endif


TCHAR  g_szIniPath[MAX_PATH]  = {0};
ISpatLibManager* g_lpManager = NULL; 
LPCSTR g_sczModule = "SpatLibMan";
CAppVer theVer;
//////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        {
            theVer.Init(hModule);

            GetModuleFileName(hModule, g_szIniPath, MAX_PATH);
            LPTSTR pPos = _tcsrchr(g_szIniPath, _T('\\'));
            if (NULL != pPos)
            {
                *pPos = _T('\0');
            }
             _tcscat_s(g_szIniPath, _T("\\SpatLibManager.ini"));
        }
        break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
SPLM_EXPORT ISpatLibManager* SP_API CreateSpatLibManager(void)
{
    try
    {
        return (ISpatLibManager* )new CSpatLibManager();
    }
    catch (const std::bad_alloc& /*e*/)
    {
        return NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
SPLM_EXPORT BOOL SP_API SP_LoadSpatLib(PCWSTR lpszLibDirPath)
{
    SP_FreeSpatLib();

    g_lpManager = CreateSpatLibManager();
    if (NULL != g_lpManager)
    {
        return g_lpManager->Load(lpszLibDirPath);
    }
    
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////
SPLM_EXPORT void SP_API SP_FreeSpatLib(void)
{
    if (NULL != g_lpManager)
    {
        g_lpManager->Free();
        g_lpManager->Release();
        g_lpManager = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
SPLM_EXPORT UINT32 SP_API SP_GetSpatLibCount(void)
{
    if (NULL != g_lpManager)
    {
        return  g_lpManager->GetLibCount();
    }
    else
    {
        return 0;
    }
}

//////////////////////////////////////////////////////////////////////////
SPLM_EXPORT LPCSPATLIB_INFO  SP_API SP_GetSpatLibInfoByIndex(UINT32 u32Index)
{
    if (NULL != g_lpManager)
    {
        return  g_lpManager->GetLibInfo(u32Index);
    }
    else
    {
        return NULL;
    }   
}

//////////////////////////////////////////////////////////////////////////
SPLM_EXPORT LPCSPATLIB_INFO  SP_API SP_GetSpatLibInfoByName(LPCWSTR lpszLibName)
{
    if (NULL != g_lpManager)
    {
        return  g_lpManager->GetLibInfo(lpszLibName);
    }
    else
    {
        return NULL;
    }
}

