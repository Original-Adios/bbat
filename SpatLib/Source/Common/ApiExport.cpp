#include "StdAfx.h"
#include "ActionApp.h"


#ifdef _VLD_
/// http://vld.codeplex.com/
    #include "vld.h"
#endif

///

CActionApp myApp;
HMODULE g_hModule  = NULL;
LPCSTR  g_szModule = "SpatLib"; 

#ifdef __AFX_H__ 
// MFC 
#else
TCHAR  g_szProcessDir[MAX_PATH] = { 0 };
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            g_hModule = hModule;
            GetModuleFileName(NULL, g_szProcessDir, MAX_PATH);
            TCHAR* pPos = _tcsrchr(g_szProcessDir, _T('\\'));
            if (NULL != pPos)
            {
                *pPos = _T('\0');
            }
        }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif 

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT SPRESULT SP_API Spat_Startup(
    SPAT_DATA** lppData 
    )
{
    return myApp.Startup(g_hModule, lppData);   
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT void SP_API Spat_Cleanup(void)
{
    myApp.Cleanup();
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT SP_HANDLE SP_API Spat_CreateAction(
    LPCWSTR  lpFuncName,
    LPCWSTR  lpClassName,
    LPVOID    pLogUtil,
    SPRESULT* pResCode
    )
{
    SPRESULT resCode = SP_OK;
    return myApp.CreateAction(lpFuncName, lpClassName, pLogUtil, (NULL == pResCode) ? resCode : *pResCode);
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT void SP_API Spat_FreeAction(
    SP_HANDLE hAction
    )
{
    myApp.FreeAction(hAction);
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT SPRESULT SP_API Spat_SetDriver(
    SP_HANDLE hAction, 
    INT nDriv/*SPAT_DRIVER*/, 
    LPVOID pDriv, 
    LPVOID pParam
    )
{
    return myApp.SetDriver(hAction, nDriv, pDriv, pParam);
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT SPRESULT SP_API Spat_InitAction(
    SP_HANDLE hAction, 
    LPCSPAT_INIT_PARAM pParam
    )
{
    return myApp.InitAction(hAction, pParam);
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT void SP_API Spat_EnterAction(
    SP_HANDLE hAction
    )
{
    myApp.EnterAction(hAction);
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT SPRESULT SP_API Spat_PollAction(
    SP_HANDLE hAction
    )
{
    return myApp.PollAction(hAction);
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT void SP_API Spat_LeaveAction(
    SP_HANDLE hAction
    )
{
    myApp.LeaveAction(hAction);
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT SPRESULT SP_API Spat_FinalAction(
    SP_HANDLE hAction
    )
{
    return myApp.FinalAction(hAction);
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT SPRESULT SP_API Spat_SetProperty(
    SP_HANDLE hAction, 
    INT nFlags, 
    INT nOption, 
    LPCVOID lpValue
    )
{
    return myApp.SetProperty(hAction, nFlags, nOption, lpValue);
}

//////////////////////////////////////////////////////////////////////////
SPATLIB_EXPORT SPRESULT SP_API Spat_GetProperty(
    SP_HANDLE hAction, 
    INT nFlags, 
    INT nOption, 
    LPVOID lpValue
    )
{
    return myApp.GetProperty(hAction, nFlags, nOption, lpValue);
}
