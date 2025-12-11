#include "stdafx.h"
#include "Sql_Imp.h"
#include<fstream>


//////////////////////////////////////////////////////////////////////////
	//
CSql_Imp::CSql_Imp(ISpLog* pTr)
	:m_lpTr(pTr)
{
	m_pConnection = NULL;
	m_pRecordset = NULL;
}

CSql_Imp::~CSql_Imp()
{	
}
void CSql_Imp::NewGUID(char* pszGUID, UINT32 u32Size)
{
	if (NULL != pszGUID)
	{
		GUID guid;
		if (S_OK == ::CoCreateGuid(&guid))  
		{  
			sprintf_s(pszGUID, u32Size
				, "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"  
				, guid.Data1  
				, guid.Data2  
				, guid.Data3  
				, guid.Data4[0], guid.Data4[1]  
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]  
			, guid.Data4[6], guid.Data4[7]  
			);
		}  
		else
		{
			pszGUID[0] = '\0';
		}
	}
}
BOOL CSql_Imp::ConnectServer(std::string strType, std::string strIP, std::string strDatabase, std::string strUser, std::string strPassword, std::string strDriver, std::string &strErrMsg)
{
	if (m_pConnection != NULL)
	{
		CloseServer();
	}
	m_pConnection.CreateInstance(__uuidof(Connection));
	m_pRecordset.CreateInstance(__uuidof(Recordset));


	char szConnection[PATH_MAX] = {0};

	ZeroMemory(szConnection, sizeof(szConnection));

	if (!strcmp(strType.c_str(), "SQL"))
	{
		strcpy_s(szConnection, PATH_MAX, ("driver={SQL Server};Server="));
		strcat_s(szConnection, PATH_MAX, strIP.c_str());
		strcat_s(szConnection, PATH_MAX, (";DATABASE="));
		strcat_s(szConnection, PATH_MAX, strDatabase.c_str());
		strcat_s(szConnection, PATH_MAX, (";UID="));
		strcat_s(szConnection, PATH_MAX, strUser.c_str());
		strcat_s(szConnection, PATH_MAX, (";PWD="));
		strcat_s(szConnection, PATH_MAX, strPassword.c_str());
	}
	else
	{
		sprintf_s(szConnection, PATH_MAX, ("driver={%s};Server=%s"), strDriver.c_str(), strIP.c_str());
		strcat_s(szConnection, PATH_MAX, (";DATABASE="));
		strcat_s(szConnection, PATH_MAX, strDatabase.c_str());
		strcat_s(szConnection, PATH_MAX, (";UID="));
		strcat_s(szConnection, PATH_MAX, strUser.c_str());
		strcat_s(szConnection, PATH_MAX, (";PWD="));
		strcat_s(szConnection, PATH_MAX, strPassword.c_str());
	}		
	
	m_pConnection->CursorLocation = adUseClient;
	m_pConnection->ConnectionTimeout = 10;
	m_pConnection->ConnectionString = _bstr_t(szConnection);
	m_lpTr->LogFmtStrA(SPLOGLV_INFO, "m_pConnection:%d, %s", m_pConnection, szConnection);
	try
	{
		m_pConnection->Open((""), (""), (""), adModeUnknown);
	}
	catch (_com_error &e)
	{
		strErrMsg = e.Description();
		return FALSE;
	}

	return TRUE;
}

void CSql_Imp::CloseServer(void)
{
	if(NULL != m_pRecordset)
	{
		if(adStateClosed != m_pRecordset->GetState())
		{
			m_pRecordset->Close();
		}
		m_pRecordset.Release();
		m_pRecordset = NULL;
	}

	if(NULL != m_pConnection)
	{
		if(adStateClosed != m_pConnection->GetState())
		{
			m_pConnection->Close();
		}
		m_pConnection.Release();
		m_pConnection = NULL;
	}

	return;
}

BOOL CSql_Imp::CheckTpControlBySn(std::string strSn, std::string &strErrMsg)
{
	char sqlcmd[MAX_SEND_BUFFER] = {0};

	sprintf_s(sqlcmd, MAX_SEND_BUFFER, "select ID from TpControl where SN = '%s'", strSn.c_str());

	m_lpTr->LogFmtStrA(SPLOGLV_INFO, "m_pConnection:%d, %s", m_pConnection, sqlcmd);
	try
	{
		_variant_t rowef;
		_RecordsetPtr record(__uuidof(Recordset));
		record = m_pConnection->Execute(sqlcmd, &rowef, adCmdText);
		int rownum = record->GetRecordCount();
		if(rownum == 0)
		{
			return TRUE;
		}
	}
	catch(_com_error &e)
	{
		strErrMsg = e.Description();
		return FALSE;
	}

	strErrMsg = strSn;
	strErrMsg += " Has Already Saved in TpControl";
	return FALSE;
}

