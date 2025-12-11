#include "StdAfx.h"
#include "UpdateStation.h"
#include "ActionApp.h"
#include "ModeSwitch.h"

//
IMPLEMENT_RUNTIME_CLASS(CUpdateStation)

//////////////////////////////////////////////////////////////////////////
CUpdateStation::CUpdateStation(void)
{
}

CUpdateStation::~CUpdateStation(void)
{
}

SPRESULT CUpdateStation::__PollAction(void)
{ 
    if (0 == m_StationName.size())
    {
        NOTIFY("UpdateStation", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Invalid Station Name");
        return SP_E_PHASECHECK_STATION_NOT_EXIST;
    }

    //  @JXP 20180413: 进模式/CheckPreStation失败不更新站位
    BOOL IsUpdate = FALSE;
    GetShareMemory(InternalReservedShareMemory_IsUpdateStation, (LPVOID)&IsUpdate, 4);
    if (!IsUpdate)
    {
        LogRawStrA(SPLOGLV_WARN, "No need to UpdatePhase, skip!");
        return SP_OK;
    }

    SPRESULT res = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&m_result, sizeof(m_result));
    if (SP_OK != res)
    {
        NOTIFY("GetShareMemory", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "ItemTestResult");
        return res;
    }

    RM_MODE_ENUM eCurrMode = m_pModeSwitch->GetCurrentMode();
    if ((RM_INTERNAL_POWEROFF_MODE == eCurrMode || RM_INTERNAL_DEEPSLEEP_MODE == eCurrMode)
        && !IsFakeDC()
        )
    {
        // 电流测试POWER OFF、DEEPSLEEP需要重启手机
        // 如果没有程控电源，那么重启也会失败，因为DIAG通路已关闭
        CHKRESULT_WITH_NOTIFY(m_pModeSwitch->Reboot(RM_CALIBRATION_MODE), "Reboot");
    }
    
    extern CActionApp myApp;
    std::wstring strDllpath = (std::wstring)(_T2CW(myApp.GetAppPath())) + L"\\..\\PhaseCheck.dll";
    HMODULE hLib = LoadLibraryExW(strDllpath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == hLib)
    {
        LogFmtStrW(SPLOGLV_ERROR, L"LoadLibrary <%s> fail, WinErr = %d",strDllpath.c_str(), ::GetLastError());
        NOTIFY("LoadLibrary", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "PhaseCheck.dll");
        return makeResultCode(MODULE_SPATLIB, SP_E_LOAD_LIBRARY);
    }

    pfUpdatePhase UpdatePhase = (pfUpdatePhase)GetProcAddress(hLib, "UpdatePhase");
    if (NULL == UpdatePhase)
    {
        LogFmtStrA(SPLOGLV_ERROR, "GetProcAddress <UpdatePhase> fail, WinErr = %d",::GetLastError());
        FreeLibrary(hLib);
        NOTIFY("LoadLibrary", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "PhaseCheck.dll");
        return makeResultCode(MODULE_SPATLIB, SP_E_LOAD_LIBRARY);
    }
    
    BOOL bPASS = (SP_OK == m_result.errCode) ? TRUE : FALSE;
    std::string strErrMsg = bPASS ? "PASS" : ((std::string)m_result.szFailItem + " : " + m_result.szDescription);
    PHASECHECK_RESULT e = (*UpdatePhase)(m_hDUT, m_StationName.c_str(), bPASS, strErrMsg.c_str());
    FreeLibrary(hLib);

    return ParseResultAndNotify(e, m_StationName, CCheckStation::UPDATE);
}
