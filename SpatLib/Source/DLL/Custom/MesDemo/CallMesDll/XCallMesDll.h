#pragma once
#include "ICallMesdll.h"
#pragma warning (disable:4146)
#import "../../../NonRF/Provision/ado/msadox.dll"   //no_namespace rename("EOF","adoEOF")
#import "../../../NonRF/Provision/ado/msado15.dll" rename_namespace("ADOCG") rename("EOF", "EndOfFile")// for ado
using namespace ADOCG;


#include "ISpLogExport.h"
class CXCallMesDll sealed
{
public:
	CXCallMesDll(ISpLog* pLogUtil);
	virtual ~CXCallMesDll(void);

	virtual BOOL Connect(LPCWSTR lpwMesMode, LPCWSTR lpwIP, LPCWSTR lpwDatabase, LPCWSTR lpwUser, LPCWSTR lpwPassword);
	virtual BOOL Close(void);
	virtual BOOL CheckFlow(LPCWSTR lpwSN);    
	virtual BOOL SendTestResult(LPCWSTR lpwSN, LPCWSTR lpwBatchName, UINT32 Result, LPCWSTR lpwToolsVersion, UINT32 ErrCode, LPCWSTR lpwErrMsg, LPCWSTR lpwIP, UINT32 ElapsedTime, UINT8 StationID, UINT8 Operator);
	virtual BOOL UploadDataLog(LPCWSTR lpcPathDataLog);
private:
	BOOL ReadDataLog(LPCWSTR lpcPathDataLog, LPWSTR lpwBuf, size_t ubufLen);
	BOOL LogStrstr(LPCWSTR lpcData, LPCWSTR lpwStr, LPWSTR lpcBuff, size_t ubufLen);
	BOOL LogValueStrstr(LPCWSTR lpcData, std::wstring &strItem, std::wstring &strResult, std::wstring &strValue, 
		std::wstring &strLowLimit, std::wstring &strUpLimit, std::wstring &strUnit);
	ISpLog* m_pLogUtil;
	_ConnectionPtr  m_pConnection;
	_RecordsetPtr	m_pRecordset;
};

