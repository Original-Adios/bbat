#include "stdafx.h"
#include "DBHelper.h"
#include "CLocks.h"

#define CDBHelperSafeLog    if (NULL != m_pLogUtil)   m_pLogUtil

#define CDBHelperSafeClosePtr(p)  \
    if (NULL != p) { \
        if (adStateClosed != p->GetState()) { \
            p->Close(); \
        } \
    }

//////////////////////////////////////////////////////////////////////////
CDBHelper::CDBHelper(std::wstring& strConn, ISpLog* pLogUtil)
    : m_strConn(strConn)
    , m_pLogUtil(pLogUtil)
    , m_pConnection(NULL)
    , m_pRecordset(NULL)
{
    // Make sure memory can be safely destroyed if throw exception in constructor.
    Connect();  
}

CDBHelper::~CDBHelper(void)
{
    // Forbid to throw exception in destructor
    DisConn();
}

BOOL CDBHelper::Connect(void)
{
    ::CoInitialize(NULL);

    m_pConnection.CreateInstance(__uuidof(Connection));
    m_pRecordset.CreateInstance(__uuidof(Recordset));
    m_pConnection->ConnectionTimeout = 10;
    m_pConnection->ConnectionString = _bstr_t(m_strConn.c_str());

    try
    {
        m_pConnection->Open(_T(""), _T(""), _T(""), adModeUnknown);
    }
    catch (_com_error &e)  //lint !e1752
    {
        CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%: %s", __FUNCTION__, (CHAR* )e.Description());
        return FALSE;
    }

    return TRUE;
}

void CDBHelper::DisConn(void)
{
    if (NULL != m_pRecordset)
    {
        if (adStateClosed != m_pRecordset->GetState())
        {
            m_pRecordset->Close();	
        }

        m_pRecordset.Release();
        m_pRecordset = NULL;
    }	
    if (NULL != m_pConnection)
    {
        if (adStateClosed != m_pConnection->GetState())
        {
            m_pConnection->Close();
        }

        m_pConnection.Release();
        m_pConnection = NULL;
    }

    ::CoUninitialize();
}

BOOL CDBHelper::QueryCount(std::string& strSQL, LONG& lCount)
{
    lCount = 0;

    if (   NULL == m_pConnection 
        || adStateClosed == m_pConnection->GetState()
        )
    {
        CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: Database is disconnected.", __FUNCTION__);
        return FALSE;
    }

    CDBHelperSafeClosePtr(m_pRecordset);

    try
    {
        m_pRecordset->Open((_bstr_t)strSQL.c_str(), _variant_t((IDispatch*)m_pConnection, TRUE), adOpenStatic, adLockOptimistic, adCmdText);
        lCount = m_pRecordset->GetRecordCount();
        m_pRecordset->Close();
        return TRUE;
    }
    catch (_com_error& e)   //lint !e1752
    {
        CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: %s", __FUNCTION__, (CHAR* )e.Description());
        return FALSE;
    } 
}

BOOL CDBHelper::QueryItem(const char *pCmd, std::string& strResult)
{

	if (NULL == m_pConnection || adStateClosed == m_pConnection->GetState())
	{
		CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: Database is disconnected.", __FUNCTION__);
		return FALSE;
	}

	CDBHelperSafeClosePtr(m_pRecordset);

	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, "%s", pCmd);
	try
	{
		m_pRecordset->Open((_bstr_t)pCmd, _variant_t((IDispatch*)m_pConnection, TRUE), adOpenStatic, adLockOptimistic, adCmdText);
		LONG lCount = m_pRecordset->GetRecordCount();
		if (1 != lCount)
		{
			CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: rows:%d is not 1", __FUNCTION__, lCount);
			return FALSE;
		}
		LONG nColCount = m_pRecordset->GetFields()->GetCount();
		std::string strTemp = "";
		for (LONG k = 0; k < nColCount; k++)
		{
			std::string strColName = (CHAR*)(m_pRecordset->GetFields()->GetItem(k)->Name);
			std::string strColValue = (CHAR*)(_bstr_t)(m_pRecordset->GetFields()->GetItem(k)->Value);
			strResult += (strColValue + ",");
			strTemp += (strColName + ":" + strColValue + ",");
		}

		strResult.erase(strResult.length()-1, 1);
		strTemp.erase(strTemp.length()-1, 1);
		CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, "%s", strTemp.c_str());
		m_pRecordset->Close();
		return TRUE;
	}
	catch (_com_error& e)
	{
		CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: %s", __FUNCTION__, (CHAR* )e.Description());
		return FALSE;
	} 
}



BOOL CDBHelper::Insert(std::string& strSQL)
{
    const int maxRetries = 3; // Maximum number of retries
    int retries = 0;

    while (retries < maxRetries) {
        std::lock_guard<std::mutex> lock(m_Lock);

        if (NULL == m_pConnection || adStateClosed == m_pConnection->GetState()) {
            CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: Database is disconnected.", __FUNCTION__);
            return FALSE;
        }

        try {
            m_pConnection->BeginTrans();
            _variant_t vRowCount(0);
            m_pConnection->Execute((_bstr_t)strSQL.c_str(), &vRowCount, adCmdText);
            if (1 != vRowCount.lVal) {
                m_pConnection->RollbackTrans();
            }
            m_pConnection->CommitTrans();
            return TRUE; // Successful insertion, exit the loop
        }
        catch (_com_error& e) {
            CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: %s", __FUNCTION__, (CHAR*)e.Description());
            if (m_pConnection->GetState() != adStateClosed) {
                m_pConnection->RollbackTrans(); // Rollback transaction if connection is still open
            }
            // Increment the retry counter
            retries++;
            if (retries < maxRetries) {
                CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, "%s: Retrying insert operation. Attempt %d.", __FUNCTION__, retries + 1);
            }
        }
    }

    // If we reach here, all attempts have failed
    CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: Insert operation failed after %d attempts.", __FUNCTION__, maxRetries);
    return FALSE;
}

