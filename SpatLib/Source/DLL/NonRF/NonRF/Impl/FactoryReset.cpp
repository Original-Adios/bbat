#include "StdAfx.h"
#include "FactoryReset.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CFactoryReset)

///
CFactoryReset::CFactoryReset(void)
{
   
}

CFactoryReset::~CFactoryReset(void)
{

}

SPRESULT CFactoryReset::__PollAction()
{
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
    SPRESULT res = SP_SendATCommand(m_hDUT, AT, TRUE, recvBuf, sizeof(recvBuf), NULL, TIMEOUT_3S);
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
