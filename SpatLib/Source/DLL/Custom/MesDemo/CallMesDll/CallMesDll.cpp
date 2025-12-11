#include "StdAfx.h"
#include "CallMesDll.h"
#include "XCallMesDll.h"



#define CXCallMesDllSafe(pHandle)    if (NULL == pHandle)   return FALSE;

CCallMesDll::CCallMesDll(void)
{
}


CCallMesDll::~CCallMesDll(void)
{
}


MESSDLL BOOL _CreateMesDllObject(HANDLE pSpLog, HANDLE **pHandle)
{
	*pHandle = (HANDLE*)reinterpret_cast<HANDLE>(new CXCallMesDll((ISpLog*)pSpLog));
	CXCallMesDllSafe(pHandle);
	return TRUE;
}
MESSDLL void _ReleaseMesDllObject(HANDLE pHandle)
{
	if (pHandle)
	{
		delete pHandle;
	}
}

MESSDLL BOOL _Connect(HANDLE pHandle, LPCWSTR lpwMesMode, LPCWSTR lpwIP, LPCWSTR lpwDatabase, LPCWSTR lpwUser, LPCWSTR lpwPassword)
{
	CXCallMesDllSafe(pHandle);

	return ((CXCallMesDll*)pHandle)->Connect(lpwMesMode, lpwIP, lpwDatabase, lpwUser, lpwPassword);
}

MESSDLL BOOL _Close(HANDLE pHandle)
{
	CXCallMesDllSafe(pHandle);
	return ((CXCallMesDll*)pHandle)->Close();
}
MESSDLL BOOL _CheckFlow(HANDLE pHandle, LPCWSTR lpwSN)
{
	CXCallMesDllSafe(pHandle);
	return ((CXCallMesDll*)pHandle)->CheckFlow(lpwSN);
}
MESSDLL BOOL _SendTestResult(HANDLE pHandle, LPCWSTR lpwSN, LPCWSTR lpwBatchName, UINT32 Result, LPCWSTR lpwToolsVersion, UINT32 ErrCode, LPCWSTR lpwErrMsg, LPCWSTR lpwIP, UINT32 ElapsedTime, UINT8 StationID, UINT8 Operator)
{
	CXCallMesDllSafe(pHandle);
	return ((CXCallMesDll*)pHandle)->SendTestResult(lpwSN, lpwBatchName, Result, lpwToolsVersion, ErrCode, lpwErrMsg, lpwIP, ElapsedTime, StationID, Operator);
}
MESSDLL BOOL _UploadDataLog(HANDLE pHandle, LPCWSTR lpcPathDataLog)
{
	CXCallMesDllSafe(pHandle);
	return ((CXCallMesDll*)pHandle)->UploadDataLog(lpcPathDataLog);
}
