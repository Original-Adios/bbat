#include "StdAfx.h"
#include "CheckStation.h"
#include "ActionApp.h"


//
IMPLEMENT_RUNTIME_CLASS(CCheckStation)

//////////////////////////////////////////////////////////////////////////
CCheckStation::CCheckStation(void)
{
}

CCheckStation::~CCheckStation(void)
{
}

BOOL CCheckStation::LoadXMLConfig(void)
{
    std::wstring strValue = GetConfigValue(L"Option:Station", L"");
    trimW(strValue);
    if (0 == strValue.length())
    {
        m_StationName = ""; 
    }
    else
    {
        m_StationName = _W2CA(strValue.c_str());
    }

    return TRUE;
}

SPRESULT CCheckStation::__PollAction(void)
{ 
    //  @JXP 20180413: 进模式/CheckPreStation失败不更新站位
    BOOL IsUpdate = FALSE;
    SetShareMemory(InternalReservedShareMemory_IsUpdateStation, (LPCVOID)&IsUpdate, 4);

    if (0 == m_StationName.size())
    {
        NOTIFY("CheckStation", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Invalid Station Name");
        return SP_E_PHASECHECK_STATION_NOT_EXIST;
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

    pfCheckPhase CheckPhase = (pfCheckPhase)GetProcAddress(hLib, "CheckPhase");
    if (NULL == CheckPhase)
    {
        LogFmtStrA(SPLOGLV_ERROR, "GetProcAddress <CheckPhase> fail, WinErr = %d",::GetLastError());
        FreeLibrary(hLib);
        NOTIFY("LoadLibrary", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "PhaseCheck.dll");
        return makeResultCode(MODULE_SPATLIB, SP_E_LOAD_LIBRARY);
    }

    PHASECHECK_RESULT e = (*CheckPhase)(m_hDUT, m_StationName.c_str());
    FreeLibrary(hLib);
    
    SPRESULT res = ParseResultAndNotify(e, m_StationName, CHECK);
    if (SP_OK == res)
    {
        IsUpdate = TRUE;
        SetShareMemory(InternalReservedShareMemory_IsUpdateStation, (LPCVOID)&IsUpdate, 4);
    }

    return res;
}

SPRESULT CCheckStation::ParseResultAndNotify(
    PHASECHECK_RESULT e,
    const std::string& strStatioName, 
    CMD eCmd
    )
{
	//binglin.wen
	std::string strItem = "Check [" + strStatioName + "]";
	switch(eCmd)
	{
		case CHECK:
			break;
		case UPDATE:
			strItem = "Update [" + strStatioName + "]";
			break;
		case RESET:
			strItem = "Reset [" + strStatioName + "]";
			break;
	}
	//
    //std::string strItem = (CHECK == eCmd) ? ((std::string)"Check [" + strStatioName + "]") : ((std::string)"Update [" + strStatioName + "]");
    SPRESULT res = SP_OK;
    switch(e)
    {
    case PHASECHECK_PASS:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 1, 1, NULL, -1, NULL, "pass");
        res = SP_OK;
        break;
    case PHASECHECK_INVALID_PARAMETER:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Invalid parameter");
        res = SP_E_SPAT_INVALID_PARAMETER;
        break;
    case PHASECHECK_INVALID_MAGIC_NUMBER:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Unknown Magic");
        res = SP_E_PHASECHECK_INVALID_MAGIC_NUMBER;
        break;
    case PHASHCHECK_STATION_NOT_EXIST:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "%s not exist",  strStatioName.c_str());
        res = SP_E_PHASECHECK_STATION_NOT_EXIST;
        break;
    case PHASHCHECK_STATION_NOT_TEST:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "%s not tested", strStatioName.c_str());
        res = SP_E_PHASECHECK_STATION_NOT_TESTED;
        break;
    case PHASHCHECK_STATION_NOT_PASS:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "%s not pass",   strStatioName.c_str());
        res = SP_E_PHASECHECK_STATION_TEST_FAIL;
        break;
    case PHASHCHECK_SAVE_FAIL:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Update fail");
        res = SP_E_PHASECHECK_STATION_UPDATE_FAIL;
        break;
    case PHASECHECK_LOAD_FAIL:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Load fail");
        res =SP_E_PHASECHECK_LOAD_FAIL;
        break;
    case PHASECHECK_INVALID_CONFIG_FILE:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Invalid PhaseCheck.ini");
        res = SP_E_PHASECHECK_INVALID_CONFIGURATION_FILE;
        break;
    default:
        NOTIFY(strItem.c_str(), LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Unknown");
        res = SP_E_FAIL;
        break;
    }

    return res;
}
