#include "StdAfx.h"
#include "Upload.h"
#include "../drv/SLData.h"

IMPLEMENT_RUNTIME_CLASS(CUpload)
//////////////////////////////////////////////////////////////////////////
CUpload::CUpload(void)
{
}

CUpload::~CUpload(void)
{
}

SPRESULT CUpload::__PollAction(void)
{
    SPRESULT res = SP_OK;
    IItemDataObserver* pObs = NULL;
    if (SP_OK != GetShareMemory(ShareMemoryItemDataObserver, (void* )&pObs, sizeof(pObs)) || (NULL == pObs))
    {
        NOTIFY("Upload", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Upload task is not started");
        return SP_E_SPDB_START_TEST;
    }

    CSLData* pSLData = (CSLData* )pObs;
    if (!pSLData->isRunning())
    {
        CHKRESULT_WITH_NOTIFY(pSLData->StartUpload(TRUE) ? SP_OK : SP_E_SPDB_START_TEST, "StartUploading");
    }
    pSLData->StartUpload(FALSE);

    // Upload test data to server
    CHAR sn[ShareMemory_SN_SIZE] = {0};
    GetShareMemory(ShareMemory_SN1, (void* )sn, sizeof(sn));
    SPTEST_RESULT_T rslt;
    CHKRESULT(GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&rslt, sizeof(rslt)));

    CHAR szInfo[512] = {0};
    if (SP_OK != rslt.errCode)
    {
        sprintf_s(szInfo, "ERRCODE=0x%X|ERRTYPE=%s|ERRMSG=%s", rslt.errCode, rslt.szFailItem, rslt.szDescription);
    }
    if (!pSLData->m_pSLDrv->SL_ExtDatatoPhone(SL_SP_FT, sn, SP_OK == rslt.errCode ? 0 : 1, rslt.nElapsed/1000, szInfo))
    {
        NOTIFY("Upload", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, pSLData->m_pSLDrv->SL_GetLastError());
        res = SP_E_SPDB_UPLOAD_DATA_FAIL;
    }
    else
    {
        NOTIFY("Upload", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
    }

    pSLData->Release();
    pSLData = NULL;
    CHKRESULT(SetShareMemory(ShareMemoryItemDataObserver, (const void* )&pSLData, sizeof(pSLData), IContainer::System));
    return res;
}

