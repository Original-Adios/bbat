#include "StdAfx.h"
#include "MES_SendTestResult.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_SendTestResult)
//////////////////////////////////////////////////////////////////////////
CMES_SendTestResult::CMES_SendTestResult(void)
{
}

CMES_SendTestResult::~CMES_SendTestResult(void)
{
}

BOOL CMES_SendTestResult::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_SendTestResult::__PollAction(void)
{
	CONST CHAR ITEM_NAME[] = "MES_SendTestResult";
	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	HANDLE hMes = m_pMesDrv->GetMesHandle(); 
	if (NULL == hMes)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES HANDLE == NULL");
		return SP_E_FAIL;
	}

	SPRESULT sp_result = SP_OK;
	char szSN[64] = {0};
	sp_result = MES_GetSN(szSN,sizeof(szSN));
	if (SP_OK != sp_result)
	{
		return sp_result;
	}

	SPTEST_RESULT_T simba_result;
	sp_result = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&simba_result, sizeof(simba_result));
	if (SP_OK != sp_result)
	{   
		return sp_result;
	}
	SPRESULT errCode = simba_result.errCode;

	Get_Tool_Info(m_szToolName, m_szToolVersion);
	std::wstring strToolInfor = _A2CW(m_szToolName);
	strToolInfor += L"_";
	strToolInfor += _A2CW(m_szToolVersion);

	char szIp[MAX_BUFF_LENGTH] = {0};
	GetHostIP(szIp);
	std::wstring strIp = _A2CW(szIp);

	double dStartTime = 0.0; 
	GetShareMemory(ShareMemory_MES_Start_Time, (void*)&dStartTime, sizeof(dStartTime));
	double dElapsedTime = (GetTickCount() - dStartTime)/1000.0;

	PROJINFOR ProjInfor;
	CHKRESULT(GetShareMemory(ShareMemory_MES_V1_Batch_info, (void*)&ProjInfor, sizeof(ProjInfor)));
	std::wstring strBatchName = _A2CW(ProjInfor.m_szBatchName);

	/************************************************************************/
	/* ImpBase.h   Create TestResult_Table.sql                              */
	/************************************************************************/
	/*[ID],[BatchName],[SN],[Result],[ToolsVersion],[ErrCode],[ErrMsg],[IP],[Elapsed],[StationID],[Operator],[UploadTime]*/
	if (TRUE != m_pMesDrv->_SendTestResult(hMes, _A2CW(szSN), strBatchName.c_str(), UINT8(errCode == SP_OK ? 1 : 0), strToolInfor.c_str(),
		errCode, errCode == SP_OK ? L"": _A2CW(simba_result.szDescription), \
		strIp.c_str(), (UINT32)dElapsedTime, UINT8(1), UINT8(1)))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, "SendTestResult");
		return SP_E_FAIL;
	}

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, "");
	return SP_OK;
}

