#include "StdAfx.h"
#include "CheckStationExist.h"
#include "ActionApp.h"

//
IMPLEMENT_RUNTIME_CLASS(CCheckStationExist)

//////////////////////////////////////////////////////////////////////////
CCheckStationExist::CCheckStationExist(void)
{
}

CCheckStationExist::~CCheckStationExist(void)
{
}

BOOL CCheckStationExist::LoadXMLConfig(void)
{
    WCHAR szKey[32] = {0};
    std::wstring strValue = L"";
    for (int i=0; i<MAX_STATION_NUM; i++)
    {
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
    
    return TRUE;
}

SPRESULT CCheckStationExist::__PollAction(void)
{ 
    extern CActionApp myApp;
    std::wstring strDllpath = (std::wstring)(_T2CW(myApp.GetAppPath())) + L"\\..\\PhaseCheck.dll";
    HMODULE hLib = LoadLibraryExW(strDllpath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == hLib)
    {
        LogFmtStrW(SPLOGLV_ERROR, L"LoadLibrary <%s> fail, WinErr = %d", strDllpath.c_str(), ::GetLastError());
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

    BOOL bAllPass = FALSE;
    STATION_RESULTS lst;
    PHASECHECK_RESULT e = (*CheckAllStations)(m_hDUT, &bAllPass, &lst);
    FreeLibrary(hLib);
    hLib = NULL;
    if (PHASECHECK_PASS != e)
    {
        return ParseResultAndNotify(e, "CheckAllStation", CCheckStation::CHECK);
    }
 
    BOOL bAllExists = TRUE;
    for (UINT32 i=0; i<MAX_STATION_NUM; i++)
    {
        if (m_arrStationName[i].empty())
        {
            continue;
        }

        BOOL bFound = FALSE;
        for (UINT32 j=0; j<lst.count; j++)
        {
            if (0 == _stricmp(m_arrStationName[i].c_str(), lst.station[j].name))
            {
                bFound = TRUE;
                break;
            }
        }

        if (!bFound)
        {
            bAllExists = FALSE;

            CHAR szName[64] = {0};
            _snprintf_s(szName, 64, "Station [%s] not exist", m_arrStationName[i].c_str());
            NOTIFY(szName, LEVEL_UI, 1, 0, 1, NULL, -1, NULL);
        }
    }
        
    return bAllExists ? SP_OK : SP_E_PHASECHECK_STATION_NOT_EXIST;
}
