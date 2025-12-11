#pragma once
#include <iostream>
#import "..\ado\msadox.dll"   //no_namespace rename("EOF","adoEOF")
#import "..\ado\msado15.dll" rename_namespace("ADOCG") rename("EOF", "EndOfFile")// for ado
#include <Tr.h>
using namespace ADOCG;
using namespace std;

#define PATH_MAX 512
#define MAX_SEND_BUFFER 1024
#define MAX_LOTNAME_LENGTH (8+1)
#define MAX_IMSI_LENGTH (15+1)
#define MAX_ICCID_LENGTH (20+1)
#define MAX_MISSDN_LENGTH (13+1)
#define MAX_SN_LENGTH (22+1)
#define MAX_EID_LENGTH (20+1)

typedef enum
{
	SERVER_SQL = 0,
	SERVER_MYSQL
}EM_SERVER_TYPE;

#define ShareMemory_SQLDriver    L"SQLObject"
//////////////////////////////////////////////////////////////////////////
class CSql_Imp
{
public:
	CSql_Imp(ISpLog* pTr);
	virtual ~CSql_Imp();

	BOOL ConnectServer(std::string strType, std::string strIP, std::string strDatabase, std::string strUser, std::string strPassword, std::string strDriver, std::string &strErrMsg);
	BOOL CheckTpControlBySn(std::string strSn, std::string &strErrMsg);
	BOOL CheckTpInputBySn(std::string strSn, std::string &strErrMsg);
	BOOL QueryCCFlagFrTpPlanByBatchName(std::string strBatchName, std::string &strCCFlag, std::string &strDetailModel, std::string &strErrMsg);
	BOOL AddInfo2TpCountryCode(std::string strSn, std::string strSoftwarePN, std::string strCountryCode, std::string strBatchName,std::string strCCFlag, std::string strDetailModel, std::string &strErrMsg);
	BOOL AddInfo2CCDeleteRecord(std::string strSn, std::string strSoftwarePN, std::string strMO, int nUpdateFlag, std::string &strErrMsg);
	void CloseServer(void);
private:
	void NewGUID(char* pszGUID, UINT32 u32Size);	

	_ConnectionPtr  m_pConnection;
	_RecordsetPtr	m_pRecordset;

	ISpLog* m_lpTr;
};