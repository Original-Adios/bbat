#include "stdAfx.h"
#include "DCSFactory.h"
#include "ActionApp.h"

//////////////////////////////////////////////////////////////////////////
CDCSFactory::CDCSFactory(CSpatBase* pImp)
    : m_pImp(pImp)
    , m_pDevManager(NULL)
    , m_hDevManagerLib(NULL)
{

}

CDCSFactory::~CDCSFactory(void)
{

}

void CDCSFactory::ReleaseDevManager(IDevManager*& lpDevManager)
{
    if (NULL != lpDevManager)
    {
        lpDevManager->Free();
        lpDevManager->Release();
        lpDevManager = NULL;
    }

    if (NULL != m_hDevManagerLib)
    {
        FreeLibrary(m_hDevManagerLib);
        m_hDevManagerLib = NULL;
    }
}

IDevManager* CDCSFactory::CreateDevManager(const std::wstring& strDevDir, LPVOID pLogUtil)
{
    std::wstring strDLLPath = strDevDir + L"\\DeviceManager.dll";
    m_hDevManagerLib = LoadLibraryExW(strDLLPath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == m_hDevManagerLib)
    {
        m_pImp->LogFmtStrW(SPLOGLV_ERROR, L"LoadLibraryExW <%s> fail! WinErr = %d", strDLLPath.c_str(), ::GetLastError());
        return NULL;
    }

    do 
    {
        pfCreateDevManager pCreate = (pfCreateDevManager)GetProcAddress(m_hDevManagerLib, "CreateDevManager");
        if (NULL == pCreate)
        {
            m_pImp->LogFmtStrA(SPLOGLV_ERROR, "GetProcAddress(CreateDevManager) fail! WinErr = %d", ::GetLastError());
            break;
        }

        IDevManager* pDevManager = (*pCreate)(pLogUtil);
        if (NULL  == pDevManager)
        {
            m_pImp->LogFmtStrA(SPLOGLV_ERROR, "%s: %d, CreateDevManager() fail!", __FILE__, __LINE__);
            break;
        }

        m_pImp->LogRawStrA(SPLOGLV_INFO, "Loading device libraries ...");
        if (!pDevManager->Load(strDevDir.c_str()))
        {
            pDevManager->Release();
            m_pImp->LogFmtStrW(SPLOGLV_ERROR, L"Load device libraries fail! Path: %s", strDevDir.c_str());
            break;
        }

        return pDevManager;

#pragma warning(disable:4127)
    } while (0); 
#pragma warning(default:4127)

    FreeLibrary(m_hDevManagerLib);
    m_hDevManagerLib = NULL;
    return NULL;
}

void CDCSFactory::DeleteDCS(IDCS*& pDCS)
{
    if (NULL != pDCS)
    {
        pDCS->Release();
        pDCS = NULL;
    }

    ReleaseDevManager(m_pDevManager);
}

IDCS* CDCSFactory::CreateDCS(const GPIB_PARAM& io)
{
    SPRESULT res = SP_OK;
    if (NULL == m_pDevManager)
    {
         WCHAR szDevPath[MAX_PATH] = {0};
        if (NULL != m_pImp->m_pDCSource)
        {
            char szTempDevPath[MAX_PATH] = { 0 };
            res = m_pImp->m_pDCSource->GetProperty(DCP_LIBRARY_PATH, 0, (LPVOID)&szTempDevPath[0]);

			CUtility Utility;
			memcpy(szDevPath, Utility._A2CW(szTempDevPath), MAX_PATH);

            size_t nLen = wcslen(szDevPath);
            if (nLen > 0 && szDevPath[nLen - 1] == L'\\')
            {
                // Remove last '\'
                szDevPath[nLen - 1] = L'\0'; 
            }
        }

        if (NULL == m_pImp->m_pDCSource || SP_OK != res)
        {
            extern CActionApp myApp;
            swprintf_s(szDevPath, L"%s\\..\\Device", _T2CW(myApp.GetAppPath()));
        }

        m_pDevManager = CreateDevManager(szDevPath, (LPVOID)(m_pImp->GetISpLogObject()));
        if (NULL == m_pDevManager)
        {
            return NULL;
        }
    }

    IDCS* pDCS = (IDCS* )m_pDevManager->CreateDevice(io, (LPVOID)(m_pImp->GetISpLogObject()));
    if (NULL == pDCS)
    {
        ReleaseDevManager(m_pDevManager);
        m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Create DCS <%s> fail.", io.szGpibAddr);
        return NULL;
    }

    return pDCS;
}

