#include "StdAfx.h"
#include "MES_SfcKeyCollect_New.h"
#include "ActionApp.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_SfcKeyCollect_New)
//////////////////////////////////////////////////////////////////////////
CMES_SfcKeyCollect_New::CMES_SfcKeyCollect_New(void)
{
}

CMES_SfcKeyCollect_New::~CMES_SfcKeyCollect_New(void)
{
}

BOOL CMES_SfcKeyCollect_New::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_SfcKeyCollect_New::__PollAction(void)
{
	extern CActionApp  myApp;
	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	char szSFC[128]={0},szMsg[512]={0},szData[512]={0},szNumber[512]={0};

	SPRESULT sp_result = MES_GetSN(szSFC,sizeof(szSFC));
	if (SP_OK != sp_result)
	{
		return sp_result;
	}

	sp_result = GetShareMemory(ShareMemory_BydNumber, (void* )&szNumber, sizeof(szNumber));
	if (SP_OK != sp_result)
	{
		NOTIFY("MES_SfcKeyCollect_New", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_BydNumber");
		return sp_result;
	}
	//需要对szData数据解析加1
	SetSfcKeyCollectData(szNumber,szData,sizeof(szData));
	LogFmtStrA(SPLOGLV_INFO, "SfcKeyCollect_New(%s)",szData);
	/************************************************************************************
	函数：	SfcKeyCollect_New
	功能：	关键物料收集(绑定)
	参数：
		parSFC[]			SFC
		parData[]			关键物料数据(格式为“名称:值:允许重复采集的数量;……;名称:值:允许重复采集的数量”)
		*retMessage			返回的消息
	返回：	如果成功则返回1，否则返回0
	Data_API int SfcKeyCollect_New(char parSFC[], char parData[], char *retMessage);
	************************************************************************************/
	BOOL bOK = m_pMesDrv->SfcKeyCollect_New(szSFC, szData, szMsg);
	if (TRUE != bOK)
	{
		NOTIFY("MES_SfcKeyCollect_New", LEVEL_ITEM, 1, 0, 1, NULL, -1, szMsg);
		return SP_E_FAIL;
	}

	NOTIFY("MES_SfcKeyCollect_New", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	return SP_OK;
}

