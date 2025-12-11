#pragma once
#include <Windows.h>
#include <string>
#include "drvdef.h"

#define ShareMemory_MesDriver    L"MesDriverObject"

//////////////////////////////////////////////////////////////////////////
class CMesDriver sealed
{
public:
    CMesDriver(void);
   ~CMesDriver(void);

    // Load or free CustomerSolutionMes.dll
    BOOL Startup(std::wstring& strDLL);
    void Cleanup(void);

    // Invoke interfaces CustomerSolutionMes.dll
	BOOL _Handle_Create(HANDLE pSpLog, HANDLE **pHandle);
	void _Handle_ReleaseAll(HANDLE pHandle);
	BOOL _Login(HANDLE pHandle, LPCWSTR lpwMesMode, LPCWSTR lpwIP, LPCWSTR lpwDatabase, LPCWSTR lpwUser, LPCWSTR lpwPassword);
	BOOL _Logout(HANDLE pHandle);
	BOOL _CheckFlow(HANDLE pHandle, LPCWSTR lpwSN);
	BOOL _SendTestResult(HANDLE pHandle, LPCWSTR lpwSN, LPCWSTR lpwBatchName, UINT32 Result, LPCWSTR lpwToolsVersion, UINT32 ErrCode, LPCWSTR lpwErrMsg, LPCWSTR lpwIP, UINT32 ElapsedTime, UINT8 StationID, UINT8 Operator);
	BOOL _UploadDataLog(HANDLE pHandle, LPCWSTR lpcPathDataLog);

	MES_DRIVER_T&GetDrvObject(void) { return m_drv; };

	HANDLE GetMesHandle(void) { return m_hMes; };
	void SetMesHandle(HANDLE hMes){m_hMes = hMes;};
private:
    MES_DRIVER_T m_drv;
    HMODULE m_hDLL;
	HANDLE m_hMes;
    CHAR m_szErrMsg[512];
};
