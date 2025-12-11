#include "StdAfx.h"
#include "AssertUE.h"
#include "ModeSwitch.h"

//
IMPLEMENT_RUNTIME_CLASS(CAssertUE)

//////////////////////////////////////////////////////////////////////////
CAssertUE::CAssertUE(void)
{
}

CAssertUE::~CAssertUE(void)
{
}

SPRESULT CAssertUE::__PollAction(void)
{
    SPRESULT res = SP_AssertUE(m_hDUT);
    NOTIFY("AssertUE", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, "-", NULL);
    if (SP_OK == res)
    {
        m_pModeSwitch->SetCurrentMode(RM_INVALID_MODE);

        // Wait a few seconds to receive the assert message from UE
        Sleep(TIMEOUT_3S);

        // Wait for memory dumping if need, SP_EndPhoneTest MUST be invoked.
        SP_EndPhoneTest(m_hDUT);
    }

    return SP_OK;
}