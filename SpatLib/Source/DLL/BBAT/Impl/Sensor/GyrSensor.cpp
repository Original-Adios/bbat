#include "StdAfx.h"
#include "GyrSensor.h"

//
IMPLEMENT_RUNTIME_CLASS(CGyrSensor)

CGyrSensor::CGyrSensor(void)
{
}
CGyrSensor::~CGyrSensor(void)
{
}

SPRESULT CGyrSensor::__PollAction(void)
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Gyroscope_Sensor), "Gyroscope Sensor", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Gyroscope Sensor", LEVEL_ITEM);
    return SP_OK;
}

BOOL CGyrSensor::LoadXMLConfig(void)
{
    return TRUE;
}
