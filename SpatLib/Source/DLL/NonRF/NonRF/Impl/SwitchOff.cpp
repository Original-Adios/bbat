#include "StdAfx.h"
#include "SwitchOff.h"
#include "ModeSwitch.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSwitchOff)

///
CSwitchOff::CSwitchOff(void)
: m_u32DelayTime(2000)
, m_bAutoMode(TRUE)
{

}

CSwitchOff::~CSwitchOff(void)
{
}

BOOL CSwitchOff::LoadXMLConfig(void)
{
    LPCWSTR lpszAutoMode = GetConfigValue(L"Option:Mode", L"Auto");
    if (NULL != lpszAutoMode)
    {
        if (0 != _wcsicmp(lpszAutoMode, L"Auto"))
        {
            m_bAutoMode = FALSE;
        }
    }

    m_u32DelayTime = GetConfigValue(L"Option:DelayTime", 2000);

    return TRUE;
}

SPRESULT CSwitchOff::__PollAction(void)
{ 
    SPRESULT res = m_pModeSwitch->PowerOff(m_bAutoMode);
    if (SP_OK == res && IsFakeDC())
    {
        Sleep(m_u32DelayTime);
    }

/*  12/9/2019 JXP Bug1221798 
    If DUT is powering off and meanwhile shut down PowerSupply, it may cause unexpected problem.
    So if power off by command, keep the power off output without shut down.
   
    if (!IsFakeDC())   // Bug1178333
    {
        CHKRESULT(m_pDCSource->SetVoltage((float)-1.0));

        // Bug1024842: Switch off VBUS 
        IDCS* pVBUS = NULL;
        if (   (SP_OK == GetShareMemory(ShareMemory_VBusPowerSupplyObject, (LPVOID)&pVBUS, sizeof(IDCS*))) 
            && (NULL != pVBUS)
            )
        {
            // 独立的VBUS电源
            CHKRESULT(pVBUS->SetVoltage(static_cast<float>(-1.0)));
        }
        else
        {
            BOOL bSupport2ndChannel = FALSE;
            if (SP_OK != m_pDCSource->GetProperty(DCP_DUALCHANNEL, NULL, &bSupport2ndChannel))
            {
                bSupport2ndChannel = FALSE;
            }
            if (bSupport2ndChannel)
            {
                CHKRESULT(m_pDCSource->SetVoltage(static_cast<float>(-1.0), 2));
            }
        }
    }
*/

    NOTIFY("PowerOff", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL);
    
    /* 考虑到多线程（多DUT）同时跑的情况，这里去掉POWER OFF仪器RST
    if (NULL != m_pRFTester)
    {
        U_RF_PARAM param;
        param.dDataRate = 0;
        CHKRESULT(m_pRFTester->SetParameter(PT_RESET, param));
    }
    */

    return res;
}

SPRESULT CSwitchOff::__InitAction( void )
{
    return SP_OK;
}
