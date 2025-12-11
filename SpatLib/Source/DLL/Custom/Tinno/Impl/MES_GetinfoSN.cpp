#include "StdAfx.h"
#include "MES_GetinfoSN.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_GetinfoSN)
//////////////////////////////////////////////////////////////////////////
CMES_GetinfoSN::CMES_GetinfoSN(void)
{

}

CMES_GetinfoSN::~CMES_GetinfoSN(void)
{
}

BOOL CMES_GetinfoSN::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_GetinfoSN::__PollAction(void)
{
	extern CActionApp  myApp;

	char szPathIni[512]={0},szNote[128]={0},szUser[128]={0},szPwd[128]={0},szDescription[1024]={0},szMoNo[32]={0};

	string_t strIniFile = (string_t)myApp.GetAppPath() + _T("\\..\\..\\Config\\TinnoMes.ini");
	strcpy_s(szPathIni, _T2CA(strIniFile.data()));

	GetPrivateProfileStringA("Connect", "username", "", szUser, sizeof(szUser), szPathIni);
	GetPrivateProfileStringA("Connect", "password", "", szPwd, sizeof(szPwd), szPathIni);
	GetPrivateProfileStringA("Connect", "mesnote", "", szNote, sizeof(szNote), szPathIni);
	GetPrivateProfileStringA("Connect", "mono", "", szMoNo, sizeof(szMoNo), szPathIni);


	SPRESULT sp_result = SP_OK;
	char szSN1[ShareMemory_SN_SIZE] = {0};
	char szIMEI1[ShareMemory_IMEI_SIZE] = {0};
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
	else
	{
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
	}
	
	CHAR out_IMEI_M[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_IMEI_S[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_SN[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_MAC[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_BT[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_SpecialSN[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_OTPpwd[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_imeimo[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_RoutingStep[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_RoutingStep_Pre[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_RoutingStep_Next[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_MONOSMT[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_MONOPack[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_MONOAssy[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_SONO[MAX_OUTPUT_PARAM_LEN] = {0};

	CHAR out_SIMLockCode[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_Color[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_MEID[MAX_OUTPUT_PARAM_LEN] = {0};
	CHAR out_pESN[MAX_OUTPUT_PARAM_LEN] = {0};

	if(strcmp(szNote,"070")==0)
	{
		if (TINNO_SUCC != MES_GetinfoIMEI(
			szNote, szUser, szPwd, szIMEI1, "",szMoNo, 
			out_IMEI_M, out_IMEI_S, out_SN, out_MAC, out_BT, out_SpecialSN, out_OTPpwd, out_imeimo, 
			out_RoutingStep, out_RoutingStep_Pre, out_RoutingStep_Next,out_Color,out_MEID,out_pESN,out_SONO,out_MONOSMT, out_MONOPack, out_MONOAssy,out_SIMLockCode,
			szDescription)
			)
		{				
			NOTIFY("MES_GetinfoIMEI", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szDescription);
			return SP_E_SPDB_FAIL;
		}
	}
	else
	{
		if (TINNO_SUCC != MES_GetinfoSN(szNote, szUser, szPwd, szSN1, szMoNo, 
			out_IMEI_M, out_IMEI_S, out_SN, out_MAC, out_BT, out_SpecialSN, out_OTPpwd, out_imeimo, 
			out_RoutingStep, out_RoutingStep_Pre, out_RoutingStep_Next, out_MONOSMT, out_MONOPack, out_MONOAssy, out_SONO,
			szDescription))
		{
			NOTIFY("MES_GetinfoSN", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szDescription);
			return SP_E_SPDB_FAIL;
		}
	}

	NOTIFY("MES_GetinfoSN", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	_START_TIME start_time;
	start_time.dStartTime = GetTickCount();

	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	sprintf_s(start_time.szStartTime,sizeof(start_time.szStartTime),"%04d-%02d-%02d %02d:%02d:%02d",sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
	CHKRESULT(SetShareMemory(ShareMemory_MES_Start_Time, (const void*)&start_time, sizeof(start_time)));

    return SP_OK;
}

