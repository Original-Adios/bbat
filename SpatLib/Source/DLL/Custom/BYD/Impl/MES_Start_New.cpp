#include "StdAfx.h"
#include "MES_Start_New.h"
#include "ActionApp.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_Start_New)
//////////////////////////////////////////////////////////////////////////
CMES_Start_New::CMES_Start_New(void)
{
}

CMES_Start_New::~CMES_Start_New(void)
{
}

BOOL CMES_Start_New::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_Start_New::__PollAction(void)
{
	extern CActionApp  myApp;
	if (SP_OK != GetMesDriverFromShareMemory())
	{
		m_pMesDrv = new CMesDriver();
		CHKRESULT(SetMesDriverIntoShareMemory());
	}

	string_t strDrvPath = (string_t)myApp.GetAppPath() + _T("\\..\\..\\Customized\\BYD\\MES2Interface.dll");
	CHKRESULT_WITH_NOTIFY(m_pMesDrv->Startup(strDrvPath) ? SP_OK : SP_E_LOAD_LIBRARY, "Load MES2Interface.dll");
	/************************************************************************************
	函数：	Start_New
	功能：	对指定“位置”的SFC执行Start操作(CHECK流程信息)
	参数：
			parSFC[]			SFC
			parBoardCount[]		分板数量(可选，不选传NULL)
			parWorkStation[]	工位(可选，不选传NULL)
			parLogOperation[]	设备（电脑名或SMT机编号，可选，不选传NULL)
			parLogResource[]	资源名称（夹具，可选，不选传NULL)
			parRemark[]			备注(可选，不选传NULL)
			*retMessage			返回的消息
	返回：	如果成功则返回1，否则返回0
	Data_API int Start_New(char parSFC[], char parBoardCount[], char parWorkStation[], char parLogOperation[], char parLogResource[], char parRemark[], char *retMessage);
	************************************************************************************/
	char szSFC[128]={0},szMsg[512]={0};

	SPRESULT sp_result = MES_GetSN(szSFC,sizeof(szSFC));
	if (SP_OK != sp_result)
	{
		return sp_result;
	}

	if (TRUE != m_pMesDrv->Start_New(szSFC, NULL, NULL, NULL, NULL, NULL,szMsg))
	{
		NOTIFY("MES_Start_New", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szMsg);
		return SP_E_FAIL;
	}
	NOTIFY("MES_Start_New", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");

    return SP_OK;
}

