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

    m_drv.m_pfnMesGetVersion = (pfnGetVersion)GetProcAddress(m_hDLL, "fnGetVersion");
	m_drv.m_pfnMesStart = (pfnMesStart)GetProcAddress( m_hDLL, "fnMesStart");
	m_drv.m_pfnMesComplete = (pfnMesComplete)GetProcAddress(m_hDLL, "fnMesComplete");
	m_drv.m_pfnGetCodeFromStore = (pfnGetCodeFromStore)GetProcAddress(m_hDLL, "fnGetCodeFromStore");

    if (!m_drv.IsValid())
    {
        Cleanup();
        return FALSE;
    }

    return TRUE;
}

BOOL CMesDriver::fnGetVersion(char *ver)
{
    if (NULL != m_drv.m_pfnMesGetVersion)
    {
#ifdef _DUMMY_MES_
        return TRUE;
#else
        return m_drv.m_pfnMesGetVersion(ver)==1 ? TRUE : FALSE;
#endif
    }
    else
    {
        return FALSE;
    }
}
BOOL CMesDriver::fnMesStart(char *inMessage, char *retMessage, int iMesType)
{
	if (NULL != m_drv.m_pfnMesStart)
	{
#ifdef _DUMMY_MES_
		return TRUE;
#else
		return m_drv.m_pfnMesStart(inMessage,retMessage,iMesType)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::fnMesComplete(char *inMessage, int iTestRes, char *retMessage, int iMesType)
{
	if (NULL != m_drv.m_pfnMesComplete)
	{
#ifdef _DUMMY_MES_
		return TRUE;
#else
		return m_drv.m_pfnMesComplete(inMessage,iTestRes,retMessage,iMesType)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::fnGetCodeFromStore(const char* pMsg, const char* pStore, const unsigned int iLength, char* pRes)
{
	if (NULL != m_drv.m_pfnGetCodeFromStore)
	{
#ifdef _DUMMY_MES_
		return TRUE;
#else
		return m_drv.m_pfnGetCodeFromStore(pMsg,pStore,iLength,pRes)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}