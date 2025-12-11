#include "stdafx.h"
#include "SqlDisConnect.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSqlDisConnect)

	//
CSqlDisConnect::CSqlDisConnect()
{
}

CSqlDisConnect::~CSqlDisConnect()
{	
}

SPRESULT CSqlDisConnect::__PollAction(void)
{
	CONST CHAR * ACTION = "Sql_DisConnect";
	std::string strErrMsg;
	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "SqlDisConnect");	

	m_pSql_Imp->CloseServer();
	if (NULL != m_pSql_Imp)
	{
		delete m_pSql_Imp;
		m_pSql_Imp = NULL;
	}
	CHKRESULT_WITH_NOTIFY(SetMesDriverIntoShareMemory(), "SqlDisConnect");	
	NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "");
	return SP_OK;
}
