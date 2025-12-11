#include "StdAfx.h"
#include "CheckAllStation.h"
#include "ActionApp.h"

//
IMPLEMENT_RUNTIME_CLASS(CCheckAllStation)

//////////////////////////////////////////////////////////////////////////
CCheckAllStation::CCheckAllStation(void)
: m_bAutoMode(TRUE)
{
}

CCheckAllStation::~CCheckAllStation(void)
{
}

BOOL CCheckAllStation::LoadXMLConfig(void)
{
    std::wstring strValue = GetConfigValue(L"Option:Station:Mode", L"Automatically");
    m_bAutoMode = (0 == strValue.compare(L"Automatically")) ? TRUE : FALSE;
    if (!m_bAutoMode)
    {
        for (int i=0; i<MAX_STATION_NUM; i++)
        {
            WCHAR szKey[32] = {0};
            swprintf_s(szKey, L"Option:Station:Station%02d", i + 1);
            strValue = GetConfigValue(szKey, L"");
            trimW(strValue);
            if (0 == strValue.length())
            {
                m_arrStationName[i].clear();
            }
            else
            {
                m_arrStationName[i] = _W2CA(strValue.c_str());
            }
        }
    }
    
    return TRUE;
}

SPRESULT CCheckAllStation::__PollAction(void)
{ 
    //  @JXP 20180413: 进模式/CheckPreStation失败不更新站位
    BOOL IsUpdate = FALSE;
    SetShareMemory(InternalReservedShareMemory_IsUpdateStation, (LPCVOID)&IsUpdate, 4);

    extern CActionApp myApp;
    std::wstring strDllpath = (std::wstring)(_T2CW(myApp.GetAppPath())) + L"\\..\\PhaseCheck.dll";
    HMODULE hLib = LoadLibraryExW(strDllpath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == hLib)
    {
        LogFmtStrW(SPLOGLV_ERROR, L"LoadLibrary <%s> fail, WinErr = %d",strDllpath.c_str(), ::GetLastError());
        NOTIFY("LoadLibrary", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "PhaseCheck.dll");
        return makeResultCode(MODULE_SPATLIB, SP_E_LOAD_LIBRARY);
    }

    pfCheckPhase CheckPhase = (pfCheckPhase)GetProcAddress(hLib, "CheckPhase");
    pfCheckAllStations CheckAllStations = (pfCheckAllStations)GetProcAddress(hLib, "CheckAllStations");
    if (NULL == CheckPhase || NULL == CheckAllStations)
    {
        LogFmtStrA(SPLOGLV_ERROR, "GetProcAddress <CheckPhase/CheckAllStations> fail, WinErr = %d",::GetLastError());
        FreeLibrary(hLib);
        NOTIFY("LoadLibrary", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "PhaseCheck.dll");
        return makeResultCode(MODULE_SPATLIB, SP_E_LOAD_LIBRARY);
    }

    BOOL  bAllPass = FALSE;
    STATION_RESULTS result;
    if (m_bAutoMode)
    {
        PHASECHECK_RESULT e = (*CheckAllStations)(m_hDUT, &bAllPass, &result);
        FreeLibrary(hLib);
        if (PHASECHECK_PASS != e)
        {
            return ParseResultAndNotify(e, "CheckAllStation", CCheckStation::CHECK);
        }
    }
    else
    {
        bAllPass = TRUE;
        for (int i=0; i<MAX_STATION_NUM; i++)
        {
            if (m_arrStationName[i].empty())
            {
                continue;
            }

            PHASECHECK_RESULT e = (*CheckPhase)(m_hDUT, m_arrStationName[i].c_str());
            if (PHASECHECK_PASS != e)
            {
                bAllPass = FALSE; // If one station fail, then fail
            }

            strncpy_s(result.station[result.count].name, m_arrStationName[i].c_str(), sizeof(result.station[result.count].name) - 1);
            result.station[result.count].result = e;
            result.count++;
        }

        FreeLibrary(hLib);
    }

    SPRESULT res = SP_OK;
    for (UINT32 i=0; i<result.count; i++)
    {
        // Display All
        SPRESULT e = ParseResultAndNotify(result.station[i].result, result.station[i].name, CCheckStation::CHECK);
        if (SP_OK == res && SP_OK != e)
        {
            res = e; // report the 1st error code
        }
    }

    if (bAllPass)
    {
        IsUpdate = TRUE;
        SetShareMemory(InternalReservedShareMemory_IsUpdateStation, (LPCVOID)&IsUpdate, 4);
    }

    return bAllPass ? SP_OK : res;
}
