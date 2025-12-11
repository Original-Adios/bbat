#include "StdAfx.h"
#include "RestartPhone.h"
#include "ModeSwitch.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CRestartPhone)

///
CRestartPhone::CRestartPhone(void)
{

}

CRestartPhone::~CRestartPhone(void)
{
}

SPRESULT CRestartPhone::__PollAction(void)
{ 
    SPRESULT res = m_pModeSwitch->RestartPhone();
    NOTIFY("RestartPhone", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL);
    return res;
}
