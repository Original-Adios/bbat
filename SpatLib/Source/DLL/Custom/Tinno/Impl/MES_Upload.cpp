#include "StdAfx.h"
#include "MES_Upload.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_Upload)
//////////////////////////////////////////////////////////////////////////
CMES_Upload::CMES_Upload(void)
:  m_eUpload(SYNC)
{
}

CMES_Upload::~CMES_Upload(void)
{
}

BOOL CMES_Upload::LoadXMLConfig(void)
{
    std::wstring strUpload = GetConfigValue(L"Option:Upload", L"Sync");
    if (strUpload == L"Async")
    {
        m_eUpload = ASYNC;
    }
    else if (strUpload == L"Sync")
    {
        m_eUpload = SYNC;
    }
    else
    {
        m_eUpload = NONE;
    }

    return TRUE;
}

SPRESULT CMES_Upload::__PollAction(void)
{
	SPRESULT sp_res = SP_OK;
	extern CActionApp  myApp;

	char szPathIni[512]={0},szNote[128]={0},szUser[128]={0},szPwd[128]={0},szMoNo[128]={0},szDescription[1024]={0},szPathMesLog[512]={0};
	_MES_LOG_HEAD log_head;

	string_t strIniFile = (string_t)myApp.GetAppPath() + _T("\\..\\..\\Config\\TinnoMes.ini");
	strcpy_s(szPathIni, _T2CA(strIniFile.data()));

	GetPrivateProfileStringA("Connect", "username", "", szUser, sizeof(szUser), szPathIni);
	GetPrivateProfileStringA("Connect", "password", "", szPwd, sizeof(szPwd), szPathIni);
	GetPrivateProfileStringA("Connect", "mesnote", "", szNote, sizeof(szNote), szPathIni);
	GetPrivateProfileStringA("Connect", "mono", "", szMoNo, sizeof(szMoNo), szPathIni);

	GetPrivateProfileStringA("TestInfo", "FixtureNo", "", log_head.szFixtureNO, sizeof(log_head.szFixtureNO), szPathIni);
	GetPrivateProfileStringA("TestInfo", "RFCableNo", "", log_head.szCableNO, sizeof(log_head.szCableNO), szPathIni);
	GetPrivateProfileStringA("TestInfo", "InstrumentNo", "", log_head.szInstrumentNO, sizeof(log_head.szInstrumentNO), szPathIni);
	GetPrivateProfileStringA("TestInfo", "platform", "", log_head.szPlatform, sizeof(log_head.szPlatform), szPathIni);

	strncpy_s(log_head.szUserName,szUser,CopySize(szUser));
	strncpy_s(log_head.szOrderID,szMoNo,CopySize(szMoNo));
	strncpy_s(log_head.szOperationID,szNote,CopySize(szNote));
	//UPLOAD LOG
	GetPrivateProfileStringA("UPLOAD LOG", "MES Log Path", "", szPathMesLog, sizeof(szPathMesLog), szPathIni);
	log_head.iUploadLevel = GetPrivateProfileIntA("UPLOAD LOG", "Upload Log Level",0,szPathIni);

	char szFtpIp[128]={0},szFtpUser[128]={0},szFtpPwd[128]={0};
	strIniFile = (string_t)myApp.GetAppPath() + _T("\\..\\..\\Config\\Mes.ini");
	strcpy_s(szPathIni, _T2CA(strIniFile.data()));

	GetPrivateProfileStringA("FTP Server", "ip", "", szFtpIp, sizeof(szFtpIp), szPathIni);
	GetPrivateProfileStringA("FTP Server", "username", "", szFtpUser, sizeof(szFtpUser), szPathIni);
	GetPrivateProfileStringA("FTP Server", "password", "", szFtpPwd, sizeof(szFtpPwd), szPathIni);

	char szDstFile[512]={0},szDstFileName[512]={0},szRemoteFTPDirectory[512]={0};

	if (SP_OK != UploadMesLog(szPathMesLog,&log_head,szDstFile,sizeof(szDstFile)))
	{
		NOTIFY("FTP_ConnectFTPServer", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, szDescription);
		sp_res = SP_E_SPDB_UPLOAD_DATA_FAIL;
	}
	else
	{
		const string szFname("\\");
		string strName(szDstFile);
		string::size_type posS = strName.rfind(szFname);
		strName = strName.substr(posS + szFname.length(), strName.length()-(posS + szFname.length()));

		strcpy_s(szDstFileName,sizeof(szDstFileName),strName.data());
		Get_Host_PCName(szRemoteFTPDirectory,sizeof(szRemoteFTPDirectory));



		if(TINNO_SUCC != FTP_ConnectFTPServer(szFtpIp,szFtpUser,szFtpPwd,szDescription))
		{
			NOTIFY("FTP_ConnectFTPServer", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, szDescription);
			sp_res = SP_E_SPDB_UPLOAD_DATA_FAIL;
		}
		else
		{
			if(TINNO_SUCC != FTP_UpLoad_TestLogFile_To_FTPServer(szMoNo,szDstFile,szDstFileName,szRemoteFTPDirectory,szDescription))
			{
				NOTIFY("FTP_UpLoad_TestLogFile_To_FTPServer", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, szDescription);
				sp_res = SP_E_SPDB_UPLOAD_DATA_FAIL;
			}
		}
		FTP_DisConnectFTPServer();
		NOTIFY("FTP_UpLoad_TestLogFile_To_FTPServer", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	}
    return sp_res;
}

SPRESULT CMES_Upload::UploadMesLog(char*pszDirMesLog,_pMES_LOG_HEAD p_log_head,char*pszPathMesLog,int iSize)
{ 
	IItemDataObserver*pObs;
	if ((SP_OK != GetShareMemory(ShareMemoryItemDataObserver, (void* )&pObs, sizeof(pObs))) || (NULL == pObs))
	{
		return SP_E_FAIL;
	}
	CMesData* pMesData = (CMesData* )pObs;

	SPRESULT sp_result = SP_OK;
	char szSN1[ShareMemory_SN_SIZE] = {0};
	ZeroMemory(m_InputSN,sizeof(m_InputSN));
	sp_result = GetShareMemory(ShareMemory_SN1, (void* )szSN1, sizeof(szSN1));
	if (SP_OK != sp_result || 0 == strlen(szSN1))
	{
		sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
		if (SP_OK != sp_result || 0 == strlen(m_InputSN[BC_SN1].szCode))
		{
			LogFmtStrW(SPLOGLV_ERROR, L"Get share memory < %s > <%s> failure!", ShareMemory_SN1,ShareMemory_My_UserInputSN);
			NOTIFY("GetShareMemory", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "SN1");
			return SP_E_FAIL;
		}
		if (0 == strlen(m_InputSN[BC_SN1].szCode))
		{
			LogFmtStrW(SPLOGLV_ERROR, L"GetShareMemory(SN1 Length == 0)");
			NOTIFY("GetShareMemory", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "SN1 Length == 0");
			return SP_E_FAIL;
		}
		strcpy_s(szSN1,sizeof(szSN1),m_InputSN[BC_SN1].szCode);
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
				sprintf_s(p_log_head->szInstrumentNO,sizeof(p_log_head->szInstrumentNO),"%s_%s",szArray[1],szArray[2]);
			}
		}		
	}

	SPTEST_RESULT_T simba_result;
	sp_result = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&simba_result, sizeof(simba_result));
	if (SP_OK != sp_result)
	{   
		return sp_result;
	}
	else
	{
		strcpy_s(simba_result.SN,sizeof(simba_result.SN),szSN1);
	}

	char szRepairCode[50]={0};
	sp_result = GetProperty(Property_RepairCode, sizeof(szRepairCode), (LPVOID)szRepairCode);
	if (SP_OK != sp_result)
	{   
		return sp_result;
	}

	_START_TIME start_time;
	GetShareMemory(ShareMemory_MES_Start_Time, (void*)&start_time, sizeof(start_time));

	double dEndTime = GetTickCount();
	p_log_head->dElapsedTime = (dEndTime-start_time.dStartTime)/1000.0;

	strcpy_s(p_log_head->szStartTime,sizeof(p_log_head->szStartTime),start_time.szStartTime);

	p_log_head->iTestResult = simba_result.errCode ==SP_OK? 1 : 0;
	strcpy_s(p_log_head->szSN,sizeof(p_log_head->szSN),simba_result.SN);

	sprintf_s(p_log_head->szTestMsg,sizeof(p_log_head->szTestMsg),"%s %s",simba_result.szFailItem,simba_result.szDescription);

	Get_Host_PCName(p_log_head->szWorkUnitID,sizeof(p_log_head->szWorkUnitID));
	Get_Tool_Info(p_log_head->szTestName,p_log_head->szVersion);

	pMesData->MES_Log_Path_Init(pszDirMesLog,p_log_head->szOperationID);
	if(!pMesData->MES_Log_Head(p_log_head))
	{
		return SP_E_FAIL;
	}

	if (!pMesData->isRunning())
	{
		CHKRESULT_WITH_NOTIFY(pMesData->StartUpload(TRUE) ? SP_OK : SP_E_SPDB_START_TEST, "StartUploading");
	}

	pMesData->StartUpload(FALSE);
	pMesData->MES_Log_Path_New(p_log_head->iTestResult,p_log_head->szSN,pszDirMesLog,p_log_head->szOperationID);
	pMesData->MES_Log_Get_New_Name(pszPathMesLog,iSize);
	pMesData->MES_Log_Path_Rename();
	
	pMesData->Release();
	pMesData = NULL;
	CHKRESULT(SetShareMemory(ShareMemoryItemDataObserver, (const void* )&pMesData, sizeof(pMesData),IContainer::System));

	return SP_OK;
}

