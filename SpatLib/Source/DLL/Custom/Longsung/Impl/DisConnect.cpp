#include "StdAfx.h"
#include "DisConnect.h"
#include "../drv/SLData.h"

IMPLEMENT_RUNTIME_CLASS(CDisConnect)
//////////////////////////////////////////////////////////////////////////
CDisConnect::CDisConnect(void)
{
}

CDisConnect::~CDisConnect(void)
{
}

SPRESULT CDisConnect::__PollAction(void)
{
    CHKRESULT_WITH_NOTIFY(GetSLDriverFromShareMemory(), "GetSLDriver");

    IItemDataObserver* pObs = NULL;
    if (SP_OK == GetShareMemory(ShareMemoryItemDataObserver, (void* )&pObs, sizeof(pObs)) && (NULL != pObs))
    {
        CSLData* pSLData = (CSLData* )pObs;
        pSLData->StartUpload(FALSE);
        pSLData->Release();
        pSLData = NULL;
        CHKRESULT(SetShareMemory(ShareMemoryItemDataObserver, (const void* )&pSLData, sizeof(pSLData), IContainer::System));
    }

    if (!m_pSLDrv->SL_quit())
    {
        NOTIFY("DisconnServer", LEVEL_ITEM, 1, 0, 1, NULL, -1, m_pSLDrv->SL_GetLastError());
        return SP_E_SPDB_DISCONNECT_SERVER_FAIL;
    }
        
    m_pSLDrv->Cleanup();
    delete m_pSLDrv;
    m_pSLDrv = NULL;
    CHKRESULT(SetSLDriverIntoShareMemory());

    NOTIFY("DisconnServer", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
    return SP_OK;
}

