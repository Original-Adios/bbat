#include "StdAfx.h"
#include "CheckStationFrMisc.h"

IMPLEMENT_RUNTIME_CLASS(CCheckStationFrMisc)
//////////////////////////////////////////////////////////////////////////
CCheckStationFrMisc::CCheckStationFrMisc(void)
{
	m_nStation = 0;
}

CCheckStationFrMisc::~CCheckStationFrMisc(void)
{
}

BOOL CCheckStationFrMisc::LoadXMLConfig(void)
{
	m_nStation = (UINT8)GetConfigValue(L"Option:CheckStation", 0);
	if (!IN_RANGE(1, m_nStation, MAX_STATION_NUMBER))
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid Station Set, Range[%d-%d-%d]", 1, m_nStation, MAX_STATION_NUMBER);
		return FALSE;
	}	  
	return TRUE;
}

SPRESULT CCheckStationFrMisc::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "CheckStationFromMisc"; 

	uint8 rbuff[MAX_STATION_NUMBER] = {0};
	SPRESULT sRet = SP_LoadMiscData(m_hDUT, CUSTOM_TEST_STATION_POSITION, rbuff, MAX_STATION_NUMBER, TIMEOUT_3S);
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "LoadCustomerMiscData");
		return sRet;
	}

	if ('P' != rbuff[m_nStation - 1])
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Check Station %d Fail", m_nStation);
		return SP_E_MISMATCHED_CU;
	}
	
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Check Station %d Pass", m_nStation);
    return SP_OK;
}
