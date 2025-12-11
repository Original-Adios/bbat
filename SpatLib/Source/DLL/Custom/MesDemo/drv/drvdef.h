#pragma once

typedef BOOL  (*pCreateMesDllObject)(HANDLE pLogUtil, HANDLE **pHandle);
typedef BOOL  (*pReleaseMesDllObject)(HANDLE pHandle);

typedef BOOL  (*pConnect)(HANDLE pHandle, LPCWSTR lpwMesMode, LPCWSTR lpwIP, LPCWSTR lpwDatabase, LPCWSTR lpwUser, LPCWSTR lpwPassword);
typedef BOOL  (*pClose)(HANDLE pHandle);
typedef BOOL  (*pCheckFlow)(HANDLE pHandle, LPCWSTR lpwSN);//SN1 or IMEI1
typedef BOOL  (*pSendTestResult)(HANDLE pHandle, LPCWSTR lpwSN, LPCWSTR lpwBatchName, UINT32 Result, LPCWSTR lpwToolsVersion, UINT32 ErrCode, LPCWSTR lpwErrMsg, LPCWSTR lpwIP, UINT32 ElapsedTime, UINT8 StationID, UINT8 Operator);
typedef BOOL  (*pUploadDataLog)(HANDLE pHandle, LPCWSTR lpcPathDataLog);




typedef struct _tagMES_DRIVER_T
{
	pCreateMesDllObject _Handle_Create;
	pReleaseMesDllObject _Handle_ReleaseAll; 
	pConnect _Login;
	pClose _Logout;
	pCheckFlow _CheckFlow;
	pSendTestResult _SendTestResult;
	pUploadDataLog _UploadDataLog;

    _tagMES_DRIVER_T(void) {
        Reset();
    };

    void Reset(void) {
		_Handle_Create = NULL;
		_Handle_ReleaseAll = NULL;
		_Login = NULL;
		_Logout = NULL;	
		_CheckFlow = NULL;
		_SendTestResult = NULL;
		_UploadDataLog = NULL;
    };

    BOOL IsValid(void) {
        return ((NULL != _Handle_Create) && (NULL != _Handle_ReleaseAll) && (NULL != _Login) 
			&& (NULL != _Logout) && (NULL != _SendTestResult)
			&& (NULL != _UploadDataLog) && (NULL != _CheckFlow));
    };

} MES_DRIVER_T;

