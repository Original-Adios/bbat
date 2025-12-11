#include "StdAfx.h"
#include "QueryCurrMode.h"
#include "RunMode.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CQueryCurrMode)

//////////////////////////////////////////////////////////////////////////
CQueryCurrMode::CQueryCurrMode(void)
{
}

CQueryCurrMode::~CQueryCurrMode(void)
{
}

SPRESULT CQueryCurrMode::__PollAction(void)
{
    INT    nMode = -1;
    SPRESULT res = SP_QueryCurrRunMode(m_hDUT, nMode);
    if (SP_OK != res)
    {
        NOTIFY("QueryCurrentMode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return res;
    }

    const char* lpszMode = CRunMode::QueryMode(nMode);
    if (NULL == lpszMode)
    {
        NOTIFY("QueryCurrentMode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unknown mode");
        return SP_E_PHONE_UNKNOWN_RUN_MODE;
    }
    else
    {
        NOTIFY("QueryCurrentMode", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "%s", lpszMode);
        return SP_OK;
    }
}
