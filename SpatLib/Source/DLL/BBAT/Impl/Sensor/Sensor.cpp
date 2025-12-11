#include "StdAfx.h"
#include "Sensor.h"
#include "ShareMemoryDefine.h"
#include "CLocks.h"
#include "BarCodeUtility.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSensor)

/// 
CSensor::CSensor(void)
{
	m_bMagnetic = TRUE;
	m_bProximity = TRUE;
	m_bLight = TRUE;
}

CSensor::~CSensor(void)
{
}
SPRESULT CSensor::__PollAction(void)
{
	if (m_bMagnetic)
	{
		CHKRESULT(RunMag());
	}
	if (m_bProximity)
	{
		CHKRESULT(RunPro());
	}
	if (m_bLight)
	{
		CHKRESULT(RunLig());
	}

	return SP_OK;
}
BOOL CSensor::LoadXMLConfig(void)
{
	m_bMagnetic = GetConfigValue(L"Option:Mag_Sensor:Magnetic", 0);
	m_bProximity = GetConfigValue(L"Option:Pro_Sensor:Proximity", 0);
	m_bLight = GetConfigValue(L"Option:Light_Sensor:Light", 0);

	m_MagValueX.upp = GetConfigValue(L"Option:Mag_Sensor:X_SpecUp",0.0);
	m_MagValueX.low = GetConfigValue(L"Option:Mag_Sensor:X_SpecDown", 0.0);
	m_MagValueY.upp = GetConfigValue(L"Option:Mag_Sensor:Y_SpecUp", 0.0);
	m_MagValueY.low = GetConfigValue(L"Option:Mag_Sensor:Y_SpecDown", 0.0);
	m_MagValueZ.upp = GetConfigValue(L"Option:Mag_Sensor:Z_SpecUp", 0.0);
	m_MagValueZ.low = GetConfigValue(L"Option:Mag_Sensor:Z_SpecDown", 0.0);

	m_ProValue.upp = GetConfigValue(L"Option:Pro_Sensor:SpecUp", 0);
	m_ProValue.low = GetConfigValue(L"Option:Pro_Sensor:SpecDown", 0);

	m_LigValue.upp = GetConfigValue(L"Option:Light_Sensor:SpecUp", 0);
	m_LigValue.low = GetConfigValue(L"Option:Light_Sensor:SpecDown", 0);

	return TRUE;
}
SPRESULT CSensor::RunMag()
{
	MagReturn DataRecv;
	memset(&DataRecv, 0, sizeof(DataRecv));

	CHKRESULT(PrintErrorMsg(SP_BBAT_MagneticTest(m_hDUT, &DataRecv), "Magnetic Test Cmd Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "Magnetic Test Cmd Pass", LEVEL_ITEM);
	
	//Need Check
	CHKRESULT(PrintErrorMsg(CheckMag(DataRecv), "Axis Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "Axis Pass", LEVEL_ITEM);

	return SP_OK;
}
SPRESULT CSensor::CheckMag(MagReturn DataRecv)
{
	m_MagValueX.val = DataRecv.X_Axis;
	m_MagValueY.val = DataRecv.Y_Axis;
	m_MagValueZ.val = DataRecv.Z_Axis;
	SPRESULT sRes = SP_OK;
	_UiSendMsg("X Axis", LEVEL_ITEM, m_MagValueX.low, m_MagValueX.val, m_MagValueX.upp, nullptr, -1, nullptr);
	if (!m_MagValueX)
	{
		sRes = SP_E_BBAT_MAG_SENSOR_VALUE_FAIL;
	}
	_UiSendMsg("Y Axis", LEVEL_ITEM, m_MagValueY.low, m_MagValueY.val, m_MagValueY.upp, nullptr, -1, nullptr);
	if (!m_MagValueY)
	{
		sRes = SP_E_BBAT_MAG_SENSOR_VALUE_FAIL;
	}
	_UiSendMsg("Z Axis", LEVEL_ITEM, m_MagValueZ.low, m_MagValueZ.val, m_MagValueZ.upp, nullptr, -1, nullptr);
	if (!m_MagValueZ)
	{
		sRes = SP_E_BBAT_MAG_SENSOR_VALUE_FAIL;
	}
	return sRes;
}
SPRESULT CSensor::RunPro()
{
	int iProLevel = 0;

	CHKRESULT(PrintErrorMsg(SP_BBAT_ProximityTest(m_hDUT, &iProLevel), "BBAT Sensor Test: Proximity Sensor Open Fail",
		LEVEL_ITEM));

	PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Proximity Sensor Open Pass", LEVEL_ITEM);
	//Need Check
	m_ProValue.val = iProLevel;
	_UiSendMsg("Proximity Sensor Check", LEVEL_ITEM, m_ProValue.low, m_ProValue.val, m_ProValue.upp, nullptr, -1, nullptr);
	if (m_ProValue)
	{
		return SP_OK;
	}
	return SP_E_BBAT_PRO_SENSOR_VALUE_FAIL;
}

SPRESULT CSensor::RunLig()
{
	int iLightValue = 0;

	CHKRESULT(PrintErrorMsg(SP_BBAT_LightTest(m_hDUT, &iLightValue), "BBAT Sensor Test: Light Sensor Open Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Light Sensor Open Pass", LEVEL_ITEM);
	
	m_LigValue.val = iLightValue;
	_UiSendMsg("Light Sensor Check", LEVEL_ITEM, m_LigValue.low, m_LigValue.val, m_LigValue.upp, nullptr, -1, nullptr);
	if (m_LigValue)
	{
		return SP_OK;
	}	
	return SP_E_BBAT_LIT_SENSOR_VALUE_FAIL;
}


