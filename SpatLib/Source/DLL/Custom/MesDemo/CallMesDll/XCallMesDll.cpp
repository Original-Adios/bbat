#include "StdAfx.h"
#include "XCallMesDll.h"

#include <Windows.h>
#pragma comment(lib,"Version.lib")

#include "fstream"
using namespace std;

const uint16 MAX_BUFF_LENGTH = 1024;
#define CDBHelperSafeLog    if (NULL != m_pLogUtil)   m_pLogUtil

#define CXCallMesDllSafeClosePtr(p)  \
	if (NULL != p) { \
		if (adStateClosed != p->GetState()) { \
		p->Close(); \
		} \
	}

CXCallMesDll::CXCallMesDll(ISpLog* pLogUtil)
	:m_pLogUtil(pLogUtil)
{
	m_pConnection = NULL;
	m_pRecordset = NULL;
    ::CoInitialize(NULL);
}

CXCallMesDll::~CXCallMesDll(void)
{
}

BOOL CXCallMesDll::Connect(LPCWSTR lpwMesMode, LPCWSTR lpwIP, LPCWSTR lpwDatabase, LPCWSTR lpwUser, LPCWSTR lpwPassword)
{
	::CoInitialize(NULL);
	m_pConnection.CreateInstance(__uuidof(Connection));
	m_pRecordset.CreateInstance(__uuidof(Recordset));
	WCHAR szConnection[MAX_PATH] = {0};

	if (!wcscmp(lpwMesMode, L"SQL"))
	{
		swprintf_s(szConnection, L"driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", \
			lpwIP, lpwDatabase,lpwUser,lpwPassword);
	}
	else
	{
		swprintf_s(szConnection, L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s", lpwDatabase);
	}

	m_pConnection->CursorLocation = adUseClient;
	m_pConnection->ConnectionTimeout = 10;
	m_pConnection->ConnectionString = _bstr_t(szConnection);

	CDBHelperSafeLog->LogFmtStrW(SPLOGLV_INFO, szConnection);
	try
	{
		m_pConnection->Open((""), (""), (""), adModeUnknown);
	}
	catch (_com_error &e)
	{
		CDBHelperSafeLog->LogFmtStrW(SPLOGLV_INFO, (WCHAR*)e.Description());
		return FALSE;
	}

	return TRUE;
}

BOOL CXCallMesDll::Close(void)
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

    ::CoUninitialize();
	return TRUE;
}

/*//////////////////////////////////////////////////////////////////////////
Table:TestResult_Table
ITEM:[SN1],[Result]
[Result]:1,已写过，0,解绑或未写
return;TRUE,未过站，FAIL，已过站
//////////////////////////////////////////////////////////////////////////*/
BOOL CXCallMesDll::CheckFlow(LPCWSTR lpwSN)
{
	if (NULL == m_pConnection || adStateClosed == m_pConnection->GetState())
	{
		CDBHelperSafeLog->LogFmtStrW(SPLOGLV_ERROR, L"Database is disconnected");
		return FALSE;
	}

	CXCallMesDllSafeClosePtr(m_pRecordset);

	WCHAR szCmd[MAX_BUFF_LENGTH] = {0};
	swprintf_s(szCmd, L"SELECT SN FROM TestResult_Table WHERE SN = '%s' and Result = 1", lpwSN);

	CDBHelperSafeLog->LogFmtStrW(SPLOGLV_INFO, szCmd);

	try
	{
		m_pRecordset->Open((_bstr_t)szCmd, _variant_t((IDispatch*)m_pConnection, TRUE), adOpenStatic, adLockOptimistic, adCmdText);
		LONG lRowCount = m_pRecordset->GetRecordCount();
		m_pRecordset->Close();
		if (0 == lRowCount)
		{
			return TRUE;
		}
	}
	catch (_com_error& e)
	{
		CDBHelperSafeLog->LogFmtStrW(SPLOGLV_INFO, (WCHAR*)e.Description());
		return FALSE;
	}

	return FALSE;
}

