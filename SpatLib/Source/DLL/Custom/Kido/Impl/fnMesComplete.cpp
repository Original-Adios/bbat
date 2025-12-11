#include "StdAfx.h"
#include "fnMesComplete.h"
#include "ActionApp.h"
//
IMPLEMENT_RUNTIME_CLASS(CfnMesComplete)
//////////////////////////////////////////////////////////////////////////
CfnMesComplete::CfnMesComplete(void)
{
}

CfnMesComplete::~CfnMesComplete(void)
{
}

SPRESULT CfnMesComplete::__PollAction(void)
{
	extern CActionApp  myApp;
	CONST CHAR ITEM_NAME[] = "fnMesComplete"; 
	SPRESULT sp_result = SP_OK;

	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	KIDOMES_T m_kidoMes;
	sp_result = GetShareMemory(ShareMemory_KidoMesConfig, (void*)&m_kidoMes, sizeof(m_kidoMes));
	if (SP_OK != sp_result)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_KidoMesConfig");
		return sp_result;
	}

	char szSN[64] = {0};
	sp_result = MES_GetSN(szSN, sizeof(szSN));
	if (SP_OK != sp_result)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES_GetSN");
		return sp_result;
	}

	char szToolName[64] = {0}, szToolVer[64] = {0};
	sp_result = Get_Tool_Info(szToolName, szToolVer);
	if (SP_OK != sp_result)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get_Tool_Info");
		return sp_result;
	}

	SPTEST_RESULT_T simba_result;
	sp_result = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&simba_result, sizeof(simba_result));
	if (SP_OK != sp_result)
	{   
		return sp_result;
	}

	char szTestItem[128] = {0}, szErrorMsg[128] = {0};
	int iResult = simba_result.errCode == SP_OK? 1 : 0;
	strcpy_s(szTestItem, sizeof(szTestItem), simba_result.szFailItem);
	strcpy_s(szErrorMsg, sizeof(szErrorMsg), simba_result.szDescription);

	char szSend[512] = {0}, szRecv[512] = {0};
	sprintf_s(szSend,sizeof(szSend),"ZS_STA:%s;ORDER:%s;SN:%s;NGTYPE:%s;NGCOMM:%s;NGCODE:%d;NGITEM:%s;NGVAL:%s;FIX_NUM:%s;LINE:%s;OPERATOR:%s;SPATH:%s;MODEL:%s;TOOLVER:%s;", 
		m_kidoMes.Station, m_kidoMes.Batch, szSN, "", szTestItem,szErrorMsg, szTestItem,"", m_kidoMes.LineNumber, m_kidoMes.LineNumber, m_kidoMes.Operator, m_kidoMes.CurrPath, m_kidoMes.ProductName, szToolVer);
	if (TRUE != m_pMesDrv->fnMesComplete(szSend,iResult, szRecv,m_kidoMes.type))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, szRecv);
		return SP_E_FAIL;
	}

	m_pMesDrv->Cleanup();
	delete m_pMesDrv;
	m_pMesDrv = NULL;
	CHKRESULT(SetMesDriverIntoShareMemory());

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	return SP_OK;
}