BOOL CSql_Imp::CheckTpInputBySn(std::string strSn, std::string &strErrMsg)
{
	char sqlcmd[MAX_SEND_BUFFER] = {0};

	sprintf_s(sqlcmd, MAX_SEND_BUFFER, "select ID from TpInput where SN = '%s'", strSn.c_str());

	m_lpTr->LogFmtStrA(SPLOGLV_INFO, "m_pConnection:%d, %s", m_pConnection, sqlcmd);
	try
	{
		_variant_t rowef;
		_RecordsetPtr record(__uuidof(Recordset));
		record = m_pConnection->Execute(sqlcmd, &rowef, adCmdText);
		int rownum = record->GetRecordCount();
		if(rownum == 0)
		{
			strErrMsg = "Record is null! By";
			strErrMsg += sqlcmd;
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
		strErrMsg = e.Description();
		return FALSE;
	}

	return TRUE;
}
BOOL CSql_Imp::QueryCCFlagFrTpPlanByBatchName(std::string strBatchName, std::string &strCCFlag, std::string &strDetailModel, std::string &strErrMsg)
{
	char sqlcmd[MAX_SEND_BUFFER] = {0};

	sprintf_s(sqlcmd, MAX_SEND_BUFFER, "select CCFlag,DetailModel from TpPlan where SMO = '%s' ORDER BY ID DESC", strBatchName.c_str());

	m_lpTr->LogFmtStrA(SPLOGLV_INFO, "m_pConnection:%d, %s", m_pConnection, sqlcmd);
	try
	{
		_variant_t rowef;
		_RecordsetPtr record(__uuidof(Recordset));
		record = m_pConnection->Execute(sqlcmd, &rowef, adCmdText);
		int rownum = record->GetRecordCount();
		if(rownum == 0)
		{
			strErrMsg = "Record is null! By";
			strErrMsg += sqlcmd;
			return FALSE;
		}

		_variant_t temp;
//		while(!record->EndOfFile)
		{
			temp = record->GetCollect("CCFlag");
			strCCFlag = (char*)(_bstr_t)temp;
			temp = record->GetCollect("DetailModel");
			strDetailModel = (char*)(_bstr_t)temp;
//			record->MoveNext();
		}

	}
	catch(_com_error &e)
	{
		strErrMsg = e.Description();
		return FALSE;
	}

	return TRUE;
}
BOOL CSql_Imp::AddInfo2TpCountryCode(std::string strSn, std::string strSoftwarePN, std::string strCountryCode, std::string strBatchName,std::string strCCFlag, std::string strDetailModel, std::string &strErrMsg)
{
	char sqlcmd[MAX_SEND_BUFFER] = {0};

	sprintf_s(sqlcmd, MAX_SEND_BUFFER, "Insert TpCountryCode(SN,SoftwarePN,CountryCode,SMO,CCFlag,DetailModel) values ('%s','%s','%s','%s','%s','%s')",
		strSn.c_str(), strSoftwarePN.c_str(), strCountryCode.c_str(), strBatchName.c_str(), strCCFlag.c_str(), strDetailModel.c_str());

	m_lpTr->LogFmtStrA(SPLOGLV_INFO, "m_pConnection:%d, %s", m_pConnection, sqlcmd);
	try
	{
		_variant_t rowef;
		m_pConnection->Execute(sqlcmd, &rowef, adCmdText);
	}
	catch(_com_error &e)
	{
		strErrMsg = e.Description();
		return FALSE;
	}

	return TRUE;
}
BOOL CSql_Imp::AddInfo2CCDeleteRecord(std::string strSn, std::string strSoftwarePN, std::string strMO, int nUpdateFlag, std::string &strErrMsg)
{
	char sqlcmd[MAX_SEND_BUFFER] = {0};

	sprintf_s(sqlcmd, MAX_SEND_BUFFER, "Insert CCDeleteRecord(MO,SN,SoftwarePN,UpdateFlag) select '%s','%s','%s',%d",
		strMO.c_str(), strSn.c_str(), strSoftwarePN.c_str(), nUpdateFlag);

	m_lpTr->LogFmtStrA(SPLOGLV_INFO, "m_pConnection:%d, %s", m_pConnection, sqlcmd);
	try
	{
		_variant_t rowef;
		m_pConnection->Execute(sqlcmd, &rowef, adCmdText);
	}
	catch(_com_error &e)
	{
		strErrMsg = e.Description();
		return FALSE;
	}

	return TRUE;
}
