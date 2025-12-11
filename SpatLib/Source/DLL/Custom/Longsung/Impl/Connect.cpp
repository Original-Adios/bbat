#include "StdAfx.h"
#include "Connect.h"
#include "../drv/SLData.h"
#include "ActionApp.h"

//
IMPLEMENT_RUNTIME_CLASS(CConnect)
//////////////////////////////////////////////////////////////////////////
CConnect::CConnect(void)
:  m_eUpload(SYNC)
{
}

CConnect::~CConnect(void)
{
}

BOOL CConnect::LoadXMLConfig(void)
{
    m_strProject = _W2CA(GetConfigValue(L"Option:Project", L"LST_MU9820E_052"));

    std::wstring strUpload = GetConfigValue(L"Option:Upload", L"Sync");
    if (strUpload == L"Async")
    {
        m_eUpload = ASYNC;
    }
    else if (strUpload == L"Sync")
    {
        m_eUpload = SYNC;
    }
    else
    {
        m_eUpload = NONE;
    }

    return TRUE;
}

SPRESULT CConnect::__PollAction(void)
{
    extern CActionApp  myApp;

    // Get IP & port from ini file
    string_t strIniFile = (string_t)myApp.GetAppPath() + _T("\\..\\..\\Customized\\Longsung\\SLNETDB.ini");
    TCHAR szText[24] = {0};
    GetPrivateProfileString(_T("SLNETDB"), _T("IP"), _T("127.0.0.1"), szText, _countof(szText), strIniFile.c_str());
    CHAR szIP[24] = {0};
    strcpy_s(szIP, _T2CA(szText));
    INT nPort = GetPrivateProfileInt(_T("SLNETDB"), _T("Port"), 0, strIniFile.c_str());
   
    CHAR szItem[64] = {0};
    sprintf_s(szItem, "ConnectSL[%s:%d]", szIP, nPort);

    if (SP_OK != GetSLDriverFromShareMemory())
    {
        m_pSLDrv = new CSLDriver();
        CHKRESULT(SetSLDriverIntoShareMemory());
    }

    string_t strDrvPath = (string_t)myApp.GetAppPath() + _T("\\..\\..\\Customized\\Longsung\\SLNETDB.dll");
    CHKRESULT_WITH_NOTIFY(m_pSLDrv->Startup(strDrvPath) ? SP_OK : SP_E_LOAD_LIBRARY, "Load SLNETDB.dll");
    
    m_pSLDrv->SL_quit();
    if (!m_pSLDrv->SL_init((char* )szIP, nPort))
    {
        NOTIFY(szItem, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SL_init fail, %s", m_pSLDrv->SL_GetLastError());
        return SP_E_SPDB_SERVER_CONNECT;
    }

    INT  lineid = -1;
    INT  itemid = -1;
    CHAR itemname[1024] = {0};
    SYSTEMTIME    t;
    GetLocalTime(&t);
    CHAR szTime[32] = {0};
    sprintf_s(szTime, "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
    if (!m_pSLDrv->SL_UserSoftver(
        m_strProject.c_str(), 
        "",
        "Simba", 
        "", 
        szTime,
        &lineid,
        &itemid,
        itemname)
        )
    {
        NOTIFY(szItem, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SL_UserSoftver fail, %s", m_pSLDrv->SL_GetLastError());
        return SP_E_SPDB_SERVER_CONNECT;
    }

    if (!m_pSLDrv->SL_isConnected())
    {
        NOTIFY(szItem, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SL_isConnected fail, %s", m_pSLDrv->SL_GetLastError());
        return SP_E_SPDB_SERVER_CONNECT;
    }

    if (NONE != m_eUpload)
    {
		CSLData* pSLData = NULL;
		if ((SP_OK == GetShareMemory(ShareMemoryItemDataObserver, (void* )&pSLData, sizeof(pSLData))) && (NULL != pSLData))
		{
			delete pSLData;
			pSLData = NULL;
		}
        pSLData = new CSLData(m_pSLDrv, GetISpLogObject());
        if (ASYNC == m_eUpload)
        {
            // Start asynchronous upload thread
            CHKRESULT_WITH_NOTIFY(pSLData->StartUpload(TRUE) ? SP_OK : SP_E_SPDB_START_TEST, "StartUploading");
        }
        IItemDataObserver* pObs = (IItemDataObserver* )pSLData;
        CHKRESULT(SetShareMemory(ShareMemoryItemDataObserver, (const void* )&pObs, sizeof(pObs), IContainer::System));
    }

    NOTIFY(szItem, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
    return SP_OK;
}

