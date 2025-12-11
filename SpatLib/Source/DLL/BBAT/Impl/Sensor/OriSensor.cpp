#include "StdAfx.h"
#include "OriSensor.h"

//
IMPLEMENT_RUNTIME_CLASS(COriSensor)

COriSensor::COriSensor(void)
{
}
COriSensor::~COriSensor(void)
{
}

SPRESULT COriSensor::__PollAction(void)
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Orientation_Sensor), "Orientation Sensor", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Orientation Sensor", LEVEL_ITEM);
    return SP_OK;
}

BOOL COriSensor::LoadXMLConfig(void)
{
    return TRUE;
}
