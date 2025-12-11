#include "StdAfx.h"
#include "PressureSensor.h"

IMPLEMENT_RUNTIME_CLASS(CPreSensor)

CPreSensor::CPreSensor(void)
{
}

CPreSensor::~CPreSensor(void)
{
}
SPRESULT CPreSensor::__PollAction(void)
{
	CHKRESULT(RunPressureTest());
	return SP_OK;
}

BOOL CPreSensor::LoadXMLConfig(void)
{
	m_nSpecUp = GetConfigValue(L"Option:SpecUp", 0);
	m_nSpecDown = GetConfigValue(L"Option:SpecDown", 0);
	return TRUE;
}

SPRESULT CPreSensor::RunPressureTest()
{
	int nPreValue = 0;
	CHKRESULT(PrintErrorMsg(
		SP_BBAT_PressureTest(m_hDUT, &nPreValue),
		"Pressure Sensor", LEVEL_ITEM));

	_UiSendMsg("Pressure Sensor", LEVEL_ITEM,
		m_nSpecDown, nPreValue, m_nSpecUp,
		nullptr, -1, nullptr,
		"Value = %d", nPreValue);

	if (!IN_RANGE(m_nSpecDown, nPreValue, m_nSpecUp))
	{
		return SP_E_BBAT_SENSOR_VALUE_FAIL;
	}

	return SP_OK;
}
