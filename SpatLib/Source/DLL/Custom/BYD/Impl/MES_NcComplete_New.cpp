#include "StdAfx.h"
#include "MES_NcComplete_New.h"
#include "ActionApp.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_NcComplete_New)
//////////////////////////////////////////////////////////////////////////
CMES_NcComplete_New::CMES_NcComplete_New(void)
{
}

CMES_NcComplete_New::~CMES_NcComplete_New(void)
{
}

BOOL CMES_NcComplete_New::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_NcComplete_New::__PollAction(void)
{
	extern CActionApp  myApp;
	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	SPTEST_RESULT_T simba_result;
	SPRESULT sp_result = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&simba_result, sizeof(simba_result));
	if (SP_OK != sp_result)
	{   
		NOTIFY("MES_NcComplete_New", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(TestResult)");
		return sp_result;
	}
	if (simba_result.errCode ==SP_OK)
	{
		LogFmtStrW(SPLOGLV_INFO, L"The final test result passed, MES_NcComplete_New don't be executed.");
		return SP_OK;
	}

	char szSFC[128]={0},szMsg[512]={0},szNcType[64]={0},szNcCode[64]={0},szNcContext[128]={0},szFailItem[128]={0};

	sp_result = MES_GetSN(szSFC,sizeof(szSFC));
	if (SP_OK != sp_result)
	{
		return sp_result;
	}
	/************************************************************************************
	函数：	NcComplete_New
	功能：	对指定“位置”的SFC执行NC_Complete操作
	参数：
			parSFC[]			SFC
			parNcType[]			不良类型
			parNcCode[]			不良代码
			parNcContext[]		不良描述
			parFailItem[]		FAIL项（可选，不选传NULL）
			parFailValue[]		FAIL值（可选，不选传NULL）
			parBoardCount[]		分板数量(可选，不选传NULL)
			parWorkStation[]	工位（可选，不选传NULL）
			parLogOperation[]	设备（电脑名或SMT机编号，可选，不选传NULL）
			parLogResource[]	资源名称（夹具，可选，不选传NULL)
			parNcPlace[]		不良位置(可选，不选传NULL)
			parCreateUser[]		不良录入人(可选，不选传NULL)
			parOldStationName[]	原工站名称(可选，不选传NULL)
			parRemark[]			备注（可选，不选传NULL）
			*retMessage			返回的消息
	返回：	如果成功则返回1，否则返回0
	Data_API int NcComplete_New(char parSFC[], char parNcType[], char parNcCode[], char parNcContext[], char parFailItem[], char parFailValue[], char parBoardCount[]
	, char parWorkStation[], char parLogOperation[], char parLogResource[], char parNcPlace[], char parCreateUser[], char parOldStationName[], char parRemark[]
	, char *retMessage);
	************************************************************************************/
	//RepairCode
	sp_result = GetProperty(Property_RepairCode, sizeof(szNcCode), (LPVOID)szNcCode);
	if (SP_OK != sp_result)
	{   
		NOTIFY("MES_NcComplete_New", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "GetProperty(Property_RepairCode)");
		return sp_result;
	}

	sprintf_s(szNcType,sizeof(szNcType),"%d",simba_result.errCode);
	strcpy_s(szNcContext,sizeof(szNcContext),simba_result.szDescription);
	strcpy_s(szFailItem,sizeof(szFailItem),simba_result.szFailItem);

	if (TRUE != m_pMesDrv->NcComplete_New(szSFC, szNcType, szNcCode, szNcContext, szFailItem,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,szMsg))
	{
		NOTIFY("MES_NcComplete_New", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szMsg);
		return SP_E_FAIL;
	}
	NOTIFY("MES_NcComplete_New", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");

	return SP_OK;
}

