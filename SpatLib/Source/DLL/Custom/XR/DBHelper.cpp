#include "stdafx.h"
#include "DBHelper.h"

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

BOOL CDBHelper::Insert(std::string& strSQL)
{
    if (   NULL == m_pConnection 
        || adStateClosed == m_pConnection->GetState()
        )
    {
        CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: Database is disconnected.", __FUNCTION__);
        return FALSE;
    }

    try
    {
        m_pConnection->BeginTrans();

        _variant_t vRowCount(0);
        m_pConnection->Execute((_bstr_t)strSQL.c_str(), &vRowCount, adCmdText);
        if (1 != vRowCount.lVal)
        {
            m_pConnection->RollbackTrans();
        }

        m_pConnection->CommitTrans();
        return TRUE;
    }
    catch (_com_error& e)  //lint !e1752
    {
        CDBHelperSafeLog->LogFmtStrA(SPLOGLV_ERROR, "%s: %s", __FUNCTION__, (CHAR* )e.Description());
        return FALSE;
    }
}

