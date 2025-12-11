#include "StdAfx.h"
#include "MES_SaveTestRecord.h"
#include "ActionApp.h"
#include "../drv/MesData.h"

//
IMPLEMENT_RUNTIME_CLASS(CMES_SaveTestRecord)
//////////////////////////////////////////////////////////////////////////
CMES_SaveTestRecord::CMES_SaveTestRecord(void)
{
}

CMES_SaveTestRecord::~CMES_SaveTestRecord(void)
{
}

BOOL CMES_SaveTestRecord::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_SaveTestRecord::__PollAction(void)
{
	extern CActionApp  myApp;

	SPRESULT sp_result = SP_OK;
	char szSN1[ShareMemory_SN_SIZE] = {0};
	char szIMEI1[ShareMemory_IMEI_SIZE] = {0};

	char szPathIni[512]={0},szNote[128]={0},szUser[128]={0},szPwd[128]={0},szDescription[1024]={0},szShiftID[32]={0},szFailCode[64]={0};
	char szTestName[128]={0}, szFixtureNo[128]={0}, szRFCableNo[128]={0}, szInstrumentNo[128]={0}, szToolVersion[128]={0},szPlatform[128]={0};


	string_t strIniFile = (string_t)myApp.GetAppPath() + _T("\\..\\..\\Config\\TinnoMes.ini");
	strcpy_s(szPathIni, _T2CA(strIniFile.data()));

	GetPrivateProfileStringA("Connect", "username", "", szUser, sizeof(szUser), szPathIni);
	GetPrivateProfileStringA("Connect", "password", "", szPwd, sizeof(szPwd), szPathIni);
	GetPrivateProfileStringA("Connect", "mesnote", "", szNote, sizeof(szNote), szPathIni);
	GetPrivateProfileStringA("TestInfo", "TestName", "", szTestName, sizeof(szTestName), szPathIni);
	GetPrivateProfileStringA("TestInfo", "FixtureNo", "", szFixtureNo, sizeof(szFixtureNo), szPathIni);
	GetPrivateProfileStringA("TestInfo", "RFCableNo", "", szRFCableNo, sizeof(szRFCableNo), szPathIni);
	GetPrivateProfileStringA("TestInfo", "InstrumentNo", "", szInstrumentNo, sizeof(szInstrumentNo), szPathIni);
	GetPrivateProfileStringA("TestInfo", "platform", "", szPlatform, sizeof(szPlatform), szPathIni);

	Get_Tool_Info(szTestName,szToolVersion);

	ZeroMemory(m_InputSN,sizeof(m_InputSN));
	if(strcmp(szNote,"070")==0)
	{
		sp_result = GetShareMemory(ShareMemory_IMEI1, (void*)szIMEI1, sizeof(szIMEI1));
		if (SP_OK != sp_result || 0 == strlen(szIMEI1))
		{
			sp_result = GetShareMemory(ShareMemory_My_UserInputIMEI, (void* )&m_InputSN, sizeof(m_InputSN));
			if (SP_OK != sp_result || 0 == strlen(m_InputSN[BC_IMEI1].szCode))
			{
				NOTIFY("MES_SaveTestRecord", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_My_UserInputIMEI)");
				return SP_E_FAIL;
			}
			if (0 == strlen(m_InputSN[BC_IMEI1].szCode))
			{
				NOTIFY("MES_SaveTestRecord", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "IMEI1 Length == 0");
				return SP_E_FAIL;
			}
			strcpy_s(szIMEI1,sizeof(szIMEI1),m_InputSN[BC_IMEI1].szCode);
		}
	}


	sp_result = GetShareMemory(ShareMemory_SN1, (void*)szSN1, sizeof(szSN1));
	if (SP_OK != sp_result || 0 == strlen(szSN1))
	{
		sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
		if (SP_OK != sp_result || 0 == strlen(m_InputSN[BC_SN1].szCode))
		{
			NOTIFY("MES_SaveTestRecord", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_My_UserInputSN)");
			return SP_E_FAIL;
		}
		if (0 == strlen(m_InputSN[BC_SN1].szCode))
		{
			NOTIFY("MES_SaveTestRecord", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SN1 Length == 0");
			return SP_E_FAIL;
		}
		strcpy_s(szSN1,sizeof(szSN1),m_InputSN[BC_SN1].szCode);
	}

	//MES_SendTestResult
	SPTEST_RESULT_T simba_result;
	sp_result = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&simba_result, sizeof(simba_result));
	if (SP_OK != sp_result)
	{   
		NOTIFY("MES_SaveTestRecord", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(TestResult)");
		return sp_result;
	}
	else
	{
		strncpy_s(simba_result.SN,szSN1,CopySize(simba_result.SN));
	}
	
	if(NULL != m_pRFTester)
	{
		RFDEVICE_INFO rf_info;
		ZeroMemory(&rf_info,sizeof(RFDEVICE_INFO));
		m_pRFTester->GetProperty(DP_DEV_INFO, 0, (LPVOID)&rf_info);
		if (NULL!=rf_info.pDevInfo)
		{
			char szIDN[512]={0},szArray[11][512]={0};
			strncpy_s(szIDN,rf_info.pDevInfo,CopySize(szIDN));		
			StringToArray(10,',',szIDN,szArray[0],sizeof(szArray[0]));
			if (NULL != szArray[1][0] && NULL != szArray[2][0] )
			{
				StringReplace(szArray[2]);
				sprintf_s(szInstrumentNo,sizeof(szInstrumentNo),"%s_%s",szArray[1],szArray[2]);
			}
		}		
	}

	SYSTEMTIME time_local;
	GetLocalTime(&time_local);
	if(time_local.wHour>=8 && time_local.wHour <20)
	{
		strcpy_s(szShiftID,"310001");   //白班
	}else
	{
		strcpy_s(szShiftID,"310002");   //晚班
	}

	sprintf_s(szFailCode, "%d", simba_result.errCode);

	char szRepairCode[128]={0};
	sp_result = GetProperty(Property_RepairCode, sizeof(szRepairCode), (LPVOID)szRepairCode);
	if (SP_OK != sp_result)
	{   
		NOTIFY("MES_DataLogEnd", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetProperty(Property_RepairCode)");
		return sp_result;
	}	
	
	strcat_s(szRepairCode, CopySize(szRepairCode), ",");
	strcat_s(szRepairCode, CopySize(szRepairCode), simba_result.szFailItem);
	sprintf_s(simba_result.szDescription, "%s", (SP_OK == simba_result.errCode) ? "" : szRepairCode);

	_START_TIME start_time;
	GetShareMemory(ShareMemory_MES_Start_Time, (void*)&start_time, sizeof(start_time));
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	sprintf_s(start_time.szEndTime,sizeof(start_time.szEndTime),"%04d-%02d-%02d %02d:%02d:%02d",sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);

	if (TINNO_SUCC != MES_SaveTestRecord(szNote, szUser, szPwd, 
		simba_result.SN, 
		szShiftID, 
		start_time.szStartTime,
		start_time.szEndTime,
		(SP_OK == simba_result.errCode) ? "pass" : "fail", // 测试结果，pass or fail
		" ",   // in_TestComment：备注信息
		"ADD", // in_Type：添加记录或者更新记录 ，ADD or Update
		szTestName, 
		szFixtureNo, 
		szRFCableNo, 
		szInstrumentNo, 
		szToolVersion,
		szPlatform,
		szFailCode, 
		simba_result.szDescription, 
		szDescription))
	{
		NOTIFY("MES_SaveTestRecord", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szDescription);
		return SP_E_SPDB_SERVER_CONNECT;
	}

	NOTIFY("MES_SaveTestRecord", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");

	if(SP_OK == simba_result.errCode && strcmp(szNote,"004")==0) //004 校准综测
	{		
		if (TINNO_SUCC != MES_NextSN(szNote, szUser, szPwd, simba_result.SN, szShiftID,szDescription))
		{
			NOTIFY("MES_NextSN", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szDescription);
			return SP_E_SPDB_FAIL;
		}
		NOTIFY("MES_NextSN", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	}

	if(SP_OK == simba_result.errCode && strcmp(szNote,"070")==0) //004 校准综测
	{		
		if (TINNO_SUCC != MES_NextIMEI(szNote, szUser, szPwd, szIMEI1, "", szShiftID, "", "", "", szDescription))
		{
			NOTIFY("MES_NextIMEI", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szDescription);
			return SP_E_SPDB_FAIL;
		}
		NOTIFY("MES_NextIMEI", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	}

    return SP_OK;
}

