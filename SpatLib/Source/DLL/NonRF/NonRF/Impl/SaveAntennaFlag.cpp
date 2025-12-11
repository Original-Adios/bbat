#include "StdAfx.h"
#include "SaveAntennaFlag.h"
#include "ModeSwitch.h"

//
IMPLEMENT_RUNTIME_CLASS(CSaveAntennaFlag)

//////////////////////////////////////////////////////////////////////////
CSaveAntennaFlag::CSaveAntennaFlag(void)
{
}

CSaveAntennaFlag::~CSaveAntennaFlag(void)
{
}

BOOL CSaveAntennaFlag::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CSaveAntennaFlag::__PollAction(void)
{ 
    uint32 u32Flag = 0;
    SPRESULT res0 = GetShareMemory(ShareMemory_TG_CalibrationFlag, (void* )&u32Flag, sizeof(u32Flag));
    if (SP_OK == res0 && IS_BIT_SET(u32Flag, TEST_FLAG_ANTENNA))
    {
        NOTIFY("SaveAntennaFlag", LEVEL_UI, 1, 1, 1, NULL, -1, NULL);
        return S_OK;
    }

#define LOCAL_CHKRESULT_WITH_BREAK(resCode, statement)      { \
        resCode = (statement);    \
        if (SP_OK != resCode)  {  \
            break;                \
        }                         \
    }

    SPRESULT res = SP_OK;
    do
    {
        // 1st change to GSM mode
        LOCAL_CHKRESULT_WITH_BREAK(res, m_pModeSwitch->Change(RM_CALIBRATION_MODE, NULL));

        if (SP_OK != res0)
        {
            LOCAL_CHKRESULT_WITH_BREAK(res, SP_gsmLoadCalFlag(m_hDUT, &u32Flag));
        }

        u32Flag |= TEST_FLAG_ANTENNA;
        LOCAL_CHKRESULT_WITH_BREAK(res, SP_gsmSaveCalFlag(m_hDUT, u32Flag));

        LOCAL_CHKRESULT_WITH_BREAK(res, SP_gsmSaveToFlash(m_hDUT, TIMEOUT_60S));

#pragma warning(disable:4127)
    } while (0);
#pragma warning(default:4127)

    NOTIFY("SaveAntennaFlag", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL);
    return res;
}
