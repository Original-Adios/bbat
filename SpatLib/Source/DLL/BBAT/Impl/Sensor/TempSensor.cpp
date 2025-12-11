#include "StdAfx.h"
#include "TempSensor.h"

//
IMPLEMENT_RUNTIME_CLASS(CTempSensor)

//////////////////////////////////////////////////////////////////////////
CTempSensor::CTempSensor(void)
{
}

CTempSensor::~CTempSensor(void)
{
}
SPRESULT CTempSensor::__PollAction(void)
{
	CHKRESULT(RunTemp());
	return SP_OK;
}
BOOL CTempSensor::LoadXMLConfig(void)
{

	nUpSpec = GetConfigValue(L"Option:SpecUp", 0);
	nDownSpec = GetConfigValue(L"Option:SpecDown", 0);

	return TRUE;
}

SPRESULT CTempSensor::RunTemp()
{
	int nTempValue = 0;
	CHKRESULT(PrintErrorMsg(SP_BBAT_TemperatureTest(m_hDUT, &nTempValue), "Temperatrue Sensor", LEVEL_ITEM));
	_UiSendMsg("Temperatrue Sensor", LEVEL_ITEM,
		nDownSpec, nTempValue, nUpSpec,
		nullptr, -1, nullptr,
		"Temp Value = %d", nTempValue);
	if (!IN_RANGE(nDownSpec, nTempValue, nUpSpec))
	{
		return SP_E_BBAT_SENSOR_VALUE_FAIL;
	}
	return SP_OK;
}



