#include "StdAfx.h"
#include "fnMesStart.h"
#include "ActionApp.h"
//
IMPLEMENT_RUNTIME_CLASS(CfnMesStart)
//////////////////////////////////////////////////////////////////////////
CfnMesStart::CfnMesStart(void)
{
}

CfnMesStart::~CfnMesStart(void)
{
}

BOOL CfnMesStart::LoadXMLConfig(void)
{
	m_kidoMes.type = (uint8)GetConfigValue(L"Option:Mes:Type", 1);
	std::string strProductName = _W2CA(GetConfigValue(L"Option:Mes:ProductName", L""));
	if (strProductName.empty() || strProductName.length() > 128)
	{
		return false;
	}
	memcpy(m_kidoMes.ProductName, strProductName.c_str(), strProductName.length());

	std::string strBatch = _W2CA(GetConfigValue(L"Option:Mes:Batch", L""));
	if (strBatch.empty() || strBatch.length() > 128)
	{
		return false;
	}
	memcpy(m_kidoMes.Batch, strBatch.c_str(), strBatch.length());

	std::string strStation = _W2CA(GetConfigValue(L"Option:Mes:Station", L""));
	if (strStation.empty() || strStation.length() > 128)
	{
		return false;
	}
	memcpy(m_kidoMes.Station, strStation.c_str(), strStation.length());

	std::string strLineNumber = _W2CA(GetConfigValue(L"Option:Mes:LineNumber", L""));
	if (strLineNumber.empty() || strLineNumber.length() > 128)
	{
		return false;
	}
	memcpy(m_kidoMes.LineNumber, strLineNumber.c_str(), strLineNumber.length());

	std::string strOperator = _W2CA(GetConfigValue(L"Option:Mes:Operator", L""));
	if (strOperator.empty() || strOperator.length() > 128)
	{
		return false;
	}
	memcpy(m_kidoMes.Operator, strOperator.c_str(), strOperator.length());
    return TRUE;
}

SPRESULT CfnMesStart::__PollAction(void)
{
	extern CActionApp  myApp;
	CONST CHAR ITEM_NAME[] = "fnMesStart"; 
	SPRESULT sp_result = SP_OK;
	if (SP_OK != GetMesDriverFromShareMemory())
	{
		m_pMesDrv = new CMesDriver();
		CHKRESULT(SetMesDriverIntoShareMemory());
	}

	std::wstring strDrvPath = (std::wstring)myApp.GetAppPath() + L"\\..\\MES\\Kidosc";
	sprintf_s(m_kidoMes.CurrPath, sizeof(m_kidoMes.CurrPath), "%s", _W2CA(strDrvPath.c_str()));
	sp_result = SetShareMemory(ShareMemory_KidoMesConfig, (void*)&m_kidoMes, sizeof(m_kidoMes), IContainer::System);
	if (SP_OK != sp_result)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_KidoMesConfig");
		return sp_result;
	}

	strDrvPath += L"\\MesCtrl.dll";
	CHKRESULT_WITH_NOTIFY(m_pMesDrv->Startup(strDrvPath) ? SP_OK : SP_E_LOAD_LIBRARY, "Load MesCtrl.dll");


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

	char szSend[512] = {0}, szRecv[512] = {0};
	sprintf_s(szSend, sizeof(szSend),"ZS_STA:%s;ORDER:%s;SN:%s;LINE:%s;OPERATOR:%s;SPATH:%s;MODEL:%s;TOOLVER:%s;", 
		m_kidoMes.Station, m_kidoMes.Batch, szSN, m_kidoMes.LineNumber, m_kidoMes.Operator, m_kidoMes.CurrPath, m_kidoMes.ProductName, szToolVer);

	if (TRUE != m_pMesDrv->fnMesStart(szSend,szRecv, m_kidoMes.type))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szRecv);
		return SP_E_FAIL;
	}
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
    return SP_OK;
}

