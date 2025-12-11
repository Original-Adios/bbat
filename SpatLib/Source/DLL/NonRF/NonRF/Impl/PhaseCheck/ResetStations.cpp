#include "StdAfx.h"
#include "ResetStations.h"
#include "ActionApp.h"
#include "ModeSwitch.h"

//
IMPLEMENT_RUNTIME_CLASS(CResetStations)

//////////////////////////////////////////////////////////////////////////
CResetStations::CResetStations(void)
{
}

CResetStations::~CResetStations(void)
{
}

BOOL CResetStations::LoadXMLConfig(void)
{
	std::wstring strValue;
    for (int i=0; i<MAX_STATION_NUM; i++)
    {
        WCHAR szKey[32] = {0};
        swprintf_s(szKey, L"Option:Station%02d", i + 1);
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

    return TRUE;
}
SPRESULT CResetStations::__PollAction(void)
{ 
    extern CActionApp myApp;
    std::wstring strDllpath = (std::wstring)(_T2CW(myApp.GetAppPath())) + L"\\..\\PhaseCheck.dll";
    HMODULE hLib = LoadLibraryExW(strDllpath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == hLib)
    {
        LogFmtStrW(SPLOGLV_ERROR, L"LoadLibrary <%s> fail, WinErr = %d",strDllpath.c_str(), ::GetLastError());
        NOTIFY("LoadLibrary", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "PhaseCheck.dll");
        return makeResultCode(MODULE_SPATLIB, SP_E_LOAD_LIBRARY);
    }
    pfResetStation ResetStation = (pfResetStation)GetProcAddress(hLib, "ResetStation");
    if (NULL == ResetStation)
    {
        LogFmtStrA(SPLOGLV_ERROR, "GetProcAddress <ResetStation> fail, WinErr = %d",::GetLastError());
        FreeLibrary(hLib);
        NOTIFY("LoadLibrary", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "PhaseCheck.dll");
        return makeResultCode(MODULE_SPATLIB, SP_E_LOAD_LIBRARY);
    }
    //
    BOOL  bAllPass = TRUE;
    STATION_RESULTS result;
    for (int i=0; i<MAX_STATION_NUM; i++)
    {
        if (m_arrStationName[i].empty())
        {
            continue;
        }

		PHASECHECK_RESULT e = (*ResetStation)(m_hDUT, m_arrStationName[i].c_str());

        if (PHASECHECK_PASS != e)
        {
            bAllPass = FALSE; // If one station fail, then fail
        }

        strncpy_s(result.station[i].name, m_arrStationName[i].c_str(), sizeof(result.station[i].name) - 1);
        result.station[i].result = e;
        result.count++;
    }
    FreeLibrary(hLib);
	//
    SPRESULT res = SP_OK;
    for (UINT32 i=0; i<result.count; i++)
    {
        // Display All
        SPRESULT e = ParseResultAndNotify(result.station[i].result, result.station[i].name, CCheckStation::RESET);
        if (SP_OK == res && SP_OK != e)
        {
            res = e; // report the 1st error code
        }
    }

    return bAllPass ? SP_OK : res;
}
