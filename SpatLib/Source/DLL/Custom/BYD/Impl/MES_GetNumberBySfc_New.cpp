#include "StdAfx.h"
#include "MES_GetNumberBySfc_New.h"
#include "ActionApp.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_GetNumberBySfc_New)
//////////////////////////////////////////////////////////////////////////
CMES_GetNumberBySfc_New::CMES_GetNumberBySfc_New(void)
{
}

CMES_GetNumberBySfc_New::~CMES_GetNumberBySfc_New(void)
{
}

BOOL CMES_GetNumberBySfc_New::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_GetNumberBySfc_New::__PollAction(void)
{
	extern CActionApp  myApp;
	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	char szSFC[128]={0},szMsg[512]={0},szNumber[512]={0},szNumberStore[128]={0},szModel[128]={0},szCustomData[512]={0};

	SPRESULT sp_result = MES_GetSN(szSFC,sizeof(szSFC));
	if (SP_OK != sp_result)
	{
		return sp_result;
	}

	/************************************************************************************
	函数：	GetNumberBySfc_New
	功能：	通过SFC分配号码
	参数：
			parSFC[]			SFC
			parNumberStore[]	号码库的名称。（若有网标，此处也可传入网标）SN，IMEI1,IMEI2，BT，WIFI,KEYBOX, LUCKY,Battery
			parModel[]			需要分配的号码属于那个“MODEL”。(号码库有MODEL必选，无MODEL不选，不选传NULL)
			parModuleID[]       模块ID(可选，不选传NULL)
			parCustomStatus[]	自定义状态(可选，不选传NULL)
			parRemak[]			备注(可选，不选传NULL)
			*retNumber			返回号码信息，格式为“号码名称1:号码值1;……;号码名称n:号码值n”
			*retMessage			返回的错误消息
	返回：	如果成功则返回1，否则返回0
	Data_API int GetNumberBySfc_New (char parSFC[], char parNumberStore[], char parModel[], char parModuleID[], char parCustomStatus[], char parRemark[], char *retNumber
	, char *retMessage);
	************************************************************************************/

	/************************************************************************************
	函数：	GetCustomDatabyShoporder_New
	功能：	通过工单获得项目、产品（定制料号）和工单的自定义数据
	参数：
	*retCustomData		成功时返回的自定义数据（格式为“名称≡值≡描述|……|名称≡值≡描述”）
	*retMessage			失败时返回的错误信息
	返回：	如果成功则返回1，否则返回0
	Data_API int GetCustomDatabyShoporder_New(char *retCustomData, char *retMessage);
	备注：	工单、产品（定制料号）ID、项目ID从\cfg\mes_config.ini文件中的Resource、PRODUCT_ID、PROJECT_ID中获取
	************************************************************************************/
	BOOL bOK = m_pMesDrv->GetCustomDatabyShoporder_New(szCustomData, szMsg);
	if (TRUE != bOK)
	{
		NOTIFY("MES_GetCustomDatabyShoporder_New", LEVEL_ITEM, 1, 0, 1, NULL, -1, szMsg);
		return SP_E_FAIL;
	}	
	NOTIFY("MES_GetCustomDatabyShoporder_New", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	LogFmtStrA(SPLOGLV_INFO, "MES_GetCustomDatabyShoporder_New(%s)",szCustomData);
	//从szCustomData中获取Number_store和Model
	bOK = GetInfoFromCustomData(szCustomData,"number_store",szNumberStore,sizeof(szNumberStore));
	if (TRUE != bOK)
	{
		NOTIFY("MES_GetNumberBySfc_New", LEVEL_ITEM, 1, 0, 1, NULL, -1, "GetInfoFromCustomData(number_store)");
		return SP_E_FAIL;
	}

	bOK = GetInfoFromCustomData(szCustomData,"model",szModel,sizeof(szModel));
	if (TRUE != bOK)
	{
		NOTIFY("MES_GetNumberBySfc_New", LEVEL_ITEM, 1, 0, 1, NULL, -1, "GetInfoFromCustomData(model)");
		return SP_E_FAIL;
	}

	bOK = m_pMesDrv->GetNumberBySfc_New(szSFC, szNumberStore, szModel, NULL, NULL,NULL,szNumber,szMsg);
	if (TRUE != bOK)
	{
		NOTIFY("MES_GetNumberBySfc_New", LEVEL_ITEM, 1, 0, 1, NULL, -1, szMsg);
		return SP_E_FAIL;
	}
	CHKRESULT(SetShareMemory(ShareMemory_BydNumber, (const void*)szNumber, sizeof(szNumber)));
	NOTIFY("MES_GetNumberBySfc_New", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	LogFmtStrA(SPLOGLV_INFO,  "MES_GetNumberBySfc_New(%s)",szNumber);
	return SP_OK;
}

