#include "StdAfx.h"
#include "Finalize.h"
#include "ModeSwitch.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CFinalize)

///
CFinalize::CFinalize(void)
: m_u32PowerOffDelayTime(3000)
, m_bAutoPowerOff(TRUE)
{
   
}

CFinalize::~CFinalize(void)
{

}

BOOL CFinalize::LoadXMLConfig(void)
{
    LPCWSTR lpszAutoMode = GetConfigValue(L"Option:Mode", L"Auto");
    if (NULL != lpszAutoMode)
    {
        if (0 != _wcsicmp(lpszAutoMode, L"Auto"))
        {
            m_bAutoPowerOff = FALSE;
        }
    }

    m_u32PowerOffDelayTime = GetConfigValue(L"Option:DelayTime", 3000);

    return TRUE;
}

SPRESULT CFinalize::__PollAction()
{
    SPTEST_RESULT_T rslt;
    SPRESULT res = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (LPVOID)&rslt, sizeof(rslt));
    if (SP_OK == res && SP_OK == rslt.errCode)
    {
        // If pass, factory reset.
        LPCSTR AT = "AT+ETSRESET";
     /*
        @JXP
            PC immediately close port without waiting for a reply from the device side.
            It may cause the power off and restart not work well, 
            the reason is if there is an ack package of restart/power off needs to send back to PC,
            meanwhile if PC port is closed, DUT cannot send the ack package successfully and then 
            USB driver code of DUT will be in dead loop, 
            the result is restart and power off not be executed. 
    */
        // Here we should want ack response at first and then close port
        CHAR recvBuf[64] = {0};
        res = SP_SendATCommand(m_hDUT, AT, TRUE, recvBuf, sizeof(recvBuf), NULL, TIMEOUT_3S);
        if (SP_OK == res)
        {
            SP_EndPhoneTest(m_hDUT);
            NOTIFY("FactoryReset", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
        }
        else
        {
            NOTIFY("FactoryReset", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        }

        return res;
    }
    else
    {
        // if fail, power off
        BOOL bPowerOffDC = (!IsFakeDC() && m_bAutoPowerOff);
        res = m_pModeSwitch->PowerOff(bPowerOffDC);
        if (SP_OK == res && !bPowerOffDC)
        {
            // If power off by command, we need to wait until finished
            Sleep(m_u32PowerOffDelayTime);
        }

        NOTIFY("PowerOff", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL);

        return res;
    }
}
