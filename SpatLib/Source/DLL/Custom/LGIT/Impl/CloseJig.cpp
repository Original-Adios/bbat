#include "StdAfx.h"
#include "CloseJig.h"
#include "Jig.h"

//
IMPLEMENT_RUNTIME_CLASS(CCloseJig)
//////////////////////////////////////////////////////////////////////////
CCloseJig::CCloseJig(void)
{
}

CCloseJig::~CCloseJig(void)
{
}

SPRESULT CCloseJig::__PollAction(void)
{
    // Get JIG UART port and baudrate which is configured by CStartJig action
    DWORD dwPort = 0;
    DWORD dwBaud = 0;
    CHKRESULT(GetShareMemory(LGIT_ShareMemory_JigPort, (VOID* )&dwPort, sizeof(dwPort)));
    CHKRESULT(GetShareMemory(LGIT_ShareMemory_JigBaud, (VOID* )&dwBaud, sizeof(dwBaud)));

    // Get the test result of Simba
    SPTEST_RESULT_T rslt;
    CHKRESULT(GetShareMemory(InternalReservedShareMemory_ItemTestResult, (VOID* )&rslt, sizeof(rslt)));

    CJig Jig(GetISpLogObject());
    JIG_ERROR e = Jig.Close(dwPort, dwBaud, (SP_OK == rslt.errCode) ? TRUE : FALSE);
    if (JIG_OK == e)
    {
        NOTIFY("CloseJig", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
        return SP_OK;
    }
    else
    {
        NOTIFY("CloseJig", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, Jig.GetErrMsg(e));
        return SP_E_JIG_ERROR;
    }
}

