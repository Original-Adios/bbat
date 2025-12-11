#include "StdAfx.h"
#include "MesDriver.h"
#include <assert.h>

#ifdef _DEBUG
//#define _DUMMY_BYD_MES_     
#endif

#ifdef _DUMMY_BYD_MES_
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

    m_drv.Start_New = (pStart_New)GetProcAddress(m_hDLL, "Start_New");
	m_drv.Complete_New = (pComplete_New)GetProcAddress(m_hDLL, "Complete_New");
	m_drv.NcComplete_New = (pNcComplete_New)GetProcAddress(m_hDLL, "NcComplete_New");
	m_drv.SfcKeyCollect_New = (pSfcKeyCollect_New)GetProcAddress(m_hDLL, "SfcKeyCollect_New");
	m_drv.GetNumberBySfc_New = (pGetNumberBySfc_New)GetProcAddress(m_hDLL, "GetNumberBySfc_New");
	m_drv.GetCustomDatabyShoporder_New = (pGetCustomDatabyShoporder_New)GetProcAddress(m_hDLL, "GetCustomDatabyShoporder_New");

    if (!m_drv.IsValid())
    {
        Cleanup();
        return FALSE;
    }

    return TRUE;
}

BOOL CMesDriver::Start_New(char parSFC[], char parBoardCount[], char parWorkStation[], char parLogOperation[], char parLogResource[], char parRemark[], char *retMessage)
{
    if (NULL != m_drv.Start_New)
    {
#ifdef _DUMMY_BYD_MES_
        return TRUE;
#else
        return m_drv.Start_New(parSFC,parBoardCount,parWorkStation,parLogOperation,parLogResource,parRemark,retMessage)==1 ? TRUE : FALSE;
#endif
    }
    else
    {
        return FALSE;
    }
}
BOOL CMesDriver::Complete_New(char parSFC[], char parBoardCount[], char parQualityBatchNum[], char parWorkStation[], char parRemark[], char *retMessage)
{
	if (NULL != m_drv.Complete_New)
	{
#ifdef _DUMMY_BYD_MES_
		return TRUE;
#else
		return m_drv.Complete_New(parSFC,parBoardCount,parQualityBatchNum,parWorkStation,parRemark,retMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::NcComplete_New(char parSFC[], char parNcType[], char parNcCode[], char parNcContext[], char parFailItem[], char parFailValue[], char parBoardCount[], char parWorkStation[], char parLogOperation[], char parLogResource[], char parNcPlace[], char parCreateUser[], char parOldStationName[], char parRemark[], char *retMessage)
{
	if (NULL != m_drv.NcComplete_New)
	{
#ifdef _DUMMY_BYD_MES_
		return TRUE;
#else
		return m_drv.NcComplete_New(parSFC,parNcType,parNcCode,parNcContext,parFailItem,parFailValue,parBoardCount,parWorkStation,parLogOperation,parLogResource,parNcPlace,parCreateUser,parOldStationName,parRemark,retMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::SfcKeyCollect_New(char parSFC[], char parData[], char *retMessage)
{
	if (NULL != m_drv.SfcKeyCollect_New)
	{
#ifdef _DUMMY_BYD_MES_
		return TRUE;
#else
		return m_drv.SfcKeyCollect_New(parSFC,parData,retMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::GetNumberBySfc_New(char parSFC[], char parNumberStore[], char parModel[], char parModuleID[], char parCustomStatus[], char parRemark[], char *retNumber, char *retMessage)
{
	if (NULL != m_drv.GetNumberBySfc_New)
	{
#ifdef _DUMMY_BYD_MES_
		return TRUE;
#else
		return m_drv.GetNumberBySfc_New(parSFC,parNumberStore,parModel,parModuleID,parCustomStatus,parRemark,retNumber,retMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::GetCustomDatabyShoporder_New(char *retCustomData, char *retMessage)
{
	if (NULL != m_drv.GetCustomDatabyShoporder_New)
	{
#ifdef _DUMMY_BYD_MES_
		return TRUE;
#else
		return m_drv.GetCustomDatabyShoporder_New(retCustomData,retMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}