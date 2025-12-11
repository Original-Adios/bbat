#pragma once
#include "ISpLogExport.h"
#include <string>
#pragma warning (disable:4146)
#import "./ado/msadox.dll"   //no_namespace rename("EOF","adoEOF")
#import "./ado/msado15.dll" rename_namespace("ADOCG") rename("EOF", "EndOfFile")// for ado
using namespace ADOCG;
#pragma warning (default:4146)

//////////////////////////////////////////////////////////////////////////
class CDBHelper sealed
{
public:
    // MDB: Provider=Microsoft.Jet.OLEDB.4.0;Data Source=
    CDBHelper(std::wstring& strConn, ISpLog* pLogUtil = NULL);
   ~CDBHelper(void);

   // "SELECT * FROM IMEI where %s =\'%s\'"
    BOOL QueryCount(std::string& strSQL, LONG& lCount);
    BOOL Insert(std::string& strSQL);

private:
    BOOL Connect(void);
    void DisConn(void);

private:
    _ConnectionPtr	m_pConnection;
    _RecordsetPtr	m_pRecordset;
    ISpLog*         m_pLogUtil;
    std::wstring    m_strConn;
};