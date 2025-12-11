#include "StdAfx.h"
#include "ActiveArmLog.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CActiveArmLog)

///
CActiveArmLog::CActiveArmLog(void)
{
}

CActiveArmLog::~CActiveArmLog(void)
{
}

SPRESULT CActiveArmLog::__PollAction(void)
{
    uint32 nValue = 0;
    uint8 recvbuf[128] = { 0 };
    //Add this Action must be active armlog.
    SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ARMLOG=1", TRUE, recvbuf, sizeof(recvbuf), NULL, TIMEOUT_2S);
    if (SP_OK != res)
    {
        // If AT+ARMLOG=1 is not support, then using the old DIAG command
        res = SP_EnableArmLog(m_hDUT, TRUE);
        if (SP_OK == res)
        {
            // ArmLogel playback need CP version 
            CHAR szVer[1000] = { 0 };
            SP_GetSWVer(m_hDUT, CP, szVer, sizeof(szVer));
        }
    }

    NOTIFY("EnableArmLog", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL);

    // Enable DSP Log
    Sleep(50);
    if (SP_OK == SP_SendATCommand(m_hDUT, "AT+SPDSPOP=2", TRUE, recvbuf, sizeof(recvbuf), NULL, TIMEOUT_2S) || SP_OK == res)
    {
        NOTIFY("ActiveLogel", LEVEL_UI, 1, 1, 1);
    }

    /// ArmLogel needs software information to playback 
    CHAR szSW[2500] = { 0 };
    CHKRESULT(SP_GetSWVer(m_hDUT, CP, szSW, sizeof(szSW)));

    return SP_OK;
}
