#include "StdAfx.h"
#include "UpdateStationToMisc.h"

IMPLEMENT_RUNTIME_CLASS(CUpdateStationToMisc)
//////////////////////////////////////////////////////////////////////////
CUpdateStationToMisc::CUpdateStationToMisc(void)
{
	m_nStation = 0;
}

CUpdateStationToMisc::~CUpdateStationToMisc(void)
{
}

BOOL CUpdateStationToMisc::LoadXMLConfig(void)
{
	m_nStation = (UINT8)GetConfigValue(L"Option:UpdateStation", 0);
	if (!IN_RANGE(1, m_nStation, MAX_STATION_NUMBER))
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid Station Set, Range[%d-%d-%d]", 1, m_nStation, MAX_STATION_NUMBER);
		return FALSE;
	}
	return TRUE;
}

SPRESULT CUpdateStationToMisc::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "UpdateStationToMisc";	

	//Get Simba Result
	SPTEST_RESULT_T simba_result;
	CHKRESULT_WITH_NOTIFY(GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&simba_result, sizeof(simba_result)), "GetSimbaResult");

	SPRESULT sRet = WriteCustMisdata(CUSTOM_TEST_STATION_POSITION + m_nStation - 1, (SP_OK == simba_result.errCode) ? "P" : "F", 1);
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Update Station %d Fail", m_nStation);
		return sRet;
	}

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Update Station %d Success", m_nStation);
    return SP_OK;
}
