#include "StdAfx.h"
#include "SLDriver.h"
#include <assert.h>

#ifdef _DEBUG
#define _DUMMY_SLNETDB_     1
#endif

#ifdef _DUMMY_SLNETDB_
#pragma warning(disable:4100)
#endif
//////////////////////////////////////////////////////////////////////////
CSLDriver::CSLDriver(void)
    : m_hDLL(NULL)
{
}

CSLDriver::~CSLDriver(void)
{
}

void CSLDriver::Cleanup(void)
{
    if (NULL != m_hDLL)
    {
        FreeLibrary(m_hDLL);
        m_hDLL = NULL;
        m_drv.Reset();
    }
}

BOOL CSLDriver::Startup(std::wstring& strDLL)
{
    m_hDLL = LoadLibraryExW(strDLL.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == m_hDLL)
    {
        return FALSE;
    }

    m_drv.SL_init = (pSL_init)GetProcAddress(m_hDLL, "SL_init");
    m_drv.SL_quit = (pSL_quit)GetProcAddress(m_hDLL, "SL_quit");
    m_drv.SL_GetLastError = (pSL_GetLastError)GetProcAddress(m_hDLL, "SL_GetLastError");
    m_drv.SL_ClearLastError = (pSL_ClearLastError)GetProcAddress(m_hDLL, "SL_ClearLastError");
    m_drv.SL_isConnected = (pSL_isConnected)GetProcAddress(m_hDLL, "SL_isConnected");
    m_drv.SL_PutDatatoPhone = (pSL_PutDatatoPhone)GetProcAddress(m_hDLL, "SL_PutDatatoPhone");
    m_drv.SL_ClrDatatoPhone = (pSL_ClrDatatoPhone)GetProcAddress(m_hDLL, "SL_ClrDatatoPhone");
    m_drv.SL_ExtDatatoPhone = (pSL_ExtDatatoPhone)GetProcAddress(m_hDLL, "SL_ExtDatatoPhone");
    m_drv.SL_UserSoftver = (pSL_UserSoftver)GetProcAddress(m_hDLL, "SL_UserSoftver");
    m_drv.SL_AddnewPhone = (pSL_AddnewPhone)GetProcAddress(m_hDLL, "SL_AddnewPhone");
    if (!m_drv.IsValid())
    {
        Cleanup();
        return FALSE;
    }

    return TRUE;
}

BOOL CSLDriver::SL_init(char *ip, int port /* = 3395 */, SL_Receive_callback proc1 /* = NULL */, SL_Status_callback proc2 /* = NULL */,int steptime /* = 500 */, int buf_send_Max /* = 300 */, int buf_recv_Max /* = 300 */, char *bufpath /* = NULL */)
{
    if (NULL != ip && NULL != m_drv.SL_init)
    {
#ifdef _DUMMY_SLNETDB_
        return TRUE;
#else
        return m_drv.SL_init(ip, port, proc1, proc2, steptime, buf_send_Max, buf_recv_Max, bufpath) ? TRUE : FALSE;
#endif
    }
    else
    {
        return FALSE;
    }
}

BOOL CSLDriver::SL_isConnected(void)
{
    if (NULL != m_drv.SL_isConnected)
    {
    #ifdef _DUMMY_SLNETDB_
        return TRUE;
    #else
        return  m_drv.SL_isConnected() ? TRUE : FALSE;
    #endif
    }
    else
    {
        return FALSE;
    }
}

BOOL CSLDriver::SL_quit(void)
{
    if (NULL != m_drv.SL_quit)
    {
    #ifdef _DUMMY_SLNETDB_
        return TRUE;
    #else
        return  m_drv.SL_quit() ? TRUE : FALSE;
    #endif
    }
    else
    {
        return FALSE;
    }
}

LPCSTR CSLDriver::SL_GetLastError(void)
{
    m_szErrMsg[0] = '\0';
    if (NULL != m_drv.SL_GetLastError)
    {
    #ifndef _DUMMY_SLNETDB_
        m_drv.SL_GetLastError(m_szErrMsg);
    #endif
    }
    
    return m_szErrMsg;
}

BOOL CSLDriver::SL_ClearLastError(void)
{
    if (NULL != m_drv.SL_ClearLastError)
    {
    #ifdef _DUMMY_SLNETDB_
        return TRUE;
    #else
        return  m_drv.SL_ClearLastError() ? TRUE : FALSE;
    #endif
    }
    else
    {
        return FALSE;
    }
}

BOOL CSLDriver::SL_PutDatatoPhone(const char* name, bool success, int taketime, const char* head, const char* field, const char* values, const char* title /* = NULL */, int index /* = 0 */, int tread /* = 0 */)
{
    if (NULL != m_drv.SL_PutDatatoPhone)
    {
    #ifdef _DUMMY_SLNETDB_
        return TRUE;
    #else
        return  m_drv.SL_PutDatatoPhone(name, success, taketime, head, field, values, title, index, tread) ? TRUE : FALSE;
    #endif
    }
    else
    {
        return FALSE;
    }
}

BOOL CSLDriver::SL_ClrDatatoPhone(int tread /* = 0 */)
{
    if (NULL != m_drv.SL_ClrDatatoPhone)
    {
#ifdef _DUMMY_SLNETDB_
        return TRUE;
#else
        return  m_drv.SL_ClrDatatoPhone(tread) ? TRUE : FALSE;
    #endif
    }
    else
    {
        return FALSE;
    }
}

BOOL CSLDriver::SL_ExtDatatoPhone(SL_Station Stationattr, const char* sn, int success, int taketime, const char* head, int timeout /* = 0 */, int tread /* = 0 */, char* decdatas /* = NULL */)
{
    if (NULL != m_drv.SL_ExtDatatoPhone)
    {
    #ifdef _DUMMY_SLNETDB_
            return TRUE;
    #else
        return  m_drv.SL_ExtDatatoPhone(Stationattr, sn, success, taketime, head, timeout, tread, decdatas) ? TRUE : FALSE;
    #endif
    }
    else
    {
        return FALSE;
    }
}

BOOL CSLDriver::SL_AddnewPhone(const char* sn, int timeout /* = 5000 */)
{
    if (NULL != m_drv.SL_AddnewPhone)
    {
#ifdef _DUMMY_SLNETDB_
        return TRUE;
#else
        return  m_drv.SL_AddnewPhone(sn, timeout) ? TRUE : FALSE;
#endif
    }
    else
    {
        return FALSE;
    }
}

BOOL CSLDriver::SL_UserSoftver(
    const char* itemname_in, const char* linename, const char* softname, \
    const char* PCinfo, const char* lastconfigdate, \
    int *lineid /* = NULL */, int *itemid /* = NULL */, char* itemname /* = NULL */, int timeout /* = 5000\ */ )
{
    if (NULL != m_drv.SL_UserSoftver)
    {
#ifdef _DUMMY_SLNETDB_
        return TRUE;
#else
        return  m_drv.SL_UserSoftver(itemname_in, linename, softname, PCinfo, lastconfigdate, lineid, itemid, itemname, timeout) ? TRUE : FALSE;
#endif
    }
    else
    {
        return FALSE;
    }
}

#ifdef _DUMMY_SLNETDB_
#pragma warning(default:4100)
#endif