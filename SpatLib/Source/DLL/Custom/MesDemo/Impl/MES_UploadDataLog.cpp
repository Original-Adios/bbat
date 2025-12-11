#include "StdAfx.h"
#include "MES_UploadDataLog.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_UploadDataLog)
//////////////////////////////////////////////////////////////////////////
CMES_UploadDataLog::CMES_UploadDataLog(void)
{
}

CMES_UploadDataLog::~CMES_UploadDataLog(void)
{
}

BOOL CMES_UploadDataLog::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_UploadDataLog::__PollAction(void)
{
	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	HANDLE hMes = m_pMesDrv->GetMesHandle(); 
	if (NULL == hMes)
	{
		NOTIFY("MES_UploadDataLog", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES HANDLE == NULL");
		return SP_E_FAIL;
	}

	char szPathDataLog[512]={0};
	if(SP_OK != UploadMesLog(szPathDataLog,sizeof(szPathDataLog)))
	{
		NOTIFY("MES_UploadDataLog", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "UploadMesLog");
	}
	/************************************************************************/
	/* ImpBase.h   Create TestValue_Table.sql                              */
	/************************************************************************/
	/*[ID],[BatchName],[SN],[Item],[Result],[Station],[TestValue],[LowLimit],[UpLimit],[Unit],[UploadTime]*/
	if (TRUE != m_pMesDrv->_UploadDataLog(hMes, _A2CW(szPathDataLog)))
	{

		NOTIFY("MES_UploadDataLog", LEVEL_ITEM, 1, 0, 1, NULL, -1);
		return SP_E_FAIL;
	}

	NOTIFY("MES_UploadDataLog", LEVEL_ITEM, 1, 1, 1, NULL, -1);
	return SP_OK;
}

SPRESULT CMES_UploadDataLog::UploadMesLog(char*pszPath,int iSize)
{ 
	extern CActionApp  myApp;
	IItemDataObserver*pObs;
	if ((SP_OK != GetShareMemory(ShareMemoryItemDataObserver, (void* )&pObs, sizeof(pObs))) || (NULL == pObs))
	{
		return SP_E_FAIL;
	}
	CMesData* pMesData = (CMesData* )pObs;

	SPRESULT sp_result = SP_OK;
	char szSN[64] = {0};
	sp_result = MES_GetSN(szSN,sizeof(szSN));
	if (SP_OK != sp_result)
	{
		return sp_result;
	}

	_MES_LOG_HEAD log_head;

	log_head.iSlot = GetAdjParam().nTASK_ID;

	//
	WCHAR szSeqFile[MAX_PATH] = {0};
	WCHAR szSeqVer[32] = {0};
	GetSeqParse()->GetProperty(MAX_PATH, Property_SeqFile, (LPVOID)szSeqFile);
	GetSeqParse()->GetProperty(32, Property_SeqVersion, (LPVOID)szSeqVer);
	strcpy_s(log_head.szSeqName,sizeof(log_head.szSeqName), _W2CA(szSeqFile));
	strcpy_s(log_head.szSeqVersion,sizeof(log_head.szSeqVersion), _W2CA(szSeqVer));

	//
	if(NULL != m_pRFTester)
	{
		RFDEVICE_INFO rf_info;
		ZeroMemory(&rf_info,sizeof(RFDEVICE_INFO));
		m_pRFTester->GetProperty(DP_DEV_INFO, 0, (LPVOID)&rf_info);
		if (NULL!=rf_info.pDevInfo)
		{
			strncpy_s(log_head.szRfDeviceID,rf_info.pDevInfo,CopySize(log_head.szRfDeviceID));		
		}		
	}

	if (NULL != m_pDCSource)
	{
		DCS_INFO dc;
		ZeroMemory(&dc,sizeof(DCS_INFO));
		m_pDCSource->GetProperty(DCP_DCS_INFO, 0, (LPVOID)&dc);
		if (NULL!=dc.lpDevName)
		{
			strncpy_s(log_head.szPwr1ID,dc.lpDevName,CopySize(log_head.szPwr1ID));
		}	
	}

	IDCS*pVBUS = NULL;
	GetShareMemory(ShareMemory_VBusPowerSupplyObject, (LPVOID)&pVBUS, sizeof(IDCS*));
	if (NULL != pVBUS)
	{
		DCS_INFO dc;
		ZeroMemory(&dc,sizeof(DCS_INFO));
		pVBUS->GetProperty(DCP_DCS_INFO, 0, (LPVOID)&dc);
		if (NULL!=dc.lpDevName)
		{
			strncpy_s(log_head.szPwr2ID,dc.lpDevName,CopySize(log_head.szPwr2ID));
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
		strcpy_s(simba_result.SN,sizeof(simba_result.SN),szSN);
	}

	char szRepairCode[50]={0};
	sp_result = GetProperty(Property_RepairCode, sizeof(szRepairCode), (LPVOID)szRepairCode);
	if (SP_OK != sp_result)
	{   
		return sp_result;
	}

	double dStartTime = 0.0; 
	GetShareMemory(ShareMemory_MES_Start_Time, (void*)&dStartTime, sizeof(dStartTime));
	double dEndTime = GetTickCount();
	log_head.dElapsedTime = (dEndTime-dStartTime)/1000.0;

	log_head.iTestResult = simba_result.errCode ==SP_OK? 1 : 0;
	strcpy_s(log_head.szSN,sizeof(log_head.szSN),simba_result.SN);
	strcpy_s(log_head.szErrorCode,sizeof(log_head.szErrorCode),simba_result.errCode ==SP_OK?"":szRepairCode);
	sprintf_s(log_head.szErrorMsg,sizeof(log_head.szErrorMsg),"%s %s",simba_result.szFailItem,simba_result.szDescription);

	
	log_head.iCsvLogUploadLevel = pMesData->m_iCsvLogUploadLevel;
	memcpy(log_head.szBatch, pMesData->szBatch, sizeof(char) * strlen(pMesData->szBatch));
	memcpy(log_head.szStation, pMesData->szStation, sizeof(char) * strlen(pMesData->szStation));

	std::string strMesLogPath = "D:\\MesDemo";
	string strDir(strMesLogPath.c_str()), strPath = "";
	const string strFname(":");
	string::size_type posS = strDir.find(strFname);
	strPath += strDir.substr(0, posS);
	strPath += strFname;
	strPath += "\\";
	if(!PathIsDirectoryA(strPath.data()))     
	{  
		NOTIFY("MES_Disconnect", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "\"%s\" does not exist.",strPath.data());
		return SP_E_FAIL;
	}

	if (!pMesData->MES_Log_Path_Init(log_head.iSlot, (char*)strMesLogPath.c_str(), log_head.szStation))
	{
		return SP_E_FAIL;
	}

	if(!pMesData->MES_Log_Head(&log_head))
	{
		return SP_E_FAIL;
	}

	if (!pMesData->isRunning())
	{
		CHKRESULT_WITH_NOTIFY(pMesData->StartUpload(TRUE) ? SP_OK : SP_E_SPDB_START_TEST, "StartUploading");
	}

	pMesData->StartUpload(FALSE);
	pMesData->MES_Log_Path_New(log_head.iTestResult, log_head.iSlot, log_head.szSN, (char*)strMesLogPath.c_str(), log_head.szStation);
	pMesData->MES_Log_Path_Rename(pszPath, iSize);

	pMesData->Release();
	pMesData = NULL;
	CHKRESULT(SetShareMemory(ShareMemoryItemDataObserver, (const void* )&pMesData, sizeof(pMesData),IContainer::System));

	return SP_OK;
}

