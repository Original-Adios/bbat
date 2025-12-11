#include "StdAfx.h"
#include "FingerPrint.h"

//
IMPLEMENT_RUNTIME_CLASS(CFingerPrint)

CFingerPrint::CFingerPrint(void)
{
}
CFingerPrint::~CFingerPrint(void)
{
}

SPRESULT CFingerPrint::__PollAction(void)
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Finger_Print), "Finger Print Sensor", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Finger Print Sensor", LEVEL_ITEM);
    return SP_OK;
}

BOOL CFingerPrint::LoadXMLConfig(void)
{
    return TRUE;
}
