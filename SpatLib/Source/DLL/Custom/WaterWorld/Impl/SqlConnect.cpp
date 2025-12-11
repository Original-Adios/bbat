#include "stdafx.h"
#include "SqlConnect.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSqlConnect)

CSqlConnect::CSqlConnect()
{
}
CSqlConnect::~CSqlConnect()
{	
}

BOOL CSqlConnect::LoadXMLConfig(void)
{
	m_strServerType = _W2CA(GetConfigValue(L"Option:Server:Type", L""));
	m_strServerIP = _W2CA(GetConfigValue(L"Option:Server:Server", L""));
	m_strServerDatabase = _W2CA(GetConfigValue(L"Option:Server:Database", L""));
	m_strServerUser = _W2CA(GetConfigValue(L"Option:Server:User", L""));
	m_strServerPassword = _W2CA(GetConfigValue(L"Option:Server:Password", L""));
	m_strServerDriver = _W2CA(GetConfigValue(L"Option:Server:ServerDriver", L""));

	m_strUrl = _W2CA(GetConfigValue(L"Option:WebApi:BaseApiUrl", L""));
	m_strToken = _W2CA(GetConfigValue(L"Option:WebApi:AccessToken", L""));
	m_strWebApi = _W2CA(GetConfigValue(L"Option:WebApi:WriteCardWebAPI", L""));
    m_strOTAWebApi = _W2CA(GetConfigValue(L"Option:WebApi:OTAResWebAPI", L""));
	return TRUE;
}

SPRESULT CSqlConnect::__PollAction(void)
{
	CONST CHAR * ACTION = "Sql_Connect";
	std::string strErrMsg;
	if (SP_OK != GetMesDriverFromShareMemory())
	{
		m_pSql_Imp = new CSql_Imp(GetISpLogObject());
	    LogFmtStrA(SPLOGLV_INFO, "Port: %d, m_pSql_Imp: %d", GetAdjParam().nTASK_ID, m_pSql_Imp);
		CHKRESULT(SetMesDriverIntoShareMemory());
	}
	
	if (TRUE != m_pSql_Imp->ConnectServer(m_strServerType, m_strServerIP, m_strServerDatabase,
		m_strServerUser, m_strServerPassword, m_strServerDriver, strErrMsg))
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "File of ConnectServer, %s!", strErrMsg.c_str());
		return SP_E_MES_ERROR;
	}


	NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "");
	return SP_OK;
}
