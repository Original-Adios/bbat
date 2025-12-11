#include "StdAfx.h"
#include "InitPhase.h"
#include "ActionApp.h"
#include "PhaseCheckExport.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"

//
IMPLEMENT_RUNTIME_CLASS(CInitPhase)

//////////////////////////////////////////////////////////////////////////
CInitPhase::CInitPhase(void)
{
}

CInitPhase::~CInitPhase(void)
{
}

SPRESULT CInitPhase::__PollAction(void)
{ 
    extern CActionApp myApp;
    std::wstring strDllpath = (std::wstring)(_T2CW(myApp.GetAppPath())) + L"\\..\\PhaseCheck.dll";
    HMODULE hLib = LoadLibraryExW(strDllpath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == hLib)
    {
        LogFmtStrW(SPLOGLV_ERROR, L"LoadLibrary <%s> fail, WinErr = %d",strDllpath.c_str(), ::GetLastError());
        NOTIFY("LoadLibrary", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "PhaseCheck.dll");
        return SP_E_LOAD_LIBRARY;
    }

    pfInitPhase InitPhase = (pfInitPhase)GetProcAddress(hLib, "InitPhase");
    if (NULL == InitPhase)
    {
        LogFmtStrA(SPLOGLV_ERROR, "GetProcAddress <InitPhase> fail, WinErr = %d",::GetLastError());
        FreeLibrary(hLib);
        NOTIFY("LoadLibrary", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "PhaseCheck.dll");
        return SP_E_LOAD_LIBRARY;
    }

    CHAR* lpSN1 = NULL;
    CHAR* lpSN2 = NULL;
    INPUT_CODES_T InputCodes[BC_MAX_NUM];
    SPRESULT res = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&InputCodes, sizeof(InputCodes));
    if (SP_OK == res)
    {
        if (InputCodes[BC_SN1].bEnable)
        {
            lpSN1 = &InputCodes[BC_SN1].szCode[0];
        }

        if (InputCodes[BC_SN2].bEnable)
        {
            lpSN2 = &InputCodes[BC_SN2].szCode[0];
        }
    }
    
    PHASECHECK_RESULT e = InitPhase(m_hDUT, (LPCSTR)lpSN1, (LPCSTR)lpSN2);
    FreeLibrary(hLib);
    InitPhase = NULL;

    switch(e)
    {
    case PHASECHECK_PASS:
        NOTIFY("InitPhase", LEVEL_UI, 1, 1, 1, NULL, -1);
        res = SP_OK;
        break;
    case PHASECHECK_INVALID_CONFIG_FILE:
        NOTIFY("InitPhase", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Invalid PhaseCheck.ini");
        res = SP_E_PHASECHECK_INVALID_CONFIGURATION_FILE;
        break;
    case PHASHCHECK_SAVE_FAIL:
        NOTIFY("InitPhase", LEVEL_UI, 1, 0, 1, NULL, -1, NULL);
        res = SP_E_PHASECHECK_STATION_UPDATE_FAIL;
        break;
    default:
        NOTIFY("InitPhase", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Unknown");
        res = SP_E_PHASECHECK_STATION_UPDATE_FAIL;
        break;
    }

    return res;
}
