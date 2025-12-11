#include "StdAfx.h"
#include "ChipConnect.h"


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CChipConnect)

/// 
CChipConnect::CChipConnect(void)
{
	m_bAccelerometer = TRUE;
	m_bMagnetic = TRUE;
	m_bOrientation = TRUE;
	m_bGyroscope = TRUE;
	m_bLight = TRUE;
	m_bPressure = TRUE;
	m_bTemperature = TRUE;
	m_bProximity = TRUE;
	m_bGravity = TRUE;
	m_bLinear = TRUE;
	m_bRotation = TRUE;
	m_bFinger = TRUE;
    m_bHeartRate = TRUE;
	m_Blood = TRUE;
	m_Sar = TRUE;
	m_Step = TRUE;
}

CChipConnect::~CChipConnect(void)
{
}

BOOL CChipConnect::LoadXMLConfig(void)
{
	m_bAccelerometer = GetConfigValue(L"Option:Accelerometer", 0);
	m_bMagnetic = GetConfigValue(L"Option:Magnetic", 0);
	m_bOrientation = GetConfigValue(L"Option:Orientation", 0);
	m_bGyroscope = GetConfigValue(L"Option:Gyroscope", 0);
	m_bLight = GetConfigValue(L"Option:Light", 0);
	m_bPressure = GetConfigValue(L"Option:Pressure", 0);
	m_bTemperature = GetConfigValue(L"Option:Temperature", 0);
	m_bProximity = GetConfigValue(L"Option:Proximity", 0);
	m_bGravity = GetConfigValue(L"Option:Gravity", 0);
	m_bLinear = GetConfigValue(L"Option:Linear_Acceleration", 0);
	m_bRotation = GetConfigValue(L"Option:Rotation_Vector", 0);
	m_bFinger = GetConfigValue(L"Option:Finger_Print", 0);
	m_bHeartRate = GetConfigValue(L"Option:Heart_Rate", 0);
	m_Blood = GetConfigValue(L"Option:Blood_Press", 0);
	m_Sar = GetConfigValue(L"Option:Sar", 0);
	m_Step = GetConfigValue(L"Option:Step", 0);

	return TRUE;
}



SPRESULT CChipConnect::__PollAction(void)
{
	if (m_bAccelerometer)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Accelerometer_Sensor),"Acc Sensor",LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Acc Sensor", LEVEL_ITEM);
	}
	if (m_bMagnetic)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Magnetic_Sensor), "Magnetic Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Magnetic Sensor", LEVEL_ITEM);
	}

	if (m_bOrientation)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Orientation_Sensor), "Orientation Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Orientation Sensor", LEVEL_ITEM);
	}

	if (m_bGyroscope)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Gyroscope_Sensor), "Gyroscope Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Gyroscope Sensor", LEVEL_ITEM);
	}

	if (m_bLight)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Light_Sensor), "Light Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Light Sensor", LEVEL_ITEM);
	}
	if (m_bPressure)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Pressure_Sensor), "Pressure Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Pressure Sensor", LEVEL_ITEM);
	}

	if (m_bTemperature)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Temperature_Sensor), "Temperature Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Temperature Sensor", LEVEL_ITEM);
	}
	if (m_bProximity)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Proximity_Sensor), "Proximity Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Proximity Sensor", LEVEL_ITEM);
	}
	if (m_bGravity)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Gravity_Sensor), "Gravity Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Gravity Sensor", LEVEL_ITEM);
	}

	if (m_bLinear)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Linear_Acceleration_Sensor), "Linear Acceleration Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Linear Acceleration Sensor", LEVEL_ITEM);
	}

	if (m_bRotation)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Rotation_Vector_Sensor), "Rotation Vector Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Rotation Vector Sensor", LEVEL_ITEM);

	}
	if (m_bFinger)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Finger_Print), "Finger Print Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Finger Print Sensor", LEVEL_ITEM);

	}
	if (m_bHeartRate)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Heart_Rate), "Heart Rate Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Heart Rate Sensor", LEVEL_ITEM);

	}
	if (m_Blood)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Blood_Press_Sensor), "Blood Press Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Blood Press Sensor", LEVEL_ITEM);

	}
	if (m_Sar)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Sar_Sensor), "Sar Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Sar Sensor", LEVEL_ITEM);

	}
	if (m_Step)
	{
		CHKRESULT(PrintErrorMsg(SP_BBAT_SensorTest(m_hDUT, Step_Sensor), "Step Sensor", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Step Sensor", LEVEL_ITEM);

	}
	
	return SP_OK;
}
