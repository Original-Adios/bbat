#ifndef __IMESSDLL_H__
#define __IMESSDLL_H__

#ifdef MESSDLL_EXPORTS
#define MESSDLL  extern "C" __declspec(dllexport) 
#else
#define MESSDLL  extern "C" __declspec(dllimport) 
#endif

MESSDLL BOOL _CreateMesDllObject(HANDLE pSpLog, HANDLE **pHandle);
MESSDLL void _ReleaseMesDllObject(HANDLE pHandle);
MESSDLL BOOL _Connect(HANDLE pHandle, LPCWSTR lpwMesMode, LPCWSTR lpwIP, LPCWSTR lpwDatabase, LPCWSTR lpwUser, LPCWSTR lpwPassword);
MESSDLL BOOL _Close(HANDLE pHandle);
MESSDLL BOOL _CheckFlow(HANDLE pHandle, LPCWSTR lpwSN);//input SN1
MESSDLL BOOL _SendTestResult(HANDLE pHandle, LPCWSTR lpwSN, LPCWSTR lpwBatchName, UINT32 Result, LPCWSTR lpwToolsVersion, UINT32 ErrCode, LPCWSTR lpwErrMsg, LPCWSTR lpwIP, UINT32 ElapsedTime, UINT8 StationID, UINT8 Operator);
MESSDLL BOOL _UploadDataLog(HANDLE pHandle, LPCWSTR lpcPathDataLog);

#endif // __IMESSDLL_H__