/*//////////////////////////////////////////////////////////////////////////
Table:TestResult_Table
ITEM:[SN1],[BatchName],[Result],[ToolsVersion],[ErrCode],[ErrMsg],[IP],[StartTestTime],[EndTestTime],[StationID],[Operator]
[Result]:1,Success，0,Fail
return;TRUE,Write Success;FAIL，Write Fail
//////////////////////////////////////////////////////////////////////////*/
BOOL CXCallMesDll::SendTestResult(LPCWSTR lpwSN, LPCWSTR lpwBatchName, UINT32 Result, LPCWSTR lpwToolsVersion, UINT32 ErrCode,
	LPCWSTR lpwErrMsg, LPCWSTR lpwIP, UINT32 ElapsedTime, UINT8 StationID, UINT8 Operator)
{
	if (NULL == m_pConnection || adStateClosed == m_pConnection->GetState())
	{
		CDBHelperSafeLog->LogFmtStrW(SPLOGLV_ERROR, L"Database is disconnected");
		return FALSE;
	}

	CXCallMesDllSafeClosePtr(m_pRecordset);

	WCHAR szCmd[MAX_BUFF_LENGTH] = {0};
	swprintf_s(szCmd, L"INSERT TestResult_Table(SN,BatchName,Result,ToolsVersion,ErrCode,ErrMsg,IP,Elapsed,StationID,Operator) VALUES ('%s', '%s', %d, '%s', %d, '%s', '%s', %d, %d, %d)",
					   lpwSN, lpwBatchName, Result, lpwToolsVersion, ErrCode, lpwErrMsg, lpwIP, ElapsedTime, StationID, Operator);

	CDBHelperSafeLog->LogFmtStrW(SPLOGLV_INFO, szCmd);

	try
	{
		m_pConnection->BeginTrans();

		_variant_t vRowCount(0);
		m_pConnection->Execute((_bstr_t)szCmd, &vRowCount, adCmdText);
		if (1 != vRowCount.lVal)
		{
			m_pConnection->RollbackTrans();
			return FALSE;
		}

		m_pConnection->CommitTrans();
	}
	catch (_com_error& e)
	{
		m_pConnection->RollbackTrans();
		CDBHelperSafeLog->LogFmtStrW(SPLOGLV_INFO, (WCHAR*)e.Description());
		return FALSE;
	}
	
	return TRUE;
}

/*//////////////////////////////////////////////////////////////////////////
Table:TestResult_Table
ITEM:[BatchName],[SN],[Item],[Result],[Station],[TestValue],[LowLimit],[UpLimit],[Unit]
[Result]:PASS,FAIL
return;TRUE,Write Success;FAIL，Write Fail
//////////////////////////////////////////////////////////////////////////*/
CONST UINT32 COL_LENGTH = 64;
BOOL CXCallMesDll::UploadDataLog(LPCWSTR lpcPathDataLog)
{
	if (NULL == m_pConnection || adStateClosed == m_pConnection->GetState())
	{
		CDBHelperSafeLog->LogFmtStrW(SPLOGLV_ERROR, L"Database is disconnected");
		return FALSE;
	}

	CXCallMesDllSafeClosePtr(m_pRecordset);

	WCHAR szCmd[MAX_BUFF_LENGTH] = {0};

	WCHAR lpwBuffer[1024*10] = L"";
	ReadDataLog(lpcPathDataLog, lpwBuffer, 1024*10);

	
	WCHAR szSN[COL_LENGTH] = {0};
	WCHAR szBatch[COL_LENGTH] = {0};
	WCHAR szStation[COL_LENGTH] = {0};
	
	LPWSTR lpwData = lpwBuffer;
	LogStrstr(lpwData, L"SN,", szSN, COL_LENGTH);
	LogStrstr(lpwData, L"Batch,", szBatch, COL_LENGTH);
	LogStrstr(lpwData, L"Station,", szStation, COL_LENGTH);


	CONST WCHAR DATA_START[] = L"TestValue,LSL,USL,Unit,ElapsedTime";
	std::wstring strData = lpwBuffer;
	std::wstring::size_type pos = strData.find(DATA_START, 0);
	strData.erase(0, pos + wcslen(DATA_START)+1);
	strData += L"\n";


	try
	{
		m_pConnection->BeginTrans();
		while ((pos = strData.find(L"\n", 0)) != std::wstring::npos)
		{
			std::wstring strTemp = strData.substr(0, pos);
			if (0 == strTemp.length())
			{
				break;
			}
			std::wstring strItem;
			std::wstring strResult;
			std::wstring strValue;
			std::wstring strLowLimit;
			std::wstring strUpLimit;
			std::wstring strUnit;
			LogValueStrstr(strTemp.c_str(), strItem, strResult, strValue, strLowLimit, strUpLimit, strUnit);
			swprintf_s(szCmd, L"INSERT TestValue_Table(BatchName,SN,Item,Result,Station,TestValue,LowLimit,UpLimit,Unit) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
				szBatch, szSN, strItem.c_str(), strResult.c_str(), szStation, strValue.c_str(), strLowLimit.c_str(), strUpLimit.c_str(), strUnit.c_str());

			strData.erase(0, pos + 1);
			CDBHelperSafeLog->LogFmtStrW(SPLOGLV_INFO, szCmd);
			_variant_t vRowCount(0);
			m_pConnection->Execute((_bstr_t)szCmd, &vRowCount, adCmdText);
			if (1 != vRowCount.lVal)
			{
				m_pConnection->RollbackTrans();
			}
		}
		m_pConnection->CommitTrans();
		return TRUE;
	}
	catch (_com_error& e)
	{
		m_pConnection->RollbackTrans();
		CDBHelperSafeLog->LogFmtStrW(SPLOGLV_INFO, (WCHAR*)e.Description());
		return FALSE;
	}
	
	return FALSE;
}

