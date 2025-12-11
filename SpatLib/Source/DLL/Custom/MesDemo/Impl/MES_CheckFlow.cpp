#include "StdAfx.h"
#include "MES_CheckFlow.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_CheckFlow)
//////////////////////////////////////////////////////////////////////////
CMES_CheckFlow::CMES_CheckFlow(void)
{
}

CMES_CheckFlow::~CMES_CheckFlow(void)
{
}

BOOL CMES_CheckFlow::LoadXMLConfig(void)
{

    return TRUE;
}

SPRESULT CMES_CheckFlow::__PollAction(void)
{
	CONST CHAR ITEM_NAME[] = "MES_CheckFlow";
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

	/************************************************************************/
	/* ImpBase.h   Create TestResult_Table.sql                              */
	/************************************************************************/
	/*[ID],[BatchName],[SN],[Result],[ToolsVersion],[ErrCode],[ErrMsg],[IP],[Elapsed],[StationID],[Operator],[UploadTime]*/
	if (TRUE != m_pMesDrv->_CheckFlow(hMes, _A2CW(szSN)))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, "");
		return SP_E_FAIL;
	}

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, "");
	return SP_OK;
}

