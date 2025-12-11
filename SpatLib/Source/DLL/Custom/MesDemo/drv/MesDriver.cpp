#include "StdAfx.h"
#include "MesDriver.h"
#include <assert.h>

#ifdef _DEBUG
//#define _DUMMY_TINNO_MES_     
#endif

#ifdef _DUMMY_MES_
#pragma warning(disable:4100)
#endif
//////////////////////////////////////////////////////////////////////////
CMesDriver::CMesDriver(void)
    : m_hDLL(NULL)
{
}

CMesDriver::~CMesDriver(void)
{
}

void CMesDriver::Cleanup(void)
{
    if (NULL != m_hDLL)
    {
        FreeLibrary(m_hDLL);
        m_hDLL = NULL;
        m_drv.Reset();
    }
}

BOOL CMesDriver::Startup(std::wstring& strDLL)
{
    m_hDLL = LoadLibraryExW(strDLL.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == m_hDLL)
    {
        return FALSE;
    }

    m_drv._Handle_Create = (pCreateMesDllObject)GetProcAddress(m_hDLL, "_CreateMesDllObject");
	m_drv._Handle_ReleaseAll = (pReleaseMesDllObject)GetProcAddress(m_hDLL, "_ReleaseMesDllObject");
	m_drv._Login = (pConnect)GetProcAddress(m_hDLL, "_Connect");
	m_drv._Logout = (pClose)GetProcAddress(m_hDLL, "_Close");
	m_drv._CheckFlow = (pCheckFlow)GetProcAddress(m_hDLL, "_CheckFlow");
	m_drv._SendTestResult = (pSendTestResult)GetProcAddress(m_hDLL, "_SendTestResult");
	m_drv._UploadDataLog = (pUploadDataLog)GetProcAddress(m_hDLL, "_UploadDataLog");

    if (!m_drv.IsValid())
    {
        Cleanup();
        return FALSE;
    }

    return TRUE;
}

BOOL CMesDriver::_Handle_Create(HANDLE pLogUtil, HANDLE **pHandle)
{
    if (NULL != m_drv._Handle_Create)
    {
#ifdef _DUMMY_MES_
        return FALSE;
#else
        return m_drv._Handle_Create(pLogUtil, pHandle);
#endif
    }
    else
    {
        return FALSE;
    }
}

void CMesDriver::_Handle_ReleaseAll(HANDLE pHandle)
{
	if (NULL != m_drv._Handle_ReleaseAll)
	{
#ifdef _DUMMY_MES_
#else
		m_drv._Handle_ReleaseAll(pHandle);
#endif
	}
}


BOOL CMesDriver::_Login(HANDLE pHandle, LPCWSTR lpwMesMode, LPCWSTR lpwIP, LPCWSTR lpwDatabase, LPCWSTR lpwUser, LPCWSTR lpwPassword)
{
	if (NULL != m_drv._Login)
	{
#ifdef _DUMMY_MES_
		return TRUE;
#else
		return m_drv._Login(pHandle, lpwMesMode, lpwIP, lpwDatabase, lpwUser, lpwPassword);
#endif
	}
	else
	{
		return FALSE;
	}
}

BOOL CMesDriver::_Logout(HANDLE pHandle)
{
	if (NULL != m_drv._Logout)
	{
#ifdef _DUMMY_MES_
		return TRUE;
#else
		return m_drv._Logout(pHandle);
#endif
	}
	else
	{
		return FALSE;
	}
}

BOOL CMesDriver::_CheckFlow(HANDLE pHandle, LPCWSTR lpwSN)
{
	if (NULL != m_drv._CheckFlow)
	{
#ifdef _DUMMY_MES_
		return TRUE;
#else
		return m_drv._CheckFlow(pHandle, lpwSN);
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::_SendTestResult(HANDLE pHandle, LPCWSTR lpwSN, LPCWSTR lpwBatchName, UINT32 Result, LPCWSTR lpwToolsVersion, UINT32 ErrCode, LPCWSTR lpwErrMsg, LPCWSTR lpwIP, UINT32 ElapsedTime, UINT8 StationID, UINT8 Operator)
{
	if (NULL != m_drv._SendTestResult)
	{
#ifdef _DUMMY_MES_
		return TRUE;
#else
		return m_drv._SendTestResult(pHandle, lpwSN, lpwBatchName, Result, lpwToolsVersion, ErrCode, lpwErrMsg, lpwIP, ElapsedTime, StationID, Operator);
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::_UploadDataLog(HANDLE pHandle, LPCWSTR lpcPathDataLog)
{
	if (NULL != m_drv._UploadDataLog)
	{
#ifdef _DUMMY_MES_
		return TRUE;
#else
		return m_drv._UploadDataLog(pHandle, lpcPathDataLog);
#endif
	}
	else
	{
		return FALSE;
	}
}