BOOL CXCallMesDll::LogValueStrstr(LPCWSTR lpcData, std::wstring &strItem, std::wstring &strResult, std::wstring &strValue, 
	std::wstring &strLowLimit, std::wstring &strUpLimit, std::wstring &strUnit)
{
	
	std::wstring strData = lpcData;
	std::wstring::size_type pos = 0;
	int k = 0;
	while((pos = strData.find(L",", 0)) != std::wstring::npos)
	{
		std::wstring strTemp = strData.substr(0, pos);
		if (1 == k)
		{
			strItem = strTemp;
		}
		else if (6 == k)
		{
			strResult = strTemp;
		}
		else if (7 == k)
		{
			strValue = strTemp;
		}
		else if (8 == k)
		{
			strLowLimit = strTemp;
		}
		else if (9 == k)
		{
			strUpLimit = strTemp;
		}
		else if (10 == k)
		{
			strUnit = strTemp;
		}
		strData.erase(0, pos + 1);
		k++;
	}
	
	return TRUE;
}

BOOL CXCallMesDll::LogStrstr(LPCWSTR lpcData, LPCWSTR lpwStr, LPWSTR lpcBuff, size_t ubufLen)
{
	LPCWSTR tokenStart = wcsstr(lpcData, lpwStr);
	if (NULL == tokenStart)
	{
		return FALSE;
	}
	LPCWSTR tokenEnd = wcsstr(tokenStart, L"\n");
	if (NULL == tokenEnd)
	{
		return FALSE;
	}

	if (ubufLen < (tokenEnd - tokenStart - wcslen(lpwStr)))
	{
		return FALSE;
	}
	memcpy(lpcBuff, tokenStart + wcslen(lpwStr), sizeof(WCHAR)* (tokenEnd - tokenStart - wcslen(lpwStr)));
	return TRUE;
}
	
BOOL CXCallMesDll::ReadDataLog(LPCWSTR lpcPathDataLog, LPWSTR lpwBuf, size_t ubufLen)
{
	USES_CONVERSION;
	std::string	strMsg;
	ifstream fsdata;

	size_t iImeiSize  = 0;
	LPSTR lpImeiData = NULL;

	fsdata.open(W2A(lpcPathDataLog), ios::in);
	if (!fsdata)
	{
		return false;
	}
	fsdata.seekg(0, ios::end);
	iImeiSize = (size_t)fsdata.tellg();
	if (iImeiSize > ubufLen)
	{
		return false;
	}
	fsdata.seekg(0, ios::beg);
	lpImeiData = (char *)malloc(iImeiSize);
	if (!lpImeiData)
	{
		fsdata.close();
		return false;
	}
	memset(lpImeiData, 0, iImeiSize);
	fsdata.read(lpImeiData, iImeiSize);
	fsdata.close();
	lpImeiData[iImeiSize] = '\0';
	memcpy(lpwBuf, A2W(lpImeiData), sizeof(WCHAR) * iImeiSize);
	return TRUE;
}
