#include "StdAfx.h"
#include "ClosePort.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CClosePort)

///
CClosePort::CClosePort(void)
{
}

CClosePort::~CClosePort(void)
{
}

SPRESULT CClosePort::__PollAction(void)
{
    SP_EndPhoneTest(m_hDUT);
    NOTIFY("ClosePort", LEVEL_UI, 1, 1, 1, NULL, -1, NULL);
    return SP_OK;
}

