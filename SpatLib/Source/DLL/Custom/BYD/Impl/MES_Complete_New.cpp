#include "StdAfx.h"
#include "MES_Complete_New.h"
#include "ActionApp.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_Complete_New)
//////////////////////////////////////////////////////////////////////////
CMES_Complete_New::CMES_Complete_New(void)
{
}

CMES_Complete_New::~CMES_Complete_New(void)
{
}

BOOL CMES_Complete_New::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_Complete_New::__PollAction(void)
{
	extern CActionApp  myApp;
	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	char szSFC[128]={0},szMsg[512]={0};

	SPRESULT sp_result = MES_GetSN(szSFC,sizeof(szSFC));
	if (SP_OK != sp_result)
	{
		return sp_result;
	}

	/************************************************************************************
	函数：	Complete_New
	功能：	对指定“位置”的SFC执行Complete操作
	参数：
	parSFC[]				SFC
	parBoardCount[]			分板数量(可选，不选传NULL)
	parQualityBatchNum[]	批次数量(可选，不选传NULL)
	parWorkStation[]		工位(可选，不选传NULL)
	parRemark[]				备注(可选，不选传NULL)
	*retMessage				返回的消息
	返回：	如果成功则返回1，否则返回0
	Data_API int Complete_New(char parSFC[], char parBoardCount[], char parQualityBatchNum[], char parWorkStation[], char parRemark[], char *retMessage);
	************************************************************************************/
	BOOL bOK = m_pMesDrv->Complete_New(szSFC, NULL, NULL, NULL, NULL,szMsg);

	m_pMesDrv->Cleanup();
	delete m_pMesDrv;
	m_pMesDrv = NULL;
	CHKRESULT(SetMesDriverIntoShareMemory());

	if (TRUE != bOK)
	{
		NOTIFY("MES_Complete_New", LEVEL_ITEM, 1, 0, 1, NULL, -1, szMsg);
		return SP_E_FAIL;
	}
	NOTIFY("MES_Complete_New", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	return SP_OK;
}

